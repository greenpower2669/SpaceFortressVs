#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <cassert>
#include <cstdio>
#include <vector>

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

static void tapTarget(SDL_FingerID id=31)
{
    const SDL_Rect r=sfTutorialTargetRect(780,1680);
    auto e=finger(SDL_FINGERDOWN,id,float(r.x+r.w/2)/780.0f,float(r.y+r.h/2)/1680.0f);
    assert(sfTutorialHandleEvent(&e));
}

static void testAllSequence()
{
    sfTutorialStart(SfTutorialModule::All);
    assert(sfTutorialState.active && sfTutorialState.step==SfTutorialStep::Move);
    const SfTutorialStep expected[]={SfTutorialStep::Move,SfTutorialStep::Fire,SfTutorialStep::Hud,
        SfTutorialStep::Energy,SfTutorialStep::HullShield,SfTutorialStep::Dust,SfTutorialStep::Kinetic,
        SfTutorialStep::SurgeHold,SfTutorialStep::SurgeReady,SfTutorialStep::SurgeRelease,
        SfTutorialStep::Danger,SfTutorialStep::Complete};
    std::vector<SfTutorialStep> seen{sfTutorialState.step};

    auto down=finger(SDL_FINGERDOWN,1,.20f,.70f);assert(sfTutorialHandleEvent(&down));
    auto move=finger(SDL_FINGERMOTION,1,.52f,.70f);assert(sfTutorialHandleEvent(&move));
    seen.push_back(sfTutorialState.step);assert(sfTutorialState.step==SfTutorialStep::Fire);
    auto fire=finger(SDL_FINGERDOWN,2,.65f,.40f);assert(sfTutorialHandleEvent(&fire));
    seen.push_back(sfTutorialState.step);assert(sfTutorialState.step==SfTutorialStep::Hud);
    for(auto step:{SfTutorialStep::Energy,SfTutorialStep::HullShield,SfTutorialStep::Dust,SfTutorialStep::Kinetic,SfTutorialStep::SurgeHold}) {
        tapTarget();seen.push_back(sfTutorialState.step);assert(sfTutorialState.step==step);
    }
    auto hold=finger(SDL_FINGERDOWN,9,.60f,.60f);assert(sfTutorialHandleEvent(&hold));
    sfTutorialTick(1.99f);assert(sfTutorialState.step==SfTutorialStep::SurgeHold);
    sfTutorialTick(.02f);seen.push_back(sfTutorialState.step);assert(sfTutorialState.step==SfTutorialStep::SurgeReady);
    auto release=finger(SDL_FINGERUP,9,.60f,.60f);assert(sfTutorialHandleEvent(&release));
    seen.push_back(sfTutorialState.step);assert(sfTutorialState.step==SfTutorialStep::SurgeRelease);
    sfTutorialTick(.36f);seen.push_back(sfTutorialState.step);assert(sfTutorialState.step==SfTutorialStep::Danger);
    tapTarget();seen.push_back(sfTutorialState.step);assert(sfTutorialState.step==SfTutorialStep::Complete);

    assert(seen.size()==sizeof(expected)/sizeof(expected[0]));
    for(size_t i=0;i<seen.size();++i) assert(seen[i]==expected[i]);
    sfTutorialExit();assert(!sfTutorialState.active);
}

static void testIndividualAndIsolation()
{
    Spritej1->pv=731;Spritej1->nrj=19;Spritej1->x=123;Spritej2->pv=642;Spritej2->nrj=27;
    sfCoop.encounter=17;sfCoop.health=543;sfCoop.time=44;
    sfCampaignSave.selected=29;sfCampaignSave.cleared=12;
    const auto fameSize=sfCampaignSave.fame.size();
    sfBossDangerIndex=6;
    auto *rock=new sprite;rock->tacticalId=999;sa1.push_back(rock);
    const size_t rockCount=sa1.size(),shot1=entitiesj1.size(),shot2=entitiesj2.size();

    sfTutorialStart(SfTutorialModule::Energy);
    assert(sfTutorialState.step==SfTutorialStep::Energy);
    tapTarget(41);assert(sfTutorialState.step==SfTutorialStep::Complete);
    sfTutorialTick(4);sfTutorialExit();

    assert(Spritej1->pv==731 && Spritej1->nrj==19 && Spritej1->x==123);
    assert(Spritej2->pv==642 && Spritej2->nrj==27);
    assert(sfCoop.encounter==17 && sfCoop.health==543 && sfCoop.time==44);
    assert(sfCampaignSave.selected==29 && sfCampaignSave.cleared==12 && sfCampaignSave.fame.size()==fameSize);
    assert(sfBossDangerIndex==6 && sa1.size()==rockCount && entitiesj1.size()==shot1 && entitiesj2.size()==shot2);
    delete rock;sa1.pop_back();
}

static void testLayouts()
{
    for(auto size:{SDL_Point{360,780},SDL_Point{780,360}}) {
        for(SDL_Rect r:{sfTutorialTargetRect(size.x,size.y),sfTutorialExitRect(size.x,size.y)}) {
            assert(r.x>=0 && r.y>=0 && r.x+r.w<=size.x && r.y+r.h<=size.y);
            assert(r.w>=44 && r.h>=44);
        }
        auto *surface=SDL_CreateRGBSurfaceWithFormat(0,size.x,size.y,32,SDL_PIXELFORMAT_RGBA32);
        auto *renderer=SDL_CreateSoftwareRenderer(surface);assert(renderer);
        sfTutorialStart(SfTutorialModule::All);sfTutorialDraw(renderer);sfTutorialExit();
        SDL_DestroyRenderer(renderer);SDL_FreeSurface(surface);
    }
}

int main()
{
    testAllSequence();testIndividualAndIsolation();testLayouts();
    std::puts("PASS: tutorial follows 11 guided modules, remains isolated and fits portrait/landscape");
    return 0;
}
