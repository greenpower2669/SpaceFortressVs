#pragma once
#include <SDL2/SDL.h>

namespace sfsolo {
// SOLO-028: render-only overlay after the single human pilot loses.
// Returning exits the child SOLO Activity; Android exposes the still-open
// historical main menu. No changes to canonical COOP defeat or save logic.
struct GameOverUi {
    static SDL_Rect panel(int w,int h){
        return {int(w*.065f),int(h*.32f),int(w*.87f),int(h*.37f)};
    }
    static SDL_Rect menuButton(int w,int h){
        return {int(w*.12f),int(h*.545f),int(w*.76f),int(h*.10f)};
    }
    static bool hitMenu(float x,float y,int w,int h){
        if(w<=0||h<=0)return false;
        const auto button=menuButton(w,h);
        return x>=button.x && x<button.x+button.w &&
               y>=button.y && y<button.y+button.h;
    }
};
} // namespace sfsolo
