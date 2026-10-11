#pragma once
#include <algorithm>
#include <cmath>
#include "ship_energy_rules.hpp"
// Historical sprite API delegates to common canonical pure functions.
static void sfAddShipHeat(sprite *ship,float amount)
{
    ship->nrj=sfShipHeat(sfShipHeat(ship->nrj)+amount);
}

static SDL_Rect sfEnergyMarkerRect(float heat,SDL_Rect full,const SDL_Rect &gun)
{
    const float spent=sfShipHeat(heat)/SF_MAX_SHIP_HEAT;
    const float gunX=gun.x+(gun.w-full.w)*.5f;
    const float gunY=gun.y+(gun.h-full.h)*.5f;
    full.x=std::lround(full.x+(gunX-full.x)*spent);
    full.y=std::lround(full.y+(gunY-full.y)*spent);
    return full;
}

static bool sfSpendMainEnergy(sprite *ship)
{
    float heat=ship->nrj;
    const bool missile=sfSpendMainEnergyReserve(heat);
    ship->nrj=heat;
    return missile;
}
static float sfApplyShieldImpact(sprite *ship,float incomingDamage)
{
    if (!ship || incomingDamage<=0) return 0;
    const float heatBefore=sfShipHeat(ship->nrj);
    const float hullDamage=sfShieldDamage(incomingDamage,heatBefore);
    sfAddShipHeat(ship,sfShieldWear(incomingDamage,heatBefore));
    return hullDamage;
}
static float sfApplyShieldContinuousImpact(sprite *ship,float incomingPerSecond,float dt)
{
    if (!ship || incomingPerSecond<=0 || dt<=0) return 0;
    constexpr float step=1.0f/240.0f;
    float remaining=dt,total=0;
    while (remaining>1e-6f) {
        const float slice=std::min(step,remaining);
        total+=sfApplyShieldImpact(ship,incomingPerSecond*slice);
        remaining-=slice;
    }
    return total;
}
static void sfRegenerateHull(sprite *ship,float dt)
{
    if (!ship || ship->pv<=0 || ship->pv>=1000 || dt<=0) return;
    ship->pv=std::min(1000.0f,ship->pv+sfHullRegenPerSecond(sfShipHeat(ship->nrj))*dt);
}
