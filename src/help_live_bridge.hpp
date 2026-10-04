#pragma once

// Final help/input bridge. It is included after campaign_runtime.hpp so it can
// pause/resume a live cooperative encounter without resetting any gameplay
// state. It deliberately owns the final SDL event/present boundaries.

#include <SDL2/SDL.h>
#include <algorithm>

static void sfHelpLiveFinishClose()
{
    const bool resume=sfHelpState.fromLiveGame && sfHelpState.returnScreen==SF_UI_GAME;
    const int returnScreen=sfHelpState.returnScreen;
    sfHelpState.open=false;
    sfHelpState.closeRequested=false;
    sfHelpState.tutorialRequested=false;
    sfHelpState.view=SfHelpView::Hub;
    sfHelpState.page=0;
    sfHelpState.fromLiveGame=false;

    if(resume) {
        if(sfIsCoop() && sfCoop.phase==SfCoopPhase::Paused) sfCoop.phase=SfCoopPhase::Combat;
        sfFixRequestedScreen.store(SF_UI_GAME);
        sfUiScreen=SF_UI_GAME;
        setgui=false;
    } else {
        sfFixRequestedScreen.store(returnScreen);
    }
}

static void sfHelpLiveOpenFromGame(SDL_FingerID finger)
{
    sfFixConsumedFingers.insert(finger);
    sfFixResetGearGesture();
    if(sfIsCoop()) sfCampaignSuspend();
    else {
        sfKineticAudioReset();
        sfKineticResetSurges();
        sfObserved={};
    }
    if(Spritej1) {Spritej1->ctrl=false;Spritej1->id=100;}
    if(Spritej2) {Spritej2->ctrl=false;Spritej2->id=100;}
    sfHelpOpen(SF_UI_GAME,true);
    sfFixRequestedScreen.store(SF_UI_HELP);
}

static bool sfHelpLiveHitGameButton(const SDL_TouchFingerEvent &finger)
{
    const int width=std::max(1,int(tw));
    const int height=std::max(1,int(th));
    return sfHelpPointIn(sfHelpGameButtonRect(width,height),finger.x,finger.y,width,height);
}

static void sfHelpLiveHandleHelpEvent(SDL_Event *event)
{
    if(!event) return;
    if(event->type==SDL_FINGERDOWN) {
        sfFixConsumedFingers.insert(event->tfinger.fingerId);
        const int width=std::max(1,int(tw));
        const int height=std::max(1,int(th));
        sfHelpHandleTap(event->tfinger.x,event->tfinger.y,width,height);
        event->type=SDL_USEREVENT;
    } else if(event->type==SDL_FINGERMOTION || event->type==SDL_FINGERUP) {
        event->type=SDL_USEREVENT;
    } else if(event->type==SDL_KEYDOWN &&
              (event->key.keysym.sym==SDLK_ESCAPE || event->key.keysym.sym==SDLK_AC_BACK)) {
        sfHelpBack();
        event->type=SDL_USEREVENT;
    }
    if(sfHelpState.closeRequested) sfHelpLiveFinishClose();
}

// One final routing function is used both by the SDL wrapper and by direct
// regression/integration callers. The historical router remains the fallback.
static void SpaceFortressHelpLive_HandleEvent(SDL_Event *event)
{
    if(!event) return;

    if((event->type==SDL_FINGERMOTION || event->type==SDL_FINGERUP) &&
       sfFixConsumedFingers.count(event->tfinger.fingerId)) {
        if(event->type==SDL_FINGERUP) {
            const SDL_FingerID fid=event->tfinger.fingerId;
            sfFixConsumedFingers.erase(fid);
            // Preserve the historical gear-release cleanup even when the
            // final help router owns the consumed finger's FINGERUP.
            if(sfFixGear1Down && sfFixGear1Finger==fid) {
                sfFixGear1Down=false;sfFixGear1Finger=0;
            }
            if(sfFixGear2Down && sfFixGear2Finger==fid) {
                sfFixGear2Down=false;sfFixGear2Finger=0;
            }
        }
        event->type=SDL_USEREVENT;
        return;
    }

    const int requested=sfFixRequestedScreen.load();
    if(requested==SF_UI_HELP && sfHelpState.open) {
        sfHelpLiveHandleHelpEvent(event);
        return;
    }

    if(requested==SF_UI_GAME && event->type==SDL_FINGERDOWN && sfHelpLiveHitGameButton(event->tfinger)) {
        sfHelpLiveOpenFromGame(event->tfinger.fingerId);
        event->type=SDL_USEREVENT;
        return;
    }

    if(requested==SF_UI_HOME && event->type==SDL_FINGERDOWN &&
       event->tfinger.y>=.735f && event->tfinger.y<=.85f) {
        sfHelpOpen(SF_UI_HOME,false);
        sfFixHandleEvent(event);
        return;
    }

    sfFixHandleEvent(event);
}

#ifdef SDL_WaitEvent
#undef SDL_WaitEvent
#endif

static int SpaceFortressHelpLive_WaitEvent(SDL_Event *event)
{
    const int result=SDL_WaitEvent(event);
    if(!result || !event) return result;
    std::lock_guard<std::recursive_mutex> lock(sfGameMutex);
    SpaceFortressHelpLive_HandleEvent(event);
    return result;
}

#ifdef SDL_RenderPresent
#undef SDL_RenderPresent
#endif

static void SpaceFortressHelpLive_RenderPresent(SDL_Renderer *renderer)
{
    if(!renderer) return;
    sfFixApplyUiRequests();
    if(sfUiScreen==SF_UI_HELP && sfHelpState.open) {
        sfHelpDrawActive(renderer);
        SDL_RenderPresent(renderer);
        return;
    }
    if(sfUiScreen==SF_UI_GAME) sfHelpDrawGameButton(renderer);
    SpaceFortressVisualRestore_RenderPresent(renderer);
}

// Calls that occur after this final bridge (notably integration tests) use the
// same router as the actual SDL boundary instead of bypassing live-help logic.
#define sfFixHandleEvent SpaceFortressHelpLive_HandleEvent
#define SDL_WaitEvent SpaceFortressHelpLive_WaitEvent
#define SDL_RenderPresent SpaceFortressHelpLive_RenderPresent
