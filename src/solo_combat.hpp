#pragma once
// First isolated SOLO shooting prototype: aimed projectile hits, boss HP and
// enemy removal. The classic DUEL/COOP weapon system is not modified.
#include "solo_session.hpp"
#include <algorithm>
#include <cmath>
#include <vector>

namespace sfsolo {
struct Shot {
    float x=0,y=0,vx=0,vy=0,life=0;
    bool missile=false;
    float damage=10,age=0,aimX=0,aimY=0;
};
struct ImpactFx {float x=0,y=0,life=.36f;bool frozen=false;};
struct Combat {
    std::vector<Shot> shots;
    std::vector<ImpactFx> impacts;
    unsigned pendingImpacts=0;
    float fireCooldown=0;
    float bossHealth=100;
    float bossMaxHealth=100;
    bool fire(Session &s,float aimX,float aimY){
        if(s.phase!=Phase::Flying && s.phase!=Phase::BossFight)return false;
        if(fireCooldown>0||shots.size()>=64)return false;
        const float dx=aimX-s.pilot.x,dy=aimY-s.pilot.y;
        const float len=std::sqrt(dx*dx+dy*dy);
        if(len<.001f)return false;
        float speed=22.0f,damage=10.0f;
        bool missile=false;
        const float heatBefore=sfShipHeat(s.heatSpent);
        if(s.canonicalShipActive){
            speed=sfCanonicalMainShotSpeed(heatBefore,float(s.map.width));
            missile=s.spendShotEnergy();
            if(missile)speed=float(s.map.width)*.65f;
            damage=missile?60.0f:12.0f;
        }
        shots.push_back({s.pilot.x,s.pilot.y,dx/len*speed,dy/len*speed,
                         missile?4.0f:3.0f,missile,damage,0,aimX,aimY});
        fireCooldown=s.canonicalShipActive?.20f+.010f*heatBefore:.18f;
        return true;
    }
    void step(Session &s,float dt){
        if(s.phase!=Phase::Flying && s.phase!=Phase::BossFight)return;
        dt=std::clamp(dt,0.0f,.05f);
        fireCooldown=std::max(0.0f,fireCooldown-dt);
        for(auto &burst:impacts)burst.life-=dt;
        impacts.erase(std::remove_if(impacts.begin(),impacts.end(),
            [](const ImpactFx &fx){return fx.life<=0;}),impacts.end());
        for(auto &shot:shots){
            if(shot.life<=0)continue;
            if(shot.missile){
                // Canonical missile acceleration; SOLO adds 360-degree
                // guidance toward its chosen aim coordinate, not a false orb.
                const float speed=float(s.map.width)*
                    std::min(1.5f,.65f+1.8f*(shot.age+dt*.5f));
                const float dx=shot.aimX-shot.x,dy=shot.aimY-shot.y;
                const float length=std::hypot(dx,dy);
                if(length>.001f){
                    const float maxDelta=float(s.map.width)*1.4f*dt;
                    shot.vx+=std::clamp(dx/length*speed-shot.vx,-maxDelta,maxDelta);
                    shot.vy+=std::clamp(dy/length*speed-shot.vy,-maxDelta,maxDelta);
                    const float magnitude=std::hypot(shot.vx,shot.vy);
                    if(magnitude>speed){shot.vx*=speed/magnitude;shot.vy*=speed/magnitude;}
                }
                shot.age+=dt;
            }
            const float distance=std::sqrt(shot.vx*shot.vx+shot.vy*shot.vy)*dt;
            const int substeps=std::max(1,int(std::ceil(distance/.25f)));
            const float stepTime=dt/substeps;
            for(int i=0;i<substeps && shot.life>0;i++){
                shot.x+=shot.vx*stepTime;shot.y+=shot.vy*stepTime;
                if(shot.x<0||shot.y<0||shot.x>=s.map.width||shot.y>=s.map.height){
                    shot.life=0;break;
                }
                const int x=int(shot.x),y=int(shot.y);
                const Tile tile=s.map.at(x,y);
                if(tile==Tile::Enemy){
                    s.map.tiles[size_t(y)*s.map.width+x]=Tile::Empty;
                    if(s.enemiesDefeated<s.enemiesAvailable)s.defeatEnemy();
                    shot.life=0;
                }else if(tile==Tile::Boss){
                    if(!s.bossDefeated){
                        bossHealth=std::max(0.0f,bossHealth-shot.damage);
                        if(bossHealth<=0){
                            s.phase=Phase::BossFight;
                            s.defeatBoss();
                            // Clear the defeated boss tile: it must no longer
                            // block the route to the finish.
                            s.map.tiles[size_t(y)*size_t(s.map.width)+size_t(x)]=Tile::Empty;
                        }
                    }
                    shot.life=0;
                }else if(tile==Tile::IceAsteroid){
                    // Aimed fire fractures ice into drifting white dust.
                    s.map.tiles[size_t(y)*s.map.width+x]=Tile::Empty;
                    s.scatterIce(x+.5f,y+.5f);
                    shot.life=0;
                }else if(tile==Tile::Rock||tile==Tile::Asteroid){
                    shot.life=0;
                }
                if(shot.life<=0 && tile!=Tile::Empty){
                    ++pendingImpacts;
                    if(impacts.size()<80)
                        impacts.push_back({shot.x,shot.y,.36f,
                                           tile==Tile::IceAsteroid});
                }
            }
            shot.life-=dt;
        }
        shots.erase(std::remove_if(shots.begin(),shots.end(),
                                  [](const Shot &shot){return shot.life<=0;}),
                    shots.end());
    }
};
} // namespace sfsolo
