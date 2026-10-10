#pragma once
// Isolated SDL2 visual prototype for SOLO. It does not change classic or COOP.
#include "solo_session.hpp"
#include "solo_combat.hpp"
#include "solo_kinetic_control.hpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>

namespace sfsolo {
inline SDL_Color tileColor(Tile tile){
    switch(tile){
        case Tile::Empty: return {9,13,28,255};
        case Tile::Rock: return {110,118,132,255};
        case Tile::Lava: return {255,104,25,255};
        case Tile::Asteroid: return {230,231,238,255};
        case Tile::Beam: return {160,75,245,255};
        case Tile::Enemy: return {245,64,64,255};
        case Tile::Boss: return {255,40,180,255};
        case Tile::Start: return {55,135,255,255};
        case Tile::Finish: return {54,239,112,255};
        case Tile::Trigger: return {245,219,65,255};
    }
    return {0,0,0,255};
}
inline void fill(SDL_Renderer *r,SDL_Rect rect,SDL_Color c){
    SDL_SetRenderDrawColor(r,c.r,c.g,c.b,c.a);
    SDL_RenderFillRect(r,&rect);
}
inline void drawSession(SDL_Renderer *r,const Session &s,SDL_Rect viewport){
    if(!r||!s.map.valid()||viewport.w<=0||viewport.h<=0)return;
    fill(r,viewport,{5,9,23,255});
    const float cell=std::max(1.0f,float(viewport.w)/float(s.map.width));
    const float visibleRows=float(viewport.h)/cell;
    const float top=std::clamp(s.cameraY-visibleRows*.58f,0.0f,
                               std::max(0.0f,float(s.map.height)-visibleRows));
    const int first=std::max(0,int(std::floor(top)));
    const int last=std::min(s.map.height,int(std::ceil(top+visibleRows))+1);
    for(int y=first;y<last;y++)for(int x=0;x<s.map.width;x++){
        const Tile t=s.map.at(x,y);
        if(t==Tile::Empty)continue;
        const int px=viewport.x+int(x*cell);
        const int py=viewport.y+int((y-top)*cell);
        const int nextX=viewport.x+int((x+1)*cell);
        const int nextY=viewport.y+int((y+1-top)*cell);
        fill(r,{px,py,std::max(1,nextX-px),std::max(1,nextY-py)},tileColor(t));
    }
    const int shipX=viewport.x+int(s.pilot.x*cell);
    const int shipY=viewport.y+int((s.pilot.y-top)*cell);
    const int radius=std::max(4,int(cell*.5f));
    SDL_SetRenderDrawColor(r,78,233,249,255);
    SDL_RenderDrawLine(r,shipX,shipY-radius,shipX-radius,shipY+radius);
    SDL_RenderDrawLine(r,shipX-radius,shipY+radius,shipX+radius,shipY+radius);
    SDL_RenderDrawLine(r,shipX+radius,shipY+radius,shipX,shipY-radius);
    // Always-visible minimap with ship and boss markers.
    const int mw=std::max(18,viewport.w/7),mh=std::max(44,viewport.h/4);
    SDL_Rect mini{viewport.x+viewport.w-mw-8,viewport.y+8,mw,mh};
    fill(r,mini,{22,28,47,255});
    const auto p=miniMapPosition(s);
    fill(r,{mini.x+int(p.x*(mw-1))-2,mini.y+int(p.y*(mh-1))-2,5,5},
         {70,241,255,255});
    if(!s.bossDefeated)
        fill(r,{mini.x+mw/2-2,mini.y+int(9.5f/s.map.height*mh)-2,5,5},
             {255,54,176,255});
    // Hull indicator: visual-only until canonical HUD is integrated.
    fill(r,{viewport.x+8,viewport.y+8,std::max(1,int(viewport.w*.32f)),7},
         {85,32,38,255});
    fill(r,{viewport.x+8,viewport.y+8,
            std::max(0,int(viewport.w*.32f*std::clamp(s.pilot.health/100.0f,0.0f,1.0f))),7},
         {90,220,130,255});
}


 // Temporary high-contrast charge aid for the SOLO debug prototype.
 // Values and cone geometry come from the real shared kinetic model, not
 // from a parallel copy of its two-second timer or danger-level formula.
 inline void drawKineticCharge(SDL_Renderer *r,const Session &s,SDL_Rect viewport){
     if(!r || viewport.w<=0 || viewport.h<=0 || !sfKineticSurges[1].held)return;
     const auto &surge=sfKineticSurges[1];
     const float progress=std::clamp(
         surge.heldSeconds/SF_KINETIC_SURGE_HOLD_SECONDS,0.0f,1.0f);
     const int barW=std::max(60,viewport.w/2);
     const int barH=std::max(12,viewport.h/65);
     const int barX=viewport.x+(viewport.w-barW)/2;
     const int barY=viewport.y+viewport.h-barH-18;
     fill(r,{barX-3,barY-3,barW+6,barH+6},{255,255,255,255});
     fill(r,{barX,barY,barW,barH},{20,37,62,255});
     fill(r,{barX,barY,int(barW*progress),barH},
          surge.charged ? SDL_Color{94,255,153,255}
                        : SDL_Color{255,224,94,255});
     if(!sfKineticSurgeVisible(1))return;
     const float cell=std::max(1.0f,float(viewport.w)/float(s.map.width));
     const float rows=float(viewport.h)/cell;
     const float top=std::clamp(s.cameraY-rows*.58f,0.0f,
                               std::max(0.0f,float(s.map.height)-rows));
     const int shipX=viewport.x+int(s.pilot.x*cell);
     const int shipY=viewport.y+int((s.pilot.y-top)*cell);
     const float range=2.0f*s.shipRadius*sfKineticSurgeMiningRangeDiameters();
     const int frontY=shipY-int(range*cell);
     const int half=int(sfKineticSurgeConeHalfWidth(range)*cell);
     SDL_SetRenderDrawColor(r,115,230,255,255);
     SDL_RenderDrawLine(r,shipX,shipY,shipX-half,frontY);
     SDL_RenderDrawLine(r,shipX,shipY,shipX+half,frontY);
     SDL_RenderDrawLine(r,shipX-half,frontY,shipX+half,frontY);
 }

// Render SOLO projectiles and boss health using the same camera transform.
inline void drawCombat(SDL_Renderer *r,const Session &s,const Combat &combat,SDL_Rect viewport){
    if(!r||!s.map.valid()||viewport.w<=0||viewport.h<=0)return;
    const float cell=std::max(1.0f,float(viewport.w)/float(s.map.width));
    const float rows=float(viewport.h)/cell;
    const float top=std::clamp(s.cameraY-rows*.58f,0.0f,
                              std::max(0.0f,float(s.map.height)-rows));
    for(const auto &shot:combat.shots){
        const int px=viewport.x+int(shot.x*cell);
        const int py=viewport.y+int((shot.y-top)*cell);
        if(py<viewport.y||py>=viewport.y+viewport.h)continue;
        fill(r,{px-2,py-4,5,9},{255,232,100,255});
    }
    if(combat.bossHealth<combat.bossMaxHealth && !s.bossDefeated){
        const int width=std::max(1,viewport.w/2);
        const int x=viewport.x+(viewport.w-width)/2;
        const int y=viewport.y+24;
        fill(r,{x,y,width,9},{76,26,54,255});
        fill(r,{x,y,int(width*std::clamp(combat.bossHealth/
             std::max(1.0f,combat.bossMaxHealth),0.0f,1.0f)),9},
             {255,65,176,255});
    }
}
} // namespace sfsolo
