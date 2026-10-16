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
        // Nine varied encounter areas plus 1..3 nearby pack companions.
        // Prefer real intersections/safe rooms near irregular route targets;
        // side cells are eligible, so guards do not form a metronomic row.
        const auto candidates=world.reachablePickupCells(start,hull);
        Rng placement(0x1700u+world.map.world*137u+world.map.stage*31u);
        const auto placeNear=[&](CoopWorldPoint desired,float spread)->bool{
            float best=std::numeric_limits<float>::max();CoopWorldPoint chosen{};bool found=false;
            for(const auto at:candidates){
                if(std::hypot(at.x-start.x,at.y-start.y)<world.combatWidth*.6f ||
                   std::hypot(at.x-lair.x,at.y-lair.y)<world.combatWidth*.5f)continue;
                bool overlap=false;
                for(const auto other:minibossAnchors)if(std::hypot(at.x-other.x,at.y-other.y)<world.combatWidth*.21f){overlap=true;break;}
                if(overlap)continue;
                const float distance=std::hypot(at.x-desired.x,at.y-desired.y);
                if(spread>0 && distance>spread)continue;
                const int x=int(at.x/world.pixelsPerCell),y=int(at.y/world.pixelsPerCell);
                int degree=0;for(const auto offset:std::array<std::array<int,2>,4>{{{1,0},{-1,0},{0,1},{0,-1}}})
                    degree+=world.map.at(x+offset[0],y+offset[1])!=Tile::Rock;
                const float score=distance-degree*world.pixelsPerCell*.22f;
                if(score<best){best=score;chosen=at;found=true;}
            }
            if(found)minibossAnchors.push_back(chosen);return found;
        };
        for(unsigned n=0;n<9;++n){
            const float fraction=(n+.55f+(placement.next()%70)/100.f)/10.f;
            const auto desired=path[std::min(path.size()-2,size_t(fraction*(path.size()-1)))];
            if(!placeNear(desired,0))return false;
        }
        const auto packAnchor=minibossAnchors[4];
        for(unsigned n=0;n<1+world.map.stage%3;++n){
            const float angle=n*2.1f;
            if(!placeNear({packAnchor.x+std::cos(angle)*world.combatWidth*.32f,
                           packAnchor.y+std::sin(angle)*world.combatWidth*.32f},world.combatWidth*1.3f))
                if(!placeNear(packAnchor,0))return false;
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
