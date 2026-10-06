// Regression contract for asteroid white-dust yields and red kinetic contacts.
// Exercises the real generated Android/classic field, not a substitute simulator.
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#define __ANDROID__ 1
#define main sfLegacyMain
#include "main_android_compat.cpp"
#undef main

static void clearDustField()
{
    for (auto *rock : sa1) delete rock;
    sa1.clear(); incra1=0;
}

static sprite *dustRock(float x,float y,float size,float vx=0,float vy=0)
{
    auto *rock=new sprite;
    rock->setv(x,y,size,size,0,0,1);
    rock->w=rock->h=rock->sw=rock->sh=size;
    rock->vx=vx; rock->vy=vy; rock->pv=1; rock->timer=0; rock->name="a1";
    rock->startup(); sa1.push_back(rock); return rock;
}

static void resetDustScenario()
{
    sfTacticsReset(); clearDustField();
    W=WIDTH=sfArenaW=780; H=HEIGHT=sfArenaH=1680; k0=1;
    sfActiveMode=sfSelectedMode=SF_DUEL_LOCAL; sfUiScreen=SF_UI_GAME; setgui=false;
    loosej1->pv=loosej2->pv=0;
    Spritej1->setxywh(390,840,100,100); Spritej1->w=Spritej1->h=Spritej1->sw=Spritej1->sh=100;
    Spritej1->startup(); Spritej1->pv=1000; Spritej1->nrj=0;
    Spritej2->setxywh(100,1500,100,100); Spritej2->w=Spritej2->h=Spritej2->sw=Spritej2->sh=100;
    Spritej2->startup(); Spritej2->pv=0; Spritej2->nrj=0;
    sfKineticDustFlashes.clear();
}

static SfKineticVector meanWhiteVelocity()
{
    SfKineticVector mean{}; int count=0;
    for (auto *dust:particules) if(dust && dust->pv>0) {
        mean.x+=dust->vx; mean.y+=dust->vy; ++count;
    }
    if(count) {mean.x/=count;mean.y/=count;}
    return mean;
}

static void yieldContract()
{
    assert(std::abs(sfKineticWhiteDustYieldFraction(SfKineticDustCause::SurgePurge)-1.0f)<1e-6f);
    assert(std::abs(sfKineticWhiteDustYieldFraction(SfKineticDustCause::AsteroidCollision)-.1f)<1e-6f);
    assert(std::abs(sfKineticWhiteDustYieldFraction(SfKineticDustCause::KineticField)-.1f)<1e-6f);
    const int small=sfKineticWhiteDustParticleCount(40.0f*40.0f,1680.0f,SfKineticDustCause::SurgePurge);
    const int large=sfKineticWhiteDustParticleCount(120.0f*120.0f,1680.0f,SfKineticDustCause::SurgePurge);
    assert(small>0 && large>small);
    assert(sfKineticWhiteDustParticleCount(120.0f*120.0f,1680.0f,SfKineticDustCause::AsteroidCollision)<large);

    const auto yellow=sfKineticRedDustFlashColor(0.0f,1.0f);
    const auto orange=sfKineticRedDustFlashColor(.5f,1.0f);
    const auto red=sfKineticRedDustFlashColor(.98f,1.0f);
    assert(yellow.r>=240 && yellow.g>=180 && yellow.b<=120);
    assert(orange.r>=240 && orange.g<yellow.g && orange.g>red.g);
    assert(red.r>=240 && red.g<80 && red.b<80);

    const auto consumed=sfKineticRespondRedDust(120,-20,1,0,1,.10f,780);
    const auto survivor=sfKineticRespondRedDust(120,-20,1,0,1,.95f,780);
    assert(consumed.consumed && consumed.flashStrength>0);
    assert(!survivor.consumed && survivor.motion.deflected && survivor.motion.vibrated);
}

static void purgeYield()
{
    resetDustScenario();
    auto *small=dustRock(Spritej1->x,Spritej1->y,40,3,-1);
    const int expectedSmall=sfKineticWhiteDustParticleCount(small->w*small->h,sfArenaH,SfKineticDustCause::SurgePurge);
    assert(sfKineticPurgeAsteroids(0)==1 && small->pv==0);
    assert(int(particules.size())==expectedSmall);
    for(auto *dust:particules) assert(std::abs(dust->vx)<1e-5f && std::abs(dust->vy)<1e-5f);
    const auto once=particules.size();
    assert(sfKineticPurgeAsteroids(0)==0 && particules.size()==once);

    resetDustScenario();
    auto *large=dustRock(Spritej1->x,Spritej1->y,120,-2,2);
    const int expectedLarge=sfKineticWhiteDustParticleCount(large->w*large->h,sfArenaH,SfKineticDustCause::SurgePurge);
    assert(sfKineticPurgeAsteroids(0)==1);
    assert(int(particules.size())==expectedLarge && expectedLarge>expectedSmall);
    for(auto *dust:particules) assert(std::abs(dust->vx)<1e-5f && std::abs(dust->vy)<1e-5f);
    std::puts("PASS: purge emits one proportional 100% white cloud with quasi-zero velocity");
}

static void collisionYield()
{
    resetDustScenario(); Spritej1->pv=Spritej2->pv=0;
    for(int i=0;i<6;++i) dustRock(40+i*110,100,20);
    dustRock(390,840,360,0,0);
    const float incidentX=3.0f,incidentY=-1.0f;
    const float destroyedArea=180.0f*180.0f;
    dustRock(390,840,180,incidentX,incidentY);
    const int expected=sfKineticWhiteDustParticleCount(destroyedArea,sfArenaH,SfKineticDustCause::AsteroidCollision);
    sfLegacyFieldStep(nullptr);
    assert(int(particules.size())==expected);
    const auto mean=meanWhiteVelocity();
    assert(mean.x*incidentX+mean.y*incidentY>0);
    assert(std::sqrt(mean.x*mean.x+mean.y*mean.y)>1.0f);
    std::puts("PASS: asteroid collision emits exactly one 10% cloud following the destroyed asteroid vector");
}

static void normalFieldYield()
{
    resetDustScenario();
    auto *rock=dustRock(430,840,10,2,1);
    const int expected=sfKineticWhiteDustParticleCount(rock->w*rock->h,sfArenaH,SfKineticDustCause::KineticField);
    SfKineticSolution solved{};
    solved.relativeSpeed=180; solved.rawDamage=100; solved.residualDamage=20; solved.dissipationFraction=.8f;
    assert(sfKineticFragmentRock(rock,Spritej1,0,SfKineticLayer::Inner,solved));
    assert(rock->pv==0 && int(particules.size())==expected);
    const auto mean=meanWhiteVelocity();
    assert(mean.x*2+mean.y>0);
    const auto once=particules.size();
    assert(!sfKineticFragmentRock(rock,Spritej1,0,SfKineticLayer::Inner,solved));
    assert(particules.size()==once);
    std::puts("PASS: normal kinetic destruction emits one projected 10% white cloud without duplication");
}

static void redDustContact()
{
    resetDustScenario();
    const float hp=Spritej1->pv,heat=Spritej1->nrj;
    for(int i=0;i<20;++i) {
        auto *dust=new parts(Spritej1->x+12,Spritej1->y);
        dust->vx=120; dust->vy=-20; dust->pv=600; particulesr.push_back(dust);
    }
    auto *white=new parts(Spritej1->x+12,Spritej1->y);
    white->vx=10;white->vy=5;white->pv=600;particules.push_back(white);
    sfKineticTriggerWave(0,1.0f,1.0f);
    sfKineticUpdateEffects(1.0f/60.0f);
    int alive=0,deflected=0;
    for(auto *dust:particulesr) if(dust->pv>0) {
        ++alive; if(std::abs(dust->vx-120)>1e-4f || std::abs(dust->vy+20)>1e-4f) ++deflected;
    }
    assert(alive>0 && alive<20 && deflected==alive);
    assert(!sfKineticDustFlashes.empty());
    assert(std::abs(white->vx-10)<1e-6f && std::abs(white->vy-5)<1e-6f);
    assert(Spritej1->pv==hp && Spritej1->nrj==heat);
    std::puts("PASS: red dust contact consumes a majority, flashes locally and deflects only survivors without gameplay effects");
}

int main()
{
    yieldContract();
    purgeYield();
    collisionYield();
    normalFieldYield();
    redDustContact();
    resetDustScenario(); clearDustField();
    return 0;
}
