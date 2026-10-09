#pragma once
// Isolated SDL2 visual prototype for SOLO. It does not change classic or COOP.
#include "solo_session.hpp"
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
} // namespace sfsolo
