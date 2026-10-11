#include "../src/solo_touch_controls.hpp"
#include <cassert>
#include <cmath>

static SDL_Event finger(Uint32 type,SDL_FingerID id,float x=.5f,float y=.5f){
    SDL_Event e{};
    e.type=type;
    e.tfinger.type=type;
    e.tfinger.fingerId=id;
    e.tfinger.x=x;
    e.tfinger.y=y;
    return e;
}
int main(){
    using namespace sfsolo;
    TouchPilot touch;
    Session s(generate({}));
    assert(s.start());
    constexpr int w=540,h=960;
    float ax=2,ay=2;
    assert(touch.handle(finger(SDL_FINGERDOWN,10,.2f,.6f),w,h)==TouchAction::None);
    assert(touch.down && !touch.chargeDown && touch.finger==10);
    pilotInput(s,touch,w,h,ax,ay);
    assert(std::isfinite(ax) && std::isfinite(ay));
    const float originalX=touch.targetX,originalY=touch.targetY;
    assert(touch.handle(finger(SDL_FINGERDOWN,20,.8f,.4f),w,h)==TouchAction::ChargePressed);
    assert(touch.down && touch.chargeDown && touch.chargeFinger==20);
    // The second finger never steals steering even if it moves across screen.
    assert(touch.handle(finger(SDL_FINGERMOTION,20,.1f,.1f),w,h)==TouchAction::None);
    assert(touch.targetX==originalX && touch.targetY==originalY);
    // A third finger must not cause a second press/release.
    assert(touch.handle(finger(SDL_FINGERDOWN,30,.9f,.9f),w,h)==TouchAction::None);
    assert(touch.handle(finger(SDL_FINGERUP,30),w,h)==TouchAction::None);
    assert(touch.chargeDown);
    assert(touch.handle(finger(SDL_FINGERUP,20),w,h)==TouchAction::ChargeReleased);
    assert(touch.down && !touch.chargeDown);
    assert(touch.handle(finger(SDL_FINGERMOTION,10,.75f,.2f),w,h)==TouchAction::None);
    assert(touch.targetX>originalX && touch.targetY<originalY);
    assert(touch.handle(finger(SDL_FINGERUP,10),w,h)==TouchAction::None);
    assert(!touch.down);
    pilotInput(s,touch,w,h,ax,ay);
    assert(ax==0 && ay==0);
    touch.handle(finger(SDL_FINGERDOWN,40),w,h);
    touch.handle(finger(SDL_FINGERDOWN,41),w,h);
    touch.reset();
    assert(!touch.down && !touch.chargeDown);
    assert(touch.chargeFinger==-1);
    // Ship-relative gesture routing: drag directly from the vessel while a
    // one-finger tap elsewhere triggers the directional weapon.
    const float shipX=w*.5f,shipY=h*.67f;
    assert(touch.handle(finger(SDL_FINGERDOWN,50,.93f,.11f),
                        w,h,shipX,shipY)==TouchAction::ChargePressed);
    assert(!touch.down && touch.chargeDown);
    assert(touch.handle(finger(SDL_FINGERUP,50,.93f,.11f),
                        w,h,shipX,shipY)==TouchAction::ChargeReleased);
    assert(touch.handle(finger(SDL_FINGERDOWN,51,.5f,.67f),
                        w,h,shipX,shipY)==TouchAction::None);
    assert(touch.down && !touch.chargeDown);
    assert(touch.handle(finger(SDL_FINGERDOWN,52,.9f,.11f),
                        w,h,shipX,shipY)==TouchAction::ChargePressed);
    assert(touch.handle(finger(SDL_FINGERMOTION,52,.12f,.5f),
                        w,h,shipX,shipY)==TouchAction::None);
    assert(touch.aimX<w*.2f);
    assert(touch.handle(finger(SDL_FINGERUP,52,.12f,.5f),
                        w,h,shipX,shipY)==TouchAction::ChargeReleased);
    assert(touch.down);
    touch.reset();

    // Phone drag with WORLD anchoring: panning the camera must not move the
    // ship's requested destination when the finger has stopped.
    Session motion(generate({}));assert(motion.start());
    motion.pilot.y=motion.map.height/2.0f;
    const SDL_Rect phone{0,0,709,1536};
    soloUpdateCamera(motion,phone);
    const auto initial=soloViewport(motion,phone);
    const float startX=initial.x(motion.pilot.x);
    const float startY=initial.y(motion.pilot.y);
    const float worldStart=motion.pilot.x;
    auto normalized=[&](float px,float py,SDL_FingerID id,Uint32 type){
        return finger(type,id,px/phone.w,py/phone.h);
    };
    assert(touch.handle(normalized(startX,startY,100,SDL_FINGERDOWN),
                       phone.w,phone.h,startX,startY,
                       motion.pilot.x,motion.pilot.y)==TouchAction::None);
    assert(touch.down&&touch.hasWorldAnchor);
    assert(touch.handle(normalized(startX+120,startY,100,SDL_FINGERMOTION),
                       phone.w,phone.h,startX,startY,
                       motion.pilot.x,motion.pilot.y)==TouchAction::None);
    float targetX=0,targetY=0;
    pilotTarget(motion,touch,phone.w,phone.h,targetX,targetY);
    assert(std::abs(targetX-(worldStart+120/initial.cell))<.0001f);
    assert(std::abs(targetY-motion.pilot.y)<.0001f);

    // Cross the right third. World camera pans, but pilot target is fixed.
    motion.pilot.x+=3.0f;
    motion.pilot.vx=9.0f;
    soloUpdateCamera(motion,phone,.05f);
    assert(soloViewport(motion,phone).left>initial.left);
    float afterPanX=0,afterPanY=0;
    pilotTarget(motion,touch,phone.w,phone.h,afterPanX,afterPanY);
    assert(std::abs(targetX-afterPanX)<.0001f);
    assert(std::abs(targetY-afterPanY)<.0001f);

    // Frozen controls don't shift aim/steering; only world physics slow.
    motion.iceSeconds=1.6f;
    pilotTarget(motion,touch,phone.w,phone.h,afterPanX,afterPanY);
    assert(std::abs(targetX-afterPanX)<.0001f);

    // Second finger continues aiming independently during the ship drag.
    const auto movedCamera=soloViewport(motion,phone);
    const auto action=touch.handle(normalized(phone.w*.9f,phone.h*.16f,101,
                                              SDL_FINGERDOWN),
                                   phone.w,phone.h,
                                   movedCamera.x(motion.pilot.x),
                                   movedCamera.y(motion.pilot.y),
                                   motion.pilot.x,motion.pilot.y);
    assert(action==TouchAction::ChargePressed);
    assert(touch.down && touch.chargeDown);
    touch.handle(normalized(phone.w*.08f,phone.h*.15f,101,SDL_FINGERMOTION),
                 phone.w,phone.h);
    assert(touch.aimX<phone.w*.2f);
    pilotTarget(motion,touch,phone.w,phone.h,afterPanX,afterPanY);
    assert(std::abs(targetX-afterPanX)<.0001f);
    assert(touch.handle(normalized(phone.w*.08f,phone.h*.15f,101,SDL_FINGERUP),
                        phone.w,phone.h)==TouchAction::ChargeReleased);
    assert(touch.handle(normalized(startX+120,startY,100,SDL_FINGERUP),
                        phone.w,phone.h)==TouchAction::None);
    assert(!touch.down && !touch.hasWorldAnchor);
    touch.reset();
}
