#include "solo_fire_pad.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
int main(){
    sfsolo::FirePad p;const int w=540,h=960;
    const float x=p.cx(w),y=p.cy(h);
    assert(p.handle(SDL_FINGERDOWN,1,x,y,w,h).owned);
    auto a=p.handle(SDL_FINGERUP,1,x,y,w,h);assert(a.fire&&p.dy==-1);
    p.handle(SDL_FINGERDOWN,2,x,y,w,h);
    p.handle(SDL_FINGERMOTION,2,x+70,y,w,h);
    a=p.handle(SDL_FINGERUP,2,x+70,y,w,h);assert(a.fire&&p.dx>.99f);
    p.handle(SDL_FINGERDOWN,3,x,y,w,h);
    a=p.handle(SDL_FINGERUP,3,x,y,w,h);assert(a.fire&&p.dx>.99f);
    for(int i=0;i<8;++i){
        const float phi=i*sfsolo::FirePad::padPi*.25f;
        a=p.handle(SDL_FINGERDOWN,100+i,x+std::cos(phi)*p.orbit(w),
            y+std::sin(phi)*p.orbit(w),w,h);
        assert(a.fire&&a.owned);assert(std::abs(p.dx-std::cos(phi))<.001f);
        assert(std::abs(p.dy-std::sin(phi))<.001f);
        p.handle(SDL_FINGERUP,100+i,x,y,w,h);
    }
    p.right=true;assert(p.cx(w)>w/2.f);
    puts("PASS 8 direction buttons, aim memory, release and mirrored pad");
}