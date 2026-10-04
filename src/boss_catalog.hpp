#pragma once
#include <array>
#include <algorithm>

struct SfBossProfile {
    const char *name;
    int index, family, tier, planet, backdrop;
    float health, interval, shotSpeed, damage;
    int difficulty=0;
};

inline constexpr std::array<const char*,50> sfBossNames{{
    "IRIS DE QUARTZ","SCARABEE CENDRE","MEDUSE DU VIDE","GUEULE DE BRAISE","ETOILE DE GIVRE",
    "MANTA D ACIER","REINE DES SPORES","HYDRE PLASMA","SCORPION OBSIDIEN","VEILLEUR ANCIEN",
    "PRISME HURLEUR","SCARABEE TITANE","ORACLE ABYSSAL","FOURNAISE ERRANTE","LEVIATHAN POLAIRE",
    "LAME CELESTE","RUCHER TOXIQUE","HYDRE IONIQUE","DARD DE NUIT","TITAN AUX TROIS YEUX",
    "COURONNE DE CRISTAL","COLOSSE CARAPACE","TISSEUSE DU NEANT","DEVOREUR MAGMATIQUE","DRAGON DE BANQUISE",
    "FAUCHEUSE ORBITALE","MATRICE VENENEUSE","HYDRE AUX NEUF FEUX","EMPEREUR SCORPION","ARCHON DES ETOILES",
    "SOUVERAIN PRISMATIQUE","BASTION VIVANT","MEDUSE DES ECLIPSES","SOLEIL CARNASSIER","MONARQUE DU ZERO",
    "SERAPHIN MECANIQUE","IMPERATRICE DES RONCES","HYDRE DES TEMPETES","TYRAN D OBSIDIENNE","GARDIEN DES SINGULARITES",
    "OEIL DE L INFINI","FORTERESSE CHITINE","CONSCIENCE DU VIDE","COEUR DE SUPERNOVA","HIVER ETERNEL",
    "ANGE DE L ANTIMATIERE","REINE DU DERNIER MONDE","HYDRE DES CINQUANTE SOLEILS","SCORPION APOCALYPSE","LE DERNIER COSMOS"
}};
inline constexpr std::array<const char*,10> sfBossHints{{
    "SALVES SUR VOTRE POSITION FUTURE","SPIRALES : CHANGEZ DE COULOIR","ANNEAUX : CHERCHEZ L OUVERTURE",
    "RAYON : BOUGEZ APRES LE SIGNAL","MINES : GARDEZ VOS DISTANCES","CROIX TOURNANTES : DECALEZ VOUS",
    "SPORES GUIDEES : CHANGEZ DE CAP","VAGUES : TRAVERSEZ LES INTERVALLES","ONDE : SORTEZ DU CERCLE",
    "SATELLITES : SURVEILLEZ LES FLANCS"
}};

static const std::array<SfBossProfile,50> &sfBossCatalog()
{
    static const auto profiles=[] {
        std::array<SfBossProfile,50> out{};
        // The planet atlas contains 55 worlds (11x5). Exclude five ringed
        // cells whose rings touch neighbours; each encounter uses a unique disc.
        int planet=0;
        for (int i=0;i<50;++i) {
            while (planet==4 || planet==21 || planet==33 || planet==46 || planet==54) ++planet;
            out[i]={sfBossNames[i],i,i%10,i/10,planet++,(i%10+i/10)%6,
                900.0f+72*i+2*i*i,1.75f-.021f*i,.24f+.004f*i,12.0f+.6f*i};
        }
        return out;
    }();
    return profiles;
}

constexpr int SF_BOSS_COUNT=50,SF_DIFFICULTY_COUNT=4,SF_ENCOUNTER_COUNT=200;
inline constexpr std::array<const char*,4> sfDifficultyNames{{"VIF","ENDURANT","VICIEUX","ULTIME"}};
static int sfBossIndex(int encounter) {return std::clamp(encounter,0,199)%50;}
static int sfDifficultyIndex(int encounter) {return std::clamp(encounter,0,199)/50;}
static const SfBossProfile &sfEncounterProfile(int encounter)
{
    static const auto profiles=[] {
        std::array<SfBossProfile,200> out{};
        const float health[]={1,1.3f,1.6f,2.0f},cadence[]={1.05f,.92f,.84f,.72f};
        const float speed[]={1,1.10f,1.20f,1.35f},damage[]={1,1.12f,1.25f,1.45f};
        for(int i=0;i<200;++i) {
            auto p=sfBossCatalog()[i%50];const int d=i/50;p.difficulty=d;
            p.health*=health[d];p.interval*=cadence[d];p.shotSpeed*=speed[d];p.damage*=damage[d];out[i]=p;
        }
        return out;
    }();
    return profiles[std::clamp(encounter,0,199)];
}
