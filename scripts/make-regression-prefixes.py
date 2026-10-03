from pathlib import Path

# Instrument the single remaining failing restoration test after the v3 adapter ran.
rp = Path('tests/restoration_regressions.hpp')
rt = rp.read_text()
start = rt.index('static void testCoopIncomingDamageMultiplier()')
end = rt.index('static void testCoopHudAndMissile()', start)
body = rt[start:end]

def add_checkpoint(anchor, tag):
    global body
    if body.count(anchor) != 1:
        raise AssertionError(f'G2 checkpoint {tag}: anchor count={body.count(anchor)}')
    check = f'    if (const char *s=std::getenv("SF_DIAG_STOP"); s && std::string(s)=="{tag}") return;\n'
    body = body.replace(anchor, anchor + check, 1)

add_checkpoint('        assert(other->pv==1000);\n    }\n', 'G2A')
add_checkpoint('    assert(std::abs((2000.0f-Spritej1->pv)-sfBossDangerMultiplier()*waveDamage)<.01f);\n', 'G2B')
add_checkpoint('    assert(std::abs((1000.0f-Spritej1->pv)-asteroidExpected)<.01f && Spritej1->nrj>25);\n', 'G2C')
add_checkpoint('    sfBossDangerIndex=2;assert(sfBossDangerMultiplier()==10.0f);\n', 'G2D')
add_checkpoint('    assert(bossDust->pv==0 && sfCoop.health>bossBefore && sfCoop.health<=bossMax);\n', 'G2E')
add_checkpoint('    assert(ordinaryDust->pv>0 && sfCoop.health==ordinaryBefore);\n', 'G2F')
add_checkpoint('    assert(Spritej1->pv==pvBeforeRed && Spritej1->nrj==heatBeforeRed && red->pv>0);\n', 'G2G')
rp.write_text(rt[:start] + body + rt[end:])

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
    testCoopDifficultyChain();
    if(stop("G1")) return 0;
    testCoopIncomingDamageMultiplier();
    if(stop("G2A")||stop("G2B")||stop("G2C")||stop("G2D")||stop("G2E")||stop("G2F")||stop("G2G")) return 0;
    testCoopHudAndMissile();
    if(stop("G3")) return 0;
    testDurableV1Migration(campaignDirectory);testDifficultySelection();testDifficultyGameplay();testDifficultyRendering(std::getenv("SPACEFORTRESS_DIFFICULTY_PREVIEW"));
    if(stop("H")) return 0;
    return 0;
}
'''
p.write_text(t[:cut] + main)
print('Injected regression checkpoints including G2A-G2G')
