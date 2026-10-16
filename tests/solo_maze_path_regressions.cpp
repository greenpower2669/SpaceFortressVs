#include "solo_maze_path.hpp"
#include <cassert>
#include <cstdio>
int main(){
 using namespace sfsolo;CoopSoloWorldBridge w;MapOptions o;o.width=54;o.height=160;assert(w.create(o,{0,0,780,1680},780));
 std::fill(w.map.tiles.begin(),w.map.tiles.end(),Tile::Empty);
 for(int y=0;y<15;++y)w.map.tiles[y*w.map.width+7]=Tile::Rock;
 const float c=w.pixelsPerCell;CoopWorldPoint start{5.5f*c,5.5f*c},goal{9.5f*c,5.5f*c};
 assert(w.firstWallImpact(start,goal,10)<1);
 const auto route=mazeRoute(w,start,goal,10);assert(route.size()>20);
 auto p=start;bool rounded=false;for(auto q:route){assert(w.canOccupy(q,10));assert(w.firstWallImpact(p,q,10)>=1);p=q;rounded|=q.y>15*c;}
 assert(rounded && std::hypot(p.x-goal.x,p.y-goal.y)<c);
 w.centerOn(start);assert(!w.directView(start,goal));assert(w.directView(start,{start.x,start.y+c}));
 const auto screen=w.toScreen(start);w.setZoom(2,start);const auto p2=w.toScreen(start);assert(std::hypot(screen.x-p2.x,screen.y-p2.y)<.01f);
 const auto back=w.toWorld(w.toScreen(goal));assert(std::hypot(back.x-goal.x,back.y-goal.y)<.01f);
 w.followPhysicalShip(goal,{1000,0},1.f/60);const auto on=w.toScreen(goal);assert(on.x>0 && on.x<w.screen.w);
 const auto blocked=w.moveWithoutTunnelling(start,{start.x+c*1000,start.y},10);assert(blocked.x<7*c && w.canOccupy(blocked,10));
 puts("PASS: real maze routing, occlusion, zoom mapping, drift visibility and long swept wall collision");
}
