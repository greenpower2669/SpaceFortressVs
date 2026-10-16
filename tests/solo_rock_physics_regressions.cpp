#include "solo_rock_physics.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
int main(){
    using namespace sfsolo;
    RockBody a{0,0,0,0,10},b{19,0,0,0,10};
    for(int i=0;i<10000;++i){assert(rockContact(a,b)==0);assert(a.x==0 && b.x==19 && a.vx==0 && b.vx==0);}
    a.vx=100;const float energy=rockContact(a,b);assert(energy>0 && b.vx>0 && a.vx<100);
    const auto after=a.vx;assert(rockContact(a,b)==0 && a.vx==after); // separating pair never re-impulsed
    for(int i=0;i<2000;++i){rockDamp(a,1.f/60,1);rockDamp(b,1.f/60,1);}
    assert(a.vx==0 && b.vx==0);
    for(int fps:{30,60,120}){RockBody c{0,0,100,20,10};for(int i=0;i<fps;++i)rockDamp(c,1.f/fps,1);assert(std::abs(c.vx-100*std::exp(-.85f))<.01f);}
    puts("PASS: static pairs, dissipative impact, wake, no repeated impulse and stable sleep");
}
