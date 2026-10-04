#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <cassert>
#include <cmath>
#include <cstdio>

#define __ANDROID__ 1
#include "t.hpp"
#include "th2.h"

bool setgui=true,setia=false,sdlstarted=true;
float k0=1,tw=780,th=1680;
int tirj1=0,tirj2=0,incra1=0;
std::list<sprite*> sa1;
std::list<sprite*> entitiesj1,entitiesj2,burnsj1,burnsj2;
std::list<parts*> particules,particulesr;
std::list<eexpl*> explos;
bool tirjz=false,tirj1z=false,tirj2z=false;
sprite *Spritej1=new sprite,*Spritej2=new sprite;
sprite *loosej1=new sprite,*loosej2=new sprite;
sprite *rouage1=new sprite,*rouage2=new sprite,*Suiveur=new sprite;
enti *iago=new enti,*iago1=new enti,*iacalc=new enti,*iatake=new enti;

static void expectNear(float actual,float expected)
{
    assert(std::fabs(actual-expected)<0.0001f);
}

int main()
{
    sprite orangeShot,blueShot;
    orangeShot.shotOwner=0;
    blueShot.shotOwner=1;

    sfBossDangerIndex=2; // ROCK N ROLL = x10 internally.
    sfActiveMode=SF_DUEL_AI;
    assert(sfClassicAiHostileShot(&orangeShot,Spritej2));
    assert(!sfClassicAiHostileShot(&blueShot,Spritej1));
    assert(!sfClassicAiHostileShot(&orangeShot,Spritej1));
    expectNear(sfClassicIncomingNonKinetic(&orangeShot,Spritej2,7.0f),70.0f);
    expectNear(sfClassicIncomingNonKinetic(&blueShot,Spritej1,7.0f),7.0f);

    sfActiveMode=SF_DUEL_LOCAL;
    expectNear(sfClassicIncomingNonKinetic(&orangeShot,Spritej2,7.0f),7.0f);
    expectNear(sfClassicIncomingNonKinetic(&blueShot,Spritej1,7.0f),7.0f);

    sfActiveMode=SF_COOP_LOCAL;
    expectNear(sfClassicIncomingNonKinetic(&orangeShot,Spritej2,7.0f),7.0f);
    sfActiveMode=SF_COOP_AI;
    expectNear(sfClassicIncomingNonKinetic(&orangeShot,Spritej2,7.0f),7.0f);

    // This helper is explicitly non-kinetic: zero/negative raw input stays a
    // pure scalar transform and asteroid/kinetic callers never route through it.
    sfActiveMode=SF_DUEL_AI;
    sfBossDangerIndex=8;
    expectNear(sfClassicIncomingNonKinetic(&orangeShot,Spritej2,0.0f),0.0f);

    std::puts("PASS: classic AI danger scales only owner-0 hostile shots into the blue human in AI duel");
    return 0;
}
