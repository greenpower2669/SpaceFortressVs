#pragma once
#include <algorithm>
#include <cmath>

// Visual-only expression of kinetic fatigue. Collision timing and damage are
// resolved before these overlays are drawn; this layer never mutates gameplay.
static float sfKineticWaveEnergyFraction(const SfKineticWave &wave)
{
    return std::clamp(wave.energyFraction,0.0f,1.0f);
}

static float sfKineticEnergyFlashFactor(float energyFraction,Uint64 ticks)
{
    energyFraction=std::clamp(energyFraction,0.0f,1.0f);
    if(energyFraction>=SF_KINETIC_LOW_ENERGY_WARNING_FRACTION) return 1.0f;
    const float seconds=float(ticks)*.001f;
    const float pulse=.5f+.5f*std::sin(seconds*2.0f*float(PI)*SF_KINETIC_LOW_ENERGY_FLASH_HZ);
    return .42f+.58f*pulse;
}

static SDL_Color sfKineticEnergyWaveColor(SDL_Color team,float energyFraction,Uint8 alpha,float flash)
{
    energyFraction=std::clamp(energyFraction,0.0f,1.0f);
    flash=std::clamp(flash,0.0f,1.0f);
    const float fatigue=1.0f-energyFraction;
    const SDL_Color orange{255,118,34,255};
    const SDL_Color red{255,24,18,255};
    SDL_Color target=orange;
    float t=std::clamp(fatigue/.70f,0.0f,1.0f);
    if(energyFraction<.30f) {
        target=red;
        t=std::clamp((.30f-energyFraction)/.30f,0.0f,1.0f);
        const float r=orange.r+(red.r-orange.r)*t;
        const float g=orange.g+(red.g-orange.g)*t;
        const float b=orange.b+(red.b-orange.b)*t;
        target={Uint8(r),Uint8(g),Uint8(b),255};
        t=1.0f;
    }
    const float blend=energyFraction<.30f ? 1.0f : t;
    const float r=team.r+(target.r-team.r)*blend;
    const float g=team.g+(target.g-team.g)*blend;
    const float b=team.b+(target.b-team.b)*blend;
    const float boost=1.0f+.70f*fatigue;
    const float lowFlash=energyFraction<SF_KINETIC_LOW_ENERGY_WARNING_FRACTION ? flash : 1.0f;
    const float a=std::clamp(float(alpha)*boost*lowFlash,0.0f,255.0f);
    return {Uint8(std::clamp(r,0.0f,255.0f)),Uint8(std::clamp(g,0.0f,255.0f)),
            Uint8(std::clamp(b,0.0f,255.0f)),Uint8(a)};
}

static void sfDrawKineticEnergyWarningOverlay(SDL_Renderer *renderer)
{
    if(!renderer || sfUiScreen!=SF_UI_GAME) return;
    SDL_BlendMode previous;SDL_GetRenderDrawBlendMode(renderer,&previous);
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    const Uint64 ticks=SDL_GetTicks64();

    for(const auto &wave:sfKineticWaves) {
        if(wave.owner<0 || wave.owner>1 || !sfKineticWaveAlive(wave)) continue;
        const auto *ship=wave.owner==0 ? Spritej1 : Spritej2;
        if(!ship || ship->pv<=0) continue;
        const float energy=sfKineticWaveEnergyFraction(wave);
        if(energy>.88f) continue; // healthy shields keep the clean historical team wave.
        const float diameter=std::max(1.0f,std::max({ship->sw,ship->sh,ship->w,ship->h}));
        const float radius=sfKineticWaveRadiusAt(wave,diameter,wave.age);
        if(radius<=0) continue;
        const float progress=sfKineticWaveProgressAt(wave,wave.age);
        const float birth=std::clamp(progress/.055f,0.0f,1.0f);
        const float fade=std::pow(std::max(0.0f,1.0f-progress),.48f);
        const float fatigue=1.0f-energy;
        const Uint8 baseAlpha=Uint8(std::clamp((95.0f+130.0f*fatigue)*birth*fade,0.0f,245.0f));
        if(baseAlpha<3) continue;
        const float flash=sfKineticEnergyFlashFactor(energy,ticks);
        const SDL_Color team=wave.owner==0 ? SDL_Color{255,188,96,255} : SDL_Color{96,210,255,255};
        const auto color=sfKineticEnergyWaveColor(team,energy,baseAlpha,flash);
        sfTacticalRing(renderer,tupl(ship->x,ship->y),radius,color);
        sfTacticalRing(renderer,tupl(ship->x,ship->y),std::max(1.0f,radius-3.5f),
            SDL_Color{color.r,color.g,color.b,Uint8(color.a*.62f)});
    }

    // During the two-second surge, an almost empty reserve also warns the pilot
    // with a red pulse around the charge aura. The iridescent charge itself stays visible.
    for(int owner=0;owner<2;++owner) if(sfKineticSurgeVisible(owner)) {
        const auto *ship=owner==0 ? Spritej1 : Spritej2;
        if(!ship || ship->pv<=0) continue;
        const float energy=sfKineticEnergyFraction(ship->nrj);
        if(energy>=SF_KINETIC_LOW_ENERGY_WARNING_FRACTION) continue;
        const float diameter=std::max(1.0f,std::max({ship->sw,ship->sh,ship->w,ship->h}));
        const float flash=sfKineticEnergyFlashFactor(energy,ticks);
        const Uint8 alpha=Uint8(110+130*flash);
        sfTacticalRing(renderer,tupl(ship->x,ship->y),diameter*SF_KINETIC_MAX_SHIELD_DIAMETER*.5f,
            SDL_Color{255,22,16,alpha});
    }
    SDL_SetRenderDrawBlendMode(renderer,previous);
}
