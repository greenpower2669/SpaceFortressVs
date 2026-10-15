// SDLActivity dispatch. Preserve the historical default entry point.
// The SOLO prototype uses an explicitly requested, debug-only Android launcher.
#include "solo_prototype.hpp"
#include "solo_android_diagnostics.hpp"
#include <cstring>
#include <cstdlib>
#include <string>

extern int SDL_main();
extern "C" int runCoopSoloArena();
extern "C" int runSoloGame();
#include "boss_danger.hpp"

extern "C" int SpaceFortressMain(int argc, char **argv)
{
    for(int i=0;argv && i<argc;++i)
        if(argv[i] && std::strncmp(argv[i],"--boss-danger=",14)==0)
            sfBossDangerIndex=std::clamp(std::atoi(argv[i]+14),0,8);
    for (int i=0; argv && i<argc; ++i) {
        if(argv[i] && std::strcmp(argv[i],"--spacefortress-solo-game")==0)
            return runSoloGame();
        if(argv[i] && std::strcmp(argv[i],"--spacefortress-coop-solo-arena")==0){
            sfDebugNativeLog("coopsolo","SDL_ENTRY_REAL_COOP_ARENA");
            const int result=runCoopSoloArena();
            sfDebugNativeLog("coopsolo",
                (std::string("SDL_COOP_ARENA_RETURN_")+std::to_string(result)).c_str());
            return result;
        }
        if (argv[i] && std::strcmp(argv[i], "--spacefortress-solo-prototype")==0) {
            sfDebugNativeLog("solo","SDL_ENTRY_DISPATCH_SOLO");
            const int result=runSoloPrototype();
            sfDebugNativeLog("solo",
                (std::string("SDL_SOLO_RETURN_")+std::to_string(result)).c_str());
            return result;
        }
    }
    // All existing Android launches remain byte-for-byte equivalent in intent:
    // without the explicit debug SOLO switch, start the historical game.
    sfDebugNativeLog("classic","SDL_ENTRY_DISPATCH_CLASSIC");
    const int result=SDL_main();
    sfDebugNativeLog("classic",
        (std::string("SDL_CLASSIC_RETURN_")+std::to_string(result)).c_str());
    return result;
}
