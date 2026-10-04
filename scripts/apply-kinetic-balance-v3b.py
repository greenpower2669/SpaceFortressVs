from pathlib import Path
import re

ROOT=Path(__file__).resolve().parents[1]

def read(path): return (ROOT/path).read_text()
def write(path,text): (ROOT/path).write_text(text)
def sub(path,pattern,repl,count=1,flags=0):
    text=read(path)
    text2,n=re.subn(pattern,repl,text,count=count,flags=flags)
    if n!=count:
        raise AssertionError(f'{path}: expected {count} substitutions, got {n}: {pattern[:120]}')
    write(path,text2)

def literal(path,old,new,count=1):
    text=read(path)
    if text.count(old)<count:
        raise AssertionError(f'{path}: missing literal: {old[:120]!r}')
    write(path,text.replace(old,new,count))

# Named boss difficulty selector. Multipliers remain internal only.
write('src/boss_danger.hpp','''#pragma once
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
''')

# Linear asteroid kinetic law: largest reference asteroid at reference max speed/full closing = 250 PV (25%).
literal('src/kinetic_shield.hpp',
        'constexpr float SF_KINETIC_MASS_DAMAGE_FLOOR = .01f;\n',
        'constexpr float SF_KINETIC_ASTEROID_MAX_HULL_DAMAGE = 250.0f; // 25% of the canonical 1000 PV hull.\n')
sub('src/kinetic_shield.hpp',
    r'''    const float impactRatio=out\.impactSpeed/referenceSpeed;\n    out\.relativeRatio=out\.relativeSpeed/referenceSpeed;\n    out\.massFactor=std::max\(0\.0f,massFactor\);\n    // .*?\n    out\.speedFactor=SF_KINETIC_MASS_DAMAGE_FLOOR\+impactRatio\*impactRatio;\n    out\.rawDamage=std::max\(0\.0f,baseDamage\)\*out\.massFactor\*out\.speedFactor;''',
    '''    out.relativeRatio=out.relativeSpeed/referenceSpeed;\n    out.massFactor=std::clamp(massFactor,0.0f,1.0f);\n    const float speedRatio=std::clamp(out.relativeRatio,0.0f,1.0f);\n    const float closingFactor=out.relativeSpeed>.0001f\n        ? std::clamp(out.impactSpeed/out.relativeSpeed,0.0f,1.0f) : 0.0f;\n    // Canon: linear mass x relative speed x actual inward/closing component.\n    out.speedFactor=speedRatio*closingFactor;\n    out.rawDamage=std::max(0.0f,baseDamage)*out.massFactor*out.speedFactor;''')

# Shared asteroid resolver: mass normalized 0..1, same law classic + coop.
literal('src/legacy_field_runtime.hpp',
'''    const float area=std::max(1.0f,rock->w*rock->h);\n    const float refArea=sfKineticReferenceArea(sfArenaH);\n    return sfResolveKinetic(refArea*.05f,sfKineticMassFactorFromArea(area,sfArenaH),\n''',
'''    const float area=std::max(1.0f,rock->w*rock->h);\n    const float massRelative=std::clamp(sfKineticMassFactorFromArea(area,sfArenaH),0.0f,1.0f);\n    return sfResolveKinetic(SF_KINETIC_ASTEROID_MAX_HULL_DAMAGE,massRelative,\n''')

# Mark any actual field dissipation as an interaction so it cannot immediately fall through to a full hull collision.
sub('src/legacy_field_runtime.hpp',
    r'''static bool sfKineticTryLayer\(sprite \*rock,sprite \*ship,int owner,SfKineticLayer layer,\n                    const SfKineticSolution &raw\)\n\{\n    const auto solved=''',
    '''static bool sfKineticTryLayer(sprite *rock,sprite *ship,int owner,SfKineticLayer layer,\n                    const SfKineticSolution &raw,bool *interacted=nullptr)\n{\n    if(interacted) *interacted=false;\n    const auto solved=''')
literal('src/legacy_field_runtime.hpp',
        '    if(solved.dissipationFraction<=.001f) return false;\n    sfAddShipHeat(ship,solved.energyCost);',
        '    if(solved.dissipationFraction<=.001f) return false;\n    if(interacted) *interacted=true;\n    sfAddShipHeat(ship,solved.energyCost);')
literal('src/legacy_field_runtime.hpp',
'''  if(rock->kineticStage==0 && raw.suggestedLayer==SfKineticLayer::Outer && travelled<=outer) {\n      rock->kineticStage=1;\n      if(sfKineticTryLayer(rock,ship,owner,SfKineticLayer::Outer,raw)) continue;\n  }\n  if(rock->pv<=0) continue;\n  if(rock->kineticStage<2 && raw.selectedRange>0 && travelled<=inner) {\n      rock->kineticStage=2;\n      if(sfKineticTryLayer(rock,ship,owner,SfKineticLayer::Inner,raw)) continue;\n  }\n''',
'''  bool interacted=false;\n  if(rock->kineticStage==0 && raw.suggestedLayer==SfKineticLayer::Outer && travelled<=outer) {\n      rock->kineticStage=1;\n      if(sfKineticTryLayer(rock,ship,owner,SfKineticLayer::Outer,raw,&interacted)) continue;\n      if(interacted) continue;\n  }\n  if(rock->pv<=0) continue;\n  interacted=false;\n  if(rock->kineticStage<2 && raw.selectedRange>0 && travelled<=inner) {\n      rock->kineticStage=2;\n      if(sfKineticTryLayer(rock,ship,owner,SfKineticLayer::Inner,raw,&interacted)) continue;\n      if(interacted) continue;\n  }\n''')

# Red dust is visual-only. Keep the compatibility hook, but it mutates no gameplay state.
sub('src/campaign_runtime.hpp',r'\nstatic constexpr float SF_COOP_RED_DUST_HEAT = \.08f;\nstatic constexpr float SF_COOP_RED_DUST_ARMED_PV = 560\.0f;','')
sub('src/campaign_runtime.hpp',r'\nstatic void sfCoopCollectRedDust\(\)\n\{.*?\n\}\n\nstatic void sfCoopHurt',
'''\nstatic void sfCoopCollectRedDust()\n{\n    // Visual tracer only. Motion/deflection is owned by the shared kinetic field.\n}\n\nstatic void sfCoopHurt''', flags=re.S)

# White dust: exact full energy first (nrj -> 0), then strong hull healing on subsequent dust.
literal('src/tactical_runtime.hpp','static void sfCollectDust()\n{',
'''inline constexpr float SF_WHITE_DUST_ENERGY_RESTORE = 1.0f;\ninline constexpr float SF_WHITE_DUST_FULL_ENERGY_HEAL = 4.0f;\n\nstatic void sfCollectDust()\n{''')
literal('src/tactical_runtime.hpp',
'''            const float value=std::clamp(dust->pv*k0/600,0.0f,1.0f);\n            // nrj is depletion/heat: a lower value means MORE available energy.\n            winner->nrj=sfShipHeat(sfShipHeat(winner->nrj)-.25f*value);\n            winner->pv=std::min(1000.0f,winner->pv+.30f*value);\n            dust->pv=0; sfPickupGlow[owner]=.65f;''',
'''            const float value=std::clamp(dust->pv*k0/600,0.0f,1.0f);\n            // nrj is depletion/heat: 0 means a genuinely full reserve.\n            const float heatBefore=sfShipHeat(winner->nrj);\n            if (heatBefore>0) winner->nrj=sfShipHeat(heatBefore-SF_WHITE_DUST_ENERGY_RESTORE*value);\n            else winner->pv=std::min(1000.0f,winner->pv+SF_WHITE_DUST_FULL_ENERGY_HEAL*value);\n            dust->pv=0; sfPickupGlow[owner]=.65f;''')

# Every kinetic wave becomes an unmistakable centre->outside moving front.
literal('src/tactical_runtime.hpp','static void sfDrawKineticEffects(SDL_Renderer *renderer)\n{',
'''static Uint8 sfKineticWaveFrontAlpha(float progress,float strength)\n{\n    progress=std::clamp(progress,0.0f,1.0f);\n    strength=std::clamp(strength,0.0f,1.0f);\n    const float birth=std::clamp(progress/.055f,0.0f,1.0f);\n    const float fade=std::pow(std::max(0.0f,1.0f-progress),.55f);\n    return Uint8(std::clamp(245.0f*birth*fade*(.50f+.50f*strength),0.0f,245.0f));\n}\n\nstatic void sfDrawKineticEffects(SDL_Renderer *renderer)\n{''')
literal('src/tactical_runtime.hpp',
'''        const float p=sfKineticWaveProgressAt(wave,wave.age);\n        const float envelope=std::sin(float(PI)*p)*wave.strength;\n        if(envelope<=.01f) continue;\n        const SDL_Color team=wave.owner==0 ? SDL_Color{255,188,96,255} : SDL_Color{96,210,255,255};\n        sfTacticalRing(renderer,tupl(ship->x,ship->y),radius,\n            SDL_Color{team.r,team.g,team.b,Uint8(std::clamp(105.0f*envelope,0.0f,135.0f))});''',
'''        const float p=sfKineticWaveProgressAt(wave,wave.age);\n        const Uint8 frontAlpha=sfKineticWaveFrontAlpha(p,wave.strength);\n        if(frontAlpha<3) continue;\n        if(sfKineticSurgeVisible(wave.owner)) {\n            const float phase=std::fmod(float(SDL_GetTicks64())*.00042f+wave.owner*.17f+p*.55f,1.0f);\n            sfTacticalRainbowRing(renderer,tupl(ship->x,ship->y),radius,phase,frontAlpha);\n            sfTacticalRainbowRing(renderer,tupl(ship->x,ship->y),std::max(1.0f,radius-3.0f),phase+.16f,Uint8(frontAlpha*.72f));\n            sfTacticalRainbowRing(renderer,tupl(ship->x,ship->y),std::max(1.0f,radius-6.0f),phase+.31f,Uint8(frontAlpha*.38f));\n        } else {\n            const SDL_Color team=wave.owner==0 ? SDL_Color{255,188,96,255} : SDL_Color{96,210,255,255};\n            sfTacticalRing(renderer,tupl(ship->x,ship->y),radius,SDL_Color{team.r,team.g,team.b,frontAlpha});\n            sfTacticalRing(renderer,tupl(ship->x,ship->y),std::max(1.0f,radius-3.0f),SDL_Color{team.r,team.g,team.b,Uint8(frontAlpha*.68f)});\n            sfTacticalRing(renderer,tupl(ship->x,ship->y),std::max(1.0f,radius-6.0f),SDL_Color{255,255,255,Uint8(frontAlpha*.30f)});\n        }''')

# Home UI: names only; each touch cycles selector; selector and launch hit boxes are disjoint.
literal('src/start_ui.hpp',
'''    const std::string dangerText = std::string("< DANGER BOSS : X") +\n        std::to_string(int(sfBossDangerMultiplier())) + " >";''',
'''    const std::string dangerText = std::string("< DANGER BOSS : ") + sfBossDangerName() + " >";''')
text=read('src/start_ui.hpp').replace('sfBossDangerAdjust(x < .5f ? -1 : 1);','sfBossDangerNext();')
write('src/start_ui.hpp',text)
literal('src/remaster_ai_fix.hpp',
'''                if (y >= 0.39f && y <= 0.52f) {\n                    sfSelectedMode=(sfSelectedMode+1)%4;\n                    sfFixRequestedIa.store(sfModeHasAi(sfSelectedMode));\n                } else if (y >= 0.545f && y <= 0.69f) {\n                    sfFixLaunchPending.store(true);\n                } else if (y >= 0.71f && y <= 0.85f) {\n                    sfFixRequestedScreen.store(SF_UI_HELP);\n                }''',
'''                if (y >= 0.39f && y <= 0.505f) {\n                    sfSelectedMode=(sfSelectedMode+1)%4;\n                    sfFixRequestedIa.store(sfModeHasAi(sfSelectedMode));\n                } else if (y >= 0.515f && y <= 0.60f) {\n                    sfBossDangerNext();\n                } else if (y >= 0.615f && y <= 0.715f) {\n                    sfFixLaunchPending.store(true);\n                } else if (y >= 0.735f && y <= 0.85f) {\n                    sfFixRequestedScreen.store(SF_UI_HELP);\n                }''')

print('Applied kinetic balance v3b gameplay changes')
