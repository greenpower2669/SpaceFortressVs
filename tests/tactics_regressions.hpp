#pragma once

static void setupTactics()
{
    sfTacticsReset(); sfFixResetAsteroidField();
    sfArenaW=780; sfArenaH=1680; sfFrameDt=1.0f/60; k0=1;
    sfUiScreen=SF_UI_GAME; sfFixRequestedScreen.store(SF_UI_GAME);
    setgui=false; setia=true;
    Spritej1->setxywh(390,300,100,100); Spritej2->setxywh(390,1200,100,100);
    for (auto *ship : {Spritej1,Spritej2}) {
        ship->sw=ship->sh=100;ship->boundedBreathing=false;
    }
    Spritej1->pv=Spritej2->pv=1000; Spritej1->nrj=Spritej2->nrj=20;
    loosej1->pv=loosej2->pv=0;
}

static void testTacticalPilot()
{
    setupTactics();
    const tupl target(390,1200), origin(390,300);
    auto stationary=sfPredictIntercept(origin,target,tuplv(0,0),1000);
    assert(stationary.x==390 && stationary.y==1200);
    auto lead=sfPredictIntercept(origin,target,tuplv(120,0),1000);
    assert(lead.x>440 && lead.x<=486.01f && lead.y==1200);
    auto opposite=sfPredictIntercept(origin,target,tuplv(-120,0),1000);
    assert(opposite.x<340);
    auto fast=sfPredictIntercept(origin,target,tuplv(4000,0),1);
    assert(std::isfinite(fast.x) && fast.x<=target.x+3200.1f);
    sfObserved[1].observe(target,.1f);
    sfObserved[1].observe(tupl(410,1200),.1f);
    assert(sfObserved[1].velocity.vx>0);
    sfObserved[1].observe(tupl(40,1200),.1f);
    assert(sfObserved[1].velocity.vx==0); // Teleport/restart is not a real velocity.
    sfObserved={}; sfPilot.cooldown=0;
    for (int i=0;i<9;++i) sfUpdatePilot(sfFrameDt);
    assert(entitiesj2.empty());
    sfUpdatePilot(sfFrameDt);
    assert(entitiesj2.size()==1 && entitiesj2.front()->vy>0 && sfRmFlashJ1Start>0);
    for (int i=0;i<10;++i) sfUpdatePilot(sfFrameDt);
    assert(entitiesj2.size()==1);
    assert(sfFireMain(1) && entitiesj1.back()->vy<0 && tirj2z);

    setupTactics();
    auto *dust=new parts(420,350); dust->pv=600; particules.push_back(dust);
    Spritej1->nrj=30; sfThinkPilot();
    assert(sfPilot.mode==SfAiMode::Collect && sfPilot.goal.x==420);
    // When both ships overlap one particle, only the nearest may recharge.
    Spritej1->setxywh(390,350,100,100); Spritej2->setxywh(421,350,100,100);
    Spritej1->nrj=Spritej2->nrj=30; Spritej2->pv=999.9f;
    sfCollectDust();
    assert(dust->pv==0 && Spritej2->nrj<30 && Spritej1->nrj==30 && Spritej2->pv<=1000);
    const float charged=Spritej2->nrj; sfCollectDust(); assert(Spritej2->nrj==charged);
    sfTacticsReset();
    assert(particules.empty() && entitiesj1.empty() && entitiesj2.empty() && tirj1==0 && tirj2==0);

    setupTactics();
    auto *rock=new sprite; rock->setxywh(350,550,100,100);
    rock->pv=100; rock->vx=rock->vy=0; sa1.push_back(rock);
    Spritej1->nrj=30; sfThinkPilot();
    assert(sfPilot.mode==SfAiMode::Mine && sfPilot.goal.y<rock->y-100);

    // Head-on encounter: measure clearance along the path, not final offset
    // after the pilot has already returned toward its goal.
    for (int fps : {30,60}) {
        setupTactics(); sfFrameDt=1.0f/fps;
        auto *obstacle=new sprite; obstacle->setxywh(390,600,100,100);
        obstacle->pv=100; obstacle->vx=0; obstacle->vy=-120*sfFrameDt;
        sa1.push_back(obstacle);
        tupl position(390,200); tuplv velocity;
        float nearest=10000, maxOffset=0;
        for (int frame=0;frame<fps*3;++frame) {
            velocity=sfAvoidAsteroids(position,tupl(390,650),velocity,43);
            position.x+=velocity.vx*sfFrameDt; position.y+=velocity.vy*sfFrameDt;
            obstacle->y+=obstacle->vy;
            nearest=std::min(nearest,vlong(position.x-obstacle->x,position.y-obstacle->y));
            maxOffset=std::max(maxOffset,std::abs(position.x-390));
            assert(position.x>=40 && position.x<=740 && position.y>=40 && position.y<840);
        }
        assert(nearest>93 && maxOffset>40);
    }
    setupTactics(); setgui=true;
    const float previousY=Spritej1->y; sfUpdatePilot(.1f);
    assert(Spritej1->y==previousY && entitiesj2.empty());
    std::puts("PASS: predictive aiming, alignment delay, 30/60fps avoidance, mining and single-owner energy collection");
}

static void testTacticalTurrets()
{
    for (int owner=0;owner<2;++owner) {
        setupTactics(); setia=false;
        sprite *enemy=owner==0 ? Spritej2 : Spritej1;
        auto &shots=owner==0 ? entitiesj2 : entitiesj1;
        enemy->y=sfArenaH*(owner==0 ? .52f : .48f);
        for (int frame=0;frame<90;++frame) sfUpdateTurrets(sfFrameDt);
        assert(sfTurrets[owner*SF_TURRETS_PER_TEAM].deploy>.98f && shots.empty());
        enemy->y=sfArenaH*(owner==0 ? .43f : .57f);
        for (int frame=0;frame<120 && shots.empty();++frame) sfUpdateTurrets(sfFrameDt);
        assert(!shots.empty() && sfRmFlashJ1Start==0 && sfRmFlashJ2Start==0);
        bool found=false;
        for (int i=owner*SF_TURRETS_PER_TEAM;i<(owner+1)*SF_TURRETS_PER_TEAM;++i) if (sfTurrets[i].flash>=.159f) {
            const tupl muzzle=sfTurretMuzzle(i);
            for (const auto *shot : shots) {
                if (vlong(shot->x-muzzle.x,shot->y-muzzle.y)<.1f) {
                    assert(shot->vx*(enemy->x-shot->x)+shot->vy*(enemy->y-shot->y)>0);
                    found=true;
                }
            }
        }
        assert(found);
        for (auto &turret : sfTurrets) turret.cooldown=1;
        const auto count=shots.size();
        for (int frame=0;frame<15;++frame) sfUpdateTurrets(sfFrameDt);
        assert(shots.size()==count);
        enemy->y=sfArenaH*(owner==0 ? .80f : .20f);
        for (int frame=0;frame<90;++frame) sfUpdateTurrets(sfFrameDt);
        for (int i=owner*SF_TURRETS_PER_TEAM;i<(owner+1)*SF_TURRETS_PER_TEAM;++i) assert(sfTurrets[i].deploy==0);
    }
    sfFixResetMatchState();
    assert(entitiesj1.empty() && entitiesj2.empty() && sfSceneSeconds==0);
    std::puts("PASS: both teams' turret deployment, territory gate, muzzle origin, cooldown, retraction and match reset");
}

static void testJupiterMotion()
{
    for (const auto size : {SDL_Point{720,1560},SDL_Point{640,360}}) {
        auto *surface=SDL_CreateRGBSurfaceWithFormat(0,size.x,size.y,32,SDL_PIXELFORMAT_RGBA32);
        auto *renderer=SDL_CreateSoftwareRenderer(surface); assert(renderer);
        SDL_Rect legacy{-600,-300,1680,1680};
        sfSceneSeconds=0; auto start=sfFinalRoundBody(renderer,legacy,false);
        sfSceneSeconds=20; auto later=sfFinalRoundBody(renderer,legacy,false);
        sfSceneSeconds=20.02; auto next=sfFinalRoundBody(renderer,legacy,false);
        assert(start.w==later.w && start.h==later.h);
        assert(std::abs(later.x-start.x)+std::abs(later.y-start.y)>15);
        assert(std::abs(next.x-later.x)+std::abs(next.y-later.y)<=2);
        assert(later.x>0 && later.y>0 && later.x+later.w<size.x && later.y+later.h<size.y);
        SDL_DestroyRenderer(renderer); SDL_FreeSurface(surface);
    }
    sfSceneSeconds=0;
    std::puts("PASS: continuous Jupiter drift, fixed apparent size and portrait/landscape containment");
}

static void testRaidsAndDefence()
{
    // One moving resource is tracked through a real crossing, then abandoned.
    for (int fps : {30,60}) {
        setupTactics(); sfFrameDt=1.0f/fps;
        Spritej1->y=450; Spritej1->nrj=5;
        sfPilot.aggressiveFor=8; sfPilot.nextAggression=100;
        auto *rock=new sprite; rock->setxywh(390,700,100,100);
        rock->pv=100; rock->vx=0; rock->vy=60*sfFrameDt; sa1.push_back(rock);
        sfUpdatePilot(sfFrameDt);
        assert(sfPilot.mode==SfAiMode::RaidMine && sfPilot.asteroidId!=0);
        const Uint64 targetId=sfPilot.asteroidId;
        bool entered=false, retreated=false, returned=false;
        for (int frame=0;frame<fps*16;++frame) {
            rock->y+=rock->vy;
            sfUpdatePilot(sfFrameDt);
            entered=entered || Spritej1->y>sfArenaH*.5f;
            if (sfPilot.mode==SfAiMode::RaidMine) assert(sfPilot.asteroidId==targetId);
            if (sfPilot.mode==SfAiMode::Retreat) { retreated=true; assert(sfPilot.asteroidId==0); }
            if (retreated && Spritej1->y<sfArenaH*.34f) { returned=true; break; }
        }
        assert(entered && retreated && returned);
        assert(sfPilot.raidCooldown>0);
    }

    setupTactics(); Spritej1->nrj=5; sfPilot.aggressiveFor=4; sfPilot.nextAggression=100;
    sfThinkPilot(); const float pushY=sfPilot.goal.y;
    Spritej1->nrj=38; sfUpdatePilot(sfFrameDt);
    assert(sfPilot.aggressiveFor==0 && sfPilot.goal.y<=pushY);

    // Incoming defence fire overrides mining immediately; no stale pointer is
    // dereferenced when the asteroid is then destroyed by the world simulation.
    setupTactics(); Spritej1->y=890; Spritej1->nrj=10;
    sfPilot.mode=SfAiMode::RaidMine; sfPilot.asteroidId=++sfNextAsteroidId;
    auto *rock=new sprite; rock->setxywh(390,1050,100,100);
    rock->tacticalId=sfPilot.asteroidId; rock->vy=1; sa1.push_back(rock);
    auto *bullet=sfMakeShot(1,true); assert(bullet);
    bullet->setxywh(390,1290,16,16); bullet->shotVelocityX=0; bullet->shotVelocityY=-1170;
    sfUpdatePilot(sfFrameDt);
    assert(sfPilot.mode==SfAiMode::Retreat && sfPilot.asteroidId==0);
    sfFixResetAsteroidField(); sfThinkPilot(); assert(sfPilot.mode==SfAiMode::Retreat);
    float nearest=10000, maxOffset=0;
    for (int i=0;i<60;++i) {
        sfAdvanceProjectile(bullet); sfUpdatePilot(sfFrameDt);
        nearest=std::min(nearest,vlong(Spritej1->x-bullet->x,Spritej1->y-bullet->y));
        maxOffset=std::max(maxOffset,std::abs(Spritej1->x-390));
    }
    assert(nearest>51 && maxOffset>40);

    // Defence owns its ammunition and cannot spend the vessel's energy or
    // block its gun merely by filling the shared legacy rendering list.
    setupTactics(); setia=false; Spritej2->y=sfArenaH*.43f;
    for (int i=0;i<50;++i) assert(sfMakeShot(0));
    assert(!sfMakeShot(0));
    sfObserved[1].velocity.set(150,0);
    const float energy1=Spritej1->nrj, energy2=Spritej2->nrj;
    for (int frame=0;frame<120;++frame) sfUpdateTurrets(sfFrameDt);
    int defences=0;
    for (auto *shot : entitiesj2) if (shot->defensiveShot) {
        ++defences;
        assert(vlong(shot->shotVelocityX,shot->shotVelocityY)>sfArenaW*1.4f);
    }
    assert(defences>=10 && Spritej1->nrj==energy1 && Spritej2->nrj==energy2);
    for (int i=0;i<SF_TURRETS_PER_TEAM;++i) assert(sfTurrets[i].virtualTarget.x>Spritej2->x);

    // A fast projectile crosses the target between frames and still hits.
    setupTactics();
    bullet=sfMakeShot(1,true); bullet->setxywh(390,1000,16,16);
    bullet->shotVelocityX=0; bullet->shotVelocityY=-1170;
    sfFrameDt=.1f; sfAdvanceProjectile(bullet);
    Spritej1->setxywh(390,940,24,24);
    assert(std::abs(bullet->y-Spritej1->y)>35 && sfShotCrosses(Spritej1,bullet));
    const float hitHeat=sfShotHeat(bullet), from=bullet->y;
    sfFrameDt=1.0f/60; sfAdvanceProjectile(bullet);
    assert(std::abs((from-bullet->y)-19.5f)<.01f && sfShotHeat(bullet)==hitHeat);
    std::puts("PASS: mining raids and return at 30/60fps, retreat under fire, projectile evasion, virtual turret targets and vessel-only energy");
}

static void writeTurretPreview(const char *path)
{
    if (!path) return;
    setupTactics(); setia=false; sfArenaW=709; sfArenaH=1536;
    Spritej1->setxywh(200,230,165,165); Spritej2->setxywh(360,610,165,165);
    auto *surface=SDL_CreateRGBSurfaceWithFormat(0,709,1536,32,SDL_PIXELFORMAT_RGBA32);
    auto *renderer=SDL_CreateSoftwareRenderer(surface); assert(renderer);
    auto *galaxy=IMG_LoadTexture(renderer,"resources/assets/pict/fond4hlz.png");
    auto *sun=IMG_LoadTexture(renderer,"resources/assets/pict/suno.png");
    auto *planet=IMG_LoadTexture(renderer,"resources/assets/pict/jupsoeur4.png");
    assert(galaxy && sun && planet);
    SDL_Rect background{0,0,780,1690}, solar{-240,-110,800,800}, jovian{850,850,1680,1680};
    SDL_RenderCopy(renderer,galaxy,nullptr,&background);
    SDL_RenderCopy(renderer,sun,nullptr,&solar); SDL_RenderCopy(renderer,planet,nullptr,&jovian);
    for (int frame=0;frame<120;++frame) sfUpdateTurrets(sfFrameDt);
    for (int i=0;i<SF_TURRETS_PER_TEAM;++i) sfTurrets[i].flash=.16f;
    sfDrawTacticalEffects(renderer);
    SDL_RenderFlush(renderer); assert(IMG_SavePNG(surface,path)==0);
    SDL_DestroyRenderer(renderer); SDL_FreeSurface(surface);
}
