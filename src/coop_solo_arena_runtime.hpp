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
#include <coop_solo_charge_audio.hpp>
#include <coop_solo_world_bridge.hpp>
#include <coop_solo_world_draw.hpp>
#include "coop_solo_bonus_draw.hpp"

// Keep a build-time guarantee that Android's historical HOME/HELP overlay
// cannot hijack the experimental window's final presentation. The macro is
// intentionally restored in th2.h AFTER this header for historical modes.
#ifdef SDL_RenderPresent
#error "SOLO COOP diagnostic must use raw SDL_RenderPresent, not the classic UI router"
#endif

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
    SfCoopSoloBonusRenderer bonusRenderer(renderer);
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
    sfCoopDisableSoloTurretsAndBonuses(); // every new diagnostic process
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
    SDL_Log("SOLO_014_BONUS_VISUALS arena %.0fx%.0f activeMask=%d boss=%d",
            sfArenaW,sfArenaH,int(sfCoop.pilots.activeMask),sfCoop.bossPresent);

    bool audioInit=false,mixerOpen=false;
    Mix_Chunk *shotCue=nullptr,*hitCue=nullptr,*altExplosionCue=nullptr;
    Mix_Chunk *slowExplosionCue=nullptr,*fastExplosionCue=nullptr;
    std::vector<Uint8> slowExplosionStorage,fastExplosionStorage;
    SfAsteroidExplosionBank asteroidExplosionBank{};
    SfCoopSoloChargeCueGate chargeCueGate;
    if(SDL_InitSubSystem(SDL_INIT_AUDIO)==0){
        audioInit=true;Mix_Init(MIX_INIT_OGG);
        if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048)==0){
            mixerOpen=true;
            Mix_AllocateChannels(12);
            shotCue=Mix_LoadWAV("resources/assets/sounds/laser2.ogg");
            hitCue=Mix_LoadWAV("resources/assets/sounds/explo1.ogg");
            altExplosionCue=Mix_LoadWAV("resources/assets/sounds/explo2.ogg");
            slowExplosionCue=sfCreateRateShiftedChunk(hitCue,.97f,slowExplosionStorage);
            fastExplosionCue=sfCreateRateShiftedChunk(altExplosionCue?altExplosionCue:hitCue,1.03f,fastExplosionStorage);
            asteroidExplosionBank.variants={
                slowExplosionCue?slowExplosionCue:hitCue,
                hitCue,
                altExplosionCue?altExplosionCue:hitCue,
                fastExplosionCue?fastExplosionCue:(altExplosionCue?altExplosionCue:hitCue)};
            // The SAME kinetic audio implementation as historical COOP:
            // charge at .20 s, READY at 2 s, quiet hold, then release/cancel.
            sfKineticAudioEnsure();
            sfKineticAudioReset();
        }
    }

    // WORLD TEST is opt-in. Existing canonical one-pilot arena remains the
    // default, including BOSS TEST and the exact same audio/weapon loops.
    sfsolo::CoopSoloWorldBridge world;
    bool worldActive=false;
    SDL_FingerID moveFinger=-1,aimFinger=-1;
    float moveScreenX=width*.5f,moveScreenY=height*.5f;
    float aimScreenX=width*.5f,aimScreenY=height*.4f;
    bool mouseAimHeld=false,running=true,smokeVisible=true,bonusTestOpen=false;
    Uint64 previous=SDL_GetPerformanceCounter(),lastLog=SDL_GetTicks64();
    const double frequency=double(SDL_GetPerformanceFrequency());
    double bonusFrameTotalMs=0,bonusFrameMaxMs=0;unsigned bonusFrameCount=0;
    auto fromFinger=[&](float tx,float ty)->tupl{
        if(!worldActive)return tupl(tx,ty);
        const auto point=world.toWorld({tx,ty});
        return tupl(point.x,point.y);
    };
    auto aimAt=[&](float tx,float ty){
        aimScreenX=tx;aimScreenY=ty;
        const auto point=fromFinger(tx,ty);
        sfCoop.soloAimTarget=point;
        sfCoop.soloKineticAxes.aimAt(1,point.x-Spritej2->x,point.y-Spritej2->y);
    };
    auto setWorldMode=[&](bool enabled){
        if(enabled==worldActive)return;
        if(enabled){
            sfsolo::MapOptions options;
            options.world=1;options.stage=1;options.difficulty=1;
            options.width=54;options.height=160;
            if(!world.create(options,{0,0,width,height},sfArenaW)){
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"SOLO_COOP_WORLD_MAP_INIT_FAILED");
                return;
            }
        }
        sfCoop.shots.clear();sfCoop.beams.clear();sfCoop.waves.clear();
        sfFixResetAsteroidField();sfFieldRemainder=0;
        sfCoop.controls[1].down=false;sfCoop.controls[1].finger=-1;
        sfCoop.controls[1].velocity.set(0,0);
        sfKineticSurgeCancel(1);sfKineticAudioCancel(1);
        chargeCueGate.reset();aimFinger=moveFinger=-1;mouseAimHeld=false;
        sfCoop.phase=SfCoopPhase::Combat;
        sfCoop.bossPresent=false;sfCoop.health=sfCoopProfile().health;
        sfCoop.soloWorldMode=enabled;
        sfCoop.soloBonus.reset();sfCoop.soloBonusPending.clear();sfCoop.soloBonusImpacts.clear();
        sfCoop.soloBonusEjections.clear();
        sfCoop.soloLootRetry=sfCoop.soloCollectUntil=0;
        sfCoopDisableSoloTurretsAndBonuses(); // both WORLD and ARENA transitions
        sfCoop.soloWorldContext=enabled?&world:nullptr;
        sfCoop.soloWorldWidth=enabled?world.worldWidth():0;
        sfCoop.soloWorldHeight=enabled?world.worldHeight():0;
        sfCoop.soloWorldResolve=enabled?
            +[](void *context,tupl from,tupl desired,float radius)->tupl{
                const auto *maze=static_cast<sfsolo::CoopSoloWorldBridge*>(context);
                const auto actual=maze->moveWithoutTunnelling(
                    {from.x,from.y},{desired.x,desired.y},radius);
                return tupl(actual.x,actual.y);
            }:nullptr;
        sfCoop.soloWorldCanOccupy=enabled?
            +[](void *context,tupl at,float radius)->bool{
                return static_cast<sfsolo::CoopSoloWorldBridge*>(context)->
                    canOccupy({at.x,at.y},radius);
            }:nullptr;
        sfCoop.soloWorldFirstImpact=enabled?
            +[](void *context,tupl from,tupl to,float radius)->float{
                return static_cast<sfsolo::CoopSoloWorldBridge*>(context)->
                    firstWallImpact({from.x,from.y},{to.x,to.y},radius);
            }:nullptr;
        sfCoop.soloWorldPreparePickups=enabled?
            +[](void *context,tupl ship,float clearance){
                auto *maze=static_cast<sfsolo::CoopSoloWorldBridge*>(context);
                maze->pickupBatchCells=maze->reachablePickupCells({ship.x,ship.y},clearance);
            }:nullptr;
        sfCoop.soloWorldPlacePickup=enabled?
            +[](void *context,tupl,tupl desired,float clearance,tupl &result)->bool{
                const auto *maze=static_cast<sfsolo::CoopSoloWorldBridge*>(context);
                std::vector<sfsolo::CoopWorldPoint> occupied,eligible;
                float maxRadius=std::max(10.0f,sfArenaW*.023f);
                for(const auto &p:sfCoop.soloBonus.pickups){occupied.push_back({p.x,p.y});maxRadius=std::max(maxRadius,p.radius);}
                // Landing cells must be reachable by the pilot AND visible from
                // the boss so the BONUS-013 ejection never cuts through ROCK.
                const sfsolo::CoopWorldPoint boss{sfCoop.position.x,sfCoop.position.y};
                for(const auto &cell:maze->pickupBatchCells)
                    if(maze->firstWallImpact(boss,cell,clearance)>=.999f)eligible.push_back(cell);
                sfsolo::CoopWorldPoint at;
                const bool found=maze->selectPickup(eligible,{desired.x,desired.y},occupied,maxRadius*2+3,at);
                if(found)result=tupl(at.x,at.y);
                return found;
            }:nullptr;
        worldActive=enabled;
        const auto start=enabled?world.start():
            sfsolo::CoopWorldPoint{sfArenaW*.5f,sfArenaH*.72f};
        Spritej2->x=start.x;Spritej2->y=start.y;
        sfFixResetSpriteHistory(Spritej2);Spritej2->startup();
        sfObserved[1]={};
        if(enabled)world.centerOn(start);
        sfCoop.controls[1].target=tupl(start.x,start.y);
        sfCoop.soloAimTarget=tupl(start.x,start.y-120.0f);
        sfCoop.soloKineticAxes.aimAt(1,0,-1);
        SDL_Log("SOLO_COOP_WORLD_MODE enabled=%d ship=(%.1f,%.1f) map=%dx%d combatWidth=%.1f",
            int(enabled),start.x,start.y,enabled?world.map.width:0,
            enabled?world.map.height:0,sfArenaW);
    };
    auto beginBoss=[&](){
        sfCoopDisableSoloTurretsAndBonuses(); // boss toggle cannot revive extras
        // In WORLD this is an explicit, stationary boss TEST. Full maze boss
        // navigation/AI is a later phase; historical arena boss stays exact.
        if(sfCoop.bossPresent){
            sfCoop.bossPresent=false;sfCoop.soloBonus.clearBoss();
            sfCoop.shots.clear();sfCoop.beams.clear();sfCoop.waves.clear();
            sfCoop.soloBonusPending.clear();sfCoop.soloBonusEjections.clear();
        }else{
            tupl position(sfArenaW*.5f,sfArenaH*.25f);
            if(worldActive){
                const auto *ship=Spritej2;
                bool located=false;
                for(int ring=0;ring<5 && !located;++ring){
                    for(int direction=0;direction<16 && !located;++direction){
                        const float angle=(direction/16.0f)*2.0f*float(PI);
                        const float distance=sfArenaW*(.40f+.16f*ring);
                        const tupl candidate(ship->x+std::cos(angle)*distance,
                                             ship->y+std::sin(angle)*distance);
                        if(world.canOccupy({candidate.x,candidate.y},sfCoopBossRadius())){
                            position=candidate;located=true;
                        }
                    }
                }
                if(!located){SDL_Log("SOLO_WORLD_BOSS_NO_CLEAR_SPACE");return;}
            }
            sfCoop.bossPresent=true;
            sfCoop.health=sfCoopProfile().health;
            sfCoop.position=position;
            sfCoop.bonusTimer=worldActive?.01f:sfCoop.bonusTimer;
            sfCoop.attack=2;sfCoop.volley=0;sfCoop.phaseNumber=0;
            sfCoop.motion.valid=false;sfCoop.bossThreat={};
            sfCoopSoloBonusBeginEncounter();
        }
        sfCoopDisableSoloTurretsAndBonuses();
        SDL_Log("SOLO_COOP_BOSS_TOGGLE on=%d soloTurrets=0 bonusCore=012",sfCoop.bossPresent);
    };
    auto chargeRelease=[&](){
        const float held=sfKineticSurges[1].heldSeconds;
        const bool fire=sfKineticSurges[1].held&&held<.20f;
        const bool purge=sfKineticSurgeRelease(1);
        chargeCueGate.reset();
        if(purge){
            sfKineticPurgeAsteroids(1); // real historical radial discharge
            if(mixerOpen)sfKineticAudioRelease(1);
        }else if(mixerOpen)sfKineticAudioCancel(1);
        if(fire)sfCoopFire(1);
    };
    auto bonusTestButtonRect=[&](int index)->SDL_Rect{
        const int col=index%3,row=index/3;
        const int gap=std::max(4,int(width*.008f));
        const int w=(width-gap*4)/3;
        const int h=std::max(36,int(height*.055f));
        return {gap+col*(w+gap),int(height*.145f)+row*(h+gap),w,h};
    };
    auto triggerBonusTest=[&](int index){
        if(!sfCoop.bossPresent || sfCoop.health<=0){
            SDL_Log("SOLO_BONUS_013 TEST_BONUS ignored: BOSS TEST required");return;
        }
        if(index>=0 && index<8)sfCoopSoloBonusQueueTest(sfsolo::bonus::Kind(index));
        else if(index==8)sfCoopSoloBonusQueueAllTests();
    };
    // CI-only real-world smoke: opt-in ONLY when the test runner requests it.
    // The ordinary phone launch ALWAYS opens in the previously validated arena.
    if(SDL_getenv("SF_COOP_SOLO_TEST_WORLD"))setWorldMode(true);
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
                sfCampaignSuspend();chargeCueGate.reset();
                moveFinger=aimFinger=-1;mouseAimHeld=false;
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
                if(event.key.keysym.sym==SDLK_m)setWorldMode(!worldActive);
                if(event.key.keysym.sym==SDLK_SPACE&&!mouseAimHeld){
                    mouseAimHeld=true;sfKineticSurgePress(1);
                    chargeCueGate.reset(); // no sound during the first 0.20 s
                }
            }
            if(event.type==SDL_KEYUP&&event.key.keysym.sym==SDLK_SPACE&&mouseAimHeld){
                mouseAimHeld=false;chargeRelease();
            }
            if(event.type==SDL_FINGERDOWN||event.type==SDL_FINGERMOTION||event.type==SDL_FINGERUP){
                // SDL touch coords are normalized 0..1. The WORLD mode
                // converts both steering and targeting using its live camera.
                const float tx=event.tfinger.x*sfArenaW;
                const float ty=event.tfinger.y*sfArenaH;
                const SDL_FingerID id=event.tfinger.fingerId;
                if(event.type==SDL_FINGERDOWN){
                    // Fixed diagnostic clickboxes never pause or reset gameplay.
                    if(ty<sfArenaH*.13f && tx<sfArenaW*.33f){
                        bonusTestOpen=!bonusTestOpen;continue;
                    }
                    if(bonusTestOpen){
                        bool handled=false;
                        for(int test=0;test<9;++test){
                            const auto rect=bonusTestButtonRect(test);
                            if(tx>=rect.x&&tx<rect.x+rect.w&&ty>=rect.y&&ty<rect.y+rect.h){
                                triggerBonusTest(test);handled=true;break;
                            }
                        }
                        if(handled)continue;
                    }
                    if(ty<sfArenaH*.13f && tx>sfArenaW*.35f&&tx<sfArenaW*.65f){
                        setWorldMode(!worldActive);continue;
                    }
                    if(ty<sfArenaH*.13f && tx>sfArenaW*.68f){
                        beginBoss();
                        continue;
                    }
                    if(moveFinger<0){
                        moveFinger=id;
                        moveScreenX=tx;moveScreenY=ty;
                        auto &c=sfCoop.controls[1];
                        c.finger=id;c.down=true;c.target=fromFinger(tx,ty);
                    }else if(aimFinger<0&&id!=moveFinger){
                        aimFinger=id;aimAt(tx,ty);sfKineticSurgePress(1);
                        chargeCueGate.reset(); // no sound during the first 0.20 s
                    }
                }else if(event.type==SDL_FINGERMOTION){
                    if(id==moveFinger){
                        moveScreenX=tx;moveScreenY=ty;
                        sfCoop.controls[1].target=fromFinger(tx,ty);
                    }
                    if(id==aimFinger)aimAt(tx,ty);
                }else{
                    if(id==moveFinger){
                        moveFinger=-1;sfCoop.controls[1].down=false;
                        sfCoop.controls[1].finger=-1;
                        // WORLD camera must stop when the steering fingertip
                        // lifts. Prevent legacy touch inertia from pushing
                        // the pilot beyond the now-frozen viewport.
                        if(worldActive)sfCoop.controls[1].velocity.set(0,0);
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
            for(int i=0;i<substeps;++i){
                const float subDt=dt/substeps;
                const float oldX=Spritej2->x,oldY=Spritej2->y;
                if(worldActive&&moveFinger>=0){
                    // One still fingertip = continuing exploration if it is
                    // outside the INNER circle. Exact target at finger caused
                    // the freeze regression once the ship arrived underneath.
                    const auto aim=world.heldFingerPilotTarget(
                        {moveScreenX,moveScreenY});
                    sfCoop.controls[1].target=tupl(aim.x,aim.y);
                }
                if(worldActive&&aimFinger>=0)aimAt(aimScreenX,aimScreenY);
                sfCoopTick(subDt); // ORIGINAL physics in world coords, not a copy
                if(worldActive&&moveFinger>=0)
                    world.panIfShipActuallyMoved({Spritej2->x,Spritej2->y},
                        {Spritej2->x-oldX,Spritej2->y-oldY},subDt,
                        sfsolo::CoopWorldPoint{moveScreenX,moveScreenY});
                // Start the true kinetic charge sound only when the actual
                // COOP field becomes visible and vulnerable (0.20 s).
                if(chargeCueGate.startWhenVisible(sfKineticSurges[1])){
                    if(mixerOpen)sfKineticAudioStartCharge(1);
                    SDL_Log("SOLO_COOP_CHARGE_SOUND_START held=%.3f",
                        sfKineticSurges[1].heldSeconds);
                }
            }
        }
        SDL_SetRenderDrawColor(renderer,4,9,23,255);SDL_RenderClear(renderer);
        if(worldActive){
            // GPU-safe SCREEN projection, not a 54x160-cell viewport.
            // Existing COOP simulation and historic actor assets are reused,
            // no world position is ever overwritten just to draw an image.
            sfDrawCampaignSpace(renderer,sfCoop.encounter,sfCoop.time,width,height);
            world.draw(renderer);
            sfCoopSoloWorldActors(renderer,world);
        }else sfCoopDrawArena(renderer,width,height);
        bonusRenderer.draw(worldActive?&world:nullptr,width,height);
        if(worldActive && smokeFrames>0 && smokeVisible){
            // Deterministic REAL SDL2 pixel readback, not just a geometry
            // assertion. Protect Android from losing the ship again because
            // of oversized viewport dimensions or an absent texture.
            SDL_Rect viewport{};
            SDL_RenderGetViewport(renderer,&viewport);
            const auto shipOnScreen=world.toScreen({Spritej2->x,Spritej2->y});
            SDL_Rect pixel{int(std::lround(shipOnScreen.x)),
                           int(std::lround(shipOnScreen.y)),1,1};
            Uint32 rgba=0;
            SDL_PixelFormat *format=SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
            Uint8 r=0,g=0,b=0,a=0;
            const bool sampled=format &&
                pixel.x>=0 && pixel.x<width &&
                pixel.y>=0 && pixel.y<height &&
                SDL_RenderReadPixels(renderer,&pixel,
                    SDL_PIXELFORMAT_RGBA8888,&rgba,4)==0;
            if(sampled)SDL_GetRGBA(rgba,format,&r,&g,&b,&a);
            smokeVisible=sampled && viewport.x==0 && viewport.y==0 &&
                viewport.w==width && viewport.h==height &&
                g>=150 && b>=160;
            if(!smokeVisible)
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                    "SOLO_COOP_WORLD_SHIP_NOT_VISIBLE screen=(%d,%d) rgba=(%d,%d,%d,%d) viewport=(%d,%d,%d,%d)",
                    pixel.x,pixel.y,int(r),int(g),int(b),int(a),
                    viewport.x,viewport.y,viewport.w,viewport.h);
            SDL_FreeFormat(format);
        }
        const float energy=sfKineticEnergyFraction(Spritej2->nrj);
        const int margin=std::max(8,width/30),barW=std::max(100,int(width*.43f));
        // SOLO draws its own bottom HUD after the common campaign HUD. Apply
        // the Android safe inset HERE as well; otherwise Samsung navigation
        // still overlays these two diagnostic bars.
        const int soloHudLift=sfPlayerSafeBottomInset(height);
        const int lifeY=height-58-soloHudLift,energyY=height-40-soloHudLift;
        const int titleY=height-85-soloHudLift;
        SDL_Rect lifeBg{margin,lifeY,barW,12};
        SDL_SetRenderDrawColor(renderer,42,26,57,255);SDL_RenderFillRect(renderer,&lifeBg);
        SDL_Rect life{margin,lifeY,int(barW*std::clamp(Spritej2->pv/1000.0f,0.0f,1.0f)),12};
        SDL_SetRenderDrawColor(renderer,105,229,150,255);SDL_RenderFillRect(renderer,&life);
        SDL_Rect energyBg{margin,energyY,barW,10};
        SDL_SetRenderDrawColor(renderer,32,31,69,255);SDL_RenderFillRect(renderer,&energyBg);
        SDL_Rect energyBar{margin,energyY,int(barW*energy),10};
        SDL_SetRenderDrawColor(renderer,169,135,242,255);SDL_RenderFillRect(renderer,&energyBar);
        sfCoopText(renderer,margin,titleY,
                   worldActive?"MONDE / LABYRINTHE SOLO-014":"COOP CORE SOLO-014",
                   width-2*margin,2);
        sfCoopButton(renderer,{int(width*.02f),8,int(width*.30f),int(height*.08f)},
                     bonusTestOpen?"TEST BONUS ON":"TEST BONUS OFF");
        sfCoopButton(renderer,{int(width*.355f),8,int(width*.29f),int(height*.08f)},
                     worldActive?"MONDE ON":"MONDE TEST");
        sfCoopButton(renderer,{int(width*.68f),8,int(width*.30f),int(height*.08f)},
                     worldActive?(sfCoop.bossPresent?"BOSS OFF":"BOSS TEST"):
                         sfCoop.bossPresent?"BOSS OFF":"BOSS TEST");
        if(bonusTestOpen){
            using sfsolo::bonus::Kind;
            const char *labels[]={"PV","ENERGIE","FURTIF","MISSILES","PRISON","GLACE","LAVE","METEORE","TOUS X8"};
            for(int test=0;test<9;++test)sfCoopButton(renderer,bonusTestButtonRect(test),labels[test]);
            if(!sfCoop.bossPresent)
                sfCoopCentered(renderer,width,int(height*.335f),"BOSS TEST REQUIS",2,{255,180,120,235});
        }
        if(sfCoop.phase==SfCoopPhase::Defeat)
            sfCoopCentered(renderer,width,int(height*.50f),"TEST TERMINE - RETOUR",3);
        // Raw SDL call: show ONLY this arena's frame. The legacy macro would
        // apply sfFixRequestedScreen=HOME and repaint the Classic mode selector.
        SDL_RenderPresent(renderer);
        if(mixerOpen){
            sfCoopPlaySounds(nullptr,shotCue,nullptr,hitCue,hitCue,altExplosionCue,&asteroidExplosionBank);
        }
        const double frameWorkMs=(SDL_GetPerformanceCounter()-counter)*1000.0/frequency;
        bonusFrameTotalMs+=frameWorkMs;bonusFrameMaxMs=std::max(bonusFrameMaxMs,frameWorkMs);++bonusFrameCount;
        if(SDL_GetTicks64()-lastLog>3000){
            SDL_Log("SOLO_BONUS_012 frameWorkMeanMs=%.3f frameWorkMaxMs=%.3f pickups=%zu pending=%zu fx=%zu charges=%u",
                bonusFrameTotalMs/std::max(1u,bonusFrameCount),bonusFrameMaxMs,
                sfCoop.soloBonus.pickups.size(),sfCoop.soloBonusPending.size(),
                sfCoop.soloBonusImpacts.size(),sfCoop.soloBonus.charges);
            bonusFrameTotalMs=bonusFrameMaxMs=0;bonusFrameCount=0;
            lastLog=SDL_GetTicks64();
            const auto pilotScreen=worldActive?
                world.toScreen({Spritej2->x,Spritej2->y}):
                sfsolo::CoopWorldPoint{Spritej2->x,Spritej2->y};
            const float touchLag=moveFinger>=0?
                std::hypot(pilotScreen.x-moveScreenX,pilotScreen.y-moveScreenY):0.0f;
            SDL_Log("SOLO_COOP_NATIVE_HEARTBEAT ship=(%.1f,%.1f) screen=(%.1f,%.1f) finger=(%.1f,%.1f) lag=%.1f pv=%.1f nrj=%.2f shots=%zu rocks=%zu boss=%d world=%d camera=(%.1f,%.1f)",
                Spritej2->x,Spritej2->y,pilotScreen.x,pilotScreen.y,
                moveScreenX,moveScreenY,touchLag,Spritej2->pv,Spritej2->nrj,
                sfCoop.shots.size(),sa1.size(),sfCoop.bossPresent,int(worldActive),
                worldActive?world.cameraLeft:0,worldActive?world.cameraTop:0);
        }
        SDL_Delay(1);
        if(smokeFrames>0&&--smokeFrames==0)running=false;
    }
    sfCampaignSuspend();chargeCueGate.reset();
    sfCoop.soloWorldMode=false;sfCoop.soloWorldContext=nullptr;
    sfCoop.soloWorldResolve=nullptr;
    sfCoop.soloWorldCanOccupy=nullptr;
    sfCoop.soloWorldFirstImpact=nullptr;sfCoop.soloWorldPlacePickup=nullptr;sfCoop.soloWorldPreparePickups=nullptr;
    sfCoop.soloBonus.reset();sfCoop.soloBonusPending.clear();sfCoop.soloBonusImpacts.clear();
    sfCoop.soloBonusEjections.clear();
    sfCoop.soloWorldWidth=sfCoop.soloWorldHeight=0;
    sfFixResetAsteroidField();sfCoop.shots.clear();sfCoop.beams.clear();sfCoop.waves.clear();
    if(mixerOpen)Mix_HaltChannel(-1);
    for(Mix_Chunk *p:{slowExplosionCue,fastExplosionCue,shotCue,hitCue,altExplosionCue})if(p)Mix_FreeChunk(p);
    if(mixerOpen){
        // This Android :coopsolo process owns the historical kinetic chunks.
        // Halt before free; no pointers survive its audio device.
        sfKineticAudioReset();
        for(Mix_Chunk *p:{sfKineticAudio.charge,sfKineticAudio.ready,
                          sfKineticAudio.release})if(p)Mix_FreeChunk(p);
        sfKineticAudio=SfKineticAudioState{};
        Mix_CloseAudio();
    }
    if(audioInit){Mix_Quit();SDL_QuitSubSystem(SDL_INIT_AUDIO);}
    bonusRenderer.release();
    sfCampaignForgetRenderer(renderer);
    SDL_DestroyRenderer(renderer);SDL_DestroyWindow(window);
    IMG_Quit();SDL_Quit();
    return smokeVisible?0:7;
}
