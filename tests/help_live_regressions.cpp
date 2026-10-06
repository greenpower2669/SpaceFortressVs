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

static SDL_Event finger(Uint32 type,SDL_FingerID id,float x,float y)
{
    SDL_Event e{};e.tfinger.type=type;e.tfinger.fingerId=id;e.tfinger.x=x;e.tfinger.y=y;return e;
}

static SDL_Event backEvent()
{
    SDL_Event e{};e.type=SDL_KEYDOWN;e.key.keysym.sym=SDLK_AC_BACK;return e;
}

static SDL_Event helpTap(SDL_FingerID id)
{
    const int width=int(tw),height=int(th);
    const SDL_Rect r=sfHelpGameButtonRect(width,height);
    return finger(SDL_FINGERDOWN,id,float(r.x+r.w/2)/width,float(r.y+r.h/2)/height);
}

static void clearRocks()
{
    for(auto *r:sa1) delete r;
    sa1.clear();incra1=0;
}

static void resetUi(int mode)
{
    sfSelectedMode=sfActiveMode=mode;
    sfFixRequestedIa.store(sfModeHasAi(mode));
    sfFixRequestedScreen.store(SF_UI_GAME);
    sfFixLaunchPending.store(false);
    sfFixConsumedFingers.clear();
    sfUiScreen=SF_UI_GAME;setgui=false;
    sfHelpState=SfHelpState{};
}

static void testDuelHelpPreservesLiveMatch()
{
    resetUi(SF_DUEL_AI);clearRocks();
    Spritej1->pv=713;Spritej1->nrj=17;Spritej1->x=123;Spritej1->y=234;
    Spritej2->pv=641;Spritej2->nrj=23;Spritej2->x=612;Spritej2->y=1310;
    auto *rock=new sprite;rock->tacticalId=4242;rock->x=333;rock->y=777;sa1.push_back(rock);incra1=1;
    const auto beforeRock=sa1.front();

    auto down=helpTap(71);sfFixHandleEvent(&down);
    assert(down.type==SDL_USEREVENT);
    assert(sfFixRequestedScreen.load()==SF_UI_HELP);
    assert(sfHelpState.open && sfHelpState.fromLiveGame && sfHelpState.returnScreen==SF_UI_GAME);
    sfFixApplyUiRequests();
    assert(sfUiScreen==SF_UI_HELP && setgui);

    auto motion=finger(SDL_FINGERMOTION,71,.5f,.5f);sfFixHandleEvent(&motion);assert(motion.type==SDL_USEREVENT);
    auto up=finger(SDL_FINGERUP,71,.5f,.5f);sfFixHandleEvent(&up);assert(up.type==SDL_USEREVENT);

    auto back=backEvent();sfFixHandleEvent(&back);sfFixApplyUiRequests();
    assert(back.type==SDL_USEREVENT);
    assert(sfUiScreen==SF_UI_GAME && !setgui);
    assert(Spritej1->pv==713 && Spritej1->nrj==17 && Spritej1->x==123 && Spritej1->y==234);
    assert(Spritej2->pv==641 && Spritej2->nrj==23 && Spritej2->x==612 && Spritej2->y==1310);
    assert(sa1.size()==1 && sa1.front()==beforeRock && sa1.front()->tacticalId==4242);
    assert(!sfHelpState.open);
}

static void testCoopHelpPausesAndResumesCombat()
{
    resetUi(SF_COOP_LOCAL);clearRocks();
    sfCoop.phase=SfCoopPhase::Combat;sfCoop.time=12.5f;sfCoop.phaseTime=3.25f;sfCoop.health=456.0f;
    Spritej1->pv=801;Spritej1->nrj=14;Spritej2->pv=755;Spritej2->nrj=29;
    const float beforeTime=sfCoop.time,beforePhaseTime=sfCoop.phaseTime,beforeHealth=sfCoop.health;

    auto down=helpTap(81);sfFixHandleEvent(&down);sfFixApplyUiRequests();
    assert(down.type==SDL_USEREVENT);
    assert(sfUiScreen==SF_UI_HELP && setgui && sfHelpState.fromLiveGame);
    assert(sfCoop.phase==SfCoopPhase::Paused);

    auto back=backEvent();sfFixHandleEvent(&back);sfFixApplyUiRequests();
    assert(sfUiScreen==SF_UI_GAME && !setgui);
    assert(sfCoop.phase==SfCoopPhase::Combat);
    assert(sfCoop.time==beforeTime && sfCoop.phaseTime==beforePhaseTime && sfCoop.health==beforeHealth);
    assert(Spritej1->pv==801 && Spritej1->nrj==14 && Spritej2->pv==755 && Spritej2->nrj==29);
}

int main()
{
    testDuelHelpPreservesLiveMatch();
    testCoopHelpPausesAndResumesCombat();
    clearRocks();
    std::puts("PASS: live help consumes its touch and resumes duel/coop without resetting state");
    return 0;
}
