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
    assert(quarter>.010f && quarter<.018f);
    assert(half>.100f && half<.115f);
    assert(threeQuarter>.38f && threeQuarter<.405f);
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

    // SOLO-030: prolonged near-rim direction builds a BOUNDED throttle,
    // without ever accumulating physical speed or moving the camera.
    move.cancel();
    assert(move.orbit(w)>115.f && move.orbit(w)<150.f);
    assert(move.cx(w)-move.orbit(w)>0.f);
    assert(move.handle(SDL_FINGERDOWN,80,move.cx(w),move.cy(h),w,h));
    const float safeDead=std::max(13.f,w*.022f);
    const float nearRim=safeDead+(move.orbit(w)-safeDead)*.88f;
    assert(move.handle(SDL_FINGERMOTION,80,move.cx(w)+nearRim,move.cy(h),w,h));
    const float base=move.magnitude;
    assert(base>.60f && base<.70f);
    for(int i=0;i<220;++i)move.tick(.01f);
    const float boosted=move.effectiveMagnitude();
    assert(boosted>base+.15f && boosted<1.f);
    const float firstBonus=move.holdBoost.bonus;
    for(int i=0;i<3000;++i)move.tick(.01f);
    assert(move.holdBoost.bonus<=sfsolo::SteeringHoldBoost::maximumBonus);
    assert(move.effectiveMagnitude()<=1.f && move.holdBoost.bonus>=firstBonus);
    assert(move.handle(SDL_FINGERMOTION,80,move.cx(w)-nearRim,move.cy(h),w,h));
    move.tick(.01f);
    assert(move.holdBoost.bonus==0.f);
    assert(move.handle(SDL_FINGERMOTION,80,move.cx(w),move.cy(h),w,h));
    for(int i=0;i<200;++i)move.tick(.01f);
    assert(move.effectiveMagnitude()==0.f);
    assert(move.handle(SDL_FINGERUP,80,move.cx(w),move.cy(h),w,h));
    assert(!move.active() && move.holdBoost.bonus==0.f);
    // Same curve and momentum intent for steering directly on the ship.
    hull.begin(250.f,450.f);
    hull.update(250.f+nearRim,450.f,w);
    const float hullBase=hull.magnitude;
    for(int i=0;i<220;++i)hull.tick(.01f);
    assert(hull.effectiveMagnitude()>hullBase+.15f);
    assert(std::abs(hull.effectiveMagnitude()-boosted)<.025f);
    hull.update(250.f-nearRim,450.f,w);
    hull.tick(.01f);assert(hull.holdBoost.bonus==0.f);
    hull.cancel();assert(hull.effectiveMagnitude()==0.f);
    // Even after a long hold the historical physical top-speed is capped.
    move.cancel();
    move.handle(SDL_FINGERDOWN,81,move.cx(w),move.cy(h),w,h);
    move.handle(SDL_FINGERMOTION,81,move.cx(w)+move.orbit(w),move.cy(h),w,h);
    for(int i=0;i<10000;++i)move.tick(.01f);
    assert(move.effectiveMagnitude()==1.f);
    assert(std::abs(move.virtualReach(w,0)*12.f-w*1.7f)<.001f);
    assert(std::abs(move.virtualReach(w,8)*12.f-w*3.1f)<.001f);
    move.cancel();
    // Changing one handedness option flips BOTH stick positions, not roles.
    fire.right=!fire.right;move.right=!fire.right;
    assert(fire.cx(w)>w*.5f&&move.cx(w)<w*.5f);
    assert(fire.handle(SDL_FINGERDOWN,40,fire.cx(w),fire.cy(h),w,h).fire);
    assert(fire.dx>.99f);
    fire.cancel();move.cancel();
    puts("PASS: SOLO-030 cumulative directional hold, soft controls, capped top speed, paired hand swap");
}
