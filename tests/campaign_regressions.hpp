#pragma once
#include <set>

static SDL_Event sfCoopFinger(Uint32 type,SDL_FingerID id,float x,float y)
{
    SDL_Event event{};event.type=type;event.tfinger.type=type;event.tfinger.fingerId=id;event.tfinger.x=x;event.tfinger.y=y;return event;
}
static void setupCampaign(int boss=0,bool ai=false)
{
    setupTactics();sfSelectedMode=sfActiveMode=ai ? SF_COOP_AI : SF_COOP_LOCAL;
    sfFixRequestedIa.store(ai);setia=ai;
    sfCampaignSave.pending=false;sfCampaignSave.selected=boss;sfCampaignSave.cleared=std::max(sfCampaignSave.cleared,boss);
    sfCampaignStart();sfCoop.phase=SfCoopPhase::Combat;
}
static void testVelocityGhosts()
{
    for (int fps : {30,60,120}) for (int owner=0;owner<2;++owner) {
        setupTactics();sfActiveMode=sfSelectedMode=SF_DUEL_LOCAL;sfFrameDt=1.0f/fps;
        auto *shooter=owner==0 ? Spritej1 : Spritej2;auto *target=owner==0 ? Spritej2 : Spritej1;
        shooter->nrj=20;
        sfObserved[1-owner].velocity.set(owner==0 ? 100 : -100,0);
        const auto ghost=sfShipGhost(1-owner);
        const auto aim=ghost.intercept(tupl(shooter->x,shooter->y),sfMainShotSpeed(shooter->nrj));
        assert((aim.x-target->x)*sfObserved[1-owner].velocity.vx>0);
        assert(sfFireMain(owner,&aim));
        auto *shot=(owner==0 ? entitiesj2 : entitiesj1).back();
        assert((shot->shotVelocityX)*sfObserved[1-owner].velocity.vx>0);
        float nearest=10000;
        for (int frame=0;frame<fps*2 && shot->pv>0;++frame) {
            target->x+=sfObserved[1-owner].velocity.vx*sfFrameDt;sfAdvanceProjectile(shot);
            nearest=std::min(nearest,sfSegmentDistance(tupl(shot->shotFromX,shot->shotFromY),tupl(shot->x,shot->y),tupl(target->x,target->y)));
        }
        assert(nearest<16); // Aimed at a future point, not the starting location.
    }
    setupTactics();sfActiveMode=sfSelectedMode=SF_DUEL_AI;
    Spritej2->y=840;sfObserved[1].velocity.set(160,-90);sfThinkPilot();
    const auto right=sfPilot.goal;
    sfObserved[1].velocity.set(-160,90);sfThinkPilot();
    assert(right.x>sfPilot.goal.x && right.y<sfPilot.goal.y);
    assert(sa1.empty()); // Ghosts never become collidable entities.
    sfActiveMode=sfSelectedMode=SF_DUEL_LOCAL;
    std::puts("PASS: both weapons intercept moving velocity ghosts at 30/60/120fps; pursuit changes X and Y with future motion");
}

static void testCampaignPersistence(const std::string &directory)
{
    sfCampaignStoragePath=directory+"/campaign-v1.dat";sfCampaignLoaded=false;sfCampaignStorageBlocked=false;sfCampaignStorageError.clear();
    sfCampaignSave=SfCampaignSave{};sfLoadCampaign();assert(sfCampaignSave.fame.empty());
    setupCampaign();
    sfCoop.health=0;sfCoop.time=12;sfCoopWin();
    assert(sfCampaignSave.pending && sfCampaignSave.cleared==1 && sfCoop.pendingSaved);
    const auto pendingId=sfCampaignSave.victory.id;
    // A process restart before name entry must recover the victory prompt.
    sfCampaignSave=SfCampaignSave{};sfCampaignLoaded=false;sfLoadCampaign();sfCampaignStart();
    assert(sfCampaignSave.pending && sfCampaignSave.victory.id==pendingId && sfCoop.phase==SfCoopPhase::Name);
    sfCoop.names={{"Fabrice","Yann","Equipe Eclipse"}};
    sfCoop.nameField=2;
    SDL_Event input{};input.type=SDL_TEXTINPUT;std::strcpy(input.text.text," ");sfFixHandleEvent(&input);
    input.type=SDL_TEXTINPUT;std::strcpy(input.text.text,"é");sfFixHandleEvent(&input);
    assert(sfCoop.names[2]=="Equipe Eclipse é");
    SDL_Event backspace{};backspace.type=SDL_KEYDOWN;backspace.key.keysym.sym=SDLK_BACKSPACE;sfFixHandleEvent(&backspace);
    assert(sfCoop.names[2]=="Equipe Eclipse ");
    sfCoop.names[2]="Équipe Éclipse";
    assert(sfCoopSaveNames());assert(!sfCampaignSave.pending && sfCampaignSave.fame.size()==1);
    assert(!sfCoopSaveNames() && sfCampaignSave.fame.size()==1); // No duplicate award.
    sfCampaignSave=SfCampaignSave{};sfCampaignLoaded=false;sfLoadCampaign();
    assert(sfCampaignSave.fame.size()==1 && sfCampaignSave.fame[0].names[2]=="Équipe Éclipse");
    auto candidate=sfCampaignSave;candidate.cleared=2;assert(sfSaveCampaign(candidate));
    {std::ofstream out(sfCampaignStoragePath);out<<"damaged";}
    sfCampaignLoaded=false;sfLoadCampaign();
    assert(sfCampaignSave.fame.size()==1 && !sfCampaignStorageError.empty());
    assert(sfSaveCampaign(sfCampaignSave));
    // Unknown future formats are preserved, not reset into an empty hall.
    sfCampaignStoragePath=directory+"/future.dat";
    {std::ofstream out(sfCampaignStoragePath);out<<"SPACEFORTRESS_CAMPAIGN 9\nfuture data\n";}
    sfCampaignLoaded=false;sfLoadCampaign();assert(sfCampaignStorageBlocked);
    assert(!sfSaveCampaign(sfCampaignSave));
    {std::ifstream in(sfCampaignStoragePath);std::string bytes((std::istreambuf_iterator<char>(in)),{});assert(bytes.find("future data")!=std::string::npos);}
    sfCampaignStoragePath=directory+"/campaign-v1.dat";sfCampaignLoaded=false;sfCampaignStorageBlocked=false;sfLoadCampaign();
    // Simulate a write failure. The durable hall and its entries remain intact.
    const auto original=sfCampaignStoragePath;sfCampaignStoragePath=directory+"/missing/directory/file.dat";
    assert(!sfSaveCampaign(sfCampaignSave));sfCampaignStoragePath=original;
    assert(sfReadCampaignFile(original,candidate) && candidate.fame.size()==1);
    sfCampaignStorageError.clear();
    std::puts("PASS: UTF-8 names, pending victory restart, atomic hall writes, duplicate prevention, backup recovery and failure preservation");
}

static void testCoopGameplay()
{
    sfCampaignSave=SfCampaignSave{};
    std::set<std::string> names;std::set<int> planets;
    float previousHealth=0,previousInterval=2;
    for (const auto &profile : sfBossCatalog()) {
        assert(names.insert(profile.name).second && planets.insert(profile.planet).second);
        assert(profile.health>previousHealth && profile.interval<previousInterval && profile.planet<55);
        previousHealth=profile.health;previousInterval=profile.interval;
        setupCampaign(profile.index);sfCoopPattern(profile.family);
        assert(!sfCoop.shots.empty() || !sfCoop.beams.empty() || !sfCoop.waves.empty());
        assert(Spritej1->nrj==0 && Spritej2->nrj==0);
        for (int frame=0;frame<90;++frame) sfCoopTick(1.0f/60);
        for (const auto &shot : sfCoop.shots) assert(std::isfinite(shot.position.x) && std::isfinite(shot.position.y) && shot.damage>0);
        assert(sfCoop.shots.size()<=600);
    }
    assert(names.size()==50 && planets.size()==50);
    setupCampaign();sfCoop.shots.clear();sfCoop.rocks.clear();
    Spritej1->setxywh(390,400,100,100);Spritej2->setxywh(390,500,100,100);
    sfCoop.position=tupl(650,840);
    sfCoopEmit(tupl(390,450),float(PI)*.5f,600,0,100);
    for (int frame=0;frame<20;++frame) sfCoopProjectiles(1.0f/60);
    assert(Spritej2->pv==1000 && Spritej2->nrj==0); // Ally fire never hurts.
    sfCoop.shots.clear();sfCoop.position=tupl(390,840);
    sfCoopEmit(tupl(390,900),-float(PI)*.5f,1200,1,12);
    sfCoopProjectiles(.1f);assert(sfCoop.health==sfCoopProfile().health-12);
    sfCoop.beams={{tupl(390,840),-float(PI)*.5f,0,1}};
    sfCoop.shots.clear();
    sfCoopProjectiles(.5f);assert(Spritej1->pv==1000);
    sfCoopProjectiles(.51f);assert(Spritej1->pv<1000); // Real telegraph before damage.

    setupCampaign();Spritej1->setxywh(390,300,100,100);Spritej2->setxywh(400,310,100,100);
    Spritej1->nrj=Spritej2->nrj=30;
    sfCoop.dust.push_back({tupl(400,310),tuplv(0,0),10});sfCoopResources(.01f);
    assert(Spritej1->nrj==30 && Spritej2->nrj==26 && sfCoop.dust.empty());
    Spritej2->pv=0;Spritej1->nrj=0;
    for (int frame=0;frame<130;++frame) sfCoopMovePlayers(1.0f/60);
    assert(Spritej2->pv==450 && sfCoop.revives==2);
    Spritej1->pv=Spritej2->pv=0;const auto count=sfCampaignSave.fame.size();sfCoopTick(.016f);
    assert(sfCoop.phase==SfCoopPhase::Defeat && sfCampaignSave.fame.size()==count);

    // Intro/controls/pause use the real final event bridge, including held
    // touches released after a menu action and movement across the midline.
    setupCampaign();sfFixConsumedFingers.clear();
    auto upper=sfCoopFinger(SDL_FINGERDOWN,801,.3f,.25f);sfFixHandleEvent(&upper);
    auto lower=sfCoopFinger(SDL_FINGERDOWN,802,.7f,.75f);sfFixHandleEvent(&lower);
    assert(sfCoop.controls[0].down && sfCoop.controls[1].down);
    const float initialX=Spritej1->x;
    upper=sfCoopFinger(SDL_FINGERMOTION,801,.6f,.3f);sfFixHandleEvent(&upper);
    for (int i=0;i<20;++i) sfCoopMovePlayers(1.0f/60);
    assert(Spritej1->x>initialX+50);
    SDL_Event paused{};paused.type=SDL_APP_DIDENTERBACKGROUND;sfFixHandleEvent(&paused);
    assert(sfCoop.phase==SfCoopPhase::Paused && !sfCoop.controls[0].down && !sfCoop.controls[1].down);
    const auto time=sfCoop.time;sfCoopTick(20);assert(sfCoop.time==time);
    sfActiveMode=sfSelectedMode=SF_DUEL_LOCAL;sfCampaignRestoreDuelShips();
    std::puts("PASS: all 50 progressive bosses, hostile patterns, no friendly fire, telegraphs, mining, revival and paused multitouch");
}

static void testCoopArenaBounds()
{
    for (const auto size : {std::pair<int,int>{360,780},{780,360},{709,1536},{1536,709}}) {
        setupCampaign(49);sfArenaW=size.first;sfArenaH=size.second;sfCampaignStart();
        sfCoop.phase=SfCoopPhase::Combat;
        // Spawn sprites must leave the HUD clear in either orientation.
        assert(Spritej1->y-Spritej1->h*.5f>sfArenaH*.105f);
        assert(Spritej2->y+Spritej2->h*.5f<sfArenaH*.895f);
        assert(sfCoopBossRadius()*2<sfArenaH*.4f);
        const float startX=Spritej1->x;
        sfCoop.controls[0].down=true;
        sfCoop.controls[0].target=tupl(sfArenaW*.75f,Spritej1->y);
        for (int frame=0;frame<30;++frame) sfCoopMovePlayers(1.0f/60);
        assert(Spritej1->x>startX && Spritej1->pv==1000);
    }
    sfActiveMode=sfSelectedMode=SF_DUEL_LOCAL;sfCampaignRestoreDuelShips();
    std::puts("PASS: cooperative spawn, movement and collision sizes fit portrait and landscape arenas");
}

static void testCoopCollisionMinerals()
{
    setupCampaign();sfCoop.rocks.clear();sfCoop.dust.clear();
    Spritej1->nrj=40;
    sfCoop.rocks.push_back({tupl(Spritej1->x,Spritej1->y),tuplv(0,0),20,25});
    sfCoopResources(.001f);
    assert(sfCoop.rocks.empty());
    // Seven fragments are collected once by the ship that hit the rock.
    // The hit adds two heat units, then each fragment restores four.
    assert(Spritej1->nrj==14 && sfCoop.dust.empty());
    const float energy=Spritej1->nrj;
    sfCoopResources(.001f);assert(Spritej1->nrj==energy);
    sfActiveMode=sfSelectedMode=SF_DUEL_LOCAL;sfCampaignRestoreDuelShips();
    std::puts("PASS: collision-destroyed asteroids drop and award minerals exactly once");
}

static void testCampaignProgression()
{
    sfCampaignSave=SfCampaignSave{};assert(sfSaveCampaign(sfCampaignSave));
    for (int boss=0;boss<50;++boss) {
        setupCampaign(boss);
        // Exercise the actual last hit, result phase and durable award at each
        // boundary. Difficulty balance is tested separately from progression.
        sfCoop.health=1;sfCoop.position=tupl(390,840);
        sfCoopEmit(tupl(390,900),-float(PI)*.5f,1200,0,12);
        sfCoopProjectiles(.1f);assert(sfCoop.health==0);
        sfCoopTick(.001f);assert(sfCoop.phase==SfCoopPhase::Dying && sfCampaignSave.pending);
        sfCoop.names={{"Fab","Yann","Les Forteresses"}};assert(sfCoopSaveNames());
        assert(sfCampaignSave.cleared==boss+1 && sfCampaignSave.fame.size()==size_t(boss+1));
    }
    sfCampaignSave=SfCampaignSave{};sfCampaignLoaded=false;sfLoadCampaign();
    assert(sfCampaignSave.cleared==50 && sfCampaignSave.fame.size()==50 && sfCampaignSave.fame.back().boss==50);
    sfActiveMode=sfSelectedMode=SF_DUEL_LOCAL;sfCampaignRestoreDuelShips();
    std::puts("PASS: 50 actual last hits, 50 durable named victories, final boss boundary and complete hall after reload");
}

static void testCampaignEntryAndFights()
{
    setupCampaign();sfCampaignSave=SfCampaignSave{};
    sfFixConsumedFingers.clear();sfFixLaunchPending.store(false);
    sfFixRequestedScreen.store(SF_UI_CAMPAIGN);
    auto card=sfCampaignCard(49,int(sfArenaW),int(sfArenaH));
    auto touch=sfCoopFinger(SDL_FINGERDOWN,901,(card.x+3)/sfArenaW,(card.y+3)/sfArenaH);
    sfFixHandleEvent(&touch);assert(!sfFixLaunchPending.load()); // Locked boss stays locked.
    card=sfCampaignCard(0,int(sfArenaW),int(sfArenaH));
    touch=sfCoopFinger(SDL_FINGERDOWN,902,(card.x+3)/sfArenaW,(card.y+3)/sfArenaH);
    sfFixHandleEvent(&touch);assert(sfFixLaunchPending.load());
    sfFixApplyUiRequests();assert(sfIsCoop() && sfCoop.phase==SfCoopPhase::Intro && sfUiScreen==SF_UI_GAME);
    // Ordinary full fights, without changing health or injecting last hits.
    for (int boss : {0,9,24,39,49}) {
        setupCampaign(boss,true);
        for (int frame=0;frame<60*240 && sfCoop.phase==SfCoopPhase::Combat;++frame) {
            sfCoop.controls[1].down=true;
            sfCoop.controls[1].target=tupl(sfArenaW*(.5f+.28f*std::sin(frame/70.0f)),sfArenaH*(.78f+.07f*std::sin(frame/95.0f)));
            sfCoopTick(1.0f/60);
            assert(std::isfinite(sfCoop.health) && sfCoop.shots.size()<=600);
        }
        assert(sfCoop.phase==SfCoopPhase::Dying || sfCoop.phase==SfCoopPhase::Defeat);
        if (boss==0) assert(sfCoop.phase==SfCoopPhase::Dying);
        std::printf("PLAYTEST: boss %d, %s in %.1fs, HP %.0f/%.0f\n",boss+1,
            sfCoop.phase==SfCoopPhase::Dying ? "victory" : "defeat",sfCoop.time,Spritej1->pv,Spritej2->pv);
    }
    sfSelectedMode=SF_DUEL_LOCAL;sfFixLaunchPending.store(true);sfFixApplyUiRequests();
    assert(!sfIsCoop() && !sfDuelShipStylesSaved && Spritej1->nrj==0 && Spritej2->nrj==0);
    std::puts("PASS: locked/unlocked mission entry, five complete fights, cooperative AI and return to fresh duel");
}

static void testCampaignRendering(const char *previewDirectory)
{
    auto *surface=SDL_CreateRGBSurfaceWithFormat(0,709,1536,32,SDL_PIXELFORMAT_RGBA32);
    auto *renderer=SDL_CreateSoftwareRenderer(surface);assert(renderer);
    auto &textures=sfCoopTextures(renderer);assert(textures.bosses && textures.planets && textures.backgrounds);
    for (int boss=0;boss<50;++boss) {
        const auto a=sfBossVertices(textures.bosses,boss,tupl(350,760),130,0);
        const auto b=sfBossVertices(textures.bosses,boss,tupl(350,760),130,.65f);
        int moving=0;
        for (int i=0;i<81;++i) {
            assert(std::isfinite(a[i].position.x) && std::isfinite(b[i].position.y));
            if (vlong(a[i].position.x-b[i].position.x,a[i].position.y-b[i].position.y)>.5f) ++moving;
            assert(a[i].tex_coord.x>=0 && a[i].tex_coord.x<=1 && a[i].tex_coord.y>=0 && a[i].tex_coord.y<=1);
        }
        assert(moving>35); // Articulated/deforming mesh for every monster.
    }
    auto save=[&](const std::string &name) {
        if (!previewDirectory) return;
        SDL_RenderFlush(renderer);assert(IMG_SavePNG(surface,(std::string(previewDirectory)+"/"+name+".png").c_str())==0);
    };
    for (int boss : {0,12,24,39,49}) {
        setupCampaign(boss);sfArenaW=709;sfArenaH=1536;
        for (int i=0;i<120;++i) sfCoopTick(1.0f/60);
        sfCoopDrawArena(renderer,709,1536);save("boss-"+std::to_string(boss+1));
    }
    sfCampaignDrawSelect(renderer);save("missions");
    sfCampaignDrawHall(renderer);save("hall");
    sfCoop.phase=SfCoopPhase::Name;sfCoop.names={{"Fabrice","Yann","Les Forteresses"}};
    sfCoopDrawArena(renderer,709,1536);sfCoopDrawOverlay(renderer,709,1536);save("victory");
    if (previewDirectory) {
        setupCampaign(22);sfArenaW=709;sfArenaH=1536;
        for (int frame=0;frame<60;++frame) {
            sfCoopTick(1.0f/20);sfCoopDrawArena(renderer,709,1536);
            char name[32];std::snprintf(name,sizeof(name),"animation-%03d",frame);save(name);
        }
    }
    SDL_DestroyRenderer(renderer);assert(sfCampaignTextures.empty());SDL_FreeSurface(surface);
    if (previewDirectory) {
        surface=SDL_CreateRGBSurfaceWithFormat(0,1536,709,32,SDL_PIXELFORMAT_RGBA32);
        renderer=SDL_CreateSoftwareRenderer(surface);assert(renderer);
        setupCampaign(49);sfArenaW=1536;sfArenaH=709;sfCampaignStart();sfCoop.phase=SfCoopPhase::Combat;
        sfCoopDrawArena(renderer,1536,709);save("landscape-arena");
        sfCampaignDrawSelect(renderer);save("landscape-missions");
        sfCampaignDrawHall(renderer);save("landscape-hall");
        sfCoop.phase=SfCoopPhase::Name;sfCoop.names={{"Fab","Yann","Les Forteresses"}};
        sfCoopDrawArena(renderer,1536,709);sfCoopDrawOverlay(renderer,1536,709);save("landscape-victory");
        SDL_DestroyRenderer(renderer);assert(sfCampaignTextures.empty());SDL_FreeSurface(surface);
    }
    sfActiveMode=sfSelectedMode=SF_DUEL_LOCAL;sfCampaignRestoreDuelShips();
    std::puts("PASS: 50 animated meshes and UV bounds, actual campaign textures, mission grid, name entry, hall and renderer cleanup");
}
