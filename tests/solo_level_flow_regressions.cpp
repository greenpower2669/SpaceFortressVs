#include "solo_level_flow.hpp"
#include <cassert>
#include <cstdio>
int main(){
    using namespace sfsolo;
    CoopSoloWorldBridge world;MapOptions options;options.width=54;options.height=160;
    assert(world.create(options,{0,0,540,960},540));
    LevelFlow flow;assert(flow.create(world,40.5f));
    assert(flow.minibossAnchors.size()==4);
    assert(world.canOccupy(flow.lair,40.5f));
    for(unsigned i=0;i<4;++i){
        assert(world.canOccupy(flow.minibossAnchors[i],40.5f));
        for(unsigned j=0;j<i;++j)
            assert(std::hypot(flow.minibossAnchors[i].x-flow.minibossAnchors[j].x,
                              flow.minibossAnchors[i].y-flow.minibossAnchors[j].y)>100);
    }
    // The camera can be over the entrance while the physical ship is far away.
    world.centerOn(flow.lair);assert(!flow.enterLair(world.start()));
    assert(flow.phase==LevelPhase::Exploration);
    assert(flow.enterLair(flow.lair));assert(!flow.enterLair(flow.lair));
    flow.advanceTransition(.5f,false);assert(flow.transitionSeconds==0);
    assert(!flow.advanceTransition(.99f,true));
    assert(flow.advanceTransition(.01f,true));
    assert(flow.phase==LevelPhase::FinalArena);
    assert(!flow.enterLair(flow.lair));
    flow.phase=LevelPhase::Transition;flow.transitionSeconds=0;
    assert(flow.gameplayTime(.5f,false)==0 && flow.transitionSeconds==0);
    assert(flow.gameplayTime(.98f,true)==0);
    assert(std::abs(flow.gameplayTime(.05f,true)-.03f)<1e-6f);
    assert(flow.phase==LevelPhase::FinalArena);
    assert(flow.gameplayTime(.05f,true)==.05f);
    for(unsigned w=1;w<=6;++w)for(unsigned stage:{1u,10u,20u}){
        options.world=w;options.stage=stage;options.difficulty=stage==1?1:stage==10?5:9;
        assert(world.create(options,{0,0,540,960},540));
        assert(flow.create(world,40.5f) && flow.minibossAnchors.size()==4);
    }
    // Full hull traversability, not only open centres: a blocked map fails safely.
    std::fill(world.map.tiles.begin(),world.map.tiles.end(),Tile::Rock);
    assert(!flow.create(world,40.5f));
    std::puts("PASS: accessible miniboss route, physical lair trigger and paused one-shot transition");
}
