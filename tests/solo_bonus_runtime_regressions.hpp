#pragma once
// Independent integration checks against the REAL sfCoop runtime included by
// regressions.cpp. No mock ships and no replacement projectile simulation.

static void sfBonusTestSetup(bool boss=true){
    setupCampaign();sfBossDangerIndex=0;
    sfFixResetAsteroidField();
    sfCoop.soloDiagnostic=true;sfCoop.soloFreeAim=true;
    sfCoop.pilots=SfCoopPilotRoster::single(1);
    sfCoop.bossPresent=boss;sfCoop.attack=1000;
    sfCoop.asteroidSpawnTimer=1000;
    sfCoop.controls[1].down=false;sfCoop.controls[1].velocity.set(0,0);
    Spritej2->x=390;Spritej2->y=1300;Spritej2->pv=1000;Spritej2->nrj=20;
    sfCoop.position=tupl(390,400);sfCoop.health=1000;
    sfCoop.soloAimTarget=sfCoop.position;
    sfCoopSoloBonusBeginEncounter();
    sfCoopDisableSoloTurretsAndBonuses();
}

static void sfBonusTestWorld(sfsolo::CoopSoloWorldBridge &world){
    sfsolo::MapOptions options;options.width=54;options.height=160;
    assert(world.create(options,{0,0,780,1680},780));
    std::fill(world.map.tiles.begin(),world.map.tiles.end(),sfsolo::Tile::Empty);
    sfCoop.soloWorldMode=true;sfCoop.soloWorldContext=&world;
    sfCoop.soloWorldWidth=world.worldWidth();sfCoop.soloWorldHeight=world.worldHeight();
    sfCoop.soloWorldCanOccupy=+[](void *c,tupl at,float radius){
        return static_cast<sfsolo::CoopSoloWorldBridge*>(c)->canOccupy({at.x,at.y},radius);
    };
    sfCoop.soloWorldFirstImpact=+[](void *c,tupl from,tupl to,float radius){
        return static_cast<sfsolo::CoopSoloWorldBridge*>(c)->firstWallImpact({from.x,from.y},{to.x,to.y},radius);
    };
    sfCoop.soloWorldPlacePickup=+[](void *c,tupl ship,tupl desired,float radius,tupl &out){
        const auto *maze=static_cast<sfsolo::CoopSoloWorldBridge*>(c);
        auto cells=maze->reachablePickupCells({ship.x,ship.y},radius);
        std::vector<sfsolo::CoopWorldPoint> eligible;
        const sfsolo::CoopWorldPoint boss{sfCoop.position.x,sfCoop.position.y};
        for(const auto &cell:cells)if(maze->firstWallImpact(boss,cell,radius)>=.999f)eligible.push_back(cell);
        sfsolo::CoopWorldPoint p;
        const bool found=maze->selectPickup(eligible,{desired.x,desired.y},{},radius*2+3,p);
        if(found)out=tupl(p.x,p.y);return found;
    };
}

static void testSoloBonusRealManualWeapons(){
    using namespace sfsolo::bonus;
    sfBonusTestSetup();
    auto &s=sfCoop.soloBonus;
    s.grant(Kind::Prison,Spritej2->pv,Spritej2->nrj);
    Spritej2->nrj=20;
    assert(sfCoopFire(1));
    assert(sfCoop.shots.size()==3 && s.charges==2 && Spritej2->nrj==21);
    const float delay=sfCoop.cooldown[1];
    assert(std::abs(delay-sfCoopFireDelay(20))<.0001f);
    assert(!sfCoopFire(1));
    assert(sfCoop.shots.size()==3 && s.charges==2 && Spritej2->nrj==21);
    for(const auto &p:sfCoop.shots){
        assert(p.owner==1 && p.bonusKind==Kind::Prison && p.bonusEncounter==s.encounter);
    }
    // A capacity refusal cannot consume charge, heat or cadence.
    sfCoop.cooldown[1]=0;sfCoop.shots.resize(599);
    assert(!sfCoopFire(1));assert(s.charges==2 && Spritej2->nrj==21 && sfCoop.cooldown[1]==0);
    sfCoop.shots.clear();Spritej2->nrj=0;
    assert(sfCoopFire(1));assert(Spritej2->nrj==11 && s.charges==1);
    sfCoop.shots.clear();sfCoop.cooldown[1]=0;
    s.grant(Kind::Missiles,Spritej2->pv,Spritej2->nrj);
    const float heat=Spritej2->nrj;
    assert(sfCoopFire(1));
    assert(sfCoop.shots.size()==1 && sfCoop.shots.front().kind==4);
    assert(Spritej2->nrj==heat && s.charges==1);
    assert(!sfCoopFire(1) && sfCoop.shots.size()==1);
    sfCoopSoloBonusTick(3);assert(!s.missiles());
    sfCoop.cooldown[1]=0;sfCoop.shots.clear();
    sfCoop.bossPresent=false;
    assert(sfCoopFire(1));assert(s.charges==1 && sfCoop.shots.size()==1);
    assert(sfCoop.shots.front().bonusKind==Kind::None);
    // A simulation frame never supplies a human firing command itself.
    sfCoop.shots.clear();s.grant(Kind::Missiles,Spritej2->pv,Spritej2->nrj);
    for(int i=0;i<20;++i)sfCoopTick(1.0f/60);
    for(const auto &shot:sfCoop.shots)assert(shot.owner!=1);
    assert(!SF_COOP_SOLO_METEOR_WAVES_ENABLED);
}

static void testSoloBonusRealGuidanceAndEffects(){
    using namespace sfsolo::bonus;
    for(Kind kind:{Kind::Prison,Kind::Ice,Kind::Lava,Kind::Meteor}){
        sfBonusTestSetup();
        sfCoop.soloBonus.grant(kind,Spritej2->pv,Spritej2->nrj);
        assert(sfCoopFire(1));assert(sfCoop.shots.size()==3);
        const auto originals=sfCoop.shots;
        // Move the target AFTER launch. Every boosted orb must curve toward
        // the new live target, rather than keep its launch aim point.
        sfCoop.position.x+=180;
        sfCoopProjectiles(.05f);
        assert(sfCoop.shots.size()==3);
        for(std::size_t i=0;i<3;++i){
            assert(std::abs(sfCoop.shots[i].phase-originals[i].phase)>.00001f);
            assert(sfCoop.shots[i].position.y<originals[i].position.y);
        }
        const float hp=sfCoop.health;
        for(int step=0;step<800 && !sfCoop.shots.empty();++step)sfCoopProjectiles(.005f);
        assert(sfCoop.shots.empty());
        const float expected=3*(kind==Kind::Meteor?120.0f:12.0f);
        assert(std::abs((hp-sfCoop.health)-expected)<.02f);
        assert(!sfCoop.soloBonusImpacts.empty());
        if(kind==Kind::Prison)assert(sfCoop.soloBonus.bossSpeedScale()==0);
        if(kind==Kind::Ice)assert(std::abs(sfCoop.soloBonus.bossSpeedScale()-.4f)<.001f);
        if(kind==Kind::Lava){
            const float beforeBurn=sfCoop.health;
            sfCoopSoloBonusTick(1);assert(std::abs(beforeBurn-sfCoop.health-24)<.02f);
            sfCoopSoloBonusTick(4);assert(std::abs(beforeBurn-sfCoop.health-96)<.02f);
        }
        assert(sa1.empty()); // targeted Meteor never creates METEOR-009 rocks
    }
    // A prior encounter's airborne status cannot hit the next boss.
    sfBonusTestSetup();sfCoop.soloBonus.grant(Kind::Lava,Spritej2->pv,Spritej2->nrj);
    assert(sfCoopFire(1));const float hp=sfCoop.health;
    sfCoopSoloBonusBeginEncounter();sfCoopProjectiles(.1f);
    assert(sfCoop.shots.empty() && sfCoop.health==hp);
    sfCoop.cooldown[1]=0;assert(sfCoopFire(1));
    sfCoop.bossPresent=false;sfCoopProjectiles(.1f);assert(sfCoop.shots.empty());
}

static void testSoloBonusStealthKeepsPhysicalDamage(){
    using namespace sfsolo::bonus;
    sfBonusTestSetup();sfCoop.soloBonus.grant(Kind::Stealth,Spritej2->pv,Spritej2->nrj);
    assert(!sfCoopPilotTargetable(1));
    assert(sfCoopSoloShipAlpha(1,255)>0 && sfCoopSoloShipAlpha(1,255)<255);
    sfCoop.bossAggressor=1;sfCoop.bossAggroTime=5;
    sfCoopSoloBonusTick(.1f);assert(sfCoop.bossAggressor==-1);
    // Already emitted guided rounds fly without reacquiring the invisible pilot.
    SfCoopShot tracking;tracking.owner=-1;tracking.kind=1;
    tracking.position=tracking.previous=tupl(Spritej2->x-200,Spritej2->y+100);
    tracking.phase=-float(PI)*.5f;tracking.velocity=tuplv(0,-100);
    sfCoop.shots.push_back(tracking);sfCoopProjectiles(.05f);
    assert(sfCoop.shots.size()==1 && sfCoop.shots.front().phase==tracking.phase);
    sfCoop.shots.clear();Spritej2->nrj=50;sfCoop.invulnerable[1]=0;
    sfCoopEmit(tupl(Spritej2->x,Spritej2->y),0,0,-1,100,0);
    const float hp=Spritej2->pv;sfCoopProjectiles(.001f);
    assert(Spritej2->pv<hp); // invisibility never means invulnerability
    sfCoop.position=tupl(Spritej2->x,Spritej2->y);sfCoop.chargeActive=false;
    Spritej2->nrj=50;const float beforeContact=Spritej2->pv;
    sfCoopBossContact(1,.1f);assert(Spritej2->pv<beforeContact);
    sfCoopSoloBonusTick(4.9f);
    assert(sfCoopPilotTargetable(1) && sfCoopSoloShipAlpha(1,255)==255);
}

static void testSoloBonusStatusClockAndPause(){
    using namespace sfsolo::bonus;
    sfBonusTestSetup();sfCoop.soloBonus.hitBoss(Kind::Ice);sfCoop.soloBonus.hitBoss(Kind::Prison);
    const float start=sfCoop.soloBossClock;
    sfCoopSoloBonusTick(1);assert(sfCoop.soloBossClock==start);
    sfCoopSoloBonusTick(2);assert(std::abs(sfCoop.soloBossClock-start-.4f)<.001f);
    sfCoopSoloBonusTick(2);assert(std::abs(sfCoop.soloBossClock-start-1.8f)<.001f);
    sfCoop.soloBonus.grant(Kind::Missiles,Spritej2->pv,Spritej2->nrj);
    sfCoop.phase=SfCoopPhase::Paused;const auto paused=sfCoop.soloBonus.time;
    sfCoopTick(10);assert(sfCoop.soloBonus.time==paused && sfCoop.soloBonus.missiles());
    sfCoop.phase=SfCoopPhase::Combat;sfCoop.bossPresent=false;
    sfCoopSoloBonusTick(.1f);assert(sfCoop.soloBonus.bossSpeedScale()==1);
    assert(sfCoop.soloBonus.remaining(Kind::Lava)==0);
}

static void testSoloBonusWorldPlacementAndWallImpact(){
    using namespace sfsolo::bonus;
    sfBonusTestSetup();sfsolo::CoopSoloWorldBridge world;sfBonusTestWorld(world);
    const float cell=world.pixelsPerCell;
    // A nearby wall exists, but boss and pilot share the same reachable side:
    // every reward must be both pilot-accessible and direct-line ejectable.
    for(int y=0;y<world.map.height;++y)world.map.tiles[std::size_t(y)*world.map.width+5]=sfsolo::Tile::Rock;
    Spritej2->x=cell*3.5f;Spritej2->y=cell*5.5f;
    sfCoop.position=tupl(cell*4.1f,cell*5.5f);
    sfCoop.health=sfCoopProfile().health*.75f;
    sfCoopSoloBonusCollectAndDrop();
    assert(sfCoop.soloBonus.pickups.size()==3);
    assert(sfCoop.soloBonusEjections.size()==3);
    for(const auto &e:sfCoop.soloBonusEjections){
        assert(std::abs(e.origin.x-sfCoop.position.x)<.001f);
        assert(std::abs(e.origin.y-sfCoop.position.y)<.001f);
        assert(world.firstWallImpact({e.origin.x,e.origin.y},{e.target.x,e.target.y},
                                     sfCoopSoloPickupHull())>=.999f);
        assert(sfCoopSoloBonusPickupInFlight(e.pickupId));
    }
    for(const auto &p:sfCoop.soloBonus.pickups){
        assert(p.x<cell*5);
        assert(world.canOccupy({p.x,p.y},sfCoopShipRadius()));
    }
    sfCoopSoloBonusCollectAndDrop();assert(sfCoop.soloBonus.pickups.size()==3);
    const auto healthPickup=std::find_if(sfCoop.soloBonus.pickups.begin(),sfCoop.soloBonus.pickups.end(),
        [](const auto &p){return p.kind==Kind::Health;});
    assert(healthPickup!=sfCoop.soloBonus.pickups.end());
    Spritej2->x=healthPickup->x;Spritej2->y=healthPickup->y;Spritej2->pv=100;
    sfCoopSoloBonusCollectAndDrop();assert(Spritej2->pv==100); // still flying
    sfCoopSoloBonusTick(.7f);assert(sfCoop.soloBonusEjections.empty());
    sfCoopSoloBonusCollectAndDrop();assert(Spritej2->pv==450);
    // Targeted Meteor cannot hit a boss on the other side of a wall.
    sfCoop.position=tupl(cell*7.5f,cell*5.5f);
    sfCoop.soloBonus.grant(Kind::Meteor,Spritej2->pv,Spritej2->nrj);
    const float hp=sfCoop.health;assert(sfCoopFire(1));
    sfCoopProjectiles(1);
    assert(sfCoop.shots.empty() && sfCoop.health==hp);
    assert(sa1.empty() && !SF_COOP_SOLO_METEOR_WAVES_ENABLED);
    // Normal pilot projectiles use the SAME wall obstruction in SOLO.
    sfCoop.soloBonus.charges=0;sfCoop.cooldown[1]=0;Spritej2->nrj=25;
    sfCoop.soloAimTarget=sfCoop.position;assert(sfCoopFire(1));
    sfCoopProjectiles(1);assert(sfCoop.shots.empty() && sfCoop.health==hp);
    // Death drops are distributed before the diagnostic boss is deactivated.
    sfCoop.health=0;sfCoopSoloBonusCollectAndDrop();
    assert(sfCoop.soloBonus.dropped==15);
    const auto total=sfCoop.soloBonus.pickups.size()+sfCoop.soloBonusPending.size();
    Spritej2->x=cell*.5f;Spritej2->y=cell*.5f; // avoid collecting freshly dropped loot in this assertion
    sfCoopSoloBonusCollectAndDrop();
    assert(sfCoop.soloBonus.pickups.size()+sfCoop.soloBonusPending.size()==total);
    setupCampaign(); // clear stack-owned world callback before destruction
}

static void testSoloBonusDiagnosticQueueAndEjection(){
    using namespace sfsolo::bonus;
    sfBonusTestSetup();
    assert(sfCoopSoloBonusQueueAllTests()==8);
    assert(sfCoop.soloBonusTestBurst==8);
    assert(sfCoop.soloBonusPending.size()==8);
    const double before=sfCoop.soloBonus.time;
    sfCoopSoloBonusCollectAndDrop();
    assert(sfCoop.soloBonus.pickups.size()==8);
    assert(sfCoop.soloBonusEjections.size()==8);
    assert(sfCoop.soloBonusPending.empty() && sfCoop.soloBonusTestBurst==0);
    std::array<bool,8> kinds{};
    for(const auto &e:sfCoop.soloBonusEjections){
        assert(std::abs(e.origin.x-sfCoop.position.x)<.001f);
        assert(std::abs(e.origin.y-sfCoop.position.y)<.001f);
        assert(sfCoopSoloBonusPickupInFlight(e.pickupId));
        assert(std::abs(e.started-before)<.0001);
        kinds[unsigned(e.kind)]=true;
    }
    for(bool value:kinds)assert(value); // all eight kinds in the SAME batch
    sfCoopSoloBonusTick(.8f);assert(sfCoop.soloBonusEjections.empty());
    sfCoop.bossPresent=false;
    assert(!sfCoopSoloBonusQueueTest(Kind::Energy));
    assert(sfCoopSoloBonusQueueAllTests()==0);
}

static void testSoloBonusIsolationAndReset(){
    using namespace sfsolo::bonus;
    sfBonusTestSetup();sfCoop.soloBonus.grant(Kind::Missiles,Spritej2->pv,Spritej2->nrj);
    sfCoop.soloBonus.grant(Kind::Stealth,Spritej2->pv,Spritej2->nrj);
    sfCoop.soloBonus.grant(Kind::Meteor,Spritej2->pv,Spritej2->nrj);
    setupCampaign();
    assert(!sfCoopSoloBonusesEnabled());
    assert(sfCoop.soloBonus.time==0 && !sfCoop.soloBonus.missiles() && !sfCoop.soloBonus.stealth());
    assert(sfCoop.soloBonus.charges==0 && sfCoop.soloBonus.pickups.empty());
    // Even stale bonus data cannot grant powers to historical COOP.
    sfCoop.soloBonus.grant(Kind::Missiles,Spritej2->pv,Spritej2->nrj);
    sfCoop.soloBonus.grant(Kind::Stealth,Spritej2->pv,Spritej2->nrj);
    Spritej2->nrj=20;assert(sfCoopFire(1));
    assert(Spritej2->nrj==21 && sfCoop.shots.size()==1 && sfCoop.shots.front().kind==0);
    assert(sfCoopPilotTargetable(1) && sfCoopSoloShipAlpha(1,255)==255);
    sfCoop.bonusTimer=-1;sfCoopBonus(.01f);assert(sfCoop.bonusLife>0);
    for(bool world:{false,true})for(bool boss:{false,true}){
        sfBonusTestSetup(boss);sfCoop.soloWorldMode=world;
        sfCoop.turretTime=14;sfCoop.bonusLife=18;sfCoop.bonusTimer=-1;
        sfCoopDisableSoloTurretsAndBonuses();sfCoopDefences(1);sfCoopBonus(1);
        assert(sfCoop.turretTime==0 && sfCoop.bonusLife==0);
        assert(!sfCoopShouldDrawHistoricalTactics());
        for(const auto &t:sfTurrets)assert(!t.alert && t.deploy==0);
        sfCoopSoloMeteorites(100);assert(sa1.empty());
    }
    setupCampaign();
}

static void testSoloBonusDamageBeforeHealingAndDeath(){
    sfBonusTestSetup();
    const float threshold=sfCoopProfile().health*.75f;
    sfCoop.health=threshold+1;
    for(int i=0;i<12;++i){auto *dust=new parts(sfCoop.position.x,sfCoop.position.y);dust->pv=600;particules.push_back(dust);}
    sfCoopEmit(sfCoop.position,0,0,1,2,0);sfCoopProjectiles(.001f);
    assert(sfCoop.health>threshold); // healed in the SAME simulation step
    assert(sfCoop.soloBonus.dropped==1 && sfCoop.soloBonusPending.size()==3);
    sfCoop.health=1;
    auto *dust=new parts(sfCoop.position.x,sfCoop.position.y);dust->pv=600;particules.push_back(dust);
    sfCoopEmit(sfCoop.position,0,0,1,12,0);sfCoopProjectiles(.001f);
    assert(sfCoop.health==0 && sfCoop.soloBonus.dropped==15);
    sfCoopTick(.001f);assert(!sfCoop.bossPresent);
}
static void testSoloBonusActualMovingBoss(){
    using sfsolo::bonus::Kind;
    sfBonusTestSetup();sfCoopTick(.01f);
    sfCoop.soloBonus.hitBoss(Kind::Prison);
    const auto before=sfCoop.position;
    for(int i=0;i<100;++i)sfCoopTick(.01f);
    assert(std::hypot(sfCoop.position.x-before.x,sfCoop.position.y-before.y)<.001f);
    sfCoop.soloBonus.clearBoss();sfCoop.soloBonus.hitBoss(Kind::Ice);
    const float clock=sfCoop.soloBossClock;sfCoopTick(.25f);
    assert(std::abs(sfCoop.soloBossClock-clock-.1f)<.001f);
    assert(std::hypot(sfCoop.position.x-before.x,sfCoop.position.y-before.y)>.001f);
}
static void testSoloBonusRuntime012(){
    testSoloBonusRealManualWeapons();
    testSoloBonusRealGuidanceAndEffects();
    testSoloBonusStealthKeepsPhysicalDamage();
    testSoloBonusStatusClockAndPause();
    testSoloBonusWorldPlacementAndWallImpact();
    testSoloBonusDamageBeforeHealingAndDeath();
    testSoloBonusActualMovingBoss();
    testSoloBonusDiagnosticQueueAndEjection();
    testSoloBonusIsolationAndReset();
    std::puts("PASS: SOLO BONUS CORE-012 real COOP firing, homing, statuses, contacts, walls, loot and historical isolation");
}

static void testSoloBonusRendering012(){
    using namespace sfsolo::bonus;
    sfBonusTestSetup();
    SDL_Surface *surface=SDL_CreateRGBSurfaceWithFormat(0,780,1680,32,SDL_PIXELFORMAT_RGBA32);assert(surface);
    SDL_Renderer *renderer=SDL_CreateSoftwareRenderer(surface);assert(renderer);
    {
        SfCoopSoloBonusRenderer drawing(renderer);assert(drawing.atlas);
        assert(drawing.animationAtlas);
        for(auto *texture:drawing.icons)assert(texture);
        for(unsigned row=0;row<4;++row){
            const auto kind=Kind(unsigned(Kind::Prison)+row);
            assert(SfCoopSoloBonusRenderer::productionAnimationRow(kind)==int(row));
            for(int frame=0;frame<8;++frame){
                const auto crop=SfCoopSoloBonusRenderer::productionAnimationCrop(kind,frame);
                assert(crop.x==frame*192 && crop.y==int(row)*192 && crop.w==192 && crop.h==192);
            }
        }
        for(unsigned i=0;i<8;++i)
            assert(sfCoop.soloBonus.addPickup(Kind(i),100+(i%4)*190,350+(i/4)*220,24));
        sfCoop.soloBonus.grant(Kind::Stealth,Spritej2->pv,Spritej2->nrj);
        sfCoop.soloBonus.grant(Kind::Missiles,Spritej2->pv,Spritej2->nrj);
        sfCoop.soloBonus.grant(Kind::Meteor,Spritej2->pv,Spritej2->nrj);
        sfCoop.position=tupl(390,850);sfCoop.soloBonus.hitBoss(Kind::Ice);
        sfCoopDrawArena(renderer,780,1680);drawing.draw(nullptr,780,1680);
        SDL_RenderFlush(renderer);
        // Each actual icon must contribute a substantial visible footprint.
        for(unsigned i=0;i<8;++i){
            const int x=100+(i%4)*190,y=350+(i/4)*220;unsigned colored=0;
            for(int py=y-25;py<y+25;++py)for(int px=x-25;px<x+25;++px){
                const auto pixel=*(reinterpret_cast<const Uint32*>(static_cast<const Uint8*>(surface->pixels)+py*surface->pitch)+px);
                Uint8 r,g,b,a;SDL_GetRGBA(pixel,surface->format,&r,&g,&b,&a);
                if(std::max({r,g,b})>85)++colored;
            }
            assert(colored>80);
        }
        if(const char *preview=std::getenv("SF_SOLO_BONUS_PREVIEW"))assert(IMG_SavePNG(surface,preview)==0);
    }
    sfCampaignForgetRenderer(renderer);SDL_DestroyRenderer(renderer);SDL_FreeSurface(surface);
    setupCampaign();
    std::puts("PASS: eight SOLO bonus sprites plus 4x8 production animation atlas rendered by real SDL2");
}
