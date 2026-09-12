#ifndef SPACEFORTRESS_LEGACY_GAME_SAFETY_HPP
#define SPACEFORTRESS_LEGACY_GAME_SAFETY_HPP

#include <cstdlib>
#include <list>

// A new ship has zero energy. Both old IA shot-selection expressions used
// that energy as a modulo divisor, causing SIGFPE on the first aligned shot.
// Positive ranges retain their original distribution; zero selects the first
// shot, which is also what the historical '< 10' condition expects at low energy.
static int SpaceFortressRandomBelow(int upperBound)
{
    return std::rand() % (upperBound > 0 ? upperBound : 1);
}

// std::list::erase invalidates the erased iterator. Always advance using its
// return value; incrementing the old iterator after deleting a particle is a
// use-after-free. Called only from the legacy particle/render loop.
static void SpaceFortressPruneParticles(std::list<parts*> &particles, float limit)
{
    for (auto it = particles.begin(); it != particles.end();) {
        parts *particle = *it;
        if (particles.size() > limit) particle->pv = 0;
        if (particle->pv == 0) {
            it = particles.erase(it);
            delete particle;
        } else {
            ++it;
        }
    }
}

#endif
