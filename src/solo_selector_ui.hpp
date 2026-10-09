#pragma once
// SDL2 touch-friendly SOLO world and stage selector, independent of legacy menus.
#include "solo_stage_selection.hpp"
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
    auto bar=[&](float top,float bottom,SDL_Color color){
        SDL_Rect rect{viewport.x+viewport.w/10,viewport.y+int(viewport.h*top),
                      viewport.w*8/10,std::max(1,int(viewport.h*(bottom-top)))};
        selectorBar(r,rect,color);
    };
    bar(.02f,.13f,{75,90,128,255}); // back
    bar(.20f,.34f,{65,120,185,255}); // world
    bar(.39f,.54f,{80,155,145,255}); // stage
    bar(.59f,.73f,{145,95,175,255}); // difficulty
    bar(.79f,.97f,selectable(progress,selector.selection)?
        SDL_Color{50,190,120,255}:SDL_Color{100,65,65,255}); // play
    // Graphic counters: six world slots, twenty stage slots, nine danger slots.
    auto dots=[&](unsigned current,unsigned count,float top){
        const int dotW=std::max(2,(viewport.w*7/10)/int(count*2));
        for(unsigned i=0;i<count;i++){
            SDL_Rect d{viewport.x+viewport.w*15/100+
                int((i+.5f)*viewport.w*.7f/count)-dotW/2,
                viewport.y+int(viewport.h*top),dotW,std::max(4,viewport.h/65)};
            selectorBar(r,d,i+1==current?SDL_Color{255,245,170,255}:
                SDL_Color{25,40,65,255});
        }
    };
    dots(selector.selection.world,6,.27f);
    dots(selector.selection.stage,20,.46f);
    dots(selector.selection.difficulty,9,.66f);
}
} // namespace sfsolo
