#pragma once

// Keep the three help formats genuinely distinct without duplicating the help
// renderer: QUICK and DETAILED use the same in-game diagrams as static
// schematics, while ANIMATED receives the live SDL clock.
#include <SDL2/SDL.h>

static Uint64 SpaceFortressHelpAnimationTicks();

#define SDL_GetTicks64 SpaceFortressHelpAnimationTicks
#include <help_runtime.hpp>
#undef SDL_GetTicks64

static bool sfHelpAnimationsEnabled()
{
    return sfHelpState.format==SfHelpFormat::Animated;
}

static Uint64 SpaceFortressHelpAnimationTicks()
{
    return sfHelpAnimationsEnabled() ? SDL_GetTicks64() : 0;
}
