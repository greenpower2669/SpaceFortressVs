from pathlib import Path

p = Path('tests/regressions.cpp')
t = p.read_text()
marker = 'int main(int argc,char **argv)'
cut = t.find(marker)
if cut < 0:
    raise AssertionError('regressions.cpp main not found')
main = r'''int main(int argc,char **argv)
{
    (void)argc; (void)argv;
    assert(SDL_Init(SDL_INIT_TIMER) == 0);
    auto stop=[](const char *tag){
        const char *s=std::getenv("SF_DIAG_STOP");
        return s && std::string(s)==tag;
    };
    testVectors(); testLegacyCrashes(); testInput(); testTextures(); testScenicRendering();
    if(stop("A")) return 0;
    testTacticalPilot(); testTacticalTurrets(); testJupiterMotion(); testRaidsAndDefence();
    if(stop("B")) return 0;
    testEnergyFeedback(); testProjectileFeedback();
    if(stop("C")) return 0;
    char campaignDirectory[]="/tmp/spacefortress-campaign-XXXXXX";
    assert(mkdtemp(campaignDirectory));
    testVelocityGhosts();testCampaignPersistence(campaignDirectory);testCoopGameplay();testCoopArenaBounds();testCoopCollisionMinerals();testCampaignEntryAndFights();testCampaignProgression();
    testCampaignRendering(std::getenv("SPACEFORTRESS_CAMPAIGN_PREVIEW"));
    if(stop("D")) return 0;
    testDuelStyleRoundTrip();testShipBreathing();testCoopHumanAim();testUnknownSaveWithBackup(campaignDirectory);testSaveRecoveryPreservation(campaignDirectory);
    if(stop("E")) return 0;
    testNoHumanAutofire();testPassiveCoopTurrets();testSharedLinearShieldModel();testKineticFieldAndHullRegen();testCollectedBonusAndShield();testRealCoopField();
    if(stop("F")) return 0;
    testCoopDifficultyChain();testCoopIncomingDamageMultiplier();testCoopHudAndMissile();
    if(stop("G")) return 0;
    testDurableV1Migration(campaignDirectory);testDifficultySelection();testDifficultyGameplay();testDifficultyRendering(std::getenv("SPACEFORTRESS_DIFFICULTY_PREVIEW"));
    if(stop("H")) return 0;
    return 0;
}
'''
p.write_text(t[:cut] + main)
print('Injected A-H regression prefix checkpoints')
