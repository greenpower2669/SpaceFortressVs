#include "../src/kinetic_shield.hpp"
#include <cassert>
#include <cmath>
int main(){
    SfKineticAimAxes context;
    assert(context.direction(0).y==1&&context.direction(1).y==-1);
    assert(!context.active(1));
    context.aimAt(1,100,0);
    assert(context.active(1));
    assert(context.direction(1).x==1&&context.direction(1).y==0);
    context.aimAt(1,0,0); // retains last valid heading
    assert(context.direction(1).x==1);
    for(int difficulty=0;difficulty<9;++difficulty){
        sfBossDangerIndex=difficulty;
        // The exact canonical cone now rotates to horizontal +X.
        assert(sfKineticSurgeConeContainsAxis(10,20,1,0,90,20,120));
        assert(!sfKineticSurgeConeContainsAxis(10,20,1,0,-50,20,120));
        assert(!sfKineticSurgeConeContainsAxis(10,20,1,0,90,140,120));
        // Both historical orientations still agree with shared axis code.
        for(float dx:{-70.0f,0.0f,40.0f})for(float dy:{-100.0f,0.0f,140.0f}){
            const bool top=sfKineticSurgeConeContains(0,0,0,dx,dy,250);
            const bool bottom=sfKineticSurgeConeContains(1,0,0,dx,dy,250);
            assert(top==sfKineticSurgeConeContainsAxis(0,0,0,1,dx,dy,250));
            assert(bottom==sfKineticSurgeConeContainsAxis(0,0,0,-1,dx,dy,250));
        }
    }
    sfBossDangerIndex=2;
    return 0;
}
