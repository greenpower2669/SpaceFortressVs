#pragma once
#include <algorithm>
#include <cmath>
namespace sfsolo {
// World-space motion only. Width is the historical combat reference, never
// the zoomed viewport or map width. Touch is a force intention, not a clamp.
struct ShipMotion {
    float x=0,y=0,vx=0,vy=0;
    void step(float targetX,float targetY,bool held,float width,int danger,float dt){
        if(!std::isfinite(dt)||dt<=0||width<=0)return;
        const float difficulty=std::clamp(danger,0,8)/8.f;
        const float maxSpeed=width*(1.7f+1.4f*difficulty);
        const int steps=std::max(1,int(std::ceil(dt*240.f)));
        const float h=dt/steps;
        for(int i=0;i<steps;++i){
            const float speed=std::hypot(vx,vy);
            const float momentum=std::clamp(speed/maxSpeed,0.f,1.f);
            const float mass=momentum*momentum;
            float desiredX=0,desiredY=0,rate;
            if(held){
                const float dx=targetX-x,dy=targetY-y,d=std::hypot(dx,dy);
                const float desiredSpeed=std::min(maxSpeed,d*12.f);
                if(d>1e-5f){desiredX=dx/d*desiredSpeed;desiredY=dy/d*desiredSpeed;}
                rate=1.f/(.055f+mass*(.13f+.70f*difficulty));
            }else rate=6.f*(1.f-mass)+(3.5f-2.9f*difficulty)*mass;
            const float blend=-std::expm1(-rate*h);
            float dvx=(desiredX-vx)*blend,dvy=(desiredY-vy)*blend;
            const float change=std::hypot(dvx,dvy);
            const float maxChange=width*(6.f-2.f*difficulty)*h;
            if(change>maxChange){dvx*=maxChange/change;dvy*=maxChange/change;}
            x+=(vx+dvx*.5f)*h;y+=(vy+dvy*.5f)*h;
            vx+=dvx;vy+=dvy;
            if(!held && std::hypot(vx,vy)<width*.0002f)vx=vy=0;
        }
    }
};
}
