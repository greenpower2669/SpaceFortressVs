// Standalone playable SOLO prototype; deliberately does not alter the historical
// duel/COOP loop. Desktop and Android integration can reuse its session/render.
#include "solo_renderer.hpp"
#include "solo_selector_ui.hpp"
#include "solo_campaign_controller.hpp"
#include "solo_combat.hpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>
#include <memory>

namespace sfsolo {
struct TouchPilot {
    SDL_FingerID finger=-1;
    float targetX=0,targetY=0;
    bool down=false;
    void handle(const SDL_Event &e,int width,int height){
        if(e.type==SDL_FINGERDOWN && !down){
            finger=e.tfinger.fingerId;down=true;
        }
        if((e.type==SDL_FINGERMOTION||e.type==SDL_FINGERDOWN) &&
           down && e.tfinger.fingerId==finger){
            targetX=std::clamp(e.tfinger.x,0.0f,1.0f)*width;
            targetY=std::clamp(e.tfinger.y,0.0f,1.0f)*height;
        }
        if(e.type==SDL_FINGERUP && down && e.tfinger.fingerId==finger){
            down=false;finger=-1;
        }
    }
    void reset(){finger=-1;down=false;}
};
inline void pilotInput(const Session &s,const TouchPilot &touch,
                       int width,int height,float &ax,float &ay){
    ax=ay=0;
    if(!touch.down||width<=0||height<=0)return;
    const float screenX=s.pilot.x/float(s.map.width)*width;
    // The pilot stays near the camera's lower-middle area.
    const float cell=float(width)/float(s.map.width);
    const float visibleRows=float(height)/cell;
    const float top=std::clamp(s.cameraY-visibleRows*.58f,0.0f,
                               std::max(0.0f,float(s.map.height)-visibleRows));
    const float screenY=(s.pilot.y-top)*cell;
    ax=std::clamp((touch.targetX-screenX)/std::max(1.0f,width*.18f),-1.0f,1.0f);
    ay=std::clamp((touch.targetY-screenY)/std::max(1.0f,height*.18f),-1.0f,1.0f);
}
} // namespace sfsolo

// Callable entry point for a future Android mode selector; no second SDL main.
// The historical Android entry point remains unchanged.
#ifdef SF_SOLO_STANDALONE
int runSoloPrototype(){
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS)!=0)return 1;
    SDL_Window *window=SDL_CreateWindow("SpaceFortress SOLO Prototype",
        SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,540,960,
        SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
    if(!window){SDL_Quit();return 2;}
    SDL_Renderer *renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    if(!renderer)renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_SOFTWARE);
    if(!renderer){SDL_DestroyWindow(window);SDL_Quit();return 3;}
    sfsolo::CampaignController campaign;
    sfsolo::Selector selector;
    bool selecting=true;
    sfsolo::TouchPilot touch;
    sfsolo::Combat combat;
    bool running=true;Uint64 previous=SDL_GetPerformanceCounter();
    const double frequency=double(SDL_GetPerformanceFrequency());
    while(running){
        SDL_Event e;
        int w=540,h=960;SDL_GetRendererOutputSize(renderer,&w,&h);
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT)running=false;
            if(e.type==SDL_KEYDOWN && e.key.keysym.sym==SDLK_ESCAPE){
                if(selecting)running=false;
                else {campaign.abandon();selecting=true;touch.reset();}
            }
            if(selecting){
                if(e.type==SDL_FINGERDOWN || e.type==SDL_MOUSEBUTTONDOWN){
                    const float tx=e.type==SDL_FINGERDOWN?e.tfinger.x:
                        float(e.button.x)/std::max(1,w);
                    const float ty=e.type==SDL_FINGERDOWN?e.tfinger.y:
                        float(e.button.y)/std::max(1,h);
                    auto action=selector.touch(tx,ty,campaign.progression);
                    if(action==sfsolo::SelectAction::Back)running=false;
                    if(action==sfsolo::SelectAction::Play){
                        campaign.selection=selector.selection;
                        if(campaign.launch()){selecting=false;touch.reset();combat=sfsolo::Combat{};}
                    }
                }
                if(e.type==SDL_KEYDOWN && e.key.keysym.sym==SDLK_RETURN){
                    campaign.selection=selector.selection;
                    if(campaign.launch()){selecting=false;touch.reset();combat=sfsolo::Combat{};}
                }
            }else{
                if(e.type==SDL_KEYDOWN && e.key.keysym.sym==SDLK_r){
                    campaign.abandon();
                    if(!campaign.launch())selecting=true;
                    combat=sfsolo::Combat{};touch.reset();
                }
                touch.handle(e,w,h);
            }
        }
        const Uint64 now=SDL_GetPerformanceCounter();
        const float dt=std::clamp(float(double(now-previous)/frequency),0.0f,.05f);
        previous=now;
        float ax=0,ay=0;
        if(!selecting && campaign.active)
            sfsolo::pilotInput(*campaign.active,touch,w,h,ax,ay);
        const Uint8 *keys=SDL_GetKeyboardState(nullptr);
        if(keys[SDL_SCANCODE_LEFT]||keys[SDL_SCANCODE_A])ax=-1;
        if(keys[SDL_SCANCODE_RIGHT]||keys[SDL_SCANCODE_D])ax=1;
        if(keys[SDL_SCANCODE_UP]||keys[SDL_SCANCODE_W])ay=-1;
        if(keys[SDL_SCANCODE_DOWN]||keys[SDL_SCANCODE_S])ay=1;
        if(selecting){
            sfsolo::drawSelector(renderer,{0,0,w,h},selector,campaign.progression);
        }else if(campaign.active){
            campaign.active->step(ax,ay,dt);
            // Temporary touch combat input for prototype validation only.
            // Final SOLO must reuse the canonical COOP kinetic controls.
            if(touch.down || keys[SDL_SCANCODE_SPACE] || keys[SDL_SCANCODE_LCTRL] ||
               (SDL_GetMouseState(nullptr,nullptr)&SDL_BUTTON(SDL_BUTTON_RIGHT)))
                combat.fire(*campaign.active,campaign.active->pilot.x,
                            campaign.active->pilot.y-10.0f);
            combat.step(*campaign.active,dt);
            sfsolo::drawSession(renderer,*campaign.active,{0,0,w,h});
            sfsolo::drawCombat(renderer,*campaign.active,combat,{0,0,w,h});
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }
    SDL_DestroyRenderer(renderer);SDL_DestroyWindow(window);SDL_Quit();
    return 0;
}
int main(int,char**){return runSoloPrototype();}
#endif
