#include "../src/solo_navigation.hpp"
#include <SDL2/SDL.h>
#include <cassert>
#include <cmath>
static SDL_Event evt(Uint32 type,SDL_FingerID id,float x,float y){
    SDL_Event e{};e.type=type;e.tfinger.type=type;e.tfinger.fingerId=id;
    e.tfinger.x=x;e.tfinger.y=y;return e;
}
static bool near(float a,float b,float tol=.005f){return std::abs(a-b)<=tol;}
static void clearLane(sfsolo::Session &s,int cy){
    for(int y=cy-4;y<=cy+4;++y)
        for(int x=3;x<s.map.width-3;++x)
            s.map.tiles[size_t(y)*s.map.width+x]=sfsolo::Tile::Empty;
}
int main(){
    using namespace sfsolo;
    const SDL_Rect phone{0,0,709,1536};
    constexpr float dt=.025f;
    assert(near(soloFingerZonePanSpeed(.5f,6),0));
    assert(soloFingerZonePanSpeed(.2f,6)<0);
    assert(soloFingerZonePanSpeed(.8f,6)>0);
    assert(std::abs(soloFingerZonePanSpeed(.08f,6))>
           std::abs(soloFingerZonePanSpeed(.25f,6)));

    Session flight(generate({}));assert(flight.start());
    flight.pilot.x=27.5f;flight.pilot.y=80.5f;
    clearLane(flight,80);soloUpdateCamera(flight,phone);
    auto cam=soloViewport(flight,phone);
    TouchPilot finger;
    const float sx=cam.x(flight.pilot.x),sy=cam.y(flight.pilot.y);
    finger.handle(evt(SDL_FINGERDOWN,1,sx/phone.w,sy/phone.h),
                  phone.w,phone.h,sx,sy,flight.pilot.x,flight.pilot.y);
    assert(finger.down&&finger.steeringEngaged);
    finger.handle(evt(SDL_FINGERMOTION,1,.80f,sy/phone.h),phone.w,phone.h);
    bool scrolled=false;
    for(int i=0;i<42;++i){
        const float before=flight.viewLeft;
        soloNavigateFinger(flight,finger,phone,dt);
        if(flight.viewLeft>before+.001f)scrolled=true;
    }
    assert(scrolled);
    cam=soloViewport(flight,phone);
    assert(std::abs(cam.x(flight.pilot.x)/phone.w-.80f)<.025f);
    const float heldStart=flight.viewLeft;
    for(int i=0;i<12;++i){
        const float old=flight.viewLeft;
        soloNavigateFinger(flight,finger,phone,dt);
        assert(flight.viewLeft>old+.001f); // continuous, no finger motion
    }
    assert(flight.viewLeft>heldStart+.8f);
    finger.handle(evt(SDL_FINGERUP,1,.80f,sy/phone.h),phone.w,phone.h);
    assert(!finger.down&&finger.steeringEngaged);
    const float lifted=flight.viewLeft;
    for(int i=0;i<12;++i)soloNavigateFinger(flight,finger,phone,dt);
    assert(flight.viewLeft>lifted+.8f);
    cam=soloViewport(flight,phone);
    assert(std::abs(cam.x(flight.pilot.x)/phone.w-.80f)<.025f);
    const float start2=cam.x(flight.pilot.x),y2=cam.y(flight.pilot.y);
    finger.handle(evt(SDL_FINGERDOWN,2,start2/phone.w,y2/phone.h),
                  phone.w,phone.h,start2,y2,flight.pilot.x,flight.pilot.y);
    finger.handle(evt(SDL_FINGERMOTION,2,.5f,y2/phone.h),phone.w,phone.h);
    for(int i=0;i<25;++i)soloNavigateFinger(flight,finger,phone,dt);
    cam=soloViewport(flight,phone);
    assert(std::abs(cam.x(flight.pilot.x)/phone.w-.5f)<.025f);
    const float stopped=flight.viewLeft;
    for(int i=0;i<15;++i)soloNavigateFinger(flight,finger,phone,dt);
    assert(near(flight.viewLeft,stopped));

    // A solid wall blocks both physical translation and induced camera pan.
    Session wall(generate({}));assert(wall.start());
    wall.pilot.x=27.5f;wall.pilot.y=80.5f;clearLane(wall,80);
    for(int y=76;y<=84;++y)
        wall.map.tiles[size_t(y)*wall.map.width+30]=Tile::Rock;
    soloUpdateCamera(wall,phone);
    auto wc=soloViewport(wall,phone);
    TouchPilot blocked;
    const float px=wc.x(wall.pilot.x),py=wc.y(wall.pilot.y);
    blocked.handle(evt(SDL_FINGERDOWN,3,px/phone.w,py/phone.h),
                   phone.w,phone.h,px,py,wall.pilot.x,wall.pilot.y);
    blocked.handle(evt(SDL_FINGERMOTION,3,.90f,py/phone.h),phone.w,phone.h);
    for(int i=0;i<58;++i)soloNavigateFinger(wall,blocked,phone,dt);
    assert(wall.pilot.x<30.0f);
    const float pinned=wall.viewLeft;
    for(int i=0;i<10;++i)soloNavigateFinger(wall,blocked,phone,dt);
    assert(near(wall.viewLeft,pinned));

    // Upward zone works independently of X and scrolls with stationary touch.
    Session up(generate({}));assert(up.start());up.pilot.x=27.5f;up.pilot.y=80.5f;
    for(int y=15;y<145;++y)for(int x=25;x<=29;++x)
        up.map.tiles[size_t(y)*up.map.width+x]=Tile::Empty;
    soloUpdateCamera(up,phone);
    auto uc=soloViewport(up,phone);TouchPilot vertical;
    const float ux=uc.x(up.pilot.x),uy=uc.y(up.pilot.y);
    vertical.handle(evt(SDL_FINGERDOWN,4,ux/phone.w,uy/phone.h),
                    phone.w,phone.h,ux,uy,up.pilot.x,up.pilot.y);
    vertical.handle(evt(SDL_FINGERMOTION,4,ux/phone.w,.10f),
                    phone.w,phone.h);
    const float top=up.viewTop;
    for(int i=0;i<38;++i)soloNavigateFinger(up,vertical,phone,dt);
    assert(up.viewTop<top-.5f);
    assert(std::abs(soloViewport(up,phone).y(up.pilot.y)/phone.h-.10f)<.025f);
    assert(std::abs(up.viewLeft-uc.left)<.01f);
    vertical.reset();assert(!vertical.steeringEngaged);
}
