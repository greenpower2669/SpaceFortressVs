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
#include "solo_android_diagnostics.hpp"
#include "solo_audio.hpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <memory>
#include <string>



// Callable prototype entry point. The standalone main is optional so that
// an Android mode dispatcher can link this translation unit without a duplicate main.
int runSoloPrototype(){
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS,"0"); // prevent duplicate touch mouse clicks
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS)!=0){
        sfDebugNativeLog("solo",("SDL_INIT_FAILED "+
            std::string(SDL_GetError())).c_str());
        return 1;
    }
    sfDebugNativeLog("solo","SDL_INIT_OK");
    SDL_Window *window=SDL_CreateWindow("SpaceFortress SOLO Prototype",
        SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,540,960,
        SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
    if(!window){
        sfDebugNativeLog("solo",("SDL_WINDOW_FAILED "+
            std::string(SDL_GetError())).c_str());
        SDL_Quit();return 2;
    }
    sfDebugNativeLog("solo","SDL_WINDOW_OK");
    SDL_Renderer *renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    if(!renderer)renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_SOFTWARE);
    if(!renderer){
        sfDebugNativeLog("solo",("SDL_RENDERER_FAILED "+
            std::string(SDL_GetError())).c_str());
        SDL_DestroyWindow(window);SDL_Quit();return 3;
    }
    sfDebugNativeLog("solo","SDL_RENDERER_OK");
    IMG_Init(IMG_INIT_PNG);
    auto artwork=std::make_unique<sfsolo::SoloAssets>(renderer);
    auto audio=std::make_unique<sfsolo::SoloSounds>();
    sfDebugNativeLog("solo",audio->opened?"AUDIO_MIXER_READY":
                                      "AUDIO_FALLBACK_SILENT");
    sfDebugNativeLog("solo",artwork->ship?"ORIGINAL_SHIP_ASSET_OK":
                                             "SHIP_ASSET_FALLBACK");
    sfsolo::CampaignController campaign;
    // SDL supplies the app-private writable directory on Android and desktop.
    // SOLO owns its save file; never touch historical DUEL/COOP saves.
    std::string soloSavePath;
    if(char *pref=SDL_GetPrefPath("greenpower2669","SpaceFortressVs")){
        soloSavePath=std::string(pref)+"solo-progress-v1.save";
        SDL_free(pref);
    }
    const bool restored=sfsolo::restoreCampaign(campaign,soloSavePath);
    sfDebugNativeLog("solo",restored?"SAVE_RESTORED":"SAVE_EMPTY_OR_FAILED");
    sfsolo::Selector selector;
    bool selecting=true;
    sfsolo::TouchPilot touch;
    sfsolo::KineticSurgeControl kinetic;
    sfsolo::Combat combat;
    const int historicalDanger=sfBossDangerIndex;
    bool running=true;Uint64 previous=SDL_GetPerformanceCounter();
    Uint64 lastDebugHeartbeat=SDL_GetTicks64();
    sfDebugNativeLog("solo","SOLO_LOOP_ENTER");
    const double frequency=double(SDL_GetPerformanceFrequency());
    while(running){
        SDL_Event e;
        int w=540,h=960;SDL_GetRendererOutputSize(renderer,&w,&h);
        if(!selecting && campaign.active)
            sfsolo::soloUpdateCamera(*campaign.active,{0,0,w,h});
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT){
                sfDebugNativeLog("solo","SDL_QUIT_EVENT");running=false;
            }
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
                        if(campaign.launch()){
                            sfDebugNativeLog("solo","STAGE_LAUNCHED_TOUCH");
                            selecting=false;touch.reset();kinetic.cancel();
                            sfBossDangerIndex=int(campaign.launchedSelection.difficulty)-1;
                            combat=sfsolo::Combat{};
                        }else sfDebugNativeLog("solo","STAGE_LAUNCH_FAILED");
                    }
                }
                if(e.type==SDL_KEYDOWN && e.key.keysym.sym==SDLK_RETURN){
                    campaign.selection=selector.selection;
                    if(campaign.launch()){
                        sfDebugNativeLog("solo","STAGE_LAUNCHED_KEYBOARD");
                        selecting=false;touch.reset();kinetic.cancel();
                        sfBossDangerIndex=int(campaign.launchedSelection.difficulty)-1;
                        combat=sfsolo::Combat{};
                    }else sfDebugNativeLog("solo","STAGE_LAUNCH_FAILED");
                }
            }else{
                // Android needs its own game-over retry: there is no keyboard
                // "R" button on a phone. One tap relaunches this SAME stage.
                if(campaign.active &&
                   campaign.active->phase==sfsolo::Phase::Lost &&
                   (e.type==SDL_FINGERDOWN ||
                    (e.type==SDL_MOUSEBUTTONDOWN &&
                     e.button.button==SDL_BUTTON_LEFT))){
                    sfDebugNativeLog("solo","RESTART_AFTER_LOSS");
                    campaign.abandon();
                    selecting=!campaign.launch();
                    kinetic.cancel();touch.reset();combat=sfsolo::Combat{};
                    continue;
                }
                if(e.type==SDL_KEYDOWN && e.key.keysym.sym==SDLK_r){
                    campaign.abandon();
                    if(!campaign.launch())selecting=true;
                    combat=sfsolo::Combat{};touch.reset();kinetic.cancel();
                }
                const auto touchAimAt=[&](float screenX,float screenY){
                    if(!campaign.active)return;
                    float tx=0,ty=0;
                    sfsolo::aimWorldPoint(*campaign.active,screenX,screenY,w,h,tx,ty);
                    kinetic.aimAt(*campaign.active,tx,ty);
                };
                const auto aimRelease=[&](){
                    if(!campaign.active)return;
                    const bool charged=kinetic.release(*campaign.active);
                    if(charged)audio->play(sfsolo::SoundCue::Plasma);
                    else if(combat.fire(*campaign.active,
                                        kinetic.targetX,kinetic.targetY))
                        audio->play(sfsolo::SoundCue::Shot);
                    sfDebugNativeLog("solo",charged?
                                    "AIM360_CONE_RELEASE":"AIM360_TAP_FIRE");
                };
                sfsolo::TouchAction action=sfsolo::TouchAction::None;
                if(e.type==SDL_FINGERDOWN||e.type==SDL_FINGERUP||
                   e.type==SDL_FINGERMOTION){
                    float sx=-1,sy=-1;
                    if(campaign.active){
                        const auto camera=sfsolo::soloViewport(
                            *campaign.active,{0,0,w,h});
                        sx=camera.x(campaign.active->pilot.x);
                        sy=camera.y(campaign.active->pilot.y);
                    }
                    action=touch.handle(e,w,h,sx,sy,
                        campaign.active?campaign.active->pilot.x:NAN,
                        campaign.active?campaign.active->pilot.y:NAN);
                }
                if(action==sfsolo::TouchAction::ChargePressed){
                    touchAimAt(touch.aimX,touch.aimY);
                    sfDebugNativeLog("solo","AIM360_PRESS");
                    kinetic.press();
                    audio->play(sfsolo::SoundCue::Charge);
                }
                if(action==sfsolo::TouchAction::ChargeReleased){
                    touchAimAt(touch.aimX,touch.aimY);
                    aimRelease();
                }
                // Desktop/keyboard diagnostics: mouse tap fires; holding the
                // mouse (or SPACE) aims and releases the same canonical cone.
                if(e.type==SDL_KEYDOWN && !e.key.repeat &&
                   e.key.keysym.sym==SDLK_SPACE){
                    if(campaign.active)kinetic.aimAt(*campaign.active,
                        campaign.active->pilot.x,campaign.active->pilot.y-7);
                    kinetic.press();
                    audio->play(sfsolo::SoundCue::Charge);
                }
                if(e.type==SDL_KEYUP && e.key.keysym.sym==SDLK_SPACE)
                    aimRelease();
                if(e.type==SDL_MOUSEBUTTONDOWN &&
                   (e.button.button==SDL_BUTTON_LEFT||
                    e.button.button==SDL_BUTTON_RIGHT)){
                    touchAimAt(float(e.button.x),float(e.button.y));
                    kinetic.press();
                    audio->play(sfsolo::SoundCue::Charge);
                }
                if(e.type==SDL_MOUSEBUTTONUP &&
                   (e.button.button==SDL_BUTTON_LEFT||
                    e.button.button==SDL_BUTTON_RIGHT)){
                    touchAimAt(float(e.button.x),float(e.button.y));
                    aimRelease();
                }
                if(e.type==SDL_MOUSEMOTION &&
                   (e.motion.state & (SDL_BUTTON_LMASK|SDL_BUTTON_RMASK)))
                    touchAimAt(float(e.motion.x),float(e.motion.y));
                if(e.type==SDL_APP_WILLENTERBACKGROUND){
                    sfDebugNativeLog("solo","SDL_APP_BACKGROUND");
                    touch.reset();kinetic.cancel();
                }
            }
        }
        const Uint64 now=SDL_GetPerformanceCounter();
        const float dt=std::clamp(float(double(now-previous)/frequency),0.0f,.05f);
        previous=now;
        float ax=0,ay=0;
        const Uint8 *keys=SDL_GetKeyboardState(nullptr);
        if(keys[SDL_SCANCODE_LEFT]||keys[SDL_SCANCODE_A])ax=-1;
        if(keys[SDL_SCANCODE_RIGHT]||keys[SDL_SCANCODE_D])ax=1;
        if(keys[SDL_SCANCODE_UP]||keys[SDL_SCANCODE_W])ay=-1;
        if(keys[SDL_SCANCODE_DOWN]||keys[SDL_SCANCODE_S])ay=1;
        if(selecting){
            sfsolo::drawSelector(renderer,{0,0,w,h},selector,campaign.progression);
        }else if(campaign.active){
            // The old finger-target approach STOPPED after a short drag:
            // one gesture produced one finite world-space destination.
            // Now a steering drag stores direction and throttle, which remain
            // active on finger release, so a single gesture crosses the map.
            const bool keyboard=std::abs(ax)>0||std::abs(ay)>0;
            if(keyboard){
                // Physical keys override the persistent touch cruise.
                touch.cruiseSet=false;
                campaign.active->step(ax,ay,dt);
            }else if(touch.cruiseSet){
                const auto cam=sfsolo::soloViewport(
                    *campaign.active,{0,0,w,h});
                campaign.active->stepCruise(touch.cruiseX,touch.cruiseY,
                    touch.cruiseThrottle,cam.visibleColumns(),dt);
            }else{
                campaign.active->step(0,0,dt);
            }
            // Only the ship's real screen position and velocity govern
            // the two camera acceleration zones, whether held or released.
            sfsolo::soloUpdateCamera(*campaign.active,{0,0,w,h},dt);
            if(touch.chargeDown){
                float aimX=0,aimY=0;
                sfsolo::aimWorldPoint(*campaign.active,
                                     touch.aimX,touch.aimY,w,h,aimX,aimY);
                kinetic.aimAt(*campaign.active,aimX,aimY);
            }
            const bool wasCharged=sfKineticSurges[1].charged;
            kinetic.advance(*campaign.active,dt);
            if(!wasCharged && sfKineticSurges[1].charged)
                audio->play(sfsolo::SoundCue::Ready);
            if(campaign.active->phase==sfsolo::Phase::Lost)kinetic.cancel();
            // Prototype projectiles remain temporary. Touch 1 steers only;
            // touch 2 / SPACE charges COOP's real kinetic state machine.
            combat.step(*campaign.active,dt);
            if(combat.pendingImpacts){
                audio->play(sfsolo::SoundCue::Impact);
                combat.pendingImpacts=0;
            }
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
                    sfDebugNativeLog("solo","STAGE_WIN_SAVED");
                    campaign.next();
                    selector.selection=campaign.selection;
                    selecting=true;
                    touch.reset();
                    kinetic.cancel();
                    combat=sfsolo::Combat{};
                }else{
                    // Keep the completed session available for a later retry
                    // rather than unlock a stage that was never persisted.
                    sfDebugNativeLog("solo","STAGE_WIN_SAVE_FAILED");
                    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                                "SOLO progression not saved; session retained.");
                }
            }
            if(campaign.active){
                sfsolo::drawSession(renderer,*campaign.active,{0,0,w,h},
                                    artwork.get());
                sfsolo::drawCombat(renderer,*campaign.active,combat,{0,0,w,h},
                                   artwork.get());
                sfsolo::drawKineticCharge(renderer,*campaign.active,
                                          {0,0,w,h},kinetic,artwork.get());
                sfsolo::drawMissionOverlay(renderer,*campaign.active,{0,0,w,h});
            }else{
                // A successful victory consumes the session; show the selector
                // instead of dereferencing an already completed run.
                sfsolo::drawSelector(renderer,{0,0,w,h},
                                     selector,campaign.progression);
            }
        }
        SDL_RenderPresent(renderer);
        // One breadcrumb every three seconds: keeps logs small while identifying
        // which state and screen immediately preceded a native process exit.
        if(SDL_GetTicks64()-lastDebugHeartbeat>=3000){
            char event[320];
            if(campaign.active){
                const auto &p=campaign.active->pilot;
                std::snprintf(event,sizeof(event),
                    "HEARTBEAT phase=%d ship=(%.2f,%.2f) vel=(%.2f,%.2f) "
                    "cam=(%.2f,%.2f) pan=(%.2f,%.2f) hp=%.0f "
                    "ice=%.2f relays=%u/%u cruise=%d held=%d",
                    int(campaign.active->phase),p.x,p.y,p.vx,p.vy,
                    campaign.active->viewLeft,campaign.active->viewTop,
                    campaign.active->viewVelocityX,campaign.active->viewVelocityY,
                    p.health,campaign.active->iceSeconds,
                    campaign.active->relaysCollected,
                    campaign.active->relaysNeeded,
                    touch.cruiseSet?1:0,touch.down?1:0);
            }else{
                std::snprintf(event,sizeof(event),"HEARTBEAT selector=%d",
                              selecting?1:0);
            }
            sfDebugNativeLog("solo",event);
            lastDebugHeartbeat=SDL_GetTicks64();
        }
        SDL_Delay(10);
    }
    sfDebugNativeLog("solo","SOLO_LOOP_EXIT");
    kinetic.cancel();sfBossDangerIndex=historicalDanger;
    audio.reset();
    artwork.reset();
    IMG_Quit();
    SDL_DestroyRenderer(renderer);SDL_DestroyWindow(window);SDL_Quit();
    return 0;
}
#ifdef SF_SOLO_STANDALONE
int main(int,char**){return runSoloPrototype();}
#endif
