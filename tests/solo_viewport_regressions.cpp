#include "../src/solo_viewport.hpp"
#include "../src/solo_touch_controls.hpp"
#include <cassert>
#include <cmath>
#include <algorithm>

static bool close(float a,float b,float tolerance=.001f){
    return std::abs(a-b)<tolerance;
}
int main(){
    using namespace sfsolo;
    const SDL_Rect phone{0,0,709,1536};
    Session s(generate({}));assert(s.start());
    soloUpdateCamera(s,phone); // prepare, no motion/no elapsed time
    const auto initial=soloViewport(s,phone);
    assert(SOLO_MAP_ZOOM==3.0f);
    assert(close(initial.visibleColumns(),6.0f));
    assert(close(initial.cell*6.0f,phone.w));
    assert(soloShipRadiusPixels(phone)==
           std::max(17,int((phone.w/18.0f)*.90f)));
    assert(initial.x(s.pilot.x)>phone.w/3.f);
    assert(initial.x(s.pilot.x)<phone.w*2.f/3.f);
    assert(initial.y(s.pilot.y)>phone.h/3.f);
    assert(initial.y(s.pilot.y)<phone.h*2.f/3.f);
    assert(soloShipRadiusPixels(phone)*2<initial.cell*.75f);

    // Central rectangle 1/3..2/3: no scroll, even if the ship moves.
    s.pilot.x+=.5f;s.pilot.y-=.5f;
    s.pilot.vx=8.f;s.pilot.vy=-8.f;
    soloUpdateCamera(s,phone,.05f);
    assert(close(s.viewLeft,initial.left));
    assert(close(s.viewTop,initial.top));
    assert(close(s.viewVelocityX,0));
    assert(close(s.viewVelocityY,0));

    // Two DIFFERENT progressive rectangular zones. Each band has several
    // distinct speeds, with no discontinuity at the middle boundary.
    assert(close(soloRectangularCameraGain(.5f),0));
    assert(close(soloRectangularCameraGain(1.f/3.f),0));
    const float z11=soloRectangularCameraGain(.31f);
    const float z12=soloRectangularCameraGain(.25f);
    const float z13=soloRectangularCameraGain(.19f);
    assert(0<z11 && z11<z12 && z12<z13);
    assert(close(soloRectangularCameraGain(1.f/6.f),
                 SOLO_RECT_ZONE1_END_SPEED));
    const float z21=soloRectangularCameraGain(.145f);
    const float z22=soloRectangularCameraGain(.09f);
    const float z23=soloRectangularCameraGain(.075f);
    assert(SOLO_RECT_ZONE1_END_SPEED<z21 && z21<z22 && z22<z23);
    assert(z23<1.0f);
    // Full cruise speed is reached BEFORE the fixed-size ship is clipped.
    assert(close(soloRectangularCameraGain(.06f),1));
    assert(close(soloRectangularCameraGain(.04f),1));
    assert(close(soloRectangularCameraGain(0),1));
    assert(close(soloRectangularCameraGain(1),1));
    assert(close(soloRectangularCameraGain(.25f),
                 soloRectangularCameraGain(.75f)));
    assert(close(soloRectangularCameraGain(.145f),
                 soloRectangularCameraGain(.855f)));
    assert(std::abs(soloRectangularCameraGain(1.f/6.f-.0001f)-
                    soloRectangularCameraGain(1.f/6.f+.0001f))<.001f);
    assert(std::abs(soloRectangularCameraGain(1.f/3.f-.0001f)-
                    soloRectangularCameraGain(1.f/3.f+.0001f))<.001f);

    // Right/top crossing starts an actual non-zero but NOT instant-max pan.
    const float oldX=s.viewLeft,oldY=s.viewTop;
    s.pilot.x+=1.5f;s.pilot.y-=2.0f;
    s.pilot.vx=8.f;s.pilot.vy=-8.f;
    soloUpdateCamera(s,phone,.05f);
    assert(s.viewLeft>oldX && s.viewTop<oldY);
    assert(s.viewVelocityX>0 && s.viewVelocityX<s.pilot.vx);
    assert(s.viewVelocityY<0 && s.viewVelocityY>s.pilot.vy);

    // Holding direction at outer screen edge keeps world scrolling every
    // frame, while the ship can remain nearly fixed relative to the view.
    Session cruiser(generate({}));assert(cruiser.start());
    cruiser.pilot.x=cruiser.map.width*.5f;
    cruiser.pilot.y=cruiser.map.height*.5f;
    soloUpdateCamera(cruiser,phone);
    const float cols=soloViewport(cruiser,phone).visibleColumns();
    cruiser.pilot.x=cruiser.viewLeft+cols*.80f;
    cruiser.pilot.vx=9.f;cruiser.pilot.vy=0;
    soloUpdateCamera(cruiser,phone,.05f);
    const float early=cruiser.viewVelocityX;
    for(int i=0;i<35;++i){
        cruiser.pilot.x+=cruiser.pilot.vx*.05f;
        const float before=cruiser.viewLeft;
        soloUpdateCamera(cruiser,phone,.05f);
        assert(cruiser.viewLeft>before);
        assert(cruiser.pilot.x-cruiser.viewLeft<cols);
    }
    assert(cruiser.viewVelocityX>early);
    assert(cruiser.viewVelocityX<=cruiser.pilot.vx+.001f);
    const float beforeStop=cruiser.viewLeft;
    cruiser.pilot.vx=0;
    for(int i=0;i<90;++i)soloUpdateCamera(cruiser,phone,.05f);
    assert(close(cruiser.viewVelocityX,0));
    assert(cruiser.viewLeft-beforeStop<2.0f); // inertia finite, not forever

    // When the pilot turns inwards the camera eases out of its former speed.
    Session reversing(generate({}));assert(reversing.start());
    reversing.pilot.x=reversing.map.width*.5f;
    reversing.pilot.y=reversing.map.height*.5f;
    soloUpdateCamera(reversing,phone);
    reversing.pilot.x=reversing.viewLeft+cols*.86f;
    reversing.pilot.vx=8.f;
    for(int i=0;i<12;++i)soloUpdateCamera(reversing,phone,.05f);
    const float outwardSpeed=reversing.viewVelocityX;
    assert(outwardSpeed>0);
    reversing.pilot.vx=-8.f;reversing.pilot.x-=.12f;
    soloUpdateCamera(reversing,phone,.05f);
    assert(reversing.viewVelocityX<outwardSpeed);
    for(int i=0;i<80;++i)soloUpdateCamera(reversing,phone,.05f);
    assert(close(reversing.viewVelocityX,0));

    // Each axis independently scrolls at its own gradual speed: corner
    // diagonals are the composition of two rectangular camera zones.
    Session diagonal(generate({}));assert(diagonal.start());
    diagonal.pilot.x=diagonal.map.width*.5f;
    diagonal.pilot.y=diagonal.map.height*.5f;
    soloUpdateCamera(diagonal,phone);
    const auto centred=soloViewport(diagonal,phone);
    diagonal.pilot.x=centred.left+centred.visibleColumns()*.91f;
    diagonal.pilot.y=centred.top+centred.visibleRows()*.25f;
    diagonal.pilot.vx=7.f;diagonal.pilot.vy=-5.f;
    soloUpdateCamera(diagonal,phone,.05f);
    assert(diagonal.viewVelocityX>0 && diagonal.viewVelocityY<0);
    assert(std::abs(diagonal.viewVelocityX)>std::abs(diagonal.viewVelocityY));
    // Independently stop vertical panning while horizontal keeps moving.
    const float topBefore=diagonal.viewTop;
    diagonal.pilot.y=diagonal.viewTop+centred.visibleRows()*.5f;
    diagonal.pilot.vy=0;
    for(int i=0;i<80;++i)soloUpdateCamera(diagonal,phone,.05f);
    assert(close(diagonal.viewVelocityY,0));
    assert(diagonal.viewVelocityX>0);
    assert(diagonal.viewTop<topBefore);

    // Level boundaries must not hide the ship or exceed the world.
    for(auto point:std::array<std::array<float,2>,4>{{
           { .5f,2.5f},{.5f,157.5f},{53.5f,2.5f},{53.5f,157.5f}}}){
        s.pilot.x=point[0];s.pilot.y=point[1];
        s.pilot.vx=point[0]>27?-15:15;
        s.pilot.vy=point[1]>80?-15:15;
        soloUpdateCamera(s,phone,.05f);
        const auto cam=soloViewport(s,phone);
        assert(cam.x(s.pilot.x)>=0 && cam.x(s.pilot.x)<=phone.w);
        assert(cam.y(s.pilot.y)>=0 && cam.y(s.pilot.y)<=phone.h);
    }

    // Zoom multiplies the terrain size but never the graphical ship size or
    // apparent steering speed when the world has not begun to scroll.
    Physics motion;
    Pilot original{},zoomed{};
    motion.stepToward(original,50,0,.05f,1.0f,18.0f);
    motion.stepToward(zoomed,50,0,.05f,1.0f,6.0f);
    assert(close(original.x*(phone.w/18.0f),
                 zoomed.x*(phone.w/6.0f)));
    assert(original.x>zoomed.x && zoomed.x>0);

    // Resizing always resets momentum and centres the new viewport.
    Session restart(generate({}));assert(restart.start());
    restart.pilot.y=80;
    soloUpdateCamera(restart,phone);
    restart.pilot.x=restart.viewLeft+cols*.90f;
    restart.pilot.vx=10;
    soloUpdateCamera(restart,phone,.05f);
    assert(restart.viewVelocityX>0);
    const SDL_Rect landscape{0,0,1536,709};
    soloUpdateCamera(restart,landscape);
    assert(restart.viewWidth==landscape.w && restart.viewHeight==landscape.h);
    assert(close(restart.viewVelocityX,0));
    const auto rotated=soloViewport(restart,landscape);
    assert(rotated.x(restart.pilot.x)>=0 && rotated.x(restart.pilot.x)<=landscape.w);
    assert(rotated.y(restart.pilot.y)>=0 && rotated.y(restart.pilot.y)<=landscape.h);
    return 0;
}
