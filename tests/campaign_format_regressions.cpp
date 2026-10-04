#include <algorithm>
#include <cstdint>
#include <cassert>
#include <sstream>
#include <string>
// Only the serializer is linked in this dependency-free regression.
char *SDL_GetPrefPath(const char*,const char*);
void SDL_free(void*);
constexpr int SF_COOP_LOCAL=2,SF_COOP_AI=3;
#include "campaign_save.hpp"
int main() {
    const std::string v1="SPACEFORTRESS_CAMPAIGN 1\n50 49 1 1\n\"Fab\"\n\"Yann\"\n\"Éclipse\"\n42 50 9876 120 2 100 \"Fab\" \"Yann\" \"Éclipse\"\n41 49 1234 110 2 99 \"Fab\" \"Yann\" \"Éclipse\"\n";
    SfCampaignSave save;std::istringstream in(v1);assert(sfDecodeCampaign(in,save));
    assert(save.cleared==50 && save.pending && save.victory.id==42 && save.victory.score==9876);
    assert(save.fame[0].boss==49 && save.names[2]=="Éclipse");
    save.cleared=200;save.selected=199;save.victory.boss=200;
    std::istringstream roundtrip(sfEncodeCampaign(save));SfCampaignSave decoded;
    assert(sfDecodeCampaign(roundtrip,decoded));
    assert(decoded.selected==199 && decoded.cleared==200 && decoded.victory.boss==200);
    std::puts("PASS: real v1 fixture with pending victory, names and scores; v2 up to encounter 200");
}
