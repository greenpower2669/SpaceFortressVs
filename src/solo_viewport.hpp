#pragma once
// SOLO presentation camera. Rendering scale is independent of ship size,
// physics, collision grid, original modes and exploration map dimensions.
// Freeze the view inside its central third and pan only when the ship crosses
// the one-third screen boundary in any of the four directions.
#include "solo_session.hpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>

namespace sfsolo {
inline constexpr float SOLO_MAP_ZOOM=3.0f;
inline constexpr float SOLO_REFERENCE_COLUMNS=18.0f;
inline constexpr float SOLO_DEADZONE_THIRD=1.0f/3.0f;
inline float soloMapCellPixels(SDL_Rect screen) {
    return screen.w>0?std::max(1.0f,float(screen.w)/
                  (SOLO_REFERENCE_COLUMNS/SOLO_MAP_ZOOM)):1.0f;
}
// This is the exact graphic radius used before zoom x3. Physical collision
// radius remains Session::shipRadius in world coordinates.
inline int soloShipRadiusPixels(SDL_Rect screen) {
    return std::max(17,int((float(screen.w)/SOLO_REFERENCE_COLUMNS)*.90f));
}
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
inline float soloClampCamera(float offset,float worldLength,float visible){
    // Blank sky near the edges is intentional: keep the ship visible even
    // when arriving at a world boundary, without jumping the camera.
    return std::clamp(offset,-visible*.5f,worldLength-visible*.5f);
}
inline SoloViewport soloViewport(const Session &s,SDL_Rect screen) {
    SoloViewport view;
    view.screen=screen;
    if(screen.w<=0 || screen.h<=0 || !s.map.valid())return view;
    view.cell=soloMapCellPixels(screen);
    const float cols=view.visibleColumns(),rows=view.visibleRows();
    if(s.viewReady && s.viewWidth==screen.w && s.viewHeight==screen.h){
        view.left=s.viewLeft;
        view.top=s.viewTop;
    }else{
        // Start of a new level: the ship is near the centre, never hidden by
        // Android status/navigation bars. No old camera history is reused.
        view.left=soloClampCamera(s.pilot.x-cols*.5f,float(s.map.width),cols);
        view.top=soloClampCamera(s.pilot.y-rows*.5f,float(s.map.height),rows);
    }
    return view;
}
inline void soloUpdateCamera(Session &s,SDL_Rect screen){
    if(!s.map.valid() || screen.w<=0 || screen.h<=0)return;
    const float cell=soloMapCellPixels(screen);
    const float cols=screen.w/cell,rows=screen.h/cell;
    if(!s.viewReady || s.viewWidth!=screen.w || s.viewHeight!=screen.h){
        const auto initial=soloViewport(s,screen);
        s.viewLeft=initial.left;s.viewTop=initial.top;
        s.viewReady=true;s.viewWidth=screen.w;s.viewHeight=screen.h;
    }
    const float lowX=s.viewLeft+cols*SOLO_DEADZONE_THIRD;
    const float highX=s.viewLeft+cols*(1.0f-SOLO_DEADZONE_THIRD);
    const float lowY=s.viewTop+rows*SOLO_DEADZONE_THIRD;
    const float highY=s.viewTop+rows*(1.0f-SOLO_DEADZONE_THIRD);
    // Axis-independent, pixel-exact, no damping or lag: crossing an edge
    // scrolls the WORLD, never modifies the pilot's motion.
    if(s.pilot.x<lowX)
        s.viewLeft=s.pilot.x-cols*SOLO_DEADZONE_THIRD;
    else if(s.pilot.x>highX)
        s.viewLeft=s.pilot.x-cols*(1.0f-SOLO_DEADZONE_THIRD);
    if(s.pilot.y<lowY)
        s.viewTop=s.pilot.y-rows*SOLO_DEADZONE_THIRD;
    else if(s.pilot.y>highY)
        s.viewTop=s.pilot.y-rows*(1.0f-SOLO_DEADZONE_THIRD);
    s.viewLeft=soloClampCamera(s.viewLeft,float(s.map.width),cols);
    s.viewTop=soloClampCamera(s.viewTop,float(s.map.height),rows);
}
} // namespace sfsolo
