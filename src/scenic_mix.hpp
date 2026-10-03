#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <utility>

constexpr int SF_SCENIC_BASE_COUNT=20;
constexpr int SF_SCENIC_CANDIDATE_COUNT=100;

struct SfRgb { std::uint8_t r=255,g=255,b=255; };
struct SfScenicProfile {
    int candidate=0;
    int baseBackdrop=0;
    int basePlanet=0;
    int backdrop=0;
    int planetSlot=0;
    int variant=0;
    std::uint32_t starSeed=0;
    float tintStrength=.10f;
    SfRgb accent{};
};
struct SfScenicBag {
    std::array<int,SF_SCENIC_CANDIDATE_COUNT> order{};
    int cursor=SF_SCENIC_CANDIDATE_COUNT;
    int last=-1;
    std::uint32_t cycle=0;
};

static inline std::uint32_t sfScenicHash(std::uint32_t x)
{
    x^=x>>16;x*=0x7feb352du;x^=x>>15;x*=0x846ca68bu;x^=x>>16;return x;
}
static inline std::uint8_t sfScenicLerpByte(std::uint8_t a,std::uint8_t b,float t)
{
    t=std::clamp(t,0.0f,1.0f);
    return std::uint8_t(std::clamp(int(a+(b-a)*t+.5f),0,255));
}
static inline SfRgb sfScenicLerp(SfRgb a,SfRgb b,float t)
{
    return {sfScenicLerpByte(a.r,b.r,t),sfScenicLerpByte(a.g,b.g,t),sfScenicLerpByte(a.b,b.b,t)};
}
static inline SfRgb sfProgressRgb(int encounter)
{
    const float p=std::clamp(encounter,0,199)/199.0f;
    const SfRgb blue{68,132,255},green{72,226,118},red{255,76,68};
    return p<=.5f ? sfScenicLerp(blue,green,p*2.0f) : sfScenicLerp(green,red,(p-.5f)*2.0f);
}
static inline float sfBossTravelProgress(int encounter)
{
    return std::clamp(encounter,0,199)/199.0f;
}
static inline int sfScenicPlanetAtlasIndex(int slot)
{
    slot=std::clamp(slot,0,49);
    int logical=0;
    for(int cell=0;cell<55;++cell) {
        if(cell==4 || cell==21 || cell==33 || cell==46 || cell==54) continue;
        if(logical++==slot) return cell;
    }
    return 0;
}
static inline SfScenicProfile sfScenicProfileForCandidate(int candidate,int colorEncounter)
{
    candidate=((candidate%SF_SCENIC_CANDIDATE_COUNT)+SF_SCENIC_CANDIDATE_COUNT)%SF_SCENIC_CANDIDATE_COUNT;
    SfScenicProfile out{};out.candidate=candidate;
    out.baseBackdrop=candidate%SF_SCENIC_BASE_COUNT;
    out.variant=candidate/SF_SCENIC_BASE_COUNT;
    out.basePlanet=(out.baseBackdrop*7+out.variant*3+1)%SF_SCENIC_BASE_COUNT;
    if(out.basePlanet==out.baseBackdrop) out.basePlanet=(out.basePlanet+1)%SF_SCENIC_BASE_COUNT;
    out.backdrop=out.baseBackdrop%6;
    out.planetSlot=(out.basePlanet+out.variant+4*out.baseBackdrop+3)%50;
    out.starSeed=sfScenicHash(0x1968ab12u+std::uint32_t(candidate)*71539u);
    out.tintStrength=.09f+.035f*(out.variant/4.0f);
    out.accent=sfProgressRgb(colorEncounter);
    return out;
}
static inline SfScenicProfile sfScenicProfileForEncounter(int encounter)
{
    encounter=std::clamp(encounter,0,199);
    return sfScenicProfileForCandidate((encounter*37)%SF_SCENIC_CANDIDATE_COUNT,encounter);
}
static inline void sfScenicBagRefill(SfScenicBag &bag,std::uint32_t entropy)
{
    for(int i=0;i<SF_SCENIC_CANDIDATE_COUNT;++i) bag.order[i]=i;
    std::uint32_t state=sfScenicHash(entropy^0x9e3779b9u^(bag.cycle+1u)*0x85ebca6bu);
    for(int i=SF_SCENIC_CANDIDATE_COUNT-1;i>0;--i) {
        state=sfScenicHash(state+std::uint32_t(i)*0x27d4eb2du);
        const int j=int(state%std::uint32_t(i+1));
        std::swap(bag.order[i],bag.order[j]);
    }
    if(bag.last>=0 && bag.order[0]==bag.last) std::swap(bag.order[0],bag.order[1]);
    bag.cursor=0;++bag.cycle;
}
static inline int sfScenicBagNext(SfScenicBag &bag,std::uint32_t entropy)
{
    if(bag.cursor>=SF_SCENIC_CANDIDATE_COUNT) sfScenicBagRefill(bag,entropy);
    const int value=bag.order[bag.cursor++];bag.last=value;return value;
}
