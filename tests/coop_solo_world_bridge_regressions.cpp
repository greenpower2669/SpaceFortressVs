#include "../src/coop_solo_world_bridge.hpp"
#include <cassert>
#include <cmath>
using namespace sfsolo;

static bool same(float a,float b){return std::abs(a-b)<.002f;}
int main(){
    CoopSoloWorldBridge bridge;
    MapOptions opts;opts.width=54;opts.height=160;opts.difficulty=1;
    assert(bridge.create(opts,{0,0,540,960},540));
    assert(bridge.map.valid());
    assert(soloMazeConnected(bridge.map));
    assert(same(bridge.pixelsPerCell,90));
    assert(same(bridge.combatWidth,540));
    assert(same(bridge.worldWidth(),4860));
    assert(same(bridge.worldHeight(),14400));
    const auto start=bridge.start();
    assert(start.x>0&&start.y>0);
    bridge.centerOn(start);
    const auto originOnScreen=bridge.toScreen(start);
    const auto inverse=bridge.toWorld(originOnScreen);
    assert(same(inverse.x,start.x)&&same(inverse.y,start.y));
    assert(bridge.canOccupy(start,12));
    assert(bridge.canOccupy(start,40)); // actual historical ship half-width
    // A large step cannot jump through an opaque Rock tile.
    int rockX=-1,rockY=-1;
    for(int y=1;y<opts.height-1&&rockX<0;++y)
      for(int x=1;x<opts.width-1;++x)
        if(bridge.map.at(x,y)==Tile::Rock){rockX=x;rockY=y;break;}
    assert(rockX>=0);
    const CoopWorldPoint rock{(rockX+.5f)*bridge.pixelsPerCell,
                              (rockY+.5f)*bridge.pixelsPerCell};
    assert(!bridge.canOccupy(rock,12));
    const auto moved=bridge.moveWithoutTunnelling(start,rock,12);
    assert(bridge.canOccupy(moved,12));
    assert(std::hypot(moved.x-start.x,moved.y-start.y)<
           std::hypot(rock.x-start.x,rock.y-start.y));
    // No physical displacement means no camera pan, regardless of the zone.
    bridge.cameraLeft=start.x-540*.88f; // ship near rightmost concentric ring
    bridge.cameraTop=start.y-960*.5f;
    const float oldX=bridge.cameraLeft,oldY=bridge.cameraTop;
    bridge.panIfShipActuallyMoved(start,{0,0},.05f);
    assert(same(oldX,bridge.cameraLeft)&&same(oldY,bridge.cameraTop));
    // When a ship really moves toward outer circle, camera can follow,
    // bounded by the actual permitted physical displacement.
    bridge.panIfShipActuallyMoved({start.x+20,start.y},{20,0},.05f);
    assert(bridge.cameraLeft>oldX);  // actual radial pan followed live movement
    assert(bridge.cameraLeft<=oldX+20.001f);
    assert(same(bridge.combatWidth,540));
    return 0;
}
