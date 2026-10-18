#include "solo_fire_pad.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
int main(){
    const int w=540,h=960;
    sfsolo::FirePad fire;
    sfsolo::MovePad move;
    move.right=!fire.right;
    const float fx=fire.cx(w),fy=fire.cy(h);
    assert(fx<w*.5f&&move.cx(w)>w*.5f);
    assert(fire.handle(SDL_FINGERDOWN,1,fx,fy,w,h).owned);
    // Immediate press uses canonical remembered direction (-Y).
    assert(fire.dy==-1 && fire.tick(.02f));
    assert(fire.handle(SDL_FINGERMOTION,1,fx+80,fy,w,h).owned);
    assert(fire.dx>.99f && std::abs(fire.dy)<.01f);
    assert(fire.handle(SDL_FINGERUP,1,fx+80,fy,w,h).owned);
    assert(!fire.active());
    assert(fire.handle(SDL_FINGERDOWN,2,fx,fy,w,h).fire);
    assert(fire.dx>.99f); // centre re-tap retained the previous right vector
    int on=0,off=0;
    for(int i=0;i<220;++i){
        if(fire.tick(.02f))++on;else ++off;
    }
    assert(on>0&&off>0); // periodic mandatory rest protects the shield
    fire.handle(SDL_FINGERUP,2,fx,fy,w,h);
    assert(!fire.active());

    // Opposite-hand violet movement uses its own finger.
    const float mx=move.cx(w),my=move.cy(h);
    assert(move.handle(SDL_FINGERDOWN,30,mx,my,w,h));
    assert(move.active() && move.magnitude==0);
    assert(!fire.handle(SDL_FINGERDOWN,30,mx,my,w,h).owned);
    assert(move.handle(SDL_FINGERMOTION,30,mx-90,my-90,w,h));
    assert(move.dx<-.69f&&move.dy<-.69f&&move.magnitude>.90f);
    assert(!move.handle(SDL_FINGERMOTION,31,mx,my,w,h)); // different finger
    assert(move.handle(SDL_FINGERUP,30,mx-90,my-90,w,h));
    assert(!move.active() && move.magnitude==0);

    // Changing one handedness option flips BOTH stick positions, not roles.
    fire.right=!fire.right;move.right=!fire.right;
    assert(fire.cx(w)>w*.5f&&move.cx(w)<w*.5f);
    assert(fire.handle(SDL_FINGERDOWN,40,fire.cx(w),fire.cy(h),w,h).fire);
    assert(fire.dx>.99f);
    fire.cancel();move.cancel();
    puts("PASS: SOLO-027 neon sticks, fire burst/rest, remembered aim, two-finger ownership and mirrored layout");
}
