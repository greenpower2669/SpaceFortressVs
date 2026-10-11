#pragma once
// SOLO: the world scrolls underneath a ship locked to the first fingertip.
// No CLASSIC/COOP code, global state or weapon rules are modified.
#include "solo_touch_controls.hpp"
#include "solo_viewport.hpp"
#include <algorithm>
#include <cmath>

namespace sfsolo {
inline void soloNavigateFinger(Session &s,const TouchPilot &touch,
                               SDL_Rect screen,float dt){
    if(!s.map.valid()||screen.w<=0||screen.h<=0||!touch.steeringEngaged)
        return;
    if(s.phase!=Phase::Flying&&s.phase!=Phase::BossFight)return;
    soloUpdateCamera(s,screen); // initialize transforms only (dt=0)
    if(!std::isfinite(dt)||dt<=0)return;
    dt=std::min(dt,.05f);
    const auto camera=soloViewport(s,screen);
    const float cols=camera.visibleColumns(),rows=camera.visibleRows();
    if(cols<=0||rows<=0)return;
    const float margin=SOLO_RECT_MAX_SPEED_MARGIN;
    const float fingerX=std::clamp(touch.targetX/float(screen.w),margin,1.0f-margin);
    const float fingerY=std::clamp(touch.targetY/float(screen.h),margin,1.0f-margin);

    // Circle centred on the visible SCREEN, not a pair of rectangles.
    // Radial speed stays constant at 6 outside the biggest circle.
    const float screenShipX=camera.x(s.pilot.x);
    const float screenShipY=camera.y(s.pilot.y);
    const auto pan=soloCirclePan(screenShipX,screenShipY,screen,cols);
    const float requestedX=pan.vx*dt;
    const float requestedY=pan.vy*dt;
    const float nextLeft=soloClampCamera(camera.left+requestedX,float(s.map.width),cols);
    const float nextTop=soloClampCamera(camera.top+requestedY,float(s.map.height),rows);
    const float panX=nextLeft-camera.left,panY=nextTop-camera.top;

    // A fixed screen finger means a moving WORLD target during camera pan.
    // Physics uses shared bounded touch motion and existing swept collisions.
    const float oldX=s.pilot.x,oldY=s.pilot.y;
    const float goalX=nextLeft+fingerX*cols,goalY=nextTop+fingerY*rows;
    // Extra WORLD speed compensates for camera pan only; the ship stays
    // finger-locked at the same visual size. Canonical touch vector still
    // bounds every frame and the collision sweep remains authoritative.
    constexpr float COOP_SPEED_FACTOR=3.10f/1.90f;
    const float panFactor=pan.speed*.72f/1.90f;
    s.stepToward(goalX,goalY,dt,cols*(COOP_SPEED_FACTOR+panFactor));

    // Prevent "camera tunnelling": if rock blocks ship translation, an
    // autonomous pan cannot drag that ship through a world obstacle.
    const float dx=s.pilot.x-oldX,dy=s.pilot.y-oldY;
    const float actualPanX=panX>0?std::min(panX,std::max(0.0f,dx)):
                           panX<0?std::max(panX,std::min(0.0f,dx)):0.0f;
    const float actualPanY=panY>0?std::min(panY,std::max(0.0f,dy)):
                           panY<0?std::max(panY,std::min(0.0f,dy)):0.0f;
    s.viewLeft=soloClampCamera(camera.left+actualPanX,float(s.map.width),cols);
    s.viewTop=soloClampCamera(camera.top+actualPanY,float(s.map.height),rows);
    s.viewVelocityX=(s.viewLeft-camera.left)/dt;
    s.viewVelocityY=(s.viewTop-camera.top)/dt;
}
} // namespace sfsolo
