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
    assert(runSoloGame()==0); // true entry: maze directly, no diagnostic capability
    assert(sfCoop.soloGame && !sfCoop.soloDiagnostic);
    assert(sfCoop.soloWorldContext==nullptr);
    sfCoop.soloGame=false;
    // Samsung/navigation-bar protection is Android-only and must affect only
    // the lower SOLO player HUD, never the boss geometry.
    sfCoop.soloDiagnostic=true;
    const auto safeEnergy=sfPlayerEnergyRect(1,540,960);
    const auto safePv=sfPlayerPvRect(1,540,960);
    assert(sfPlayerSafeBottomInset(960)>0);
    assert(safeEnergy.y+safeEnergy.h<960);
    assert(safePv.y<safeEnergy.y);
    const auto bossBefore=sfBossLifeRect(false,540,960);
    sfCoop.soloDiagnostic=false;
    const auto bossAfter=sfBossLifeRect(false,540,960);
    assert(bossBefore.x==bossAfter.x && bossBefore.y==bossAfter.y &&
           bossBefore.w==bossAfter.w && bossBefore.h==bossAfter.h);
    // SDL2 has SDL_setenv, but not SDL_unsetenv. This CI smoke exits now;
    // leaving its test-only variable within the process is harmless.
    // The experimental runner must neither invoke the historical campaign
    // victory saving function nor change the Android release mode.
    return 0;
}
