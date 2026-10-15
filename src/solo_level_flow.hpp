#pragma once
#include "coop_solo_world_bridge.hpp"

namespace sfsolo {
enum class LevelPhase { Exploration, Transition, FinalArena, Victory };
// Run metadata only. The physical ship remains owned by the COOP engine.
struct LevelFlow {
    LevelPhase phase=LevelPhase::Exploration;
    CoopWorldPoint lair{};
    std::vector<CoopWorldPoint> minibossAnchors;
    std::vector<CoopWorldPoint> relays;
    std::vector<bool> relayReached;
    float entranceRadius=0,transitionSeconds=0,seconds=0,damageTaken=0;
    bool ready=false;
    bool create(const CoopSoloWorldBridge &world,float hull){
        *this=LevelFlow{};
        if(!world.ready || !world.canOccupy(world.start(),hull))return false;
        const int w=world.map.width,h=world.map.height;
        const auto centre=[&](int i){return CoopWorldPoint{(i%w+.5f)*world.pixelsPerCell,(i/w+.5f)*world.pixelsPerCell};};
        const auto start=world.start();
        const int first=int(start.y/world.pixelsPerCell)*w+int(start.x/world.pixelsPerCell);
        std::vector<int> parent(w*h,-1);parent[first]=first;
        std::queue<int> pending;pending.push(first);int goal=-1;
        while(!pending.empty()){
            const int i=pending.front();pending.pop();
            if(world.map.at(i%w,i/w)==Tile::Boss){goal=i;break;}
            constexpr int dx[]={1,-1,0,0},dy[]={0,0,1,-1};
            for(int d=0;d<4;++d){
                const int x=i%w+dx[d],y=i/w+dy[d];
                if(x<0||x>=w||y<0||y>=h)continue;
                const int n=y*w+x;
                if(parent[n]>=0 || !world.canOccupy(centre(n),hull) ||
                   world.firstWallImpact(centre(i),centre(n),hull)<1)continue;
                parent[n]=i;pending.push(n);
            }
        }
        if(goal<0)return false;
        std::vector<CoopWorldPoint> path;
        for(int at=goal;;at=parent[at]){path.push_back(centre(at));if(at==first)break;}
        std::reverse(path.begin(),path.end());
        if(path.size()<10)return false;
        lair=path.back();entranceRadius=world.pixelsPerCell*.85f;
        // Four spaced encounters lie on the navigable route, never in walls.
        for(unsigned n=1;n<=4;++n){
            const auto at=path[(path.size()-1)*n/5];
            if(world.map.at(int(at.x/world.pixelsPerCell),int(at.y/world.pixelsPerCell))==Tile::Lava ||
               world.map.at(int(at.x/world.pixelsPerCell),int(at.y/world.pixelsPerCell))==Tile::Beam){
                // A small neighbouring search keeps the arena out of a hazard.
                bool found=false;
                const int index=int((path.size()-1)*n/5);
                for(int offset=1;offset<10&&!found;++offset)for(int sign:{-1,1}){
                    const int k=index+offset*sign;if(k<=0||k>=int(path.size())-1)continue;
                    const auto p=path[k];const auto tile=world.map.at(int(p.x/world.pixelsPerCell),int(p.y/world.pixelsPerCell));
                    if(tile!=Tile::Lava && tile!=Tile::Beam){minibossAnchors.push_back(p);found=true;break;}
                }
                if(!found)return false;
            }else minibossAnchors.push_back(at);
        }
        for(int i=0;i<w*h;++i)if(world.map.at(i%w,i/w)==Tile::Trigger)relays.push_back(centre(i));
        relayReached.resize(relays.size(),false);
        return ready=true;
    }
    void observeShip(CoopWorldPoint ship){
        if(!ready || phase!=LevelPhase::Exploration)return;
        for(unsigned i=0;i<relays.size();++i)
            if(std::hypot(ship.x-relays[i].x,ship.y-relays[i].y)<=entranceRadius)relayReached[i]=true;
    }
    bool enterLair(CoopWorldPoint ship){
        if(!ready || phase!=LevelPhase::Exploration ||
           std::hypot(ship.x-lair.x,ship.y-lair.y)>entranceRadius)return false;
        phase=LevelPhase::Transition;transitionSeconds=0;return true;
    }
    bool advanceTransition(float dt,bool active){
        if(!active || phase!=LevelPhase::Transition || !std::isfinite(dt)||dt<=0)return false;
        transitionSeconds=std::min(1.0f,transitionSeconds+dt);
        if(transitionSeconds<1.0f-1e-6f)return false;
        phase=LevelPhase::FinalArena;return true;
    }
    float gameplayTime(float dt,bool active){
        if(!active || !std::isfinite(dt) || dt<=0)return 0;
        if(phase!=LevelPhase::Transition)return dt;
        const float remaining=std::max(0.0f,1.0f-transitionSeconds);
        advanceTransition(dt,true);
        return std::max(0.0f,dt-remaining);
    }
};
}
