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
    // Stationary finger at centre uses EXACT world target (no auto-cruise).
    const CoopWorldPoint centre{270,480};
    const auto centerTarget=bridge.heldFingerPilotTarget(centre);
    const auto exactCenter=bridge.toWorld(centre);
    assert(same(centerTarget.x,exactCenter.x));
    assert(same(centerTarget.y,exactCenter.y));
    // Outer-zone stationary finger must request further WORLD travel;
    // otherwise sfCoopMovePlayers reaches it and stops after 1-2 frames.
    const CoopWorldPoint outer{270,90};
    const auto outside=bridge.heldFingerPilotTarget(outer);
    const auto outsideWorld=bridge.toWorld(outer);
    assert(same(outside.x,outsideWorld.x));
    assert(outside.y<outsideWorld.y-100);
    // Reverse held zone pushes in reverse direction without teleporting.
    const auto reverse=bridge.heldFingerPilotTarget({270,850});
    assert(reverse.y>bridge.toWorld({270,850}).y+100);
    // Finger-locked panning: a ship that is still behind the held fingertip
    // must not have all its progress consumed by the concentric camera.
    bridge.cameraLeft=start.x-540*.75f;
    bridge.cameraTop=start.y-960*.50f;
    const float frozen=bridge.cameraLeft;
    const CoopWorldPoint finger{540*.90f,960*.50f};
    bridge.panIfShipActuallyMoved(start,{10,0},.05f,finger);
    assert(same(bridge.cameraLeft,frozen));
    // Once it reaches the finger, the camera must use only the excess
    // movement: player stays under that SAME stationary fingertip.
    const CoopWorldPoint caught{start.x+100,start.y};
    bridge.panIfShipActuallyMoved(caught,{100,0},.05f,finger);
    assert(same(bridge.toScreen(caught).x,finger.x));
    const CoopWorldPoint next{caught.x+20,caught.y};
    bridge.panIfShipActuallyMoved(next,{20,0},.05f,finger);
    assert(same(bridge.toScreen(next).x,finger.x));
    const float blocked=bridge.cameraLeft;
    bridge.panIfShipActuallyMoved(next,{0,0},.05f,finger);
    assert(same(bridge.cameraLeft,blocked)); // rock never drags camera
    // World-to-screen projection must remain invertible even in the last
    // region of the very long map, without a gigantic Android viewport.
    bridge.cameraTop=bridge.worldHeight()-960;
    const CoopWorldPoint far{start.x,bridge.worldHeight()-130};
    const auto onScreen=bridge.toScreen(far);
    const auto roundTrip=bridge.toWorld(onScreen);
    assert(same(far.x,roundTrip.x)&&same(far.y,roundTrip.y));
    return 0;
}
