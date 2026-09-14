#pragma once
#include <set>
static SDL_Event sfCoopFinger(Uint32 type,SDL_FingerID id,float x,float y)
{
    SDL_Event e{};e.type=type;e.tfinger.type=type;e.tfinger.fingerId=id;e.tfinger.x=x;e.tfinger.y=y;return e;
}
static void setupCampaign(int boss=0,bool ai=false)
{
    setupTactics();sfSelectedMode=sfActiveMode=ai ? SF_COOP_AI : SF_COOP_LOCAL;
    sfFixRequestedIa.store(ai);setia=ai;sfCampaignSave.pending=false;
    sfCampaignSave.selected=boss;sfCampaignSave.cleared=std::max(sfCampaignSave.cleared,boss);
    sfCampaignStart();sfCoop.phase=SfCoopPhase::Combat;
}
static void testVelocityGhosts()
{
    for (int fps : {30,60,120}) for (int owner=0;owner<2;++owner) {
        setupTactics();sfActiveMode=sfSelectedMode=SF_DUEL_LOCAL;sfFrameDt=1.0f/fps;
        auto *shooter=sfCoopShip(owner);auto *target=sfCoopShip(1-owner);shooter->nrj=20;
        sfObserved[1-owner].velocity.set(owner==0 ? 100 : -100,0);
        const auto aim=sfShipGhost(1-owner).intercept(tupl(shooter->x,shooter->y),sfMainShotSpeed(shooter->nrj));
        assert((aim.x-target->x)*sfObserved[1-owner].velocity.vx>0);
        assert(sfFireMain(owner,&aim));auto *shot=(owner==0 ? entitiesj2 : entitiesj1).back();
        assert(shot->shotVelocityX*sfObserved[1-owner].velocity.vx>0);
        float nearest=10000;
        for (int frame=0;frame<fps*2 && shot->pv>0;++frame) {
            target->x+=sfObserved[1-owner].velocity.vx*sfFrameDt;sfAdvanceProjectile(shot);
            nearest=std::min(nearest,sfSegmentDistance(tupl(shot->shotFromX,shot->shotFromY),tupl(shot->x,shot->y),tupl(target->x,target->y)));
        }
        assert(nearest<16);
    }
    setupTactics();sfActiveMode=sfSelectedMode=SF_DUEL_AI;
    Spritej2->y=840;sfObserved[1].velocity.set(160,-90);sfThinkPilot();const auto right=sfPilot.goal;
    sfObserved[1].velocity.set(-160,90);sfThinkPilot();assert(right.x>sfPilot.goal.x && right.y<sfPilot.goal.y);
    assert(sa1.empty());sfActiveMode=sfSelectedMode=SF_DUEL_LOCAL;
    std::puts("PASS: moving ghost interception at 30/60/120fps, directed shots, predictive X/Y pursuit, no collidable ghost");
}
static void testCampaignPersistence(const std::string &directory)
{
    sfCampaignStoragePath=directory+"/campaign-v1.dat";sfCampaignLoaded=false;sfCampaignStorageBlocked=false;
    sfCampaignStorageError.clear();sfCampaignSave=SfCampaignSave{};sfLoadCampaign();assert(sfCampaignSave.fame.empty());
    setupCampaign();sfCoop.health=0;sfCoop.time=12;sfCoopWin();
    assert(sfCampaignSave.pending && sfCampaignSave.cleared==1 && sfCoop.pendingSaved);
    const auto id=sfCampaignSave.victory.id;
    sfCampaignSave=SfCampaignSave{};sfCampaignLoaded=false;sfLoadCampaign();sfCampaignStart();
    assert(sfCampaignSave.pending && sfCampaignSave.victory.id==id && sfCoop.phase==SfCoopPhase::Name);
    sfCoop.names={{"Fab","Yann","Equipe Eclipse"}};sfCoop.nameField=2;
    SDL_Event input{};input.type=SDL_TEXTINPUT;std::strcpy(input.text.text," é");sfFixHandleEvent(&input);
    assert(sfCoop.names[2]=="Equipe Eclipse é");
    SDL_Event back{};back.type=SDL_KEYDOWN;back.key.keysym.sym=SDLK_BACKSPACE;sfFixHandleEvent(&back);
    assert(sfCoop.names[2]=="Equipe Eclipse ");sfCoop.names[2]="Équipe Éclipse";
    assert(sfCoopSaveNames());assert(!sfCampaignSave.pending && sfCampaignSave.fame.size()==1);
    assert(!sfCoopSaveNames() && sfCampaignSave.fame.size()==1);
    sfCampaignSave=SfCampaignSave{};sfCampaignLoaded=false;sfLoadCampaign();
    assert(sfCampaignSave.fame.size()==1 && sfCampaignSave.fame[0].names[2]=="Équipe Éclipse");
    auto candidate=sfCampaignSave;candidate.cleared=2;assert(sfSaveCampaign(candidate));
    {std::ofstream out(sfCampaignStoragePath);out<<"damaged";}
    sfCampaignLoaded=false;sfLoadCampaign();assert(sfCampaignSave.fame.size()==1 && !sfCampaignStorageError.empty());
    assert(sfSaveCampaign(sfCampaignSave));
    sfCampaignStoragePath=directory+"/future.dat";
    {std::ofstream out(sfCampaignStoragePath);out<<"SPACEFORTRESS_CAMPAIGN 9\nfuture data\n";}
    sfCampaignLoaded=false;sfLoadCampaign();assert(sfCampaignStorageBlocked && !sfSaveCampaign(sfCampaignSave));
    {std::ifstream in(sfCampaignStoragePath);std::string bytes((std::istreambuf_iterator<char>(in)),{});assert(bytes.find("future data")!=std::string::npos);}
    sfCampaignStoragePath=directory+"/campaign-v1.dat";sfCampaignLoaded=false;sfCampaignStorageBlocked=false;sfLoadCampaign();
    const auto original=sfCampaignStoragePath;sfCampaignStoragePath=directory+"/missing/directory/file.dat";
    assert(!sfSaveCampaign(sfCampaignSave));sfCampaignStoragePath=original;
    assert(sfReadCampaignFile(original,candidate) && candidate.fame.size()==1);sfCampaignStorageError.clear();
    std::puts("PASS: UTF-8 input/backspace, pending victory recovery, unique awards, atomic hall, backup and failed-write preservation");
}
static void testCoopGameplay()
{
    sfCampaignSave=SfCampaignSave{};std::set<std::string> names;std::set<int> planets;
    float previousHealth=0,previousInterval=2;
    for (const auto &p : sfBossCatalog()) {
        assert(names.insert(p.name).second && planets.insert(p.planet).second);
        assert(p.health>previousHealth && p.interval<previousInterval && p.planet<50);
        previousHealth=p.health;previousInterval=p.interval;setupCampaign(p.index);sfCoopPattern(p.family);
        assert(!sfCoop.shots.empty() || !sfCoop.beams.empty() || !sfCoop.waves.empty());
        assert(Spritej1->nrj==0 && Spritej2->nrj==0);
        for (int frame=0;frame<90;++frame) sfCoopTick(1.0f/60);
        for (const auto &s : sfCoop.shots) assert(std::isfinite(s.position.x) && std::isfinite(s.position.y) && s.damage>0);
        assert(sfCoop.shots.size()<=600);
    }
    assert(names.size()==50 && planets.size()==50);
    setupCampaign();sfCoop.shots.clear();sfCoop.rocks.clear();
    Spritej1->setxywh(390,400,100,100);Spritej2->setxywh(390,500,100,100);sfCoop.position=tupl(650,840);
    sfCoopEmit(tupl(390,450),float(PI)*.5f,600,0,100);
    for (int frame=0;frame<20;++frame) sfCoopProjectiles(1.0f/60);
    assert(Spritej2->pv==1000 && Spritej2->nrj==0);
    sfCoop.shots.clear();sfCoop.position=tupl(390,840);
    sfCoopEmit(tupl(390,900),-float(PI)*.5f,1200,1,12);sfCoopProjectiles(.1f);
    assert(sfCoop.health==sfCoopProfile().health-12);
    sfCoop.beams={{tupl(390,840),-float(PI)*.5f,0,1}};sfCoop.shots.clear();
    sfCoopProjectiles(.5f);assert(Spritej1->pv==1000);sfCoopProjectiles(.51f);assert(Spritej1->pv<1000);
    setupCampaign();Spritej1->setxywh(390,300,100,100);Spritej2->setxywh(400,310,100,100);Spritej1->nrj=Spritej2->nrj=30;
    sfCoop.dust.push_back({tupl(400,310),tuplv(0,0),10});sfCoopResources(.01f);
    assert(Spritej1->nrj==30 && Spritej2->nrj==26 && sfCoop.dust.empty());
    Spritej2->pv=0;Spritej1->nrj=0;
    for (int frame=0;frame<130;++frame) sfCoopMovePlayers(1.0f/60);
    assert(Spritej2->pv==450 && sfCoop.revives==2);
    Spritej1->pv=Spritej2->pv=0;const auto count=sfCampaignSave.fame.size();sfCoopTick(.016f);
    assert(sfCoop.phase==SfCoopPhase::Defeat && sfCampaignSave.fame.size()==count);
    setupCampaign();sfFixConsumedFingers.clear();
    auto upper=sfCoopFinger(SDL_FINGERDOWN,801,.3f,.25f);sfFixHandleEvent(&upper);
    auto lower=sfCoopFinger(SDL_FINGERDOWN,802,.7f,.75f);sfFixHandleEvent(&lower);
    assert(sfCoop.controls[0].down && sfCoop.controls[1].down);
    const float initial=Spritej1->x;upper=sfCoopFinger(SDL_FINGERMOTION,801,.6f,.3f);sfFixHandleEvent(&upper);
    for (int i=0;i<20;++i) sfCoopMovePlayers(1.0f/60);assert(Spritej1->x>initial+50);
    SDL_Event pause{};pause.type=SDL_APP_DIDENTERBACKGROUND;sfFixHandleEvent(&pause);
    assert(sfCoop.phase==SfCoopPhase::Paused && !sfCoop.controls[0].down && !sfCoop.controls[1].down);
    const auto time=sfCoop.time;sfCoopTick(20);assert(sfCoop.time==time);
    sfActiveMode=sfSelectedMode=SF_DUEL_LOCAL;sfCampaignRestoreDuelShips();
    std::puts("PASS: 50 boss attacks, friendly-fire exclusion, swept hits, warned rays, single-owner dust, revival and paused multitouch");
}
static void testCampaignEntryAndFights()
{
    setupCampaign();sfCampaignSave=SfCampaignSave{};sfFixConsumedFingers.clear();sfFixLaunchPending.store(false);
    sfFixRequestedScreen.store(SF_UI_CAMPAIGN);auto card=sfCampaignCard(49,int(sfArenaW),int(sfArenaH));
    auto touch=sfCoopFinger(SDL_FINGERDOWN,901,(card.x+3)/sfArenaW,(card.y+3)/sfArenaH);sfFixHandleEvent(&touch);
    assert(!sfFixLaunchPending.load());
    card=sfCampaignCard(0,int(sfArenaW),int(sfArenaH));
    touch=sfCoopFinger(SDL_FINGERDOWN,902,(card.x+3)/sfArenaW,(card.y+3)/sfArenaH);sfFixHandleEvent(&touch);
    assert(sfFixLaunchPending.load());sfFixApplyUiRequests();
    assert(sfIsCoop() && sfCoop.phase==SfCoopPhase::Intro && sfUiScreen==SF_UI_GAME);
    for (int boss : {0,9,24,39,49}) {
        setupCampaign(boss,true);
        for (int frame=0;frame<60*240 && sfCoop.phase==SfCoopPhase::Combat;++frame) {
            sfCoop.controls[1].down=true;sfCoop.controls[1].target=tupl(sfArenaW*(.5f+.28f*std::sin(frame/70.0f)),sfArenaH*(.78f+.07f*std::sin(frame/95.0f)));
            sfCoopTick(1.0f/60);assert(std::isfinite(sfCoop.health) && sfCoop.shots.size()<=600);
        }
        assert(sfCoop.phase==SfCoopPhase::Dying || sfCoop.phase==SfCoopPhase::Defeat);
        if (boss==0) assert(sfCoop.phase==SfCoopPhase::Dying);
        std::printf("PLAYTEST: boss %d, %s in %.1fs, HP %.0f/%.0f\n",boss+1,sfCoop.phase==SfCoopPhase::Dying ? "victory" : "defeat",sfCoop.time,Spritej1->pv,Spritej2->pv);
    }
    sfSelectedMode=SF_DUEL_LOCAL;sfFixLaunchPending.store(true);sfFixApplyUiRequests();
    assert(!sfIsCoop() && !sfDuelShipStylesSaved && Spritej1->nrj==0 && Spritej2->nrj==0);
    std::puts("PASS: locked/unlocked missions, five complete unmodified fights and return to fresh duel");
}
static void testCampaignProgression()
{
    sfCampaignSave=SfCampaignSave{};assert(sfSaveCampaign(sfCampaignSave));
    for (int boss=0;boss<50;++boss) {
        setupCampaign(boss);sfCoop.health=1;sfCoop.position=tupl(390,840);
        sfCoopEmit(tupl(390,900),-float(PI)*.5f,1200,0,12);sfCoopProjectiles(.1f);assert(sfCoop.health==0);
        sfCoopTick(.001f);assert(sfCoop.phase==SfCoopPhase::Dying && sfCampaignSave.pending);
        sfCoop.names={{"Fab","Yann","Les Forteresses"}};assert(sfCoopSaveNames());
        assert(sfCampaignSave.cleared==boss+1 && sfCampaignSave.fame.size()==size_t(boss+1));
    }
    sfCampaignSave=SfCampaignSave{};sfCampaignLoaded=false;sfLoadCampaign();
    assert(sfCampaignSave.cleared==50 && sfCampaignSave.fame.size()==50 && sfCampaignSave.fame.back().boss==50);
    sfActiveMode=sfSelectedMode=SF_DUEL_LOCAL;sfCampaignRestoreDuelShips();
    std::puts("PASS: fifty last hits, fifty durable named victories, final boss and complete hall reloaded");
}
static void testCampaignRendering()
{
    const char *directory=std::getenv("SPACEFORTRESS_PREVIEW_DIR");
    auto *surface=SDL_CreateRGBSurfaceWithFormat(0,360,780,32,SDL_PIXELFORMAT_RGBA32);
    auto *renderer=SDL_CreateSoftwareRenderer(surface);assert(renderer);
    auto &textures=sfCoopTextures(renderer);assert(textures.bosses && textures.galaxy && textures.orange && textures.blue);
    std::set<uint64_t> planetHashes;
    for (int boss=0;boss<50;++boss) {
        const auto a=sfBossVertices(textures.bosses,boss,tupl(180,390),70,0),b=sfBossVertices(textures.bosses,boss,tupl(180,390),70,.65f);
        int moving=0;
        for (int i=0;i<81;++i) {
            assert(std::isfinite(a[i].position.x) && std::isfinite(b[i].position.y));
            if (vlong(a[i].position.x-b[i].position.x,a[i].position.y-b[i].position.y)>.25f) ++moving;
            assert(a[i].tex_coord.x>=0 && a[i].tex_coord.x<=1 && a[i].tex_coord.y>=0 && a[i].tex_coord.y<=1);
        }
        assert(moving>35);
        auto *planet=sfCampaignPlanet(textures,boss);assert(planet);
        SDL_SetRenderDrawColor(renderer,0,0,0,255);SDL_RenderClear(renderer);
        SDL_Rect tile{0,0,64,64};assert(SDL_RenderCopy(renderer,planet,nullptr,&tile)==0);
        std::array<Uint32,4096> pixels{};
        assert(SDL_RenderReadPixels(renderer,&tile,SDL_PIXELFORMAT_RGBA32,pixels.data(),64*4)==0);
        uint64_t hash=1469598103934665603ull;
        for (auto pixel : pixels) {hash^=pixel;hash*=1099511628211ull;}
        assert(planetHashes.insert(hash).second);
    }
    auto save=[&](const char *name) {
        if (directory) {SDL_RenderFlush(renderer);assert(IMG_SavePNG(surface,(std::string(directory)+"/"+name+".png").c_str())==0);}
    };
    setupCampaign(22);sfArenaW=360;sfArenaH=780;sfCampaignStart();sfCoop.phase=SfCoopPhase::Combat;
    for (int i=0;i<120;++i) sfCoopTick(1.0f/60);
    sfCoopDrawArena(renderer,360,780);save("arena");
    sfCampaignDrawSelect(renderer);save("missions");
    sfCampaignDrawHall(renderer);save("hall");
    sfCoop.phase=SfCoopPhase::Name;sfCoop.names={{"Fab","Yann","Les Forteresses"}};
    sfCoopDrawArena(renderer,360,780);sfCoopDrawOverlay(renderer,360,780);save("victory");
    SDL_DestroyRenderer(renderer);assert(sfCampaignTextures.empty());SDL_FreeSurface(surface);
    surface=SDL_CreateRGBSurfaceWithFormat(0,780,360,32,SDL_PIXELFORMAT_RGBA32);renderer=SDL_CreateSoftwareRenderer(surface);assert(renderer);
    setupCampaign(49);sfArenaW=780;sfArenaH=360;sfCampaignStart();sfCoop.phase=SfCoopPhase::Combat;
    sfCoopDrawArena(renderer,780,360);save("landscape");
    sfCoop.phase=SfCoopPhase::Name;sfCoop.names={{"Fab","Yann","Les Forteresses"}};
    sfCoopDrawOverlay(renderer,780,360);save("landscape-victory");
    SDL_DestroyRenderer(renderer);assert(sfCampaignTextures.empty());SDL_FreeSurface(surface);
    sfActiveMode=sfSelectedMode=SF_DUEL_LOCAL;sfCampaignRestoreDuelShips();
    std::puts("PASS: fifty animated meshes, fifty distinct rendered planets, actual assets, portrait/landscape screens and renderer cleanup");
}
static void testCampaign()
{
    char directory[]="/tmp/spacefortress-campaign-XXXXXX";assert(mkdtemp(directory));
    testVelocityGhosts();testCampaignPersistence(directory);testCoopGameplay();testCampaignEntryAndFights();testCampaignProgression();testCampaignRendering();
    for (const char *name : {"/campaign-v1.dat","/campaign-v1.dat.bak","/future.dat"}) std::remove((std::string(directory)+name).c_str());
    ::rmdir(directory);
}
