#pragma once
#include <algorithm>
#include <cmath>
namespace sfsolo {
struct RockBody {float x=0,y=0,vx=0,vy=0,radius=1;};
// Earliest swept circle contact. A resting/separating overlap never generates
// another impact, while a complete high-speed crossing cannot be missed.
inline float rockSweep(float ax,float ay,float bx,float by,float cx,float cy,float radius){
    const float dx=bx-ax,dy=by-ay,px=ax-cx,py=ay-cy;
    const float aa=dx*dx+dy*dy,bb=px*dx+py*dy,cc=px*px+py*py-radius*radius;
    if(aa<1e-8f || bb>=0)return 1;
    if(cc<=0)return 0;
    const float disc=bb*bb-aa*cc;if(disc<0)return 1;
    return std::clamp((-bb-std::sqrt(disc))/aa,0.f,1.f);
}
inline float rockContact(RockBody &a,RockBody &b){
    if(a.vx==0 && a.vy==0 && b.vx==0 && b.vy==0)return 0;
    const float dx=b.x-a.x,dy=b.y-a.y,d=std::hypot(dx,dy);
    if(d>a.radius+b.radius+.05f)return 0;
    const float nx=d>1e-5f?dx/d:1.f,ny=d>1e-5f?dy/d:0.f;
    const float closing=(a.vx-b.vx)*nx+(a.vy-b.vy)*ny;
    if(closing<=.001f)return 0;
    const float ma=std::max(1.f,a.radius*a.radius),mb=std::max(1.f,b.radius*b.radius);
    const float impulse=1.35f*closing/(1.f/ma+1.f/mb);
    a.vx-=impulse*nx/ma;a.vy-=impulse*ny/ma;
    b.vx+=impulse*nx/mb;b.vy+=impulse*ny/mb;
    return .5f*(ma*mb/(ma+mb))*closing*closing;
}
inline void rockDamp(RockBody &a,float dt,float sleepSpeed){
    const float damping=std::exp(-.85f*dt);a.vx*=damping;a.vy*=damping;
    if(std::hypot(a.vx,a.vy)<sleepSpeed)a.vx=a.vy=0;
}
}
