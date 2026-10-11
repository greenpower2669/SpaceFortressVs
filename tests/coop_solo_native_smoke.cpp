// Run the ACTUAL historical COOP-powered SOLO diagnostic loop on SDL's
// headless software renderer. This is intentionally NOT a FakeShip.
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <cassert>
#define __ANDROID__ 1
#define main sfNativeSmokeLegacyMain
#include "main_android_compat.cpp"
#undef main
// Verify the macro is back after the diagnostic definition: CLASSIC/COOP
// continue using their exact historical presentation wrapper unchanged.
#ifndef SDL_RenderPresent
#error "Historical SDL presentation hook not restored after SOLO diagnostic"
#endif

int main(){
    SDL_setenv("SDL_VIDEODRIVER","dummy",1);
    SDL_setenv("SDL_AUDIODRIVER","dummy",1);
    SDL_setenv("SDL_RENDER_DRIVER","software",1);
    SDL_setenv("SF_COOP_SOLO_TEST_FRAMES","7",1);
    assert(runCoopSoloArena()==0);
    assert(!sfCoop.soloWorldMode);
    SDL_setenv("SF_COOP_SOLO_TEST_WORLD","1",1);
    assert(runCoopSoloArena()==0); // real CANONICAL COOP loop in generated maze
    assert(!sfCoop.soloWorldMode); // no dangling world session on exit
    assert(sfCoop.soloWorldContext==nullptr);
    SDL_unsetenv("SF_COOP_SOLO_TEST_WORLD");
    // The experimental runner must neither invoke the historical campaign
    // victory saving function nor change the Android release mode.
    return 0;
}
