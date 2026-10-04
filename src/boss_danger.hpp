#pragma once
#include <algorithm>

inline constexpr float SF_BOSS_DANGER_LEVELS[5]={1.0f,5.0f,10.0f,15.0f,20.0f};
inline constexpr const char *SF_BOSS_DANGER_NAMES[5]={
    "MOU DU GENOU","CHILL","ROCK N ROLL","DUR A CUIRE","MACHINE DE GUERRE"
};
inline int sfBossDangerIndex=2; // ROCK N ROLL / x10 default, coefficient hidden from player.

static float sfBossDangerMultiplier()
{
    sfBossDangerIndex=std::clamp(sfBossDangerIndex,0,4);
    return SF_BOSS_DANGER_LEVELS[sfBossDangerIndex];
}
static const char *sfBossDangerName()
{
    sfBossDangerIndex=std::clamp(sfBossDangerIndex,0,4);
    return SF_BOSS_DANGER_NAMES[sfBossDangerIndex];
}
static void sfBossDangerNext()
{
    sfBossDangerIndex=(sfBossDangerIndex+1)%5;
}
static void sfBossDangerAdjust(int direction)
{
    if(direction<0 && sfBossDangerIndex>0) --sfBossDangerIndex;
    if(direction>0 && sfBossDangerIndex<4) ++sfBossDangerIndex;
}
