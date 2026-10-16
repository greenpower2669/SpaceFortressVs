#pragma once
// Uses the shared projectile, shield, damage, sound and explosion pipeline.
static bool sfSoloMinibossEnabled(){return sfCoop.soloGame && sfCoop.soloWorldMode;}
static bool sfSoloMiniLine(tupl from,tupl to,float radius=1){
    return sfCoop.soloWorldFirstImpact &&
        sfCoop.soloWorldFirstImpact(sfCoop.soloWorldContext,from,to,radius)>=1;
}
static void sfSoloMinibossDamage(SfSoloMiniboss &m,float damage){
    if(m.health<=0 || damage<=0 || !std::isfinite(damage))return;
    m.health=std::max(0.0f,m.health-damage);m.hit=.12f;if(sfFieldAudible(m.position.x,m.position.y))sfCoop.soundHit=true;
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
    const float danger=std::clamp(sfBossDangerIndex,0,8)/8.f;
    // Perception is evaluated before movement. Pack alerts carry only the
    // observation and cannot refresh themselves after the player disappears.
    std::vector<bool> seen(sfCoop.soloMinibosses.size(),false);
    for(size_t i=0;i<seen.size();++i){
        auto &m=sfCoop.soloMinibosses[i];if(m.health<=0)continue;
        const float range=sfArenaW*(.75f+.4f*danger);
        seen[i]=sfCoopPilotTargetable(1) && vlong(pilot.x-m.position.x,pilot.y-m.position.y)<range && sfSoloMiniLine(m.position,pilot);
        if(seen[i]){m.lastSeen=pilot;m.memory=2.0f+5.f*danger;m.searching=false;m.alerted=true;}
        else m.memory=std::max(0.f,m.memory-dt);
    }
    for(size_t i=0;i<seen.size();++i)if(seen[i]){
        const auto &caller=sfCoop.soloMinibosses[i];if(caller.pack<0)continue;
        for(auto &ally:sfCoop.soloMinibosses)if(ally.health>0 && ally.pack==caller.pack &&
            vlong(ally.position.x-caller.position.x,ally.position.y-caller.position.y)<sfArenaW*(.8f+danger)){
            ally.lastSeen=caller.lastSeen;ally.memory=2.f+5.f*danger;ally.alerted=true;ally.searching=false;
        }
    }
    int routeBudget=2; // bounded work per simulation tick
    for(size_t index=0;index<sfCoop.soloMinibosses.size();++index){
        auto &m=sfCoop.soloMinibosses[index];const bool sees=seen[index];
        m.hit=std::max(0.f,m.hit-dt);if(m.health<=0)continue;
        m.repath-=dt;m.patrolClock+=dt;
        const float distance=vlong(pilot.x-m.position.x,pilot.y-m.position.y);
        if(m.alerted && m.memory<=0 && !m.searching){
            m.searching=true;m.searchTime=1.5f+danger*2.f;
            // Fixed, local search around the last known point, not the player.
            const float angle=float(index)*2.399f;
            m.searchTarget=tupl(m.lastSeen.x+std::cos(angle)*sfArenaW*.22f,m.lastSeen.y+std::sin(angle)*sfArenaW*.22f);
        }
        if(m.searching){m.searchTime-=dt;if(m.searchTime<=0){m.alerted=false;m.searching=false;m.route.clear();}}
        tupl goal=m.anchor;
        bool moving=false;
        if(m.alerted){
            goal=m.searching?m.searchTarget:m.lastSeen;moving=true;
            if(m.role==0){ // gate guardian has a bounded territory
                if(vlong(goal.x-m.anchor.x,goal.y-m.anchor.y)>sfArenaW*.65f){goal=m.anchor;m.alerted=false;}
            }else if(m.role==2 && m.pack>=0){
                // Interception from the observed motion only; no updates once
                // LOS is lost. A side offset also separates pack destinations.
                const float angle=float(index)*2.399f;
                goal.x+=std::cos(angle)*sfArenaW*(.12f+.14f*danger);
                goal.y+=std::sin(angle)*sfArenaW*(.12f+.14f*danger);
                if(sees){goal.x+=sfCoop.controls[1].velocity.vx*.15f*danger;goal.y+=sfCoop.controls[1].velocity.vy*.15f*danger;}
            }else if(m.role==3){
                const float angle=float(index)*2.399f;
                goal.x+=std::cos(angle)*sfArenaW*.28f;goal.y+=std::sin(angle)*sfArenaW*.28f;
                if(sees && distance>sfArenaW*.24f && distance<sfArenaW*.5f)moving=false;
            }
            if(sees && distance<sfArenaW*.18f)moving=false;
        }else if(m.role==2){
            const float phase=std::floor(m.patrolClock/4.f)*1.57f+float(index);
            goal=tupl(m.anchor.x+std::cos(phase)*sfArenaW*.28f,m.anchor.y+std::sin(phase)*sfArenaW*.28f);moving=true;
        }else moving=vlong(m.position.x-m.anchor.x,m.position.y-m.anchor.y)>m.radius;
        if(moving && m.warning<=0 && sfCoop.soloWorldResolve){
            if(m.repath<=0 && routeBudget>0){
                --routeBudget;m.repath=.9f-.55f*danger+float(index%3)*.04f;m.waypoint=0;m.route.clear();
                if(sfCoop.soloRoute)sfCoop.soloRoute(sfCoop.soloWorldContext,m.position,goal,m.radius,m.role,m.route);
                else if(sfSoloMiniLine(m.position,goal,m.radius))m.route.push_back(goal);
            }
            while(m.waypoint<m.route.size() && vlong(m.route[m.waypoint].x-m.position.x,m.route[m.waypoint].y-m.position.y)<sfArenaW*.012f)++m.waypoint;
            if(m.waypoint<m.route.size()){
                const auto target=m.route[m.waypoint];
                const float speed=sfArenaW*(m.alerted?(.17f+.16f*danger):.085f);
                const auto velocity=sfUnitVelocity(m.position,target,std::min(speed,vlong(target.x-m.position.x,target.y-m.position.y)/dt));
                tupl desired(m.position.x+velocity.vx*dt,m.position.y+velocity.vy*dt);
                // Personal space, resolved by the same swept terrain collider.
                for(size_t j=0;j<sfCoop.soloMinibosses.size();++j)if(j!=index){
                    const auto &other=sfCoop.soloMinibosses[j];if(other.health<=0)continue;
                    const float dx=desired.x-other.position.x,dy=desired.y-other.position.y,d=std::hypot(dx,dy),gap=m.radius+other.radius+sfArenaW*.025f;
                    if(d>1e-4f && d<gap){desired.x+=dx/d*std::min(gap-d,speed*dt);desired.y+=dy/d*std::min(gap-d,speed*dt);}
                }
                m.position=sfCoop.soloWorldResolve(sfCoop.soloWorldContext,m.position,desired,m.radius);
            }
        }
        if(!sees){m.warning=0;m.cooldown=std::max(m.cooldown,.35f-.2f*danger);}
        else if(m.warning>0){
            m.warning=std::max(0.f,m.warning-dt);
            if(m.warning==0){
                for(int ray=-1;ray<=1;++ray)sfCoopEmit(m.position,m.aim+ray*.14f,sfArenaW*(.45f+.12f*danger),-1,14,0);
                if(sfFieldAudible(m.position.x,m.position.y))sfCoop.soundBoss=true;
                m.cooldown=1.65f-.55f*danger+float(index%3)*.1f;
            }
        }else{
            m.cooldown-=dt;
            if(m.cooldown<=0){m.warning=.5f-.22f*danger;m.aim=std::atan2(pilot.y-m.position.y,pilot.x-m.position.x);}
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
