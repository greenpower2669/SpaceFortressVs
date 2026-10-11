#include "../src/solo_touch_controls.hpp"
#include "../src/solo_viewport.hpp"
#include "../src/solo_kinetic_control.hpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cassert>
#include <cmath>

static SDL_Event touchEvent(Uint32 type, SDL_FingerID id,float x,float y){
    SDL_Event e{};
    e.type=type;e.tfinger.type=type;e.tfinger.fingerId=id;
    e.tfinger.x=x;e.tfinger.y=y;
    return e;
}
static bool near(float a,float b,float epsilon=.001f){
    return std::abs(a-b)<epsilon;
}
int main(){
    using namespace sfsolo;
    const SDL_Rect phone{0,0,709,1536};
    const float dt=.04f;
    Session flight(generate({}));
    assert(flight.start());
    // Clear a realistic long unobstructed corridor to test a continuous
    // 10-second tactile gesture, not synthetic velocities fed into a camera.
    const int mid=flight.map.width/2;
    for(int y=17;y<154;++y)for(int x=mid-2;x<=mid+2;++x)
        flight.map.tiles[size_t(y)*flight.map.width+x]=Tile::Empty;
    flight.pilot.x=mid+.5f;flight.pilot.y=148.5f;
    soloUpdateCamera(flight,phone);
    const auto cam=soloViewport(flight,phone);
    const float sx=cam.x(flight.pilot.x),sy=cam.y(flight.pilot.y);
    TouchPilot finger;
    assert(finger.handle(touchEvent(SDL_FINGERDOWN,11,sx/phone.w,sy/phone.h),
            phone.w,phone.h,sx,sy,flight.pilot.x,flight.pilot.y)
            ==TouchAction::None);
    assert(finger.down && !finger.cruiseSet);
    // The real player slides once to the upper outer band and HOLDS STILL.
    assert(finger.handle(touchEvent(SDL_FINGERMOTION,11,sx/phone.w,.06f),
            phone.w,phone.h)==TouchAction::None);
    assert(finger.cruiseSet && finger.cruiseY<-.99f);
    assert(finger.cruiseThrottle>.9f);
    const float initialY=flight.pilot.y, initialTop=flight.viewTop;
    for(int frame=0;frame<100;++frame){
        flight.stepCruise(finger.cruiseX,finger.cruiseY,
                          finger.cruiseThrottle,
                          soloViewport(flight,phone).visibleColumns(),dt);
        soloUpdateCamera(flight,phone,dt);
    }
    const float afterHold=flight.pilot.y;
    const float topAfterHold=flight.viewTop;
    assert(initialY-afterHold>20.0f);
    assert(initialTop-topAfterHold>10.0f);
    assert(finger.down);
    // Finger lifting must NEVER clear the last steering instruction.
    assert(finger.handle(touchEvent(SDL_FINGERUP,11,sx/phone.w,.06f),
            phone.w,phone.h)==TouchAction::None);
    assert(!finger.down && finger.cruiseSet && finger.cruiseY<-.99f);
    const float savedSpeed=finger.cruiseThrottle;
    for(int frame=0;frame<100;++frame){
        flight.stepCruise(finger.cruiseX,finger.cruiseY,
                          finger.cruiseThrottle,
                          soloViewport(flight,phone).visibleColumns(),dt);
        soloUpdateCamera(flight,phone,dt,finger.cameraControlX,
                                        finger.cameraControlY);
    }
    assert(afterHold-flight.pilot.y>20.0f);
    assert(topAfterHold-flight.viewTop>10.0f);
    assert(near(savedSpeed,finger.cruiseThrottle));
    assert(flight.pilot.health>0);

    // Both outer bands progressively respond to SHIP position.
    // A finger held at the centre or released at .06 makes no difference.
    const float values[6]={.31f,.25f,.19f,.145f,.09f,.075f};
    float previous=0;
    for(float v:values){
        const float next=soloRectangularCameraGain(v);
        assert(next>previous);
        previous=next;
    }
    assert(previous<1.0f);
    assert(near(soloRectangularCameraGain(.06f),1.0f));
    assert(near(soloShipCameraTarget(.50f,12.0f),0.0f));
    assert(soloShipCameraTarget(.25f,-12.0f)<0.0f);
    assert(soloShipCameraTarget(.75f,12.0f)>0.0f);
    assert(near(soloShipCameraTarget(.25f,12.0f),0.0f));

    TouchPilot zones;
    const float x=.5f,y=.5f;
    zones.handle(touchEvent(SDL_FINGERDOWN,21,x,y),phone.w,phone.h);
    zones.handle(touchEvent(SDL_FINGERMOTION,21,x,.06f),phone.w,phone.h);
    assert(zones.cruiseSet);
    zones.handle(touchEvent(SDL_FINGERUP,21,x,.06f),phone.w,phone.h);
    assert(zones.cruiseSet);
    // A second steering gesture can reverse direction, replacing the cruise.
    zones.handle(touchEvent(SDL_FINGERDOWN,22,.50f,.50f),
                 phone.w,phone.h);
    zones.handle(touchEvent(SDL_FINGERMOTION,22,.50f,.96f),
                 phone.w,phone.h);
    assert(zones.cruiseY>.99f);
    zones.handle(touchEvent(SDL_FINGERUP,22,.50f,.96f),
                 phone.w,phone.h);
    assert(zones.cruiseY>.99f && zones.cruiseSet);

    // Separate finger shooting never steals cruise or cancels acceleration.
    zones.handle(touchEvent(SDL_FINGERDOWN,23,.50f,.50f),
                 phone.w,phone.h);
    assert(zones.handle(touchEvent(SDL_FINGERDOWN,24,.88f,.12f),
                        phone.w,phone.h)==TouchAction::ChargePressed);
    assert(zones.handle(touchEvent(SDL_FINGERUP,24,.88f,.12f),
                        phone.w,phone.h)==TouchAction::ChargeReleased);
    assert(zones.cruiseSet && zones.cruiseY>.99f);
    zones.handle(touchEvent(SDL_FINGERUP,23,.50f,.50f),
                 phone.w,phone.h);

    // An intentional reset (restart/quit/background) must stop old momentum.
    zones.reset();
    assert(!zones.cruiseSet && near(zones.cruiseThrottle,0));
    Session stopped(generate({}));assert(stopped.start());
    stopped.pilot.x=stopped.map.width*.5f;
    stopped.pilot.y=stopped.map.height*.5f;
    // Build a clear horizontal lane in the ACTUAL maze model to isolate the
    // exact centre-to-zone transition from rock collisions.
    for(int cx=int(stopped.pilot.x)-5;cx<int(stopped.pilot.x)+12;++cx)
        stopped.map.tiles[size_t(int(stopped.pilot.y))*stopped.map.width+cx]
            =Tile::Empty;
    soloUpdateCamera(stopped,phone);
    const auto first=soloViewport(stopped,phone);
    const float fingerX=first.x(stopped.pilot.x);
    const float fingerY=first.y(stopped.pilot.y);
    TouchPilot latch;
    latch.handle(touchEvent(SDL_FINGERDOWN,88,fingerX/phone.w,
                            fingerY/phone.h),phone.w,phone.h,
                 fingerX,fingerY,stopped.pilot.x,stopped.pilot.y);
    latch.handle(touchEvent(SDL_FINGERMOTION,88,.98f,
                            fingerY/phone.h),phone.w,phone.h);
    assert(latch.cruiseSet && latch.cruiseX>.9f);
    latch.handle(touchEvent(SDL_FINGERUP,88,.98f,
                            fingerY/phone.h),phone.w,phone.h);
    assert(!latch.down && latch.cruiseSet);
    const float startLeft=stopped.viewLeft;
    bool sawCentralMovement=false,sawOuterPan=false;
    for(int i=0;i<35;++i){
        const float lastX=stopped.pilot.x;
        const float lastCam=stopped.viewLeft;
        const float pos=(stopped.pilot.x-stopped.viewLeft)/
                         soloViewport(stopped,phone).visibleColumns();
        stopped.stepCruise(latch.cruiseX,latch.cruiseY,latch.cruiseThrottle,
                            soloViewport(stopped,phone).visibleColumns(),dt);
        soloUpdateCamera(stopped,phone,dt);
        if(pos>1.f/3.f && pos<2.f/3.f && stopped.pilot.x>lastX)
            sawCentralMovement=true;
        if(stopped.viewLeft>lastCam+.001f)sawOuterPan=true;
    }
    assert(sawCentralMovement && sawOuterPan);
    assert(stopped.viewLeft>startLeft);
}
