#pragma once
// Shared by the diagnostic toggle and by the production lair transition.
static void sfCoopSoloBeginBoss(tupl position){
    sfCoopDisableSoloTurretsAndBonuses();
    sfCoop.soloEncounterRole=SfSoloEncounterRole::FinalBoss;
    sfCoop.bossPresent=true;sfCoop.health=sfCoopProfile().health;
    sfCoop.position=position;sfCoop.attack=2;sfCoop.volley=0;sfCoop.phaseNumber=0;
    sfCoop.motion.valid=false;sfCoop.bossThreat={};
    sfCoopSoloBonusBeginEncounter();
}
static void sfCoopSoloEnterFinalArena(){
    // Preserve the one physical pilot's health, heat, powers and fire cadence.
    sfCoop.soloWorldMode=false;sfCoop.soloWorldContext=nullptr;
    sfCoop.soloWorldResolve=nullptr;sfCoop.soloWorldCanOccupy=nullptr;
    sfCoop.soloWorldFirstImpact=nullptr;sfCoop.soloWorldPreparePickups=nullptr;
    sfCoop.soloWorldPlacePickup=nullptr;sfCoop.soloWorldWidth=sfCoop.soloWorldHeight=0;
    sfCoop.shots.clear();sfCoop.beams.clear();sfCoop.waves.clear();
    sfCoop.soloMinibosses.clear();
    sfFixResetAsteroidField();sfFieldRemainder=0;
    for(auto *dust:particules)delete dust;
    particules.clear();
    for(auto *dust:particulesr)delete dust;
    particulesr.clear();
    for(auto *effect:explos)delete effect;
    explos.clear();
    sfKineticWaves.clear();sfKineticDustFlashes.clear();
    sfCoop.asteroidSpawnTimer=.4f;
    auto *ship=sfCoopShip(1);ship->x=sfArenaW*.5f;ship->y=sfArenaH*.72f;
    sfFixResetSpriteHistory(ship);ship->startup();sfObserved[1]={};
    sfCoop.controls[1]={};sfCoop.controls[1].target=tupl(ship->x,ship->y);
    sfCoop.fireFingers.clear();sfKineticSurgeCancel(1);sfKineticAudioCancel(1);
    sfCoop.soloAimTarget=tupl(ship->x,ship->y-120);
    sfCoop.soloKineticAxes.aimAt(1,0,-1);
    sfCoopSoloBeginBoss(tupl(sfArenaW*.5f,sfArenaH*.25f));
}
