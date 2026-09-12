#ifndef SPACEFORTRESS_REMASTER_LIFECYCLE_FIX_HPP
#define SPACEFORTRESS_REMASTER_LIFECYCLE_FIX_HPP

#ifdef __ANDROID__
// SDL_DestroyRenderer also destroys every texture owned by that renderer.
// Clear the remaster references before the legacy foreground path rebuilds it;
// pointer addresses can be reused for entirely different textures afterwards.
static void SpaceFortressFinal_DestroyRenderer(SDL_Renderer *renderer)
{
    if (renderer) {
        SpaceFortress_ForgetRendererTextures(renderer);
        if (sfRmMuzzleRenderer == renderer) {
            sfRmMuzzleStrip = NULL;
            sfRmMuzzleRenderer = NULL;
            sfRmMuzzleW = sfRmMuzzleH = 0;
            sfRmFlashJ1Start = sfRmFlashJ2Start = 0;
        }
        if (sfFinalGearOrangeRenderer == renderer) {
            sfFinalGearOrangeTexture = NULL;
            sfFinalGearOrangeRenderer = NULL;
        }
    }
    SDL_DestroyRenderer(renderer);
}

#define SDL_DestroyRenderer SpaceFortressFinal_DestroyRenderer
#endif
#endif
