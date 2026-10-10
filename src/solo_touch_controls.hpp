#pragma once
// Isolated first-finger steering and second-finger charge input for SOLO.
// SDL touch-mouse synthesis is disabled in the prototype loop.
#include "solo_session.hpp"
#include "solo_viewport.hpp"
#include <SDL2/SDL.h>
#include <algorithm>

namespace sfsolo {
enum class TouchAction { None, ChargePressed, ChargeReleased };
struct TouchPilot {
    SDL_FingerID finger=-1,chargeFinger=-1;
    float targetX=0,targetY=0;
    bool down=false,chargeDown=false;
    TouchAction handle(const SDL_Event &e,int width,int height){
        if(e.type==SDL_FINGERDOWN){
            if(!down){
                finger=e.tfinger.fingerId;down=true;
            }else if(!chargeDown && e.tfinger.fingerId!=finger){
                chargeFinger=e.tfinger.fingerId;
                chargeDown=true;
                return TouchAction::ChargePressed;
            }
        }
        if((e.type==SDL_FINGERMOTION || e.type==SDL_FINGERDOWN) &&
           down && e.tfinger.fingerId==finger){
            targetX=std::clamp(e.tfinger.x,0.0f,1.0f)*width;
            targetY=std::clamp(e.tfinger.y,0.0f,1.0f)*height;
        }
        if(e.type==SDL_FINGERUP){
            if(chargeDown && e.tfinger.fingerId==chargeFinger){
                chargeDown=false;chargeFinger=-1;
                return TouchAction::ChargeReleased;
            }
            if(down && e.tfinger.fingerId==finger){
                down=false;finger=-1;
            }
        }
        return TouchAction::None;
    }
    void reset(){finger=chargeFinger=-1;down=chargeDown=false;}
};
inline void pilotInput(const Session &s,const TouchPilot &touch,
                       int width,int height,float &ax,float &ay){
    ax=ay=0;
    if(!touch.down||width<=0||height<=0)return;
    // Identical camera to the renderer: touch tracking must not aim at
    // the former 54-column overview while the view is zoomed to 18 columns.
    const SoloViewport camera=soloViewport(s,{0,0,width,height});
    const float screenX=camera.x(s.pilot.x);
    const float screenY=camera.y(s.pilot.y);
    ax=std::clamp((touch.targetX-screenX)/std::max(1.0f,width*.18f),-1.0f,1.0f);
    ay=std::clamp((touch.targetY-screenY)/std::max(1.0f,height*.18f),-1.0f,1.0f);
}
} // namespace sfsolo
