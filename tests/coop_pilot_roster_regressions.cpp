#include "../src/coop_pilot_roster.hpp"
#include <cassert>
int main(){
    const SfCoopPilotRoster legacy;
    assert(legacy.count()==2&&legacy.active(0)&&legacy.active(1));
    assert(!legacy.active(-1)&&!legacy.active(2));
    assert(!legacy.allDefeated(1000,0)); // historical ally can be dead
    assert(!legacy.allDefeated(0,1000));
    assert(legacy.allDefeated(0,0));
    const auto solo=SfCoopPilotRoster::single(1);
    assert(solo.count()==1&&!solo.active(0)&&solo.active(1));
    assert(solo.anyAlive(1000,900));
    assert(solo.anyAlive(0,900));
    assert(solo.allDefeated(1000,0)); // absent owner 0 cannot rescue/keep match alive
    const auto orange=SfCoopPilotRoster::single(0);
    assert(orange.allDefeated(0,1000)); // absent owner 1 cannot rescue/keep match alive
    assert(orange.anyAlive(900,0));
    assert(SfCoopPilotRoster::single(3).count()==0);
    return 0;
}
