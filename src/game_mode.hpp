#pragma once
#include "boss_danger.hpp"
enum SfGameMode { SF_DUEL_LOCAL, SF_DUEL_AI, SF_COOP_LOCAL, SF_COOP_AI, SF_SOLO_GAME, SF_GAME_MODE_COUNT };
inline int sfSelectedMode=SF_DUEL_LOCAL, sfActiveMode=SF_DUEL_LOCAL;
static bool sfModeIsCoop(int mode) { return mode==SF_COOP_LOCAL || mode==SF_COOP_AI; }
static bool sfIsCoop() { return sfModeIsCoop(sfActiveMode); }
#ifdef SF_ANDROID_SOLO_LAUNCH
extern "C" bool sfLaunchSoloGame(int danger);
#else
static bool sfLaunchSoloGame(int) { return false; }
#endif
inline bool sfSoloLaunchFailed=false;
static bool sfModeHasAi(int mode) { return mode==SF_DUEL_AI || mode==SF_COOP_AI; }
static void sfCampaignStart();
static void sfCampaignRestoreDuelShips();
static void sfCampaignSuspend();
static bool sfCampaignHandleEvent(SDL_Event *event);
static void sfCampaignDrawHall(SDL_Renderer *renderer);
static void sfCampaignDrawSelect(SDL_Renderer *renderer);
static bool sfCampaignFrame(SDL_Renderer *renderer);
static void sfCampaignForgetRenderer(SDL_Renderer *renderer);
