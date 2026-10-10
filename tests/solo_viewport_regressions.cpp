#include "../src/solo_viewport.hpp"
#include "../src/solo_touch_controls.hpp"
#include <cassert>
#include <cmath>

// Android-portrait regression: terrain x3, unchanged ship pixel size,
// camera follows only beyond one third on each axis, input is camera-proof.
int main(){
    using namespace sfsolo;
    const SDL_Rect phone{0,0,709,1536};
    Session s(generate({}));assert(s.start());
    assert(SOLO_MAP_ZOOM==3.0f);
    soloUpdateCamera(s,phone);
    auto camera=soloViewport(s,phone);
    assert(std::abs(camera.visibleColumns()-6.0f)<.001f);
    assert(std::abs(camera.cell*6.0f-phone.w)<.001f);
    assert(soloShipRadiusPixels(phone)==std::max(
        17,int((phone.w/18.0f)*.90f)));
    const float shipX=camera.x(s.pilot.x),shipY=camera.y(s.pilot.y);
    assert(shipX>phone.w/3.f && shipX<phone.w*2.f/3.f);
    assert(shipY>phone.h/3.f && shipY<phone.h*2.f/3.f);
    // The ship's original graphic radius is intentionally NOT multiplied x3.
    assert(soloShipRadiusPixels(phone)*2 < camera.cell*.75f);

    // Move less than one tile: the world must stand absolutely still.
    const float stillX=camera.left,stillY=camera.top;
    s.pilot.x+=.5f;s.pilot.y-=.5f;
    soloUpdateCamera(s,phone);
    camera=soloViewport(s,phone);
    assert(std::abs(camera.left-stillX)<.0001f);
    assert(std::abs(camera.top-stillY)<.0001f);

    // Enter the outer right/top third: terrain pans on the crossed axis.
    s.pilot.x+=1.5f;s.pilot.y-=2.0f;
    soloUpdateCamera(s,phone);
    camera=soloViewport(s,phone);
    assert(camera.left>stillX && camera.top<stillY);
    assert(std::abs(camera.x(s.pilot.x)-phone.w*2.f/3.f)<1.0f);
    assert(std::abs(camera.y(s.pilot.y)-phone.h/3.f)<1.0f);

    // Reverse a little: world STAYS FIXED until the other third is reached.
    const float afterRight=camera.left,afterTop=camera.top;
    s.pilot.x-=.7f;s.pilot.y+=.6f;
    soloUpdateCamera(s,phone);
    assert(std::abs(s.viewLeft-afterRight)<.0001f);
    assert(std::abs(s.viewTop-afterTop)<.0001f);

    // Further reversing triggers left/bottom scroll, including backtracking.
    s.pilot.x-=3.0f;s.pilot.y+=5.0f;
    soloUpdateCamera(s,phone);
    assert(s.viewLeft<afterRight && s.viewTop>afterTop);
    camera=soloViewport(s,phone);
    assert(std::abs(camera.x(s.pilot.x)-phone.w/3.f)<1.0f);
    assert(std::abs(camera.y(s.pilot.y)-phone.h*2.f/3.f)<1.0f);

    // The edge of the world cannot teleport or hide the visible pilot.
    s.pilot.x=.5f;s.pilot.y=2.5f;
    soloUpdateCamera(s,phone);
    camera=soloViewport(s,phone);
    assert(camera.x(s.pilot.x)>0 && camera.x(s.pilot.x)<phone.w);
    assert(camera.y(s.pilot.y)>0 && camera.y(s.pilot.y)<phone.h);
    s.pilot.x=s.map.width-.5f;s.pilot.y=s.map.height-2.5f;
    soloUpdateCamera(s,phone);
    camera=soloViewport(s,phone);
    assert(camera.x(s.pilot.x)>0 && camera.x(s.pilot.x)<phone.w);
    assert(camera.y(s.pilot.y)>0 && camera.y(s.pilot.y)<phone.h);

    // The canonical touch vector has the same SCREEN-PIXEL speed with
    // either scale. Never multiply ship motion just because terrain zooms.
    Physics motion;
    Pilot normal,zoomed;
    motion.stepToward(normal,50,0,.05f,1.0f,18.0f);
    motion.stepToward(zoomed,50,0,.05f,1.0f,6.0f);
    const float oldPixels=normal.x*(phone.w/18.0f);
    const float newPixels=zoomed.x*(phone.w/6.0f);
    assert(std::abs(oldPixels-newPixels)<.001f);
    assert(normal.x>zoomed.x && zoomed.x>0);

    // Reinitializing a level or resizing a phone makes a fresh centered view.
    Session restart(generate({}));assert(restart.start());
    soloUpdateCamera(restart,phone);
    assert(restart.viewReady);
    restart.pilot.x+=2.f;
    soloUpdateCamera(restart,phone);
    const SDL_Rect landscape{0,0,1536,709};
    soloUpdateCamera(restart,landscape);
    assert(restart.viewWidth==landscape.w && restart.viewHeight==landscape.h);
    assert(std::abs(soloViewport(restart,landscape).visibleColumns()-6.f)<.001f);
    const auto rotated=soloViewport(restart,landscape);
    assert(rotated.x(restart.pilot.x)>0 && rotated.x(restart.pilot.x)<landscape.w);
    assert(rotated.y(restart.pilot.y)>0 && rotated.y(restart.pilot.y)<landscape.h);
}
