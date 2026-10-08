#include <cassert>
#include <cmath>
#include "../src/kinetic_shield.hpp"

int main()
{
    assert(std::abs(SF_KINETIC_SURGE_VISIBLE_DELAY_SECONDS-.20f)<.0001f);
    assert(std::abs(SF_KINETIC_SURGE_HOLD_SECONDS-2.0f)<.0001f);
    assert(std::abs(SF_KINETIC_SURGE_BLAST_DIAMETER-3.0f)<.0001f);

    sfKineticResetSurges();sfKineticSurgePress(1);sfKineticAdvanceSurges(.19f);
    assert(!sfKineticSurgeVisible(1) && !sfKineticSuctionVisible(1));
    assert(sfKineticSurgePower(1)==1.0f && !sfKineticSurgeVulnerable(1));
    sfKineticAdvanceSurges(.02f);
    assert(sfKineticSurgeVisible(1) && sfKineticSuctionVisible(1));
    assert(sfKineticSurgePower(1)==0.0f && sfKineticSurgeVulnerable(1));
    assert(sfKineticSuperchargeProgress(1)>0 && sfKineticSuperchargeProgress(1)<.02f);
    sfKineticAdvanceSurges(.79f);
    assert(std::abs(sfKineticSuperchargeProgress(1)-(0.8f/1.8f))<.02f);
    sfKineticAdvanceSurges(1.01f);
    assert(sfKineticSurges[1].charged && sfKineticSuperchargeProgress(1)==1.0f);
    assert(sfKineticSurgeVisible(1) && sfKineticSuctionVisible(1) && sfKineticSurgeVulnerable(1));
    assert(sfKineticSurgeRelease(1));

    const int initialDanger=sfBossDangerIndex;sfBossDangerIndex=0;
    const float n=sfKineticMiningArenaFractionPerSecond(0),m=sfKineticMiningArenaFractionPerSecond(.5f),f=sfKineticMiningArenaFractionPerSecond(1);
    assert(n>m && m>f && f>0);
    assert(std::abs(sfKineticConeBossDpsMultiplier(0)-3.0f)<.0001f);
    assert(std::abs(sfKineticConeBossDpsMultiplier(.25f)-2.2575f)<.0002f);
    assert(std::abs(sfKineticConeBossDpsMultiplier(.5f)-1.515f)<.0002f);
    assert(std::abs(sfKineticConeBossDpsMultiplier(.75f)-.7725f)<.0002f);
    assert(std::abs(sfKineticConeBossDpsMultiplier(1)-.03f)<.0001f);

    sfKineticSurgePress(0);sfKineticAdvanceSurges(.20f);
    assert(sfKineticSurgeVisible(0) && !sfKineticSurgeRelease(0));

    sfBossDangerIndex=initialDanger;
    const int oldDanger=sfBossDangerIndex;
    sfBossDangerIndex=0;
    assert(sfKineticConeDifficultyDivisor()==1.0f);
    const float easyRange=sfKineticSurgeMiningRangeDiameters();
    const float easyWidth=sfKineticSurgeConeHalfWidth(100.0f);
    const float easyMine=sfKineticMiningArenaFractionPerSecond(.5f);
    const float easyDamage=sfKineticConeBossDpsMultiplier(.5f);
    sfBossDangerIndex=8;
    assert(sfKineticConeDifficultyDivisor()==9.0f);
    assert(std::abs(sfKineticSurgeMiningRangeDiameters()*9.0f-easyRange)<.0001f);
    assert(std::abs(sfKineticSurgeConeHalfWidth(100.0f)*9.0f-easyWidth)<.0001f);
    assert(std::abs(sfKineticMiningArenaFractionPerSecond(.5f)*9.0f-easyMine)<.0001f);
    assert(std::abs(sfKineticConeBossDpsMultiplier(.5f)*9.0f-easyDamage)<.0001f);
    sfBossDangerIndex=oldDanger;
    return 0;
}
