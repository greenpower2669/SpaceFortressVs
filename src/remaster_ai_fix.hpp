#ifndef SPACEFORTRESS_REMASTER_AI_FIX_HPP
#define SPACEFORTRESS_REMASTER_AI_FIX_HPP

// Final Android compatibility bridge for the historical IA/input model.
//
// Important: this file deliberately runs AFTER start_ui.hpp,
// remaster_runtime.hpp and remaster_visual_restore.hpp. It keeps menu/touch
// requests on the SDL event thread, but applies all gameplay state changes on
// the render/game thread immediately before presentation. This removes the
// setia / velocity data races introduced by the first remaster shim while
// leaving the historical j2lock -> iago -> asteroid logic intact.

#include <SDL2/SDL.h>
#include <atomic>
#include <cstdlib>
#include <ctime>
#include <cstdint>
#include <list>

extern enti *iago1;
extern enti *iacalc;
extern enti *iatake;
extern sprite *Suiveur;
extern std::list<sprite*> sa1;
extern int incra1;

// The generated Android copy of main.cpp aliases its old local
// `suiveurmajaf` variable to this C++17 inline variable. A new match can then
// restore the follower/cascade gate exactly instead of inheriting the previous
// match state.
inline bool SpaceFortressLegacySuiveurMajaf = true;

static std::atomic<int>  sfFixRequestedScreen(SF_UI_HOME);
static std::atomic<bool> sfFixRequestedIa(false);
static std::atomic<bool> sfFixLaunchPending(false);
static std::atomic<uint32_t> sfFixMatchSerial(0u);

static bool sfFixConsumeFinger = false;
static SDL_FingerID sfFixConsumedFinger = 0;

static bool sfFixGear1Down = false;
static bool sfFixGear2Down = false;
static SDL_FingerID sfFixGear1Finger = 0;
static SDL_FingerID sfFixGear2Finger = 0;

static void sfFixResetEnti(enti *e, float x, float y)
{
    if (!e) return;
    e->xy.setxy(x, y);
    e->v.angle = 0.0f;
    e->v.force = 0.0f;
    e->v.forcet = 0.0f;
    e->v.vx = 0.0f;
    e->v.vy = 0.0f;
    // setai() with force=0 is safe and refreshes vxt/vyt without the
    // divide-by-zero path that setvi() would take for a zero vector.
    e->v.setai();
}

static void sfFixResetSpriteHistory(sprite *s)
{
    if (!s) return;
    s->x0 = s->x;
    s->x00 = s->x;
    s->x000 = s->x;
    s->y0 = s->y;
    s->y00 = s->y;
    s->y000 = s->y;
    s->vx = 0.0f;
    s->vy = 0.0f;
    s->ctrl = false;
    s->id = 100;
}

static void sfFixReseedLegacyRandom()
{
    // The old source never calls srand(), so a fresh process starts with the
    // same asteroid/IA random sequence every time. Mix wall time, SDL uptime
    // and a per-match serial to keep retries from reproducing the same casino
    // layout/decision sequence while preserving the original rand()-based code.
    const uint64_t ticks = static_cast<uint64_t>(SDL_GetTicks64());
    const uint64_t wall = static_cast<uint64_t>(std::time(NULL));
    const uint64_t serial = static_cast<uint64_t>(sfFixMatchSerial.fetch_add(1u) + 1u);
    uint64_t mixed = ticks ^ (wall << 21) ^ (serial * 0x9E3779B97F4A7C15ull);
    mixed ^= mixed >> 33;
    mixed *= 0xff51afd7ed558ccdull;
    mixed ^= mixed >> 33;
    std::srand(static_cast<unsigned int>(mixed ^ (mixed >> 32)));
}

static void sfFixResetAsteroidField()
{
    // A HOME -> new match happens without restarting the process. The legacy
    // asteroid list and incra1 therefore survived between attempts, which can
    // reproduce the same trap/state. At end-of-frame this list is not being
    // iterated, so it is safe to release it and let the historical
    // `if (incra1 < 8) setasts(2)` path repopulate it naturally next frame.
    for (sprite *asteroid : sa1) delete asteroid;
    sa1.clear();
    incra1 = 0;
}

static void sfFixResetMatchState()
{
    sfFixReseedLegacyRandom();
    sfFixResetAsteroidField();

    if (Spritej1) {
        Spritej1->x = WIDTH * 0.25f;
        Spritej1->y = HEIGHT * 0.15f;
        Spritej1->pv = 1000;
        Spritej1->nrj = 0;
        sfFixResetSpriteHistory(Spritej1);
    }

    if (Spritej2) {
        Spritej2->x = WIDTH * 0.70f;
        Spritej2->y = HEIGHT * 0.75f;
        Spritej2->pv = 1000;
        Spritej2->nrj = 0;
        sfFixResetSpriteHistory(Spritej2);
    }

    if (loosej1) loosej1->pv = 0;
    if (loosej2) loosej2->pv = 0;

    // Restore the invisible IA cascade to a reproducible start state.
    const float aix = Spritej1 ? Spritej1->x : WIDTH * 0.25f;
    const float aiy = Spritej1 ? Spritej1->y : HEIGHT * 0.15f;
    sfFixResetEnti(iago, aix, aiy);
    sfFixResetEnti(iago1, aix, aiy);
    sfFixResetEnti(iacalc, aix, aiy);
    sfFixResetEnti(iatake, aix, aiy);

    if (Suiveur) {
        Suiveur->pv = 0;
        Suiveur->x = aix;
        Suiveur->y = aiy;
    }
    SpaceFortressLegacySuiveurMajaf = true;

    // Explicitly disable the remaster's first-generation time-normalized
    // velocity shim. The original Spritej2::unctrl() -> vx -> j2lock chain is
    // now authoritative again.
    sfRmTrackJ2 = false;
    sfRmJ2Finger = 0;
    sfRmJ2LastTick = 0;
    sfRmJ2Vx = 0.0f;
    sfRmJ2Vy = 0.0f;

    setgui = false;
}

static void sfFixApplyUiRequests()
{
    // This function is called only by the render/presentation thread.
    // Consequently setia, ships, iago and the follower state are no longer
    // mutated from SDL_WaitEvent's thread.
    sfRmTrackJ2 = false;

    if (sfFixLaunchPending.exchange(false)) {
        setia = sfFixRequestedIa.load();
        sfFixResetMatchState();
        sfUiScreen = SF_UI_GAME;
        sfFixRequestedScreen.store(SF_UI_GAME);
        return;
    }

    const int requested = sfFixRequestedScreen.load();
    if (requested != SF_UI_GAME) {
        // setia is harmless while setgui gates the old IA; keeping it in sync
        // also makes the home screen's mode label render correctly.
        setia = sfFixRequestedIa.load();
        setgui = true;
    }
    sfUiScreen = requested;
}

static void sfFixResetGearGesture()
{
    sfFixGear1Down = false;
    sfFixGear2Down = false;
    sfFixGear1Finger = 0;
    sfFixGear2Finger = 0;
}

// Remove the previous wrappers before defining the final event bridge. Calls to
// SDL_WaitEvent inside this function therefore resolve to the real SDL API.
#ifdef SDL_WaitEvent
#undef SDL_WaitEvent
#endif

static int SpaceFortressFinal_WaitEvent(SDL_Event *event)
{
    const int result = SDL_WaitEvent(event);
    if (!result || !event) return result;

    // Consume the remainder of a touch that started on HOME/HELP.
    if (sfFixConsumeFinger &&
        (event->type == SDL_FINGERMOTION || event->type == SDL_FINGERUP) &&
        event->tfinger.fingerId == sfFixConsumedFinger) {
        if (event->type == SDL_FINGERUP) sfFixConsumeFinger = false;
        event->type = SDL_USEREVENT;
        return result;
    }

    const int requestedScreen = sfFixRequestedScreen.load();

    if (requestedScreen != SF_UI_GAME) {
        if (event->type == SDL_FINGERDOWN) {
            sfFixConsumedFinger = event->tfinger.fingerId;
            sfFixConsumeFinger = true;
            const float y = event->tfinger.y;

            if (requestedScreen == SF_UI_HOME) {
                if (y >= 0.39f && y <= 0.52f) {
                    sfFixRequestedIa.store(!sfFixRequestedIa.load());
                } else if (y >= 0.545f && y <= 0.69f) {
                    sfFixLaunchPending.store(true);
                } else if (y >= 0.71f && y <= 0.85f) {
                    sfFixRequestedScreen.store(SF_UI_HELP);
                }
            } else if (requestedScreen == SF_UI_HELP && y >= 0.86f) {
                sfFixRequestedScreen.store(SF_UI_HOME);
            }
            event->type = SDL_USEREVENT;
        } else if (event->type == SDL_FINGERMOTION ||
                   event->type == SDL_FINGERUP) {
            event->type = SDL_USEREVENT;
        } else if (event->type == SDL_KEYDOWN &&
                   (event->key.keysym.sym == SDLK_ESCAPE
#ifdef SDLK_AC_BACK
                    || event->key.keysym.sym == SDLK_AC_BACK
#endif
                   )) {
            sfFixRequestedScreen.store(SF_UI_HOME);
            event->type = SDL_USEREVENT;
        }
        return result;
    }

    // Gameplay: preserve the historical event stream, intercepting only the
    // successful two-gear home gesture and Android/back.
    if (event->type == SDL_FINGERDOWN) {
        const SDL_FingerID fid = event->tfinger.fingerId;
        const bool hit1 = sfRmHitGear(event->tfinger, rouage1);
        const bool hit2 = sfRmHitGear(event->tfinger, rouage2);

        if (hit1) {
            sfFixGear1Down = true;
            sfFixGear1Finger = fid;
            event->type = SDL_USEREVENT;
        }
        if (hit2) {
            sfFixGear2Down = true;
            sfFixGear2Finger = fid;
            event->type = SDL_USEREVENT;
        }

        if (sfFixGear1Down && sfFixGear2Down &&
            sfFixGear1Finger != sfFixGear2Finger) {
            sfFixRequestedIa.store(setia);
            sfFixRequestedScreen.store(SF_UI_HOME);
            sfFixResetGearGesture();
            event->type = SDL_USEREVENT;
            return result;
        }
    } else if (event->type == SDL_FINGERUP) {
        const SDL_FingerID fid = event->tfinger.fingerId;
        if (sfFixGear1Down && fid == sfFixGear1Finger) {
            sfFixGear1Down = false;
            sfFixGear1Finger = 0;
        }
        if (sfFixGear2Down && fid == sfFixGear2Finger) {
            sfFixGear2Down = false;
            sfFixGear2Finger = 0;
        }
    } else if (event->type == SDL_KEYDOWN &&
               (event->key.keysym.sym == SDLK_ESCAPE
#ifdef SDLK_AC_BACK
                || event->key.keysym.sym == SDLK_AC_BACK
#endif
               )) {
        sfFixRequestedIa.store(setia);
        sfFixRequestedScreen.store(SF_UI_HOME);
        event->type = SDL_USEREVENT;
    }

    return result;
}

// Apply pending UI/IA changes immediately before the already-established final
// visual/presentation chain. All gameplay mutations therefore happen on one
// thread and the old IA sees a deterministic new-match state.
#ifdef SDL_RenderPresent
#undef SDL_RenderPresent
#endif

static void SpaceFortressFinal_RenderPresent(SDL_Renderer *renderer)
{
    sfFixApplyUiRequests();
    SpaceFortressVisualRestore_RenderPresent(renderer);
}

#define SDL_WaitEvent SpaceFortressFinal_WaitEvent
#define SDL_RenderPresent SpaceFortressFinal_RenderPresent

#endif // SPACEFORTRESS_REMASTER_AI_FIX_HPP
