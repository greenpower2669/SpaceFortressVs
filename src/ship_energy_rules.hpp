#pragma once
// Historical SpaceFortress nrj and shot equations: shared by tactical/COOP
// and the SOLO world. All functions are pure; never implement a second formula.
#include "kinetic_shield.hpp"
#include <algorithm>
#include <cmath>
constexpr float SF_MAX_SHIP_HEAT=50.0f;
static float sfShipHeat(float heat){
    return std::isnan(heat)?SF_MAX_SHIP_HEAT:std::clamp(heat,0.0f,SF_MAX_SHIP_HEAT);
}
static float sfShieldSpent(float heat){return sfShipHeat(heat)/SF_MAX_SHIP_HEAT;}
static float sfShieldFraction(float heat){return 1.0f-sfShieldSpent(heat);}
static float sfShieldDamage(float damage,float heat){
    return std::max(0.0f,damage)*sfShieldSpent(heat);
}
static float sfShieldWear(float damage,float heat){
    return std::max(0.0f,damage)*std::clamp(.1f+sfShieldSpent(heat),.1f,1.0f);
}
static bool sfSpendMainEnergyReserve(float &heat){
    heat=sfShipHeat(sfShipHeat(heat)+1.0f);
    const bool missile=heat<1.5f;
    if(missile)heat=sfShipHeat(heat+10.0f);
    return missile;
}
static float sfCanonicalMainShotSpeed(float heat,float arenaWidth){
    return (60.0f-sfShipHeat(heat+1.0f))*.4f*60.0f*arenaWidth/780.0f;
}
static float sfCanonicalMainShotSpreadEnvelope(float heat){
    const float spent=sfShipHeat(heat)/SF_MAX_SHIP_HEAT;
    return .018f+.162f*std::pow(spent,1.25f);
}
static float sfCanonicalMainShotSpreadRadians(float heat,float randomUnit){
    return std::clamp(randomUnit,-1.0f,1.0f)*sfCanonicalMainShotSpreadEnvelope(heat);
}
static float sfCanonicalCoopPassiveRechargeMultiplier(int dangerIndex){
    const float t=std::clamp(dangerIndex,0,SF_BOSS_DANGER_COUNT-1)/
                  float(SF_BOSS_DANGER_COUNT-1);
    return 4.0f+(2.0f-4.0f)*t;
}
static float sfCanonicalCoopPassiveRechargeHeat(float heat,float dt,int dangerIndex){
    heat=sfShipHeat(heat);
    if(dt<=0)return heat;
    const float updates=60.0f*sfCanonicalCoopPassiveRechargeMultiplier(dangerIndex)*dt;
    return sfShipHeat(heat*std::pow(.997f,updates));
}
