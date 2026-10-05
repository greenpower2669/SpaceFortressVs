#pragma once

// Final event-boundary hook for Hall synchronization. It observes durable
// campaign changes and UI transitions after the existing help/campaign router
// has handled the event. Network work itself stays asynchronous in the
// transport; no HTTP/JNI call is made from Hall drawing code.

#include "hall_sync_runtime.hpp"

#ifdef SDL_WaitEvent
#undef SDL_WaitEvent
#endif

static int SpaceFortressHallSync_WaitEvent(SDL_Event *event)
{
    const int result=SDL_WaitEvent(event);
    if(!result || !event) return result;

    bool savedVictory=false;
    bool openedHall=false;
    {
        std::lock_guard<std::recursive_mutex> lock(sfGameMutex);
        const size_t beforeFame=sfCampaignSave.fame.size();
        const int beforeScreen=sfFixRequestedScreen.load();
        SpaceFortressHelpLive_HandleEvent(event);
        savedVictory=sfCampaignSave.fame.size()>beforeFame;
        openedHall=beforeScreen!=SF_UI_HALL && sfFixRequestedScreen.load()==SF_UI_HALL;
    }

    // Campaign save has already succeeded before fame grows, so failure here
    // can never invalidate the local victory. Hall-open reconciliation closes
    // the crash window between these two durable stores.
    if(savedVictory) sfHallSyncAfterLocalVictorySaved();
    if(openedHall) sfHallSyncOnHallOpen();
    return result;
}

#define SDL_WaitEvent SpaceFortressHallSync_WaitEvent
