// SDLActivity dispatch. Preserve the historical default entry point.
// The SOLO prototype uses an explicitly requested, debug-only Android launcher.
#include "solo_prototype.hpp"
#include <cstring>

extern int SDL_main();

extern "C" int SpaceFortressMain(int argc, char **argv)
{
    for (int i=0; argv && i<argc; ++i) {
        if (argv[i] && std::strcmp(argv[i], "--spacefortress-solo-prototype")==0)
            return runSoloPrototype();
    }
    // All existing Android launches remain byte-for-byte equivalent in intent:
    // without the explicit debug SOLO switch, start the historical game.
    return SDL_main();
}
