#pragma once
#include <string>
namespace sfsolo {
// Completion data shared by the old prototype and the COOP-powered SOLO.
// No pilot object, physics integrator, textures or live campaign globals.
struct RunResult {
    std::string mapId;
    unsigned mapVersion=0,world=1,stage=1,difficulty=1;
    unsigned enemiesDefeated=0,enemiesAvailable=0;
    float seconds=0,damageTaken=0,health=0;
    bool bossDefeated=false,reachedFinish=false;
};
}
