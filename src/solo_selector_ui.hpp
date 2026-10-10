#pragma once
// SDL2 touch-friendly SOLO world and stage selector, independent of legacy menus.
#include "solo_stage_selection.hpp"
#include "solo_bitmap_font.hpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <string>

namespace sfsolo {
enum class SelectAction { None, Play, Back };
struct Selector {
    StageSelection selection{};
    // Coordinates normalized to 0..1; large touch targets for accessibility.
    SelectAction touch(float x,float y,const Progression &progress){
        if(x<0||x>1||y<0||y>1)return SelectAction::None;
        if(y<.13f)return SelectAction::Back;
        if(y>=.20f&&y<.34f){
            const int delta=x<.5f?-1:1;
            const int world=int(selection.world)+delta;
            if(world>=1&&world<=int(Progression::initialWorlds))
                selectStage(progress,selection,unsigned(world),1,selection.difficulty);
        }else if(y>=.39f&&y<.54f){
            const int delta=x<.5f?-1:1;
            const int stage=int(selection.stage)+delta;
            if(stage>=1&&stage<=int(Progression::stagesPerWorld))
                selectStage(progress,selection,selection.world,unsigned(stage),selection.difficulty);
        }else if(y>=.59f&&y<.73f){
            const int difficulty=int(selection.difficulty)+(x<.5f?-1:1);
            if(difficulty>=1&&difficulty<=9)
                selectStage(progress,selection,selection.world,selection.stage,unsigned(difficulty));
        }else if(y>=.79f&&y<=.97f&&selectable(progress,selection)){
            return SelectAction::Play;
        }
        return SelectAction::None;
    }
};
inline void selectorBar(SDL_Renderer *r,SDL_Rect rect,SDL_Color color){
    SDL_SetRenderDrawColor(r,color.r,color.g,color.b,255);
    SDL_RenderFillRect(r,&rect);
}
inline void drawSelector(SDL_Renderer *r,SDL_Rect viewport,
                         const Selector &selector,const Progression &progress){
    if(!r||viewport.w<=0||viewport.h<=0)return;
    selectorBar(r,viewport,{8,13,31,255});
    const int cx=viewport.x+viewport.w/2;
    const int left=viewport.x+viewport.w/10;
    const int wide=viewport.w*8/10;
    const int pad=std::max(2,viewport.w/200);
    const int pixels=std::max(2,std::min(viewport.w/150,viewport.h/310));
    const SDL_Color white{244,247,255,255},cream{255,247,165,255};
    const auto rect=[&](float top,float bottom){
        return SDL_Rect{left,viewport.y+int(viewport.h*top),
                        wide,std::max(1,int(viewport.h*(bottom-top)))};
    };
    const auto panel=[&](float top,float bottom,SDL_Color bg) {
        SDL_Rect panel=rect(top,bottom);
        selectorBar(r,panel,{175,198,229,255});
        selectorBar(r,{panel.x+pad,panel.y+pad,
                       panel.w-2*pad,panel.h-2*pad},bg);
        return panel;
    };
    // Big centered words replace unlabelled colored rectangles. Only the
    // debug SOLO mode changes; the existing classic/duel/COOP menu is intact.
    const SDL_Rect back=panel(.045f,.125f,{44,61,94,255});
    soloLabel(r,"RETOUR",back,white,pixels);
    soloLabel(r,"SPACEFORTRESS",{left,viewport.y+int(viewport.h*.135f),
                                wide,int(viewport.h*.048f)},cream,pixels);
    soloLabel(r,"MODE SOLO",{left,viewport.y+int(viewport.h*.172f),
                            wide,int(viewport.h*.042f)},white,pixels);

    const auto controls=[&](float top,float bottom,const std::string &title,
                           unsigned selected,unsigned count,SDL_Color color){
        const SDL_Rect row=panel(top,bottom,color);
        const SDL_Rect name{row.x+row.w/9,row.y+row.h/9,
                            row.w*7/9,row.h*5/9};
        soloLabel(r,title,name,white,pixels);
        // Big visible arrows show which half of each touch target changes value.
        const int arrowY=row.y+row.h*2/5;
        const int dx=std::max(5,row.w/65);
        const int dy=std::max(8,row.h/16);
        const int arrowL=row.x+row.w/12,arrowR=row.x+row.w*11/12;
        SDL_SetRenderDrawColor(r,255,247,165,255);
        SDL_RenderDrawLine(r,arrowL+dx,arrowY-dy,arrowL,arrowY);
        SDL_RenderDrawLine(r,arrowL,arrowY,arrowL+dx,arrowY+dy);
        SDL_RenderDrawLine(r,arrowR-dx,arrowY-dy,arrowR,arrowY);
        SDL_RenderDrawLine(r,arrowR,arrowY,arrowR-dx,arrowY+dy);
        const int dotsY=row.y+row.h*4/5;
        const int dotW=std::max(2,(row.w*7/10)/int(count*2));
        for(unsigned i=0;i<count;++i){
            const int x=row.x+int((i+.5f)*row.w*.70f/count+row.w*.15f)-dotW/2;
            SDL_Rect dot{x,dotsY,dotW,std::max(4,row.h/12)};
            selectorBar(r,dot,i+1==selected?cream:SDL_Color{20,32,53,255});
        }
    };
    const auto number=[](unsigned n) {
        return std::string(n<10 ? "0" : "")+std::to_string(n);
    };
    controls(.22f,.35f,"MONDE "+number(selector.selection.world)+" / 06",
             selector.selection.world,6,{44,102,169,255});
    controls(.40f,.55f,"NIVEAU "+number(selector.selection.stage)+" / 20",
             selector.selection.stage,20,{40,133,131,255});
    controls(.60f,.75f,"DANGER "+number(selector.selection.difficulty)+" / 09",
             selector.selection.difficulty,9,{126,73,146,255});
    const SDL_Rect play=panel(.81f,.94f,selectable(progress,selector.selection)?
        SDL_Color{32,142,94,255}:SDL_Color{93,53,60,255});
    soloLabel(r,selectable(progress,selector.selection)?"JOUER":"VERROUILLE",
              play,white,pixels+1);
    soloLabel(r,"PROTOTYPE TEST",{left,viewport.y+int(viewport.h*.954f),
                                  wide,int(viewport.h*.035f)},
              SDL_Color{175,188,210,255},std::max(2,pixels-2));
}
} // namespace sfsolo
