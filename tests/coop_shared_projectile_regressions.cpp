#include "../src/coop_shared_projectile.hpp"
#include <cassert>
#include <cmath>
#include <cstdint>
#include <algorithm>

// Frozen pre-extraction reference, from tactical_runtime.hpp and
// campaign_runtime.hpp in the Astra-audited f4526cfa commit.
// This is NOT a fake COOP simulation: production COOP now executes the
// extracted helper and each case is compared against the previous equations.
static SfCoopPoint2 historicalIntercept(SfCoopPoint2 origin,SfCoopPoint2 target,
                                        SfCoopPoint2 v,float speed,float horizon){
    const float dx=target.x-origin.x,dy=target.y-origin.y;
    const float a=v.x*v.x+v.y*v.y-speed*speed;
    const float b=2*(dx*v.x+dy*v.y),c=dx*dx+dy*dy;
    float time=std::sqrt(dx*dx+dy*dy)/std::max(1.0f,speed);
    if(std::abs(a)<.001f){if(b<-.001f)time=-c/b;}
    else{
        const float disc=b*b-4*a*c;
        if(disc>=0){
            const float root=std::sqrt(disc);
            const float t1=(-b-root)/(2*a),t2=(-b+root)/(2*a);
            if(t1>0&&t2>0)time=std::min(t1,t2);
            else if(t1>0)time=t1;
            else if(t2>0)time=t2;
        }
    }
    time=std::clamp(time,0.0f,horizon);
    return {target.x+v.x*time,target.y+v.y*time};
}
static SfCoopGuidance historicalGuidance(SfCoopPoint2 shot,SfCoopPoint2 target,
    SfCoopPoint2 v,float phase,float age,float dt,float width){
    const float speed=width*std::min(1.5f,.65f+1.8f*age);
    const auto aim=historicalIntercept(shot,target,v,speed,.7f);
    const float desired=std::atan2(aim.y-shot.y,aim.x-shot.x);
    phase+=std::clamp(std::remainder(desired-phase,2*float(3.14159265358979323846)),
                      -dt*1.1f,dt*1.1f);
    return {phase,std::cos(phase)*speed,std::sin(phase)*speed,speed};
}
static bool near(float a,float b,float tol=.0005f){
    return std::abs(a-b)<=tol*std::max(1.0f,std::max(std::abs(a),std::abs(b)));
}
int main(){
    std::uint32_t seed=0xFABC011u;
    const auto roll=[&seed](){
        seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;
        return float(seed%20001u)/10000.0f-1.0f;
    };
    for(int i=0;i<1200;++i){
        const SfCoopPoint2 shot{roll()*100,roll()*100};
        const SfCoopPoint2 target{roll()*100+140,roll()*100-35};
        const SfCoopPoint2 velocity{roll()*42,roll()*42};
        const float width=400+(i%7)*100.0f,phase=roll()*3.14f;
        const float dt= i%3==0?1.0f/30:i%3==1?1.0f/60:1.0f/120;
        const float age=.02f+(i%60)*dt;
        const auto a=historicalGuidance(shot,target,velocity,phase,age,dt,width);
        const auto b=sfCoopGuidedMissileCore(shot,target,velocity,phase,age,dt,width);
        assert(near(a.phase,b.phase));
        assert(near(a.vx,b.vx));assert(near(a.vy,b.vy));
        assert(near(a.speed,b.speed));
        const auto oldAim=historicalIntercept(shot,target,velocity,width*.5f,.7f);
        const auto newAim=sfCoopPredictInterceptCore(shot,target,velocity,width*.5f,.7f);
        assert(near(oldAim.x,newAim.x)&&near(oldAim.y,newAim.y));
    }
    // Moving boss versus immobile aim: same speed but genuinely different
    // intercept path. A future SOLO context must feed live target velocity.
    const auto moving=sfCoopGuidedMissileCore({0,0},{0,100},{40,0},
                                               0,.4f,.05f,780);
    const auto still=sfCoopGuidedMissileCore({0,0},{0,100},{0,0},
                                              0,.4f,.05f,780);
    assert(moving.speed==still.speed);
    assert(std::isfinite(moving.phase));
    // Phase steering is limited to historical 1.1 radians / second.
    assert(std::abs(moving.phase)<=.05f*1.1f+.0001f);
    assert(std::abs(moving.phase-still.phase)<=.11f);
    return 0;
}
