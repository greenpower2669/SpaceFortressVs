#include <cassert>
#include <cmath>
#include "kinetic_shield.hpp"
int main() {
    const float ref=sfKineticReferenceSpeed(780.0f);
    const auto still=sfResolveKinetic(100.0f,2.0f,{0,0},{0,0},0,-1,ref);
    const auto slow=sfResolveKinetic(100.0f,1.0f,{0,ref*.20f},{0,0},0,-1,ref);
    const auto medium=sfResolveKinetic(100.0f,1.0f,{0,ref*.55f},{0,0},0,-1,ref);
    const auto fast=sfResolveKinetic(100.0f,1.0f,{0,ref*.90f},{0,0},0,-1,ref);
    const auto veryFast=sfResolveKinetic(100.0f,1.0f,{0,ref*1.50f},{0,0},0,-1,ref);
    assert(still.rawDamage<.001f);
    assert(slow.rawDamage<medium.rawDamage && medium.rawDamage<fast.rawDamage && fast.rawDamage<veryFast.rawDamage);
    assert(slow.selectedRange==1 && medium.selectedRange==2 && fast.selectedRange==3 && veryFast.selectedRange==4);
    assert(slow.maxRadiusShipDiameters<medium.maxRadiusShipDiameters);
    assert(medium.maxRadiusShipDiameters<fast.maxRadiusShipDiameters);
    assert(fast.maxRadiusShipDiameters<veryFast.maxRadiusShipDiameters);
    assert(veryFast.maxRadiusShipDiameters<=SF_KINETIC_MAX_SHIELD_DIAMETER*.5f+.001f);
    const auto pass=sfResolveKinetic(100.0f,1.0f,{0,ref*.05f},{0,0},0,-1,ref);
    assert(pass.selectedRange==0 && pass.suggestedLayer==SfKineticLayer::None && pass.rawDamage<10.0f);

    const auto shielded=sfApplyKineticLayer(veryFast,SfKineticLayer::Outer,1.0f);
    assert(shielded.residualDamage<veryFast.rawDamage);
    assert(shielded.legacyEnergyCost>0 && shielded.energyCost>0);
    assert(std::abs(shielded.energyCost/shielded.legacyEnergyCost-SF_KINETIC_ENERGY_COST_SCALE)<1e-8f);

    // Energy fatigue must be deliberately harsher than the old linear response.
    const auto half=sfApplyKineticLayer(veryFast,SfKineticLayer::Outer,.50f);
    const auto quarter=sfApplyKineticLayer(veryFast,SfKineticLayer::Outer,.25f);
    const auto tenth=sfApplyKineticLayer(veryFast,SfKineticLayer::Outer,.10f);
    const auto empty=sfApplyKineticLayer(veryFast,SfKineticLayer::Outer,0.0f);
    const float fullFraction=shielded.dissipationFraction;
    assert(std::abs(half.dissipationFraction/fullFraction-std::pow(.50f,1.2f))<.015f);
    assert(std::abs(quarter.dissipationFraction/fullFraction-std::pow(.25f,1.2f))<.015f);
    assert(std::abs(tenth.dissipationFraction/fullFraction-std::pow(.10f,1.2f))<.015f);
    assert(empty.dissipationFraction<.0001f);
    assert(tenth.dissipationFraction/fullFraction<.08f);
    const auto tenthSurge=sfApplyKineticLayer(veryFast,SfKineticLayer::Outer,.10f,SF_KINETIC_SURGE_POWER_MULTIPLIER);
    assert(std::abs(tenthSurge.dissipationFraction-tenth.dissipationFraction*2.0f)<.002f);

    const auto white=sfKineticRespondDust(true,12,-5,1,0,1,.7f,780);
    assert(white.vx==12 && white.vy==-5 && !white.vibrated && !white.deflected);
    const auto red=sfKineticRespondDust(false,12,-5,1,0,1,.7f,780);
    assert(red.vibrated && red.deflected && (red.vx!=12 || red.vy!=-5));

    sfKineticWaves.clear();sfKineticWaveSerial=0;
    sfKineticTriggerWave(0,veryFast.maxRadiusShipDiameters,1.0f,1.0f);
    sfKineticTriggerWave(0,fast.maxRadiusShipDiameters,1.0f,.10f);
    assert(sfKineticWaves.size()==2);
    const auto fullWave=sfKineticWaves.front();
    const auto tiredWave=sfKineticWaves.back();
    assert(std::abs(fullWave.energyFraction-1.0f)<.001f);
    assert(std::abs(tiredWave.energyFraction-.10f)<.001f);
    assert(fullWave.duration>=.26f && fullWave.duration<=.30f);
    assert(tiredWave.duration>=.47f && tiredWave.duration<=.52f);
    assert(tiredWave.duration>fullWave.duration);
    assert(sfKineticWaveRadiusAt(fullWave,100,0)==0);
    const float mid=sfKineticWaveRadiusAt(fullWave,100,fullWave.duration*.5f);
    const float nearEnd=sfKineticWaveRadiusAt(fullWave,100,fullWave.duration*.99f);
    assert(mid>0 && nearEnd>mid);
    sfKineticAdvanceWaves(std::max(fullWave.duration,tiredWave.duration)*1.01f);
    assert(sfKineticWaves.empty());

    assert(sfKineticBossMass(199)>sfKineticBossMass(0));
    assert(sfHullRegenPerSecond(0)>sfHullRegenPerSecond(25));
    assert(sfHullRegenPerSecond(25)>sfHullRegenPerSecond(50));
    assert(std::abs(sfHullRegenPerSecond(0)-24.0f)<.01f);
    return 0;
}
