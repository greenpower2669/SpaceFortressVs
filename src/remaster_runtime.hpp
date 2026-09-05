#ifndef SPACEFORTRESS_REMASTER_RUNTIME_HPP
#define SPACEFORTRESS_REMASTER_RUNTIME_HPP

// 2026 remaster layer, deliberately kept outside the historical main.cpp.
// It adds Android-safe input, restores the old IA launch state, normalizes the
// historical predictive lead, and renders living scenery / weapon VFX without
// changing collision geometry or the original game rules.

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstdint>

extern sprite *Spritej1;
extern sprite *Spritej2;
extern sprite *rouage1;
extern sprite *rouage2;
extern enti *iago;
extern bool setgui;
extern bool setia;
extern int tirj1;
extern int tirj2;
extern float tw;
extern float th;

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
static int sfRmPreviousUiScreen = SF_UI_HOME;

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
    { false, 0.0f, 0.0f, 0.0f, 0.0f, 0, 700, 4200 };

static uint32_t sfRmHash(uint32_t x)
{
    x ^= x >> 16;
    x *= 0x7feb352du;
    x ^= x >> 15;
    x *= 0x846ca68bu;
    x ^= x >> 16;
    return x;
}

static void sfRmFilledCircle(SDL_Renderer *renderer, int cx, int cy, int radius,
                             Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    if (!renderer || radius <= 0) return;
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    for (int y = -radius; y <= radius; ++y) {
        const int yy = y * y;
        const int xx = static_cast<int>(std::sqrt(
            static_cast<double>(std::max(0, radius * radius - yy))));
        SDL_RenderDrawLine(renderer, cx - xx, cy + y, cx + xx, cy + y);
    }
}

static void sfRmDrawShootingStar(SDL_Renderer *renderer, int width, int height)
{
    if (!renderer || width <= 0 || height <= 0) return;

    const Uint64 now = SDL_GetTicks64();
    if (!sfRmStar.active && now >= sfRmStar.nextSpawn) {
        sfRmStar.active = true;
        sfRmStar.start = now;
        sfRmStar.duration = 520 + static_cast<Uint64>(rand() % 420);
+        sfRmStar.x = static_cast<float>(rand() % width);
+        sfRmStar.y = static_cast<float>(rand() % std::max(1, height * 2 / 3));
+
+        const float dir = (rand() % 2) ? 1.0f : -1.0f;
+        sfRmStar.vx = dir * (0.42f + (rand() % 35) * 0.01f) * width;
+        sfRmStar.vy = (0.28f + (rand() % 28) * 0.01f) * height;
+    }
+
+    if (!sfRmStar.active) return;
+
+    const Uint64 age = now - sfRmStar.start;
+    if (age >= sfRmStar.duration) {
+        sfRmStar.active = false;
+        sfRmStar.nextSpawn = now + 6500 + static_cast<Uint64>(rand() % 11500);
+        return;
+    }
+
+    const float t = static_cast<float>(age) /
+                    static_cast<float>(sfRmStar.duration);
+    const float x = sfRmStar.x + sfRmStar.vx * t;
+    const float y = sfRmStar.y + sfRmStar.vy * t;
+    const float speed = std::sqrt(sfRmStar.vx * sfRmStar.vx +
+                                  sfRmStar.vy * sfRmStar.vy);
+    if (speed <= 0.01f) return;
+
+    const float ux = sfRmStar.vx / speed;
+    const float uy = sfRmStar.vy / speed;
+    const float len = width * 0.11f;
+    const float fade = std::sin(3.14159265f * t);
+
+    SDL_SetRenderDrawColor(renderer, 130, 195, 255,
+                           static_cast<Uint8>(110 * fade));
+    SDL_RenderDrawLine(renderer,
+                      static_cast<int>(x - ux * len),
+                      static_cast<int>(y - uy * len),
+                      static_cast<int>(x), static_cast<int>(y));
+    SDL_SetRenderDrawColor(renderer, 245, 252, 255,
+                           static_cast<Uint8>(245 * fade));
+    SDL_RenderDrawLine(renderer,
+                      static_cast<int>(x - ux * len * 0.28f),
+                      static_cast<int>(y - uy * len * 0.28f),
+                      static_cast<int>(x), static_cast<int>(y));
+}
+
+static void sfRmDrawScenicBodies(SDL_Renderer *renderer, int width, int height,
+                                 Uint64 now)
+{
+    // The old remaster baked a planet and sun into one JPG. They now live as
+    // independent timed events. Most of the time the view is open deep space.
+    const Uint64 cycle = now % 44000u;
+
+    // Warm star: slowly grazes the upper-left edge, then leaves again.
+    if (cycle >= 6000u && cycle <= 17500u) {
+        const float p = static_cast<float>(cycle - 6000u) / 11500.0f;
+        const float reveal = std::sin(3.14159265f * p);
+        const int radius = std::max(24, static_cast<int>(width * 0.13f));
+        const int cx = static_cast<int>(-radius * 0.58f + reveal * radius * 0.62f);
+        const int cy = static_cast<int>(height * (0.14f + 0.025f * std::sin(p * 6.28f)));
+
+        sfRmFilledCircle(renderer, cx, cy, radius + 22, 255, 105, 22,
+                         static_cast<Uint8>(18 * reveal));
+        sfRmFilledCircle(renderer, cx, cy, radius + 10, 255, 145, 45,
+                         static_cast<Uint8>(28 * reveal));
+        sfRmFilledCircle(renderer, cx, cy, radius, 238, 166, 72,
+                         static_cast<Uint8>(185 * reveal));
+        sfRmFilledCircle(renderer, cx - radius / 5, cy - radius / 7,
+                         radius * 3 / 4, 255, 205, 105,
+                         static_cast<Uint8>(80 * reveal));
+    }
+
+    // Blue planet limb: a separate slow passage near the lower-right edge.
+    if (cycle >= 24500u && cycle <= 40500u) {
+        const float p = static_cast<float>(cycle - 24500u) / 16000.0f;
+        const float reveal = std::sin(3.14159265f * p);
+        const int radius = std::max(40, static_cast<int>(width * 0.30f));
+        const int cx = static_cast<int>(width + radius * 0.70f -
+                                        reveal * radius * 0.84f);
+        const int cy = static_cast<int>(height * (0.78f + 0.018f * std::sin(p * 6.28f)));
+
+        sfRmFilledCircle(renderer, cx, cy, radius + 7, 60, 155, 235,
+                         static_cast<Uint8>(42 * reveal));
+        sfRmFilledCircle(renderer, cx, cy, radius, 9, 28, 60,
+                         static_cast<Uint8>(245 * reveal));
+        sfRmFilledCircle(renderer, cx - radius / 6, cy - radius / 8,
+                         radius * 5 / 6, 26, 77, 125,
+                         static_cast<Uint8>(175 * reveal));
+        sfRmFilledCircle(renderer, cx - radius / 3, cy - radius / 4,
+                         radius * 3 / 5, 65, 145, 190,
+                         static_cast<Uint8>(50 * reveal));
+    }
+}
+
+static void sfRmDrawLiveBackground(SDL_Renderer *renderer)
+{
+    if (!renderer || sfUiScreen != SF_UI_GAME) return;
+
+    int width = 0, height = 0;
+    SDL_GetRendererOutputSize(renderer, &width, &height);
+    if (width <= 0 || height <= 0) return;
+
+    const Uint64 now = SDL_GetTicks64();
+    const double seconds = static_cast<double>(now) * 0.001;
+    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
+
+    // Slow translucent nebula wisps. They drift independently of the stars.
+    for (int i = 0; i < 7; ++i) {
+        const double phase = seconds * (0.012 + i * 0.0015) + i * 1.73;
+        const int x = static_cast<int>(width * (0.5 + 0.46 * std::sin(phase)));
+        const int y = static_cast<int>(height * (0.12 + i * 0.135));
+        const int w = std::max(18, width / 5 + (i % 3) * width / 12);
+        const int h = std::max(5, height / 120);
+        SDL_Rect haze = {x - w / 2, y, w, h};
+        SDL_SetRenderDrawColor(renderer,
+                               static_cast<Uint8>(28 + i * 5),
+                               static_cast<Uint8>(38 + i * 6),
+                               static_cast<Uint8>(72 + i * 8), 18);
+        SDL_RenderFillRect(renderer, &haze);
+    }
+
+    // Three parallax layers. Coordinates are deterministic, but every layer
+    // moves at its own speed and each star has an independent twinkle phase.
+    const int layerCount[3] = {58, 42, 24};
+    const float speed[3] = {0.0028f, 0.0060f, 0.0115f};
+    for (int layer = 0; layer < 3; ++layer) {
+        for (int i = 0; i < layerCount[layer]; ++i) {
+            const uint32_t h1 = sfRmHash(0x51f15eadu + layer * 1009u + i * 7919u);
+            const uint32_t h2 = sfRmHash(h1 ^ 0xa3c59ac3u);
+            const float baseX = static_cast<float>(h1 % 10000u) / 10000.0f;
+            const float baseY = static_cast<float>(h2 % 10000u) / 10000.0f;
+            const float drift = static_cast<float>(seconds) * speed[layer];
+            float ny = std::fmod(baseY + drift, 1.0f);
+            if (ny < 0.0f) ny += 1.0f;
+            const float sway = 0.006f * layer *
+                               std::sin(static_cast<float>(seconds * 0.10 + i));
+            float nx = std::fmod(baseX + sway + 1.0f, 1.0f);
+
+            const int x = static_cast<int>(nx * width);
+            const int y = static_cast<int>(ny * height);
+            const float twinkle = 0.58f + 0.42f *
+                std::sin(static_cast<float>(seconds * (1.1 + layer * 0.45) +
+                                                   (h1 % 97u) * 0.19));
+            const Uint8 alpha = static_cast<Uint8>(
+                std::max(35.0f, (90.0f + layer * 52.0f) * twinkle));
+
+            if (layer == 0)
+                SDL_SetRenderDrawColor(renderer, 150, 180, 210, alpha);
+            else if (layer == 1)
+                SDL_SetRenderDrawColor(renderer, 190, 220, 245, alpha);
+            else
+                SDL_SetRenderDrawColor(renderer, 235, 247, 255, alpha);
+
+            SDL_RenderDrawPoint(renderer, x, y);
+            if (layer == 2 && (i % 4) == 0) {
+                SDL_RenderDrawPoint(renderer, x + 1, y);
+                SDL_RenderDrawPoint(renderer, x, y + 1);
+            }
+        }
+    }
+
+    sfRmDrawScenicBodies(renderer, width, height, now);
+    sfRmDrawShootingStar(renderer, width, height);
+    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
+}
+
+// This wrapper is defined before the SDL_RenderClear macro installed below, so
+// the call here is the real SDL call. It paints the living background directly
+// after the historical black clear and therefore remains behind all gameplay.
+static int SpaceFortressRemaster_RenderClear(SDL_Renderer *renderer)
+{
+    const int result = SDL_RenderClear(renderer);
+    if (result == 0) sfRmDrawLiveBackground(renderer);
+    return result;
+}
+
+static bool sfRmHitGear(const SDL_TouchFingerEvent &finger, sprite *gear)
+{
+    if (!gear || tw <= 0.0f || th <= 0.0f) return false;
+    const float px = finger.x * tw;
+    const float py = finger.y * th * 1.09f;
+    const float dx = px - gear->x;
+    const float dy = py - gear->y;
+    const float radius = std::max(gear->w, gear->h) * 0.95f;
+    return (dx * dx + dy * dy) <= radius * radius;
+}
+
+static void sfRmResetGearTouches()
+{
+    sfRmGear1Down = false;
+    sfRmGear2Down = false;
+    sfRmGear1Finger = 0;
+    sfRmGear2Finger = 0;
+}
+
+static void sfRmGoHome()
+{
+    sfUiScreen = SF_UI_HOME;
+    // Keep the historical engine in its GUI/options state while the new home
+    // screen is visible. This prevents IA and controls running behind the menu.
+    setgui = true;
+    sfRmResetGearTouches();
+    sfRmTrackJ2 = false;
+
+    if (Spritej1) {
+        Spritej1->ctrl = false;
+        Spritej1->id = 100;
+    }
+    if (Spritej2) {
+        Spritej2->ctrl = false;
+        Spritej2->id = 100;
+    }
+}
+
+static void sfRmResetAiAnchor()
+{
+    if (!setia || !Spritej1 || !iago) return;
+    // Old IA overwrites Spritej1 from iago->xy. Reset that invisible pointer
+    // at the instant the match starts so a pointer that wandered behind the
+    // menu cannot teleport the IA ship off screen on the first update.
+    iago->xy.setxy(Spritej1->x, Spritej1->y);
+}
+
+static void sfRmSyncUiEngineState()
+{
+    if (sfUiScreen != SF_UI_GAME) {
+        setgui = true;
+        sfRmTrackJ2 = false;
+    } else if (sfRmPreviousUiScreen != SF_UI_GAME) {
+        sfRmResetAiAnchor();
+        setgui = false;
+        sfRmJ2LastTick = 0;
+        sfRmJ2Vx = sfRmJ2Vy = 0.0f;
+    }
+    sfRmPreviousUiScreen = sfUiScreen;
+}
+
+static void sfRmUpdateJ2Velocity(const SDL_TouchFingerEvent &finger,
+                                 bool firstSample)
+{
+    if (tw <= 0.0f || th <= 0.0f) return;
+    const Uint64 now = SDL_GetTicks64();
+    const float px = finger.x * tw;
+    const float py = finger.y * th;
+
+    if (firstSample || sfRmJ2LastTick == 0) {
+        sfRmJ2LastX = px;
+        sfRmJ2LastY = py;
+        sfRmJ2LastTick = now;
+        sfRmJ2Vx = sfRmJ2Vy = 0.0f;
+        return;
+    }
+
+    const Uint64 dtMs = now - sfRmJ2LastTick;
+    if (dtMs > 0 && dtMs <= 120) {
+        const float scale = 10.0f / static_cast<float>(dtMs);
+        const float rawVx = (px - sfRmJ2LastX) * scale;
+        const float rawVy = (py - sfRmJ2LastY) * scale;
+        sfRmJ2Vx = sfRmJ2Vx * 0.50f + rawVx * 0.50f;
+        sfRmJ2Vy = sfRmJ2Vy * 0.50f + rawVy * 0.50f;
+    } else if (dtMs > 120) {
+        sfRmJ2Vx = sfRmJ2Vy = 0.0f;
+    }
+
+    sfRmJ2LastX = px;
+    sfRmJ2LastY = py;
+    sfRmJ2LastTick = now;
+}
+
+static int SpaceFortressRemaster_WaitEvent(SDL_Event *event)
+{
+    const int result = SpaceFortressUi_WaitEvent(event);
+    sfRmSyncUiEngineState();
+    if (!result || !event || sfUiScreen != SF_UI_GAME) return result;
+
+    if (event->type == SDL_FINGERDOWN) {
+        const SDL_FingerID fid = event->tfinger.fingerId;
+        const bool hit1 = sfRmHitGear(event->tfinger, rouage1);
+        const bool hit2 = sfRmHitGear(event->tfinger, rouage2);
+
+        if (hit1) {
+            sfRmGear1Down = true;
+            sfRmGear1Finger = fid;
+            event->type = SDL_USEREVENT;
+        }
+        if (hit2) {
+            sfRmGear2Down = true;
+            sfRmGear2Finger = fid;
+            event->type = SDL_USEREVENT;
+        }
+        if (sfRmGear1Down && sfRmGear2Down &&
+            sfRmGear1Finger != sfRmGear2Finger) {
+            sfRmGoHome();
+            event->type = SDL_USEREVENT;
+            return result;
+        }
+
+        const float py = event->tfinger.y * th;
+        if (setia && !hit1 && !hit2 && py > HEIGHT * 0.5f &&
+            Spritej2 && Spritej2->id == 100) {
+            sfRmTrackJ2 = true;
+            sfRmJ2Finger = fid;
+            sfRmUpdateJ2Velocity(event->tfinger, true);
+        }
+    } else if (event->type == SDL_FINGERMOTION) {
+        const SDL_FingerID fid = event->tfinger.fingerId;
+        if (sfRmTrackJ2 && fid == sfRmJ2Finger)
+            sfRmUpdateJ2Velocity(event->tfinger, false);
+    } else if (event->type == SDL_FINGERUP) {
+        const SDL_FingerID fid = event->tfinger.fingerId;
+        if (sfRmGear1Down && fid == sfRmGear1Finger) {
+            sfRmGear1Down = false;
+            sfRmGear1Finger = 0;
+        }
+        if (sfRmGear2Down && fid == sfRmGear2Finger) {
+            sfRmGear2Down = false;
+            sfRmGear2Finger = 0;
+        }
+        if (sfRmTrackJ2 && fid == sfRmJ2Finger) {
+            sfRmUpdateJ2Velocity(event->tfinger, false);
+            sfRmTrackJ2 = false;
+            sfRmJ2Finger = 0;
+        }
+    }
+    return result;
+}
+
+static void sfRmEnsureTextures(SDL_Renderer *renderer)
+{
+    if (!renderer || sfRmMuzzleStrip) return;
+    sfRmMuzzleStrip = IMG_LoadTexture(
+        renderer, "./resources/assets/pict/remaster/muzzle_strip.png");
+    if (sfRmMuzzleStrip) {
+        SDL_QueryTexture(sfRmMuzzleStrip, NULL, NULL, &sfRmMuzzleW, &sfRmMuzzleH);
+        SDL_SetTextureBlendMode(sfRmMuzzleStrip, SDL_BLENDMODE_BLEND);
+    }
+}
+
+static void sfRmDrawCorePulse(SDL_Renderer *renderer, sprite *ship,
+                              Uint8 r, Uint8 g, Uint8 b, bool inverted)
+{
+    if (!renderer || !ship || !sfRmMuzzleStrip) return;
+    const Uint64 now = SDL_GetTicks64();
+    const float wave = 0.5f + 0.5f * std::sin(now * 0.006);
+    const int size = static_cast<int>(std::max(ship->w, ship->h) *
+                                      (0.16f + 0.035f * wave));
+    if (size <= 0) return;
+
+    const int frameW = sfRmMuzzleW > 0 ? sfRmMuzzleW / 4 : 1;
+    SDL_Rect src = {0, 0, frameW, std::max(1, sfRmMuzzleH)};
+    SDL_Rect dst = {static_cast<int>(ship->x - size * 0.5f),
+                    static_cast<int>(ship->y - size * 0.5f), size, size};
+    SDL_SetTextureColorMod(sfRmMuzzleStrip, r, g, b);
+    SDL_SetTextureAlphaMod(sfRmMuzzleStrip,
+                           static_cast<Uint8>(75 + wave * 70));
+    SDL_RenderCopyEx(renderer, sfRmMuzzleStrip, &src, &dst,
+                     inverted ? 180.0 : 0.0, NULL, SDL_FLIP_NONE);
+}
+
+static void sfRmDrawMuzzleFlash(SDL_Renderer *renderer, sprite *ship,
+                                Uint64 flashStart, bool upward,
+                                Uint8 r, Uint8 g, Uint8 b)
+{
+    if (!renderer || !ship || !sfRmMuzzleStrip || flashStart == 0) return;
+    const Uint64 age = SDL_GetTicks64() - flashStart;
+    if (age >= 180) return;
+    int frame = std::min(3, static_cast<int>(age / 45));
+    const int frameW = sfRmMuzzleW > 0 ? sfRmMuzzleW / 4 : 1;
+    SDL_Rect src = {frame * frameW, 0, frameW, std::max(1, sfRmMuzzleH)};
+    const int size = static_cast<int>(std::max(ship->w, ship->h) * 0.34f);
+    if (size <= 0) return;
+
+    const float forward = ship->h * 0.42f;
+    const float side = ship->w * 0.22f;
+    const float fy = ship->y + (upward ? -forward : forward);
+    SDL_SetTextureColorMod(sfRmMuzzleStrip, r, g, b);
+    SDL_SetTextureAlphaMod(sfRmMuzzleStrip, 235);
+    for (int s = -1; s <= 1; s += 2) {
+        SDL_Rect dst = {static_cast<int>(ship->x + side * s - size * 0.5f),
+                        static_cast<int>(fy - size * 0.5f), size, size};
+        SDL_RenderCopyEx(renderer, sfRmMuzzleStrip, &src, &dst,
+                         upward ? 180.0 : 0.0, NULL, SDL_FLIP_NONE);
+    }
+}
+
+static void sfRmApplyLegacyLeadVelocity()
+{
+    if (sfUiScreen != SF_UI_GAME || !setia || !Spritej2 ||
+        !Spritej2->ctrl || !sfRmTrackJ2) return;
+
+    const Uint64 now = SDL_GetTicks64();
+    if (sfRmJ2LastTick == 0 || now - sfRmJ2LastTick > 85) {
+        Spritej2->vx = 0.0f;
+        Spritej2->vy = 0.0f;
+        return;
+    }
+    Spritej2->vx = sfRmJ2Vx;
+    Spritej2->vy = sfRmJ2Vy;
+}
+
+static void SpaceFortressRemaster_RenderPresent(SDL_Renderer *renderer)
+{
+    if (renderer && sfUiScreen == SF_UI_GAME) {
+        sfRmApplyLegacyLeadVelocity();
+        sfRmEnsureTextures(renderer);
+
+        const Uint64 now = SDL_GetTicks64();
+        if (tirj1 > sfRmLastTirJ1) sfRmFlashJ1Start = now;
+        if (tirj2 > sfRmLastTirJ2) sfRmFlashJ2Start = now;
+        sfRmLastTirJ1 = tirj1;
+        sfRmLastTirJ2 = tirj2;
+
+        sfRmDrawCorePulse(renderer, Spritej1, 255, 145, 65, false);
+        sfRmDrawCorePulse(renderer, Spritej2, 80, 185, 255, true);
+        sfRmDrawMuzzleFlash(renderer, Spritej1, sfRmFlashJ1Start,
+                            false, 255, 145, 55);
+        sfRmDrawMuzzleFlash(renderer, Spritej2, sfRmFlashJ2Start,
+                            true, 90, 195, 255);
+
+        if (sfRmMuzzleStrip) {
+            SDL_SetTextureColorMod(sfRmMuzzleStrip, 255, 255, 255);
+            SDL_SetTextureAlphaMod(sfRmMuzzleStrip, 255);
+        }
+    }
+
+    SpaceFortressUi_RenderPresent(renderer);
+    // start_ui may have converted the historical setgui gear signal into HOME.
+    // Reassert the pause after that conversion so IA cannot run behind HOME.
+    sfRmSyncUiEngineState();
+}
+
+// start_ui.hpp installed the first compatibility hooks. Replace only the final
+// public macros; its wrappers remain callable. SDL_RenderClear is additionally
+// hooked so the living scenery is guaranteed to stay behind historical sprites.
+#ifdef SDL_WaitEvent
+#undef SDL_WaitEvent
+#endif
+#ifdef SDL_RenderPresent
+#undef SDL_RenderPresent
+#endif
+#ifdef SDL_RenderClear
+#undef SDL_RenderClear
+#endif
+#define SDL_WaitEvent SpaceFortressRemaster_WaitEvent
+#define SDL_RenderPresent SpaceFortressRemaster_RenderPresent
+#define SDL_RenderClear SpaceFortressRemaster_RenderClear
+
+#endif // SPACEFORTRESS_REMASTER_RUNTIME_HPP
