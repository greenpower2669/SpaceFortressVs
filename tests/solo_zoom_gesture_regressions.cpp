#include "solo_zoom_gesture.hpp"
#include <cassert>
#include <cstdio>
int main(){
    sfsolo::ZoomGesture g;
    // Ordinary two fingers away from the ship stay available for weapons.
    assert(!g.down(1,300,300,0,100,100,30));
    assert(!g.down(2,350,300,0,100,100,30));
    g.reset();
    assert(g.down(1,100,100,0,100,100,30));
    assert(g.up(1,100,100,.1f));assert(g.selected);
    assert(g.down(2,100,100,.2f,100,100,30));
    assert(g.down(3,200,100,.2f,100,100,30));
    assert(g.motion(3,300,100,.3f));assert(g.zoom==2);
    g.motion(3,110,100,.4f);assert(g.zoom==1);
    assert(g.up(3,110,100,.5f));
    assert(g.motion(2,500,100,.6f)); // consumed until BOTH fingers lift
    assert(g.up(2,500,100,.7f));
    g.reset();assert(!g.selected && g.zoom==1);
    puts("PASS: explicit selection, pinch limits and exclusive touch ownership");
}
