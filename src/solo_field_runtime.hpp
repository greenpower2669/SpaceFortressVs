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
        fragment->as=0;fragment->timer=0;fragment->startup();sa1.push_back(fragment);
    }
}
static void sfSoloRockMove(sprite *rock){
    const float r=sfSoloRockRadius(rock);
    if(rock->timer>0 && ++rock->timer>250)rock->timer=0;
    if(rock->vx==0 && rock->vy==0){rock->startup();return;}
    const tupl from(rock->x,rock->y),desired(from.x+rock->vx,from.y+rock->vy);
    tupl accepted=desired;
    if(sfSoloField.maze && sfSoloField.resolve)
        accepted=sfSoloField.resolve(sfSoloField.context,from,desired,r);
    else {accepted.x=std::clamp(desired.x,r,sfArenaW-r);accepted.y=std::clamp(desired.y,r,sfArenaH-r);}
    const float before=std::hypot(rock->vx,rock->vy)*60.f;
    const bool hitX=std::abs(accepted.x-desired.x)>.001f,hitY=std::abs(accepted.y-desired.y)>.001f;
    rock->x=accepted.x;rock->y=accepted.y;
    if(hitX)rock->vx*=-.38f;if(hitY)rock->vy*=-.38f;
    if(hitX||hitY){
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
    if(sfFieldAudible((a->x+b->x)*.5f,(a->y+b->y)*.5f))sfFieldCollisionSound=true;
    sfFieldImpact(a,b,true);
    auto *small=aa.radius<bb.radius?a:b;
    const float mass=sfSoloRockRadius(small)*sfSoloRockRadius(small);
    if(energy>mass*sfArenaW*sfArenaW*.12f){
        sfSoloRockFragments(small);sfKineticDestroyAsteroid(small,SfKineticDustCause::AsteroidCollision);
    }
}
