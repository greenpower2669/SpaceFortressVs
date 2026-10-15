#pragma once
// Uses the shared projectile, shield, damage, sound and explosion pipeline.
static bool sfSoloMinibossEnabled(){return sfCoop.soloGame && sfCoop.soloWorldMode;}
static bool sfSoloMiniLine(tupl from,tupl to,float radius=1){
    return sfCoop.soloWorldFirstImpact &&
        sfCoop.soloWorldFirstImpact(sfCoop.soloWorldContext,from,to,radius)>=1;
}
static void sfSoloMinibossDamage(SfSoloMiniboss &m,float damage){
    if(m.health<=0 || damage<=0 || !std::isfinite(damage))return;
    m.health=std::max(0.0f,m.health-damage);m.hit=.12f;sfCoop.soundHit=true;
    if(m.health==0){
        ++sfCoop.soloMinibossDefeated;
        if(explos.size()<128)explos.push_back(new eexpl("explo",m.position.x,m.position.y,
                                                      m.radius*3,m.radius*3,0,0));
        // Deliberately no call into the boss threshold/reward pipeline.
    }
}
static void sfSoloMinibossHit(SfCoopShot &shot){
    if(!sfSoloMinibossEnabled() || shot.owner<0 || shot.life<=0)return;
    for(auto &m:sfCoop.soloMinibosses)if(m.health>0 &&
        sfSegmentDistance(shot.previous,shot.position,m.position)<m.radius+shot.radius){
        sfSoloMinibossDamage(m,shot.damage);shot.life=0;break;
    }
}
static void sfSoloMinibossGuide(SfCoopShot &shot,float dt){
    if(!sfSoloMinibossEnabled() || shot.kind!=4 || shot.owner!=1)return;
    const SfSoloMiniboss *target=nullptr;float nearest=sfArenaW*1.3f;
    for(const auto &m:sfCoop.soloMinibosses)if(m.health>0){
        const float d=vlong(m.position.x-shot.position.x,m.position.y-shot.position.y);
        if(d<nearest && sfSoloMiniLine(shot.position,m.position,shot.radius)){nearest=d;target=&m;}
    }
    if(!target)return;
    const auto guide=sfCoopGuidedMissileCore({shot.position.x,shot.position.y},
        {target->position.x,target->position.y},{0,0},shot.phase,shot.age,dt,sfArenaW);
    shot.phase=guide.phase;shot.velocity=tuplv(guide.vx,guide.vy);
}
static void sfSoloMinibossPurge(int owner,bool waveAlreadyEmitted=false){
    if(!sfSoloMinibossEnabled() || owner!=1)return;
    const auto *ship=sfCoopShip(owner);
    const float radius=sfKineticShipDiameter(ship)*SF_KINETIC_SURGE_BLAST_DIAMETER*.5f;
    bool hit=false;
    for(auto &m:sfCoop.soloMinibosses)if(m.health>0 &&
        vlong(m.position.x-ship->x,m.position.y-ship->y)<=radius+m.radius &&
        sfSoloMiniLine(tupl(ship->x,ship->y),m.position)){
        sfSoloMinibossDamage(m,SF_COOP_PHASER_DAMAGE*3);hit=true;
    }
    if(hit && !waveAlreadyEmitted)sfKineticTriggerWave(owner,SF_KINETIC_SURGE_BLAST_DIAMETER*.5f,1,
                              sfKineticEnergyFraction(ship->nrj));
}
static void sfSoloMinibossTick(float dt){
    if(!sfSoloMinibossEnabled() || dt<=0 || sfCoop.phase!=SfCoopPhase::Combat)return;
    const auto *ship=sfCoopShip(1);if(ship->pv<=0)return;
    const tupl pilot(ship->x,ship->y);
    for(auto &m:sfCoop.soloMinibosses){
        m.hit=std::max(0.0f,m.hit-dt);
        if(m.health<=0)continue;
        const float distance=vlong(pilot.x-m.position.x,pilot.y-m.position.y);
        const bool sees=sfCoopPilotTargetable(1) && distance<sfArenaW*1.05f && sfSoloMiniLine(m.position,pilot);
        m.alerted=sees;
        if(!sees){m.warning=0;m.cooldown=1.1f;}
        if(sees){
            // Short pursuit within this encounter's territory, with swept walls.
            if(distance>sfArenaW*.22f && m.warning<=0){
                const auto velocity=sfUnitVelocity(m.position,pilot,sfArenaW*.12f);
                tupl desired(m.position.x+velocity.vx*dt,m.position.y+velocity.vy*dt);
                if(vlong(desired.x-m.anchor.x,desired.y-m.anchor.y)<=sfArenaW*.55f && sfCoop.soloWorldResolve){
                    const float fraction=sfCoop.soloWorldFirstImpact(sfCoop.soloWorldContext,m.position,desired,m.radius);
                    desired=tupl(m.position.x+(desired.x-m.position.x)*fraction,
                                 m.position.y+(desired.y-m.position.y)*fraction);
                    if(sfCoop.soloWorldCanOccupy && sfCoop.soloWorldCanOccupy(sfCoop.soloWorldContext,desired,m.radius))
                        m.position=desired;
                }
            }
            if(m.warning>0){
                m.warning=std::max(0.0f,m.warning-dt);
                if(m.warning==0){
                    for(int ray=-1;ray<=1;++ray)
                        sfCoopEmit(m.position,m.aim+ray*.14f,sfArenaW*.50f,-1,14,0);
                    sfCoop.soundBoss=true;m.cooldown=1.4f;
                }
            }else{
                m.cooldown-=dt;
                if(m.cooldown<=0){m.warning=.45f;m.aim=std::atan2(pilot.y-m.position.y,pilot.x-m.position.x);}
            }
        }
        // Invisibility suppresses targeting, never physical contact or damage
        // from the player's weapons. Both still respect terrain.
        if(vlong(pilot.x-m.position.x,pilot.y-m.position.y)<m.radius+sfCoopShipRadius() &&
           sfSoloMiniLine(m.position,pilot))sfCoopHurt(1,20);
        // The same visible kinetic cone and DPS rule, on the current 360° axis.
        if(sfKineticSuctionVisible(1)){
            const float diameter=sfKineticShipDiameter(ship);
            const auto axis=sfCoop.soloKineticAxes.direction(1);
            const float nx=ship->x+axis.x*diameter*.42f,ny=ship->y+axis.y*diameter*.42f;
            const float range=diameter*sfKineticSurgeMiningRangeDiameters();
            if(sfKineticSurgeConeContainsAxis(nx,ny,axis.x,axis.y,m.position.x,m.position.y,range,m.radius) &&
               sfSoloMiniLine(tupl(nx,ny),m.position)){
                const float fraction=std::clamp(((m.position.x-nx)*axis.x+(m.position.y-ny)*axis.y)/range,0.0f,1.0f);
                sfSoloMinibossDamage(m,sfCoopPhaserDps(1)*sfKineticConeBossDpsMultiplier(fraction)*dt);
            }
        }
    }
}
