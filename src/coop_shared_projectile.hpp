#pragma once
// SOLO-COOP-SOCLE-001: the REAL historical predictive aim and missile turning
// equations, extracted from tactical_runtime.hpp and campaign_runtime.hpp.
// Numeric 2-D PODs keep FabLib definitions in the SINGLE historical TU:
// its tupl/tuplv API remains the original wrapper. No duplicate engine.
#include <algorithm>
#include <cmath>

struct SfCoopPoint2 {float x=0,y=0;};
struct SfCoopGuidance {float phase=0,vx=0,vy=0,speed=0;};

inline SfCoopPoint2 sfCoopPredictInterceptCore(
    SfCoopPoint2 origin,SfCoopPoint2 target,SfCoopPoint2 velocity,
    float speed,float horizon=.8f){
    const float dx=target.x-origin.x,dy=target.y-origin.y;
    const float a=velocity.x*velocity.x+velocity.y*velocity.y-speed*speed;
    const float b=2*(dx*velocity.x+dy*velocity.y),c=dx*dx+dy*dy;
    float time=std::hypot(dx,dy)/std::max(1.0f,speed);
    if(std::abs(a)<.001f){
        if(b<-.001f)time=-c/b;
    }else{
        const float discriminant=b*b-4*a*c;
        if(discriminant>=0){
            const float root=std::sqrt(discriminant);
            const float t1=(-b-root)/(2*a),t2=(-b+root)/(2*a);
            if(t1>0&&t2>0)time=std::min(t1,t2);
            else if(t1>0)time=t1;
            else if(t2>0)time=t2;
        }
    }
    time=std::clamp(time,0.0f,horizon);
    return {target.x+velocity.x*time,target.y+velocity.y*time};
}
inline SfCoopGuidance sfCoopGuidedMissileCore(
    SfCoopPoint2 projectile,SfCoopPoint2 target,SfCoopPoint2 targetVelocity,
    float priorPhase,float projectileAgeAfterStep,float dt,
    float referenceCombatWidth){
    // Historical COOP speed and TURN RATE, never the width of the SOLO map.
    const float speed=referenceCombatWidth*
        std::min(1.5f,.65f+1.8f*projectileAgeAfterStep);
    const auto aim=sfCoopPredictInterceptCore(
        projectile,target,targetVelocity,speed,.7f);
    const float desired=std::atan2(aim.y-projectile.y,aim.x-projectile.x);
    const float turn=std::clamp(std::remainder(
        desired-priorPhase,2*float(3.14159265358979323846)),
        -dt*1.1f,dt*1.1f);
    const float phase=priorPhase+turn;
    return {phase,std::cos(phase)*speed,std::sin(phase)*speed,speed};
}
