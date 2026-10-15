#pragma once
// Production must not retain the developer's hidden reward commands.
static void testSoloProductionRejectsDiagnosticRewards(){
    sfBonusTestSetup();sfCoop.soloDiagnostic=false;sfCoop.soloWorldMode=true;
    assert(!sfCoopSoloBonusQueueTest(sfsolo::bonus::Kind::Energy));
    assert(sfCoopSoloBonusQueueAllTests()==0);
    assert(sfCoop.soloBonusPending.empty());
}
static void testSoloSelectorFifthEntry(){
    sfSelectedMode=SF_COOP_AI;
    sfFixRequestedScreen.store(SF_UI_HOME);
    SDL_Event e{};e.type=SDL_FINGERDOWN;e.tfinger.fingerId=999;
    e.tfinger.x=.5f;e.tfinger.y=.45f;
    sfFixHandleEvent(&e);
    assert(sfSelectedMode==4); // historical 0..3 IDs retained, SOLO added last
    assert(!sfModeHasAi(sfSelectedMode));
    sfFixConsumedFingers.clear();
}
static void testSoloProductionPolicyAndMiniRewards(){
    using sfsolo::bonus::Kind;
    for(auto role:{SfSoloEncounterRole::None,SfSoloEncounterRole::MiniBoss}){
        sfBonusTestSetup();sfCoop.soloGame=true;sfCoop.soloDiagnostic=false;
        sfCoop.soloEncounterRole=role;
        for(float fraction:{.75f,.5f,.25f,0.0f}){
            sfCoop.health=sfCoopProfile().health*fraction;
            sfCoopSoloBonusObserveBossHealth();sfCoopSoloBonusCollectAndDrop();
            assert(sfCoop.soloBonusPending.empty() && sfCoop.soloBonus.pickups.empty());
        }
        assert(sfCoopSoloBonusesEnabled() && sfCoopSoloExcludesTurrets());
        sfCoop.turretTime=20;sfCoopDisableSoloTurretsAndBonuses();
        assert(sfCoop.turretTime==0);
    }
    sfBonusTestSetup();sfCoop.soloGame=true;sfCoop.soloDiagnostic=false;
    sfCoop.soloEncounterRole=SfSoloEncounterRole::FinalBoss;
    sfCoop.health=sfCoopProfile().health*.74f;
    sfCoopSoloBonusObserveBossHealth();assert(sfCoop.soloBonusPending.size()==3);
    assert(!sfCoopSoloBonusQueueTest(Kind::Energy));
    sfCoop.health=0;sfCoopTick(.01f);
    assert(!sfCoop.bossPresent && sfCoop.phase==SfCoopPhase::Combat);
}
static void testSoloMinibossCombat(){
    sfBonusTestSetup(false);sfCoop.soloGame=true;sfCoop.soloDiagnostic=false;
    sfCoop.soloEncounterRole=SfSoloEncounterRole::None;
    sfsolo::CoopSoloWorldBridge world;sfBonusTestWorld(world);
    sfCoop.soloWorldResolve=+[](void *c,tupl from,tupl to,float r){
        auto p=static_cast<sfsolo::CoopSoloWorldBridge*>(c)->moveWithoutTunnelling({from.x,from.y},{to.x,to.y},r);
        return tupl(p.x,p.y);
    };
    SfSoloMiniboss mini;mini.position=mini.anchor=tupl(390,1100);mini.radius=24;
    sfCoop.soloMinibosses.push_back(mini);
    for(int i=0;i<100;++i)sfSoloMinibossTick(.02f);
    assert(!sfCoop.shots.empty());
    for(auto &shot:sfCoop.shots)assert(shot.owner==-1 && shot.damage>=10);
    auto &enemy=sfCoop.soloMinibosses[0];assert(enemy.position.y!=1100);
    sfCoop.shots.clear();
    const auto target=enemy.position;
    sfCoopEmit(tupl(target.x,target.y+50),-float(PI)/2,1000,1,200,0);
    sfCoopProjectiles(.1f);
    assert(enemy.health==0 && sfCoop.soloMinibossDefeated==1);
    assert(sfCoop.soloBonusPending.empty() && sfCoop.soloBonus.pickups.empty());
    sfCoop.shots.clear();sfSoloMinibossTick(5);assert(sfCoop.shots.empty());
    // A wall intercepts a player's real shot before it reaches the miniboss.
    enemy=mini;enemy.position=enemy.anchor=tupl(world.pixelsPerCell*7.5f,world.pixelsPerCell*10.5f);
    for(int y=0;y<world.map.height;++y)world.map.tiles[y*world.map.width+6]=sfsolo::Tile::Rock;
    sfCoopEmit(tupl(world.pixelsPerCell*5.5f,enemy.position.y),0,1000,1,200,0);
    sfCoopProjectiles(.5f);assert(enemy.health==180 && sfCoop.shots.empty());
    Spritej2->x=world.pixelsPerCell*5.5f;Spritej2->y=enemy.position.y;
    for(int i=0;i<100;++i)sfSoloMinibossTick(.02f);
    assert(sfCoop.shots.empty()); // no shooting or seeing through terrain
    assert(world.canOccupy({enemy.position.x,enemy.position.y},enemy.radius));
    // Stealth prevents enemy targeting, never physical contact or our cone.
    enemy=mini;enemy.position=enemy.anchor=tupl(Spritej2->x,Spritej2->y);
    sfCoop.soloBonus.grant(sfsolo::bonus::Kind::Stealth,Spritej2->pv,Spritej2->nrj);
    sfCoop.invulnerable[1]=0;Spritej2->nrj=100;const float before=Spritej2->pv;
    sfSoloMinibossTick(.02f);
    assert(Spritej2->pv<before && sfCoop.shots.empty());
    sfCoop.soloWorldContext=nullptr;
}
static void testSoloFinalArenaPreservesPilot(){
    using sfsolo::bonus::Kind;
    sfBonusTestSetup(false);sfCoop.soloGame=true;sfCoop.soloDiagnostic=false;
    sfCoop.soloWorldMode=true;Spritej2->pv=645;Spritej2->nrj=31;
    sfCoop.cooldown[1]=.37f;sfCoop.soloBonus.grant(Kind::Prison,Spritej2->pv,Spritej2->nrj);
    sfCoop.soloBonus.grant(Kind::Missiles,Spritej2->pv,Spritej2->nrj);
    sfKineticSurgePress(1);sfKineticAdvanceSurges(2);
    explos.push_back(new eexpl("explo",100,100,20,20,0,0));
    sfKineticTriggerWave(1,1.5f);
    const auto time=sfCoop.soloBonus.time;
    sfCoopSoloEnterFinalArena();
    assert(particules.empty() && particulesr.empty() && explos.empty());
    assert(sfKineticWaves.empty() && sfKineticDustFlashes.empty());
    assert(Spritej2->pv==645 && Spritej2->nrj==31 && sfCoop.cooldown[1]==.37f);
    assert(sfCoop.soloBonus.charges==3 && sfCoop.soloBonus.remaining(Kind::Missiles)==3);
    assert(sfCoop.soloBonus.time==time && sfCoop.shots.empty() && !sfKineticSurges[1].held);
    assert(sfCoop.bossPresent && !sfCoop.soloWorldMode && sfCoop.soloWorldContext==nullptr);
    assert(sfCoop.soloEncounterRole==SfSoloEncounterRole::FinalBoss);
    const auto boss=sfCoop.position;sfCoopTick(.1f);
    assert(sfCoop.position.x!=boss.x || sfCoop.position.y!=boss.y); // real arena AI
}
static void testSoloGame015(){
    testSoloProductionRejectsDiagnosticRewards();testSoloSelectorFifthEntry();testSoloProductionPolicyAndMiniRewards();testSoloMinibossCombat();testSoloFinalArenaPreservesPilot();
}
