#pragma once
// SOLO-COOP-WORLD-VISUAL-006
// Render the genuine COOP actors with their historic textures, but project
// each draw into a SCREEN-SIZED viewport. Android GPUs may reject an SDL
// viewport of the whole 54x160 map (tens of thousands of pixels tall).
// No player/projectile/dust position in the PHYSICS engine is ever modified.
#include "coop_solo_world_bridge.hpp"
#include <algorithm>
#include <cmath>

static void sfCoopSoloWorldActors(SDL_Renderer *renderer,
                                  const sfsolo::CoopSoloWorldBridge &world)
{
    if(!renderer || !world.ready)return;
    // The render target and viewport are the actual DISPLAY, not the map.
    SDL_Rect original{};
    SDL_RenderGetViewport(renderer,&original);
    SDL_Rect screen{0,0,world.screen.w,world.screen.h};
    SDL_RenderSetViewport(renderer,&screen);
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    auto &textures=sfCoopTextures(renderer);
    auto project=[&](float x,float y){
        return world.toScreen({x,y});
    };
    auto visible=[&](float x,float y,float radius){
        return x>=-radius && x<=world.screen.w+radius &&
               y>=-radius && y<=world.screen.h+radius;
    };
    auto disc=[&](float x,float y,float radius,SDL_Color color){
        const auto p=project(x,y);
        if(visible(p.x,p.y,radius))
            sfCoopDisc(renderer,p.x,p.y,radius,color);
    };
    // Historical four asteroid atlases + their actual physics dimensions.
    for(const auto *rock:sa1){
        if(!rock || rock->pv<=0)continue;
        const auto p=project(rock->x,rock->y);
        const float radius=std::max(rock->w,rock->h)*.75f;
        if(!visible(p.x,p.y,radius))continue;
        const int ix=rock->name.size()==2?
            std::clamp(rock->name[1]-'1',0,3):0;
        SDL_Rect dest{int(p.x-rock->w*.5f),int(p.y-rock->h*.5f),
                      int(rock->w),int(rock->h)};
        if(textures.rocks[ix])
            SDL_RenderCopyEx(renderer,textures.rocks[ix],nullptr,&dest,
                rock->rand0+sfCoop.time*rock->as*rock->asign,nullptr,rock->flip);
    }
    // Same dust sprites/colors as the historical COOP renderer, screen
    // projection alone is new.
    for(const auto *d:particules)if(d && d->pv>0)
        disc(d->x,d->y,2.5f,{125,235,255,220});
    for(const auto *d:particulesr)if(d && d->pv>0)
        disc(d->x,d->y,2.0f,{255,130,70,200});
    for(const auto *e:explos){
        if(!e || e->pv<=0 || e->w<=0 || e->h<=0)continue;
        const auto p=project(e->x,e->y);
        if(!visible(p.x,p.y,std::max(e->w,e->h)))continue;
        const bool smoke=e->name=="pous" || e->name=="tj1" || e->name=="tj2";
        const bool weak=sfShieldFraction(Spritej2->nrj)<.35f &&
            vlong(e->x-Spritej2->x,e->y-Spritej2->y)<sfCoopShipRadius()*2.2f;
        SDL_Texture *fx=(smoke&&!weak)?textures.impact:textures.explosion;
        if(!fx)fx=textures.explosion?textures.explosion:textures.impact;
        if(!fx)continue;
        SDL_Rect rect{int(p.x-e->w*.5f),int(p.y-e->h*.5f),int(e->w),int(e->h)};
        SDL_RenderCopyEx(renderer,fx,nullptr,&rect,
            sfCoop.time*e->as*e->asign*5,nullptr,e->flip);
    }
    // Real SfCoopShot, laser/orb/missile sprites; no duplicate shot logic.
    for(const auto &shot:sfCoop.shots){
        const auto p=project(shot.position.x,shot.position.y);
        const float size=shot.kind==4?shot.radius*5.0f:shot.radius*2.0f;
        if(!visible(p.x,p.y,size))continue;
        SDL_Color color=shot.owner==0 ? SDL_Color{255,175,75,255} :
            shot.owner==1?SDL_Color{100,220,255,255}:SDL_Color{255,70,140,255};
        if(shot.kind==2)
            color=shot.age<1.2f?SDL_Color{255,200,75,190}:SDL_Color{255,70,50,255};
        if(shot.kind==4 && textures.missile){
            const int w=std::max(8,int(shot.radius*1.55f)),
                      h=std::max(18,int(shot.radius*4.8f));
            SDL_Rect rect{int(p.x-w*.5f),int(p.y-h*.5f),w,h};
            const float speed=std::max(1.0f,vlong(shot.velocity.vx,shot.velocity.vy));
            const float ux=shot.velocity.vx/speed,uy=shot.velocity.vy/speed;
            for(int i=1;i<=4;++i){
                const float trail=shot.radius*(1.5f+i*1.05f);
                const float rr=shot.radius*(.85f-.12f*i);
                disc(shot.position.x-ux*trail,shot.position.y-uy*trail,
                     std::max(1.0f,rr),
                     {255,Uint8(std::max(70,205-i*30)),45,Uint8(180-i*26)});
            }
            disc(shot.position.x,shot.position.y,shot.radius*1.65f,
                 {color.r,color.g,color.b,58});
            const double angle=std::atan2(shot.velocity.vy,shot.velocity.vx)*180.0/PI+90.0;
            SDL_RenderCopyEx(renderer,textures.missile,nullptr,&rect,angle,
                             nullptr,SDL_FLIP_NONE);
        }else if(shot.kind==0 && shot.owner>=0){
            SDL_Texture *orb=shot.owner==0?textures.orbOrange:textures.orbBlue;
            if(orb){
                const int side=std::max(7,int(shot.radius*2.55f));
                SDL_Rect rect{int(p.x-side*.5f),int(p.y-side*.5f),side,side};
                SDL_RenderCopy(renderer,orb,nullptr,&rect);
            }else disc(shot.position.x,shot.position.y,shot.radius,color);
        }else{
            disc(shot.position.x,shot.position.y,shot.radius*1.7f,
                 {color.r,color.g,color.b,65});
            disc(shot.position.x,shot.position.y,shot.radius,color);
        }
    }
    // The cone/rings obey the exact historic charge state, direction and
    // range. Only their VERTICES are projected into screen coordinates.
    if(sfKineticSuctionVisible(1) && Spritej2->pv>0){
        const float d=std::max(1.0f,std::max({Spritej2->w,Spritej2->h,
                                              Spritej2->sw,Spritej2->sh}));
        const SfKineticVector axis=sfCoop.soloKineticAxes.active(1)?
            sfCoop.soloKineticAxes.direction(1):SfKineticVector{0,-1};
        const float noseX=Spritej2->x+axis.x*d*.42f;
        const float noseY=Spritej2->y+axis.y*d*.42f;
        const float distance=d*sfKineticSurgeDustRangeDiameters();
        const float half=sfKineticSurgeConeHalfWidth(distance);
        const auto nose=project(noseX,noseY);
        const auto l=project(noseX+axis.x*distance-axis.y*half,
                             noseY+axis.y*distance+axis.x*half);
        const auto r=project(noseX+axis.x*distance+axis.y*half,
                             noseY+axis.y*distance-axis.x*half);
        SDL_Color orange{255,132,28,22};
        const SDL_Vertex wedge[]={{{nose.x,nose.y},orange,{0,0}},
                                  {{l.x,l.y},orange,{0,0}},
                                  {{r.x,r.y},orange,{0,0}}};
        const int faces[]={0,1,2};
        SDL_RenderGeometry(renderer,nullptr,wedge,3,faces,3);
        SDL_SetRenderDrawColor(renderer,255,142,36,62);
        SDL_RenderDrawLineF(renderer,nose.x,nose.y,l.x,l.y);
        SDL_RenderDrawLineF(renderer,nose.x,nose.y,r.x,r.y);
    }
    if(sfKineticSurgeVisible(1) && Spritej2->pv>0){
        const auto p=project(Spritej2->x,Spritej2->y);
        const float diameter=std::max(1.0f,std::max({Spritej2->w,Spritej2->h,
                                                     Spritej2->sw,Spritej2->sh}));
        const float radius=diameter*SF_KINETIC_MAX_SHIELD_DIAMETER*.5f;
        const float pulse=.5f+.5f*std::sin(float(SDL_GetTicks64())*.010f+1.7f);
        const float progress=sfKineticSuperchargeProgress(1);
        sfTacticalRing(renderer,tupl(p.x,p.y),radius,
            sfKineticSuperchargeColor(progress,Uint8(132+38*pulse)));
        sfTacticalRing(renderer,tupl(p.x,p.y),std::max(1.0f,radius-3.0f),
            sfKineticSuperchargeColor(progress,Uint8(70+24*pulse)));
    }
    for(const auto &wave:sfKineticWaves){
        if(wave.owner!=1 || !sfKineticWaveAlive(wave))continue;
        const float diameter=std::max(1.0f,std::max({Spritej2->w,Spritej2->h,
                                                     Spritej2->sw,Spritej2->sh}));
        const float radius=sfKineticWaveRadiusAt(wave,diameter,wave.age);
        if(radius<=0)continue;
        const auto p=project(Spritej2->x,Spritej2->y);
        sfTacticalRing(renderer,tupl(p.x,p.y),radius,{255,185,98,92});
    }

    // The atlas boss and its kinetic shield are projected to the REAL
    // viewport, never drawn at the original arena origin. This is a WORLD
    // diagnostic encounter; the existing boss physics handles meteor hits.
    if(sfCoop.bossPresent){
        const auto boss=project(sfCoop.position.x,sfCoop.position.y);
        const float radius=sfCoopBossRadius();
        if(visible(boss.x,boss.y,radius*1.3f)){
            sfCoopCircleAlpha(renderer,int(boss.x),int(boss.y),
                int(radius*SF_COOP_BOSS_KINETIC_FIELD_RADIUS_SCALE),
                145,95,215,sfCoopBossKineticVisibilityAlpha(sfCoop.bossKineticReserve));
            sfDrawEncounterBoss(renderer,textures.bosses,sfCoop.encounter,
                tupl(boss.x,boss.y),radius,sfCoop.time,sfCoop.hit,0.0f);
        }
    }

    // Draw the REAL historic blue COOP ship LAST. Never conflate its green
    // kinetic READY circle with its physical hull. A small bright cyan
    // fallback retains an identifiable craft if the Android PNG is absent.
    auto *ship=Spritej2;
    const auto p=project(ship->x,ship->y);
    const SDL_Rect hull{int(p.x-ship->w*.5f),int(p.y-ship->h*.5f),
                        std::max(1,int(ship->w)),std::max(1,int(ship->h))};
    if(ship->pv>0 && visible(p.x,p.y,ship->w)){
        if(textures.blue){
            SDL_SetTextureColorMod(textures.blue,255,255,255);
            SDL_SetTextureAlphaMod(textures.blue,
                sfCoop.invulnerable[1]>0 && int(sfCoop.time*12)%2?140:255);
            SDL_RenderCopyEx(renderer,textures.blue,nullptr,&hull,
                             -sfCoop.time*17,nullptr,SDL_FLIP_NONE);
            SDL_SetTextureAlphaMod(textures.blue,255);
        }else{
            const SDL_Color blue{80,222,255,245};
            const SDL_Vertex shipVertices[]={
                {{p.x,p.y-ship->h*.42f},blue,{0,0}},
                {{p.x-ship->w*.35f,p.y+ship->h*.28f},{25,110,245,240},{0,0}},
                {{p.x+ship->w*.35f,p.y+ship->h*.28f},{25,110,245,240},{0,0}}};
            const int tri[]={0,1,2};
            SDL_RenderGeometry(renderer,nullptr,shipVertices,3,tri,3);
        }
        // Always distinguish the ship's centre from its larger power ring.
        sfCoopDisc(renderer,p.x,p.y,std::max(2.0f,ship->w*.055f),
                   {180,246,255,220});
    }
    SDL_RenderSetViewport(renderer,&original);
}
