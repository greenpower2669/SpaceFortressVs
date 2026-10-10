#pragma once
// First isolated SOLO shooting prototype: aimed projectile hits, boss HP and
// enemy removal. The classic DUEL/COOP weapon system is not modified.
#include "solo_session.hpp"
#include <algorithm>
#include <cmath>
#include <vector>

namespace sfsolo {
struct Shot {float x=0,y=0,vx=0,vy=0;float life=0;};
struct Combat {
    std::vector<Shot> shots;
    float fireCooldown=0;
    float bossHealth=100;
    float bossMaxHealth=100;
    bool fire(const Session &s,float aimX,float aimY){
        if(s.phase!=Phase::Flying && s.phase!=Phase::BossFight)return false;
        if(fireCooldown>0||shots.size()>=64)return false;
        const float dx=aimX-s.pilot.x,dy=aimY-s.pilot.y;
        const float len=std::sqrt(dx*dx+dy*dy);
        if(len<.001f)return false;
        shots.push_back({s.pilot.x,s.pilot.y,dx/len*22,dy/len*22,3.0f});
        fireCooldown=.18f;
        return true;
    }
    void step(Session &s,float dt){
        if(s.phase!=Phase::Flying && s.phase!=Phase::BossFight)return;
        dt=std::clamp(dt,0.0f,.05f);
        fireCooldown=std::max(0.0f,fireCooldown-dt);
        for(auto &shot:shots){
            if(shot.life<=0)continue;
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
                        bossHealth=std::max(0.0f,bossHealth-10.0f);
                        if(bossHealth<=0){
                            s.phase=Phase::BossFight;
                            s.defeatBoss();
                        }
                    }
                    shot.life=0;
                }else if(tile==Tile::Rock||tile==Tile::Asteroid){
                    shot.life=0;
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
