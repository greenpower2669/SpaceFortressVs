#include <algorithm>
#include <cstdint>
#include <cassert>
#include <sstream>
#include <string>
// Only the serializer and Hall of Fame score helpers are linked in this dependency-free regression.
char *SDL_GetPrefPath(const char*,const char*);
void SDL_free(void*);
constexpr int SF_COOP_LOCAL=2,SF_COOP_AI=3;
#include "campaign_save.hpp"
#include "hall_of_fame.hpp"

int main() {
    const std::string v1="SPACEFORTRESS_CAMPAIGN 1\n50 49 1 1\n\"Fab\"\n\"Yann\"\n\"Éclipse\"\n42 50 9876 120 2 100 \"Fab\" \"Yann\" \"Éclipse\"\n41 49 1234 110 2 99 \"Fab\" \"Yann\" \"Éclipse\"\n";
    SfCampaignSave save;std::istringstream in(v1);assert(sfDecodeCampaign(in,save));
    assert(save.cleared==50 && save.pending && save.victory.id==42 && save.victory.score==9876);
    assert(save.fame[0].boss==49 && save.names[2]=="Éclipse");
    save.cleared=200;save.selected=199;save.victory.boss=200;
    std::istringstream roundtrip(sfEncodeCampaign(save));SfCampaignSave decoded;
    assert(sfDecodeCampaign(roundtrip,decoded));
    assert(decoded.selected==199 && decoded.cleared==200 && decoded.victory.boss==200);

    // Fab's canonical Hall of Fame danger bands: 50 encounters per star.
    assert(sfFameDanger(1)==1 && sfFameDanger(50)==1);
    assert(sfFameDanger(51)==2 && sfFameDanger(100)==2);
    assert(sfFameDanger(101)==3 && sfFameDanger(150)==3);
    assert(sfFameDanger(151)==4 && sfFameDanger(200)==4);
    assert(sfFameStars(1)=="*" && sfFameStars(4)=="****");

    // Points = |boss*(danger-minutes)| + boss*(danger-minutes), evaluated to the second.
    assert(sfFamePoints(200,120)==800); // boss 200, 4 stars, 2:00
    assert(sfFamePoints(200,150)==600); // boss 200, 4 stars, 2:30
    assert(sfFamePoints(200,240)==0);   // boss 200, 4 stars, 4:00
    assert(sfFamePoints(200,360)==0);   // negative branch clamps naturally to zero

    // Ranking is points first, then shortest time; exact ties stay stable.
    assert(sfFamePoints(100,0)==400);
    assert(sfFamePoints(200,180)==400);
    assert(sfFameRanksBefore(200,120,151,120));
    assert(sfFameRanksBefore(100,0,200,180));
    assert(!sfFameRanksBefore(200,180,100,0));
    assert(!sfFameRanksBefore(200,120,200,120));
    assert(sfFameTime(150)=="2:30");

    std::puts("PASS: campaign format plus Fab Hall of Fame score and ranking contract");
}
