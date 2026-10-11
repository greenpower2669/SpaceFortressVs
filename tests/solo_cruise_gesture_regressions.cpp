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
    assert(near(finger.cameraControlY,.06f));
    const float initialY=flight.pilot.y, initialTop=flight.viewTop;
    for(int frame=0;frame<100;++frame){
        flight.stepCruise(finger.cruiseX,finger.cruiseY,
                          finger.cruiseThrottle,
                          soloViewport(flight,phone).visibleColumns(),dt);
        soloUpdateCamera(flight,phone,dt,finger.cameraControlX,
                                        finger.cameraControlY);
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
    const float savedControlY=finger.cameraControlY;
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
    assert(near(savedControlY,finger.cameraControlY));
    assert(near(savedSpeed,finger.cruiseThrottle));
    assert(flight.pilot.health>0);

    // Both rectangular bands are accelerated by FINGER position, not ship
    // position. Sample 3 distinct values WITHIN each progressive band.
    TouchPilot zones;
    const float x=.5f,y=.5f;
    zones.handle(touchEvent(SDL_FINGERDOWN,21,x,y),
                 phone.w,phone.h);
    const float values[6]={.31f,.25f,.19f,.145f,.09f,.075f};
    float previous=0;
    for(float v:values){
        zones.handle(touchEvent(SDL_FINGERMOTION,21,x,v),phone.w,phone.h);
        assert(zones.cruiseSet);
        assert(near(zones.cameraControlY,v));
        const float next=soloRectangularCameraGain(zones.cameraControlY);
        assert(next>previous);
        previous=next;
    }
    assert(previous<1.0f);
    zones.handle(touchEvent(SDL_FINGERMOTION,21,x,.06f),phone.w,phone.h);
    assert(near(soloRectangularCameraGain(zones.cameraControlY),1));
    zones.handle(touchEvent(SDL_FINGERUP,21,x,.06f),phone.w,phone.h);
    assert(zones.cruiseSet);
    assert(near(zones.cameraControlY,.06f));
    // A second steering gesture can reverse direction, replacing the cruise.
    zones.handle(touchEvent(SDL_FINGERDOWN,22,.50f,.50f),
                 phone.w,phone.h);
    zones.handle(touchEvent(SDL_FINGERMOTION,22,.50f,.96f),
                 phone.w,phone.h);
    assert(zones.cruiseY>.99f && zones.cameraControlY>.95f);
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
    assert(near(zones.cameraControlX,.5f));
    assert(near(zones.cameraControlY,.5f));
}
