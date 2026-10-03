#pragma once

inline constexpr float SF_BOSS_DANGER_LEVELS[5]={1.0f,5.0f,10.0f,15.0f,20.0f};
inline int sfBossDangerIndex=2; // x10 is the canonical default.

static float sfBossDangerMultiplier()
{
    if(sfBossDangerIndex<0) sfBossDangerIndex=0;
    if(sfBossDangerIndex>4) sfBossDangerIndex=4;
    return SF_BOSS_DANGER_LEVELS[sfBossDangerIndex];
}
static void sfBossDangerAdjust(int direction)
{
    if(direction<0 && sfBossDangerIndex>0) --sfBossDangerIndex;
    if(direction>0 && sfBossDangerIndex<4) ++sfBossDangerIndex;
}
