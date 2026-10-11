#include "../src/solo_combat.hpp"
#include <cassert>
#include <cmath>
int main(){
    using namespace sfsolo;
    Session s(generate({}));assert(s.start());
    s.canonicalShipActive=true;
    assert(std::abs(s.energyFraction()-1)<1e-6f);
    Combat c;
    assert(c.fire(s,s.pilot.x,s.pilot.y-10));
    assert(c.shots.size()==1&&c.shots[0].missile);
    assert(c.shots[0].damage==60&&std::abs(s.heatSpent-11)<.001f);
    assert(!c.fire(s,s.pilot.x,s.pilot.y-10));
    for(int n=0;n<5;++n)c.step(s,.05f); // real frame dt capped at 50 ms
    assert(c.fire(s,s.pilot.x,s.pilot.y-10));
    assert(!c.shots.back().missile&&c.shots.back().damage==12);
    assert(s.heatSpent==12);
    const float h=s.heatSpent,health=s.pilot.health;
    const float hull=sfShieldDamage(50,h),wear=sfShieldWear(50,h);
    s.damage(50);
    assert(std::abs(s.pilot.health-(health-hull*.1f))<.0001f);
    assert(std::abs(s.heatSpent-(h+wear))<.001f);
    const float spent=s.heatSpent;
    s.step(0,0,.05f);
    assert(s.heatSpent<spent);
    s.heatSpent=0;
    const float full=s.pilot.health;
    s.damage(8);
    assert(std::abs(s.pilot.health-full)<.0001f);
    assert(s.heatSpent>=.8f);
    sfKineticWaves.clear();
}
