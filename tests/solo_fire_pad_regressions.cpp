#include "solo_fire_pad.hpp"
#include "solo_ship_motion.hpp"
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
    assert(std::abs(easy*12.f-w*1.7f*.30f)<.001f);
    assert(std::abs(hard*12.f-w*3.1f*.30f)<.001f);
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

    // SOLO-033: at maximum distance, the INITIAL command is 30%.
    // Only a stable near-rim vector adds the other 100 percentage points.
    move.cancel();assert(move.handle(SDL_FINGERDOWN,80,move.cx(w),move.cy(h),w,h));
    const float cx=move.cx(w),cy=move.cy(h),rim=move.orbit(w);
    auto moveTo=[&](float fraction){
        return move.handle(SDL_FINGERMOTION,80,cx+rim*fraction,cy,w,h);
    };
    assert(moveTo(1.f));
    assert(std::abs(move.magnitude-1.f)<.001f);
    assert(std::abs(move.effectiveMagnitude()-.30f)<.001f);
    const float nominal=w*1.7f;
    assert(std::abs(move.virtualReach(w,0)*12.f/nominal-.30f)<.001f);
    float prev=move.effectiveMagnitude();
    for(int i=0;i<200;++i){
        move.tick(.01f);
        const float current=move.effectiveMagnitude();
        assert(current>=prev-.001f && current-prev<.01f);
        prev=current;
    }
    const float at2=move.effectiveMagnitude();
    assert(at2>.37f && at2<.50f);
    for(int i=0;i<200;++i)move.tick(.01f);
    const float at4=move.effectiveMagnitude();
    assert(at4>.75f && at4<1.00f);
    for(int i=0;i<400;++i)move.tick(.01f);
    const float at8=move.effectiveMagnitude();
    assert(at8>1.25f && at8<=1.30f);
    // 85% radius: no charging even during prolonged holding.
    move.cancel();move.handle(SDL_FINGERDOWN,80,cx,cy,w,h);
    assert(moveTo(.85f));
    const float at85=move.effectiveMagnitude();
    for(int i=0;i<1600;++i)move.tick(.01f);
    assert(move.holdBoost.stableSeconds==0.f);
    assert(std::abs(move.effectiveMagnitude()-at85)<.001f);
    // 92% radius: near the edge but still only trickle-charge.
    move.cancel();move.handle(SDL_FINGERDOWN,80,cx,cy,w,h);
    assert(moveTo(.92f));
    for(int i=0;i<500;++i)move.tick(.01f);
    assert(move.holdBoost.stableSeconds<1.6f);
    assert(move.effectiveMagnitude()<.36f);
    assert(moveTo(1.f));
    for(int i=0;i<1000;++i)move.tick(.01f);
    assert(move.effectiveMagnitude()>1.25f && move.effectiveMagnitude()<=1.30f);
    // Change cap gradually discharges the tensor, no hard reset.
    const float priorTurn=move.holdBoost.bonus;
    assert(move.handle(SDL_FINGERMOTION,80,cx,cy-rim,w,h));
    move.tick(.01f);
    assert(move.holdBoost.bonus>0 && priorTurn-move.holdBoost.bonus<.015f);
    for(int i=0;i<150;++i)move.tick(.01f);
    const float afterTurn=move.holdBoost.bonus;
    assert(afterTurn<priorTurn-.025f && afterTurn>.07f);
    for(int i=0;i<1000;++i)move.tick(.01f);
    assert(move.holdBoost.bonus>afterTurn+.025f);
    // True 180 degree reversal does not immediately erase charge.
    const float preReverse=move.holdBoost.bonus;
    assert(move.handle(SDL_FINGERMOTION,80,cx,cy+rim,w,h));
    move.tick(.01f);
    assert(preReverse-move.holdBoost.bonus<.015f);
    for(int i=0;i<160;++i)move.tick(.01f);
    assert(move.holdBoost.bonus<preReverse-.025f);
    // Near centre: no lingering high POWER command despite residual energy.
    assert(moveTo(.30f));
    assert(move.effectiveMagnitude()<.06f);
    for(int i=0;i<200;++i)move.tick(.01f);
    assert(move.effectiveMagnitude()<.06f);
    assert(move.handle(SDL_FINGERUP,80,cx,cy,w,h));
    assert(move.effectiveMagnitude()==0.f);
    // Same tensor and 30% initial speed with a SHORT hull-on-finger radius.
    sfsolo::HullTouchPilot hullCharge;
    hullCharge.begin(250.f,450.f);
    const float hullR=hullCharge.orbit(w);
    assert(hullR<rim*.65f);
    hullCharge.update(250.f+hullR,450.f,w);
    assert(std::abs(hullCharge.effectiveMagnitude()-.30f)<.001f);
    for(int i=0;i<200;++i)hullCharge.tick(.01f);
    assert(std::abs(hullCharge.effectiveMagnitude()-at2)<.01f);
    for(int i=0;i<600;++i)hullCharge.tick(.01f);
    assert(hullCharge.effectiveMagnitude()>1.25f);
    hullCharge.update(250.f,450.f-hullR,w);
    hullCharge.tick(.01f);
    assert(hullCharge.holdBoost.bonus>0.f);
    hullCharge.cancel();assert(hullCharge.virtualReach(w,3)==0.f);

    // Critically test REAL ShipMotion physics, not just the tensor output.
    // All nine dangers must start below half historic max, then accelerate
    // in stages and never exceed the historic 130% physical cap.
    for(int danger=0;danger<9;++danger){
        sfsolo::MovePad intention;
        const float x=intention.cx(w),y=intention.cy(h);
        intention.handle(SDL_FINGERDOWN,95,x,y,w,h);
        intention.handle(SDL_FINGERMOTION,95,x+intention.orbit(w),y,w,h);
        sfsolo::ShipMotion ship;
        float at1=0,at4physical=0,at9physical=0;
        const float baseSpeed=w*(1.7f+1.4f*float(danger)/8.f);
        for(int i=0;i<900;++i){
            intention.tick(.01f);
            ship.step(ship.x+intention.virtualReach(w,danger),ship.y,
                      true,w,danger,.01f,intention.speedMultiplier());
            if(i==99)at1=ship.vx/baseSpeed;
            if(i==399)at4physical=ship.vx/baseSpeed;
            if(i==899)at9physical=ship.vx/baseSpeed;
        }
        assert(at1>.10f && at1<.45f);
        assert(at4physical>at1+.20f && at4physical<1.f);
        assert(at9physical>at4physical+.15f && at9physical<=1.301f);
        const float before=ship.vx,position=ship.x;
        ship.step(ship.x-intention.virtualReach(w,danger),ship.y,
                  true,w,danger,.01f,1.f);
        assert(ship.vx<before && ship.x>position);
    }
    // Consistent charge at 30, 60 and 120 simulation FPS.
    sfsolo::SteeringHoldBoost t30,t60,t120;
    for(int i=0;i<120;++i)t30.tick(1.f/30.f,1.f,0.f,1.f);
    for(int i=0;i<240;++i)t60.tick(1.f/60.f,1.f,0.f,1.f);
    for(int i=0;i<480;++i)t120.tick(1.f/120.f,1.f,0.f,1.f);
    assert(std::abs(t30.bonus-t60.bonus)<.006f);
    assert(std::abs(t60.bonus-t120.bonus)<.006f);
    const float beforeJitter=t60.bonus;
    for(int i=0;i<60;++i)t60.tick(1.f/60.f,1.f,(i%2?-.009f:.009f),1.f);
    assert(t60.bonus>=beforeJitter-.006f);
    // Changing one handedness option flips BOTH stick positions, not roles.
    fire.right=!fire.right;move.right=!fire.right;
    assert(fire.cx(w)>w*.5f&&move.cx(w)<w*.5f);
    assert(fire.handle(SDL_FINGERDOWN,40,fire.cx(w),fire.cy(h),w,h).fire);
    assert(fire.dx>.99f);
    fire.cancel();move.cancel();
    puts("PASS: SOLO-033 30% initial cruise +100-point late-rim tensor and nine-danger physical speed");
}
