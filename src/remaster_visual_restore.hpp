#ifndef SPACEFORTRESS_REMASTER_VISUAL_RESTORE_HPP
#define SPACEFORTRESS_REMASTER_VISUAL_RESTORE_HPP

// Visual regression restore pass.
//
// Keep the historical rich galaxy renderer, but add a very small modern layer
// around scenic assets. The sun and planet keep their legacy movement while a
// software-rendered aura and size clamp make them clean on modern screens.

#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>

// Restore the real SDL_RenderClear for all historical calls compiled after this
// header. This lets fond4hlz.png remain the base field.
#ifdef SDL_RenderClear
#undef SDL_RenderClear
#endif

#ifdef __ANDROID__
static SDL_Rect sfVisualCenteredSquare(const SDL_Rect &src, int diameter)
{
    const int cx = src.x + src.w / 2;
    const int cy = src.y + src.h / 2;
    SDL_Rect out = {cx - diameter / 2, cy - diameter / 2, diameter, diameter};
    return out;
}

static SDL_Rect sfVisualExpanded(const SDL_Rect &src, float scale)
{
    const int diameter = std::max(1, static_cast<int>(src.w * scale));
    return sfVisualCenteredSquare(src, diameter);
}

static void sfVisualDrawAuraLayer(SDL_Renderer *renderer,
                                  SDL_Texture *texture,
                                  const SDL_Rect *srcRect,
                                  const SDL_Rect &body,
                                  float scale,
                                  Uint8 r, Uint8 g, Uint8 b, Uint8 alpha)
{
    SDL_Rect halo = sfVisualExpanded(body, scale);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureColorMod(texture, r, g, b);
    SDL_SetTextureAlphaMod(texture, alpha);
    SDL_RenderCopy(renderer, texture, srcRect, &halo);
}

// Intercept only the two scenic textures. Gameplay sprites, UI and collisions
// keep their historical renderer untouched.
static int SpaceFortressVisualRestore_RenderCopy(SDL_Renderer *renderer,
                                                  SDL_Texture *texture,
                                                  const SDL_Rect *srcRect,
                                                  const SDL_Rect *dstRect)
{
    if (!renderer || !texture || !dstRect || sfUiScreen != SF_UI_GAME ||
        (texture != SpaceFortressSunTexture &&
         texture != SpaceFortressPlanetTexture)) {
        return SDL_RenderCopy(renderer, texture, srcRect, dstRect);
    }

    int outputW = 0, outputH = 0;
    SDL_GetRendererOutputSize(renderer, &outputW, &outputH);
    if (outputW <= 0 || outputH <= 0)
        return SDL_RenderCopy(renderer, texture, srcRect, dstRect);

    const bool isSun = texture == SpaceFortressSunTexture;
    const double seconds = static_cast<double>(SDL_GetTicks64()) * 0.001;

    // The legacy destination rectangles were intentionally enormous on the old
    // phone. Keep their moving centre, but cap the diameter for today's screens.
    const float maxScreenFraction = isSun ? 0.58f : 0.76f;
    const int maxDiameter = std::max(32,
        static_cast<int>(outputW * maxScreenFraction));
    const int legacyDiameter = std::max(1, std::min(dstRect->w, dstRect->h));
    int diameter = std::min(legacyDiameter, maxDiameter);

    // Very gentle breathing: visible as life, never as a gameplay distraction.
    const double speed = isSun ? 1.10 : 0.48;
    const float amplitude = isSun ? 0.018f : 0.010f;
    const float pulse = 1.0f + amplitude *
        static_cast<float>(std::sin(seconds * speed));
    diameter = std::max(24, static_cast<int>(diameter * pulse));

    SDL_Rect body = sfVisualCenteredSquare(*dstRect, diameter);

    Uint8 oldR = 255, oldG = 255, oldB = 255, oldA = 255;
    SDL_BlendMode oldBlend = SDL_BLENDMODE_NONE;
    SDL_GetTextureColorMod(texture, &oldR, &oldG, &oldB);
    SDL_GetTextureAlphaMod(texture, &oldA);
    SDL_GetTextureBlendMode(texture, &oldBlend);

    if (isSun) {
        // Warm multi-pass bloom following the real alpha silhouette.
        sfVisualDrawAuraLayer(renderer, texture, srcRect, body,
                              1.20f, 255, 105, 20, 22);
        sfVisualDrawAuraLayer(renderer, texture, srcRect, body,
                              1.12f, 255, 150, 40, 34);
        sfVisualDrawAuraLayer(renderer, texture, srcRect, body,
                              1.055f, 255, 205, 105, 52);
    } else {
        // Cyan atmospheric aura, deliberately softer than the sun.
        sfVisualDrawAuraLayer(renderer, texture, srcRect, body,
                              1.17f, 35, 145, 255, 18);
        sfVisualDrawAuraLayer(renderer, texture, srcRect, body,
                              1.10f, 60, 195, 255, 30);
        sfVisualDrawAuraLayer(renderer, texture, srcRect, body,
                              1.045f, 125, 230, 255, 44);
    }

    SDL_SetTextureBlendMode(texture, oldBlend);
    SDL_SetTextureColorMod(texture, oldR, oldG, oldB);
    SDL_SetTextureAlphaMod(texture, oldA);

    return SDL_RenderCopy(renderer, texture, srcRect, &body);
}
#endif // __ANDROID__

// Keep shooting stars as a real runtime effect rather than pixels baked into a
// background image.
static void SpaceFortressVisualRestore_RenderPresent(SDL_Renderer *renderer)
{
    if (renderer && sfUiScreen == SF_UI_GAME) {
        int width = 0;
        int height = 0;
        SDL_GetRendererOutputSize(renderer, &width, &height);
        if (width > 0 && height > 0) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            sfRmDrawShootingStar(renderer, width, height);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
    }

    SpaceFortressRemaster_RenderPresent(renderer);
}

#ifdef SDL_RenderPresent
#undef SDL_RenderPresent
#endif
#define SDL_RenderPresent SpaceFortressVisualRestore_RenderPresent

#ifdef __ANDROID__
#ifdef SDL_RenderCopy
#undef SDL_RenderCopy
#endif
#define SDL_RenderCopy SpaceFortressVisualRestore_RenderCopy
#endif

#endif // SPACEFORTRESS_REMASTER_VISUAL_RESTORE_HPP
