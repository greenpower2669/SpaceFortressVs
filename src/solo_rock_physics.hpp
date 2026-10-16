#pragma once
#include <algorithm>
#include <cmath>
namespace sfsolo {
struct RockBody {float x=0,y=0,vx=0,vy=0,radius=1;};
inline float rockContact(RockBody &a,RockBody &b){
    if(a.vx==0 && a.vy==0 && b.vx==0 && b.vy==0)return 0;
    const float dx=b.x-a.x,dy=b.y-a.y,d=std::hypot(dx,dy);
    if(d>a.radius+b.radius)return 0;
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
