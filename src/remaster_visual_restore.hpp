#ifndef SPACEFORTRESS_REMASTER_VISUAL_RESTORE_HPP
#define SPACEFORTRESS_REMASTER_VISUAL_RESTORE_HPP

// Visual regression restore pass.
//
// remaster_runtime.hpp originally replaced SDL_RenderClear to paint a fully
// procedural dark background. The historical engine already has the better
// architecture for this game: a rich galaxy base plus separate moving scenic
// sprites. Disable only that RenderClear interception and keep the proven game
// renderer intact.

#include <SDL2/SDL.h>

// Restore the real SDL_RenderClear for all historical calls compiled after this
// header. This lets fond4hlz.png become the base field again.
#ifdef SDL_RenderClear
#undef SDL_RenderClear
#endif

// Keep shooting stars as a real runtime effect rather than pixels baked into a
// background image. Everything else (sun, planet, asteroids) is rendered and
// animated by the historical sprite machinery using remapped modern assets.
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

#endif // SPACEFORTRESS_REMASTER_VISUAL_RESTORE_HPP
