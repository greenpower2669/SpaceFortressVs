#pragma once
#include <algorithm>
#include <array>
#include <cmath>

// Shared kinetic model for duel and campaign. Velocities are pixels/second.
// Kinetic damage is mass * velocity^2, normalized around the historical rock
// damage so a nearly stationary rock still has a non-zero mass-only impact.
constexpr float SF_KINETIC_COOP_DAMAGE_MULTIPLIER = 15.0f;
constexpr float SF_KINETIC_OUTER_SPEED_RATIO = 1.35f;
constexpr float SF_KINETIC_INNER_SPEED_RATIO = .55f;
constexpr float SF_KINETIC_OUTER_DISSIPATION = .94f;
constexpr float SF_KINETIC_INNER_DISSIPATION = .78f;
constexpr float SF_KINETIC_OUTER_RADIUS_DIAMETERS = 2.0f;
constexpr float SF_KINETIC_INNER_RADIUS_DIAMETERS = 1.25f;
constexpr float SF_KINETIC_BOSS_BASE_DAMAGE = 50.0f;

struct SfKineticVector { float x=0,y=0; };
enum class SfKineticLayer { None, Inner, Outer };
struct SfKineticSolution {
    SfKineticLayer suggestedLayer=SfKineticLayer::None;
    SfKineticLayer appliedLayer=SfKineticLayer::None;
    float relativeSpeed=0,impactSpeed=0,tangentSpeed=0;
    float massFactor=0,speedFactor=1,rawDamage=0;
    float dissipationFraction=0,dissipatedDamage=0,residualDamage=0,heatCost=0;
};
struct SfKineticPulseState { float outer=0,inner=0,phase=0; };
inline std::array<SfKineticPulseState,2> sfKineticPulses{};

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
static float sfKineticEnergyFraction(float heat)
{
    if (std::isnan(heat)) return 0;
    return 1.0f-std::clamp(heat/50.0f,0.0f,1.0f);
}
static float sfHullRegenPerSecond(float heat)
{
    const float energy=sfKineticEnergyFraction(heat);
    const float x=std::clamp((energy-.90f)/.10f,0.0f,1.0f);
    const float perfect=x*x*(3.0f-2.0f*x);
    return 6.0f*energy*energy+18.0f*perfect;
}
static SfKineticSolution sfResolveKinetic(float baseDamage,float massFactor,
                                SfKineticVector objectVelocity,
                                SfKineticVector shipVelocity,
                                float normalX,float normalY,
                                float referenceSpeed)
{
    SfKineticSolution out;
    const float nx=normalX,ny=normalY;
    const float nlen=std::sqrt(nx*nx+ny*ny);
    const float ux=nlen>.0001f ? nx/nlen : 0;
    const float uy=nlen>.0001f ? ny/nlen : -1;
    const float rvx=objectVelocity.x-shipVelocity.x;
    const float rvy=objectVelocity.y-shipVelocity.y;
    out.relativeSpeed=std::sqrt(rvx*rvx+rvy*rvy);
    out.impactSpeed=std::max(0.0f,-(rvx*ux+rvy*uy));
    out.tangentSpeed=std::sqrt(std::max(0.0f,out.relativeSpeed*out.relativeSpeed-out.impactSpeed*out.impactSpeed));
    referenceSpeed=std::max(1.0f,referenceSpeed);
    const float impactRatio=out.impactSpeed/referenceSpeed;
    const float relativeRatio=out.relativeSpeed/referenceSpeed;
    out.massFactor=std::max(0.0f,massFactor);
    out.speedFactor=std::max(1.0f,impactRatio*impactRatio);
    out.rawDamage=std::max(0.0f,baseDamage)*out.massFactor*out.speedFactor;
    out.residualDamage=out.rawDamage;
    out.suggestedLayer=relativeRatio>=SF_KINETIC_OUTER_SPEED_RATIO ? SfKineticLayer::Outer :
             relativeRatio>=SF_KINETIC_INNER_SPEED_RATIO ? SfKineticLayer::Inner : SfKineticLayer::None;
    return out;
}
static SfKineticSolution sfApplyKineticLayer(SfKineticSolution out,SfKineticLayer layer,
                                    float energyFraction,float damageMultiplier=1.0f)
{
    energyFraction=std::clamp(energyFraction,0.0f,1.0f);
    const float maximum=layer==SfKineticLayer::Outer ? SF_KINETIC_OUTER_DISSIPATION :
              layer==SfKineticLayer::Inner ? SF_KINETIC_INNER_DISSIPATION : 0.0f;
    out.appliedLayer=layer;
    out.dissipationFraction=std::clamp(maximum*energyFraction,0.0f,.97f);
    out.dissipatedDamage=out.rawDamage*out.dissipationFraction;
    out.residualDamage=out.rawDamage-out.dissipatedDamage;
    if (out.dissipatedDamage>0) {
        const float weighted=out.dissipatedDamage*std::max(0.0f,damageMultiplier);
        out.heatCost=std::min(4.0f,.04f+weighted*.002f);
    }
    return out;
}
static void sfKineticTriggerPulse(int owner,SfKineticLayer layer,float strength=1.0f)
{
    if (owner<0 || owner>1) return;
    strength=std::clamp(strength,0.0f,1.0f);
    if (layer==SfKineticLayer::Outer) sfKineticPulses[owner].outer=std::max(sfKineticPulses[owner].outer,strength);
    if (layer==SfKineticLayer::Inner) sfKineticPulses[owner].inner=std::max(sfKineticPulses[owner].inner,strength);
}
