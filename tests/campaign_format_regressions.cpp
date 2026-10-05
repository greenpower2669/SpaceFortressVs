#include <algorithm>
#include <cstdint>
#include <cassert>
#include <sstream>
#include <string>
// Only the serializer and Hall of Fame score helpers are linked in this dependency-free regression.
char *SDL_GetPrefPath(const char*,const char*);
void SDL_free(void*);
constexpr int SF_COOP_LOCAL=2,SF_COOP_AI=3;
inline int sfBossDangerIndex=0;
#include "campaign_save.hpp"
#include "hall_of_fame.hpp"

int main() {
    // Legacy v1/v2 saves never stored the selected Danger Boss. They must remain
    // readable and explicitly carry danger=0 (unknown) rather than inventing it.
    const std::string v1="SPACEFORTRESS_CAMPAIGN 1\n50 49 1 1\n\"Fab\"\n\"Yann\"\n\"Éclipse\"\n42 50 9876 120 2 100 \"Fab\" \"Yann\" \"Éclipse\"\n41 49 1234 110 2 99 \"Fab\" \"Yann\" \"Éclipse\"\n";
    SfCampaignSave save;std::istringstream in(v1);assert(sfDecodeCampaign(in,save));
    assert(save.cleared==50 && save.pending && save.victory.id==42 && save.victory.score==9876);
    assert(save.victory.danger==0 && save.fame[0].danger==0);
    assert(save.fame[0].boss==49 && save.names[2]=="Éclipse");

    const std::string v2="SPACEFORTRESS_CAMPAIGN 2\n1 0 0 1\n\"Fab\"\n\"Yann\"\n\"IATEST\"\n41 1 1234 48 2 99 \"ORION IA\" \"FAB\" \"IATEST\"\n";
    SfCampaignSave old;std::istringstream in2(v2);assert(sfDecodeCampaign(in2,old));
    assert(old.fame.size()==1 && old.fame[0].danger==0);

    // A fresh victory captures the currently selected 1..9 Boss Danger.
    sfBossDangerIndex=8;
    save.victory={};
    assert(save.victory.danger==9);
    save.cleared=200;save.selected=199;save.victory.boss=200;
    std::istringstream roundtrip(sfEncodeCampaign(save));SfCampaignSave decoded;
    assert(sfDecodeCampaign(roundtrip,decoded));
    assert(decoded.selected==199 && decoded.cleared==200 && decoded.victory.boss==200);
    assert(decoded.victory.danger==9);

    assert(sfFameStars(1)=="*");
    assert(sfFameStars(9)=="*********");
    assert(sfFameStars(0)=="?");

    // Fab's canonical formula uses the REAL selected danger, evaluated to the second:
    // Points = |boss*(danger-minutes)| + boss*(danger-minutes).
    assert(sfFamePoints(1,1,41)==0);     // Mou du genou, boss 1, 0:41 -> truncates below 1 point
    assert(sfFamePoints(1,9,41)==16);    // Apocalypse, same boss/time -> clearly non-zero
    assert(sfFamePoints(200,4,120)==800);// boss 200, danger 4, 2:00
    assert(sfFamePoints(200,4,150)==600);// boss 200, danger 4, 2:30
    assert(sfFamePoints(200,4,240)==0);  // zero branch
    assert(sfFamePoints(200,4,360)==0);  // negative branch clamps naturally to zero
    assert(sfFamePoints(200,0,120)==0);  // legacy unknown danger is never fabricated

    // Ranking is points first, then shortest time; unknown legacy danger sorts with 0 points.
    assert(sfFameRanksBefore(1,9,41,1,1,41));
    assert(sfFameRanksBefore(200,4,120,151,4,120));
    assert(!sfFameRanksBefore(200,4,120,200,4,120));
    assert(sfFameTime(150)=="2:30");

    std::puts("PASS: campaign v1/v2 compatibility plus persisted Danger Boss Hall of Fame scoring");
}
