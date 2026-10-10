#pragma once
// Isolated SDL2 visual prototype for SOLO. It does not change classic or COOP.
#include "solo_session.hpp"
#include "solo_viewport.hpp"
#include "solo_combat.hpp"
#include "solo_kinetic_control.hpp"
#include "solo_bitmap_font.hpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>

namespace sfsolo {
inline SDL_Color tileColor(Tile tile){
    switch(tile){
        case Tile::Empty: return {9,13,28,255};
        case Tile::Rock: return {110,118,132,255};
        case Tile::Lava: return {255,104,25,255};
        case Tile::Asteroid: return {168,180,196,255};
        case Tile::IceAsteroid: return {238,250,255,255};
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
    const SoloViewport camera=soloViewport(s,viewport);
    const float cell=camera.cell;
    const int firstX=std::max(0,int(std::floor(camera.left)));
    const int lastX=std::min(s.map.width,int(std::ceil(camera.left+camera.visibleColumns()))+1);
    const int firstY=std::max(0,int(std::floor(camera.top)));
    const int lastY=std::min(s.map.height,int(std::ceil(camera.top+camera.visibleRows()))+1);
    // Small fixed star marks give the playfield scale without affecting map
    // collision data or drawing a zoomed-out topological overview.
    SDL_SetRenderDrawColor(r,40,60,95,255);
    for(int y=firstY;y<lastY;++y)for(int x=firstX;x<lastX;++x){
        const unsigned hash=unsigned(x*92821u+y*68917u+2167u);
        if(hash%47u==0u){
            const int px=camera.pixelX(x+.26f),py=camera.pixelY(y+.38f);
            SDL_RenderDrawPoint(r,px,py);
        }
    }
    for(int y=firstY;y<lastY;y++)for(int x=firstX;x<lastX;x++){
        const Tile t=s.map.at(x,y);
        if(t==Tile::Empty)continue;
        const int px=camera.pixelX(float(x)),py=camera.pixelY(float(y));
        const int nextX=camera.pixelX(float(x+1)),nextY=camera.pixelY(float(y+1));
        const SDL_Rect tileRect{px,py,std::max(1,nextX-px),std::max(1,nextY-py)};
        if(t==Tile::Rock || t==Tile::Lava || t==Tile::Beam){
            fill(r,tileRect,tileColor(t));
        }else{
            // Distinct visible markers in the debug prototype, not the final
            // original artwork: asteroid = rough cross, enemy = red square.
            const int cx=camera.pixelX(x+.5f),cy=camera.pixelY(y+.5f);
            const int radius=std::max(5,int(cell*.30f));
            if(t==Tile::Asteroid || t==Tile::IceAsteroid){
                fill(r,{cx-radius,cy-radius/2,radius*2+1,radius+1},tileColor(t));
                fill(r,{cx-radius/2,cy-radius,radius+1,radius*2+1},tileColor(t));
            }else{
                fill(r,{cx-radius,cy-radius,radius*2+1,radius*2+1},tileColor(t));
            }
        }
    }
    // Readable player ship, anchored safely above Android's navigation bar.
    const int shipX=camera.pixelX(s.pilot.x);
    const int shipY=camera.pixelY(s.pilot.y);
    const int radius=std::max(14,int(cell*.52f));
    SDL_SetRenderDrawColor(r,85,229,255,255);
    SDL_RenderDrawLine(r,shipX,shipY-radius,shipX-radius,shipY+radius);
    SDL_RenderDrawLine(r,shipX-radius,shipY+radius,shipX,shipY+radius/2);
    SDL_RenderDrawLine(r,shipX,shipY+radius/2,shipX+radius,shipY+radius);
    SDL_RenderDrawLine(r,shipX+radius,shipY+radius,shipX,shipY-radius);
    SDL_SetRenderDrawColor(r,255,255,255,255);
    SDL_RenderDrawLine(r,shipX,shipY-radius+3,shipX,shipY+radius/2);
    fill(r,{shipX-3,shipY-2,7,12},{80,193,255,255});
    if(s.iceActive()){
        SDL_SetRenderDrawColor(r,220,251,255,255);
        const int halo=radius+std::max(4,radius/3);
        SDL_RenderDrawLine(r,shipX-halo,shipY,shipX,shipY-halo);
        SDL_RenderDrawLine(r,shipX,shipY-halo,shipX+halo,shipY);
        SDL_RenderDrawLine(r,shipX+halo,shipY,shipX,shipY+halo);
        SDL_RenderDrawLine(r,shipX,shipY+halo,shipX-halo,shipY);
    }
    // The REAL minimap remains a separate, smaller upper-right widget.
    const int mw=std::max(46,viewport.w/7),mh=std::max(66,viewport.h/6);
    const int hudY=viewport.y+std::max(40,viewport.h/18);
    SDL_Rect mini{viewport.x+viewport.w-mw-12,hudY,mw,mh};
    fill(r,mini,{25,35,58,255});
    const auto p=miniMapPosition(s);
    fill(r,{mini.x+int(p.x*(mw-1))-3,mini.y+int(p.y*(mh-1))-3,7,7},
         {70,241,255,255});
    if(!s.bossDefeated)
        fill(r,{mini.x+mw/2-2,mini.y+int(9.5f/s.map.height*mh)-2,5,5},
             {255,54,176,255});
    if(s.iceActive()){
        // Accessible status: temporary 2.5-second ice malus, never permanent.
        const SDL_Rect tag{viewport.x+12,hudY+26,
                           std::max(135,viewport.w*2/5),std::max(37,viewport.h/30)};
        fill(r,tag,{28,86,128,255});
        soloLabel(r,"GLACE",tag,{255,255,255,255},std::max(3,viewport.w/180));
    }
    // HUD also clears the phone status bar for a readable health indicator.
    fill(r,{viewport.x+12,hudY,std::max(1,int(viewport.w*.35f)),11},
         {85,32,38,255});
    fill(r,{viewport.x+12,hudY,
            std::max(0,int(viewport.w*.35f*std::clamp(s.pilot.health/100.0f,0.0f,1.0f))),11},
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
     const SoloViewport camera=soloViewport(s,viewport);
     const float cell=camera.cell;
     const int shipX=camera.pixelX(s.pilot.x);
     const int shipY=camera.pixelY(s.pilot.y);
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
    const SoloViewport camera=soloViewport(s,viewport);
    const float cell=camera.cell;
    for(const auto &shot:combat.shots){
        const int px=camera.pixelX(shot.x);
        const int py=camera.pixelY(shot.y);
        if(px<viewport.x||px>=viewport.x+viewport.w||py<viewport.y||py>=viewport.y+viewport.h)continue;
        fill(r,{px-2,py-4,5,9},{255,232,100,255});
    }
    if(combat.bossHealth<combat.bossMaxHealth && !s.bossDefeated){
        const int width=std::max(1,viewport.w/2);
        const int x=viewport.x+(viewport.w-width)/2;
        const int y=viewport.y+std::max(50,viewport.h/18)+25;
        fill(r,{x,y,width,9},{76,26,54,255});
        fill(r,{x,y,int(width*std::clamp(combat.bossHealth/
             std::max(1.0f,combat.bossMaxHealth),0.0f,1.0f)),9},
             {255,65,176,255});
    }
}
} // namespace sfsolo
