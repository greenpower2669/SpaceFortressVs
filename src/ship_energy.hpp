#pragma once
#include <algorithm>
#include <cmath>

// Historical nrj measures spent energy/heat: 0 is full, 50 is exhausted.
// Clamp mutations as well as the display; an impact must not create an
// unbounded reserve deficit that firing then silently erases.
constexpr float SF_MAX_SHIP_HEAT = 50.0f;

static float sfShipHeat(float heat)
{
    return std::isnan(heat) ? SF_MAX_SHIP_HEAT : std::clamp(heat,0.0f,SF_MAX_SHIP_HEAT);
}

static void sfAddShipHeat(sprite *ship,float amount)
{
    ship->nrj=sfShipHeat(sfShipHeat(ship->nrj)+amount);
}

static SDL_Rect sfEnergyMarkerRect(float heat,SDL_Rect full,const SDL_Rect &gun)
{
    // The crystal travels only between the full-energy anchor and the gun.
    // Interpolate centres, so animation/asset dimensions cannot shift its limit.
    const float spent=sfShipHeat(heat)/SF_MAX_SHIP_HEAT;
    const float gunX=gun.x+(gun.w-full.w)*.5f;
    const float gunY=gun.y+(gun.h-full.h)*.5f;
    full.x=std::lround(full.x+(gunX-full.x)*spent);
    full.y=std::lround(full.y+(gunY-full.y)*spent);
    return full;
}

// Shared historical main-weapon cost, including the full-reserve missile.
static bool sfSpendMainEnergy(sprite *ship)
{
    sfAddShipHeat(ship,1);
    const bool missile=ship->nrj<1.5f;
    if (missile) sfAddShipHeat(ship,10);
    return missile;
}
static float sfShieldSpent(float heat)
{
    return sfShipHeat(heat)/SF_MAX_SHIP_HEAT;
}
static float sfShieldFraction(float heat)
{
    return 1.0f-sfShieldSpent(heat);
}
static float sfShieldDamage(float damage,float heat)
{
    return std::max(0.0f,damage)*sfShieldSpent(heat);
}
static float sfShieldWear(float damage,float heat)
{
    const float factor=std::clamp(.1f+sfShieldSpent(heat),.1f,1.0f);
    return std::max(0.0f,damage)*factor;
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
