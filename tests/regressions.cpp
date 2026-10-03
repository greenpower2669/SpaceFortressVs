#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <list>
#include <string>
#include <vector>

#define main spacefortress_original_main
#include "../src/main.cpp"
#undef main

#include "../src/remaster_stability.hpp"
#include "../src/remaster_runtime.hpp"
#include "../src/start_ui.hpp"
#include "../src/legacy_field_primitives.hpp"
#include "../src/kinetic_shield.hpp"
#include "../src/ship_energy.hpp"
#include "../src/scenic_progression.hpp"
#include "../src/remaster_visual_restore.hpp"
#include "../src/tactical_runtime.hpp"
#include "../src/campaign_runtime.hpp"
#include "../src/remaster_ai_fix.hpp"
#include "restoration_regressions.hpp"

static SDL_Event finger(Uint32 type,SDL_FingerID id,float x,float y)
{
    SDL_Event event{};event.type=type;event.tfinger.type=type;event.tfinger.fingerId=id;event.tfinger.x=x;event.tfinger.y=y;return event;
}

static void testVectors()
{
    vecteurs zero(0, 0);
    assert(std::isfinite(zero.angle) && std::isfinite(zero.vxt) && std::isfinite(zero.vyt));
    vecteurs distinct(3, 4);
    assert(distinct.vx == 3 && distinct.vy == 4);
    assert(std::isfinite(conv360(1.0000001f, 0)));
    assert(std::isfinite(conv360(0, -1.0000001f)));
    std::puts("PASS: stationary, moving and rounded vector inputs stay finite");
}

static void testLegacyCrashes()
{
    for (int bound : {0, -1, 1, 10, 50, 500}) {
        for (int attempt = 0; attempt < 128; ++attempt) {
            const int choice = SpaceFortressRandomBelow(bound);
            assert(choice >= 0 && choice < (bound > 0 ? bound : 1));
        }
    }
    std::list<parts*> particles;
    for (int index = 0; index < 6; ++index) {
        auto *particle = new parts(0, 0);
        particle->pv = (index == 0 || index == 2 || index == 5) ? 0 : 100;
        particles.push_back(particle);
    }
    SpaceFortressPruneParticles(particles, 1000);
    assert(particles.size() == 3);
    for (auto *particle : particles) assert(particle->pv == 100);
    SpaceFortressPruneParticles(particles, 2);
    assert(particles.size() == 2);
    for (auto *particle : particles) particle->pv = 0;
    SpaceFortressPruneParticles(particles, 2);
    assert(particles.empty());
    SpaceFortressPruneParticles(particles, 2);
    std::puts("PASS: zero-energy IA shots and removal of first/middle/last/all particles");
}

static void testInput()
{
    sfFixRequestedScreen.store(SF_UI_HOME);
    sfFixConsumedFingers.clear();
    auto first = finger(SDL_FINGERDOWN, 11, .5f, .66f);
    sfFixHandleEvent(&first);
    auto second = finger(SDL_FINGERDOWN, 12, .5f, .30f);
    sfFixHandleEvent(&second);
    sfFixApplyUiRequests();
    assert(sfUiScreen == SF_UI_GAME && !setgui);
    for (auto id : {11, 12}) {
        auto motion = finger(SDL_FINGERMOTION, id, .5f, .8f);
        sfFixHandleEvent(&motion); assert(motion.type == SDL_USEREVENT);
        auto up = finger(SDL_FINGERUP, id, .5f, .8f);
        sfFixHandleEvent(&up); assert(up.type == SDL_USEREVENT);
    }
    assert(sfFixConsumedFingers.empty());
    SDL_Event back{}; back.type = SDL_KEYDOWN; back.key.keysym.sym = SDLK_AC_BACK;
    Spritej1->ctrl = Spritej2->ctrl = true;
    sfFixHandleEvent(&back); sfFixApplyUiRequests();
    assert(back.type == SDL_USEREVENT && sfUiScreen == SF_UI_HOME && setgui);
    assert(!Spritej1->ctrl && !Spritej2->ctrl);
    // Returning immediately after PLAY must cancel its deferred launch.
    auto launch = finger(SDL_FINGERDOWN, 13, .5f, .66f);
    sfFixHandleEvent(&launch);
    back.type = SDL_KEYDOWN; sfFixHandleEvent(&back); sfFixApplyUiRequests();
    assert(sfUiScreen == SF_UI_HOME);
    sfFixRequestedScreen.store(SF_UI_GAME);
    sfFixApplyUiRequests();
    std::puts("PASS: home/help touches are consumed and deferred launch/back sequencing is deterministic");
}

static void testHomeDifficultySelectorDoesNotLaunch()
{
    sfFixRequestedScreen.store(SF_UI_HOME);
    sfFixLaunchPending.store(false);
    sfBossDangerIndex=2;
    auto danger = finger(SDL_FINGERDOWN, 14, .5f, .56f);
    sfFixHandleEvent(&danger);
    assert(sfBossDangerIndex==3);
    assert(!sfFixLaunchPending.load());
    sfFixApplyUiRequests();
    assert(sfUiScreen==SF_UI_HOME && setgui);
    auto up=finger(SDL_FINGERUP,14,.5f,.56f);sfFixHandleEvent(&up);
    std::puts("PASS: difficulty selector cycles one step and never starts a battle");
}

int main()
{
    SDL_SetHint(SDL_HINT_RENDER_DRIVER,"software");
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER);
    IMG_Init(IMG_INIT_PNG);
    testVectors();
    testLegacyCrashes();
    testInput();
    testHomeDifficultySelectorDoesNotLaunch();
    runRestorationRegressions();
    IMG_Quit();SDL_Quit();
    return 0;
}
