#pragma once
// Shared SOLO camera transform used by the renderer, input and projectiles.
// The original prototype squeezed all 54 columns into the phone width and
// clamped the start ship against the bottom of the screen (hidden by Android
// navigation). Keep a local 18-cell-wide play view; the minimap stays global.
#include "solo_session.hpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>

namespace sfsolo {
struct SoloViewport {
    SDL_Rect screen{};
    float cell=1.0f,left=0.0f,top=0.0f;
    float visibleColumns() const {return screen.w/cell;}
    float visibleRows() const {return screen.h/cell;}
    float x(float worldX) const {return screen.x+(worldX-left)*cell;}
    float y(float worldY) const {return screen.y+(worldY-top)*cell;}
    int pixelX(float worldX) const {return int(std::lround(x(worldX)));}
    int pixelY(float worldY) const {return int(std::lround(y(worldY)));}
};
inline SoloViewport soloViewport(const Session &s,SDL_Rect screen) {
    SoloViewport view;
    view.screen=screen;
    if(screen.w<=0 || screen.h<=0 || !s.map.valid())return view;
    // One screen now contains around 18 cells horizontally instead of 54.
    view.cell=std::max(1.0f,float(screen.w)/18.0f);
    const float cols=view.visibleColumns(),rows=view.visibleRows();
    view.left=std::clamp(s.pilot.x-cols*.50f,0.0f,
                         std::max(0.0f,float(s.map.width)-cols));
    // Follow the ship at about two-thirds of screen height. Allow a small
    // out-of-world margin at start/end rather than pinning the ship below the
    // Android navigation bar when starting near the bottom of the map.
    const float margin=rows*.18f;
    view.top=std::clamp(s.cameraY-rows*.65f,-margin,
                        std::max(-margin,float(s.map.height)-rows+margin));
    return view;
}
} // namespace sfsolo
