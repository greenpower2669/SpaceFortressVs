#!/usr/bin/env python3
from pathlib import Path
import sys

ROOT=Path(__file__).resolve().parents[1]

def read(p): return (ROOT/p).read_text()
def write(p,s): (ROOT/p).write_text(s)
def replace_once(text, old, new, label):
    n=text.count(old)
    if n!=1: raise SystemExit(f"{label}: expected 1 occurrence, found {n}")
    return text.replace(old,new,1)
def replace_between(text,start,end,repl,label):
    a=text.find(start)
    if a<0: raise SystemExit(f"{label}: start not found")
    b=text.find(end,a)
    if b<0: raise SystemExit(f"{label}: end not found")
    return text[:a]+repl+text[b:]
def append_once(path, marker, block):
    t=read(path)
    if marker not in t:
        if not t.endswith('\n'): t+='\n'
        t+='\n'+block.strip()+'\n'
        write(path,t)

RED_CPP=r'''#include <cassert>
#include "kinetic_shield.hpp"
int main(){
    const float ref=sfKineticReferenceSpeed(780.0f);
    const auto almostStill=sfResolveKinetic(100.0f,2.0f,{0,0},{0,0},0,-1,ref);
    // New contract: nearly stationary mass must not keep the historical full mass damage.
    assert(almostStill.rawDamage < 5.0f);
    return 0;
}
'''
RED_PY=r'''from pathlib import Path
root=Path(__file__).resolve().parents[1]
k=(root/'src/kinetic_shield.hpp').read_text()
c=(root/'src/campaign_runtime.hpp').read_text()
t=(root/'src/tactical_runtime.hpp').read_text()
assert 'SF_KINETIC_COOP_DAMAGE_MULTIPLIER' not in k
assert 'SF_KINETIC_COOP_DAMAGE_MULTIPLIER' not in c
assert 'sfKineticWaves' in t
'''

FINAL_HEADER=r'''#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

// Shared kinetic model for classic and coop/campaign. Velocities are pixels/second.
// Gameplay energy remains the historical shared nrj reserve; no new visible heat system exists.
constexpr float SF_KINETIC_PASS_SPEED_RATIO = .16f;
constexpr float SF_KINETIC_MEDIUM_SPEED_RATIO = .42f;
constexpr float SF_KINETIC_FAST_SPEED_RATIO = .72f;
constexpr float SF_KINETIC_VERY_FAST_SPEED_RATIO = 1.10f;
constexpr float SF_KINETIC_OUTER_DISSIPATION = .94f;
constexpr float SF_KINETIC_INNER_DISSIPATION = .78f;
constexpr float SF_KINETIC_MASS_DAMAGE_FLOOR = .01f;
constexpr float SF_KINETIC_ENERGY_COST_SCALE = .00001f; // 0.001% of the previous cost.
constexpr float SF_KINETIC_MIN_SHIELD_DIAMETER = 1.05f;
constexpr float SF_KINETIC_MAX_SHIELD_DIAMETER = 1.58f;
constexpr float SF_KINETIC_INNER_MAX_RADIUS_SHIP_DIAMETERS = .575f;
constexpr float SF_KINETIC_BOSS_BASE_DAMAGE = 50.0f;
constexpr float SF_KINETIC_WAVE_DURATION = .27f;

struct SfKineticVector { float x=0,y=0; };
enum class SfKineticLayer { None, Inner, Outer };
struct SfKineticSolution {
    SfKineticLayer suggestedLayer=SfKineticLayer::None;
    SfKineticLayer appliedLayer=SfKineticLayer::None;
    float relativeSpeed=0,impactSpeed=0,tangentSpeed=0,relativeRatio=0;
    float massFactor=0,speedFactor=0,rawDamage=0;
    int selectedRange=0;
    float maxRadiusShipDiameters=0;
    float dissipationFraction=0,dissipatedDamage=0,residualDamage=0;
    float legacyEnergyCost=0,energyCost=0;
};
struct SfKineticWave {
    int owner=-1;
    unsigned serial=0;
    float age=0,duration=SF_KINETIC_WAVE_DURATION,maxRadiusShipDiameters=0,strength=0;
    bool loggedMid=false,loggedRed=false,loggedWhite=false;
};
struct SfKineticDustMotion {
    float vx=0,vy=0;
    bool vibrated=false,deflected=false;
};
inline std::vector<SfKineticWave> sfKineticWaves;
inline unsigned sfKineticWaveSerial=0;

static float sfKineticReferenceSpeed(float arenaWidth)
{
    return std::max(1.0f,arenaWidth*.30f);
}
static float sfKineticReferenceArea(float arenaHeight)
{
    return std::max(1.0f,arenaHeight*arenaHeight*.008f);
}
static float sfKineticMassFactorFromArea(float area,float arenaHeight)
{
    return std::max(0.0f,area)/sfKineticReferenceArea(arenaHeight);
}
static float sfKineticBossMass(int encounter)
{
    const float p=std::clamp(encounter/199.0f,0.0f,1.0f);
    return 1.0f+1.8f*std::pow(p,1.25f);
}
static float sfKineticEnergyFraction(float reserveSpent)
{
    if (std::isnan(reserveSpent)) return 0;
    return 1.0f-std::clamp(reserveSpent/50.0f,0.0f,1.0f);
}
static float sfHullRegenPerSecond(float reserveSpent)
{
    const float energy=sfKineticEnergyFraction(reserveSpent);
    const float x=std::clamp((energy-.90f)/.10f,0.0f,1.0f);
    const float perfect=x*x*(3.0f-2.0f*x);
    return 6.0f*energy*energy+18.0f*perfect;
}
static int sfKineticSelectedRange(float relativeRatio)
{
    if(relativeRatio<SF_KINETIC_PASS_SPEED_RATIO) return 0;
    if(relativeRatio<SF_KINETIC_MEDIUM_SPEED_RATIO) return 1;
    if(relativeRatio<SF_KINETIC_FAST_SPEED_RATIO) return 2;
    if(relativeRatio<SF_KINETIC_VERY_FAST_SPEED_RATIO) return 3;
    return 4;
}
static float sfKineticMaxRadiusShipDiameters(float relativeRatio)
{
    if(relativeRatio<SF_KINETIC_PASS_SPEED_RATIO) return 0;
    const float t=std::clamp((relativeRatio-SF_KINETIC_PASS_SPEED_RATIO)/(1.35f-SF_KINETIC_PASS_SPEED_RATIO),0.0f,1.0f);
    const float smooth=t*t*(3.0f-2.0f*t);
    const float shieldDiameter=SF_KINETIC_MIN_SHIELD_DIAMETER+
        (SF_KINETIC_MAX_SHIELD_DIAMETER-SF_KINETIC_MIN_SHIELD_DIAMETER)*smooth;
    return shieldDiameter*.5f;
}
static float sfKineticInnerRadiusShipDiameters(const SfKineticSolution &s)
{
    return std::min(s.maxRadiusShipDiameters,SF_KINETIC_INNER_MAX_RADIUS_SHIP_DIAMETERS);
}
static SfKineticSolution sfResolveKinetic(float baseDamage,float massFactor,
                                SfKineticVector objectVelocity,
                                SfKineticVector shipVelocity,
                                float normalX,float normalY,
                                float referenceSpeed)
{
    SfKineticSolution out;
    const float nlen=std::sqrt(normalX*normalX+normalY*normalY);
    const float ux=nlen>.0001f ? normalX/nlen : 0;
    const float uy=nlen>.0001f ? normalY/nlen : -1;
    const float rvx=objectVelocity.x-shipVelocity.x;
    const float rvy=objectVelocity.y-shipVelocity.y;
    out.relativeSpeed=std::sqrt(rvx*rvx+rvy*rvy);
    out.impactSpeed=std::max(0.0f,-(rvx*ux+rvy*uy));
    out.tangentSpeed=std::sqrt(std::max(0.0f,out.relativeSpeed*out.relativeSpeed-out.impactSpeed*out.impactSpeed));
    referenceSpeed=std::max(1.0f,referenceSpeed);
    const float impactRatio=out.impactSpeed/referenceSpeed;
    out.relativeRatio=out.relativeSpeed/referenceSpeed;
    out.massFactor=std::max(0.0f,massFactor);
    // Small mass-only floor avoids a mathematically exact zero, but slow bodies now stay genuinely weak.
    out.speedFactor=SF_KINETIC_MASS_DAMAGE_FLOOR+impactRatio*impactRatio;
    out.rawDamage=std::max(0.0f,baseDamage)*out.massFactor*out.speedFactor;
    out.residualDamage=out.rawDamage;
    out.selectedRange=sfKineticSelectedRange(out.relativeRatio);
    out.maxRadiusShipDiameters=sfKineticMaxRadiusShipDiameters(out.relativeRatio);
    out.suggestedLayer=out.selectedRange>=3 ? SfKineticLayer::Outer :
                       out.selectedRange>0 ? SfKineticLayer::Inner : SfKineticLayer::None;
    return out;
}
static SfKineticSolution sfApplyKineticLayer(SfKineticSolution out,SfKineticLayer layer,float energyFraction)
{
    energyFraction=std::clamp(energyFraction,0.0f,1.0f);
    const float maximum=layer==SfKineticLayer::Outer ? SF_KINETIC_OUTER_DISSIPATION :
                        layer==SfKineticLayer::Inner ? SF_KINETIC_INNER_DISSIPATION : 0.0f;
    out.appliedLayer=layer;
    out.dissipationFraction=std::clamp(maximum*energyFraction,0.0f,.97f);
    out.dissipatedDamage=out.rawDamage*out.dissipationFraction;
    out.residualDamage=out.rawDamage-out.dissipatedDamage;
    if(out.dissipatedDamage>0) {
        out.legacyEnergyCost=std::min(4.0f,.04f+out.dissipatedDamage*.002f);
        out.energyCost=out.legacyEnergyCost*SF_KINETIC_ENERGY_COST_SCALE;
    }
    return out;
}
static void sfKineticTriggerWave(int owner,float maxRadiusShipDiameters,float strength=1.0f)
{
    if(owner<0 || owner>1 || maxRadiusShipDiameters<=0) return;
    if(sfKineticWaves.size()>=24) sfKineticWaves.erase(sfKineticWaves.begin());
    SfKineticWave w;w.owner=owner;w.serial=++sfKineticWaveSerial;
    w.maxRadiusShipDiameters=maxRadiusShipDiameters;w.strength=std::clamp(strength,0.0f,1.0f);
    sfKineticWaves.push_back(w);
}
static bool sfKineticWaveAlive(const SfKineticWave &wave)
{
    return wave.age<wave.duration;
}
static float sfKineticWaveProgressAt(const SfKineticWave &wave,float age)
{
    return std::clamp(age/std::max(.001f,wave.duration),0.0f,1.0f);
}
static float sfKineticWaveRadiusAt(const SfKineticWave &wave,float shipDiameter,float age)
{
    const float p=sfKineticWaveProgressAt(wave,age);
    const float fastExpansion=1.0f-(1.0f-p)*(1.0f-p);
    return std::max(0.0f,shipDiameter)*wave.maxRadiusShipDiameters*fastExpansion;
}
static void sfKineticAdvanceWaves(float dt)
{
    if(dt<=0) return;
    for(auto &wave:sfKineticWaves) wave.age+=dt;
    sfKineticWaves.erase(std::remove_if(sfKineticWaves.begin(),sfKineticWaves.end(),
        [](const auto &wave){return !sfKineticWaveAlive(wave);}),sfKineticWaves.end());
}
static SfKineticDustMotion sfKineticRespondDust(bool collectibleWhite,float vx,float vy,
                                        float normalX,float normalY,float strength,
                                        float variation,float arenaWidth)
{
    SfKineticDustMotion out{vx,vy,false,false};
    if(collectibleWhite || strength<=0) return out; // White resource dust is physically untouchable.
    const float nlen=std::sqrt(normalX*normalX+normalY*normalY);
    if(nlen<=.0001f) return out;
    const float nx=normalX/nlen,ny=normalY/nlen;
    variation=std::clamp(variation,0.0f,1.0f);
    strength=std::clamp(strength,0.0f,1.0f);
    const float radial=std::max(1.0f,arenaWidth)*(.018f+.032f*variation)*strength;
    const float tangent=std::max(1.0f,arenaWidth)*.014f*(variation-.5f)*strength;
    out.vx+=nx*radial-ny*tangent;
    out.vy+=ny*radial+nx*tangent;
    out.vibrated=true;out.deflected=true;
    return out;
}
'''

FINAL_TEST=r'''#include <cassert>
#include <cmath>
#include "kinetic_shield.hpp"
int main() {
    const float ref=sfKineticReferenceSpeed(780.0f);
    const auto still=sfResolveKinetic(100.0f,2.0f,{0,0},{0,0},0,-1,ref);
    const auto slow=sfResolveKinetic(100.0f,1.0f,{0,ref*.20f},{0,0},0,-1,ref);
    const auto medium=sfResolveKinetic(100.0f,1.0f,{0,ref*.55f},{0,0},0,-1,ref);
    const auto fast=sfResolveKinetic(100.0f,1.0f,{0,ref*.90f},{0,0},0,-1,ref);
    const auto veryFast=sfResolveKinetic(100.0f,1.0f,{0,ref*1.50f},{0,0},0,-1,ref);
    assert(still.rawDamage>0 && still.rawDamage<5.0f);
    assert(slow.rawDamage<medium.rawDamage && medium.rawDamage<fast.rawDamage && fast.rawDamage<veryFast.rawDamage);
    assert(slow.selectedRange==1 && medium.selectedRange==2 && fast.selectedRange==3 && veryFast.selectedRange==4);
    assert(slow.maxRadiusShipDiameters<medium.maxRadiusShipDiameters);
    assert(medium.maxRadiusShipDiameters<fast.maxRadiusShipDiameters);
    assert(fast.maxRadiusShipDiameters<veryFast.maxRadiusShipDiameters);
    assert(veryFast.maxRadiusShipDiameters<=SF_KINETIC_MAX_SHIELD_DIAMETER*.5f+.001f);
    const auto pass=sfResolveKinetic(100.0f,20.0f,{0,ref*.05f},{0,0},0,-1,ref);
    assert(pass.selectedRange==0 && pass.suggestedLayer==SfKineticLayer::None && pass.rawDamage<30.0f);

    const auto shielded=sfApplyKineticLayer(veryFast,SfKineticLayer::Outer,1.0f);
    assert(shielded.residualDamage<veryFast.rawDamage);
    assert(shielded.legacyEnergyCost>0 && shielded.energyCost>0);
    assert(std::abs(shielded.energyCost/shielded.legacyEnergyCost-SF_KINETIC_ENERGY_COST_SCALE)<1e-8f);

    const auto white=sfKineticRespondDust(true,12,-5,1,0,1,.7f,780);
    assert(white.vx==12 && white.vy==-5 && !white.vibrated && !white.deflected);
    const auto red=sfKineticRespondDust(false,12,-5,1,0,1,.7f,780);
    assert(red.vibrated && red.deflected && (red.vx!=12 || red.vy!=-5));

    sfKineticWaves.clear();sfKineticWaveSerial=0;
    sfKineticTriggerWave(0,veryFast.maxRadiusShipDiameters,.9f);
    sfKineticTriggerWave(0,fast.maxRadiusShipDiameters,.6f);
    assert(sfKineticWaves.size()==2);
    const auto first=sfKineticWaves.front();
    assert(sfKineticWaveRadiusAt(first,100,0)==0);
    const float mid=sfKineticWaveRadiusAt(first,100,first.duration*.5f);
    const float nearEnd=sfKineticWaveRadiusAt(first,100,first.duration*.99f);
    assert(mid>0 && nearEnd>mid);
    sfKineticAdvanceWaves(first.duration*1.01f);
    assert(sfKineticWaves.empty());

    assert(sfKineticBossMass(199)>sfKineticBossMass(0));
    assert(sfHullRegenPerSecond(0)>sfHullRegenPerSecond(25));
    assert(sfHullRegenPerSecond(25)>sfHullRegenPerSecond(50));
    assert(std::abs(sfHullRegenPerSecond(0)-24.0f)<.01f);
    return 0;
}
'''

FINAL_INTEGRATION=r'''from pathlib import Path
root=Path(__file__).resolve().parents[1]
k=(root/'src/kinetic_shield.hpp').read_text()
c=(root/'src/campaign_runtime.hpp').read_text()
l=(root/'src/legacy_field_runtime.hpp').read_text()
t=(root/'src/tactical_runtime.hpp').read_text()
main=(root/'src/main.cpp').read_text()
assert 'SF_KINETIC_COOP_DAMAGE_MULTIPLIER' not in k
assert 'SF_KINETIC_COOP_DAMAGE_MULTIPLIER' not in c
assert 'static_assert(SF_COOP_INCOMING_DAMAGE_MULTIPLIER' not in c
# Non-kinetic coop attacks keep x15.
assert 'const float incoming=SF_COOP_INCOMING_DAMAGE_MULTIPLIER*damage;' in c
assert 'const float incomingPerSecond=SF_COOP_INCOMING_DAMAGE_MULTIPLIER*650.0f;' in c
# Kinetic asteroid and boss charge paths do not use x15.
assert 'static void sfCoopAsteroidHurt' in c and 'const float incoming=legacyDamage;' in c
charge=c[c.index('static void sfCoopBossContact'):c.index('static void sfCoopAsteroidHurt')]
assert 'SF_COOP_INCOMING_DAMAGE_MULTIPLIER*solved.residualDamage' not in charge
assert 'const float incoming=solved.residualDamage;' in charge
# Both classic and coop share the legacy field resolver; no permanent pulse rings remain.
assert 'sfLegacyFieldFrame' in l and 'sfKineticTryLayer' in l
assert 'sfKineticWaves' in t and 'sfKineticWaveRadiusAt' in t
assert 'pulse.outer' not in t and 'pulse.inner' not in t
# White dust is observed for diagnostics only and is never fed through the physical response helper.
assert 'sfKineticRespondDust(false' in l
assert 'sfKineticRespondDust(true' not in l
assert 'type=white' in l and 'deflected=false' in l
assert 'KINETIC_IMPACT' in l and 'KINETIC_WAVE' in l and 'KINETIC_DUST' in l
assert 'KINETIC_IMPACT' in c
# Historical source is still not rewritten by this feature.
assert '#include <iostream>' in main
'''

NEW_UPDATE_EFFECTS=r'''static void sfKineticUpdateEffects(float dt)
{
    if(dt<=0) return;
    for(auto &wave:sfKineticWaves) {
        if(wave.owner<0 || wave.owner>1) continue;
        auto *ship=wave.owner==0 ? Spritej1 : Spritej2;
        if(!ship || ship->pv<=0) continue;
        const float diameter=sfKineticShipDiameter(ship);
        const float previousRadius=sfKineticWaveRadiusAt(wave,diameter,wave.age);
        const float nextAge=std::min(wave.duration,wave.age+dt);
        const float currentRadius=sfKineticWaveRadiusAt(wave,diameter,nextAge);
        const float shellMin=std::max(0.0f,std::min(previousRadius,currentRadius)-diameter*.055f);
        const float shellMax=std::max(previousRadius,currentRadius)+diameter*.055f;
        const float progress=sfKineticWaveProgressAt(wave,nextAge);
        if(!wave.loggedMid && progress>=.50f) {
            SDL_Log("KINETIC_WAVE owner=%d start=0 currentRadius=%.3f maxRadius=%.3f impactStrength=%.3f",
                wave.owner,currentRadius,diameter*wave.maxRadiusShipDiameters,wave.strength);
            wave.loggedMid=true;
        }
        int index=0;
        for(auto *dust:particulesr) {
            if(!dust || dust->pv<=0) {++index;continue;}
            const float dx=dust->x-ship->x,dy=dust->y-ship->y,d=std::max(1.0f,vlong(dx,dy));
            if(d<shellMin || d>shellMax) {++index;continue;}
            const float beforeVx=dust->vx,beforeVy=dust->vy;
            const float variation=.5f+.5f*std::sin(index*1.73f+wave.serial*.61f+d*.019f);
            const auto response=sfKineticRespondDust(false,dust->vx,dust->vy,dx/d,dy/d,wave.strength,variation,sfArenaW);
            dust->vx=response.vx;dust->vy=response.vy;
            const float wobble=std::sin(wave.serial*.77f+index*1.31f+progress*18.0f)*diameter*.018f*wave.strength;
            dust->x+=(-dy/d)*wobble;dust->y+=(dx/d)*wobble;
            if(!wave.loggedRed) {
                SDL_Log("KINETIC_DUST type=red vibrated=%s deflected=%s velocityBefore=(%.3f,%.3f) velocityAfter=(%.3f,%.3f)",
                    response.vibrated?"true":"false",response.deflected?"true":"false",beforeVx,beforeVy,dust->vx,dust->vy);
                wave.loggedRed=true;
            }
            ++index;
        }
        if(!wave.loggedWhite) for(auto *dust:particules) {
            if(!dust || dust->pv<=0) continue;
            const float d=vlong(dust->x-ship->x,dust->y-ship->y);
            if(d<shellMin || d>shellMax) continue;
            SDL_Log("KINETIC_DUST type=white vibrated=false deflected=false velocityBefore=(%.3f,%.3f) velocityAfter=(%.3f,%.3f)",
                dust->vx,dust->vy,dust->vx,dust->vy);
            wave.loggedWhite=true;break;
        }
    }
    sfKineticAdvanceWaves(dt);
}
'''

NEW_TRY_LAYER=r'''static bool sfKineticTryLayer(sprite *rock,sprite *ship,int owner,SfKineticLayer layer,
                    const SfKineticSolution &raw)
{
    const auto solved=sfApplyKineticLayer(raw,layer,sfKineticEnergyFraction(ship->nrj));
    if(solved.dissipationFraction<=.001f) return false;
    sfAddShipHeat(ship,solved.energyCost);
    const float strength=std::clamp(.28f+solved.dissipationFraction*.72f,0.0f,1.0f);
    sfKineticTriggerWave(owner,raw.maxRadiusShipDiameters,strength);
    const float diameter=sfKineticShipDiameter(ship);
    sfKineticEmitRedDust(owner,layer==SfKineticLayer::Outer ? 7 : 4,diameter*.46f);
    SDL_Log("KINETIC_IMPACT owner=%d massFactor=%.5f relativeSpeed=%.3f impactSpeed=%.3f rawDamage=%.5f residualDamage=%.5f selectedRange=%d maxRadius=%.3f energyCost=%.8f",
        owner,raw.massFactor,raw.relativeSpeed,raw.impactSpeed,raw.rawDamage,solved.residualDamage,
        raw.selectedRange,diameter*raw.maxRadiusShipDiameters,solved.energyCost);
    SDL_Log("KINETIC_WAVE owner=%d start=0 currentRadius=0 maxRadius=%.3f impactStrength=%.3f",
        owner,diameter*raw.maxRadiusShipDiameters,strength);
    if(sfKineticFragmentRock(rock,ship,owner,layer,solved)) return true;
    // Population cap fallback: preserve matter, shed kinetic speed and deflect it sideways.
    const float dx=rock->x-ship->x,dy=rock->y-ship->y,d=std::max(1.0f,vlong(dx,dy));
    const float retained=std::sqrt(std::clamp(solved.residualDamage/std::max(.001f,solved.rawDamage),.02f,1.0f));
    const float wx=rock->vx*retained,wy=rock->vy*retained;
    rock->vx=(-dy/d)*vlong(wx,wy)*(owner ? -1.0f : 1.0f);rock->vy=(dx/d)*vlong(wx,wy)*(owner ? -1.0f : 1.0f);
    rock->kineticStage=layer==SfKineticLayer::Outer ? 1 : 2;
    return false;
}

'''

NEW_FIELD_OWNER_BLOCK=r'''  const float diameter=sfKineticShipDiameter(ship),rockRadius=std::max(rock->w,rock->h)*.5f;
  const auto raw=sfKineticRockSolution(rock,ship,owner);
  const float travelled=sfKineticSegmentDistance(fromX,fromY,rock->x,rock->y,ship->x,ship->y);
  const float outer=diameter*raw.maxRadiusShipDiameters+rockRadius;
  const float inner=diameter*sfKineticInnerRadiusShipDiameters(raw)+rockRadius;
  if(rock->kineticStage==0 && raw.suggestedLayer==SfKineticLayer::Outer && travelled<=outer) {
      rock->kineticStage=1;
      if(sfKineticTryLayer(rock,ship,owner,SfKineticLayer::Outer,raw)) continue;
  }
  if(rock->pv<=0) continue;
  if(rock->kineticStage<2 && raw.selectedRange>0 && travelled<=inner) {
      rock->kineticStage=2;
      if(sfKineticTryLayer(rock,ship,owner,SfKineticLayer::Inner,raw)) continue;
  }
  if(rock->pv<=0) continue;
  const float hullRadius=std::max(ship->sw,ship->sh)*.52f+rockRadius;
  const bool hullHit=colee(ship,rock) || travelled<=hullRadius;
  if(!hullHit) continue;
'''

NEW_COLLISION_TAIL=r'''  sfFieldCollisionSound=true;sfFieldImpact(rock,ship);
  sfKineticEmitRedDust(owner,3,diameter*.45f);
  SDL_Log("KINETIC_IMPACT owner=%d massFactor=%.5f relativeSpeed=%.3f impactSpeed=%.3f rawDamage=%.5f residualDamage=%.5f selectedRange=%d maxRadius=%.3f energyCost=0",
      owner,raw.massFactor,raw.relativeSpeed,raw.impactSpeed,raw.rawDamage,raw.rawDamage,
      raw.selectedRange,diameter*raw.maxRadiusShipDiameters);
  if (hurt) hurt(owner,raw.rawDamage);
  else ship->pv=std::max(0.0f,ship->pv-sfApplyShieldImpact(ship,raw.rawDamage));
  rock->pv=0;
'''

NEW_DRAW=r'''static void sfDrawKineticEffects(SDL_Renderer *renderer)
{
    if (!renderer || sfUiScreen!=SF_UI_GAME || sfKineticWaves.empty()) return;
    SDL_BlendMode previous;SDL_GetRenderDrawBlendMode(renderer,&previous);
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    for(const auto &wave:sfKineticWaves) {
        if(wave.owner<0 || wave.owner>1 || !sfKineticWaveAlive(wave)) continue;
        const auto *ship=wave.owner==0 ? Spritej1 : Spritej2;
        if(!ship || ship->pv<=0) continue;
        const float diameter=std::max(1.0f,std::max({ship->sw,ship->sh,ship->w,ship->h}));
        const float radius=sfKineticWaveRadiusAt(wave,diameter,wave.age);
        if(radius<=0) continue;
        const float p=sfKineticWaveProgressAt(wave,wave.age);
        const float envelope=std::sin(float(PI)*p)*wave.strength;
        if(envelope<=.01f) continue;
        const SDL_Color team=wave.owner==0 ? SDL_Color{255,188,96,255} : SDL_Color{96,210,255,255};
        sfTacticalRing(renderer,tupl(ship->x,ship->y),radius,
            SDL_Color{team.r,team.g,team.b,Uint8(std::clamp(190.0f*envelope,0.0f,220.0f))});
    }
    SDL_SetRenderDrawBlendMode(renderer,previous);
}

'''

NEW_BOSS_CONTACT=r'''static void sfCoopBossContact(int owner,float dt)
{
    auto *ship=sfCoopShip(owner);
    if (ship->pv<=0 || sfCoop.phase!=SfCoopPhase::Combat || dt<=0) return;
    if(sfCoop.chargeActive) {
        // Only explicit boss charges are kinetic. Ordinary body contact remains on the historical path.
        if(!sfCoopChargeImpactActive() || sfCoop.chargeHit[owner]) return;
        const float dx=sfCoop.position.x-ship->x,dy=sfCoop.position.y-ship->y,d=std::max(1.0f,vlong(dx,dy));
        const auto raw=sfResolveKinetic(SF_KINETIC_BOSS_BASE_DAMAGE,sfKineticBossMass(sfCoop.encounter),
            {sfCoop.motion.velocity.vx,sfCoop.motion.velocity.vy},
            {sfObserved[owner].velocity.vx,sfObserved[owner].velocity.vy},dx/d,dy/d,sfKineticReferenceSpeed(sfArenaW));
        auto solved=raw;
        if(raw.suggestedLayer!=SfKineticLayer::None)
            solved=sfApplyKineticLayer(raw,raw.suggestedLayer,sfKineticEnergyFraction(ship->nrj));
        sfAddShipHeat(ship,solved.energyCost);
        const float incoming=solved.residualDamage; // Never apply coop x15 to kinetic damage.
        ship->pv=std::max(0.0f,ship->pv-sfApplyShieldImpact(ship,incoming));
        const float strength=std::clamp(.25f+solved.dissipationFraction*.75f,0.0f,1.0f);
        if(raw.maxRadiusShipDiameters>0) {
            sfKineticTriggerWave(owner,raw.maxRadiusShipDiameters,strength);
            SDL_Log("KINETIC_WAVE owner=%d start=0 currentRadius=0 maxRadius=%.3f impactStrength=%.3f",
                owner,sfKineticShipDiameter(ship)*raw.maxRadiusShipDiameters,strength);
        }
        SDL_Log("KINETIC_IMPACT owner=%d massFactor=%.5f relativeSpeed=%.3f impactSpeed=%.3f rawDamage=%.5f residualDamage=%.5f selectedRange=%d maxRadius=%.3f energyCost=%.8f",
            owner,raw.massFactor,raw.relativeSpeed,raw.impactSpeed,raw.rawDamage,solved.residualDamage,
            raw.selectedRange,sfKineticShipDiameter(ship)*raw.maxRadiusShipDiameters,solved.energyCost);
        sfCoopEmitRedDust(owner,6);
        sfCoop.chargeHit[owner]=true;sfCoop.soundHit=true;
        return;
    }
    const float incomingPerSecond=SF_COOP_INCOMING_DAMAGE_MULTIPLIER*650.0f;
    ship->pv=std::max(0.0f,ship->pv-sfApplyShieldContinuousImpact(ship,incomingPerSecond,dt));
    sfCoop.soundHit=true;sfCoopEmitRedDust(owner,2);
}
static void sfCoopAsteroidHurt(int owner,float legacyDamage)
{
    auto *ship=sfCoopShip(owner);
    if (ship->pv<=0 || sfCoop.phase!=SfCoopPhase::Combat) return;
    const float incoming=legacyDamage; // Kinetic asteroid damage is shared with classic: no coop x15.
    ship->pv=std::max(0.0f,ship->pv-sfApplyShieldImpact(ship,incoming));
    sfCoop.soundHit=true;
}

'''

def stage_red():
    write('tests/kinetic_rebalance_red.cpp',RED_CPP)
    write('tests/test_kinetic_rebalance_red.py',RED_PY)

def apply_final():
    for p in ['tests/kinetic_rebalance_red.cpp','tests/test_kinetic_rebalance_red.py']:
        q=ROOT/p
        if q.exists(): q.unlink()
    write('src/kinetic_shield.hpp',FINAL_HEADER)
    write('tests/kinetic_regressions.cpp',FINAL_TEST)
    write('tests/test_kinetic_integration.py',FINAL_INTEGRATION)

    # Ensure the full suite runs the static integration contract.
    p='scripts/test-regressions.sh';t=read(p)
    marker='python3 "$sf_repo/tests/test_scenic_integration.py"\n'
    if 'test_kinetic_integration.py' not in t:
        t=replace_once(t,marker,marker+'python3 "$sf_repo/tests/test_kinetic_integration.py"\n','kinetic integration hook')
    write(p,t)

    # Shared asteroid field: waves, compact speed-dependent ranges, red-dust response, no kinetic x15.
    p='src/legacy_field_runtime.hpp';t=read(p)
    t=replace_between(t,'static void sfKineticUpdateEffects(float dt)\n','static bool sfKineticFragmentRock',NEW_UPDATE_EFFECTS,'wave update')
    t=replace_between(t,'static bool sfKineticTryLayer(','static void sfLegacyFieldStep',NEW_TRY_LAYER,'kinetic layer')
    old_block='''  const float diameter=sfKineticShipDiameter(ship),rockRadius=std::max(rock->w,rock->h)*.5f;\n  const float outer=diameter*SF_KINETIC_OUTER_RADIUS_DIAMETERS+rockRadius;\n  const float inner=diameter*SF_KINETIC_INNER_RADIUS_DIAMETERS+rockRadius;\n  const auto raw=sfKineticRockSolution(rock,ship,owner);\n  const float multiplier=hurt ? SF_KINETIC_COOP_DAMAGE_MULTIPLIER : 1.0f;\n  if(rock->kineticStage==0 && raw.suggestedLayer==SfKineticLayer::Outer &&\n     sfKineticSegmentDistance(fromX,fromY,rock->x,rock->y,ship->x,ship->y)<=outer) {\n      rock->kineticStage=1;\n      if(sfKineticTryLayer(rock,ship,owner,SfKineticLayer::Outer,raw,multiplier)) continue;\n  }\n  if(rock->pv<=0) continue;\n  if(rock->kineticStage<2 && raw.suggestedLayer!=SfKineticLayer::None &&\n     sfKineticSegmentDistance(fromX,fromY,rock->x,rock->y,ship->x,ship->y)<=inner) {\n      rock->kineticStage=2;\n      if(sfKineticTryLayer(rock,ship,owner,SfKineticLayer::Inner,raw,multiplier)) continue;\n  }\n  if(rock->pv<=0) continue;\n  const float hullRadius=std::max(ship->sw,ship->sh)*.52f+rockRadius;\n  const bool hullHit=colee(ship,rock) ||\n      sfKineticSegmentDistance(fromX,fromY,rock->x,rock->y,ship->x,ship->y)<=hullRadius;\n  if(!hullHit) continue;\n'''
    t=replace_once(t,old_block,NEW_FIELD_OWNER_BLOCK,'field owner block')
    old_tail='''  sfFieldCollisionSound=true;sfFieldImpact(rock,ship);\n  sfKineticTriggerPulse(owner,SfKineticLayer::Inner,.42f);\n  sfKineticEmitRedDust(owner,3,diameter*.62f);\n  if (hurt) hurt(owner,raw.rawDamage);\n  else ship->pv=std::max(0.0f,ship->pv-sfApplyShieldImpact(ship,raw.rawDamage));\n  rock->pv=0;\n'''
    t=replace_once(t,old_tail,NEW_COLLISION_TAIL,'hull collision tail')
    t=replace_once(t,'fragment->startup();sa1.push_back(fragment);',
'''fragment->startup();\n        if(i==0) SDL_Log("KINETIC_DUST type=debris vibrated=false deflected=true velocityBefore=(%.3f,%.3f) velocityAfter=(%.3f,%.3f)",\n            rock->vx,rock->vy,fragment->vx,fragment->vy);\n        sa1.push_back(fragment);''','debris diagnostic')
    write(p,t)

    # Renderer: transient centre-out waves only; no permanent max-radius rings.
    p='src/tactical_runtime.hpp';t=read(p)
    t=replace_once(t,'sfPilot=SfPilot{}; sfObserved={}; sfPickupGlow={}; sfTurrets={}; sfKineticPulses={};',
        'sfPilot=SfPilot{}; sfObserved={}; sfPickupGlow={}; sfTurrets={}; sfKineticWaves.clear(); sfKineticWaveSerial=0;','kinetic reset')
    t=replace_between(t,'static void sfDrawKineticEffects(SDL_Renderer *renderer)\n','static void sfCannonQuad',NEW_DRAW,'kinetic draw')
    write(p,t)

    # Campaign: preserve non-kinetic x15, remove it from asteroid and explicit boss-charge kinetics.
    p='src/campaign_runtime.hpp';t=read(p)
    t=t.replace('static_assert(SF_COOP_INCOMING_DAMAGE_MULTIPLIER==SF_KINETIC_COOP_DAMAGE_MULTIPLIER,"coop multiplier must be applied exactly once");\n','')
    t=replace_between(t,'static void sfCoopBossContact(int owner,float dt)\n','static void sfCoopMovePlayers(float dt)',NEW_BOSS_CONTACT,'boss/asteroid kinetic paths')
    write(p,t)

    # Living memories: mark the superseding contract and the exact separation of kinetic/non-kinetic damage.
    append_once('brain.md','D-140-15 kinetic rebalance',r'''## 2026-10-03 — D-140-15 kinetic rebalance
Fab supersedes D-140-14 kinetic tuning in BOTH classic and coop/campaign: kinetic damage has no coop x15, slow impacts are genuinely weak, interception distance grows continuously with relative speed, waves expand from the ship then disappear, energy cost is previous shared cost ×0.00001, red dust is perturbed but white resource dust is never physically deflected. Non-kinetic coop attacks keep their validated ×15.''')
    append_once('brainmap.md','D-140-15',r'''### D-140-15
`kinetic_shield.hpp` = shared low-speed-safe mass×v² core + compact speed ranges + transient multi-wave state. `legacy_field_runtime.hpp` = both-mode asteroid interception/fragmentation/red dust. `campaign_runtime.hpp` = charge-only boss kinetics without ×15; ordinary contact/projectiles keep non-kinetic ×15. White dust remains physically untouched.''')
    append_once('debughistorical.md','D-140-15',r'''## D-140-15 — Cinétique létal à faible vitesse
Cause confirmée: plancher `speedFactor>=1`, coop ×15 réinjecté dans astéroïdes/charges, coût de réserve trop élevé et anneaux rendus près du rayon final. Correctif: petit plancher masse 0.01 + vImpact², aucun ×15 cinétique, coût ×0.00001, plages compactes 1.05→1.58 diamètres de champ selon vitesse, vagues centre→extérieur→disparition, rouge dévié / blanc intact.''')
    append_once('todo.md','D-140-15',r'''## D-140-15 — Boucliers cinétiques v2
- [x] Deux modes: classique + coop/campagne sur le même cœur.
- [x] Retirer ×15 des astéroïdes et charges cinétiques uniquement.
- [x] Faible vitesse = faible dégât; petite composante masse résiduelle.
- [x] Interception compacte et variable avec la vitesse; très lent passe.
- [x] Vagues individuelles centre→rayon max→disparition, plusieurs simultanées possibles.
- [x] Coût énergétique cinétique = ancien coût ×0.00001.
- [x] Poussière rouge vibrée/déviée; poussière blanche jamais déviée.
- [x] Logs KINETIC_IMPACT/WAVE/DUST.
- [ ] Validation ressenti téléphone Fab avant toute merge/release.''')
    append_once('ordres-de-mission.md','D-140-15 — ordre canonique',r'''## D-140-15 — ordre canonique Fab (03/10/2026)
Remplace les réglages cinétiques incompatibles de D-140-14: BOTH modes, aucun ×15 cinétique, dégâts masse×vitesse d'impact² avec faible plancher masse, champs compacts dépendant de la vitesse, vagues transitoires, coût réserve ×0.00001, rouge réactif, blanc physiquement intangible, aucune merge/release avant test téléphone.''')

if __name__=='__main__':
    mode=sys.argv[1] if len(sys.argv)>1 else ''
    if mode=='--red': stage_red()
    elif mode=='--apply': apply_final()
    else: raise SystemExit('usage: apply-kinetic-rebalance.py --red|--apply')
