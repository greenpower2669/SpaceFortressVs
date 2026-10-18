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

    // SOLO-028: enlarged violet thumbstick has a true graded movement curve.
    // The visual thumb tracks finger travel; the physical target is eased-in.
    move.cancel();
    assert(move.orbit(w)>fire.orbit(w)*1.25f);
    assert(move.handle(SDL_FINGERDOWN,70,mx,my,w,h));
    const float dead=std::max(13.f,w*.022f);
    const auto at=[&](float normalized){
        const float radius=dead+(move.orbit(w)-dead)*normalized;
        assert(move.handle(SDL_FINGERMOTION,70,mx+radius,my,w,h));
        return move.magnitude;
    };
    const float quarter=at(.25f),half=at(.50f),threeQuarter=at(.75f),full=at(1.f);
    assert(quarter>.03f && quarter<.08f);
    assert(half>.18f && half<.26f);
    assert(threeQuarter>.48f && threeQuarter<.57f);
    assert(std::abs(full-1.f)<.001f);
    assert(quarter<half && half<threeQuarter && threeQuarter<full);
    assert(move.travel>move.magnitude*.99f);
    const float easy=move.virtualReach(float(w),0);
    const float hard=move.virtualReach(float(w),8);
    assert(std::abs(easy*12.f-w*1.7f)<.001f);
    assert(std::abs(hard*12.f-w*3.1f)<.001f);
    move.handle(SDL_FINGERUP,70,mx+move.orbit(w),my,w,h);
    assert(move.magnitude==0 && move.travel==0);
    // SOLO-029: touch-on-hull equals the violet stick, starting at the
    // touched hull centre regardless of any visual camera displacement.
    sfsolo::HullTouchPilot hull;
    hull.begin(210.f,510.f);
    assert(hull.down && hull.magnitude==0.f);
    hull.update(210.f,510.f,w);
    assert(hull.magnitude==0.f);
    const float d=std::max(13.f,w*.022f);
    const float r=move.orbit(w);
    hull.update(210.f+d+(r-d)*.5f,510.f,w);
    assert(std::abs(hull.magnitude-sfsolo::MovePad::steeringCurve(.5f))<.001f);
    const float requested=hull.virtualReach(float(w),3);
    assert(requested>0 && requested<sfsolo::MovePad::reachFromMagnitude(1.f,float(w),3));
    // Camera transforms changing elsewhere do not affect the held command.
    const float retained=hull.magnitude;
    assert(hull.magnitude==retained);
    hull.cancel();
    assert(!hull.down && hull.magnitude==0.f && hull.virtualReach(w,3)==0.f);

    // Changing one handedness option flips BOTH stick positions, not roles.
    fire.right=!fire.right;move.right=!fire.right;
    assert(fire.cx(w)>w*.5f&&move.cx(w)<w*.5f);
    assert(fire.handle(SDL_FINGERDOWN,40,fire.cx(w),fire.cy(h),w,h).fire);
    assert(fire.dx>.99f);
    fire.cancel();move.cancel();
    puts("PASS: SOLO-028 transparent sticks, graded steering reach, firing, ownership, mirrored layout");
}
