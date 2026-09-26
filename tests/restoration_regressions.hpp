#pragma once
static void testNoHumanAutofire()
{
    setupCampaign();
    for(int i=0;i<120;++i) sfCoopMovePlayers(1.0f/60);
    assert(sfCoop.shots.empty());
    auto a=sfCoopFinger(SDL_FINGERDOWN,1001,.3f,.2f);
    auto b=sfCoopFinger(SDL_FINGERDOWN,1002,.7f,.8f);
    sfFixHandleEvent(&a);sfFixHandleEvent(&b);
    for(int i=0;i<20;++i) sfCoopMovePlayers(1.0f/60);
    assert(sfCoop.shots.empty());
    a=sfCoopFinger(SDL_FINGERDOWN,1003,.4f,.2f);
    b=sfCoopFinger(SDL_FINGERDOWN,1004,.6f,.8f);
    sfFixHandleEvent(&a);sfFixHandleEvent(&b);
    assert(sfCoop.shots.size()==2);
    assert(sfCoop.shots[0].kind==4 && sfCoop.shots[1].kind==4);
    assert(Spritej1->nrj==11 && Spritej2->nrj==11);
    for(int i=0;i<120;++i) sfCoopMovePlayers(1.0f/60);
    assert(sfCoop.shots.size()==2); // Holding or moving the firing finger never repeats.
    a=sfCoopFinger(SDL_FINGERMOTION,1003,.8f,.8f);sfFixHandleEvent(&a);
    assert(sfCoop.controls[0].finger==1001 && sfCoop.controls[1].finger==1002);
    a=sfCoopFinger(SDL_FINGERUP,1003,.8f,.8f);sfFixHandleEvent(&a);
    a=sfCoopFinger(SDL_FINGERDOWN,1003,.4f,.2f);sfFixHandleEvent(&a);
    assert(sfCoop.shots.size()==3 && sfCoop.shots.back().kind==0);
    const auto ordinaryVelocity=sfCoop.shots.back().velocity;
    sfCoop.position.x+=120;sfCoopProjectiles(1.0f/60);
    assert(std::abs(sfCoop.shots.back().velocity.vx-ordinaryVelocity.vx)<.001f);
    assert(std::abs(sfCoop.shots.back().velocity.vy-ordinaryVelocity.vy)<.001f);
    SDL_Event pause{};pause.type=SDL_APP_DIDENTERBACKGROUND;sfFixHandleEvent(&pause);
    assert(!sfCoop.controls[0].down && !sfCoop.controls[1].down);
    std::puts("PASS: two pilots/four fingers, tap-only shots, full-energy missiles and pause cancellation");
}
static void testPassiveCoopTurrets()
{
    setupCampaign();
    for(int i=0;i<180;++i) sfCoopDefences(1.0f/60);
    assert(sfCoop.shots.empty());
    for(const auto &t:sfTurrets) assert(t.deploy==0);
    std::puts("PASS: cooperative turrets remain inactive without a collected bonus");
}

static void testCollectedBonusAndShield()
{
    setupCampaign();sfCoop.bonusTimer=0;sfCoopBonus(.01f);
    assert(sfCoop.bonusLife>0 && sfCoop.turretTime==0);
    sfCoop.bonusPosition=tupl(Spritej2->x,Spritej2->y);sfCoop.bonusVelocity.set(0,0);
    sfCoopBonus(.01f);assert(sfCoop.bonusLife==0 && sfCoop.turretTime==14);
    for(int i=0;i<180;++i) sfCoopDefences(1.0f/60);
    assert(!sfCoop.shots.empty() && Spritej1->nrj==0 && Spritej2->nrj==0);
    assert(sfTurrets[0].energy<100);
    sfCoop.shots.clear();sfCoop.turretTime=.001f;sfCoopDefences(.01f);
    for(int i=0;i<180;++i) sfCoopDefences(1.0f/60);
    assert(sfCoop.shots.empty());
    for(auto *ship:{Spritej1,Spritej2}) ship->pv=1000;
    Spritej1->nrj=0;Spritej2->nrj=50;
    sfCoopHurt(0,100);sfCoopHurt(1,100);
    assert(Spritej1->pv==1000 && Spritej2->pv==900);
    assert(Spritej1->nrj>0 && Spritej2->nrj==50);
    std::puts("PASS: random floating bonus, collection, independent turret energy, expiration and reserve-dependent shield");
}

static void testRealCoopField()
{
    setupCampaign();
    sfCoopTick(1.0f/60);
    assert(!sa1.empty());
    const auto *rock=sa1.back();const auto x=rock->x,y=rock->y;
    sfCoopTick(1.0f/60);
    assert(rock->x!=x || rock->y!=y);
    std::puts("PASS: cooperative tick advances the actual historical asteroid list");
}


static void testCoopDifficultyChain()
{
    setupCampaign();
    float highSpread=0,lowSpread=0;
    for(unsigned i=1;i<=32;++i) {
        highSpread+=std::abs(sfCoopShotSpread(5,i,1));
        lowSpread+=std::abs(sfCoopShotSpread(45,i,1));
    }
    assert(sfCoopFireDelay(45)>sfCoopFireDelay(5) && lowSpread>highSpread*4);
    Spritej2->nrj=5;sfCoop.cooldown[1]=.5f;
    const float heat=Spritej2->nrj;const auto shots=sfCoop.shots.size();
    assert(!sfCoopFire(1) && Spritej2->nrj==heat && sfCoop.shots.size()==shots);

    for(int fps:{30,60,120}) {
        setupCampaign();auto *ship=Spritej1;ship->pv=1000;ship->nrj=0;
        for(int frame=0;frame<fps*2;++frame) sfCoopBossContact(0,1.0f/fps);
        assert(ship->nrj>=49.9f && ship->pv<500);
    }

    setupCampaign();Spritej1->pv=1000;Spritej1->nrj=40;sfCoop.invulnerable[0]=10;
    const float p0=Spritej1->pv;sfCoopAsteroidHurt(0,500);const float p1=Spritej1->pv;
    sfCoopAsteroidHurt(0,500);const float p2=Spritej1->pv;
    assert(p1<p0 && p2<p1 && Spritej1->nrj>40);

    setupCampaign();Spritej1->setxywh(390,300,100,100);Spritej2->setxywh(700,1500,100,100);
    Spritej1->nrj=30;Spritej1->pv=900;
    for(int i=0;i<29;++i) {auto *dust=new parts(390,300);dust->pv=600;particules.push_back(dust);}
    sfCollectDust();
    assert(Spritej1->nrj>25 && Spritej1->pv<=909);
    std::puts("PASS: energy lowers cadence/accuracy, boss contact is continuous, rocks bypass shot i-frames and ore no longer resets the shield");
}

static void testCoopHudAndMissile()
{
    const int width=400,height=800;
    const auto low=sfPlayerPvRect(1,width,height),up=sfPlayerPvRect(0,width,height);
    const auto mirrored=sfMirrorRect180(low,width,height);
    assert(up.x==mirrored.x && up.y==mirrored.y && up.w==mirrored.w && up.h==mirrored.h);
    const auto b0=sfBossLifeRect(false,width,height),b1=sfBossLifeRect(true,width,height);
    const auto bm=sfMirrorRect180(b0,width,height);assert(b1.x==bm.x && b1.y==bm.y);
    const auto full=sfHealthColor(1),empty=sfHealthColor(0);
    assert(full.g>full.r && empty.r>empty.g);

    auto *surface=SDL_CreateRGBSurfaceWithFormat(0,width,height,32,SDL_PIXELFORMAT_RGBA32);
    auto *renderer=SDL_CreateSoftwareRenderer(surface);assert(surface && renderer);
    setupCampaign();sfArenaW=width;sfArenaH=height;
    auto &textures=sfCoopTextures(renderer);assert(textures.missile);
    sfCoop.shots.clear();sfCoopEmit(tupl(200,400),-float(PI)*.5f,240,1,60,4);
    SDL_SetRenderDrawColor(renderer,0,0,0,255);SDL_RenderClear(renderer);sfCoopDrawArena(renderer,width,height);
    std::vector<Uint32> missile(width*height);assert(SDL_RenderReadPixels(renderer,nullptr,SDL_PIXELFORMAT_RGBA32,missile.data(),width*4)==0);
    sfCoop.shots.clear();sfCoopEmit(tupl(200,400),-float(PI)*.5f,240,1,12,0);
    SDL_SetRenderDrawColor(renderer,0,0,0,255);SDL_RenderClear(renderer);sfCoopDrawArena(renderer,width,height);
    std::vector<Uint32> plasma(width*height);assert(SDL_RenderReadPixels(renderer,nullptr,SDL_PIXELFORMAT_RGBA32,plasma.data(),width*4)==0);
    assert(missile!=plasma);
    sfCampaignForgetRenderer(renderer);SDL_DestroyRenderer(renderer);
    renderer=SDL_CreateSoftwareRenderer(surface);assert(renderer && sfCoopTextures(renderer).missile);
    sfCampaignForgetRenderer(renderer);SDL_DestroyRenderer(renderer);SDL_FreeSurface(surface);
    std::puts("PASS: mirrored upper HUD, duplicated boss life gradient and historical missile texture survive renderer recreation");
}
