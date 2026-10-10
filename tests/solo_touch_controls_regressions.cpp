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
}
