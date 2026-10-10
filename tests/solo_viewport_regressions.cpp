#include "../src/solo_viewport.hpp"
#include "../src/solo_touch_controls.hpp"
#include <cassert>
#include <cmath>
int main(){
    using namespace sfsolo;
    Session s(generate({}));
    assert(s.start());
    // Actual Samsung-like portrait: see a local 18-cell playfield, not all 54.
    const SDL_Rect phone{0,0,709,1536};
    const auto cam=soloViewport(s,phone);
    assert(std::abs(cam.visibleColumns()-18.0f)<.01f);
    assert(cam.cell>=35.0f);
    assert(cam.left>0 && cam.left<25);
    const float shipScreenX=cam.x(s.pilot.x);
    const float shipScreenY=cam.y(s.pilot.y);
    assert(shipScreenX>phone.w*.35f && shipScreenX<phone.w*.65f);
    // Regression: start ship used to be anchored at the Android nav bar.
    assert(shipScreenY>phone.h*.35f && shipScreenY<phone.h*.82f);
    // Common mapping for arrows, missiles, charge and pointer movement.
    assert(std::abs(cam.x(s.pilot.x+2)-shipScreenX-2*cam.cell)<.01f);
    assert(std::abs(cam.y(s.pilot.y-3)-shipScreenY+3*cam.cell)<.01f);
    TouchPilot pilot;
    SDL_Event e{};
    e.type=SDL_FINGERDOWN;e.tfinger.type=SDL_FINGERDOWN;
    e.tfinger.fingerId=9;
    e.tfinger.x=shipScreenX/phone.w;
    e.tfinger.y=shipScreenY/phone.h;
    pilot.handle(e,phone.w,phone.h);
    float ax=0,ay=0;
    pilotInput(s,pilot,phone.w,phone.h,ax,ay);
    assert(std::abs(ax)<.01f && std::abs(ay)<.01f);
    // A finger moved to the right/up of the visible ship uses that same view.
    e.type=SDL_FINGERMOTION;e.tfinger.type=SDL_FINGERMOTION;
    e.tfinger.x=(shipScreenX+100)/phone.w;
    e.tfinger.y=(shipScreenY-100)/phone.h;
    pilot.handle(e,phone.w,phone.h);
    pilotInput(s,pilot,phone.w,phone.h,ax,ay);
    assert(ax>0 && ay<0);
    s.pilot.y=9.5f;s.cameraY=s.pilot.y; // boss/top-of-map edge
    const auto boss=soloViewport(s,phone);
    assert(boss.y(s.pilot.y)>0 && boss.y(s.pilot.y)<phone.h);
    s.pilot.x=1.5f;
    assert(soloViewport(s,phone).left==0);
    // The world scrolls with the ship. Faster SOLO ship must also advance the
    // camera significantly faster, without running past the screen safe area.
    Session fast(generate({})),slow(generate({}));
    assert(fast.start() && slow.start());
    fast.pilot.y=slow.pilot.y=80.5f;
    fast.cameraY=slow.cameraY=80.5f;
    slow.physics.thrust=5.0f;slow.physics.maxSpeed=7.0f;
    const float initialTop=soloViewport(fast,phone).top;
    for(int n=0;n<12;++n){fast.step(0,-1,.05f);slow.step(0,-1,.05f);}
    const float fastScroll=std::abs(soloViewport(fast,phone).top-initialTop);
    const float slowScroll=std::abs(soloViewport(slow,phone).top-initialTop);
    assert(slowScroll>0 && fastScroll>=3.0f*slowScroll);
    assert(soloViewport(fast,phone).y(fast.pilot.y)>phone.h*.35f);
    assert(soloViewport(fast,phone).y(fast.pilot.y)<phone.h*.87f);
    // A frozen ship needs four times as much thumb displacement for the same
    // input value, with the physical thrust and speed limited to one quarter.
    const auto view=soloViewport(fast,phone);
    TouchPilot control;control.down=true;
    control.targetX=view.x(fast.pilot.x)+20;
    control.targetY=view.y(fast.pilot.y);
    float normalX=0,normalY=0,iceX=0,iceY=0;
    pilotInput(fast,control,phone.w,phone.h,normalX,normalY);
    fast.iceSeconds=2.0f;
    pilotInput(fast,control,phone.w,phone.h,iceX,iceY);
    assert(normalX>0 && iceX>0);
    assert(std::abs(normalX/iceX-4.0f)<.02f);
}
