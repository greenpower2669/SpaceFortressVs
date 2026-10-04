#include <cassert>
#include <cmath>
#include <cstring>
#include <cstdio>
#include "boss_danger.hpp"

int main()
{
    static const float expectedLevels[9]={1.0f,5.0f,10.0f,15.0f,20.0f,25.0f,30.0f,35.0f,40.0f};
    static const char *expectedNames[9]={
        "MOU DU GENOU","CHILL","ROCK N ROLL","DUR A CUIRE","MACHINE DE GUERRE",
        "CA VA PIQUER","SANS PITIE","ENFER STELLAIRE","APOCALYPSE"
    };

    assert(SF_BOSS_DANGER_COUNT==9);
    for(int i=0;i<SF_BOSS_DANGER_COUNT;++i) {
        assert(std::fabs(SF_BOSS_DANGER_LEVELS[i]-expectedLevels[i])<0.0001f);
        assert(std::strcmp(SF_BOSS_DANGER_NAMES[i],expectedNames[i])==0);
    }

    assert(sfBossDangerIndex==2);
    assert(std::strcmp(sfBossDangerName(),"ROCK N ROLL")==0);
    assert(std::fabs(sfBossDangerMultiplier()-10.0f)<0.0001f);

    sfBossDangerIndex=8;
    sfBossDangerNext();
    assert(sfBossDangerIndex==0);
    sfBossDangerAdjust(-1);
    assert(sfBossDangerIndex==0);
    sfBossDangerAdjust(1);
    assert(sfBossDangerIndex==1);

    sfBossDangerIndex=0;
    assert(std::fabs(sfApplyHostileDanger(2.0f)-2.0f)<0.0001f);
    sfBossDangerIndex=2;
    assert(std::fabs(sfApplyHostileDanger(2.0f)-20.0f)<0.0001f);
    sfBossDangerIndex=8;
    assert(std::fabs(sfApplyHostileDanger(2.0f)-80.0f)<0.0001f);

    std::puts("PASS: nine named danger levels, ROCK N ROLL default, hidden damage helper");
    return 0;
}
