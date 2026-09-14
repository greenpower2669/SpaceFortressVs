#pragma once
enum SfGameMode { SF_DUEL_LOCAL, SF_DUEL_AI, SF_COOP_LOCAL, SF_COOP_AI };
inline int sfSelectedMode=SF_DUEL_LOCAL,sfActiveMode=SF_DUEL_LOCAL;
static bool sfIsCoop() { return sfActiveMode>=SF_COOP_LOCAL; }
static bool sfModeHasAi(int mode) { return mode==SF_DUEL_AI || mode==SF_COOP_AI; }
static void sfCampaignStart();
static void sfCampaignSuspend();
static void sfCampaignRestoreDuelShips();
static bool sfCampaignHandleEvent(SDL_Event *);
static bool sfCampaignFrame(SDL_Renderer *);
static void sfCampaignDrawHall(SDL_Renderer *);
static void sfCampaignDrawSelect(SDL_Renderer *);
static void sfCampaignForgetRenderer(SDL_Renderer *);
