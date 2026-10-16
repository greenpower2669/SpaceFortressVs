#include "../src/coop_solo_world_bridge.hpp"
#include <cassert>
#include <cmath>
using namespace sfsolo;

static bool same(float a,float b){return std::abs(a-b)<.002f;}
static void testSweptWallsAndReachableRewards(){
    CoopSoloWorldBridge world;
    MapOptions options;options.width=54;options.height=160;
    assert(world.create(options,{0,0,540,960},540));
    std::fill(world.map.tiles.begin(),world.map.tiles.end(),Tile::Empty);
    const float cell=world.pixelsPerCell;
    for(int y=0;y<world.map.height;++y)world.map.tiles[std::size_t(y)*world.map.width+5]=Tile::Rock;
    const CoopWorldPoint left{3.5f*cell,5.5f*cell},right{7.5f*cell,5.5f*cell};
    const float impact=world.firstWallImpact(left,right,12);
    assert(same(impact,(5*cell-12-left.x)/(right.x-left.x)));
    assert(world.firstWallImpact(right,left,12)<1);
    assert(world.firstWallImpact(left,{left.x,left.y+cell},12)==1);
    assert(world.firstWallImpact({5.5f*cell,5.5f*cell},right,12)==0);
    assert(world.firstWallImpact(left,{world.worldWidth()+1000,left.y},12)<impact);
    // Rebound in the SAME frame, not stop-and-wait. Keep free tangent.
    const auto front=world.moveWithBounce(left,{6.5f*cell,left.y},
                                          {3000,0},12,.12f);
    assert(front.collided && front.normal.x<0 && front.velocity.x<0);
    assert(front.position.x<5*cell-12);
    const auto diagonal=world.moveWithBounce(left,{6.5f*cell,left.y+cell},
                                             {3000,400},12,.12f);
    assert(diagonal.collided && diagonal.velocity.x<0 && diagonal.velocity.y>0);
    assert(world.canOccupy(diagonal.position,12));
    // A parallel tangent does not get mistaken for an entry into the wall.
    assert(world.firstWallImpact({5*cell-12,4*cell},{5*cell-12,8*cell},12)==1);
    CoopWorldPoint reward;
    assert(world.reachablePickup(left,right,20,reward));
    assert(reward.x<5*cell && world.canOccupy(reward,20));
    // Open a passage; now the actual ship can reach the desired component.
    world.map.tiles[std::size_t(5)*world.map.width+5]=Tile::Empty;
    assert(world.reachablePickup(left,right,20,reward));
    assert(same(reward.x,right.x)&&same(reward.y,right.y));
    // A gap one cell wide is too narrow for a hull whose diameter exceeds it.
    assert(world.reachablePickup(left,right,cell*.51f,reward));
    assert(reward.x<5*cell && world.canOccupy(reward,cell*.51f));
    world.map.tiles[std::size_t(5)*world.map.width+7]=Tile::Lava;
    assert(world.reachablePickup(left,right,20,reward));
    assert(world.map.at(int(reward.x/cell),int(reward.y/cell))!=Tile::Lava);
    assert(!world.reachablePickup({5.5f*cell,4.5f*cell},right,20,reward));
    // One component traversal, then distinct placements even when the
    // nearest reward slots are occupied. No repeated BFS or queue starvation.
    const auto cells=world.reachablePickupCells(left,20);
    std::vector<CoopWorldPoint> occupied;
    for(int i=0;i<24;++i){
        assert(world.selectPickup(cells,right,occupied,30,reward));
        for(const auto p:occupied)assert(std::hypot(p.x-reward.x,p.y-reward.y)>=30);
        occupied.push_back(reward);
    }
}
int main(){
    testSweptWallsAndReachableRewards();
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
    assert(outside.y<outsideWorld.y-70);
    // SOLO inertia uses distance*12 for its desired velocity; the cruise
    // lead expresses the circular camera law before physical acceleration.
    for(const CoopWorldPoint held:{
        CoopWorldPoint{384,480}, // just outside central dead zone
        CoopWorldPoint{440,480}, // middle circle
        CoopWorldPoint{270,160}, // outer circle, outside HUD controls
        CoopWorldPoint{270,850}}){
        const auto goal=bridge.heldFingerPilotTarget(held);
        const auto exact=bridge.toWorld(held);
        const auto pan=soloCirclePan(held.x,held.y,bridge.screen,
                                     bridge.combatWidth/bridge.pixelsPerCell);
        const float requestedVelocity=std::hypot(pan.vx,pan.vy)*bridge.pixelsPerCell;
        const float trueCoopVelocity=std::min(bridge.combatWidth*3.10f,
                                              std::hypot(goal.x-exact.x,goal.y-exact.y)*12.0f);
        assert(std::abs(trueCoopVelocity-
                        std::min(bridge.combatWidth*3.10f,requestedVelocity))<.02f);
    }
    // Reverse held zone pushes in reverse direction without teleporting.
    const auto reverse=bridge.heldFingerPilotTarget({270,850});
    assert(reverse.y>bridge.toWorld({270,850}).y+70);
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
    // Placement and trajectory are separate questions. Two free locations
    // can have a solid rock wall BETWEEN them; a spawn-point check alone
    // cannot authorize movement. This tests map geometry, NOT asteroid
    // motion (the historical asteroid step currently ignores this resolver).
    const int cellX=12,cellY=12;
    auto setTile=[&](int x,int y,Tile tile){
        bridge.map.tiles[size_t(y)*bridge.map.width+x]=tile;
    };
    setTile(cellX-1,cellY,Tile::Empty);
    setTile(cellX,cellY,Tile::Rock);
    setTile(cellX+1,cellY,Tile::Empty);
    const float cell=bridge.pixelsPerCell;
    const CoopWorldPoint before{(cellX-.5f)*cell,(cellY+.5f)*cell};
    const CoopWorldPoint inside{(cellX+.5f)*cell,(cellY+.5f)*cell};
    const CoopWorldPoint after{(cellX+1.5f)*cell,(cellY+.5f)*cell};
    assert(bridge.canOccupy(before,12));
    assert(bridge.canOccupy(after,12));
    assert(!bridge.canOccupy(inside,12));
    // SAMPLE the entire trajectory independently of the ship resolver.
    // A geometric wall is present although BOTH endpoints are available.
    // Do not claim asteroid path physics is fixed by this placement test.
    int obstructedSamples=0;
    for(int i=0;i<=40;++i){
        const float t=float(i)/40.0f;
        if(!bridge.canOccupy({before.x+(after.x-before.x)*t,
                              before.y+(after.y-before.y)*t},12))
            ++obstructedSamples;
    }
    assert(obstructedSamples>0);
    return 0;
}
