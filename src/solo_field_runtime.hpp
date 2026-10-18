#pragma once
#include "solo_rock_physics.hpp"
// Real legacy sprites, velocities converted at the fixed 60 Hz boundary.
static float sfSoloRockRadius(const sprite *rock){return std::max(rock->w,rock->h)*.5f;}
static bool sfSoloRockFits(tupl p,float radius){
    if(sfSoloField.maze)return sfSoloField.occupy && sfSoloField.occupy(sfSoloField.context,p,radius);
    return p.x>=radius && p.y>=radius && p.x<=sfArenaW-radius && p.y<=sfArenaH-radius;
}
static void sfSoloRockFragments(sprite *rock){
    const float r=sfSoloRockRadius(rock),child=r*.42f;
    if(child<sfArenaH/220 || sa1.size()>150)return;
    const float speed=std::hypot(rock->vx,rock->vy),nx=speed>.001f?-rock->vy/speed:1.f,ny=speed>.001f?rock->vx/speed:0.f;
    for(int side:{-1,1}){
        const tupl at(rock->x+nx*side*child*1.2f,rock->y+ny*side*child*1.2f);
        if(!sfSoloRockFits(at,child))continue;
        bool clear=true;
        for(const auto *other:sa1)if(other!=rock && other->pv>0 &&
            std::hypot(other->x-at.x,other->y-at.y)<child+sfSoloRockRadius(other)+1){clear=false;break;}
        if(!clear)continue;
        auto *fragment=new sprite;fragment->setxywh(at.x,at.y,child*2,child*2);
        fragment->sw=fragment->w;fragment->sh=fragment->h;fragment->pv=1;fragment->name=rock->name;
        fragment->vx=rock->vx*.5f+nx*side*.3f;fragment->vy=rock->vy*.5f+ny*side*.3f;
        fragment->as=0;fragment->timer=0;fragment->rand0=0;fragment->kineticStage=rock->kineticStage;fragment->startup();sa1.push_back(fragment);
    }
}
static void sfSoloRockPair(sprite *a,sprite *b);
static void sfSoloRockMove(sprite *rock){
    const float r=sfSoloRockRadius(rock);
    if(rock->timer>0 && ++rock->timer>250)rock->timer=0;
    if(rock->vx==0 && rock->vy==0){rock->startup();return;}
    const tupl from(rock->x,rock->y),desired(from.x+rock->vx,from.y+rock->vy);
    tupl accepted=desired;bool hitX=false,hitY=false;
    if(sfSoloField.maze && sfSoloField.sweep){
        // Meteors rebound along a straight swept segment. The ship's sliding
        // resolver may travel around a corner; its endpoint chord is NOT a
        // valid trajectory for intermediate object collisions.
        const float t=sfSoloField.sweep(sfSoloField.context,from,desired,r);
        if(t<1){
            const float safe=std::max(0.f,t-.01f/std::max(1.f,std::hypot(rock->vx,rock->vy)));
            accepted=tupl(from.x+rock->vx*safe,from.y+rock->vy*safe);
            const tupl probeX(accepted.x+std::copysign(.1f,rock->vx),accepted.y);
            const tupl probeY(accepted.x,accepted.y+std::copysign(.1f,rock->vy));
            hitX=rock->vx!=0 && sfSoloField.sweep(sfSoloField.context,accepted,probeX,r)<1;
            hitY=rock->vy!=0 && sfSoloField.sweep(sfSoloField.context,accepted,probeY,r)<1;
            if(!hitX&&!hitY){hitX=rock->vx!=0;hitY=rock->vy!=0;}
        }
    }else if(sfSoloField.maze){accepted=from;hitX=rock->vx!=0;hitY=rock->vy!=0;}
    else {
        accepted.x=std::clamp(desired.x,r,sfArenaW-r);accepted.y=std::clamp(desired.y,r,sfArenaH-r);
        hitX=accepted.x!=desired.x;hitY=accepted.y!=desired.y;
    }
    const float before=std::hypot(rock->vx,rock->vy)*60.f;
    // Sweep against the other real bodies before accepting any displacement.
    // Sequential contact resolution is dissipative, including a moving rock
    // waking a sleeping deposit. No static/static correction is ever applied.
    float fraction=1; sprite *contact=nullptr;
    for(auto *other:sa1)if(other!=rock && other->pv>0){
        const float t=sfsolo::rockSweep(from.x,from.y,accepted.x,accepted.y,other->x,other->y,r+sfSoloRockRadius(other));
        if(t<fraction){fraction=t;contact=other;}
    }
    if(sfSoloField.bossPresent){
        const float t=sfsolo::rockSweep(from.x,from.y,accepted.x,accepted.y,
            sfSoloField.bossCentre.x,sfSoloField.bossCentre.y,r+sfSoloField.bossRadius);
        if(t<fraction){fraction=t;contact=nullptr;}
    }
    rock->x=from.x+(accepted.x-from.x)*fraction;rock->y=from.y+(accepted.y-from.y)*fraction;
    if(contact)sfSoloRockPair(rock,contact);
    if(rock->pv<=0){rock->startup();return;}
    if(fraction>=1 && hitX)rock->vx*=-.38f;if(fraction>=1 && hitY)rock->vy*=-.38f;
    if(fraction>=1 && (hitX||hitY)){
        if(sfFieldAudible(rock->x,rock->y))sfFieldCollisionSound=true;
        if(before>sfArenaW*.9f){sfSoloRockFragments(rock);sfKineticDestroyAsteroid(rock,SfKineticDustCause::AsteroidCollision);}
    }
    sfsolo::RockBody body{rock->x,rock->y,rock->vx*60,rock->vy*60,r};
    sfsolo::rockDamp(body,1.f/60,sfArenaW*.002f);
    rock->vx=body.vx/60;rock->vy=body.vy/60;rock->startup();
}
static void sfSoloRockPair(sprite *a,sprite *b){
    sfsolo::RockBody aa{a->x,a->y,a->vx*60,a->vy*60,sfSoloRockRadius(a)};
    sfsolo::RockBody bb{b->x,b->y,b->vx*60,b->vy*60,sfSoloRockRadius(b)};
    const float energy=sfsolo::rockContact(aa,bb);
    if(energy<=0)return;
    a->vx=aa.vx/60;a->vy=aa.vy/60;b->vx=bb.vx/60;b->vy=bb.vy/60;
    if(sfFieldAudible((a->x+b->x)*.5f,(a->y+b->y)*.5f)){
        sfFieldCollisionSound=true;
        sfCoop.soundRockPair=true; // SOLO-025 dedicated rock-rock bank
    }
    sfFieldImpact(a,b,true);
    auto *small=aa.radius<bb.radius?a:b;
    const float mass=sfSoloRockRadius(small)*sfSoloRockRadius(small);
    if(energy>mass*sfArenaW*sfArenaW*.12f){
        sfSoloRockFragments(small);sfKineticDestroyAsteroid(small,SfKineticDustCause::AsteroidCollision);
    }
}
