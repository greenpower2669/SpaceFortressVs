#include <cassert>
#include <cmath>
#include <cstring>
#include "kinetic_shield.hpp"
#include "boss_danger.hpp"

int main() {
    const float ref=sfKineticReferenceSpeed(780.0f);
    const auto full=sfResolveKinetic(SF_KINETIC_ASTEROID_MAX_HULL_DAMAGE,1.0f,{0,ref},{0,0},0,-1,ref);
    const auto halfSpeed=sfResolveKinetic(SF_KINETIC_ASTEROID_MAX_HULL_DAMAGE,1.0f,{0,ref*.5f},{0,0},0,-1,ref);
    const auto halfMass=sfResolveKinetic(SF_KINETIC_ASTEROID_MAX_HULL_DAMAGE,.5f,{0,ref},{0,0},0,-1,ref);
    const auto tangent=sfResolveKinetic(SF_KINETIC_ASTEROID_MAX_HULL_DAMAGE,1.0f,{ref,0},{0,0},0,-1,ref);
    const auto still=sfResolveKinetic(SF_KINETIC_ASTEROID_MAX_HULL_DAMAGE,1.0f,{0,0},{0,0},0,-1,ref);
    assert(std::abs(full.rawDamage-250.0f)<.01f);
    assert(std::abs(halfSpeed.rawDamage-125.0f)<.01f);
    assert(std::abs(halfMass.rawDamage-125.0f)<.01f);
    assert(tangent.rawDamage<.001f);
    assert(still.rawDamage<.001f);
    assert(SF_KINETIC_MAX_SHIELD_DIAMETER==2.0f);

    sfBossDangerIndex=2;
    assert(std::strcmp(sfBossDangerName(),"ROCK N ROLL")==0);
    assert(sfBossDangerMultiplier()==10.0f);
    sfBossDangerNext();
    assert(std::strcmp(sfBossDangerName(),"DUR A CUIRE")==0 && sfBossDangerMultiplier()==15.0f);
    sfBossDangerNext();
    assert(std::strcmp(sfBossDangerName(),"MACHINE DE GUERRE")==0 && sfBossDangerMultiplier()==20.0f);
    sfBossDangerNext();
    assert(std::strcmp(sfBossDangerName(),"MOU DU GENOU")==0 && sfBossDangerMultiplier()==1.0f);
    return 0;
}
