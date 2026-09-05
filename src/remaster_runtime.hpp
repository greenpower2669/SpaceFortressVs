#ifndef SPACEFORTRESS_REMASTER_RUNTIME_HPP
#define SPACEFORTRESS_REMASTER_RUNTIME_HPP

// 2026 remaster layer.
// Loaded after start_ui.hpp so the historical engine remains untouched.
// It adds:
//   - robust Android two-gear return-to-home handling,
//   - time-normalized human velocity for the historical j2lock AI lead,
//   - animated cannon flashes / energy pulse,
//   - occasional shooting stars.

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>

extern sprite *Spritej1;
extern sprite *Spritej2;
extern sprite *rouage1;
extern sprite *rouage2;
extern bool setgui;
extern bool setia;
extern int tirj1;
extern int tirj2;
extern float tw;
extern float th;
extern double angletest;

static bool sfRmGear1Down = false;
static bool sfRmGear2Down = false;
static SDL_FingerID sfRmGear1Finger = 0;
static SDL_FingerID sfRmGear2Finger = 0;

static bool sfRmTrackJ2 = false;
static SDL_FingerID sfRmJ2Finger = 0;
static float sfRmJ2LastX = 0.0f;
static float sfRmJ2LastY = 0.0f;
static float sfRmJ2Vx = 0.0f;
static float sfRmJ2Vy = 0.0f;
static Uint64 sfRmJ2LastTick = 0;

static SDL_Texture *sfRmMuzzleStrip = NULL;
static int sfRmMuzzleW = 0;
static int sfRmMuzzleH = 0;
static int sfRmLastTirJ1 = 0;
static int sfRmLastTirJ2 = 0;
static Uint64 sfRmFlashJ1Start = 0;
static Uint64 sfRmFlashJ2Start = 0;

struct SpaceFortressShootingStar {
    bool active;
    float x;
    float y;
    float vx;
    float vy;
    Uint64 start;
    Uint64 duration;
    Uint64 nextSpawn;
};

static SpaceFortressShootingStar sfRmStar =
    { false, 0.0f, 0.0f, 0.0f, 0.0f, 0, 700, 3500 };

static bool sfRmHitGear(const SDL_TouchFingerEvent &finger, sprite *gear)
{
    if (!gear || tw <= 0.0f || th <= 0.0f) return false;

    // Match the historical touch coordinate conversion:
    // tx = finger.x * tw; ty = finger.y * th, with the touch FX using y * 1.09.
    const float px = finger.x * tw;
    const float py = finger.y * th * 1.09f;
    const float dx = px - gear->x;
    const float dy = py - gear->y;
    const float radius = std::max(gear->w, gear->h) * 0.95f;

    return (dx * dx + dy * dy) <= radius * radius;
}

static void sfRmResetGearTouches()
{
    sfRmGear1Down = false;
    sfRmGear2Down = false;
    sfRmGear1Finger = 0;
    sfRmGear2Finger = 0;
}

static void sfRmGoHome()
{
    sfUiScreen = SF_UI_HOME;
    setgui = false;
    sfRmResetGearTouches();

    // Avoid leaving a finger captured by the historical controller.
    if (Spritej1) {
        Spritej1->ctrl = false;
        Spritej1->id = 100;
    }
    if (Spritej2) {
        Spritej2->ctrl = false;
        Spritej2->id = 100;
    }
}

static void sfRmUpdateJ2Velocity(const SDL_TouchFingerEvent &finger, bool firstSample)
{
    if (tw <= 0.0f || th <= 0.0f) return;

    const Uint64 now = SDL_GetTicks64();
    const float px = finger.x * tw;
    const float py = finger.y * th;

    if (firstSample || sfRmJ2LastTick == 0) {
        sfRmJ2LastX = px;
        sfRmJ2LastY = py;
        sfRmJ2LastTick = now;
        sfRmJ2Vx = 0.0f;
        sfRmJ2Vy = 0.0f;
        return;
    }

    const Uint64 dtMs = now - sfRmJ2LastTick;
    if (dtMs > 0 && dtMs <= 120) {
        // The old tuning was effectively made around ~100 fps.
        // Convert real touch velocity back to "pixels per legacy 10 ms frame".
        const float legacyFrameMs = 10.0f;
        const float scale = legacyFrameMs / static_cast<float>(dtMs);
        const float rawVx = (px - sfRmJ2LastX) * scale;
        const float rawVy = (py - sfRmJ2LastY) * scale;

        // The historical x000/x00/x0 code averaged roughly two samples.
        // This low-pass keeps the same smooth, predictive personality.
        sfRmJ2Vx = sfRmJ2Vx * 0.50f + rawVx * 0.50f;
        sfRmJ2Vy = sfRmJ2Vy * 0.50f + rawVy * 0.50f;
    } else if (dtMs > 120) {
        sfRmJ2Vx = 0.0f;
        sfRmJ2Vy = 0.0f;
    }

    sfRmJ2LastX = px;
    sfRmJ2LastY = py;
    sfRmJ2LastTick = now;
}

static int SpaceFortressRemaster_WaitEvent(SDL_Event *event)
{
    const int result = SpaceFortressUi_WaitEvent(event);
    if (!result || !event || sfUiScreen != SF_UI_GAME) return result;

    if (event->type == SDL_FINGERDOWN) {
        const SDL_FingerID fid = event->tfinger.fingerId;
        const bool hit1 = sfRmHitGear(event->tfinger, rouage1);
        const bool hit2 = sfRmHitGear(event->tfinger, rouage2);

        if (hit1) {
            sfRmGear1Down = true;
            sfRmGear1Finger = fid;
            event->type = SDL_USEREVENT;
        }
        if (hit2) {
            sfRmGear2Down = true;
            sfRmGear2Finger = fid;
            event->type = SDL_USEREVENT;
        }

        if (sfRmGear1Down && sfRmGear2Down &&
            sfRmGear1Finger != sfRmGear2Finger) {
            sfRmGoHome();
            event->type = SDL_USEREVENT;
            return result;
        }

        // In IA mode player 2 is the human pilot. Track the control finger
        // before the historical dispatcher assigns Spritej2->id.
        const float py = event->tfinger.y * th;
        if (setia && !hit1 && !hit2 && py > HEIGHT * 0.5f &&
            Spritej2 && Spritej2->id == 100) {
            sfRmTrackJ2 = true;
            sfRmJ2Finger = fid;
            sfRmUpdateJ2Velocity(event->tfinger, true);
        }
    } else if (event->type == SDL_FINGERMOTION) {
        const SDL_FingerID fid = event->tfinger.fingerId;
        if (sfRmTrackJ2 && fid == sfRmJ2Finger) {
            sfRmUpdateJ2Velocity(event->tfinger, false);
        }
    } else if (event->type == SDL_FINGERUP) {
        const SDL_FingerID fid = event->tfinger.fingerId;

        if (sfRmGear1Down && fid == sfRmGear1Finger) {
            sfRmGear1Down = false;
            sfRmGear1Finger = 0;
        }
        if (sfRmGear2Down && fid == sfRmGear2Finger) {
            sfRmGear2Down = false;
            sfRmGear2Finger = 0;
        }

        if (sfRmTrackJ2 && fid == sfRmJ2Finger) {
            sfRmUpdateJ2Velocity(event->tfinger, false);
            sfRmTrackJ2 = false;
            sfRmJ2Finger = 0;
        }
    }

    return result;
}

static void sfRmEnsureTextures(SDL_Renderer *renderer)
{
    if (!renderer || sfRmMuzzleStrip) return;
    sfRmMuzzleStrip = IMG_LoadTexture(
        renderer, "./resources/assets/pict/remaster/muzzle_strip.png");

    if (sfRmMuzzleStrip) {
        SDL_QueryTexture(sfRmMuzzleStrip, NULL, NULL, &sfRmMuzzleW, &sfRmMuzzleH);
        SDL_SetTextureBlendMode(sfRmMuzzleStrip, SDL_BLENDMODE_BLEND);
    }
}

static void sfRmDrawCorePulse(SDL_Renderer *renderer, sprite *ship,
                              Uint8 r, Uint8 g, Uint8 b, bool inverted)
{
    if (!renderer || !ship || !sfRmMuzzleStrip) return;

    const Uint64 now = SDL_GetTicks64();
    const float wave = 0.5f + 0.5f * std::sin(now * 0.006);
    const int size = static_cast<int>(std::max(ship->w, ship->h) *
                                      (0.16f + 0.035f * wave));
    if (size <= 0) return;

    const int frameW = sfRmMuzzleW > 0 ? sfRmMuzzleW / 4 : 1;
    const int frameH = sfRmMuzzleH > 0 ? sfRmMuzzleH : 1;
    SDL_Rect src = {0, 0, frameW, frameH};
    SDL_Rect dst = {
        static_cast<int>(ship->x - size * 0.5f),
        static_cast<int>(ship->y - size * 0.5f),
        size, size
    };

    SDL_SetTextureColorMod(sfRmMuzzleStrip, r, g, b);
    SDL_SetTextureAlphaMod(sfRmMuzzleStrip,
                           static_cast<Uint8>(75 + wave * 70));
    SDL_RenderCopyEx(renderer, sfRmMuzzleStrip, &src, &dst,
                     inverted ? 180.0 : 0.0, NULL, SDL_FLIP_NONE);
}

static void sfRmDrawMuzzleFlash(SDL_Renderer *renderer, sprite *ship,
                                Uint64 flashStart, bool upward,
                                Uint8 r, Uint8 g, Uint8 b)
{
    if (!renderer || !ship || !sfRmMuzzleStrip || flashStart == 0) return;

    const Uint64 now = SDL_GetTicks64();
    const Uint64 age = now - flashStart;
    const Uint64 flashDuration = 180;
    if (age >= flashDuration) return;

    int frame = static_cast<int>(age / 45);
    if (frame > 3) frame = 3;

    const int frameW = sfRmMuzzleW > 0 ? sfRmMuzzleW / 4 : 1;
    const int frameH = sfRmMuzzleH > 0 ? sfRmMuzzleH : 1;
    SDL_Rect src = {frame * frameW, 0, frameW, frameH};
    const int size = static_cast<int>(std::max(ship->w, ship->h) * 0.34f);
    if (size <= 0) return;

    const float forward = ship->h * 0.42f;
    const float side = ship->w * 0.22f;
    const float fy = ship->y + (upward ? -forward : forward);

    SDL_SetTextureColorMod(sfRmMuzzleStrip, r, g, b);
    SDL_SetTextureAlphaMod(sfRmMuzzleStrip, 235);

    for (int s = -1; s <= 1; s += 2) {
        SDL_Rect dst = {
            static_cast<int>(ship->x + side * s - size * 0.5f),
            static_cast<int>(fy - size * 0.5f),
            size, size
        };
        SDL_RenderCopyEx(renderer, sfRmMuzzleStrip, &src, &dst,
                         upward ? 180.0 : 0.0, NULL, SDL_FLIP_NONE);
    }
}

static void sfRmDrawShootingStar(SDL_Renderer *renderer)
{
    if (!renderer) return;

    int width = 0, height = 0;
    SDL_GetRendererOutputSize(renderer, &width, &height);
    if (width <= 0 || height <= 0) return;

    const Uint64 now = SDL_GetTicks64();

    if (!sfRmStar.active && now >= sfRmStar.nextSpawn) {
        sfRmStar.active = true;
        sfRmStar.start = now;
        sfRmStar.duration = 520 + static_cast<Uint64>(rand() % 360);
        sfRmStar.x = static_cast<float>(rand() % width);
        sfRmStar.y = static_cast<float>(rand() % std::max(1, height * 2 / 3));

        const float dir = (rand() % 2) ? 1.0f : -1.0f;
        sfRmStar.vx = dir * (0.38f + (rand() % 35) * 0.01f) * width;
        sfRmStar.vy = (0.30f + (rand() % 30) * 0.01f) * height;
    }

    if (!sfRmStar.active) return;

    const Uint64 age = now - sfRmStar.start;
    if (age >= sfRmStar.duration) {
        sfRmStar.active = false;
        sfRmStar.nextSpawn = now + 5000 + static_cast<Uint64>(rand() % 9000);
        return;
    }

    const float t = static_cast<float>(age) /
                    static_cast<float>(sfRmStar.duration);
    const float x = sfRmStar.x + sfRmStar.vx * t;
    const float y = sfRmStar.y + sfRmStar.vy * t;
    const float speedLen = std::sqrt(sfRmStar.vx * sfRmStar.vx +
                                     sfRmStar.vy * sfRmStar.vy);
    if (speedLen <= 0.01f) return;

    const float ux = sfRmStar.vx / speedLen;
    const float uy = sfRmStar.vy / speedLen;
    const float len = width * 0.10f;
    const float tailX = x - ux * len;
    const float tailY = y - uy * len;
    const float fade = std::sin(3.14159265f * t);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, 205, 232, 255,
                           static_cast<Uint8>(170 * fade));
    SDL_RenderDrawLine(renderer, static_cast<int>(tailX),
                      static_cast<int>(tailY),
                      static_cast<int>(x), static_cast<int>(y));

    SDL_SetRenderDrawColor(renderer, 255, 255, 255,
                           static_cast<Uint8>(235 * fade));
    SDL_RenderDrawLine(renderer,
                      static_cast<int>(x - ux * len * 0.25f),
                      static_cast<int>(y - uy * len * 0.25f),
                      static_cast<int>(x), static_cast<int>(y));
    SDL_RenderDrawPoint(renderer, static_cast<int>(x),
                       static_cast<int>(y));
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

static void sfRmApplyLegacyLeadVelocity()
{
    if (!setia || !Spritej2 || !Spritej2->ctrl || !sfRmTrackJ2) return;

    const Uint64 now = SDL_GetTicks64();
    if (sfRmJ2LastTick == 0 || now - sfRmJ2LastTick > 85) {
        // Finger held still: old sampled velocity naturally fell back to zero.
        Spritej2->vx = 0.0f;
        Spritej2->vy = 0.0f;
        return;
    }

    Spritej2->vx = sfRmJ2Vx;
    Spritej2->vy = sfRmJ2Vy;
}

static void SpaceFortressRemaster_RenderPresent(SDL_Renderer *renderer)
{
    if (renderer && sfUiScreen == SF_UI_GAME) {
        sfRmApplyLegacyLeadVelocity();
        sfRmEnsureTextures(renderer);

        const Uint64 now = SDL_GetTicks64();

        if (tirj1 > sfRmLastTirJ1) sfRmFlashJ1Start = now;
        if (tirj2 > sfRmLastTirJ2) sfRmFlashJ2Start = now;
        sfRmLastTirJ1 = tirj1;
        sfRmLastTirJ2 = tirj2;

        // Existing player textures already rotate in the historical renderer.
        // These animated energy cores make the remaster visibly alive without
        // changing collision geometry or gameplay.
        sfRmDrawCorePulse(renderer, Spritej1, 255, 145, 65, false);
        sfRmDrawCorePulse(renderer, Spritej2, 80, 185, 255, true);

        sfRmDrawMuzzleFlash(renderer, Spritej1, sfRmFlashJ1Start,
                            false, 255, 145, 55);
        sfRmDrawMuzzleFlash(renderer, Spritej2, sfRmFlashJ2Start,
                            true, 90, 195, 255);

        sfRmDrawShootingStar(renderer);

        // Restore default modulation for the next frame.
        if (sfRmMuzzleStrip) {
            SDL_SetTextureColorMod(sfRmMuzzleStrip, 255, 255, 255);
            SDL_SetTextureAlphaMod(sfRmMuzzleStrip, 255);
        }
    }

    SpaceFortressUi_RenderPresent(renderer);
}

// start_ui.hpp installed the first compatibility hooks.
// Replace only the final public macros; the original wrappers stay callable.
#ifdef SDL_WaitEvent
#undef SDL_WaitEvent
#endif
#ifdef SDL_RenderPresent
#undef SDL_RenderPresent
#endif
#define SDL_WaitEvent SpaceFortressRemaster_WaitEvent
#define SDL_RenderPresent SpaceFortressRemaster_RenderPresent

#endif // SPACEFORTRESS_REMASTER_RUNTIME_HPP
