#pragma once
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
constexpr float SF_KINETIC_ASTEROID_MAX_HULL_DAMAGE = 250.0f; // 25% of the canonical 1000 PV hull.
constexpr float SF_KINETIC_ENERGY_COST_SCALE = .00001f; // 0.001% of the previous cost.
constexpr float SF_KINETIC_MIN_SHIELD_DIAMETER = 1.05f;
constexpr float SF_KINETIC_MAX_SHIELD_DIAMETER = 2.0f;
constexpr float SF_KINETIC_INNER_MAX_RADIUS_SHIP_DIAMETERS = .575f;
constexpr float SF_KINETIC_BOSS_BASE_DAMAGE = 50.0f;
constexpr float SF_KINETIC_WAVE_DURATION = .27f;
constexpr float SF_KINETIC_LOW_ENERGY_WAVE_DURATION = .50f;
constexpr float SF_KINETIC_ENERGY_FATIGUE_EXPONENT = 1.20f;
constexpr float SF_KINETIC_LOW_ENERGY_WARNING_FRACTION = .10f;
constexpr float SF_KINETIC_LOW_ENERGY_FLASH_HZ = 4.5f;
constexpr float SF_KINETIC_SURGE_VISIBLE_DELAY_SECONDS = .30f;
constexpr float SF_KINETIC_SURGE_HOLD_SECONDS = 2.0f;
constexpr float SF_KINETIC_SURGE_BLAST_DIAMETER = 3.0f;
constexpr float SF_KINETIC_SURGE_POWER_MULTIPLIER = 2.0f;

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
    float energyFraction=1.0f;
    bool loggedMid=false,loggedRed=false,loggedWhite=false;
};
struct SfKineticDustMotion {
    float vx=0,vy=0;
    bool vibrated=false,deflected=false;
};
inline std::vector<SfKineticWave> sfKineticWaves;
inline unsigned sfKineticWaveSerial=0;
struct SfKineticSurgeState {
    bool held=false,charged=false;
    float heldSeconds=0;
};
inline std::array<SfKineticSurgeState,2> sfKineticSurges{};

static void sfKineticSurgePress(int owner)
{
    if(owner<0 || owner>1) return;
    auto &s=sfKineticSurges[owner];
    s=SfKineticSurgeState{};s.held=true;
}
static void sfKineticSurgeCancel(int owner)
{
    if(owner<0 || owner>1) return;
    sfKineticSurges[owner]=SfKineticSurgeState{};
}
static bool sfKineticSurgeRelease(int owner)
{
    if(owner<0 || owner>1) return false;
    const bool purge=sfKineticSurges[owner].charged;
    sfKineticSurges[owner]=SfKineticSurgeState{};
    return purge;
}
static void sfKineticAdvanceSurges(float dt)
{
    if(dt<=0) return;
    for(auto &s:sfKineticSurges) {
        if(!s.held) continue;
        s.heldSeconds+=dt;
        if(!s.charged && s.heldSeconds>=SF_KINETIC_SURGE_HOLD_SECONDS)
            s.charged=true;
    }
}
static float sfKineticSurgePower(int owner)
{
    if(owner<0 || owner>1) return 1.0f;
    const auto &s=sfKineticSurges[owner];
    if(!s.held || s.heldSeconds<SF_KINETIC_SURGE_VISIBLE_DELAY_SECONDS) return 1.0f;
    return s.charged ? 0.0f : SF_KINETIC_SURGE_POWER_MULTIPLIER;
}
static bool sfKineticSurgeVisible(int owner)
{
    if(owner<0 || owner>1) return false;
    const auto &s=sfKineticSurges[owner];
    return s.held && !s.charged && s.heldSeconds>=SF_KINETIC_SURGE_VISIBLE_DELAY_SECONDS;
}
static bool sfKineticSurgeVulnerable(int owner)
{
    if(owner<0 || owner>1) return false;
    const auto &s=sfKineticSurges[owner];
    return s.held && s.charged;
}
static void sfKineticResetSurges()
{
    sfKineticSurges={};
}
static int sfKineticPurgeAsteroids(int owner);

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
static float sfKineticEffectiveEnergyFraction(float energyFraction)
{
    energyFraction=std::clamp(energyFraction,0.0f,1.0f);
    return std::pow(energyFraction,SF_KINETIC_ENERGY_FATIGUE_EXPONENT);
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
    out.relativeRatio=out.relativeSpeed/referenceSpeed;
    out.massFactor=std::clamp(massFactor,0.0f,1.0f);
    const float speedRatio=std::clamp(out.relativeRatio,0.0f,1.0f);
    const float closingFactor=out.relativeSpeed>.0001f
        ? std::clamp(out.impactSpeed/out.relativeSpeed,0.0f,1.0f) : 0.0f;
    // Canon: linear mass x relative speed x actual inward/closing component.
    out.speedFactor=speedRatio*closingFactor;
    out.rawDamage=std::max(0.0f,baseDamage)*out.massFactor*out.speedFactor;
    out.residualDamage=out.rawDamage;
    out.selectedRange=sfKineticSelectedRange(out.relativeRatio);
    out.maxRadiusShipDiameters=sfKineticMaxRadiusShipDiameters(out.relativeRatio);
    out.suggestedLayer=out.selectedRange>=3 ? SfKineticLayer::Outer :
                       out.selectedRange>0 ? SfKineticLayer::Inner : SfKineticLayer::None;
    return out;
}
static SfKineticSolution sfApplyKineticLayer(SfKineticSolution out,SfKineticLayer layer,float energyFraction,float powerMultiplier=1.0f)
{
    energyFraction=std::clamp(energyFraction,0.0f,1.0f);
    const float effectiveEnergy=sfKineticEffectiveEnergyFraction(energyFraction);
    powerMultiplier=std::clamp(powerMultiplier,0.0f,SF_KINETIC_SURGE_POWER_MULTIPLIER);
    const float maximum=layer==SfKineticLayer::Outer ? SF_KINETIC_OUTER_DISSIPATION :
                        layer==SfKineticLayer::Inner ? SF_KINETIC_INNER_DISSIPATION : 0.0f;
    out.appliedLayer=layer;
    out.dissipationFraction=std::clamp(maximum*effectiveEnergy*powerMultiplier,0.0f,.995f);
    out.dissipatedDamage=out.rawDamage*out.dissipationFraction;
    out.residualDamage=out.rawDamage-out.dissipatedDamage;
    if(out.dissipatedDamage>0) {
        out.legacyEnergyCost=std::min(4.0f,.04f+out.dissipatedDamage*.002f);
        out.energyCost=out.legacyEnergyCost*SF_KINETIC_ENERGY_COST_SCALE;
    }
    return out;
}
static float sfKineticWaveDurationForEnergy(float energyFraction)
{
    energyFraction=std::clamp(energyFraction,0.0f,1.0f);
    return SF_KINETIC_WAVE_DURATION+
        (SF_KINETIC_LOW_ENERGY_WAVE_DURATION-SF_KINETIC_WAVE_DURATION)*(1.0f-energyFraction);
}
static void sfKineticTriggerWave(int owner,float maxRadiusShipDiameters,float strength=1.0f,float energyFraction=-1.0f)
{
    if(owner<0 || owner>1 || maxRadiusShipDiameters<=0) return;
    if(sfKineticWaves.size()>=24) sfKineticWaves.erase(sfKineticWaves.begin());
    SfKineticWave w;w.owner=owner;w.serial=++sfKineticWaveSerial;
    w.maxRadiusShipDiameters=maxRadiusShipDiameters;w.strength=std::clamp(strength,0.0f,1.0f);
    // Legacy three-argument callers encode effective field strength as .28 + .72*x.
    // New callers can pass the true energy fraction explicitly; the fallback keeps
    // old diagnostics and tests deterministic without changing collision timing.
    if(energyFraction<0)
        energyFraction=std::clamp((w.strength-.28f)/.72f,0.0f,1.0f);
    w.energyFraction=std::clamp(energyFraction,0.0f,1.0f);
    w.duration=sfKineticWaveDurationForEnergy(w.energyFraction);
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
