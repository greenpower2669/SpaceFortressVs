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
