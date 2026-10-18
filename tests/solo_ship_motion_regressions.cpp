#include "solo_ship_motion.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
using sfsolo::ShipMotion;
static ShipMotion run(int danger,int fps){
    ShipMotion s;
    for(int i=0;i<fps*2;++i)s.step(10000,0,true,780,danger,1.f/fps);
    for(int i=0;i<fps;++i)s.step(0,0,false,780,danger,1.f/fps);
    return s;
}
int main(){
    float prior=0;
    for(int d=0;d<9;++d){
        ShipMotion s;s.step(10000,0,true,780,d,.01f);
        assert(s.vx>0 && s.vx<780); // acceleration, not target velocity teleport
        for(int i=0;i<200;++i)s.step(10000,0,true,780,d,.01f);
        const float speed=s.vx;
        s.step(-10000,0,true,780,d,.01f);assert(s.vx>0 && s.vx<speed);
        const float before=s.x;
        s.step(0,0,false,780,d,.1f);assert(s.x>before && s.vx>0);
        const auto a=run(d,30),b=run(d,60),c=run(d,120);
        assert(std::abs(a.x-c.x)<8 && std::abs(b.x-c.x)<8);
        assert(a.x>prior);prior=a.x;
    }
    ShipMotion s;
    for(int i=0;i<100;++i)s.step(5000,0,true,780,8,.01f);
    const float finger=s.x+10;
    s.step(finger,0,true,780,8,.05f);assert(s.x>finger);
    const float before=s.x;
    for(int i=0;i<20;++i)s.step(before-5000,0,true,780,8,.01f);
    assert(s.x>before); // genuine reversal lag
    for(int i=0;i<400;++i)s.step(before-5000,0,true,780,8,.01f);
    assert(s.vx<0 && s.x<before);
    // SOLO-031: the optional overdrive is not a teleport and never exceeds
    // the 130% physical limiter. Legacy six-parameter calls remain identical.
    for(int danger=0;danger<9;++danger){
        ShipMotion ordinary,overdrive;
        for(int i=0;i<450;++i){
            ordinary.step(100000.f,0,true,780,danger,.01f);
            overdrive.step(100000.f,0,true,780,danger,.01f,1.30f);
        }
        const float nominal=780.f*(1.7f+1.4f*float(danger)/8.f);
        assert(ordinary.vx<nominal+1.f);
        assert(overdrive.vx>ordinary.vx+nominal*.07f);
        assert(overdrive.vx<=nominal*1.30f+1.f);
        const float before=overdrive.x,priorSpeed=overdrive.vx;
        overdrive.step(-100000.f,0,true,780,danger,.025f,1.f);
        assert(overdrive.x>before && overdrive.vx<priorSpeed); // inertia
        for(int i=0;i<450;++i)
            overdrive.step(0,0,false,780,danger,.01f,1.f);
        assert(std::abs(overdrive.vx)<nominal*.03f);
    }
    puts("PASS: nine danger inertias, finite SOLO-031 overdrive, smooth brake/reverse, legacy behavior");
}
