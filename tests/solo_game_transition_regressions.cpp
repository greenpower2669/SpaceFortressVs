#include "solo_lair_transition.hpp"
#include "solo_campaign_persistence.hpp"
#include <cassert>
#include <cstdio>
#include <fstream>
int main(){
    SDL_setenv("SDL_VIDEODRIVER","dummy",1);assert(SDL_Init(SDL_INIT_VIDEO)==0);
    auto *surface=SDL_CreateRGBSurfaceWithFormat(0,320,480,32,SDL_PIXELFORMAT_RGBA8888);
    auto *renderer=SDL_CreateSoftwareRenderer(surface);assert(renderer);
    sfsolo::LairTransition effect;
    SDL_Rect viewport{1,2,300,460},clip{3,4,250,400};
    SDL_RenderSetViewport(renderer,&viewport);SDL_RenderSetClipRect(renderer,&clip);
    SDL_RenderSetScale(renderer,.75f,.8f);
    SDL_Rect initialClip{},initialViewport{};
    SDL_RenderGetClipRect(renderer,&initialClip);SDL_RenderGetViewport(renderer,&initialViewport);
    assert(effect.beginCapture(renderer,320,480,160,200));
    SDL_SetRenderDrawColor(renderer,200,60,240,255);SDL_RenderClear(renderer);
    effect.endCapture();
    SDL_Rect after{};SDL_RenderGetViewport(renderer,&after);assert(after.x==1 && after.y==2);
    assert(SDL_RenderIsClipEnabled(renderer));SDL_RenderGetClipRect(renderer,&after);
    assert(after.x==initialClip.x && after.y==initialClip.y);
    float x,y;SDL_RenderGetScale(renderer,&x,&y);assert(x==.75f && y==.8f);
    SDL_RenderSetViewport(renderer,nullptr);SDL_RenderSetClipRect(renderer,nullptr);SDL_RenderSetScale(renderer,1,1);
    for(float t:{0.0f,.3f,.65f,.9f,1.0f})effect.draw(renderer,320,480,t);
    effect.release();effect.draw(renderer,320,480,.5f); // fallback after device/target loss
    assert(effect.beginCapture(renderer,320,480,170,220));effect.endCapture();effect.release();
    // High-density Android: a bounded target still captures the full logical
    // viewport, including actors near the bottom-right of the display.
    SDL_RenderSetLogicalSize(renderer,3200,4800);
    assert(effect.beginCapture(renderer,3200,4800,1600,2400));
    int captureW=0,captureH=0;SDL_QueryTexture(effect.snapshot,nullptr,nullptr,&captureW,&captureH);
    assert(captureH==2048 && captureW<=2048);
    SDL_Rect fullScreen{0,0,3200,4800};SDL_RenderSetViewport(renderer,&fullScreen);
    SDL_SetRenderDrawColor(renderer,0,0,0,255);SDL_RenderClear(renderer);
    SDL_Rect corner{3000,4600,200,200};SDL_SetRenderDrawColor(renderer,60,220,230,255);
    SDL_RenderFillRect(renderer,&corner);
    Uint32 pixel=0;SDL_Rect sample{captureW-10,captureH-10,1,1};
    assert(SDL_RenderReadPixels(renderer,&sample,SDL_PIXELFORMAT_RGBA8888,&pixel,4)==0);
    assert(pixel==0x3cdce6ff);
    effect.endCapture();effect.release();
    SDL_DestroyRenderer(renderer);SDL_FreeSurface(surface);SDL_Quit();
    sfsolo::CampaignController campaign;
    sfsolo::RunResult run;run.mapId="maze-015";run.mapVersion=3;run.health=500;run.seconds=120;
    run.enemiesAvailable=4;run.enemiesDefeated=3;
    char directory[]="/tmp/solo-game-save-XXXXXX";assert(mkdtemp(directory));
    const auto path=std::string(directory)+"/solo-progress-v1.save";
    assert(!sfsolo::finishAndSave(campaign,run,path,"Pilote","test",1000));
    run.bossDefeated=run.reachedFinish=true;
    assert(!sfsolo::finishAndSave(campaign,run,path+"/missing","Pilote","test",1000));
    assert(!campaign.progression.isCompleted(1,1));
    assert(sfsolo::finishAndSave(campaign,run,path,"Pilote","test",1000));
    sfsolo::CampaignController restored;assert(sfsolo::restoreCampaign(restored,path));
    assert(restored.progression.unlocked(1,2) && restored.progression.isCompleted(1,1));
    assert(campaign.localHall.best.size()==1 && !campaign.active);
    run.health=0;assert(!sfsolo::finishAndSave(campaign,run,path,"Pilote","dead",1000));
    std::remove(path.c_str());
    std::puts("PASS: GPU transition restores render state, fallback/reset, SOLO result save/restore and rollback");
}
