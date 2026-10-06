#include <cassert>
#include <cmath>
#include "../src/kinetic_shield.hpp"

int main()
{
    assert(std::abs(SF_KINETIC_MAX_SHIELD_DIAMETER-2.0f)<.0001f);
    assert(std::abs(SF_KINETIC_SURGE_VISIBLE_DELAY_SECONDS-.30f)<.0001f);
    assert(std::abs(SF_KINETIC_SURGE_BLAST_DIAMETER-3.0f)<.0001f);

    const auto white=sfKineticRespondDust(true,12.0f,-8.0f,1.0f,0.0f,1.0f,.7f,780.0f);
    assert(!white.vibrated && !white.deflected);
    assert(std::abs(white.vx-12.0f)<.0001f && std::abs(white.vy+8.0f)<.0001f);

    // A short second-finger press stays classic: no rainbow and no x2 field yet.
    sfKineticResetSurges();
    sfKineticSurgePress(1);
    sfKineticAdvanceSurges(.29f);
    assert(std::abs(sfKineticSurgePower(1)-1.0f)<.0001f);
    assert(!sfKineticSurgeVisible(1));
    assert(!sfKineticSurges[1].charged);

    // At 0.30 s the visible/x2 charge begins, but the purge still arms only at 2 s.
    sfKineticAdvanceSurges(.02f);
    assert(std::abs(sfKineticSurgePower(1)-2.0f)<.0001f);
    assert(sfKineticSurgeVisible(1));
    assert(!sfKineticSurges[1].charged);

    sfKineticAdvanceSurges(1.68f);
    assert(std::abs(sfKineticSurgePower(1)-2.0f)<.0001f);
    assert(sfKineticSurgeVisible(1));
    assert(!sfKineticSurges[1].charged);

    sfKineticAdvanceSurges(.02f);
    assert(sfKineticSurges[1].charged);
    assert(std::abs(sfKineticSurgePower(1)-0.0f)<.0001f);
    assert(!sfKineticSurgeVisible(1));
    assert(sfKineticSurgeRelease(1));

    sfKineticSurgePress(0);
    sfKineticAdvanceSurges(.20f);
    assert(!sfKineticSurgeVisible(0));
    assert(std::abs(sfKineticSurgePower(0)-1.0f)<.0001f);
    assert(!sfKineticSurgeRelease(0));
    assert(std::abs(sfKineticSurgePower(0)-1.0f)<.0001f);
    return 0;
}
