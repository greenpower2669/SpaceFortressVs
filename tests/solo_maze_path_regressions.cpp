#include "solo_maze_path.hpp"
#include <cassert>
#include <cstdio>
#include "solo_ship_motion.hpp"
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
 // Identical held screen gesture keeps world cruise speed across presentation zoom.
 w.map.width=1000;w.map.tiles.assign(w.map.width*w.map.height,Tile::Empty);
 for(int danger:{0,4,8}){
  float distance[2]{};
  for(int z=1;z<=2;++z){
   w.zoom=float(z);ShipMotion s{5000,5000,0,0};w.centerOn({s.x,s.y});
   for(int i=0;i<1000;++i){
    const auto target=w.heldFingerPilotTarget({w.screen.w*.95f,w.screen.h*.5f});
    const float old=s.x;s.step(target.x,target.y,true,w.combatWidth,danger,.01f);
    w.followPhysicalShip({s.x,s.y},{s.x-old,0},.01f);
    const auto pixel=w.toScreen({s.x,s.y});assert(pixel.x>0 && pixel.x<w.screen.w);
   }
   distance[z-1]=s.x-5000;
  }
  printf("Cruise Danger %d: zoom1 %.2f zoom2 %.2f\n",danger,distance[0],distance[1]);
  assert(std::abs(distance[0]-distance[1])<distance[0]*.04f);
 }
 puts("PASS: real maze routing, occlusion, zoom mapping, drift visibility and long swept wall collision");
}
