#include "../src/solo_session.hpp"
#include "../src/solo_vector_aim.hpp"
#include <cassert>
#include <cmath>

int main(){
    using namespace sfsolo;
    for(unsigned world=1;world<=6;++world){
        for(unsigned stage=1;stage<=20;++stage){
            const Map map=generate({world,stage,1});
            assert(map.valid()&&map.version==3);
            assert(soloMazeConnected(map));
            // The model isn't a narrow straight line any more: both sides
            // have reachable open passages and three separated objectives.
            int left=0,right=0,beacons=0,rock=0;
            for(int y=0;y<map.height;++y)
                for(int x=0;x<map.width;++x){
                    const auto t=map.at(x,y);
                    rock+=(t==Tile::Rock);
                    if(soloPassable(t)){
                        if(x<map.width/3)++left;
                        if(x>map.width*2/3)++right;
                    }
                    beacons+=(t==Tile::Trigger);
                }
            assert(rock>map.width*map.height/4);
            assert(left>60&&right>60&&beacons==3);
        }
    }
    Session s(generate({}));
    assert(s.start()&&s.relaysNeeded==3);
    const int mid=s.map.width/2;
    // All three relays must be approached through the reachable maze.
    for(int y=0;y<s.map.height;++y)
        for(int x=0;x<s.map.width;++x){
            if(s.map.at(x,y)!=Tile::Trigger)continue;
            s.pilot.x=x+.5f;s.pilot.y=y+.5f;
            s.pilot.vx=s.pilot.vy=0;
            s.step(0,0,.01f);
        }
    assert(s.relaysCollected==3);
    s.pilot.x=mid+.5f;s.pilot.y=9.5f;
    s.step(0,0,.01f);
    assert(s.phase==Phase::BossFight);
    s.defeatBoss();
    s.pilot.x=mid+.5f;s.pilot.y=2.5f;
    s.step(0,0,.01f);
    assert(s.phase==Phase::Won);
    assert(s.reachedFinish && s.missionReady());

    // Shared canonical finite vector interpolation, 360 movement without
    // changing any of the other modes' acceleration/velocity code.
    const auto delta=sfTouchMovementDelta(30,-40,20,.1f);
    assert(std::abs(delta.x-1.2f)<.0001f);
    assert(std::abs(delta.y+1.6f)<.0001f);
    const auto exact=sfTouchMovementDelta(.12f,-.31f,5,.1f);
    assert(std::abs(exact.x-.12f)<.0001f);
    assert(std::abs(exact.y+.31f)<.0001f);

    // Classic cannon cone footprint rotated through all four quadrants.
    sfBossDangerIndex=0;
    const float range=9;
    const AimVector up{0,-1},down{0,1},left{-1,0},right{1,0};
    assert(inAimedCanonicalCone(10,10,10,6,up,range));
    assert(!inAimedCanonicalCone(10,10,10,6,down,range));
    assert(inAimedCanonicalCone(10,10,10,14,down,range));
    assert(inAimedCanonicalCone(10,10,6,10,left,range));
    assert(inAimedCanonicalCone(10,10,14,10,right,range));
    const auto diagonal=directionTo(10,10,14,14);
    assert(inAimedCanonicalCone(10,10,12,12,diagonal,range));
    assert(!inAimedCanonicalCone(10,10,7,7,diagonal,range));
    sfBossDangerIndex=2;

    // Frost dust hurts responsiveness TEMPORARILY, never permanently.
    Session snow(generate({}));
    assert(snow.start());
    snow.scatterIce(snow.pilot.x,snow.pilot.y);
    assert(snow.dust.size()==8);
    snow.step(0,0,.05f);
    assert(snow.iceActive());
    assert(snow.mobility()==.25f);
    snow.dust.clear();
    for(int i=0;i<55;++i)snow.step(0,0,.05f);
    assert(!snow.iceActive() && snow.mobility()==1.0f);
}
