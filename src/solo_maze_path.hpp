#pragma once
#include "coop_solo_world_bridge.hpp"
namespace sfsolo {
// Bounded BFS over real hull clearance. Returns a route to the nearest
// reachable point to the LAST observation, never an omniscient player query.
inline std::vector<CoopWorldPoint> mazeRoute(const CoopSoloWorldBridge &world,
                                            CoopWorldPoint from,CoopWorldPoint target,float radius,int variant=0){
    std::vector<CoopWorldPoint> result;
    if(!world.canOccupy(from,radius))return result;
    if(world.firstWallImpact(from,target,radius)>=1 && world.canOccupy(target,radius)){result.push_back(target);return result;}
    const int w=world.map.width,h=world.map.height;
    const auto centre=[&](int i){return CoopWorldPoint{(i%w+.5f)*world.pixelsPerCell,(i/w+.5f)*world.pixelsPerCell};};
    const int sx=int(from.x/world.pixelsPerCell),sy=int(from.y/world.pixelsPerCell);
    if(sx<0||sy<0||sx>=w||sy>=h)return result;
    int start=sy*w+sx;
    if(world.firstWallImpact(from,centre(start),radius)<1)return result;
    std::vector<int> parent(w*h,-1);std::queue<int> pending;
    parent[start]=start;pending.push(start);int goal=start;float best=std::numeric_limits<float>::max();
    constexpr int dx[]={1,0,-1,0},dy[]={0,1,0,-1};
    while(!pending.empty()){
        const int i=pending.front();pending.pop();const auto here=centre(i);
        const float distance=std::hypot(here.x-target.x,here.y-target.y);
        if(distance<best){goal=i;best=distance;}
        if(distance<world.pixelsPerCell*.5f)break;
        for(int n=0;n<4;++n){
            const int d=(n+variant)%4,x=i%w+dx[d],y=i/w+dy[d];
            if(x<0||y<0||x>=w||y>=h)continue;
            const int next=y*w+x;
            if(parent[next]>=0 || !world.canOccupy(centre(next),radius) ||
                world.firstWallImpact(here,centre(next),radius)<1)continue;
            parent[next]=i;pending.push(next);
        }
    }
    for(int at=goal;at!=start;at=parent[at])result.push_back(centre(at));
    result.push_back(centre(start));std::reverse(result.begin(),result.end());
    return result;
}
}
