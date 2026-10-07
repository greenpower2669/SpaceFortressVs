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
    assert(sfCoop.shots.empty());
    a=sfCoopFinger(SDL_FINGERUP,1003,.4f,.2f);
    b=sfCoopFinger(SDL_FINGERUP,1004,.6f,.8f);
    sfFixHandleEvent(&a);sfFixHandleEvent(&b);
    assert(sfCoop.shots.size()==2);
    assert(sfCoop.shots[0].kind==4 && sfCoop.shots[1].kind==4);
    assert(Spritej1->nrj==11 && Spritej2->nrj==11);
    for(int i=0;i<120;++i) sfCoopMovePlayers(1.0f/60);
    assert(sfCoop.shots.size()==2);
    a=sfCoopFinger(SDL_FINGERDOWN,1003,.4f,.2f);sfFixHandleEvent(&a);
    a=sfCoopFinger(SDL_FINGERMOTION,1003,.8f,.8f);sfFixHandleEvent(&a);
    assert(sfCoop.controls[0].finger==1001 && sfCoop.controls[1].finger==1002);
    a=sfCoopFinger(SDL_FINGERUP,1003,.8f,.8f);sfFixHandleEvent(&a);
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

static void testSharedLinearShieldModel()
{
    struct Case { float heat,spent,wear; };
    const Case cases[]={{0,0,.1f},{5,.1f,.2f},{10,.2f,.3f},{25,.5f,.6f},{45,.9f,1.0f},{50,1.0f,1.0f}};
    for(const auto &c:cases) {
        assert(std::abs(sfShieldFraction(c.heat)-(1.0f-c.spent))<.0001f);
        assert(std::abs(sfShieldDamage(10.0f,c.heat)-10.0f*c.spent)<.001f);
        assert(std::abs(sfShieldWear(10.0f,c.heat)-10.0f*c.wear)<.001f);
        sprite ship;ship.nrj=c.heat;
        const float hull=sfApplyShieldImpact(&ship,10.0f);
        assert(std::abs(hull-10.0f*c.spent)<.001f);
        assert(std::abs(ship.nrj-std::min(50.0f,c.heat+10.0f*c.wear))<.001f);
    }
    setupTactics();Spritej1->nrj=30;Spritej1->pv=900;
    for(int i=0;i<31;++i) {auto *dust=new parts(Spritej1->x,Spritej1->y);dust->pv=600;particules.push_back(dust);}
    sfCollectDust();
    assert(Spritej1->nrj==0 && std::abs(Spritej1->pv-904.0f)<.02f);
    std::puts("PASS: linear shield and white dust share one core across duel and coop");
}

static void testKineticFieldAndHullRegen()
{
    sprite regen;regen.pv=900;regen.nrj=0;sfRegenerateHull(&regen,1);const float full=regen.pv;
    regen.pv=900;regen.nrj=25;sfRegenerateHull(&regen,1);const float half=regen.pv;
    regen.pv=900;regen.nrj=50;sfRegenerateHull(&regen,1);const float empty=regen.pv;
    assert(full>half && half>empty && empty==900);

    setupCampaign();sfFixResetAsteroidField();sfFieldRemainder=0;
    Spritej1->setxywh(390,500,100,100);Spritej1->sw=Spritej1->sh=100;Spritej1->startup();
    Spritej2->setxywh(700,1500,100,100);Spritej1->pv=1000;Spritej1->nrj=0;sfObserved[0].velocity.set(0,0);
    const float outer=100*SF_KINETIC_MAX_SHIELD_DIAMETER*.5f;
    auto *fast=new sprite;fast->setv(390,500-outer-16,20,20,0,22,1);fast->w=fast->h=fast->sw=fast->sh=20;fast->pv=1;sa1.push_back(fast);
    for(int i=0;i<7;++i){auto *d=new sprite;d->setv(40+i*95,80,10,10,0,0,1);d->w=d->h=d->sw=d->sh=10;d->pv=1;sa1.push_back(d);}
    sfLegacyFieldStep(sfCoopAsteroidHurt);
    assert(Spritej1->pv==1000 && Spritej1->nrj>0 && !sfKineticWaves.empty() && !particulesr.empty());
    bool fragmented=false;for(auto *r:sa1) if(r->kineticStage>=1 && r->w<20) fragmented=true;assert(fragmented);

    setupCampaign();sfFixResetAsteroidField();sfFieldRemainder=0;
    Spritej1->setxywh(390,500,100,100);Spritej1->sw=Spritej1->sh=100;Spritej1->startup();Spritej1->pv=1000;Spritej1->nrj=50;
    auto *slow=new sprite;slow->setv(390,500,20,20,0,0,1);slow->w=slow->h=slow->sw=slow->sh=20;slow->kineticStage=2;slow->pv=1;sa1.push_back(slow);
    for(int i=0;i<7;++i){auto *d=new sprite;d->setv(40+i*95,80,10,10,0,0,1);d->w=d->h=d->sw=d->sh=10;d->pv=1;sa1.push_back(d);}
    sfLegacyFieldStep(sfCoopAsteroidHurt);assert(Spritej1->pv==1000);

    setupTactics();sfFixResetAsteroidField();sfFieldRemainder=0;
    Spritej1->setxywh(390,500,100,100);Spritej1->sw=Spritej1->sh=100;Spritej1->startup();Spritej1->pv=1000;Spritej1->nrj=50;
    auto *classic=new sprite;classic->setv(390,500,20,20,0,0,1);classic->w=classic->h=classic->sw=classic->sh=20;classic->kineticStage=2;classic->pv=1;sa1.push_back(classic);
    for(int i=0;i<7;++i){auto *d=new sprite;d->setv(40+i*95,80,10,10,0,0,1);d->w=d->h=d->sw=d->sh=10;d->pv=1;sa1.push_back(d);}
    sfLegacyFieldStep(nullptr);assert(Spritej1->pv==1000);

    setupCampaign();Spritej1->pv=3000;Spritej1->nrj=0;Spritej1->setxywh(390,650,100,100);Spritej1->sw=Spritej1->sh=100;Spritej1->startup();
    sfCoop.position=tupl(390,420);sfCoop.motion.valid=true;sfCoop.motion.velocity.set(0,sfArenaW*1.2f);sfObserved[0].velocity.set(0,0);
    sfCoop.chargeActive=true;sfCoop.chargeTime=.30f;sfCoop.chargeHit={false,false};
    sfCoopBossContact(0,1.0f/60);const float afterHeat=Spritej1->nrj,afterPv=Spritej1->pv;
    assert(sfCoop.chargeHit[0] && afterHeat>0 && !sfKineticWaves.empty());
    sfCoopBossContact(0,1.0f/60);assert(Spritej1->nrj==afterHeat && Spritej1->pv==afterPv);

    sfKineticResetSurges();sfKineticSurgePress(0);
    sfKineticAdvanceSurges(SF_KINETIC_SURGE_HOLD_SECONDS-.01f);
    assert(sfKineticSurgeVisible(0) && sfKineticSurgePower(0)==2.0f && !sfKineticSurgeVulnerable(0));
    sfKineticAdvanceSurges(.02f);
    assert(!sfKineticSurgeVisible(0) && sfKineticSurgePower(0)==0.0f && sfKineticSurgeVulnerable(0));
    assert(sfKineticSurgeRelease(0));

    setupTactics();sfFixResetAsteroidField();sfFieldRemainder=0;
    Spritej2->setxywh(390,1200,100,100);Spritej2->sw=Spritej2->sh=100;Spritej2->startup();
    auto *nearRock=new sprite;nearRock->setv(470,1200,20,20,0,0,1);nearRock->w=nearRock->h=nearRock->sw=nearRock->sh=20;nearRock->pv=1;sa1.push_back(nearRock);
    auto *farRock=new sprite;farRock->setv(650,1200,20,20,0,0,1);farRock->w=farRock->h=farRock->sw=farRock->sh=20;farRock->pv=1;sa1.push_back(farRock);
    auto *white=new parts(410,1200);white->pv=600;white->vx=7;white->vy=-3;particules.push_back(white);
    const float whiteVx=white->vx,whiteVy=white->vy;
    const int purged=sfKineticPurgeAsteroids(1);
    assert(purged==1 && nearRock->pv==0 && farRock->pv>0);
    assert(white->pv==600 && white->vx==whiteVx && white->vy==whiteVy);

    std::puts("PASS: shared kinetic layers, surge, purge and reserve-driven hull regeneration");
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
    sfCoopHurt(0,10);sfCoopHurt(1,10);
    assert(Spritej1->pv==1000 && Spritej2->pv==900);
    assert(std::abs(Spritej1->nrj-10.0f)<.01f && Spritej2->nrj==50);
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

    std::array<float,3> contactLoss{};int contactIndex=0;
    for(int fps:{30,60,120}) {
        setupCampaign();auto *ship=Spritej1;ship->pv=5000;ship->nrj=0;
        const float dt=1.0f/fps;
        for(int frame=0;frame<fps/2;++frame) sfCoopBossContact(0,dt);
        contactLoss[contactIndex++]=5000.0f-ship->pv;
        assert(ship->pv>0 && ship->nrj==50);
    }
    assert(std::abs(contactLoss[0]-contactLoss[1])<.1f && std::abs(contactLoss[1]-contactLoss[2])<.1f);
    setupCampaign();Spritej1->pv=1000;Spritej1->nrj=0;
    int deathFrame=-1;
    for(int frame=0;frame<120;++frame) {
        sfCoopBossContact(0,1.0f/60);
        if(Spritej1->pv<=0) {deathFrame=frame;break;}
    }
    assert(deathFrame>=0 && deathFrame<119);

    setupCampaign();Spritej1->pv=1000;Spritej1->nrj=40;sfCoop.invulnerable[0]=10;
    const float p0=Spritej1->pv;sfCoopAsteroidHurt(0,20);const float p1=Spritej1->pv;
    sfCoopAsteroidHurt(0,20);const float p2=Spritej1->pv;
    assert(p1<p0 && p2<p1 && Spritej1->nrj>40);
    setupCampaign();Spritej1->pv=100;Spritej1->nrj=50;
    sfCoopAsteroidHurt(0,100);assert(Spritej1->pv==0);

    setupCampaign();Spritej1->setxywh(390,300,100,100);Spritej2->setxywh(700,1500,100,100);
    Spritej1->nrj=30;Spritej1->pv=900;
    for(int i=0;i<31;++i) {auto *dust=new parts(390,300);dust->pv=600;particules.push_back(dust);}
    sfCollectDust();
    assert(Spritej1->nrj==0 && std::abs(Spritej1->pv-904.0f)<.02f);
    std::puts("PASS: energy lowers cadence/accuracy, boss contact is continuous, rocks bypass shot i-frames and ore no longer resets the shield");
}

static void testCoopIncomingDamageMultiplier()
{
    sfBossDangerIndex=2;
    assert(sfBossDangerMultiplier()==10.0f);
    for(bool ai:{false,true}) for(int owner=0;owner<2;++owner) for(float heat:{0.0f,25.0f,50.0f}) {
        setupCampaign(0,ai);
        auto *ship=sfCoopShip(owner);auto *other=sfCoopShip(1-owner);
        ship->pv=3000;ship->nrj=heat;other->pv=1000;other->nrj=0;
        if(owner==0) {
            ship->setxywh(200,350,100,100);other->setxywh(580,1330,100,100);
        } else {
            ship->setxywh(580,1330,100,100);other->setxywh(200,350,100,100);
        }
        sfCoop.shots.clear();sfCoop.beams.clear();sfCoop.waves.clear();
        sfCoop.invulnerable[owner]=0;
        const float expected=sfBossDangerMultiplier()*sfShieldDamage(100,heat);
        sfCoopEmit(tupl(ship->x,ship->y),0,0,-1,100);
        sfCoopProjectiles(.01f);
        assert(std::abs((3000.0f-ship->pv)-expected)<.01f);
        assert(other->pv==1000);
    }

    setupCampaign();Spritej1->pv=2000;Spritej1->nrj=50;Spritej2->setxywh(700,1500,100,100);
    Spritej1->setxywh(300,500,100,100);sfCoop.invulnerable[0]=0;
    sfCoop.shots.clear();sfCoop.waves.clear();
    const float beamDamage=sfCoopProfile().damage*1.6f;
    sfCoop.beams={{tupl(100,500),0,0,0}};
    sfCoopProjectiles(.01f);
    assert(std::abs((2000.0f-Spritej1->pv)-sfBossDangerMultiplier()*beamDamage)<.01f);

    setupCampaign();Spritej1->pv=2000;Spritej1->nrj=50;Spritej2->setxywh(700,1500,100,100);
    Spritej1->setxywh(300,500,100,100);sfCoop.invulnerable[0]=0;
    sfCoop.shots.clear();sfCoop.beams.clear();
    const float waveDamage=sfCoopProfile().damage*1.3f;
    sfCoop.waves={{tupl(300,500),.7f,0}};
    sfCoopProjectiles(.01f);
    assert(std::abs((2000.0f-Spritej1->pv)-sfBossDangerMultiplier()*waveDamage)<.01f);

    setupCampaign();sfFixResetAsteroidField();sfFieldRemainder=0;
    Spritej1->setxywh(390,400,100,100);Spritej2->setxywh(700,1500,100,100);
    Spritej1->pv=1000;Spritej1->nrj=25;
    auto *impact=new sprite;impact->setv(390,390,20,20,0,0,1);impact->vx=0;impact->vy=10;impact->kineticStage=2;impact->pv=1;sa1.push_back(impact);
    for(int i=0;i<7;++i) {
        auto *dummy=new sprite;dummy->setv(40+i*95,80,10,10,0,0,1);dummy->pv=1;sa1.push_back(dummy);
    }
    const float asteroidExpected=sfShieldDamage(sfKineticRockSolution(impact,Spritej1,0).rawDamage,25);
    // Exercise the real field collision callback before sfCoopResources()
    // performs its intentional dust pickup/recovery pass, which would mask the
    // exact impact-only PV/heat delta we are measuring here.
    sfLegacyFieldStep(sfCoopAsteroidHurt);
    assert(std::abs((1000.0f-Spritej1->pv)-asteroidExpected)<.01f && Spritej1->nrj>25);

    // Boss danger changes only the validated NON-KINETIC coop attack/contact paths.
    assert(sfBossDangerMultiplier()==10.0f);
    sfBossDangerAdjust(1);assert(sfBossDangerMultiplier()==15.0f);
    sfBossDangerAdjust(1);assert(sfBossDangerMultiplier()==20.0f);
    sfBossDangerAdjust(-1);assert(sfBossDangerMultiplier()==15.0f);
    sfBossDangerIndex=2;assert(sfBossDangerMultiplier()==10.0f);

    // A wounded boss may recover historical white dust itself.
    setupCampaign();
    const float bossMax=sfCoopProfile().health;
    sfCoop.health=bossMax-100;
    auto *bossDust=new parts(sfCoop.position.x,sfCoop.position.y);bossDust->pv=600;particules.push_back(bossDust);
    const float bossBefore=sfCoop.health;
    sfCoopProjectiles(0);
    assert(bossDust->pv==0 && sfCoop.health>bossBefore && sfCoop.health<=bossMax);

    // Only selected boss ammunition families 1/2/3 may pick white dust up.
    for(int kind:{1,2,3}) {
        setupCampaign();
        const float maxHealth=sfCoopProfile().health;sfCoop.health=maxHealth-100;
        Spritej1->setxywh(sfArenaW*.85f,sfArenaH*.15f,100,100);
        Spritej2->setxywh(sfArenaW*.85f,sfArenaH*.85f,100,100);
        auto *dust=new parts(80,80);dust->pv=600;particules.push_back(dust);
        sfCoopEmit(tupl(80,80),0,0,-1,10,kind);
        const float before=sfCoop.health;sfCoopProjectiles(0);
        assert(dust->pv==0 && sfCoop.health>before);
    }
    setupCampaign();
    sfCoop.health=sfCoopProfile().health-100;
    Spritej1->setxywh(sfArenaW*.85f,sfArenaH*.15f,100,100);
    Spritej2->setxywh(sfArenaW*.85f,sfArenaH*.85f,100,100);
    auto *ordinaryDust=new parts(80,80);ordinaryDust->pv=600;particules.push_back(ordinaryDust);
    sfCoopEmit(tupl(80,80),0,0,-1,10,0);
    const float ordinaryBefore=sfCoop.health;sfCoopProjectiles(0);
    assert(ordinaryDust->pv>0 && sfCoop.health==ordinaryBefore);

    // A boss hit makes red dust. Red dust is strictly visual: no PV, reserve or shield mutation.
    setupCampaign();Spritej1->pv=3000;Spritej1->nrj=50;sfCoop.invulnerable[0]=0;
    const auto redBefore=particulesr.size();sfCoopHurt(0,10);
    assert(particulesr.size()>redBefore);
    setupCampaign();sfFieldRemainder=-1;
    Spritej1->pv=900;Spritej1->nrj=10;
    auto *red=new parts(Spritej1->x,Spritej1->y);red->pv=550;particulesr.push_back(red);
    const float pvBeforeRed=Spritej1->pv,heatBeforeRed=Spritej1->nrj;
    sfCoopResources(0);
    assert(Spritej1->pv==pvBeforeRed && Spritej1->nrj==heatBeforeRed && red->pv>0);

    std::puts("PASS: coop boss danger is configurable x1/x5/x10/x15/x20 while kinetic and dust economy stay separate");
}

static void testCoopHudAndMissile()
{
    const int width=400,height=800;
    const auto low=sfPlayerPvRect(1,width,height),up=sfPlayerPvRect(0,width,height);
    const auto mirrored=sfMirrorRect180(low,width,height);
    assert(up.x==mirrored.x && up.y==mirrored.y && up.w==mirrored.w && up.h==mirrored.h);

    // Fab correction: pilot HUD stays the historical two-bar block (PV + energy).
    const auto lowEnergy=sfPlayerEnergyRect(1,width,height),upEnergy=sfPlayerEnergyRect(0,width,height);
    const auto mirroredEnergy=sfMirrorRect180(lowEnergy,width,height);
    assert(lowEnergy.y>low.y);
    assert(upEnergy.x==mirroredEnergy.x && upEnergy.y==mirroredEnergy.y);

    // Fab final geometry: a normal compact boss HUD for the lower player at
    // bottom-left, plus the exact same object rotated 180 degrees top-right.
    const auto life0=sfBossLifeRect(false,width,height),life1=sfBossLifeRect(true,width,height);
    const auto energy0=sfBossEnergyRect(false,width,height),energy1=sfBossEnergyRect(true,width,height);
    const auto kinetic0=sfBossKineticRect(false,width,height),kinetic1=sfBossKineticRect(true,width,height);
    assert(energy0.y<life0.y && life0.y<kinetic0.y);
    assert(life0.w<=int(width*.30f) && life0.x<width/2);
    assert(life0.y>height/2);
    assert(kinetic0.y-energy0.y<=life0.h*3+8);
    assert(life0.h<=std::max(6,width/90));
    const auto lifeMirror=sfMirrorRect180(life0,width,height);
    const auto energyMirror=sfMirrorRect180(energy0,width,height);
    const auto kineticMirror=sfMirrorRect180(kinetic0,width,height);
    assert(life1.x==lifeMirror.x && life1.y==lifeMirror.y && life1.w==lifeMirror.w && life1.h==lifeMirror.h);
    assert(energy1.x==energyMirror.x && energy1.y==energyMirror.y);
    assert(kinetic1.x==kineticMirror.x && kinetic1.y==kineticMirror.y);

    // Filling direction is part of the 180° rotation: lower grows left->right,
    // upper grows right->left and must be the geometric mirror of the lower fill.
    const auto lowerHalf=sfBossHudValueRect(life0,.5f,false);
    const auto upperHalf=sfBossHudValueRect(life1,.5f,true);
    const auto lowerHalfMirror=sfMirrorRect180(lowerHalf,width,height);
    assert(upperHalf.x==lowerHalfMirror.x && upperHalf.y==lowerHalfMirror.y &&
           upperHalf.w==lowerHalfMirror.w && upperHalf.h==lowerHalfMirror.h);

    const auto full=sfHealthColor(1),empty=sfHealthColor(0);
    assert(full.g>full.r && empty.r>empty.g);
    const auto bossEnergyColor=sfBossEnergyColor(),bossKineticColor=sfBossKineticColor();
    assert(bossEnergyColor.b>bossEnergyColor.r && bossEnergyColor.r>bossEnergyColor.g);
    assert(bossKineticColor.r>bossKineticColor.g && bossKineticColor.g>bossKineticColor.b);
    assert(sfBossHudBackgroundAlpha()<160 && sfBossHudFillAlpha()<230);

    // The whole label is mirrored too: normal above the lower block, rotated
    // counterpart below the upper block.
    const auto bossLabel0=sfBossHudLabelRect(false,width,height);
    const auto bossLabel1=sfBossHudLabelRect(true,width,height);
    const auto bossLabelMirror=sfMirrorRect180(bossLabel0,width,height);
    assert(bossLabel0.x==energy0.x && bossLabel0.w==energy0.w && bossLabel0.y<energy0.y);
    assert(bossLabel1.x==bossLabelMirror.x && bossLabel1.y==bossLabelMirror.y);
    assert(sfBossHudMirrorAlpha()>0 && sfBossHudMirrorAlpha()<sfBossHudFillAlpha());
    const auto mirrorTop=sfBossHudMirrorBand(life0,false);
    const auto mirrorBottom=sfBossHudMirrorBand(life0,true);
    assert(mirrorTop.x==life0.x && mirrorTop.w==life0.w);
    assert(mirrorBottom.x==life0.x && mirrorBottom.w==life0.w);
    assert(mirrorTop.h==mirrorBottom.h && mirrorTop.h>=1);

    // Fab canon: boss is handicapped in MOU DU GENOU and increasingly favoured
    // by the 9 Danger levels. APOCALYPSE regenerates exactly 9x faster than MOU.
    const float mou=sfCoopBossReserveRegenPerSecond(0);
    const float apocalypse=sfCoopBossReserveRegenPerSecond(SF_BOSS_DANGER_COUNT-1);
    assert(mou>0 && std::abs(apocalypse/mou-9.0f)<.001f);
    float previous=mou;
    for(int danger=1;danger<SF_BOSS_DANGER_COUNT;++danger) {
        const float current=sfCoopBossReserveRegenPerSecond(danger);
        assert(current>previous);previous=current;
    }
    assert(sfCoopBossKineticVisibilityAlpha(0.05f)>sfCoopBossKineticVisibilityAlpha(0.95f));
    const float mouReserve=sfCoopRegenerateBossReserveValue(.25f,1.0f,0);
    const float apocalypseReserve=sfCoopRegenerateBossReserveValue(.25f,1.0f,SF_BOSS_DANGER_COUNT-1);
    assert(apocalypseReserve>mouReserve && mouReserve>.25f);

    // Actual runtime regen must use Boss Danger, not campaign difficulty.
    const int savedDanger=sfBossDangerIndex;
    sfCoop.encounter=0;sfCoop.bossEnergyReserve=sfCoop.bossKineticReserve=.25f;
    sfBossDangerIndex=0;sfCoopRegenerateBossReserves(1.0f);const float runtimeMou=sfCoop.bossEnergyReserve;
    sfCoop.bossEnergyReserve=sfCoop.bossKineticReserve=.25f;
    sfBossDangerIndex=SF_BOSS_DANGER_COUNT-1;sfCoopRegenerateBossReserves(1.0f);const float runtimeApocalypse=sfCoop.bossEnergyReserve;
    sfBossDangerIndex=savedDanger;
    assert(runtimeApocalypse>runtimeMou);

    // Half-texel inset remains the bounded anti-bleeding fix.
    const SDL_Rect atlasCell{10,20,100,80};
    const auto uv00=sfAtlasSafeUv(atlasCell,1000,800,0,0);
    const auto uv11=sfAtlasSafeUv(atlasCell,1000,800,1,1);
    assert(std::abs(uv00.x-10.5f/1000.0f)<1e-6f);
    assert(std::abs(uv00.y-20.5f/800.0f)<1e-6f);
    assert(std::abs(uv11.x-109.5f/1000.0f)<1e-6f);
    assert(std::abs(uv11.y-99.5f/800.0f)<1e-6f);

    auto *surface=SDL_CreateRGBSurfaceWithFormat(0,width,height,32,SDL_PIXELFORMAT_RGBA32);
    auto *renderer=SDL_CreateSoftwareRenderer(surface);assert(surface && renderer);
    setupCampaign();sfArenaW=width;sfArenaH=height;
    auto &textures=sfCoopTextures(renderer);
    assert(textures.missile && textures.explosion && textures.orbOrange && textures.orbBlue);
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
    std::puts("PASS: pilot two-bar HUD, boss energy/life/kinetic HUD and historical missile texture survive renderer recreation");
}
