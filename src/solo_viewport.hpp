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
inline void soloUpdateCamera(Session &s,SDL_Rect screen,float dt=0.0f,
                             float fingerZoneX=NAN,float fingerZoneY=NAN){
    if(!s.map.valid() || screen.w<=0 || screen.h<=0)return;
    const float cell=soloMapCellPixels(screen);
    const float cols=screen.w/cell,rows=screen.h/cell;
    if(!s.viewReady || s.viewWidth!=screen.w || s.viewHeight!=screen.h){
        const auto initial=soloViewport(s,screen);
        s.viewLeft=initial.left;s.viewTop=initial.top;
        s.viewVelocityX=0;s.viewVelocityY=0;
        s.viewReady=true;s.viewWidth=screen.w;s.viewHeight=screen.h;
    }
    // The initial event-loop call (dt=0) just initializes the camera, so
    // rendering, touch projection and keyboard input see one stable transform.
    if(dt<=0 || !std::isfinite(dt))return;
    dt=std::min(dt,.05f);

    const float screenX=(s.pilot.x-s.viewLeft)/cols;
    const float screenY=(s.pilot.y-s.viewTop)/rows;
    // Real game: camera rate follows the FINGER ZONE, not the current screen
    // position of a ship that the camera itself has displaced.
    // The controls retain fingerZoneX/Y after release, so scrolling persists
    // until a later steering gesture changes the direction/speed.
    const bool fingerDrive=std::isfinite(fingerZoneX)&&
                           std::isfinite(fingerZoneY);
    const float gainX=soloRectangularCameraGain(
        fingerDrive?fingerZoneX:screenX);
    const float gainY=soloRectangularCameraGain(
        fingerDrive?fingerZoneY:screenY);
    // Keep older ship-relative behavior for keyboard and camera-only tests.
    const float desiredX=(!fingerDrive &&
                          s.pilot.vx*(screenX-.5f)<=0.0f)?
                          0.0f:s.pilot.vx*gainX;
    const float desiredY=(!fingerDrive &&
                          s.pilot.vy*(screenY-.5f)<=0.0f)?
                          0.0f:s.pilot.vy*gainY;
    const float response=1.0f-std::exp(-SOLO_CAMERA_RESPONSE_HZ*dt);
    s.viewVelocityX+=(desiredX-s.viewVelocityX)*response;
    s.viewVelocityY+=(desiredY-s.viewVelocityY)*response;
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

    // Guard against very fast keyboard movement / dropped frames. Never
    // hide the ship beyond the viewport when the following acceleration lags.
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
