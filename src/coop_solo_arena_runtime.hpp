#pragma once
// EXPERIMENTAL SOLO-COOP-SOCLE-001. Included ONLY in historical th2.h / TU.
// This does NOT duplicate FabLib, projectiles, asteroid/dust physics or render.
// Controls are a small SDL2 facade around real sfCoopTick/sfCoopDrawArena.
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <algorithm>
#include <cmath>
#include <string>

extern "C" int runCoopSoloArena(){
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS,"0");
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS)!=0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"SOLO_COOP SDL_Init failed: %s",SDL_GetError());
        return 1;
    }
    SDL_Window *window=SDL_CreateWindow("SpaceFortress SOLO - authentic COOP",
        SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,540,960,
        SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
    if(!window){SDL_Quit();return 2;}
    SDL_Renderer *renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    if(!renderer)renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_SOFTWARE);
    if(!renderer){SDL_DestroyWindow(window);SDL_Quit();return 3;}
    IMG_Init(IMG_INIT_PNG);
    int width=0,height=0;SDL_GetRendererOutputSize(renderer,&width,&height);
    if(width<=0||height<=0){width=540;height=960;}
    ::setw(float(width));seth(float(height));setwh();setd1();
    WIDTH=width;HEIGHT=height;
    sfArenaW=float(width);sfArenaH=float(height);k0=1.0f;
    setgui=false;setia=false;
    sfSelectedMode=sfActiveMode=SF_COOP_LOCAL;
    sfUiScreen=SF_UI_GAME;

    // New session lives in the Android :coopsolo process. We explicitly do
    // NOT load sfCampaignSave: boss victories in this test never save campaign.
    sfCoop=SfCoopState{};
    sfCoop.pilots=SfCoopPilotRoster::single(1);
    sfCoop.bossPresent=false;
    sfCoop.soloDiagnostic=true;
    sfCoop.soloFreeAim=true;
    sfCoop.phase=SfCoopPhase::Combat;
    sfCoop.encounter=0;
    sfCoop.health=sfCoopProfile().health;
    sfCoop.position=tupl(sfArenaW*.5f,sfArenaH*.24f);
    sfCoop.asteroidSpawnTimer=.4f;
    sfKineticResetSurges();sfFixResetAsteroidField();sfFieldRemainder=0;
    const float size=std::min(sfArenaW,sfArenaH)*.15f;
    for(int owner=0;owner<2;++owner){
        auto *ship=sfCoopShip(owner);
        ship->setxywh(sfArenaW*(owner==1?.50f:.30f),
                      sfArenaH*(owner==1?.72f:.20f),size,size);
        ship->sw=ship->sh=size;
        ship->boundedBreathing=true;ship->animated=false;
        ship->pv=owner==1?1000.0f:0.0f;
        ship->nrj=0;
        sfFixResetSpriteHistory(ship);
        sfCoop.controls[owner].target=tupl(ship->x,ship->y);
    }
    sfObserved={};
    sfCoop.soloAimTarget=tupl(Spritej2->x,Spritej2->y-120);
    SDL_Log("SOLO_COOP_REAL_ENGINE arena %.0fx%.0f activeMask=%d boss=%d",
            sfArenaW,sfArenaH,int(sfCoop.pilots.activeMask),sfCoop.bossPresent);

    bool audioInit=false,mixerOpen=false;
    Mix_Chunk *shotCue=nullptr,*hitCue=nullptr,*chargeCue=nullptr,*releaseCue=nullptr;
    if(SDL_InitSubSystem(SDL_INIT_AUDIO)==0){
        audioInit=true;Mix_Init(MIX_INIT_OGG);
        if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048)==0){
            mixerOpen=true;
            Mix_AllocateChannels(12);
            shotCue=Mix_LoadWAV("resources/assets/sounds/laser2.ogg");
            hitCue=Mix_LoadWAV("resources/assets/sounds/explo1.ogg");
            chargeCue=Mix_LoadWAV("resources/assets/sounds/kinetic_charge.wav");
            releaseCue=Mix_LoadWAV("resources/assets/sounds/kinetic_release.wav");
        }
    }

    SDL_FingerID moveFinger=-1,aimFinger=-1;
    bool mouseAimHeld=false,running=true;
    Uint64 previous=SDL_GetPerformanceCounter(),lastLog=SDL_GetTicks64();
    const double frequency=double(SDL_GetPerformanceFrequency());
    auto aimAt=[&](float tx,float ty){
        sfCoop.soloAimTarget=tupl(tx,ty);
    };
    auto beginBoss=[&](){
        // Same boss profile/pattern, not a simplified tile enemy.
        if(sfCoop.bossPresent){
            sfCoop.bossPresent=false;
            sfCoop.shots.clear();sfCoop.beams.clear();sfCoop.waves.clear();
        }else{
            sfCoop.bossPresent=true;
            sfCoop.health=sfCoopProfile().health;
            sfCoop.position=tupl(sfArenaW*.5f,sfArenaH*.25f);
            sfCoop.attack=2;sfCoop.volley=0;sfCoop.phaseNumber=0;
            sfCoop.motion.valid=false;sfCoop.bossThreat={};
        }
        SDL_Log("SOLO_COOP_BOSS_TOGGLE on=%d",sfCoop.bossPresent);
    };
    auto chargeRelease=[&](){
        const float held=sfKineticSurges[1].heldSeconds;
        const bool fire=sfKineticSurges[1].held&&held<.20f;
        const bool purge=sfKineticSurgeRelease(1);
        if(fire)sfCoopFire(1);
        if(purge){
            sfKineticPurgeAsteroids(1); // real historical radial discharge
            if(mixerOpen&&releaseCue)Mix_PlayChannel(-1,releaseCue,0);
        }
    };
    // CI-only deterministic lifetime. No timer or in-game automation in user
    // builds unless a test runner explicitly sets this environment variable.
    int smokeFrames=-1;
    if(const char *frames=SDL_getenv("SF_COOP_SOLO_TEST_FRAMES"))
        smokeFrames=std::clamp(std::atoi(frames),1,60);
    while(running){
        SDL_Event event{};
        while(SDL_PollEvent(&event)){
            if(event.type==SDL_QUIT){running=false;break;}
            if(event.type==SDL_APP_DIDENTERBACKGROUND){
                sfCampaignSuspend();moveFinger=aimFinger=-1;mouseAimHeld=false;
                continue;
            }
            if(event.type==SDL_APP_DIDENTERFOREGROUND){
                if(sfCoop.phase==SfCoopPhase::Paused)sfCoop.phase=SfCoopPhase::Combat;
                previous=SDL_GetPerformanceCounter();
                continue;
            }
            if(event.type==SDL_KEYDOWN){
                if(event.key.keysym.sym==SDLK_ESCAPE||event.key.keysym.sym==SDLK_AC_BACK)
                    running=false;
                if(event.key.keysym.sym==SDLK_b)beginBoss();
                if(event.key.keysym.sym==SDLK_SPACE&&!mouseAimHeld){
                    mouseAimHeld=true;sfKineticSurgePress(1);
                    if(mixerOpen&&chargeCue)Mix_PlayChannel(-1,chargeCue,0);
                }
            }
            if(event.type==SDL_KEYUP&&event.key.keysym.sym==SDLK_SPACE&&mouseAimHeld){
                mouseAimHeld=false;chargeRelease();
            }
            if(event.type==SDL_FINGERDOWN||event.type==SDL_FINGERMOTION||event.type==SDL_FINGERUP){
                // SDL touch coords are normalized 0..1; this arena's camera
                // is IDENTITY. The next milestone will inject world projection.
                const float tx=event.tfinger.x*sfArenaW;
                const float ty=event.tfinger.y*sfArenaH;
                const SDL_FingerID id=event.tfinger.fingerId;
                if(event.type==SDL_FINGERDOWN){
                    // Top-right, distinct from the movement zone: real boss demo.
                    if(tx>sfArenaW*.68f&&ty<sfArenaH*.13f){
                        beginBoss();continue;
                    }
                    if(moveFinger<0){
                        moveFinger=id;
                        auto &c=sfCoop.controls[1];
                        c.finger=id;c.down=true;c.target=tupl(tx,ty);
                    }else if(aimFinger<0&&id!=moveFinger){
                        aimFinger=id;aimAt(tx,ty);sfKineticSurgePress(1);
                        if(mixerOpen&&chargeCue)Mix_PlayChannel(-1,chargeCue,0);
                    }
                }else if(event.type==SDL_FINGERMOTION){
                    if(id==moveFinger)sfCoop.controls[1].target=tupl(tx,ty);
                    if(id==aimFinger)aimAt(tx,ty);
                }else{
                    if(id==moveFinger){
                        moveFinger=-1;sfCoop.controls[1].down=false;
                        sfCoop.controls[1].finger=-1;
                    }
                    if(id==aimFinger){
                        aimAt(tx,ty);aimFinger=-1;chargeRelease();
                    }
                }
            }
            if(event.type==SDL_MOUSEMOTION){
                if(!mouseAimHeld)aimAt(float(event.motion.x),float(event.motion.y));
            }
        }
        const Uint64 counter=SDL_GetPerformanceCounter();
        const float dt=std::clamp(float((counter-previous)/frequency),0.0f,.05f);
        previous=counter;
        if(sfCoop.phase==SfCoopPhase::Combat){
            // EXACT historic split: ceil(dt*90), never advance in renderer.
            const int substeps=std::max(1,int(std::ceil(dt*90.0f)));
            for(int i=0;i<substeps;++i)sfCoopTick(dt/substeps);
        }
        SDL_SetRenderDrawColor(renderer,4,9,23,255);SDL_RenderClear(renderer);
        sfCoopDrawArena(renderer,width,height);
        const float energy=sfKineticEnergyFraction(Spritej2->nrj);
        const int margin=std::max(8,width/30),barW=std::max(100,int(width*.43f));
        SDL_Rect lifeBg{margin,height-58,barW,12};
        SDL_SetRenderDrawColor(renderer,42,26,57,255);SDL_RenderFillRect(renderer,&lifeBg);
        SDL_Rect life{margin,height-58,int(barW*std::clamp(Spritej2->pv/1000.0f,0.0f,1.0f)),12};
        SDL_SetRenderDrawColor(renderer,105,229,150,255);SDL_RenderFillRect(renderer,&life);
        SDL_Rect energyBg{margin,height-40,barW,10};
        SDL_SetRenderDrawColor(renderer,32,31,69,255);SDL_RenderFillRect(renderer,&energyBg);
        SDL_Rect energyBar{margin,height-40,int(barW*energy),10};
        SDL_SetRenderDrawColor(renderer,169,135,242,255);SDL_RenderFillRect(renderer,&energyBar);
        sfCoopText(renderer,margin,height-85,"COOP CORE - UN PILOTE",width-2*margin,2);
        sfCoopButton(renderer,{int(width*.67f),8,int(width*.30f),int(height*.08f)},
                     sfCoop.bossPresent?"BOSS OFF":"BOSS TEST");
        if(sfCoop.phase==SfCoopPhase::Defeat)
            sfCoopCentered(renderer,width,int(height*.50f),"TEST TERMINE - RETOUR",3);
        SDL_RenderPresent(renderer);
        if(mixerOpen){
            sfCoopPlaySounds(nullptr,shotCue,nullptr,hitCue,hitCue);
        }
        if(SDL_GetTicks64()-lastLog>3000){
            lastLog=SDL_GetTicks64();
            SDL_Log("SOLO_COOP_NATIVE_HEARTBEAT ship=(%.1f,%.1f) pv=%.1f nrj=%.2f shots=%zu rocks=%zu boss=%d",
                Spritej2->x,Spritej2->y,Spritej2->pv,Spritej2->nrj,
                sfCoop.shots.size(),sa1.size(),sfCoop.bossPresent);
        }
        SDL_Delay(1);
        if(smokeFrames>0&&--smokeFrames==0)running=false;
    }
    sfCampaignSuspend();
    sfFixResetAsteroidField();sfCoop.shots.clear();sfCoop.beams.clear();sfCoop.waves.clear();
    if(mixerOpen)Mix_HaltChannel(-1);
    for(Mix_Chunk *p:{shotCue,hitCue,chargeCue,releaseCue})if(p)Mix_FreeChunk(p);
    if(mixerOpen)Mix_CloseAudio();
    if(audioInit){Mix_Quit();SDL_QuitSubSystem(SDL_INIT_AUDIO);}
    sfCampaignForgetRenderer(renderer);
    SDL_DestroyRenderer(renderer);SDL_DestroyWindow(window);
    IMG_Quit();SDL_Quit();
    return 0;
}
