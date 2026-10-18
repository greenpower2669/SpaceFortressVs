#include "../src/solo_wall_impact.hpp"
#include "../src/ship_energy_rules.hpp"
#include <cassert>
#include <cmath>
int main(){
    // A tangential slide, outward motion or a harmless low-speed touch never
    // spends either resource, regardless of total tangential velocity.
    const auto slide=sfsolo::wallPenalty(0,3000,-1,0,540);
    assert(slide.hull==0 && slide.energy==0);
    const auto outward=sfsolo::wallPenalty(-300,0,-1,0,540);
    assert(outward.hull==0 && outward.energy==0);
    const auto gentle=sfsolo::wallPenalty(90,0,-1,0,540);
    assert(gentle.hull==0 && gentle.energy==0);
    const auto medium=sfsolo::wallPenalty(540,0,-1,0,540);
    const auto severe=sfsolo::wallPenalty(1620,0,-1,0,540);
    assert(medium.hull>10 && medium.energy>2);
    assert(severe.hull>medium.hull && severe.energy>medium.energy);
    assert(severe.hull<=120 && severe.energy<=18);
    assert(sfsolo::wallPenalty(NAN,0,-1,0,540).hull==0);
    assert(sfsolo::wallPenalty(300,0,0,0,540).energy==0);

    // The canonical historical recharge retains its established rapid curve,
    // while SOLO gains resources slowly (or faster by mining).
    float historic=sfCanonicalCoopPassiveRechargeHeat(50.f,1.f,0);
    assert(sfKineticEnergyFraction(historic)>.50f);
    float easy=50.f,hard=50.f;
    for(int frame=0;frame<5*60;++frame){
        easy=sfSoloPassiveRechargeHeat(easy,1.f/60.f,0);
        hard=sfSoloPassiveRechargeHeat(hard,1.f/60.f,8);
    }
    assert(sfKineticEnergyFraction(easy)>.05f);
    assert(sfKineticEnergyFraction(easy)<.20f);
    assert(hard>easy);
    // One collected dust particle already restores 1 heat point; don't let
    // free passive recovery eclipse that resource.
    assert(50.f-easy<10.f);
    // A full reserve MUST be attainable purely by patience, not asymptotic.
    for(int frame=0;frame<140*60;++frame)
        hard=sfSoloPassiveRechargeHeat(hard,1.f/60.f,8);
    assert(hard==0.f);
    assert(sfSoloPassiveRechargeHeat(0,1.f,0)==0.f);
    return 0;
}
