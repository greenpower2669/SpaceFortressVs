// Standalone playable SOLO prototype; deliberately does not alter the historical
// duel/COOP loop. Desktop and Android integration can reuse its session/render.
#include "solo_renderer.hpp"
#include "solo_selector_ui.hpp"
#include "solo_campaign_controller.hpp"
#include "solo_campaign_persistence.hpp"
#include "solo_combat.hpp"
#include "solo_kinetic_control.hpp"
#include "solo_touch_controls.hpp"
#include "solo_prototype.hpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>
#include <memory>
#include <string>



// Callable prototype entry point. The standalone main is optional so that
// an Android mode dispatcher can link this translation unit without a duplicate main.
int runSoloPrototype(){
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS,"0"); // prevent duplicate touch mouse clicks
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS)!=0)return 1;
    SDL_Window *window=SDL_CreateWindow("SpaceFortress SOLO Prototype",
        SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,540,960,
        SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
    if(!window){SDL_Quit();return 2;}
    SDL_Renderer *renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    if(!renderer)renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_SOFTWARE);
    if(!renderer){SDL_DestroyWindow(window);SDL_Quit();return 3;}
    sfsolo::CampaignController campaign;
    // SDL supplies the app-private writable directory on Android and desktop.
    // SOLO owns its save file; never touch historical DUEL/COOP saves.
    std::string soloSavePath;
    if(char *pref=SDL_GetPrefPath("greenpower2669","SpaceFortressVs")){
        soloSavePath=std::string(pref)+"solo-progress-v1.save";
        SDL_free(pref);
    }
    sfsolo::restoreCampaign(campaign,soloSavePath);
    sfsolo::Selector selector;
    bool selecting=true;
    sfsolo::TouchPilot touch;
    sfsolo::KineticSurgeControl kinetic;
    sfsolo::Combat combat;
    const int historicalDanger=sfBossDangerIndex;
    bool running=true;Uint64 previous=SDL_GetPerformanceCounter();
    const double frequency=double(SDL_GetPerformanceFrequency());
    while(running){
        SDL_Event e;
        int w=540,h=960;SDL_GetRendererOutputSize(renderer,&w,&h);
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT)running=false;
            if(e.type==SDL_KEYDOWN && e.key.keysym.sym==SDLK_ESCAPE){
                if(selecting)running=false;
                else {campaign.abandon();selecting=true;touch.reset();kinetic.cancel();}
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
                        if(campaign.launch()){selecting=false;touch.reset();kinetic.cancel();sfBossDangerIndex=int(campaign.launchedSelection.difficulty)-1;combat=sfsolo::Combat{};}
                    }
                }
                if(e.type==SDL_KEYDOWN && e.key.keysym.sym==SDLK_RETURN){
                    campaign.selection=selector.selection;
                    if(campaign.launch()){selecting=false;touch.reset();kinetic.cancel();sfBossDangerIndex=int(campaign.launchedSelection.difficulty)-1;combat=sfsolo::Combat{};}
                }
            }else{
                if(e.type==SDL_KEYDOWN && e.key.keysym.sym==SDLK_r){
                    campaign.abandon();
                    if(!campaign.launch())selecting=true;
                    combat=sfsolo::Combat{};touch.reset();kinetic.cancel();
                }
                const auto action=touch.handle(e,w,h);
                if(action==sfsolo::TouchAction::ChargePressed)kinetic.press();
                if(action==sfsolo::TouchAction::ChargeReleased){
                    const bool surge=campaign.active && kinetic.release(*campaign.active);
                    if(!surge && campaign.active)
                        combat.fire(*campaign.active,campaign.active->pilot.x,
                                    campaign.active->pilot.y-10.0f);
                }
                // Desktop test controls mirror the second finger: hold SPACE
                // to arm the canonical surge, release early for a single shot.
                if(e.type==SDL_KEYDOWN && !e.key.repeat &&
                   e.key.keysym.sym==SDLK_SPACE)kinetic.press();
                if(e.type==SDL_KEYUP && e.key.keysym.sym==SDLK_SPACE){
                    const bool surge=campaign.active && kinetic.release(*campaign.active);
                    if(!surge && campaign.active)
                        combat.fire(*campaign.active,campaign.active->pilot.x,
                                    campaign.active->pilot.y-10.0f);
                }
                if(e.type==SDL_MOUSEBUTTONDOWN && e.button.button==SDL_BUTTON_RIGHT)
                    kinetic.press();
                if(e.type==SDL_MOUSEBUTTONUP && e.button.button==SDL_BUTTON_RIGHT){
                    const bool surge=campaign.active && kinetic.release(*campaign.active);
                    if(!surge && campaign.active)
                        combat.fire(*campaign.active,campaign.active->pilot.x,
                                    campaign.active->pilot.y-10.0f);
                }
                if(e.type==SDL_APP_WILLENTERBACKGROUND){
                    touch.reset();kinetic.cancel();
                }
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
            kinetic.advance(dt);
            if(campaign.active->phase==sfsolo::Phase::Lost)kinetic.cancel();
            // Prototype projectiles remain temporary. Touch 1 steers only;
            // touch 2 / SPACE charges COOP's real kinetic state machine.
            combat.step(*campaign.active,dt);
            if(campaign.active->phase==sfsolo::Phase::Won){
                // Completion is atomic: update SOLO progression only after a
                // successful persistent save, then unlock the next stage.
                const auto &finished=*campaign.active;
                sfsolo::ScoreInput sc;
                sc.enemiesDefeated=finished.enemiesDefeated;
                sc.enemiesAvailable=finished.enemiesAvailable;
                sc.elapsedSeconds=finished.seconds;
                sc.damageTaken=finished.pilot.damageTaken;
                sc.reachedFinish=finished.reachedFinish;
                sc.bossDefeated=finished.bossDefeated;
                sc.alive=finished.pilot.health>0;
                sc.difficultyMultiplier=1.0f+.1f*
                    float(campaign.launchedSelection.difficulty-1);
                const int points=sfsolo::score(sc);
                const std::string id="solo-local-"+
                    std::to_string(static_cast<unsigned long long>(
                        SDL_GetPerformanceCounter()));
                if(sfsolo::finishAndSave(campaign,soloSavePath,
                                          "SoloPilot",id,points)){
                    campaign.next();
                    selector.selection=campaign.selection;
                    selecting=true;
                    touch.reset();
                    kinetic.cancel();
                    combat=sfsolo::Combat{};
                }else{
                    // Keep the completed session available for a later retry
                    // rather than unlock a stage that was never persisted.
                    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                                "SOLO progression not saved; session retained.");
                }
            }
            if(campaign.active){
                sfsolo::drawSession(renderer,*campaign.active,{0,0,w,h});
                sfsolo::drawCombat(renderer,*campaign.active,combat,{0,0,w,h});
                sfsolo::drawKineticCharge(renderer,*campaign.active,{0,0,w,h});
            }else{
                // A successful victory consumes the session; show the selector
                // instead of dereferencing an already completed run.
                sfsolo::drawSelector(renderer,{0,0,w,h},
                                     selector,campaign.progression);
            }
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }
    kinetic.cancel();sfBossDangerIndex=historicalDanger;
    SDL_DestroyRenderer(renderer);SDL_DestroyWindow(window);SDL_Quit();
    return 0;
}
#ifdef SF_SOLO_STANDALONE
int main(int,char**){return runSoloPrototype();}
#endif
