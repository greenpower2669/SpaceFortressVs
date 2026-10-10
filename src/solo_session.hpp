#pragma once
#include "solo_map_generator.hpp"
#include <algorithm>
#include <cmath>

namespace sfsolo {
enum class Phase { Ready, Flying, BossFight, Won, Lost };
struct Session {
    Map map;
    Pilot pilot;
    Physics physics;
    Phase phase=Phase::Ready;
    float seconds=0, cameraY=0;
    unsigned enemiesDefeated=0;
    unsigned enemiesAvailable=0;
    bool reachedFinish=false;
    bool bossDefeated=false;
    float shipRadius=.30f;
    float collisionCooldown=0;
    static constexpr float ICE_MALUS_SECONDS=2.50f;
    float iceSeconds=0;
    bool iceActive() const {return iceSeconds>0.0f;}
    explicit Session(Map source):map(std::move(source)) {
        for(const auto tile:map.tiles)if(tile==Tile::Enemy)++enemiesAvailable;
        for(int y=0;y<map.height;y++)
            for(int x=0;x<map.width;x++)
                if(map.at(x,y)==Tile::Start){
                    pilot.x=x+.5f;pilot.y=y+.5f;
                    cameraY=pilot.y;
                    return;
                }
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
    void step(float inputX,float inputY,float dt){
        if(phase!=Phase::Flying&&phase!=Phase::BossFight)return;
        dt=std::clamp(dt,0.0f,.05f);
        seconds+=dt;
        collisionCooldown=std::max(0.0f,collisionCooldown-dt);
        iceSeconds=std::max(0.0f,iceSeconds-dt);
        const float oldX=pilot.x,oldY=pilot.y;
        physics.step(pilot,inputX,inputY,dt,iceActive()?.25f:1.0f);
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
                    collisionCooldown=.45f;
                }
            }else if(hit==Tile::Boss && !bossDefeated){
                phase=Phase::BossFight;
            }else if(hit==Tile::Finish){
                reachedFinish=true;
                if(bossDefeated)phase=Phase::Won;
            }
            if(phase==Phase::Lost || phase==Phase::Won)break;
            safeX=pilot.x;safeY=pilot.y;
        }
        // Follow real ship position every frame. x4 motion causes x4 scroll,
        // while the predictive offset is bounded to keep the pilot visible.
        const float lookAhead=std::clamp(pilot.vy*.25f,-2.0f,2.0f);
        cameraY=std::clamp(pilot.y+lookAhead,0.0f,float(map.height));
    }
};
struct MiniMapPoint { float x=0,y=0; };
inline MiniMapPoint miniMapPosition(const Session &s){
    if(s.map.width<=0||s.map.height<=0)return {};
    return {std::clamp(s.pilot.x/float(s.map.width),0.0f,1.0f),
            std::clamp(s.pilot.y/float(s.map.height),0.0f,1.0f)};
}
} // namespace sfsolo
