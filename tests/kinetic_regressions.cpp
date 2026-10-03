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
