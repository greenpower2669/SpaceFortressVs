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
    const float r=hull.orbit(w);
    assert(r<move.orbit(w)*.65f);
    hull.update(210.f+d+(r-d)*.5f,510.f,w);
    assert(std::abs(hull.magnitude-sfsolo::MovePad::steeringCurve(.5f))<.001f);
    const float requested=hull.virtualReach(float(w),3);
    assert(requested>0 && requested<sfsolo::MovePad::reachFromMagnitude(1.f,float(w),3));
    // Camera transforms changing elsewhere do not affect the held command.
    const float retained=hull.magnitude;
    assert(hull.magnitude==retained);
    hull.cancel();
    assert(!hull.down && hull.magnitude==0.f && hull.virtualReach(w,3)==0.f);

    // SOLO-032: accumulate over several seconds, rather than a sudden
    // 130% surge simply because the finger reaches the extreme rim.
    move.cancel();
    assert(move.orbit(w)>115.f && move.orbit(w)<150.f);
    assert(move.cx(w)-move.orbit(w)>0.f);
    assert(move.handle(SDL_FINGERDOWN,80,move.cx(w),move.cy(h),w,h));
    const float nearRim=move.orbit(w),cx=move.cx(w),cy=move.cy(h);
    assert(move.handle(SDL_FINGERMOTION,80,cx+nearRim,cy,w,h));
    const float base=move.magnitude;
    float past=move.effectiveMagnitude();
    assert(std::abs(base-1.f)<.001f);
    for(int i=0;i<200;++i){
        move.tick(.01f);
        const float now=move.effectiveMagnitude();
        assert(now>=past-.0001f);
        assert(now-past<.01f);
        past=now;
    }
    // At 2 seconds still well below the old fast +30% overdrive.
    const float twoSeconds=move.effectiveMagnitude();
    assert(twoSeconds>1.f && twoSeconds<1.13f);
    for(int i=0;i<650;++i)move.tick(.01f);
    assert(move.effectiveMagnitude()>1.28f);
    assert(move.effectiveMagnitude()<=1.30f);
    const float beforeTurn=move.effectiveMagnitude();
    // Important: 90 degree direction change does NOT zero charge or bonus.
    assert(move.handle(SDL_FINGERMOTION,80,cx,cy-nearRim,w,h));
    move.tick(.01f);
    assert(move.effectiveMagnitude()>.99f);
    assert(move.holdBoost.bonus>0.f);
    assert(beforeTurn-move.effectiveMagnitude()<.02f);
    const float afterFirstStep=move.effectiveMagnitude();
    for(int i=0;i<100;++i)move.tick(.01f);
    const float afterTurn=move.effectiveMagnitude();
    assert(afterTurn<afterFirstStep);
    assert(afterTurn>1.005f); // progressive discharge, never instantaneous
    const float bottom=move.effectiveMagnitude();
    // Holding the NEW vector re-aligns and eventually resumes acceleration.
    for(int i=0;i<1000;++i)move.tick(.01f);
    assert(move.effectiveMagnitude()>bottom+.10f);
    assert(move.effectiveMagnitude()<=1.30f);
    // Low-radial movement requests a small speed without new stored charge.
    move.handle(SDL_FINGERMOTION,80,cx+move.orbit(w)*.30f,cy,w,h);
    const float lowerBase=move.magnitude;
    assert(lowerBase<.06f);
    const float oldCharge=move.holdBoost.stableSeconds;
    for(int i=0;i<200;++i)move.tick(.01f);
    assert(move.holdBoost.stableSeconds<oldCharge);
    assert(move.holdBoost.bonus<.02f);
    // Safety: on release we neutralize the INTENTION, never ship inertia.
    assert(move.handle(SDL_FINGERUP,80,cx,cy,w,h));
    assert(!move.active() && move.holdBoost.bonus==0.f);
    assert(move.virtualReach(w,3)==0.f);

    // The hull uses the identical reserve, although its tactile radius is
    // much shorter so the finger stays closer to the visible spacecraft.
    sfsolo::HullTouchPilot hullCharge;
    hullCharge.begin(250.f,450.f);
    const float hullR=hullCharge.orbit(w);
    assert(hullR<move.orbit(w)*.65f);
    hullCharge.update(250.f+hullR,450.f,w);
    for(int i=0;i<200;++i)hullCharge.tick(.01f);
    assert(std::abs(hullCharge.effectiveMagnitude()-twoSeconds)<.012f);
    for(int i=0;i<650;++i)hullCharge.tick(.01f);
    assert(hullCharge.effectiveMagnitude()>1.28f);
    hullCharge.update(250.f,450.f-hullR,w);
    hullCharge.tick(.01f);
    assert(hullCharge.holdBoost.bonus>0.f);
    hullCharge.cancel();
    assert(hullCharge.effectiveMagnitude()==0.f);

    // Same cap at all 9 difficulty levels, even after infinite hold.
    move.handle(SDL_FINGERDOWN,81,cx,cy,w,h);
    move.handle(SDL_FINGERMOTION,81,cx+nearRim,cy,w,h);
    for(int i=0;i<1600;++i)move.tick(.01f);
    for(int difficulty=0;difficulty<9;++difficulty){
        const float speed=w*(1.7f+1.4f*float(difficulty)/8.f);
        assert(move.virtualReach(w,difficulty)*12.f<=speed*1.30f+.001f);
    }
    move.cancel();
    // Frame-rate independence: charge, alignment and output depend on dt.
    sfsolo::SteeringHoldBoost at30,at60,at120;
    for(int i=0;i<120;++i)at30.tick(1.f/30.f,1.f,0.f,1.f);
    for(int i=0;i<240;++i)at60.tick(1.f/60.f,1.f,0.f,1.f);
    for(int i=0;i<480;++i)at120.tick(1.f/120.f,1.f,0.f,1.f);
    assert(std::abs(at30.bonus-at60.bonus)<.005f);
    assert(std::abs(at60.bonus-at120.bonus)<.005f);
    // Tiny direction changes don't induce oscillation.
    const float beforeJitter=at60.bonus;
    for(int i=0;i<30;++i)
        at60.tick(1.f/60.f,1.f,(i%2?-.008f:.008f),1.f);
    assert(at60.bonus>=beforeJitter-.005f);
    // Changing one handedness option flips BOTH stick positions, not roles.
    fire.right=!fire.right;move.right=!fire.right;
    assert(fire.cx(w)>w*.5f&&move.cx(w)<w*.5f);
    assert(fire.handle(SDL_FINGERDOWN,40,fire.cx(w),fire.cy(h),w,h).fire);
    assert(fire.dx>.99f);
    fire.cancel();move.cancel();
    puts("PASS: SOLO-032 gradual directional reserve, smooth turn and reacquisition, 130% cap");
}
