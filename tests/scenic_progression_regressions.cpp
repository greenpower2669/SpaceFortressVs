#include <array>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <set>
#include "scenic_map_progression.hpp"

static void testClassicBagUsesEveryCandidateBeforeRepeating()
{
    SfClassicMapBag bag{};
    sfResetClassicMapBag(bag,0x12345678u);
    std::set<int> seen;
    for(int i=0;i<SF_SCENIC_CANDIDATE_COUNT;++i) {
        const int id=sfNextClassicMap(bag);
        assert(id>=0 && id<SF_SCENIC_CANDIDATE_COUNT);
        assert(seen.insert(id).second);
    }
    assert(int(seen.size())==SF_SCENIC_CANDIDATE_COUNT);
    const int firstOfNextCycle=sfNextClassicMap(bag);
    assert(firstOfNextCycle>=0 && firstOfNextCycle<SF_SCENIC_CANDIDATE_COUNT);
    std::puts("PASS: classic scenic bag avoids repeats for a complete 100-map cycle");
}

static void testTwentyBaseMapsAreMixedIntoHundredCandidates()
{
    std::array<bool,SF_SCENIC_BASE_MAP_COUNT> bases{};
    int mixed=0;
    for(int id=0;id<SF_SCENIC_CANDIDATE_COUNT;++id) {
        const auto map=sfScenicMapProfile(id);
        assert(map.baseMap>=0 && map.baseMap<SF_SCENIC_BASE_MAP_COUNT);
        assert(map.backgroundA>=0 && map.backgroundA<6);
        assert(map.backgroundB>=0 && map.backgroundB<6);
        assert(map.planetA>=0 && map.planetA<50);
        assert(map.planetB>=0 && map.planetB<50);
        assert(map.mix>=0.0f && map.mix<=1.0f);
        bases[map.baseMap]=true;
        if(map.backgroundA!=map.backgroundB || map.planetA!=map.planetB) ++mixed;
    }
    for(bool present:bases) assert(present);
    assert(mixed>=80);
    std::puts("PASS: 20 base maps feed 100 mixed scenic candidates");
}

static void testEncounterColourMovesBlueGreenRed()
{
    const auto early=sfEncounterColour(0);
    const auto middle=sfEncounterColour(99);
    const auto late=sfEncounterColour(199);
    assert(early.b>early.r && early.b>early.g);
    assert(middle.g>middle.r && middle.g>middle.b);
    assert(late.r>late.g && late.r>late.b);
    assert(early.strength<late.strength);
    std::puts("PASS: encounter tint progresses blue to green to red");
}

static void testBossMovementExpandsTowardsEdges()
{
    const auto early=sfBossMovementEnvelope(0);
    const auto middle=sfBossMovementEnvelope(99);
    const auto late=sfBossMovementEnvelope(199);
    assert(early.xSpan<middle.xSpan && middle.xSpan<late.xSpan);
    assert(early.ySpan<middle.ySpan && middle.ySpan<late.ySpan);
    assert(early.xSpan<=.18f);
    assert(late.xSpan>=.40f);
    assert(late.ySpan>=.34f);
    std::puts("PASS: boss movement grows from centre control to near-edge pressure");
}

int main()
{
    testClassicBagUsesEveryCandidateBeforeRepeating();
    testTwentyBaseMapsAreMixedIntoHundredCandidates();
    testEncounterColourMovesBlueGreenRed();
    testBossMovementExpandsTowardsEdges();
    return 0;
}
