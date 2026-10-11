#pragma once
#include "solo_map_generator.hpp"
#include <algorithm>
#include <cmath>

namespace sfsolo {
enum class Phase { Ready, Flying, BossFight, Won, Lost };
struct SoloDust {
    float x=0,y=0,vx=0,vy=0,life=0;
    bool icy=false;
};
struct Session {
    Map map;
    // The map gradually reveals previously explored passages, including
    // backtracked routes on the minimap. Exploration does not expire.
    std::vector<uint8_t> explored;
    std::vector<SoloDust> dust;
    Pilot pilot;
    Physics physics;
    Phase phase=Phase::Ready;
    float seconds=0, cameraY=0;
    // SOLO camera hysteresis. Owned by the level, not recalculated from the
    // current finger target, so a camera pan cannot feed back into steering.
    float viewLeft=0,viewTop=0;
    // Two progressive rectangular camera zones have independent X/Y inertia.
    // Camera velocity is presentation-only; it never feeds ship physics.
    float viewVelocityX=0,viewVelocityY=0;
    bool viewReady=false;
    int viewWidth=0,viewHeight=0;
    unsigned enemiesDefeated=0;
    unsigned enemiesAvailable=0;
    bool reachedFinish=false;
    bool bossDefeated=false;
    float shipRadius=.30f;
    float collisionCooldown=0;
    static constexpr float ICE_MALUS_SECONDS=2.50f;
    float iceSeconds=0, lavaSeconds=0;
    unsigned relaysCollected=0,relaysNeeded=0;
    bool iceActive() const {return iceSeconds>0.0f;}
    bool lavaActive() const {return lavaSeconds>0.0f;}
    bool slowActive() const {return iceActive()||lavaActive();}
    float mobility() const {return slowActive()? .25f:1.0f;}
    bool missionReady() const {
        return bossDefeated && relaysCollected>=relaysNeeded;
    }
    void reveal(){
        if(!map.valid())return;
        if(explored.size()!=map.tiles.size())explored.assign(map.tiles.size(),0);
        const int cx=int(pilot.x),cy=int(pilot.y);
        for(int y=std::max(0,cy-8);y<std::min(map.height,cy+9);++y)
            for(int x=std::max(0,cx-8);x<std::min(map.width,cx+9);++x)
                if((x-cx)*(x-cx)+(y-cy)*(y-cy)<=64)
                    explored[size_t(y)*map.width+x]=1;
    }
    explicit Session(Map source):map(std::move(source)) {
        for(const auto tile:map.tiles){
            if(tile==Tile::Enemy)++enemiesAvailable;
            if(tile==Tile::Trigger)++relaysNeeded;
        }
        for(int y=0;y<map.height;y++)
            for(int x=0;x<map.width;x++)
                if(map.at(x,y)==Tile::Start){
                    pilot.x=x+.5f;pilot.y=y+.5f;
                    cameraY=pilot.y;
                    reveal();
                    return;
                }
    }
    // White ice fragments originate from icy asteroids hit by the kinetic
    // cone or direct projectile. They float briefly and can slow the ship.
    void scatterIce(float px,float py){
        if(dust.size()>128)return;
        constexpr float directions[8][2]={
            {1,0},{.70f,.70f},{0,1},{-.70f,.70f},
            {-1,0},{-.70f,-.70f},{0,-1},{.70f,-.70f}
        };
        for(const auto &d:directions)
            dust.push_back({px,py,d[0]*1.8f,d[1]*1.8f,2.50f,true});
    }
    void advanceDust(float dt){
        for(auto &p:dust){
            if(p.life<=0)continue;
            p.life-=dt;
            // Ice debris drifts towards the moving ship like mining dust.
            const float dx=pilot.x-p.x,dy=pilot.y-p.y;
            const float dist=std::hypot(dx,dy);
            if(dist>.01f){
                p.vx+=dx/dist*3.0f*dt;
                p.vy+=dy/dist*3.0f*dt;
            }
            p.x+=p.vx*dt;p.y+=p.vy*dt;
            if(p.icy && std::hypot(p.x-pilot.x,p.y-pilot.y)
                        <=shipRadius+.24f){
                iceSeconds=std::max(iceSeconds,ICE_MALUS_SECONDS);
                p.life=0;
            }
        }
        dust.erase(std::remove_if(dust.begin(),dust.end(),
            [](const SoloDust &p){return p.life<=0;}),dust.end());
    }
    bool start(){
        if(!map.valid()||phase!=Phase::Ready)return false;
        phase=Phase::Flying;return true;
    }
    void damage(float amount){
        if(amount<=0||phase==Phase::Lost||phase==Phase::Won)return;
        pilot.damageTaken+=amount; // NEVER subtract repairs from cumulative damage.
        pilot.health=std::max(0.0f,pilot.health-amount);
        if(pilot.health<=0)phase=Phase::Lost;
    }
    void repair(float amount){
        if(amount>0 && phase!=Phase::Lost)
            pilot.health=std::min(100.0f,pilot.health+amount);
    }
    void defeatBoss(){
        if(phase==Phase::BossFight){bossDefeated=true;phase=Phase::Flying;}
    }
    void defeatEnemy(){++enemiesDefeated;}
    // An optional canonical touch target changes only *how* the same ship
    // receives user commands. Keyboard/controller tests keep vector thrust.
    void step(float inputX,float inputY,float dt,
              bool pointerTarget=false,float fingerWorldX=0,float fingerWorldY=0,
              float touchSpeedColumns=0,
              bool continuousCruise=false,float cruiseThrottle=0){
        if(phase!=Phase::Flying&&phase!=Phase::BossFight)return;
        dt=std::clamp(dt,0.0f,.05f);
        seconds+=dt;
        collisionCooldown=std::max(0.0f,collisionCooldown-dt);
        iceSeconds=std::max(0.0f,iceSeconds-dt);
        lavaSeconds=std::max(0.0f,lavaSeconds-dt);
        const float oldX=pilot.x,oldY=pilot.y;
        if(continuousCruise)
            physics.stepCruise(pilot,inputX,inputY,cruiseThrottle,
                touchSpeedColumns>0?touchSpeedColumns:float(map.width),
                dt,mobility());
        else if(pointerTarget)
            physics.stepToward(pilot,fingerWorldX,fingerWorldY,dt,mobility(),
                touchSpeedColumns>0?touchSpeedColumns:float(map.width));
        else physics.step(pilot,inputX,inputY,dt,mobility());
        const float targetX=std::clamp(pilot.x,shipRadius,float(map.width)-shipRadius);
        const float targetY=std::clamp(pilot.y,shipRadius,float(map.height)-shipRadius);
        // x4 movement can cross multiple cells per frame. Sweep through the
        // entire flight path so rocks and icy/ordinary asteroids cannot vanish
        // between endpoint collision checks.
        const float dx=targetX-oldX,dy=targetY-oldY;
        const int samples=std::max(1,int(std::ceil(std::hypot(dx,dy)/.20f)));
        float safeX=oldX,safeY=oldY;
        for(int i=1;i<=samples;++i){
            const float part=float(i)/float(samples);
            pilot.x=std::clamp(oldX+dx*part,shipRadius,float(map.width)-shipRadius);
            pilot.y=std::clamp(oldY+dy*part,shipRadius,float(map.height)-shipRadius);
            const Tile hit=map.at(int(pilot.x),int(pilot.y));
            if(hit==Tile::Rock){
                pilot.x=safeX;pilot.y=safeY;pilot.vx=pilot.vy=0;
                if(collisionCooldown<=0){damage(8);collisionCooldown=.45f;}
                break;
            }
            if(hit==Tile::Asteroid||hit==Tile::IceAsteroid||
               hit==Tile::Lava||hit==Tile::Beam){
                if(collisionCooldown<=0){
                    damage(hit==Tile::Lava?12.0f:
                           hit==Tile::Beam?9.0f:5.0f);
                    if(hit==Tile::IceAsteroid)
                        iceSeconds=ICE_MALUS_SECONDS;
                    if(hit==Tile::Lava)
                        lavaSeconds=ICE_MALUS_SECONDS;
                    collisionCooldown=.45f;
                }
            }else if(hit==Tile::Trigger){
                const size_t location=size_t(int(pilot.y))*map.width+
                                      size_t(int(pilot.x));
                map.tiles[location]=Tile::Empty;
                if(relaysCollected<relaysNeeded)++relaysCollected;
            }else if(hit==Tile::Boss && !bossDefeated){
                phase=Phase::BossFight;
            }else if(hit==Tile::Finish){
                reachedFinish=true;
                if(missionReady())phase=Phase::Won;
            }
            if(phase==Phase::Lost || phase==Phase::Won)break;
            safeX=pilot.x;safeY=pilot.y;
        }
        // Follow real ship position every frame. x4 motion causes x4 scroll,
        // while the predictive offset is bounded to keep the pilot visible.
        const float lookAhead=std::clamp(pilot.vy*.25f,-2.0f,2.0f);
        cameraY=std::clamp(pilot.y+lookAhead,0.0f,float(map.height));
        advanceDust(dt);
        reveal();
    }
    void stepCruise(float dirX,float dirY,float throttle,
                    float visibleWorldColumns,float dt){
        step(dirX,dirY,dt,false,0,0,visibleWorldColumns,true,throttle);
    }
    void stepToward(float worldX,float worldY,float dt,
                    float visibleWorldColumns=0) {
        // Same velocity on screen as the historical touch control. Zooming
        // the scenery x3 must not triple ship speed in screen pixels.
        if(visibleWorldColumns<=0){step(0,0,dt,true,worldX,worldY);return;}
        if(phase!=Phase::Flying&&phase!=Phase::BossFight)return;
        // Reuse collision/ice logic in step(), while passing the display-based
        // width into Physics::stepToward via an explicit optional parameter.
        step(0,0,dt,true,worldX,worldY,visibleWorldColumns);
    }
};
struct MiniMapPoint { float x=0,y=0; };
inline MiniMapPoint miniMapPosition(const Session &s){
    if(s.map.width<=0||s.map.height<=0)return {};
    return {std::clamp(s.pilot.x/float(s.map.width),0.0f,1.0f),
            std::clamp(s.pilot.y/float(s.map.height),0.0f,1.0f)};
}
} // namespace sfsolo
