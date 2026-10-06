#pragma once
#include <algorithm>

inline constexpr int SF_BOSS_DANGER_COUNT=9;
inline constexpr float SF_BOSS_DANGER_LEVELS[SF_BOSS_DANGER_COUNT]={
    1.0f,5.0f,10.0f,15.0f,20.0f,25.0f,30.0f,35.0f,40.0f
};
inline constexpr const char *SF_BOSS_DANGER_NAMES[SF_BOSS_DANGER_COUNT]={
    "MOU DU GENOU","CHILL","ROCK N ROLL","DUR A CUIRE","MACHINE DE GUERRE",
    "CA VA PIQUER","SANS PITIE","ENFER STELLAIRE","APOCALYPSE"
};
inline int sfBossDangerIndex=2; // ROCK N ROLL / x10 default, coefficient hidden from player.

static float sfBossDangerMultiplier()
{
    sfBossDangerIndex=std::clamp(sfBossDangerIndex,0,SF_BOSS_DANGER_COUNT-1);
    return SF_BOSS_DANGER_LEVELS[sfBossDangerIndex];
}
static const char *sfBossDangerName()
{
    sfBossDangerIndex=std::clamp(sfBossDangerIndex,0,SF_BOSS_DANGER_COUNT-1);
    return SF_BOSS_DANGER_NAMES[sfBossDangerIndex];
}
static void sfBossDangerNext()
{
    sfBossDangerIndex=(sfBossDangerIndex+1)%SF_BOSS_DANGER_COUNT;
}
static void sfBossDangerAdjust(int direction)
{
    if(direction<0 && sfBossDangerIndex>0) --sfBossDangerIndex;
    if(direction>0 && sfBossDangerIndex<SF_BOSS_DANGER_COUNT-1) ++sfBossDangerIndex;
}
static float sfApplyHostileDanger(float rawDamage)
{
    return rawDamage*sfBossDangerMultiplier();
}
