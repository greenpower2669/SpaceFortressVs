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
        const float oldX=pilot.x,oldY=pilot.y;
        physics.step(pilot,inputX,inputY,dt);
        pilot.x=std::clamp(pilot.x,shipRadius,float(map.width)-shipRadius);
        pilot.y=std::clamp(pilot.y,shipRadius,float(map.height)-shipRadius);
        const Tile hit=map.at(int(pilot.x),int(pilot.y));
        if(hit==Tile::Rock){
            pilot.x=oldX;pilot.y=oldY;pilot.vx=pilot.vy=0;
            if(collisionCooldown==0){damage(8);collisionCooldown=.45f;}
        }else if(hit==Tile::Lava||hit==Tile::Beam){
            if(collisionCooldown==0){damage(hit==Tile::Lava?12:9);collisionCooldown=.45f;}
        }else if(hit==Tile::Asteroid){
            if(collisionCooldown==0){damage(5);collisionCooldown=.45f;}
        }else if(hit==Tile::Boss && !bossDefeated){
            phase=Phase::BossFight;
        }else if(hit==Tile::Finish){
            reachedFinish=true;
            if(bossDefeated)phase=Phase::Won;
        }
        // Camera follows with a slight look-ahead in direction of travel.
        const float lookAhead=std::clamp(pilot.vy*.6f,-3.0f,3.0f);
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
