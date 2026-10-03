#include <cassert>
#include <cmath>
#include <cstdint>
#include <set>
#include <utility>
#include "scenic_mix.hpp"

int main()
{
    SfScenicBag bag{};
    std::set<int> firstCycle;
    int previous=-1;
    for (int i=0;i<SF_SCENIC_CANDIDATE_COUNT;++i) {
        const int scene=sfScenicBagNext(bag,0x12345678u+std::uint32_t(i*97));
        assert(scene>=0 && scene<SF_SCENIC_CANDIDATE_COUNT);
        assert(firstCycle.insert(scene).second);
        previous=scene;
    }
    assert(firstCycle.size()==SF_SCENIC_CANDIDATE_COUNT);
    const int afterReset=sfScenicBagNext(bag,0x87654321u);
    assert(afterReset>=0 && afterReset<SF_SCENIC_CANDIDATE_COUNT);
    assert(afterReset!=previous);

    std::set<std::pair<int,int>> visualPairs;
    int mixed=0;
    for (int i=0;i<SF_SCENIC_CANDIDATE_COUNT;++i) {
        const auto scene=sfScenicProfileForCandidate(i,i*2);
        assert(scene.candidate==i);
        assert(scene.baseBackdrop>=0 && scene.baseBackdrop<SF_SCENIC_BASE_COUNT);
        assert(scene.basePlanet>=0 && scene.basePlanet<SF_SCENIC_BASE_COUNT);
        assert(scene.backdrop>=0 && scene.backdrop<6);
        assert(scene.planetSlot>=0 && scene.planetSlot<50);
        const int atlas=sfScenicPlanetAtlasIndex(scene.planetSlot);
        assert(atlas>=0 && atlas<55);
        assert(atlas!=4 && atlas!=21 && atlas!=33 && atlas!=46 && atlas!=54);
        visualPairs.insert({scene.backdrop,scene.planetSlot});
        if(scene.baseBackdrop!=scene.basePlanet) ++mixed;
    }
    assert(visualPairs.size()==SF_SCENIC_CANDIDATE_COUNT);
    assert(mixed>80);

    const auto blue=sfProgressRgb(0);
    const auto green=sfProgressRgb(100);
    const auto red=sfProgressRgb(199);
    assert(blue.b>blue.g && blue.b>blue.r);
    assert(green.g>green.r && green.g>green.b);
    assert(red.r>red.g && red.r>red.b);
    for(int i=1;i<200;++i) {
        assert(sfBossTravelProgress(i)>=sfBossTravelProgress(i-1));
        const auto a=sfProgressRgb(i-1),b=sfProgressRgb(i);
        assert(std::abs(int(a.r)-int(b.r))<=4);
        assert(std::abs(int(a.g)-int(b.g))<=4);
        assert(std::abs(int(a.b)-int(b.b))<=4);
    }
    assert(sfBossTravelProgress(0)==0.0f);
    assert(sfBossTravelProgress(199)>0.999f);

    const auto e0=sfScenicProfileForEncounter(0);
    const auto e50=sfScenicProfileForEncounter(50);
    const auto e100=sfScenicProfileForEncounter(100);
    const auto e199=sfScenicProfileForEncounter(199);
    assert(e0.candidate!=e50.candidate);
    assert(e0.candidate==e100.candidate);
    assert(e0.accent.b>e0.accent.r);
    assert(e100.accent.g>e100.accent.r);
    assert(e199.accent.r>e199.accent.b);
    return 0;
}
