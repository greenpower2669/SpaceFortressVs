#pragma once
// Adapter inside the historical campaign TU: no second world, pilot, firing
// clock, energy equation, FabLib definition or global session.
static bool sfCoopSoloBonusesEnabled(){return sfCoop.soloDiagnostic || sfCoop.soloWorldMode;}
static float sfCoopSoloPickupHull(){
    const auto *ship=sfCoopShip(1);
    return std::max(sfCoopShipRadius(),std::max(ship->w,ship->h)*.5f);
}
static void sfCoopSoloBonusObserveBossHealth(){
    if(!sfCoopSoloBonusesEnabled() || !sfCoop.bossPresent)return;
    const auto drops=sfCoop.soloBonus.drops(sfCoop.health,sfCoopProfile().health);
    for(auto k:drops)if(sfCoop.soloBonusPending.size()<48)sfCoop.soloBonusPending.push_back(k);
    if(!drops.empty())sfCoop.soloLootRetry=0;
}
static bool sfCoopPilotTargetable(int owner){
    return sfCoopPilotActive(owner) && sfCoopShip(owner)->pv>0 &&
        !(sfCoopSoloBonusesEnabled() && owner==1 && sfCoop.soloBonus.stealth());
}
static float sfCoopBossClock(){return sfCoopSoloBonusesEnabled()?sfCoop.soloBossClock:sfCoop.time;}
static Uint8 sfCoopSoloShipAlpha(int owner,Uint8 normal){
    return sfCoopSoloBonusesEnabled() && owner==1 && sfCoop.soloBonus.stealth()?Uint8(100):normal;
}
static void sfCoopSoloBonusBeginEncounter(){
    if(!sfCoopSoloBonusesEnabled())return;
    sfCoop.soloBonus.beginEncounter();sfCoop.soloBossClock=sfCoop.time;
    sfCoop.chargeActive=false;sfCoop.chargeTime=0;sfCoop.chargeCooldown=4.5f;
    sfCoop.chargeOffsetX=sfCoop.chargeOffsetY=0;
    sfCoop.bossDodgeOffset=tupl(0,0);sfCoop.bossDodgeVelocity.set(0,0);
}
static void sfCoopSoloBonusImpact(tupl at,sfsolo::bonus::Kind kind){
    if(sfCoop.soloBonusImpacts.size()>=64)sfCoop.soloBonusImpacts.erase(sfCoop.soloBonusImpacts.begin());
    sfCoop.soloBonusImpacts.push_back({at,kind,sfCoop.soloBonus.time+.5});
}
static bool sfCoopSoloBonusPickupInFlight(std::uint32_t pickupId){
    const double now=sfCoop.soloBonus.time;
    return std::any_of(sfCoop.soloBonusEjections.begin(),sfCoop.soloBonusEjections.end(),
        [&](const SfSoloBonusEjection &e){return e.pickupId==pickupId && e.until>now;});
}
static bool sfCoopSoloBonusQueueTest(sfsolo::bonus::Kind kind){
    using sfsolo::bonus::Kind;
    if(!sfCoopSoloBonusesEnabled() || !sfCoop.bossPresent || sfCoop.health<=0 ||
       kind==Kind::None || unsigned(kind)>=unsigned(Kind::None) ||
       sfCoop.soloBonusPending.size()>=48)return false;
    sfCoop.soloBonusPending.push_back(kind);sfCoop.soloLootRetry=0;return true;
}
static unsigned sfCoopSoloBonusQueueAllTests(){
    unsigned queued=0;
    for(unsigned i=0;i<8;++i)queued+=sfCoopSoloBonusQueueTest(sfsolo::bonus::Kind(i))?1u:0u;
    sfCoop.soloBonusTestBurst=queued; // keep these eight in one visual launch batch
    return queued;
}
static void sfCoopSoloBonusTick(float dt){
    if(!sfCoopSoloBonusesEnabled() || !std::isfinite(dt)||dt<=0)return;
    auto &s=sfCoop.soloBonus;
    // Prison and ice affect boss locomotion/charge only, never player input,
    // projectiles, firing cadence or the simulation clock.
    using sfsolo::bonus::Kind;
    const double frozen=std::min(double(dt),s.remaining(Kind::Prison));
    const double iced=std::max(0.0,std::min(double(dt),s.remaining(Kind::Ice))-frozen);
    sfCoop.soloBossStep=float(double(dt)-frozen-.6*iced);
    sfCoop.soloBossClock+=sfCoop.soloBossStep;
    const float damage=s.advance(dt);
    if(sfCoop.bossPresent && sfCoop.health>0 && damage>0){
        sfCoopRegisterBossDamage(1,damage);
        sfCoop.health=std::max(0.0f,sfCoop.health-damage);sfCoop.hit=.08f;
        sfCoopSoloBonusObserveBossHealth();
    }
    if(!sfCoop.bossPresent)s.clearBoss();
    if(!sfCoopPilotTargetable(sfCoop.bossAggressor<0?1:sfCoop.bossAggressor)){
        sfCoop.bossAggressor=-1;sfCoop.bossAggroTime=0;
    }
    auto &fx=sfCoop.soloBonusImpacts;
    fx.erase(std::remove_if(fx.begin(),fx.end(),[&](const auto &v){return v.until<=s.time;}),fx.end());
    auto &ejections=sfCoop.soloBonusEjections;
    ejections.erase(std::remove_if(ejections.begin(),ejections.end(),
        [&](const SfSoloBonusEjection &e){return e.until<=s.time;}),ejections.end());
}
static bool sfCoopSoloBonusFire(int owner,float heatBefore){
    using namespace sfsolo::bonus;
    auto &s=sfCoop.soloBonus;
    if(!sfCoopSoloBonusesEnabled() || owner!=1 || !sfCoop.bossPresent || sfCoop.health<=0 ||
       s.missiles() || !s.charges || !isOrb(s.orb))return false;
    if(sfCoop.shots.size()+salvoSize>600)return false;
    const Kind kind=s.orb;
    if(!s.consumeSalvo(true))return false;
    auto *ship=sfCoopShip(owner);
    sfSpendMainEnergy(ship); // one canonical expense for the whole human salvo
    const float aim=std::atan2(sfCoop.position.y-ship->y,sfCoop.position.x-ship->x);
    ++sfCoop.shotSequence[owner];
    for(unsigned i=0;i<salvoSize;++i){
        const float angle=aim+(int(i)-1)*.16f;
        sfCoopEmit(tupl(ship->x,ship->y),angle,sfArenaW*.65f,owner,
                   kind==Kind::Meteor?meteorDamage:orbDamage,0);
        auto &shot=sfCoop.shots.back();
        shot.bonusKind=kind;shot.bonusEncounter=s.encounter;
        shot.radius=sfArenaW*(kind==Kind::Meteor?.017f:.011f);
    }
    sfCoop.cooldown[owner]=sfCoopFireDelay(heatBefore);
    return true;
}
static void sfCoopSoloBonusCollectAndDrop(){
    using namespace sfsolo::bonus;
    if(!sfCoopSoloBonusesEnabled())return;
    auto &s=sfCoop.soloBonus;
    auto *ship=sfCoopShip(1);
    if(ship->pv<=0)return;
    // Contact cannot reach through terrain; placement guarantees a navigable
    // component, collection additionally checks the short contact segment.
    for(auto i=s.pickups.begin();i!=s.pickups.end();){
        if(sfCoopSoloBonusPickupInFlight(i->id)){++i;continue;}
        const float reach=sfCoopSoloPickupHull()+i->radius;
        if(i->expires>s.time && std::hypot(ship->x-i->x,ship->y-i->y)<=reach &&
           (!sfCoop.soloWorldMode || (sfCoop.soloWorldFirstImpact &&
            sfCoop.soloWorldFirstImpact(sfCoop.soloWorldContext,tupl(ship->x,ship->y),tupl(i->x,i->y),1.0f)>=1))){
            sfCoop.soloLastCollected=i->kind;sfCoop.soloCollectUntil=s.time+2;
            s.grant(i->kind,ship->pv,ship->nrj);sfCoopSoloBonusImpact(tupl(i->x,i->y),i->kind);
            i=s.pickups.erase(i);
        }else ++i;
    }
    sfCoopSoloBonusObserveBossHealth();
    if(s.time<sfCoop.soloLootRetry)return;
    sfCoop.soloLootRetry=s.time+.5;
    const float radius=std::max(10.0f,sfArenaW*.023f);
    if(sfCoop.soloWorldMode && sfCoop.soloWorldPreparePickups &&
       !sfCoop.soloBonusPending.empty() && s.pickups.size()<maxPickups)
        sfCoop.soloWorldPreparePickups(sfCoop.soloWorldContext,tupl(ship->x,ship->y),std::max(radius,sfCoopSoloPickupHull()));
    while(!sfCoop.soloBonusPending.empty() && s.pickups.size()<maxPickups){
        bool placed=false;
        for(int attempt=0;attempt<16 && !placed;++attempt){
            const float angle=float(s.nextId)*2.399963f+attempt*.65f;
            const float distance=sfCoopBossRadius()+radius*2+attempt*radius;
            tupl at(sfCoop.position.x+std::cos(angle)*distance,sfCoop.position.y+std::sin(angle)*distance);
            if(sfCoop.soloWorldMode){
                if(!sfCoop.soloWorldPlacePickup || !sfCoop.soloWorldPlacePickup(sfCoop.soloWorldContext,
                    tupl(ship->x,ship->y),at,std::max(radius,sfCoopSoloPickupHull()),at))continue;
            }else{
                at.x=std::clamp(at.x,radius,sfArenaW-radius);
                at.y=std::clamp(at.y,sfArenaH*.12f,sfArenaH*.84f);
            }
            bool overlaps=false;
            const float burstFactor=sfCoop.soloBonusTestBurst?0.58f:1.0f;
            for(const auto &p:s.pickups)
                if(std::hypot(at.x-p.x,at.y-p.y)<(radius+p.radius+3)*burstFactor){overlaps=true;break;}
            if(overlaps)continue;
            const auto kind=sfCoop.soloBonusPending.front();
            const std::uint32_t pickupId=s.nextId;
            placed=s.addPickup(kind,at.x,at.y,radius);
            if(placed){
                const double flight=.42+.035*double(pickupId%4);
                if(sfCoop.soloBonusEjections.size()>=48)sfCoop.soloBonusEjections.erase(sfCoop.soloBonusEjections.begin());
                sfCoop.soloBonusEjections.push_back(
                    {kind,sfCoop.position,at,s.time,s.time+flight,pickupId});
                if(sfCoop.soloBonusTestBurst) --sfCoop.soloBonusTestBurst;
            }
        }
        if(!placed){
            // A diagnostic X8 batch must not silently degrade into eight
            // half-second launches. If the maze has no safe room, keep the
            // batch pending and retry together once space becomes available.
            if(sfCoop.soloBonusTestBurst)sfCoop.soloLootRetry=s.time+.08;
            break;
        }
        sfCoop.soloBonusPending.erase(sfCoop.soloBonusPending.begin());
    }
}
