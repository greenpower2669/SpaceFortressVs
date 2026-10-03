#include <cassert>
#include <cmath>
#include "kinetic_shield.hpp"
int main() {
    const float ref=sfKineticReferenceSpeed(780.0f);
    const auto headOn=sfResolveKinetic(100.0f,1.0f,{0,300},{0,-200},0,-1,ref);
    const auto fleeing=sfResolveKinetic(100.0f,1.0f,{0,300},{0,200},0,-1,ref);
    assert(std::abs(headOn.impactSpeed-500.0f)<.001f);
    assert(std::abs(fleeing.impactSpeed-100.0f)<.001f);
    assert(headOn.rawDamage>fleeing.rawDamage);
    const auto still=sfResolveKinetic(100.0f,2.0f,{0,0},{0,0},0,-1,ref);
    assert(std::abs(still.speedFactor-1.0f)<.001f && std::abs(still.rawDamage-200.0f)<.001f);
    const auto tangent=sfResolveKinetic(100.0f,1.0f,{500,0},{0,0},0,-1,ref);
    assert(tangent.impactSpeed==0 && tangent.relativeSpeed>499 && tangent.suggestedLayer==SfKineticLayer::Outer);
    const auto outer=sfApplyKineticLayer(headOn,SfKineticLayer::Outer,1.0f,15.0f);
    const auto inner=sfApplyKineticLayer(headOn,SfKineticLayer::Inner,1.0f,15.0f);
    const auto empty=sfApplyKineticLayer(headOn,SfKineticLayer::Outer,0.0f,15.0f);
    assert(outer.residualDamage<inner.residualDamage && outer.heatCost>0 && outer.heatCost<=4.0f);
    assert(empty.dissipatedDamage==0 && empty.heatCost==0 && std::abs(empty.residualDamage-headOn.rawDamage)<.001f);
    assert(sfKineticBossMass(199)>sfKineticBossMass(0));
    assert(sfHullRegenPerSecond(0)>sfHullRegenPerSecond(25));
    assert(sfHullRegenPerSecond(25)>sfHullRegenPerSecond(50));
    assert(std::abs(sfHullRegenPerSecond(0)-24.0f)<.01f);
    return 0;
}
