#ifndef SPACEFORTRESS_REMASTER_FRAME_FIX_HPP
#define SPACEFORTRESS_REMASTER_FRAME_FIX_HPP

// Final visual framing pass for Android.
// Keeps gameplay coordinates/collisions historical while ensuring modern round
// art is not visibly chopped by legacy destination rectangles at screen edges.

#ifdef __ANDROID__

#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>

static SDL_Texture *sfFinalGearOrangeTexture = NULL;

static SDL_Rect sfFinalClampRect(SDL_Rect rect, int boundW, int boundH, int margin)
{
    if (rect.w <= 0 || rect.h <= 0) return rect;
    if (boundW <= 0) boundW = WIDTH;
    if (boundH <= 0) boundH = HEIGHT;

    // If an old rectangle is larger than the game viewport, scale it down
    // proportionally before clamping. This is visual-only.
    const int maxW = std::max(1, boundW - margin * 2);
    const int maxH = std::max(1, boundH - margin * 2);
    if (rect.w > maxW || rect.h > maxH) {
        const float sx = static_cast<float>(maxW) / static_cast<float>(rect.w);
        const float sy = static_cast<float>(maxH) / static_cast<float>(rect.h);
        const float scale = std::min(sx, sy);
        const int cx = rect.x + rect.w / 2;
        const int cy = rect.y + rect.h / 2;
        rect.w = std::max(1, static_cast<int>(rect.w * scale));
        rect.h = std::max(1, static_cast<int>(rect.h * scale));
        rect.x = cx - rect.w / 2;
        rect.y = cy - rect.h / 2;
    }

    rect.x = std::max(margin, std::min(rect.x, boundW - margin - rect.w));
    rect.y = std::max(margin, std::min(rect.y, boundH - margin - rect.h));
    return rect;
}

static SDL_Rect sfFinalRoundBody(const SDL_Rect &legacy, bool sun)
{
    const int cx = legacy.x + legacy.w / 2;
    const int cy = legacy.y + legacy.h / 2;
    const int viewportW = WIDTH > 0 ? WIDTH : 780;
    const int viewportH = HEIGHT > 0 ? HEIGHT : 1680;

    const int legacyDiameter = std::max(1, std::min(legacy.w, legacy.h));
    const float fraction = sun ? 0.48f : 0.46f;
    const int cap = std::max(48, static_cast<int>(viewportW * fraction));

    const double seconds = static_cast<double>(SDL_GetTicks64()) * 0.001;
    const float pulse = 1.0f + (sun ? 0.012f : 0.007f) *
        static_cast<float>(std::sin(seconds * (sun ? 1.0 : 0.45)));
    const int diameter = std::max(32,
        static_cast<int>(std::min(legacyDiameter, cap) * pulse));

    SDL_Rect body = {cx - diameter / 2, cy - diameter / 2,
                     diameter, diameter};

    // Reserve enough room for the software aura so the visible result remains
    // fully round rather than being flattened by the viewport edge.
    const int auraMargin = std::max(5, static_cast<int>(diameter * 0.10f));
    body = sfFinalClampRect(body, viewportW, viewportH, auraMargin);
    return body;
}

static void sfFinalDrawSoftwareAura(SDL_Renderer *renderer,
                                    const SDL_Rect &body,
                                    bool sun)
{
    if (!renderer) return;
    const int cx = body.x + body.w / 2;
    const int cy = body.y + body.h / 2;
    const int radius = std::max(1, body.w / 2);

    SDL_BlendMode oldMode = SDL_BLENDMODE_NONE;
    SDL_GetRenderDrawBlendMode(renderer, &oldMode);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    if (sun) {
        // Procedural circles instead of scaled copies of the PNG. This avoids
        // the dark/black alpha fringe that linear filtering produced around
        // the previous aura while keeping the same luminous idea.
        sfRmFilledCircle(renderer, cx, cy, static_cast<int>(radius * 1.18f),
                         255, 88, 12, 12);
        sfRmFilledCircle(renderer, cx, cy, static_cast<int>(radius * 1.11f),
                         255, 135, 25, 20);
        sfRmFilledCircle(renderer, cx, cy, static_cast<int>(radius * 1.055f),
                         255, 205, 85, 30);
    } else {
        sfRmFilledCircle(renderer, cx, cy, static_cast<int>(radius * 1.14f),
                         25, 120, 255, 10);
        sfRmFilledCircle(renderer, cx, cy, static_cast<int>(radius * 1.08f),
                         45, 185, 255, 18);
        sfRmFilledCircle(renderer, cx, cy, static_cast<int>(radius * 1.035f),
                         120, 225, 255, 26);
    }

    SDL_SetRenderDrawBlendMode(renderer, oldMode);
}

static bool sfFinalIsGear1(const SDL_Rect &rect)
{
    if (!rouage1 || !rouage2) return false;
    const float cx = rect.x + rect.w * 0.5f;
    const float cy = rect.y + rect.h * 0.5f;
    const float d1 = (cx - rouage1->x) * (cx - rouage1->x) +
                     (cy - rouage1->y) * (cy - rouage1->y);
    const float d2 = (cx - rouage2->x) * (cx - rouage2->x) +
                     (cy - rouage2->y) * (cy - rouage2->y);
    return d1 <= d2;
}

#ifdef SDL_RenderCopy
#undef SDL_RenderCopy
#endif

static int SpaceFortressFinal_RenderCopy(SDL_Renderer *renderer,
                                          SDL_Texture *texture,
                                          const SDL_Rect *srcRect,
                                          const SDL_Rect *dstRect)
{
    if (!renderer || !texture || !dstRect)
        return SDL_RenderCopy(renderer, texture, srcRect, dstRect);

    // Replace the old texture-copy aura for scenic bodies with a clean
    // procedural halo and keep the whole circular body visible.
    if (sfUiScreen == SF_UI_GAME &&
        (texture == SpaceFortressSunTexture ||
         texture == SpaceFortressPlanetTexture)) {
        const bool sun = texture == SpaceFortressSunTexture;
        SDL_Rect body = sfFinalRoundBody(*dstRect, sun);
        sfFinalDrawSoftwareAura(renderer, body, sun);
        return SDL_RenderCopy(renderer, texture, srcRect, &body);
    }

    // Small gear icons: gear 1 belongs to the warm/orange player, gear 2 to
    // blue. Both are clamped so their teeth never disappear at an edge.
    if (sfUiScreen == SF_UI_GAME && texture == SpaceFortressGearBlueTexture) {
        SDL_Rect gearRect = sfFinalClampRect(*dstRect, WIDTH, HEIGHT, 4);
        SDL_Texture *gearTexture = texture;
        if (sfFinalIsGear1(gearRect)) {
            if (!sfFinalGearOrangeTexture) {
                sfFinalGearOrangeTexture = SpaceFortress_IMG_LoadTexture(
                    renderer,
                    "resources/assets/pict/remaster/gear_orange.png");
                if (sfFinalGearOrangeTexture)
                    SDL_SetTextureBlendMode(sfFinalGearOrangeTexture,
                                            SDL_BLENDMODE_BLEND);
            }
            if (sfFinalGearOrangeTexture) gearTexture = sfFinalGearOrangeTexture;
        }
        return SDL_RenderCopy(renderer, gearTexture, srcRect, &gearRect);
    }

    // Let the established visual restore layer handle everything else.
    return SpaceFortressVisualRestore_RenderCopy(renderer, texture,
                                                  srcRect, dstRect);
}

#ifdef SDL_RenderCopyEx
#undef SDL_RenderCopyEx
#endif

static int SpaceFortressFinal_RenderCopyEx(SDL_Renderer *renderer,
                                            SDL_Texture *texture,
                                            const SDL_Rect *srcRect,
                                            const SDL_Rect *dstRect,
                                            const double angle,
                                            const SDL_Point *center,
                                            const SDL_RendererFlip flip)
{
    if (!renderer || !texture || !dstRect)
        return SDL_RenderCopyEx(renderer, texture, srcRect, dstRect,
                                angle, center, flip);

    // The two large ring fortresses are round modern sprites rendered with
    // SDL_RenderCopyEx by the old engine. Clamp only their visual rectangle;
    // gameplay coordinates and collision geometry remain untouched.
    if (sfUiScreen == SF_UI_GAME &&
        (texture == SpaceFortressPlayerOrangeTexture ||
         texture == SpaceFortressPlayerBlueTexture)) {
        SDL_Rect body = sfFinalClampRect(*dstRect, WIDTH, HEIGHT, 8);
        return SDL_RenderCopyEx(renderer, texture, srcRect, &body,
                                angle, center, flip);
    }

    return SDL_RenderCopyEx(renderer, texture, srcRect, dstRect,
                            angle, center, flip);
}

#define SDL_RenderCopy SpaceFortressFinal_RenderCopy
#define SDL_RenderCopyEx SpaceFortressFinal_RenderCopyEx

#endif // __ANDROID__

#endif // SPACEFORTRESS_REMASTER_FRAME_FIX_HPP
