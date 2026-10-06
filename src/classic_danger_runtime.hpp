#pragma once

// Classic danger is deliberately damage-only. Owner 0 is the orange fortress,
// which is the AI-controlled side in SF_DUEL_AI; the blue fortress is the
// human victim. No other owner/mode/victim combination is hostile for this
// classic-damage rule.
static bool sfClassicAiHostileShot(const sprite *shot,const sprite *victim)
{
    return shot && victim && sfActiveMode==SF_DUEL_AI &&
           shot->shotOwner==0 && victim==Spritej2;
}

static float sfClassicIncomingNonKinetic(const sprite *shot,const sprite *victim,float rawDamage)
{
    return sfClassicAiHostileShot(shot,victim) ? sfApplyHostileDanger(rawDamage) : rawDamage;
}
