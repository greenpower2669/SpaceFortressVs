#pragma once
// SOLO campaign map model. No live duel/COOP state is mutated by this module.
// Logical maps are immutable after publication; terrain is authored as RGB pixels.
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

namespace sfsolo {
enum class Tile : uint8_t { Empty, Rock, Lava, Asteroid, Beam, Enemy, Boss, Start, Finish, Trigger };
struct Map {
    std::string id;
    unsigned version=1, world=1, stage=1;
    int width=0,height=0;
    std::vector<Tile> tiles;
    bool valid() const {
        if(width<3 || height<3 || width>4096 || height>16384 ||
           tiles.size()!=size_t(width)*size_t(height)) return false;
        int starts=0,finishes=0;
        for(auto t:tiles){ starts+=(t==Tile::Start); finishes+=(t==Tile::Finish); }
        return starts==1 && finishes==1 && !id.empty();
    }
    Tile at(int x,int y) const {
        if(x<0||y<0||x>=width||y>=height) return Tile::Rock;
        return tiles[size_t(y)*size_t(width)+size_t(x)];
    }
};
inline Tile fromRgb(uint8_t r,uint8_t g,uint8_t b) {
    const uint32_t c=(uint32_t(r)<<16)|(uint32_t(g)<<8)|b;
    switch(c){
      case 0x000000: return Tile::Empty;
      case 0x808080: return Tile::Rock;
      case 0xff8000: return Tile::Lava;
      case 0xffffff: return Tile::Asteroid;
      case 0x8000ff: return Tile::Beam;
      case 0xff0000: return Tile::Enemy;
      case 0xff00ff: return Tile::Boss;
      case 0x0000ff: return Tile::Start;
      case 0x00ff00: return Tile::Finish;
      case 0xffff00: return Tile::Trigger;
      default: return Tile::Rock; // Fail closed: unknown colors never create shortcuts.
    }
}
struct Pilot {
    float x=0,y=0,vx=0,vy=0;
    float damageTaken=0,health=100;
};
struct Physics {
    float thrust=5.0f, damping=0.35f, maxSpeed=7.0f;
    void step(Pilot &p,float inputX,float inputY,float dt) const {
        dt=std::clamp(dt,0.0f,0.05f);
        const float magnitude=std::hypot(inputX,inputY);
        if(magnitude>1.0f){inputX/=magnitude;inputY/=magnitude;}
        p.vx+=inputX*thrust*dt;
        p.vy+=inputY*thrust*dt;
        const float factor=std::exp(-std::max(0.0f,damping)*dt);
        p.vx*=factor;p.vy*=factor;
        const float speed=std::hypot(p.vx,p.vy);
        if(speed>maxSpeed && speed>0){p.vx*=maxSpeed/speed;p.vy*=maxSpeed/speed;}
        p.x+=p.vx*dt;p.y+=p.vy*dt;
    }
};
struct ScoreInput {
    unsigned enemiesDefeated=0,enemiesAvailable=0;
    float coopCombatPoints=0, bossPoints=0, elapsedSeconds=0, damageTaken=0;
    float referenceSeconds=120, referenceDamage=100;
    float difficultyMultiplier=1;
    bool reachedFinish=false,alive=false,bossDefeated=false;
};
// COOP canonical reference: (encounter+1)*500 + remaining allied PV*2 - time*5.\n// SOLO retains this base with one pilot and adds an enemy-count component.\ninline int coopStyleBase(unsigned encounter,float remainingPv,float elapsedSeconds) {\n    return std::max(0,int((encounter+1)*500.0f+std::max(0.0f,remainingPv)*2.0f-std::max(0.0f,elapsedSeconds)*5.0f));\n}\ninline int score(const ScoreInput &s) {
    if(!s.reachedFinish||!s.alive||!s.bossDefeated) return 0;
    const float participation=s.enemiesAvailable ?
        std::clamp(float(s.enemiesDefeated)/float(s.enemiesAvailable),0.0f,1.0f):1.0f;
    const float speed=std::clamp(s.referenceSeconds/std::max(1.0f,s.elapsedSeconds),0.0f,1.0f);
    const float care=std::clamp(1.0f-s.damageTaken/std::max(1.0f,s.referenceDamage),0.0f,1.0f);
    const float combat=std::max(0.0f,s.coopCombatPoints)+std::max(0.0f,s.bossPoints);
    const float result=(combat*participation+60000.0f*speed+40000.0f*care)*
                       std::max(0.0f,s.difficultyMultiplier);
    return int(std::clamp(result,0.0f,100000000.0f));
}
inline bool worldUnlocked(unsigned world,const std::vector<bool> &completedWorlds) {
    return world==1 || (world>1 && world-2<completedWorlds.size() && completedWorlds[world-2]);
}
} // namespace sfsolo
