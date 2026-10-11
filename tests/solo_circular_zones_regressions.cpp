#include "../src/solo_viewport.hpp"
#include "../src/solo_navigation.hpp"
#include <cassert>
#include <cmath>
int main(){
    using namespace sfsolo;
    const SDL_Rect phone{0,0,709,1536};
    const float cx=phone.w*.5f,cy=phone.h*.5f;
    const float outer=soloCircleOuterRadius(phone);
    assert(std::abs(outer-297.78f)<.1f);
    const float inner=outer*.5f,mid=outer*.75f;
    assert(soloCircleZoneSpeed(cx,cy,phone)==0);
    assert(soloCircleZoneSpeed(cx+inner,cy,phone)==0);
    assert(soloCircleZoneSpeed(cx+inner+2,cy,phone)>.1f);
    assert(std::abs(soloCircleZoneSpeed(cx+mid,cy,phone)-2.0f)<.001f);
    assert(soloCircleZoneSpeed(cx+mid+2,cy,phone)>2.1f);
    assert(std::abs(soloCircleZoneSpeed(cx+outer,cy,phone)-6.0f)<.001f);
    // Once past the outer circle, the speed never changes again.
    assert(soloCircleZoneSpeed(cx,20,phone)==6);
    assert(soloCircleZoneSpeed(cx,2,phone)==6);
    assert(soloCircleZoneSpeed(2,2,phone)==6);
    assert(soloCircleZoneSpeed(cx+500,cy,phone)==6);
    const float diag=outer*.67f/std::sqrt(2.0f);
    const float v=soloCircleZoneSpeed(cx+diag,cy+diag,phone);
    assert(v>0&&v<6);
    assert(std::abs(v-soloCircleZoneSpeed(cx+outer*.67f,cy,phone))<.0001f);
    auto center=soloCirclePan(cx,cy,phone,6);
    assert(center.vx==0&&center.vy==0);
    auto right=soloCirclePan(cx+outer,cy,phone,6);
    auto left=soloCirclePan(cx-outer,cy,phone,6);
    auto up=soloCirclePan(cx,cy-outer,phone,6);
    auto diagonal=soloCirclePan(cx+outer,cy+outer,phone,6);
    assert(right.vx>0&&right.vy==0&&left.vx<0&&up.vy<0);
    assert(right.speed==6&&diagonal.speed==6);
    assert(diagonal.vx>0&&diagonal.vy>0);
    assert(std::abs(std::hypot(diagonal.vx,diagonal.vy)-
                    std::hypot(right.vx,right.vy))<.0001f);
    assert(soloCircleZoneSpeed(cx,cy,SDL_Rect{0,0,0,0})==0);
}
