#pragma once
enum SfGameMode { SF_DUEL_LOCAL, SF_DUEL_AI, SF_COOP_LOCAL, SF_COOP_AI };
inline int sfSelectedMode=SF_DUEL_LOCAL, sfActiveMode=SF_DUEL_LOCAL;
static bool sfIsCoop() { return sfActiveMode==SF_COOP_LOCAL || sfActiveMode==SF_COOP_AI; }
static bool sfModeHasAi(int mode) { return mode==SF_DUEL_AI || mode==SF_COOP_AI; }
static void sfCampaignStart();
static void sfCampaignRestoreDuelShips();
static void sfCampaignSuspend();
static bool sfCampaignHandleEvent(SDL_Event *event);
static void sfCampaignDrawHall(SDL_Renderer *renderer);
static void sfCampaignDrawSelect(SDL_Renderer *renderer);
static bool sfCampaignFrame(SDL_Renderer *renderer);
static void sfCampaignForgetRenderer(SDL_Renderer *renderer);
