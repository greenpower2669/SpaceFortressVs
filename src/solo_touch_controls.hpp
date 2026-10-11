#pragma once
// Pilot by dragging from the ship, aim anywhere else (tap = shot, hold = cone).
// A second finger can independently aim while the first steers. Android touch
// events are authoritative; synthesized mouse touch clicks are disabled.
#include "solo_session.hpp"
#include "solo_viewport.hpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>

namespace sfsolo {
enum class TouchAction { None, ChargePressed, ChargeReleased };
struct TouchPilot {
    SDL_FingerID finger=-1,chargeFinger=-1;
    float targetX=0,targetY=0,aimX=0,aimY=0;
    float startFingerX=0,startFingerY=0,anchorShipX=0,anchorShipY=0;
    bool hasWorldAnchor=false;
    bool down=false,chargeDown=false;
    // Latched steering: unlike a finite world-space destination, the vector
    // and throttle persist after release. Camera zones follow the SHIP.
    float cruiseX=0,cruiseY=0,cruiseThrottle=0;
    bool cruiseSet=false;
    void updateCruise(int width,int height){
        if(width<=0||height<=0)return;
        const float dx=targetX-startFingerX,dy=targetY-startFingerY;
        const float distance=std::hypot(dx,dy);
        // Near-zero drags must not erase an existing cruising direction.
        if(distance<12.0f)return;
        cruiseX=dx/distance;
        cruiseY=dy/distance;
        cruiseThrottle=std::clamp((distance-12.0f)/
            (float(std::min(width,height))*.30f),0.05f,1.0f);
        cruiseSet=true;
    }


    TouchAction handle(const SDL_Event &e,int width,int height,
                       float shipScreenX=-1,float shipScreenY=-1,
                       float shipWorldX=NAN,float shipWorldY=NAN){
        if(width<=0||height<=0)return TouchAction::None;
        if(e.type!=SDL_FINGERDOWN && e.type!=SDL_FINGERMOTION &&
           e.type!=SDL_FINGERUP)return TouchAction::None;
        const float sx=std::clamp(e.tfinger.x,0.0f,1.0f)*width;
        const float sy=std::clamp(e.tfinger.y,0.0f,1.0f)*height;
        const auto id=e.tfinger.fingerId;
        if(e.type==SDL_FINGERDOWN){
            // When no ship position is passed (legacy desktop tests), first
            // finger remains steering by default. In-game, the ship itself is
            // an accessible draggable joystick zone.
            const bool nearShip=(shipScreenX<0||shipScreenY<0) ||
                std::hypot(sx-shipScreenX,sy-shipScreenY)
                <=std::max(48.0f,float(width)*.17f);
            if(!down && nearShip){
                finger=id;down=true;targetX=sx;targetY=sy;
                startFingerX=sx;startFingerY=sy;
                hasWorldAnchor=std::isfinite(shipWorldX)&&std::isfinite(shipWorldY);
                if(hasWorldAnchor){anchorShipX=shipWorldX;anchorShipY=shipWorldY;}
                return TouchAction::None;
            }
            if(!chargeDown && id!=finger){
                chargeFinger=id;chargeDown=true;aimX=sx;aimY=sy;
                return TouchAction::ChargePressed;
            }
        }
        if(e.type==SDL_FINGERMOTION){
            if(down&&id==finger){targetX=sx;targetY=sy;
                updateCruise(width,height);}
            if(chargeDown&&id==chargeFinger){aimX=sx;aimY=sy;}
        }
        if(e.type==SDL_FINGERUP){
            if(chargeDown&&id==chargeFinger){
                aimX=sx;aimY=sy;
                chargeDown=false;chargeFinger=-1;
                return TouchAction::ChargeReleased;
            }
            if(down&&id==finger){
                targetX=sx;targetY=sy;
                updateCruise(width,height);
                down=false;finger=-1;hasWorldAnchor=false;
                // Keep direction and throttle after release, not finger zones.
            }
        }
        return TouchAction::None;
    }
    void reset(){finger=chargeFinger=-1;down=chargeDown=false;
                 hasWorldAnchor=false;
                 cruiseX=cruiseY=cruiseThrottle=0;
                 cruiseSet=false;}
};
inline void pilotTarget(const Session &s,const TouchPilot &touch,
                        int width,int height,float &tx,float &ty) {
    tx=s.pilot.x;ty=s.pilot.y;
    if(!touch.down||width<=0||height<=0)return;
    const SoloViewport camera=soloViewport(s,{0,0,width,height});
    if(touch.hasWorldAnchor){
        // Anchor the drag to the ship's WORLD position at finger-down.
        // Camera movement never changes the target when the finger is still.
        tx=touch.anchorShipX+(touch.targetX-touch.startFingerX)/camera.cell;
        ty=touch.anchorShipY+(touch.targetY-touch.startFingerY)/camera.cell;
    }else{
        // Legacy desktop/test fallback for events without a world anchor.
        tx=camera.left+touch.targetX/camera.cell;
        ty=camera.top+touch.targetY/camera.cell;
    }
    tx=std::clamp(tx,s.shipRadius,float(s.map.width)-s.shipRadius);
    ty=std::clamp(ty,s.shipRadius,float(s.map.height)-s.shipRadius);
}
inline void aimWorldPoint(const Session &s,float screenX,float screenY,
                         int width,int height,float &tx,float &ty) {
    const SoloViewport camera=soloViewport(s,{0,0,width,height});
    tx=camera.left+screenX/camera.cell;
    ty=camera.top+screenY/camera.cell;
}
// Preserved for regression callers. Desktop arrow/controller movement is
// separate; the real finger mode uses pilotTarget + canonical target motion.
inline void pilotInput(const Session &s,const TouchPilot &touch,
                       int width,int height,float &ax,float &ay){
    ax=ay=0;
    if(!touch.down||width<=0||height<=0)return;
    float tx=0,ty=0;
    pilotTarget(s,touch,width,height,tx,ty);
    ax=std::clamp((tx-s.pilot.x)*SOLO_REACTIVITY_MULTIPLIER,-1.0f,1.0f);
    ay=std::clamp((ty-s.pilot.y)*SOLO_REACTIVITY_MULTIPLIER,-1.0f,1.0f);
}
} // namespace sfsolo
