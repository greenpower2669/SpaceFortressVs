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
// Rectangles, not circles: centre 1/3..2/3 is still, then TWO independently
// progressive bands toward every screen edge. Future round/circular masks can
// replace this weighting function without touching physics or saved levels.
inline constexpr float SOLO_RECT_ZONE2_START=1.0f/6.0f;
// Final perimeter includes the ship's unchanged graphical radius: reach
// full speed BEFORE touching/clipping the screen edge.
inline constexpr float SOLO_RECT_MAX_SPEED_MARGIN=.06f;
inline constexpr float SOLO_RECT_ZONE1_END_SPEED=.55f;
inline constexpr float SOLO_CAMERA_RESPONSE_HZ=10.0f;
inline float soloSmoothstep(float t){
    t=std::clamp(t,0.0f,1.0f);
    return t*t*(3.0f-2.0f*t);
}
inline float soloRectangularCameraGain(float normalizedPosition){
    if(!std::isfinite(normalizedPosition))return 0.0f;
    const float edge=std::min(normalizedPosition,1.0f-normalizedPosition);
    // Zone centrale: >= 1/3 distance from every edge.
    if(edge>=SOLO_DEADZONE_THIRD)return 0.0f;
    // FIRST rectangle: edge between 1/3 and 1/6, speed 0 -> 55%.
    if(edge>=SOLO_RECT_ZONE2_START){
        const float t=(SOLO_DEADZONE_THIRD-edge)/
                      (SOLO_DEADZONE_THIRD-SOLO_RECT_ZONE2_START);
        return SOLO_RECT_ZONE1_END_SPEED*soloSmoothstep(t);
    }
    // SECOND rectangle: edge 1/6 -> 6% of screen, speed 55 -> 100%.
    // The last 6% is full-speed cruising, keeping the ship fully visible.
    const float t=(SOLO_RECT_ZONE2_START-edge)/
                  (SOLO_RECT_ZONE2_START-SOLO_RECT_MAX_SPEED_MARGIN);
    return SOLO_RECT_ZONE1_END_SPEED+
           (1.0f-SOLO_RECT_ZONE1_END_SPEED)*soloSmoothstep(t);
}

// Screen-zone camera law: no dependence on ship velocity. One stationary
// fingertip in either exterior band therefore keeps the terrain scrolling.
inline float soloFingerZonePanSpeed(float normalized,float columns){
    if(!std::isfinite(normalized)||columns<=0)return 0.0f;
    const float gain=soloRectangularCameraGain(normalized);
    if(normalized<SOLO_DEADZONE_THIRD)return -columns*1.90f*gain;
    if(normalized>1.0f-SOLO_DEADZONE_THIRD)return columns*1.90f*gain;
    return 0.0f;
}
// Circular SOLO camera profile, radius computed in SCREEN PIXELS.
// Outer circle is inside the narrow side by an 8% margin. The other circles
// have radii R/2 and 3R/4. After the outer circle speed stays at exactly 6.
inline float soloCircleOuterRadius(SDL_Rect screen){
    return std::max(0.0f,float(std::min(screen.w,screen.h))*.42f);
}
inline float soloCircleZoneSpeed(float shipScreenX,float shipScreenY,SDL_Rect screen){
    const float outer=soloCircleOuterRadius(screen);
    if(outer<=0||!std::isfinite(shipScreenX)||!std::isfinite(shipScreenY))return 0;
    const float dx=shipScreenX-(screen.x+screen.w*.5f);
    const float dy=shipScreenY-(screen.y+screen.h*.5f);
    const float distance=std::hypot(dx,dy);
    const float inner=outer*.5f,middle=outer*.75f;
    if(distance<=inner+.001f)return 0.0f;
    if(distance>=outer)return 6.0f;
    if(distance<=middle+.001f){
        const float t=soloSmoothstep((distance-inner)/(middle-inner));
        return .1f+1.9f*t;
    }
    const float t=soloSmoothstep((distance-middle)/(outer-middle));
    return 2.1f+3.9f*t;
}
// WORLD speed / s, radial vector from screen centre. No fictitious joystick
// direction and no dependence on the pilot's instantaneous velocity.
struct SoloCirclePan {float vx=0,vy=0,speed=0;};
inline SoloCirclePan soloCirclePan(float screenShipX,float screenShipY,
                                  SDL_Rect screen,float visibleWorldColumns){
    const float speed=soloCircleZoneSpeed(screenShipX,screenShipY,screen);
    const float dx=screenShipX-(screen.x+screen.w*.5f);
    const float dy=screenShipY-(screen.y+screen.h*.5f);
    const float r=std::hypot(dx,dy);
    if(speed<=0||r<=.0001f||visibleWorldColumns<=0)return {};
    const float unitsPerSecond=visibleWorldColumns*.72f*speed;
    return {dx/r*unitsPerSecond,dy/r*unitsPerSecond,speed};
}
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
// A ship controls camera scrolling by its ACTUAL position inside the outer
// rectangular bands. A finger release does not change this rule. Only the
// ship's world velocity determines the pan direction, never the finger zone.
inline float soloShipCameraTarget(float normalizedShipPosition,float shipVelocity){
    if(!std::isfinite(normalizedShipPosition)||!std::isfinite(shipVelocity))
        return 0.0f;
    // Flying back toward the centre must not drag the camera in reverse while
    // the ship is still in the old outer zone. This also ensures a stable
    // rest state in the whole central third of the screen.
    if((normalizedShipPosition<=.5f && shipVelocity>=0.0f) ||
       (normalizedShipPosition>=.5f && shipVelocity<=0.0f))
        return 0.0f;
    return shipVelocity*soloRectangularCameraGain(normalizedShipPosition);
}
inline void soloUpdateCamera(Session &s,SDL_Rect screen,float dt=0.0f){
    if(!s.map.valid() || screen.w<=0 || screen.h<=0)return;
    const float cell=soloMapCellPixels(screen);
    const float cols=screen.w/cell,rows=screen.h/cell;
    if(!s.viewReady || s.viewWidth!=screen.w || s.viewHeight!=screen.h){
        const auto initial=soloViewport(s,screen);
        s.viewLeft=initial.left;s.viewTop=initial.top;
        s.viewVelocityX=0;s.viewVelocityY=0;
        s.viewReady=true;s.viewWidth=screen.w;s.viewHeight=screen.h;
    }
    // dt=0 is initialization only; all code paths in the frame use the same
    // screen-to-world camera transform.
    if(dt<=0 || !std::isfinite(dt))return;
    dt=std::min(dt,.05f);
    const float screenX=(s.pilot.x-s.viewLeft)/cols;
    const float screenY=(s.pilot.y-s.viewTop)/rows;
    const float desiredX=soloShipCameraTarget(screenX,s.pilot.vx);
    const float desiredY=soloShipCameraTarget(screenY,s.pilot.vy);
    const float response=1.0f-std::exp(-SOLO_CAMERA_RESPONSE_HZ*dt);
    // HARD STOP when the ship returns to the centre (or reverses inward).
    // Easing a previous velocity through the dead zone would still visibly
    // scroll the world and violates the agreed one-third rule.
    if(desiredX==0.0f)s.viewVelocityX=0.0f;
    else s.viewVelocityX+=(desiredX-s.viewVelocityX)*response;
    if(desiredY==0.0f)s.viewVelocityY=0.0f;
    else s.viewVelocityY+=(desiredY-s.viewVelocityY)*response;
    if(std::abs(s.viewVelocityX)<.0005f)s.viewVelocityX=0;
    if(std::abs(s.viewVelocityY)<.0005f)s.viewVelocityY=0;

    const float nextLeft=soloClampCamera(s.viewLeft+s.viewVelocityX*dt,
                                        float(s.map.width),cols);
    const float nextTop=soloClampCamera(s.viewTop+s.viewVelocityY*dt,
                                       float(s.map.height),rows);
    if(std::abs(nextLeft-(s.viewLeft+s.viewVelocityX*dt))>.0001f)
        s.viewVelocityX=0;
    if(std::abs(nextTop-(s.viewTop+s.viewVelocityY*dt))>.0001f)
        s.viewVelocityY=0;
    s.viewLeft=nextLeft;s.viewTop=nextTop;

    // Guard against dropped frames and map edges. Keep the fixed-size ship
    // visible; no camera input is ever fed into the physics integrator.
    constexpr float margin=SOLO_RECT_MAX_SPEED_MARGIN;
    const float minX=s.viewLeft+cols*margin;
    const float maxX=s.viewLeft+cols*(1.0f-margin);
    const float minY=s.viewTop+rows*margin;
    const float maxY=s.viewTop+rows*(1.0f-margin);
    if(s.pilot.x<minX)s.viewLeft=soloClampCamera(
        s.pilot.x-cols*margin,float(s.map.width),cols);
    else if(s.pilot.x>maxX)s.viewLeft=soloClampCamera(
        s.pilot.x-cols*(1.0f-margin),float(s.map.width),cols);
    if(s.pilot.y<minY)s.viewTop=soloClampCamera(
        s.pilot.y-rows*margin,float(s.map.height),rows);
    else if(s.pilot.y>maxY)s.viewTop=soloClampCamera(
        s.pilot.y-rows*(1.0f-margin),float(s.map.height),rows);
}

} // namespace sfsolo
