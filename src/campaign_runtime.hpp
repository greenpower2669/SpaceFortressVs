#pragma once
#include "boss_catalog.hpp"
#include "scenic_mix.hpp"
#include "campaign_save.hpp"
#include <map>

enum class SfCoopPhase { Intro, Combat, Dying, Name, Saved, Defeat, Paused };
struct SfCoopShot {
    tupl position,previous;
    tuplv velocity;
    float radius=5,damage=10,age=0,life=8;
    int owner=-1,kind=0;
    float phase=0;
};
struct SfCoopBeam { tupl origin; float angle=0,age=0,warning=.9f; };
struct SfCoopWave { tupl origin; float age=0,radius=0; };
struct SfCoopControl { SDL_FingerID finger=-1; tupl target; bool down=false; tuplv velocity; };
struct SfCoopState {
    SfCoopPhase phase=SfCoopPhase::Intro;
    int boss=0,encounter=0,volley=0,revives=3,phaseNumber=0;
    float time=0,phaseTime=0,health=900,attack=1,warning=0,hit=0;
    float chargeTime=0,chargeCooldown=4.5f,chargeOffsetX=0,chargeOffsetY=0;
    float bossKineticFlash=0;
    std::array<float,2> bossThreat{};
    int bossAggressor=-1;
    float bossAggroTime=0,bossDodgeCooldown=0;
    tupl bossDodgeOffset;
    tuplv bossDodgeVelocity;
    // Boss reserves are visual stress meters only. Gameplay HP/damage and the
    // canonical fixed 55% kinetic dissipation remain unchanged.
    float bossEnergyReserve=1.0f;
    float bossKineticReserve=1.0f;
    float asteroidSpawnTimer=2.0f;
    std::map<sprite*,float> bossAsteroidCooldown;
    tupl position;
    SfMotionSample motion;
    std::array<SfCoopControl,2> controls{};
    std::map<SDL_FingerID,int> fireFingers;
    float turretTime=0,bonusTimer=12,bonusLife=0;
    tupl bonusPosition;
    tuplv bonusVelocity;
    std::array<float,2> cooldown{},invulnerable{},reviveProgress{};
    std::array<unsigned,2> shotSequence{};
    std::array<float,2> redDustCooldown{};
    std::array<bool,2> chargeHit{};
    bool chargeActive=false;
    std::vector<SfCoopShot> shots;
    std::vector<SfCoopBeam> beams;
    std::vector<SfCoopWave> waves;
    std::array<std::string,3> names{};
    int nameField=0;
    bool keyboard=false,pendingSaved=false;
    std::string composition,error;
    std::array<bool,2> soundShot{};
    bool soundBoss=false,soundHit=false;
};
inline SfCoopState sfCoop;
inline int sfFamePage=0,sfCampaignPage=0;
struct SfCampaignTextures {
    SDL_Renderer *renderer=nullptr;
    SDL_Texture *bosses=nullptr,*planets=nullptr,*backgrounds=nullptr;
    SDL_Texture *orange=nullptr,*blue=nullptr,*bonus=nullptr,*impact=nullptr,*explosion=nullptr,*missile=nullptr;
    SDL_Texture *orbOrange=nullptr,*orbBlue=nullptr;
    std::array<SDL_Texture*,4> rocks{};
};
inline std::vector<SfCampaignTextures> sfCampaignTextures;
struct SfDuelShipStyle {
    float w,h,sw,sh,speed;
    int frames,frame;
    bool animated,boundedBreathing;
};
inline std::array<SfDuelShipStyle,2> sfDuelShipStyles{};
inline bool sfDuelShipStylesSaved=false;

static sprite *sfCoopShip(int owner) { return owner==0 ? Spritej1 : Spritej2; }
static const SfBossProfile &sfCoopProfile() { return sfEncounterProfile(sfCoop.encounter); }
static float sfCoopBossRadius() { return std::min(sfArenaW,sfArenaH)*(.11f+.00065f*sfCoop.boss); }
static float sfCoopShipRadius() { return std::min(sfArenaW,sfArenaH)*.053f; }
static float sfCoopBossAiSkill()
{
    return std::clamp(sfBossDangerIndex,0,SF_BOSS_DANGER_COUNT-1)/float(SF_BOSS_DANGER_COUNT-1);
}
static void sfCoopRegisterBossDamage(int owner,float damage)
{
    if(owner<0 || owner>1 || damage<=0 || sfCoop.health<=0) return;
    sfCoop.bossThreat[owner]+=damage;
}
static SfVelocityGhost sfBossGhost() { return {sfCoop.position,sfCoop.motion.velocity}; }
static float sfSegmentDistance(tupl a,tupl b,tupl point)
{
    const float dx=b.x-a.x,dy=b.y-a.y,length=dx*dx+dy*dy;
    const float t=length>.0001f ? std::clamp(((point.x-a.x)*dx+(point.y-a.y)*dy)/length,0.0f,1.0f) : 0;
    return vlong(a.x+dx*t-point.x,a.y+dy*t-point.y);
}
static tuplv sfUnitVelocity(tupl origin,tupl target,float speed)
{
    enti heading; heading.xy=origin; heading.azim(target.x,target.y);
    return tuplv(heading.v.vxt*speed,heading.v.vyt*speed);
}
static void sfCoopEmit(tupl origin,float angle,float speed,int owner,float damage,int kind=0)
{
    if (sfCoop.shots.size()>=600) return;
    SfCoopShot shot;
    shot.position=shot.previous=origin;
    shot.velocity=tuplv(std::cos(angle)*speed,std::sin(angle)*speed);
    shot.owner=owner; shot.damage=damage; shot.kind=kind;
    shot.radius=sfArenaW*(owner<0 ? .009f : .006f);
    shot.phase=angle; shot.life=kind==2 ? 5.0f : 8.0f;
    sfCoop.shots.push_back(shot);
    if (owner>=0) sfCoop.soundShot[owner]=true;
}
static constexpr float SF_COOP_BOSS_DUST_HEAL_FRACTION = .0015f;

static void sfCoopEmitRedDust(int owner,int count)
{
    if (owner<0 || owner>1 || count<=0 || sfCoop.phase!=SfCoopPhase::Combat || sfCoop.redDustCooldown[owner]>0) return;
    auto *ship=sfCoopShip(owner);
    const float ring=sfCoopShipRadius()*1.18f;
    for(int i=0;i<count && particulesr.size()<1000;++i) {
        const float angle=(i+.5f)*2*float(PI)/count+owner*.37f;
        auto *dust=new parts(ship->x+std::cos(angle)*ring,ship->y+std::sin(angle)*ring);
        dust->pv=600;
        dust->vx+=std::cos(angle)*sfArenaW*.65f;
        dust->vy+=std::sin(angle)*sfArenaW*.65f;
        particulesr.push_back(dust);
    }
    sfCoop.redDustCooldown[owner]=.16f;
}

static void sfCoopEnemyCollectWhiteDust()
{
    if (sfCoop.phase!=SfCoopPhase::Combat) return;
    const float maximum=sfCoopProfile().health;
    if (sfCoop.health>=maximum-.001f) return;
    for(auto *dust:particules) {
        if (dust->pv<=0) continue;
        const tupl dustPosition(dust->x,dust->y);
        float nearest=std::numeric_limits<float>::max();
        bool enemyWins=false;
        for(int owner=0;owner<2;++owner) {
            const auto *ship=sfCoopShip(owner);
            if (ship->pv<=0) continue;
            const float distance=vlong(dust->x-ship->x,dust->y-ship->y);
            if (distance<ship->sh*.5f && distance<nearest) nearest=distance;
        }
        const float bossDistance=vlong(dust->x-sfCoop.position.x,dust->y-sfCoop.position.y);
        if (bossDistance<sfCoopBossRadius()*.90f && bossDistance<nearest) {
            nearest=bossDistance;enemyWins=true;
        }
        for(const auto &shot:sfCoop.shots) {
            if (shot.owner>=0 || shot.life<=0 || shot.kind<1 || shot.kind>3) continue;
            const float distance=sfSegmentDistance(shot.previous,shot.position,dustPosition);
            const float pickup=std::max(shot.radius*2.5f,sfArenaW*.012f);
            if (distance<pickup && distance<nearest) {nearest=distance;enemyWins=true;}
        }
        if (!enemyWins) continue;
        const float value=std::clamp(dust->pv/600.0f,0.0f,1.0f);
        sfCoop.health=std::min(maximum,sfCoop.health+maximum*SF_COOP_BOSS_DUST_HEAL_FRACTION*value);
        dust->pv=0;
    }
}

static void sfCoopHurt(int owner,float damage)
{
    auto *ship=sfCoopShip(owner);
    if (ship->pv<=0 || sfCoop.invulnerable[owner]>0 || sfCoop.phase!=SfCoopPhase::Combat) return;
    const float incoming=sfApplyHostileDanger(damage);
    ship->pv=std::max(0.0f,ship->pv-sfApplyShieldImpact(ship,incoming));
    sfCoop.invulnerable[owner]=.38f;
    sfCoop.soundHit=true;sfCoopEmitRedDust(owner,6);
    if (ship->pv<=0) {
        sfCoop.controls[owner].down=false;sfCoop.controls[owner].finger=-1;
        sfCoop.controls[owner].velocity.set(0,0);
        for(auto i=sfCoop.fireFingers.begin();i!=sfCoop.fireFingers.end();) {
            if(i->second==owner) i=sfCoop.fireFingers.erase(i);else ++i;
        }
        sfKineticAudioCancel(owner);sfKineticSurgeCancel(owner);
    }
}

static void sfCoopPattern(int pattern)
{
    sfCoop.soundBoss=true;
    const auto &boss=sfCoopProfile();
    const float speed=sfArenaW*boss.shotSpeed;
    const int tier=boss.tier,count=3+tier+(boss.difficulty>=2 ? 1 : 0);
    const float rotation=sfCoop.volley*.37f+boss.index*.11f;
    if (pattern==3) {
        for (int owner=0;owner<2;++owner) if (sfCoopShip(owner)->pv>0) {
            const tupl aim=sfShipGhost(owner).intercept(sfCoop.position,speed,.6f);
            sfCoop.beams.push_back({sfCoop.position,std::atan2(aim.y-sfCoop.position.y,aim.x-sfCoop.position.x),0,1.0f-tier*.05f});
        }
    } else if (pattern==8) {
        sfCoop.waves.push_back({sfCoop.position,0,0});
    } else if (pattern==2 || pattern==5) {
        const int rays=pattern==2 ? 12+tier*2 : 4;
        const int gap=sfCoop.volley%rays;
        for (int i=0;i<rays;++i) {
            if (pattern==2 && (i==gap || i==(gap+1)%rays || i==(gap+rays/2)%rays)) continue;
            sfCoopEmit(sfCoop.position,rotation+i*2*float(PI)/rays,speed,-1,boss.damage);
        }
    } else if (pattern==1) {
        for (int arm=0;arm<4;++arm) for (int i=0;i<2+tier;++i)
            sfCoopEmit(sfCoop.position,rotation+arm*float(PI)*.5f+i*.12f,speed*(.8f+i*.06f),-1,boss.damage);
    } else if (pattern==4) {
        for (int i=0;i<4+tier;++i) {
            const float angle=rotation+i*2*float(PI)/(4+tier);
            sfCoopEmit(tupl(sfCoop.position.x+std::cos(angle)*sfCoopBossRadius(),
                            sfCoop.position.y+std::sin(angle)*sfCoopBossRadius()),angle,speed*.32f,-1,boss.damage*1.5f,2);
        }
    } else {
        for (int owner=0;owner<2;++owner) if (sfCoopShip(owner)->pv>0) {
            tupl origin=sfCoop.position;
            if (pattern==9) origin.x+=sfCoopBossRadius()*std::sin(rotation+owner*float(PI))*1.3f;
            const tupl aim=sfShipGhost(owner).intercept(origin,speed,.45f+.12f*boss.difficulty);
            const float angle=std::atan2(aim.y-origin.y,aim.x-origin.x);
            for (int i=0;i<count;++i) sfCoopEmit(origin,angle+(i-(count-1)*.5f)*(.12f+tier*.01f),speed,
                                                  -1,boss.damage,pattern==6 ? 1 : pattern==7 ? 3 : 0);
        }
    }
}

static float sfCoopRisk(tupl position,tuplv velocity)
{
    float risk=0; const float radius=sfCoopShipRadius();
    for (const auto &shot : sfCoop.shots) if (shot.owner<0 && shot.life>0) {
        const float dx=shot.position.x-position.x,dy=shot.position.y-position.y;
        const float vx=shot.velocity.vx-velocity.vx,vy=shot.velocity.vy-velocity.vy;
        const float speed2=vx*vx+vy*vy;
        const float t=speed2>.001f ? std::clamp(-(dx*vx+dy*vy)/speed2,0.0f,.7f) : 0;
        const float clearance=radius+shot.radius+sfArenaW*.026f;
        const float distance=vlong(dx+vx*t,dy+vy*t);
        if (distance<clearance) risk+=1+(clearance-distance)/clearance*3;
    }
    for (const auto &beam : sfCoop.beams) {
        const tupl end(beam.origin.x+std::cos(beam.angle)*sfArenaH*2,beam.origin.y+std::sin(beam.angle)*sfArenaH*2);
        const tupl future(position.x+velocity.vx*.3f,position.y+velocity.vy*.3f);
        if (sfSegmentDistance(beam.origin,end,future)<radius+sfArenaW*.025f) risk+=4;
    }
    for (const auto &wave : sfCoop.waves) if (wave.age>.25f) {
        const float futureRadius=std::max(0.0f,wave.age+.35f-.7f)*sfArenaW*.48f;
        const float distance=vlong(position.x+velocity.vx*.35f-wave.origin.x,position.y+velocity.vy*.35f-wave.origin.y);
        if (std::abs(distance-futureRadius)<radius+sfArenaW*.03f) risk+=3;
    }
    const float oldDt=sfFrameDt,oldK=k0;sfFrameDt=1.0f/60;k0=1;
    risk+=sfAsteroidRisk(position,velocity,radius);
    sfFrameDt=oldDt;k0=oldK;
    return risk;
}

static bool sfCoopAiLowEnergy()
{
    return Spritej1 && sfKineticEnergyFraction(Spritej1->nrj)<=.10f;
}
static sprite *sfCoopAiRecoveryRock(bool rescueOnly)
{
    if(!Spritej1) return nullptr;
    const tupl position(Spritej1->x,Spritej1->y);
    const tupl rescue(Spritej2 ? Spritej2->x : position.x,Spritej2 ? Spritej2->y : position.y);
    const float shipDiameter=sfKineticShipDiameter(Spritej1);
    sprite *bestRock=nullptr;float best=std::numeric_limits<float>::max();
    for(auto *rock:sa1) {
        if(!rock || rock->pv<=0) continue;
        const float size=std::max(rock->w,rock->h);
        const float rvx=rock->vx*60.0f-sfObserved[0].velocity.vx;
        const float rvy=rock->vy*60.0f-sfObserved[0].velocity.vy;
        const float relative=vlong(rvx,rvy);
        const bool small=size<=shipDiameter*1.05f && relative<=sfArenaW*.36f;
        if(!small) continue; // distinguish exploitable rock from dangerous collision.
        const float distance=vlong(rock->x-position.x,rock->y-position.y);
        if(distance>sfArenaW*(rescueOnly ? .26f : .48f)) continue;
        if(rescueOnly && sfSegmentDistance(position,rescue,tupl(rock->x,rock->y))>sfArenaW*.16f) continue;
        const float score=distance+relative*.18f+size*.20f;
        if(score<best) {best=score;bestRock=rock;}
    }
    return bestRock;
}
static bool sfCoopAiRockInMiningCone(sprite *rock)
{
    if(!rock || !Spritej1 || rock->pv<=0) return false;
    const float diameter=sfKineticShipDiameter(Spritej1);
    const float noseY=Spritej1->y+diameter*.42f;
    const float range=diameter*sfKineticSurgeMiningRangeDiameters();
    return sfKineticSurgeConeContains(0,Spritej1->x,noseY,rock->x,rock->y,range,
                                      std::max(rock->w,rock->h)*.5f);
}
static void sfCoopAiUpdateMiningCone()
{
    if(sfActiveMode!=SF_COOP_AI || !Spritej1 || Spritej1->pv<=0) return;
    const bool rescue=Spritej2 && Spritej2->pv<=0 && sfCoop.revives>0;
    sprite *rock=sfCoopAiLowEnergy() ? sfCoopAiRecoveryRock(rescue) : nullptr;
    const bool mine=rock && sfCoopAiRockInMiningCone(rock);
    if(mine && !sfKineticSurges[0].held) {
        sfKineticSurgePress(0);sfKineticAudioStartCharge(0);
    } else if(!mine && sfKineticSurges[0].held) {
        // Co-op AI uses the cone only as a mining tool: cancel, never release
        // an offensive full-charge purge on its own.
        sfKineticSurgeCancel(0);sfKineticAudioCancel(0);
    }
}
static bool sfCoopAiShouldFire()
{
    if(sfActiveMode!=SF_COOP_AI || !Spritej1 || Spritej1->pv<=0) return false;
    if(Spritej2 && Spritej2->pv<=0 && sfCoop.revives>0) return false;
    if(sfKineticEnergyFraction(Spritej1->nrj)<=.25f) return false;
    if(Spritej1->pv<420 || sfKineticSurges[0].held) return false;
    return true;
}
static tuplv sfCoopAiVelocity(float dt)
{
    auto *ship=Spritej1;
    const tupl position(ship->x,ship->y);
    const bool rescue=Spritej2->pv<=0 && sfCoop.revives>0;
    const bool lowEnergy=sfCoopAiLowEnergy();
    const auto aim=sfBossGhost().intercept(position,sfArenaW*1.4f,.55f+.35f*sfCoopBossAiSkill());
    tupl goal(aim.x+sfArenaW*.16f*std::sin(sfCoop.time*.38f),aim.y-sfArenaH*.26f);
    sprite *resourceRock=nullptr;

    // Rescue is absolute priority. At critical energy only accept a quick
    // resource detour lying near the rescue route; never cross the arena to mine.
    if(rescue) {
        goal=tupl(Spritej2->x,Spritej2->y);
        if(lowEnergy) resourceRock=sfCoopAiRecoveryRock(true);
    } else if(lowEnergy) {
        // Prefer already-created white dust, then a small safely exploitable rock.
        float bestDust=std::numeric_limits<float>::max();
        for(const auto *dust:particules) {
            if(!dust || dust->pv<=0) continue;
            const float distance=vlong(dust->x-position.x,dust->y-position.y);
            if(distance<bestDust && distance<sfArenaW*.42f &&
               sfCoopRisk(tupl(dust->x,dust->y),tuplv(0,0))<1.2f) {
                bestDust=distance;goal=tupl(dust->x,dust->y);
            }
        }
        if(bestDust==std::numeric_limits<float>::max()) resourceRock=sfCoopAiRecoveryRock(false);
    } else if (ship->nrj>22 && !particules.empty()) {
        float best=std::numeric_limits<float>::max();
        for (const auto *dust : particules) {
            if (dust->pv<=0) continue;
            const float distance=vlong(dust->x-position.x,dust->y-position.y);
            if (distance<best && sfCoopRisk(tupl(dust->x,dust->y),tuplv(0,0))<.1f) {best=distance;goal=tupl(dust->x,dust->y);}
        }
    }
    if(resourceRock) {
        // Stand just above the rock so the owner-0 forward cone (+Y) mines it.
        const float clearance=sfCoopShipRadius()+std::max(resourceRock->w,resourceRock->h)*.62f;
        goal=tupl(resourceRock->x,resourceRock->y-clearance);
    }

    const float maxSpeed=sfArenaW*.58f;
    goal.x=std::clamp(goal.x,sfArenaW*.12f,sfArenaW*.88f);
    goal.y=std::clamp(goal.y,sfArenaH*.12f,sfArenaH*.88f);
    const tuplv wanted=sfUnitVelocity(position,goal,std::min(maxSpeed,vlong(goal.x-position.x,goal.y-position.y)*3));
    tuplv chosen=wanted; float best=std::numeric_limits<float>::max();
    for (int i=-2;i<16;++i) {
        const float angle=i*2*float(PI)/16;
        const tuplv candidate=i==-2 ? wanted : i==-1 ? tuplv(0,0) : tuplv(std::cos(angle)*maxSpeed,std::sin(angle)*maxSpeed);
        const tupl future(position.x+candidate.vx*.35f,position.y+candidate.vy*.35f);
        const bool edge=future.x<sfArenaW*.07f || future.x>sfArenaW*.93f || future.y<sfArenaH*.11f || future.y>sfArenaH*.89f;
        const float nearBoss=vlong(future.x-sfCoop.position.x,future.y-sfCoop.position.y)<sfCoopBossRadius()+sfCoopShipRadius()*1.7f ? 8.0f : 0;
        float risk=sfCoopRisk(position,candidate);
        float resourceBonus=0;
        if(resourceRock) {
            const float d=vlong(future.x-resourceRock->x,future.y-resourceRock->y);
            resourceBonus=std::clamp(1.0f-d/std::max(1.0f,sfArenaW*.30f),0.0f,1.0f)*5.0f;
            // A small, slow selected rock is a resource, not an automatic flee trigger.
            risk=std::max(0.0f,risk-1.4f*resourceBonus);
        }
        const float rescueBonus=rescue ? std::clamp(1.0f-vlong(future.x-Spritej2->x,future.y-Spritej2->y)/std::max(1.0f,sfArenaW),0.0f,1.0f)*3.0f : 0;
        const float score=risk*10+(edge ? 12 : 0)+nearBoss+
                          vlong(candidate.vx-wanted.vx,candidate.vy-wanted.vy)/maxSpeed-resourceBonus-rescueBonus;
        if (score<best) {best=score;chosen=candidate;}
    }
    const auto old=sfCoop.controls[0].velocity;
    const float blend=1-std::exp(-dt*9);
    return tuplv(old.vx+(chosen.vx-old.vx)*blend,old.vy+(chosen.vy-old.vy)*blend);
}

static void sfCoopUpdateBossThreat(float dt)
{
    const float skill=sfCoopBossAiSkill();
    const float decay=std::exp(-dt*(1.15f-.45f*skill));
    for(auto &v:sfCoop.bossThreat) v*=decay;
    sfCoop.bossAggroTime=std::max(0.0f,sfCoop.bossAggroTime-dt);

    int candidate=-1;float threat=0;
    for(int owner=0;owner<2;++owner) if(sfCoopShip(owner)->pv>0 && sfCoop.bossThreat[owner]>threat) {
        threat=sfCoop.bossThreat[owner];candidate=owner;
    }
    const float threshold=sfCoopProfile().health*(.030f-.020f*skill);
    if(candidate>=0 && threat>=threshold) {
        if(sfCoop.bossAggressor<0 || sfCoopShip(sfCoop.bossAggressor)->pv<=0 ||
           candidate==sfCoop.bossAggressor ||
           sfCoop.bossThreat[candidate]>sfCoop.bossThreat[sfCoop.bossAggressor]*1.18f) {
            sfCoop.bossAggressor=candidate;
        }
        sfCoop.bossAggroTime=1.4f+1.8f*skill;
        if(!sfCoop.chargeActive)
            sfCoop.chargeCooldown=std::min(sfCoop.chargeCooldown,1.25f-.90f*skill);
    }
    if(sfCoop.bossAggroTime<=0 && (sfCoop.bossAggressor<0 || sfCoopShip(sfCoop.bossAggressor)->pv<=0))
        sfCoop.bossAggressor=-1;
}
static void sfCoopUpdateBossMissileDodge(float dt)
{
    if(dt<=0) return;
    const float skill=sfCoopBossAiSkill();
    sfCoop.bossDodgeCooldown=std::max(0.0f,sfCoop.bossDodgeCooldown-dt);
    tuplv desired(0,0);
    if(sfCoop.bossDodgeCooldown<=0) {
        float bestT=std::numeric_limits<float>::max();
        for(const auto &shot:sfCoop.shots) {
            if(shot.owner<0 || shot.kind!=4 || shot.life<=0) continue;
            const float rx=shot.position.x-sfCoop.position.x,ry=shot.position.y-sfCoop.position.y;
            const float vx=shot.velocity.vx-sfCoop.motion.velocity.vx,vy=shot.velocity.vy-sfCoop.motion.velocity.vy;
            const float vv=vx*vx+vy*vy;
            if(vv<1) continue;
            const float horizon=.45f+.70f*skill;
            const float t=std::clamp(-(rx*vx+ry*vy)/vv,0.0f,horizon);
            const float miss=vlong(rx+vx*t,ry+vy*t);
            const float reaction=.46f-.28f*skill;
            if(t<reaction || miss>sfCoopBossRadius()*(1.08f+.18f*skill) || t>=bestT) continue;
            const float speed=std::sqrt(vv);
            const float cross=vx*(-ry)-vy*(-rx);
            const float sign=cross>=0 ? 1.0f : -1.0f;
            const float dodgeSpeed=sfArenaW*(.07f+.13f*skill);
            desired=tuplv(-vy/speed*sign*dodgeSpeed,vx/speed*sign*dodgeSpeed);
            bestT=t;
        }
        if(bestT<std::numeric_limits<float>::max()) sfCoop.bossDodgeCooldown=.32f+.28f*(1.0f-skill);
    }
    const float maxAccel=sfArenaW*(.50f+.55f*skill);
    auto approach=[&](float current,float target) {
        return current+std::clamp(target-current,-maxAccel*dt,maxAccel*dt);
    };
    sfCoop.bossDodgeVelocity.vx=approach(sfCoop.bossDodgeVelocity.vx,desired.vx);
    sfCoop.bossDodgeVelocity.vy=approach(sfCoop.bossDodgeVelocity.vy,desired.vy);
    sfCoop.bossDodgeOffset.x+=sfCoop.bossDodgeVelocity.vx*dt;
    sfCoop.bossDodgeOffset.y+=sfCoop.bossDodgeVelocity.vy*dt;
    const float decay=std::exp(-dt*(1.7f-.35f*skill));
    sfCoop.bossDodgeOffset.x*=decay;sfCoop.bossDodgeOffset.y*=decay;
    const float maxOffset=sfArenaW*(.025f+.045f*skill);
    const float length=vlong(sfCoop.bossDodgeOffset.x,sfCoop.bossDodgeOffset.y);
    if(length>maxOffset) {
        sfCoop.bossDodgeOffset.x*=maxOffset/length;sfCoop.bossDodgeOffset.y*=maxOffset/length;
    }
}
static float sfCoopFireDelay(float heat)
{
    return .20f+.010f*sfShipHeat(heat);
}
static constexpr float SF_COOP_PHASER_DAMAGE=12.0f;
static float sfCoopPhaserDps(int owner)
{
    const auto *ship=sfCoopShip(owner);
    if(!ship || ship->pv<=0) return 0.0f;
    return SF_COOP_PHASER_DAMAGE/std::max(.001f,sfCoopFireDelay(ship->nrj));
}
static float sfCoopConeBossHitFraction(int owner)
{
    if(owner<0 || owner>1 || !sfKineticSuctionVisible(owner)) return -1.0f;
    const auto *ship=sfCoopShip(owner);
    if(!ship || ship->pv<=0 || sfCoop.health<=0) return -1.0f;
    const float diameter=sfKineticShipDiameter(ship),dir=owner==0 ? 1.0f : -1.0f;
    const float noseX=ship->x,noseY=ship->y+dir*diameter*.42f;
    const float range=diameter*sfKineticSurgeMiningRangeDiameters();
    const float radius=sfCoopBossRadius()*.75f;
    if(vlong(sfCoop.position.x-noseX,sfCoop.position.y-noseY)<=radius) return 0.0f;
    float best=2.0f;
    auto consider=[&](float x,float y) {
        if(!sfKineticSurgeConeContains(owner,noseX,noseY,x,y,range)) return;
        const float forward=owner==0 ? y-noseY : noseY-y;
        if(forward>=0) best=std::min(best,std::clamp(forward/std::max(1.0f,range),0.0f,1.0f));
    };
    consider(sfCoop.position.x,sfCoop.position.y);
    constexpr int samples=96;
    for(int i=0;i<samples;++i) {
        const float a=i*2.0f*float(PI)/samples;
        consider(sfCoop.position.x+std::cos(a)*radius,sfCoop.position.y+std::sin(a)*radius);
    }
    return best<=1.0f ? best : -1.0f;
}
static void sfCoopApplyMiningConeBossDamage(float dt)
{
    if(dt<=0 || sfCoop.phase!=SfCoopPhase::Combat || sfCoop.health<=0) return;
    for(int owner=0;owner<2 && sfCoop.health>0;++owner) {
        if(owner==0 && sfActiveMode==SF_COOP_AI) continue; // teammate cone = mining only.
        const float hit=sfCoopConeBossHitFraction(owner);
        if(hit<0) continue;
        const float damage=sfCoopPhaserDps(owner)*sfKineticConeBossDpsMultiplier(hit)*dt;
        if(damage<=0) continue;
        sfCoopRegisterBossDamage(owner,damage);
        sfCoop.health=std::max(0.0f,sfCoop.health-damage);
        sfCoop.hit=std::max(sfCoop.hit,.06f);
    }
}
static float sfCoopShotSpread(float heat,unsigned sequence,int owner)
{
    (void)sequence;(void)owner;
    return sfMainShotSpreadRadians(heat,sfMainShotRandomUnit());
}
static constexpr float SF_COOP_PASSIVE_RECHARGE_BASE_UPDATES_PER_SECOND=60.0f;
static constexpr float SF_COOP_PASSIVE_RECHARGE_EASIEST_MULTIPLIER=4.0f;
static constexpr float SF_COOP_PASSIVE_RECHARGE_HARDEST_MULTIPLIER=2.0f;
static float sfCoopPassiveRechargeMultiplier(int dangerIndex)
{
    const float t=std::clamp(dangerIndex,0,SF_BOSS_DANGER_COUNT-1)/float(SF_BOSS_DANGER_COUNT-1);
    return SF_COOP_PASSIVE_RECHARGE_EASIEST_MULTIPLIER+
        (SF_COOP_PASSIVE_RECHARGE_HARDEST_MULTIPLIER-
         SF_COOP_PASSIVE_RECHARGE_EASIEST_MULTIPLIER)*t;
}
static float sfCoopPassiveRechargeHeat(float heat,float dt,int dangerIndex)
{
    heat=sfShipHeat(heat);
    if(dt<=0) return heat;
    const float updates=SF_COOP_PASSIVE_RECHARGE_BASE_UPDATES_PER_SECOND*
        sfCoopPassiveRechargeMultiplier(dangerIndex)*dt;
    return sfShipHeat(heat*std::pow(.997f,updates));
}
static float sfCoopPassiveRechargeHeat(float heat,float dt)
{
    return sfCoopPassiveRechargeHeat(heat,dt,sfBossDangerIndex);
}
static bool sfCoopFire(int owner)
{
    auto *ship=sfCoopShip(owner);
    if (sfCoop.phase!=SfCoopPhase::Combat || ship->pv<=0 || sfCoop.shots.size()>=600 ||
        sfCoop.cooldown[owner]>0) return false;
    const float heatBefore=sfShipHeat(ship->nrj);
    const float speed=sfMainShotSpeed(heatBefore);
    const bool missile=sfSpendMainEnergy(ship);
    float angle=(owner==0 ? 1 : -1)*float(PI)*.5f;
    if (owner==0 && sfActiveMode==SF_COOP_AI) {
        const auto aim=sfBossGhost().intercept(tupl(ship->x,ship->y),speed,.9f);
        angle=std::atan2(aim.y-ship->y,aim.x-ship->x);
    }
    const unsigned sequence=++sfCoop.shotSequence[owner];
    if (!missile) angle+=sfCoopShotSpread(heatBefore,sequence,owner);
    const float radius=sfCoopShipRadius();
    sfCoopEmit(tupl(ship->x+std::cos(angle)*radius,ship->y+std::sin(angle)*radius),
               angle,missile ? sfArenaW*.65f : speed,owner,missile ? 60 : SF_COOP_PHASER_DAMAGE,missile ? 4 : 0);
    sfCoop.cooldown[owner]=sfCoopFireDelay(heatBefore);
    return true;
}

static constexpr float SF_COOP_CHARGE_DURATION=.90f;
static bool sfCoopChargeImpactActive()
{
    if(!sfCoop.chargeActive) return false;
    const float phase=sfCoop.chargeTime/SF_COOP_CHARGE_DURATION;
    return phase>=.08f && phase<=.62f;
}
static void sfCoopBossContact(int owner,float dt)
{
    auto *ship=sfCoopShip(owner);
    if (ship->pv<=0 || sfCoop.phase!=SfCoopPhase::Combat || dt<=0) return;
    if(sfCoop.chargeActive) {
        // Only explicit boss charges are kinetic. Ordinary body contact remains on the historical path.
        if(!sfCoopChargeImpactActive() || sfCoop.chargeHit[owner]) return;
        const float dx=sfCoop.position.x-ship->x,dy=sfCoop.position.y-ship->y,d=std::max(1.0f,vlong(dx,dy));
        const auto raw=sfResolveKinetic(SF_KINETIC_BOSS_BASE_DAMAGE,sfKineticBossMass(sfCoop.encounter),
            {sfCoop.motion.velocity.vx,sfCoop.motion.velocity.vy},
            {sfObserved[owner].velocity.vx,sfObserved[owner].velocity.vy},dx/d,dy/d,sfKineticReferenceSpeed(sfArenaW));
        auto solved=raw;
        if(raw.suggestedLayer!=SfKineticLayer::None)
            solved=sfApplyKineticLayer(raw,raw.suggestedLayer,sfKineticEnergyFraction(ship->nrj),sfKineticSurgePower(owner));
        sfAddShipHeat(ship,solved.energyCost);
        const float incoming=solved.residualDamage; // Never apply hostile danger to kinetic damage.
        ship->pv=std::max(0.0f,ship->pv-sfApplyShieldImpact(ship,incoming));
        const float strength=std::clamp(.25f+solved.dissipationFraction*.75f,0.0f,1.0f);
        if(raw.maxRadiusShipDiameters>0) {
            sfKineticTriggerWave(owner,raw.maxRadiusShipDiameters,strength);
            SDL_Log("KINETIC_WAVE owner=%d start=0 currentRadius=0 maxRadius=%.3f impactStrength=%.3f",
                owner,sfKineticShipDiameter(ship)*raw.maxRadiusShipDiameters,strength);
        }
        SDL_Log("KINETIC_IMPACT owner=%d massFactor=%.5f relativeSpeed=%.3f impactSpeed=%.3f rawDamage=%.5f residualDamage=%.5f selectedRange=%d maxRadius=%.3f energyCost=%.8f",
            owner,raw.massFactor,raw.relativeSpeed,raw.impactSpeed,raw.rawDamage,solved.residualDamage,
            raw.selectedRange,sfKineticShipDiameter(ship)*raw.maxRadiusShipDiameters,solved.energyCost);
        sfCoopEmitRedDust(owner,6);
        sfCoop.chargeHit[owner]=true;sfCoop.soundHit=true;
        return;
    }
    const float incomingPerSecond=sfApplyHostileDanger(650.0f);
    ship->pv=std::max(0.0f,ship->pv-sfApplyShieldContinuousImpact(ship,incomingPerSecond,dt));
    sfCoop.soundHit=true;sfCoopEmitRedDust(owner,2);
}
static void sfCoopAsteroidHurt(int owner,float legacyDamage)
{
    auto *ship=sfCoopShip(owner);
    if (ship->pv<=0 || sfCoop.phase!=SfCoopPhase::Combat) return;
    const float incoming=legacyDamage; // Kinetic asteroid damage is shared with classic: no hostile danger multiplier.
    ship->pv=std::max(0.0f,ship->pv-sfApplyShieldImpact(ship,incoming));
    sfCoop.soundHit=true;
}

static void sfCoopMovePlayers(float dt)
{
    const float radius=sfCoopShipRadius();
    for (int owner=0;owner<2;++owner) {
        auto *ship=sfCoopShip(owner); auto &control=sfCoop.controls[owner];
        sfCoop.redDustCooldown[owner]=std::max(0.0f,sfCoop.redDustCooldown[owner]-dt);
        ship->vib(sfCoop.time);
        sfCoop.invulnerable[owner]=std::max(0.0f,sfCoop.invulnerable[owner]-dt);
        if (ship->pv<=0) continue;
        const tupl previous(ship->x,ship->y);
        if (owner==0 && sfActiveMode==SF_COOP_AI) control.velocity=sfCoopAiVelocity(dt);
        else if (control.down) {
            const float distance=vlong(control.target.x-ship->x,control.target.y-ship->y);
            control.velocity=sfUnitVelocity(previous,control.target,std::min(sfArenaW*1.2f,distance*11));
        } else { const float decay=std::exp(-dt*8);control.velocity.vx*=decay;control.velocity.vy*=decay; }
        const float halfWidth=std::max(radius,ship->w*.5f),halfHeight=ship->h*.5f;
        ship->x=std::clamp(ship->x+control.velocity.vx*dt,halfWidth,sfArenaW-halfWidth);
        ship->y=std::clamp(ship->y+control.velocity.vy*dt,sfArenaH*.105f+halfHeight,sfArenaH*.895f-halfHeight);
        ship->vx=ship->vy=0; ship->startup();
        sfObserved[owner].observe(tupl(ship->x,ship->y),dt);
        ship->nrj=sfCoopPassiveRechargeHeat(ship->nrj,dt);
        const float collision=sfCoopBossRadius()*.72f+radius;
        const float distance=vlong(ship->x-sfCoop.position.x,ship->y-sfCoop.position.y);
        if (distance<collision) {
            const tuplv push=sfUnitVelocity(sfCoop.position,tupl(ship->x,ship->y),collision);
            ship->x=sfCoop.position.x+push.vx; ship->y=sfCoop.position.y+push.vy;
            sfCoopBossContact(owner,dt); ship->startup();
        }
        sfCoop.cooldown[owner]-=dt;
        if(owner==0 && sfActiveMode==SF_COOP_AI) {
            sfCoopAiUpdateMiningCone();
            if(sfCoop.cooldown[owner]<=0 && sfCoopAiShouldFire()) sfCoopFire(owner);
        }
    }
    for (int owner=0;owner<2;++owner) if (sfCoopShip(owner)->pv<=0) {
        const auto *ally=sfCoopShip(1-owner); auto *down=sfCoopShip(owner);
        if (ally->pv>0 && sfCoop.revives>0 && vlong(ally->x-down->x,ally->y-down->y)<sfArenaW*.16f) {
            sfCoop.reviveProgress[owner]+=dt;
            if (sfCoop.reviveProgress[owner]>=2) {
                down->pv=450; down->nrj=15; --sfCoop.revives;
                sfCoop.invulnerable[owner]=2; sfCoop.reviveProgress[owner]=0;
            }
        } else sfCoop.reviveProgress[owner]=0;
    }
}

static void sfCoopDefences(float dt)
{
    sfCoop.turretTime=std::max(0.0f,sfCoop.turretTime-dt);
    for (int i=0;i<SF_TURRET_COUNT;++i) {
        auto &turret=sfTurrets[i];
        turret.alert=sfCoop.turretTime>0;
        turret.energy=std::min(100.0f,turret.energy+dt*3);
        turret.deploy=std::clamp(turret.deploy+dt*(turret.alert ? 1.8f : -2.0f),0.0f,1.0f);
        turret.cooldown-=dt; turret.flash=std::max(0.0f,turret.flash-dt);
        const tupl base=sfTurretBase(i);
        turret.virtualTarget=sfBossGhost().intercept(sfTurretMuzzle(i),sfArenaW*1.25f,.9f);
        const float desired=std::atan2(turret.virtualTarget.y-base.y,turret.virtualTarget.x-base.x);
        const float delta=std::remainder(desired-turret.angle,2*float(PI));
        turret.angle+=std::clamp(delta,-dt*5,dt*5);
        if (turret.alert && turret.energy>=12 && turret.deploy>=.99f && turret.cooldown<=0 && std::abs(delta)<.1f) {
            turret.energy-=12;
            turret.flash=.16f;
            sfCoopEmit(sfTurretMuzzle(i),turret.angle,sfArenaW*1.25f,i/SF_TURRETS_PER_TEAM,2.5f,5);
            turret.cooldown=1.6f+(i%SF_TURRETS_PER_TEAM)*.12f;
        }
    }
}

static void sfCoopProjectiles(float dt)
{
    std::vector<SfCoopShot> bursts;
    for (auto &shot : sfCoop.shots) {
        shot.previous=shot.position; shot.age+=dt; shot.life-=dt;
        if (shot.kind==4) {
            const float speed=sfArenaW*std::min(1.5f,.65f+1.8f*shot.age);
            const auto aim=sfBossGhost().intercept(shot.position,speed,.7f);
            const float desired=std::atan2(aim.y-shot.position.y,aim.x-shot.position.x);
            shot.phase+=std::clamp(std::remainder(desired-shot.phase,2*float(PI)),-dt*1.1f,dt*1.1f);
            shot.velocity=tuplv(std::cos(shot.phase)*speed,std::sin(shot.phase)*speed);
        }
        if (shot.kind==1 && shot.age<.75f) {
            const int target=Spritej1->pv<=0 ? 1 : Spritej2->pv<=0 ? 0 :
                (vlong(shot.position.x-Spritej1->x,shot.position.y-Spritej1->y)<vlong(shot.position.x-Spritej2->x,shot.position.y-Spritej2->y) ? 0 : 1);
            const auto aim=sfShipGhost(target).intercept(shot.position,sfArenaW*sfCoopProfile().shotSpeed,.35f);
            const float desired=std::atan2(aim.y-shot.position.y,aim.x-shot.position.x);
            shot.phase+=std::clamp(std::remainder(desired-shot.phase,2*float(PI)),-dt*.8f,dt*.8f);
            const float speed=vlong(shot.velocity.vx,shot.velocity.vy);
            shot.velocity=tuplv(std::cos(shot.phase)*speed,std::sin(shot.phase)*speed);
        }
        if (shot.kind==2) {
            shot.velocity.vx*=std::exp(-dt*1.4f);shot.velocity.vy*=std::exp(-dt*1.4f);
            if (shot.age>1.2f && shot.life>0) for (int owner=0;owner<2;++owner) {
                const auto *ship=sfCoopShip(owner);
                if (ship->pv>0 && vlong(ship->x-shot.position.x,ship->y-shot.position.y)<sfArenaW*.13f) {
                    shot.life=0;
                    for (int ray=0;ray<8;++ray) {
                        SfCoopShot burst;burst.position=burst.previous=shot.position;
                        const float angle=ray*float(PI)*.25f;
                        burst.velocity=tuplv(std::cos(angle)*sfArenaW*.34f,std::sin(angle)*sfArenaW*.34f);
                        burst.damage=shot.damage;burst.radius=sfArenaW*.008f;bursts.push_back(burst);
                    }
                    break;
                }
            }
        }
        const float wobble=shot.kind==3 ? std::sin(shot.age*8)*sfArenaW*.10f : 0;
        shot.position.x+=(shot.velocity.vx-std::sin(shot.phase)*wobble)*dt;
        shot.position.y+=(shot.velocity.vy+std::cos(shot.phase)*wobble)*dt;
        if (shot.life<=0) continue;
        if (shot.owner>=0) {
            for (auto *rock : sa1) if (rock->pv>0 &&
                sfSegmentDistance(shot.previous,shot.position,tupl(rock->x,rock->y))<std::max(rock->sw,rock->sh)*.5f+shot.radius) {
                sprite impact;impact.setxywh(shot.position.x,shot.position.y,shot.radius*2,shot.radius*2);
                impact.vx=shot.velocity.vx/60;impact.vy=shot.velocity.vy/60;
                sfMineAsteroid(rock,&impact);shot.life=0;break;
            }
            if (shot.life>0 && sfSegmentDistance(shot.previous,shot.position,sfCoop.position)<sfCoopBossRadius()*.75f+shot.radius) {
                const float energyStress=.018f+.102f*std::clamp(shot.damage/60.0f,0.0f,1.0f);
                sfCoop.bossEnergyReserve=std::clamp(sfCoop.bossEnergyReserve-energyStress,0.0f,1.0f);
                sfCoopRegisterBossDamage(shot.owner,shot.damage);
                sfCoop.health=std::max(0.0f,sfCoop.health-shot.damage);shot.life=0;sfCoop.hit=.10f;
            }
        } else if (shot.kind!=2 || shot.age>1.2f) {
            for (int owner=0;owner<2;++owner) {
                auto *ship=sfCoopShip(owner);
                if (ship->pv>0 && sfSegmentDistance(shot.previous,shot.position,tupl(ship->x,ship->y))<sfCoopShipRadius()+shot.radius) {
                    sfCoopHurt(owner,shot.damage);shot.life=0;break;
                }
            }
        }
        if (shot.position.x<-50 || shot.position.x>sfArenaW+50 || shot.position.y<-50 || shot.position.y>sfArenaH+50) shot.life=0;
    }
    sfCoop.shots.erase(std::remove_if(sfCoop.shots.begin(),sfCoop.shots.end(),[](const auto &s){return s.life<=0;}),sfCoop.shots.end());
    for (const auto &shot : bursts) if (sfCoop.shots.size()<600) sfCoop.shots.push_back(shot);
    for (auto &beam : sfCoop.beams) {
        beam.age+=dt;
        if (beam.age>=beam.warning && beam.age<beam.warning+.5f) {
            const tupl end(beam.origin.x+std::cos(beam.angle)*sfArenaH*2,beam.origin.y+std::sin(beam.angle)*sfArenaH*2);
            for (int owner=0;owner<2;++owner) {
                auto *ship=sfCoopShip(owner);
                if (sfSegmentDistance(beam.origin,end,tupl(ship->x,ship->y))<sfCoopShipRadius()+sfArenaW*.016f)
                    sfCoopHurt(owner,sfCoopProfile().damage*1.6f);
            }
        }
    }
    sfCoop.beams.erase(std::remove_if(sfCoop.beams.begin(),sfCoop.beams.end(),[](const auto &b){return b.age>b.warning+.5f;}),sfCoop.beams.end());
    for (auto &wave : sfCoop.waves) {
        const float previous=wave.radius;wave.age+=dt;
        wave.radius=std::max(0.0f,wave.age-.7f)*sfArenaW*.48f;
        if (wave.age>.7f) for (int owner=0;owner<2;++owner) {
            auto *ship=sfCoopShip(owner);
            const float distance=vlong(ship->x-wave.origin.x,ship->y-wave.origin.y);
            if (distance+sfCoopShipRadius()>=previous && distance-sfCoopShipRadius()<=wave.radius)
                sfCoopHurt(owner,sfCoopProfile().damage*1.3f);
        }
    }
    sfCoop.waves.erase(std::remove_if(sfCoop.waves.begin(),sfCoop.waves.end(),[](const auto &w){return w.radius>sfArenaH*1.2f;}),sfCoop.waves.end());
    sfCoopEnemyCollectWhiteDust();
}

static void sfCoopBonus(float dt)
{
    sfCoop.bonusTimer-=dt;
    if (sfCoop.bonusLife<=0 && sfCoop.bonusTimer<=0) {
        sfCoop.bonusTimer=18+rand()%15;sfCoop.bonusLife=18;
        sfCoop.bonusPosition=tupl(sfArenaW*(.15f+(rand()%700)/1000.0f),sfArenaH*.5f);
        sfCoop.bonusVelocity=tuplv(sfArenaW*(rand()%2 ? .055f : -.055f),sfArenaH*(rand()%2 ? .025f : -.025f));
    }
    if (sfCoop.bonusLife<=0) return;
    sfCoop.bonusLife=std::max(0.0f,sfCoop.bonusLife-dt);
    auto &p=sfCoop.bonusPosition;auto &v=sfCoop.bonusVelocity;
    p.x+=v.vx*dt;p.y+=v.vy*dt;
    if ((p.x<sfArenaW*.08f && v.vx<0) || (p.x>sfArenaW*.92f && v.vx>0)) v.vx=-v.vx;
    if ((p.y<sfArenaH*.15f && v.vy<0) || (p.y>sfArenaH*.85f && v.vy>0)) v.vy=-v.vy;
    for (int owner=0;owner<2;++owner) {
        const auto *ship=sfCoopShip(owner);
        if (ship->pv>0 && vlong(ship->x-p.x,ship->y-p.y)<sfCoopShipRadius()+std::min(sfArenaW,sfArenaH)*.032f) {
            sfCoop.bonusLife=0;sfCoop.turretTime=14;break;
        }
    }
}

static constexpr float SF_COOP_BOSS_KINETIC_DISSIPATION=.55f;
static constexpr float SF_COOP_BOSS_KINETIC_FIELD_RADIUS_SCALE=1.08f;
static constexpr float SF_COOP_BOSS_ASTEROID_BASE_DAMAGE=80.0f;
static constexpr float SF_COOP_BOSS_KINETIC_RING_DURATION=.46f;
static constexpr float SF_COOP_BOSS_KINETIC_WAVE_DURATION=SF_COOP_BOSS_KINETIC_RING_DURATION;
static constexpr Uint8 SF_COOP_BOSS_KINETIC_RING_ALPHA=72;
static constexpr float SF_COOP_BOSS_RESERVE_REGEN_MAX=.18f;
static constexpr float SF_COOP_BOSS_RESERVE_REGEN_MIN=SF_COOP_BOSS_RESERVE_REGEN_MAX/9.0f;
static constexpr float SF_COOP_ASTEROID_SPAWN_SECONDS=3.5f;

static float sfCoopBossReserveRegenPerSecond(int dangerIndex)
{
    dangerIndex=std::clamp(dangerIndex,0,SF_BOSS_DANGER_COUNT-1);
    const float t=dangerIndex/float(SF_BOSS_DANGER_COUNT-1);
    return SF_COOP_BOSS_RESERVE_REGEN_MIN+
        (SF_COOP_BOSS_RESERVE_REGEN_MAX-SF_COOP_BOSS_RESERVE_REGEN_MIN)*t;
}
static float sfCoopBossKineticRegenPerSecond(int difficulty)
{
    return sfCoopBossReserveRegenPerSecond(difficulty);
}
static float sfCoopRegenerateBossReserveValue(float reserve,float dt,int difficulty)
{
    reserve=std::clamp(reserve,0.0f,1.0f);
    if(dt<=0) return reserve;
    return std::min(1.0f,reserve+sfCoopBossReserveRegenPerSecond(difficulty)*dt);
}
static float sfCoopRegenerateBossKineticReserveValue(float reserve,float dt,int difficulty)
{
    return sfCoopRegenerateBossReserveValue(reserve,dt,difficulty);
}
static void sfCoopRegenerateBossReserves(float dt)
{
    const int dangerIndex=std::clamp(sfBossDangerIndex,0,SF_BOSS_DANGER_COUNT-1);
    sfCoop.bossEnergyReserve=sfCoopRegenerateBossReserveValue(sfCoop.bossEnergyReserve,dt,dangerIndex);
    sfCoop.bossKineticReserve=sfCoopRegenerateBossReserveValue(sfCoop.bossKineticReserve,dt,dangerIndex);
}
static Uint8 sfCoopBossKineticVisibilityAlpha(float reserve)
{
    // Healthy field is intentionally discreet; depleted/stressed field becomes
    // more visible while always staying semi-transparent.
    reserve=std::clamp(reserve,0.0f,1.0f);
    return Uint8(std::clamp(18.0f+(1.0f-reserve)*58.0f,18.0f,76.0f));
}
static constexpr size_t SF_COOP_ASTEROID_CAP=24;

static float sfCoopBossKineticRingRadius(float remaining,float fieldRadius)
{
    const float progress=1.0f-std::clamp(remaining/std::max(.001f,SF_COOP_BOSS_KINETIC_RING_DURATION),0.0f,1.0f);
    const float smooth=progress*progress*(3.0f-2.0f*progress);
    return std::max(0.0f,fieldRadius)*smooth;
}
static float sfCoopBossKineticWaveRadius(float progress)
{
    const float fieldRadius=sfCoopBossRadius()*SF_COOP_BOSS_KINETIC_FIELD_RADIUS_SCALE;
    const float remaining=(1.0f-std::clamp(progress,0.0f,1.0f))*SF_COOP_BOSS_KINETIC_RING_DURATION;
    return sfCoopBossKineticRingRadius(remaining,fieldRadius);
}
static void sfCoopAdvanceBossKineticCooldowns(float dt)
{
    if(dt<=0) return;
    for(auto it=sfCoop.bossAsteroidCooldown.begin();it!=sfCoop.bossAsteroidCooldown.end();) {
        it->second-=dt;
        if(it->second<=0) it=sfCoop.bossAsteroidCooldown.erase(it); else ++it;
    }
}
static bool sfCoopBossKineticAsteroidImpact(sprite *rock)
{
    if(!rock || rock->pv<=0 || sfCoop.phase!=SfCoopPhase::Combat || sfCoop.health<=0) return false;
    auto active=sfCoop.bossAsteroidCooldown.find(rock);
    if(active!=sfCoop.bossAsteroidCooldown.end() && active->second>0) return false;
    float dx=rock->x-sfCoop.position.x,dy=rock->y-sfCoop.position.y;
    float distance=vlong(dx,dy);
    if(distance<1.0f) {dx=1.0f;dy=0.0f;distance=1.0f;}
    const float fieldRadius=sfCoopBossRadius()*SF_COOP_BOSS_KINETIC_FIELD_RADIUS_SCALE;
    const float rockRadius=std::max(rock->w,rock->h)*.5f;
    if(distance>fieldRadius+rockRadius) return false;
    const float nx=dx/distance,ny=dy/distance;
    const auto raw=sfResolveKinetic(SF_COOP_BOSS_ASTEROID_BASE_DAMAGE,
        sfKineticMassFactorFromArea(std::max(0.0f,rock->w*rock->h),sfArenaH),
        {rock->vx*60.0f,rock->vy*60.0f},
        {sfCoop.motion.velocity.vx,sfCoop.motion.velocity.vy},
        nx,ny,sfKineticReferenceSpeed(sfArenaW));
    if(raw.rawDamage<=.001f) return false;
    const float residual=raw.rawDamage*(1.0f-SF_COOP_BOSS_KINETIC_DISSIPATION);
    sfCoop.health=std::max(0.0f,sfCoop.health-residual);
    sfCoop.hit=std::max(sfCoop.hit,.10f);
    const float stress=.035f+.125f*std::clamp(raw.rawDamage/std::max(1.0f,SF_COOP_BOSS_ASTEROID_BASE_DAMAGE),0.0f,1.0f);
    sfCoop.bossKineticReserve=std::clamp(sfCoop.bossKineticReserve-stress,0.0f,1.0f);
    sfCoop.bossKineticFlash=SF_COOP_BOSS_KINETIC_RING_DURATION;
    sfCoop.bossAsteroidCooldown[rock]=.42f;
    sfFieldCollisionSound=true;

    // Boss field only absorbs/deflects: the asteroid survives.
    const float bossVx=sfCoop.motion.velocity.vx/60.0f,bossVy=sfCoop.motion.velocity.vy/60.0f;
    const float rvx=rock->vx-bossVx,rvy=rock->vy-bossVy;
    const float vn=rvx*nx+rvy*ny;
    const float tx=rvx-vn*nx,ty=rvy-vn*ny;
    const float retained=1.0f-SF_COOP_BOSS_KINETIC_DISSIPATION;
    const float bouncedNormal=(vn<0 ? -vn : vn)*retained;
    rock->vx=bossVx+nx*bouncedNormal+tx*retained;
    rock->vy=bossVy+ny*bouncedNormal+ty*retained;
    rock->x=sfCoop.position.x+nx*(fieldRadius+rockRadius+2.0f);
    rock->y=sfCoop.position.y+ny*(fieldRadius+rockRadius+2.0f);
    rock->startup();

    SDL_Log("BOSS_KINETIC_FIELD raw=%.3f dissipated=%.3f residual=%.3f hp=%.3f asteroidSurvives=true",
        raw.rawDamage,raw.rawDamage*SF_COOP_BOSS_KINETIC_DISSIPATION,residual,sfCoop.health);
    return true;
}
static void sfCoopBossKineticField()
{
    for(auto *rock:sa1) sfCoopBossKineticAsteroidImpact(rock);
}
static void sfCoopAdvanceAsteroidSpawner(float dt)
{
    if(sfCoop.phase!=SfCoopPhase::Combat || dt<=0) return;
    sfCoop.asteroidSpawnTimer-=dt;
    if(sfCoop.asteroidSpawnTimer>0) return;
    while(sfCoop.asteroidSpawnTimer<=0) sfCoop.asteroidSpawnTimer+=SF_COOP_ASTEROID_SPAWN_SECONDS;
    if(sa1.size()+4>SF_COOP_ASTEROID_CAP) return;
    const int oldW=W,oldH=H,oldWidth=WIDTH,oldHeight=HEIGHT;const float oldK=k0;
    W=WIDTH=std::max(1,int(sfArenaW));H=HEIGHT=std::max(1,int(sfArenaH));k0=1;
    setasts(1);
    W=oldW;H=oldH;WIDTH=oldWidth;HEIGHT=oldHeight;k0=oldK;
}

static void sfCoopResources(float dt)
{
    sfCoopRegenerateBossReserves(dt);
    sfCoopAdvanceBossKineticCooldowns(dt);
    sfCoopAdvanceAsteroidSpawner(dt);
    sfLegacyFieldFrame(dt,sfCoopAsteroidHurt);
    sfCoopBossKineticField();
}

static void sfCoopWin()
{
    sfCoop.phase=SfCoopPhase::Dying;sfCoop.phaseTime=0;
    sfCoop.shots.clear();sfCoop.beams.clear();sfCoop.waves.clear();
    auto next=sfCampaignSave;next.cleared=std::max(next.cleared,sfCoop.encounter+1);
    next.selected=std::min(199,sfCoop.encounter+1);next.pending=true;
    next.victory={};next.victory.id=(uint64_t(std::time(nullptr))<<24)^(SDL_GetPerformanceCounter()&0xffffffu);
    for (const auto &entry : next.fame) next.victory.id=std::max(next.victory.id,entry.id+1);
    if (!next.victory.id) next.victory.id=1;
    next.victory.boss=sfCoop.encounter+1;next.victory.mode=sfActiveMode;
    next.victory.seconds=int(sfCoop.time);next.victory.date=std::time(nullptr);
    next.victory.score=std::max(0,int((sfCoop.encounter+1)*500+(Spritej1->pv+Spritej2->pv)*2-sfCoop.time*5));
    sfCoop.pendingSaved=sfSaveCampaign(next);
    if (!sfCoop.pendingSaved) { sfCampaignSave=next;sfCoop.error=sfCampaignStorageError; }
    sfCoop.names=sfCampaignSave.names;
    if (sfActiveMode==SF_COOP_AI && sfCoop.names[0].empty()) sfCoop.names[0]="ORION IA";
}

static tupl sfCoopBossBasePosition(float time)
{
    const auto &b=sfCoopProfile();
    const float progress=sfBossTravelProgress(sfCoop.encounter);
    const float t=time*(.35f+b.tier*.018f)*(1+.45f*progress)+b.index*.3f;
    float x=std::sin(t),y=std::sin(t*.71f)*.4f;
    switch(b.family) {
        case 1:x=std::sin(t)*.7f;y=std::sin(t*2)*.7f;break;
        case 2:x=std::sin(t*.8f);y=std::cos(t)*.75f;break;
        case 3:x=std::tanh(2*std::sin(t));y=std::sin(t*.5f)*.45f;break;
        case 4:x=std::cos(t)*.8f;y=std::sin(t)*.8f;break;
        case 5:x=std::sin(t*1.4f);y=std::sin(t*2.8f)*.55f;break;
        case 6:x=std::sin(t)*std::cos(t*.3f);y=std::cos(t*.7f)*.65f;break;
        case 7:x=std::sin(t*.7f);y=std::sin(t*1.3f)*.75f;break;
        case 8:x=std::tanh(1.5f*std::sin(t*.8f));y=std::cos(t*.8f)*.65f;break;
        case 9:x=std::sin(t)*.7f+std::sin(t*2.3f)*.2f;y=std::cos(t*1.4f)*.6f;break;
    }
    const float radius=sfCoopBossRadius();
    const float safeSpanX=std::max(.12f,.5f-(radius/std::max(1.0f,sfArenaW)*.92f+.018f));
    const float safeSpanY=std::max(.08f,.5f-(radius/std::max(1.0f,sfArenaH)*.92f+.105f));
    const float spanX=std::min(.18f+.30f*progress,safeSpanX);
    const float spanY=std::min(.08f+.30f*progress,safeSpanY);
    return tupl(sfArenaW*(.5f+spanX*x),sfArenaH*(.5f+spanY*y));
}

static void sfCoopStartCharge()
{
    if(sfCoop.chargeActive || sfCoop.phase!=SfCoopPhase::Combat) return;
    int target=(sfCoop.bossAggroTime>0 && sfCoop.bossAggressor>=0 &&
                sfCoopShip(sfCoop.bossAggressor)->pv>0)
        ? sfCoop.bossAggressor : (sfCoop.volley+sfCoop.encounter)&1;
    if(sfCoopShip(target)->pv<=0) target=1-target;
    if(sfCoopShip(target)->pv<=0) return;
    const float p=sfBossTravelProgress(sfCoop.encounter);
    const auto base=sfCoopBossBasePosition(sfCoop.time);
    const auto future=sfShipGhost(target).intercept(base,sfArenaW*(.70f+.45f*p),.55f);
    const float minX=sfArenaW*(.34f-.26f*p),maxX=sfArenaW*(.66f+.26f*p);
    const float minY=sfArenaH*(.30f-.20f*p),maxY=sfArenaH*(.70f+.20f*p);
    const float tx=std::clamp(future.x,minX,maxX),ty=std::clamp(future.y,minY,maxY);
    sfCoop.chargeOffsetX=tx-base.x;sfCoop.chargeOffsetY=ty-base.y;
    sfCoop.chargeTime=0;sfCoop.chargeHit={false,false};sfCoop.chargeActive=true;
}
static void sfCoopUpdateCharge(float dt)
{
    if(sfCoop.chargeActive) {
        const float skill=sfCoopBossAiSkill();
        if(sfCoop.bossAggroTime>0 && sfCoop.bossAggressor>=0 &&
           sfCoopShip(sfCoop.bossAggressor)->pv>0) {
            const int target=sfCoop.bossAggressor;
            const auto base=sfCoopBossBasePosition(sfCoop.time);
            const auto future=sfShipGhost(target).intercept(base,sfArenaW*(.70f+.45f*sfBossTravelProgress(sfCoop.encounter)),
                                                            .18f+.48f*skill);
            const float desiredX=future.x-base.x,desiredY=future.y-base.y;
            const float blend=1.0f-std::exp(-dt*(1.2f+4.0f*skill));
            sfCoop.chargeOffsetX+=(desiredX-sfCoop.chargeOffsetX)*blend;
            sfCoop.chargeOffsetY+=(desiredY-sfCoop.chargeOffsetY)*blend;
        }
        sfCoop.chargeTime+=dt;
        if(sfCoop.chargeTime>=SF_COOP_CHARGE_DURATION) {
  sfCoop.chargeActive=false;sfCoop.chargeTime=0;sfCoop.chargeOffsetX=sfCoop.chargeOffsetY=0;
  sfCoop.chargeHit={false,false};
  sfCoop.chargeCooldown=(8.0f-3.0f*sfBossTravelProgress(sfCoop.encounter))*(1.25f-.35f*skill);
        }
        return;
    }
    sfCoop.chargeCooldown-=dt;
    if(sfCoop.chargeCooldown<=0) sfCoopStartCharge();
}
static tupl sfCoopBossPosition(float time)
{
    auto base=sfCoopBossBasePosition(time);
    const float radius=sfCoopBossRadius();
    if(!sfCoop.chargeActive) {
        base.x=std::clamp(base.x+sfCoop.bossDodgeOffset.x,radius,sfArenaW-radius);
        base.y=std::clamp(base.y+sfCoop.bossDodgeOffset.y,radius+sfArenaH*.105f,sfArenaH-radius-sfArenaH*.105f);
        return base;
    }
    const float phase=std::clamp(sfCoop.chargeTime/SF_COOP_CHARGE_DURATION,0.0f,1.0f);
    const float envelope=phase<.55f ? phase/.55f : (1.0f-phase)/.45f;
    base.x=std::clamp(base.x+sfCoop.chargeOffsetX*envelope,radius,sfArenaW-radius);
    base.y=std::clamp(base.y+sfCoop.chargeOffsetY*envelope,radius+sfArenaH*.105f,sfArenaH-radius-sfArenaH*.105f);
    base.x=std::clamp(base.x+sfCoop.bossDodgeOffset.x,radius,sfArenaW-radius);
    base.y=std::clamp(base.y+sfCoop.bossDodgeOffset.y,radius+sfArenaH*.105f,sfArenaH-radius-sfArenaH*.105f);
    return base;
}

static void sfCoopTick(float dt)
{
    if (sfCoop.phase!=SfCoopPhase::Combat) return;
    sfKineticAdvanceSurges(dt);sfKineticAudioUpdate();
    sfCoop.time+=dt;sfCoop.hit=std::max(0.0f,sfCoop.hit-dt);
    sfCoop.bossKineticFlash=std::max(0.0f,sfCoop.bossKineticFlash-dt);
    const auto &boss=sfCoopProfile();
    sfCoopUpdateBossThreat(dt);
    sfCoopUpdateBossMissileDodge(dt);
    sfCoopUpdateCharge(dt);
    sfCoop.position=sfCoopBossPosition(sfCoop.time);
    sfCoop.motion.observe(sfCoop.position,dt);
    sfCoop.phaseNumber=sfCoop.health>boss.health*.65f ? 0 : sfCoop.health>boss.health*.3f ? 1 : 2;
    sfCoopMovePlayers(dt);sfCoopDefences(dt);
    sfCoop.attack-=dt;
    sfCoop.warning=sfCoop.chargeActive ? 0 : (sfCoop.attack<.65f ? 1-sfCoop.attack/.65f : 0);
    if (!sfCoop.chargeActive && sfCoop.attack<=0) {
        int pattern=boss.family;
        if ((sfCoop.phaseNumber>0 || boss.difficulty>=2) && sfCoop.volley%2) pattern=(boss.family+boss.tier+sfCoop.phaseNumber+boss.difficulty+1)%10;
        sfCoopPattern(pattern);
        sfCoop.attack=boss.interval*(sfCoop.phaseNumber==2 ? .82f : 1.0f);
        ++sfCoop.volley;
    }
    sfCoopProjectiles(dt);sfCoopApplyMiningConeBossDamage(dt);sfCoopResources(dt);sfCoopBonus(dt);
    if (sfCoop.health<=0 && (Spritej1->pv>0 || Spritej2->pv>0)) sfCoopWin();
    else if (Spritej1->pv<=0 && Spritej2->pv<=0) {
        sfCoop.phase=SfCoopPhase::Defeat;sfCoop.phaseTime=0;
        sfCoop.shots.clear();sfCoop.beams.clear();sfCoop.waves.clear();
    }
}

static void sfCampaignStart()
{
    sfLoadCampaign();sfCoop=SfCoopState{};sfKineticResetSurges();
    sfFixResetAsteroidField();sfFieldRemainder=0;
    if (!sfDuelShipStylesSaved) {
        for (int owner=0;owner<2;++owner) {
            const auto *s=sfCoopShip(owner);
            sfDuelShipStyles[owner]={s->w,s->h,s->sw,s->sh,s->speed,s->frames,s->frame,s->animated,s->boundedBreathing};
        }
        sfDuelShipStylesSaved=true;
    }
    sfCoop.encounter=std::clamp(sfCampaignSave.selected,0,std::min(199,sfCampaignSave.cleared));
    sfCoop.boss=sfBossIndex(sfCoop.encounter);sfCampaignPage=sfCoop.encounter/10;
    sfCoop.health=sfCoopProfile().health;sfCoop.attack=2.0f;
    sfCoop.position=tupl(sfArenaW*.5f,sfArenaH*.5f);
    for (int owner=0;owner<2;++owner) {
        auto *ship=sfCoopShip(owner);
        const float size=std::min(sfArenaW,sfArenaH)*.15f;
        ship->setxywh(sfArenaW*(owner==0 ? .35f : .65f),sfArenaH*(owner==0 ? .23f : .77f),size,size);
        ship->sw=ship->sh=size;ship->boundedBreathing=true;
        ship->pv=1000;ship->nrj=0;ship->animated=false;
        sfFixResetSpriteHistory(ship);
        sfCoop.controls[owner].target=tupl(ship->x,ship->y);
    }
    sfObserved={};
    if (sfCampaignSave.pending) {
        sfCoop.encounter=sfCampaignSave.victory.boss-1;sfCoop.boss=sfBossIndex(sfCoop.encounter);sfCoop.phase=SfCoopPhase::Name;
        sfCoop.names=sfCampaignSave.names;sfCoop.pendingSaved=true;
        if (sfCampaignSave.victory.mode==SF_COOP_AI && sfCoop.names[0].empty()) sfCoop.names[0]="ORION IA";
    }
}
static void sfCampaignRestoreDuelShips()
{
    if (!sfDuelShipStylesSaved) return;
    for (int owner=0;owner<2;++owner) {
        auto *ship=sfCoopShip(owner);const auto &style=sfDuelShipStyles[owner];
        ship->w=style.w;ship->h=style.h;ship->sw=style.sw;ship->sh=style.sh;
        ship->speed=style.speed;ship->frames=style.frames;ship->frame=style.frame;
        ship->animated=style.animated;ship->boundedBreathing=style.boundedBreathing;ship->startup();
    }
    sfDuelShipStylesSaved=false;
}
static void sfCoopPlaySounds(Mix_Chunk *orange,Mix_Chunk *blue,Mix_Chunk *boss,Mix_Chunk *hit,Mix_Chunk *collision)
{
    static Uint64 last=0;const Uint64 now=SDL_GetTicks64();
    if (now-last<120) return;last=now;
    for (int owner=0;owner<2;++owner) if (sfCoop.soundShot[owner]) {
        Mix_Chunk *sound=owner==0 ? orange : blue;
        if (sound) Mix_PlayChannel(4+owner,sound,0);sfCoop.soundShot[owner]=false;
    }
    if (sfCoop.soundBoss && boss) Mix_PlayChannel(6,boss,0);
    if ((sfCoop.soundHit || sfFieldMiningSound) && hit) Mix_PlayChannel(7,hit,0);
    if (sfFieldCollisionSound && collision) Mix_PlayChannel(3,collision,0);
    sfFieldCollisionSound=sfFieldMiningSound=false;
    sfCoop.soundBoss=sfCoop.soundHit=false;
}
static void sfCampaignSuspend()
{
    for (auto &control : sfCoop.controls) {control.down=false;control.finger=-1;control.velocity.set(0,0);}
    sfCoop.fireFingers.clear();sfKineticAudioReset();sfKineticResetSurges();
    sfObserved={};sfCoop.motion.valid=false;
    if (sfCoop.phase==SfCoopPhase::Combat) sfCoop.phase=SfCoopPhase::Paused;
    if (sfCoop.keyboard) {SDL_StopTextInput();sfCoop.keyboard=false;}
}

static SfCampaignTextures &sfCoopTextures(SDL_Renderer *renderer)
{
    for (auto &entry : sfCampaignTextures) if (entry.renderer==renderer) return entry;
    SfCampaignTextures entry;entry.renderer=renderer;
    entry.bosses=IMG_LoadTexture(renderer,"resources/assets/pict/campaign/bosses.png");
    entry.planets=IMG_LoadTexture(renderer,"resources/assets/pict/campaign/planets.png");
    entry.backgrounds=IMG_LoadTexture(renderer,"resources/assets/pict/campaign/nebulae.png");
    entry.orange=IMG_LoadTexture(renderer,"resources/assets/pict/remaster/player_orange.png");
    entry.blue=IMG_LoadTexture(renderer,"resources/assets/pict/remaster/player_blue.png");
    const char *paths[]={"aa1.png","aa2.png","aa3.png","aa4.png"};
    for(int i=0;i<4;++i) entry.rocks[i]=IMG_LoadTexture(renderer,(std::string("resources/assets/pict/")+paths[i]).c_str());
    entry.bonus=IMG_LoadTexture(renderer,"resources/assets/pict/Bonus_de_tourelles.png");
    entry.impact=IMG_LoadTexture(renderer,IMG_PATHpous);
    entry.explosion=IMG_LoadTexture(renderer,IMG_PATHexplo);
    entry.missile=IMG_LoadTexture(renderer,IMG_PATHmiss);
    // Reuse the historical duel orbs instead of approximating them with discs.
    // Owner 0 (orange/red) historically renders IMG_PATHtj2; owner 1 (blue) IMG_PATHtj1.
    entry.orbOrange=IMG_LoadTexture(renderer,IMG_PATHtj2);
    entry.orbBlue=IMG_LoadTexture(renderer,IMG_PATHtj1);
    for(SDL_Texture *texture : {entry.impact,entry.explosion,entry.missile,entry.orbOrange,entry.orbBlue})
        if(texture) SDL_SetTextureBlendMode(texture,SDL_BLENDMODE_BLEND);
    sfCampaignTextures.push_back(entry);return sfCampaignTextures.back();
}
static void sfCampaignForgetRenderer(SDL_Renderer *renderer)
{
    sfCampaignTextures.erase(std::remove_if(sfCampaignTextures.begin(),sfCampaignTextures.end(),
        [renderer](const auto &entry){return entry.renderer==renderer;}),sfCampaignTextures.end());
}
static SDL_Rect sfAtlasRect(SDL_Texture *texture,int index,int columns,int rows)
{
    int width=0,height=0;SDL_QueryTexture(texture,nullptr,nullptr,&width,&height);
    const int x=width*(index%columns)/columns,y=height*(index/columns)/rows;
    return {x,y,width*((index%columns)+1)/columns-x,height*((index/columns)+1)/rows-y};
}
static SDL_FPoint sfAtlasSafeUv(const SDL_Rect &src,int textureWidth,int textureHeight,float u,float v)
{
    // Sample half a texel inside the cell. This prevents linear-filter bleeding
    // without shaving a percentage off the artwork (the previous .5% inset could
    // visibly nibble narrow extremities on some boss cells).
    textureWidth=std::max(1,textureWidth);textureHeight=std::max(1,textureHeight);
    u=std::clamp(u,0.0f,1.0f);v=std::clamp(v,0.0f,1.0f);
    const float insetX=src.w>1 ? .5f : 0.0f,insetY=src.h>1 ? .5f : 0.0f;
    const float left=(src.x+insetX)/float(textureWidth);
    const float right=(src.x+src.w-insetX)/float(textureWidth);
    const float top=(src.y+insetY)/float(textureHeight);
    const float bottom=(src.y+src.h-insetY)/float(textureHeight);
    return {left+(right-left)*u,top+(bottom-top)*v};
}
static SDL_Rect sfBossAtlasRect(SDL_Texture *texture,int index)
{
    constexpr int columns[]{0,177,343,503,672,846,1025,1202,1392,1578,1774};
    constexpr int rows[]{0,156,332,511,693,887};
    int width,height;SDL_QueryTexture(texture,nullptr,nullptr,&width,&height);
    index=sfBossIndex(index);
    const int col=index%10,row=index/10;
    const int x=(columns[col]+2)*width/1774,y=(rows[row]+2)*height/887;
    const int right=(columns[col+1]-2)*width/1774,bottom=(rows[row+1]-2)*height/887;
    return {x,y,right-x,bottom-y};
}
static void sfCoopDisc(SDL_Renderer *renderer,float x,float y,float radius,SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);
    for (int row=-int(radius);row<=int(radius);++row) {
        const int extent=int(std::sqrt(std::max(0.0f,radius*radius-row*row)));
        SDL_RenderDrawLine(renderer,int(x)-extent,int(y)+row,int(x)+extent,int(y)+row);
    }
}

static void sfCoopCircleAlpha(SDL_Renderer *renderer,int cx,int cy,int radius,
                              Uint8 r,Uint8 g,Uint8 b,Uint8 a)
{
    if(!renderer || radius<=0 || a==0) return;
    SDL_BlendMode oldBlend;Uint8 or_,og,ob,oa;
    SDL_GetRenderDrawBlendMode(renderer,&oldBlend);
    SDL_GetRenderDrawColor(renderer,&or_,&og,&ob,&oa);
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer,r,g,b,a);
    int x=radius,y=0,err=0;
    while(x>=y) {
        SDL_RenderDrawPoint(renderer,cx+x,cy+y);SDL_RenderDrawPoint(renderer,cx+y,cy+x);
        SDL_RenderDrawPoint(renderer,cx-y,cy+x);SDL_RenderDrawPoint(renderer,cx-x,cy+y);
        SDL_RenderDrawPoint(renderer,cx-x,cy-y);SDL_RenderDrawPoint(renderer,cx-y,cy-x);
        SDL_RenderDrawPoint(renderer,cx+y,cy-x);SDL_RenderDrawPoint(renderer,cx+x,cy-y);
        if(err<=0){++y;err+=2*y+1;} if(err>0){--x;err-=2*x+1;}
    }
    SDL_SetRenderDrawBlendMode(renderer,oldBlend);
    SDL_SetRenderDrawColor(renderer,or_,og,ob,oa);
}
static std::string sfDisplayText(const std::string &input)
{
    std::string result;
    for (size_t i=0;i<input.size();++i) {
        const unsigned char c=input[i];
        if (c<128) result.push_back(c>='a'&&c<='z' ? char(c-'a'+'A') : char(c));
        else if (c==0xc3 && i+1<input.size()) {
            const unsigned char next=input[++i];
            if ((next>=0x80&&next<=0x85)||(next>=0xa0&&next<=0xa5)) result+='A';
            else if ((next>=0x88&&next<=0x8b)||(next>=0xa8&&next<=0xab)) result+='E';
            else if ((next>=0x8c&&next<=0x8f)||(next>=0xac&&next<=0xaf)) result+='I';
            else if ((next>=0x92&&next<=0x96)||(next>=0xb2&&next<=0xb6)) result+='O';
            else if ((next>=0x99&&next<=0x9c)||(next>=0xb9&&next<=0xbc)) result+='U';
            else if (next==0x87||next==0xa7) result+='C';
            else if (next==0x91||next==0xb1) result+='N';
            else result+='?';
        } else if ((c&0xc0)!=0x80) result+='?';
    }
    return result;
}
static void sfCoopText(SDL_Renderer *renderer,int x,int y,const std::string &value,int maxWidth,int preferred=3,
                        SDL_Color color={228,241,255,255})
{
    const std::string text=sfDisplayText(value);
    int outputWidth,outputHeight;SDL_GetRendererOutputSize(renderer,&outputWidth,&outputHeight);
    const int scale=std::max(1,std::min({preferred,maxWidth/std::max(1,int(text.size())*6),std::max(1,outputHeight/330)}));
    sfUiText(renderer,x,y,text.c_str(),scale,color.r,color.g,color.b,color.a);
}
static void sfCoopCentered(SDL_Renderer *renderer,int width,int y,const std::string &value,int preferred=3,
                            SDL_Color color={228,241,255,255})
{
    const std::string text=sfDisplayText(value);
    int outputWidth,outputHeight;SDL_GetRendererOutputSize(renderer,&outputWidth,&outputHeight);
    const int scale=std::max(1,std::min({preferred,int(width*.9f)/std::max(1,int(text.size())*6),std::max(1,outputHeight/130)}));
    sfUiText(renderer,(width-sfUiTextWidth(text.c_str(),scale))/2,y,text.c_str(),scale,color.r,color.g,color.b,color.a);
}
static void sfCoopButton(SDL_Renderer *renderer,SDL_Rect rect,const std::string &label,bool selected=false)
{
    sfUiPanel(renderer,rect,selected ? 16 : 7,selected ? 55 : 24,selected ? 61 : 42,70,220,205);
    const int scale=std::max(1,std::min(int(rect.h/12),int(rect.w*.9f)/std::max(1,int(label.size())*6)));
    sfUiText(renderer,rect.x+(rect.w-sfUiTextWidth(label.c_str(),scale))/2,rect.y+(rect.h-7*scale)/2,
             label.c_str(),scale,231,250,255);
}

static std::array<SDL_Vertex,81> sfBossVertices(SDL_Texture *atlas,int boss,tupl centre,float radius,float time,
                                              float hit=0,float dying=0,SDL_Color tint={255,255,255,255})
{
    std::array<SDL_Vertex,81> vertices{};
    int twidth,theight;SDL_QueryTexture(atlas,nullptr,nullptr,&twidth,&theight);
    const auto src=sfBossAtlasRect(atlas,boss);
    const int family=boss%10;
    for (int row=0;row<9;++row) for (int col=0;col<9;++col) {
        const float u=col/8.0f,v=row/8.0f,x=u*2-1,y=v*2-1;
        const float edge=std::max(std::abs(x),std::abs(y));
        const float breathing=1+.025f*std::sin(time*2+boss);
        float dx=.022f*edge*std::sin(time*3+y*4+family);
        float dy=.019f*edge*std::sin(time*2.7f+x*5+boss);
        if (family==2 || family==7) {dx+=.06f*std::max(0.0f,y)*std::sin(time*4+y*5+x*2);dy+=.045f*std::max(0.0f,y)*std::cos(time*3+x*4);}
        if (family==1 || family==8) {dy+=.06f*std::abs(x)*std::sin(time*5+y*6);dx+=.025f*y*std::sin(time*4);}
        if (family==5) dy+=.10f*x*x*std::sin(time*4+std::abs(x)*2);
        if (family==0 || family==4 || family==6 || family==9) {
            const float angle=std::atan2(y,x);
            dx+=x*.035f*edge*std::sin(time*3+angle*4);dy+=y*.035f*edge*std::sin(time*3+angle*4);
        }
        if (family==3) {dx+=x*.06f*std::sin(time*3)*(1-y*y);dy+=y*.04f*std::sin(time*3+.6f);}
        const float collapse=1-std::clamp(dying,0.0f,1.0f)*.75f;
        const float turn=.04f*std::sin(time*.8f+family);
        const float px=(x*breathing+dx)*radius*collapse,py=(y*breathing+dy)*radius*collapse;
        auto &vertex=vertices[row*9+col];
        vertex.position={centre.x+px*std::cos(turn)-py*std::sin(turn),centre.y+px*std::sin(turn)+py*std::cos(turn)};
        const Uint8 hitG=Uint8(hit>0 ? 195 : 255),hitB=Uint8(hit>0 ? 165 : 255);
        vertex.color={tint.r,Uint8(unsigned(hitG)*tint.g/255u),Uint8(unsigned(hitB)*tint.b/255u),
                      Uint8(unsigned(tint.a)*Uint8(255*(1-std::clamp(dying,0.0f,1.0f)))/255u)};
        vertex.tex_coord=sfAtlasSafeUv(src,twidth,theight,u,v);
    }
    return vertices;
}
static void sfDrawBoss(SDL_Renderer *renderer,SDL_Texture *atlas,int boss,tupl centre,float radius,float time,float hit=0,float dying=0,SDL_Color tint={255,255,255,255})
{
    if (!atlas) {sfCoopDisc(renderer,centre.x,centre.y,radius*.6f,{180,60,230,220});return;}
    const auto vertices=sfBossVertices(atlas,boss,centre,radius,time,hit,dying,tint);
    std::array<int,384> indices{};int offset=0;
    for (int y=0;y<8;++y) for (int x=0;x<8;++x) {
        const int a=y*9+x;
        for (int index : {a,a+1,a+9,a+1,a+10,a+9}) indices[offset++]=index;
    }
    SDL_SetTextureBlendMode(atlas,SDL_BLENDMODE_BLEND);
    SDL_RenderGeometry(renderer,atlas,vertices.data(),int(vertices.size()),indices.data(),int(indices.size()));
}

#include "boss_difficulty_visuals.hpp"

static Uint8 sfCampaignTintChannel(Uint8 base,std::uint8_t accent,float amount)
{
    amount=std::clamp(amount,0.0f,1.0f);
    return Uint8(std::clamp(int(base*(1-amount)+accent*amount+.5f),0,255));
}
static void sfDrawCampaignSpace(SDL_Renderer *renderer,int encounter,float time,int width,int height)
{
    encounter=std::clamp(encounter,0,199);
    auto &textures=sfCoopTextures(renderer);const auto scene=sfScenicProfileForEncounter(encounter);
    const auto accent=scene.accent;
    SDL_SetRenderDrawColor(renderer,2,5,16,255);SDL_RenderClear(renderer);
    if (textures.backgrounds) {
        auto source=sfAtlasRect(textures.backgrounds,scene.backdrop,3,2);
        const float aspect=float(width)/height;
        if (aspect<1) {
            const int wanted=int(source.h*aspect),travel=source.w-wanted;
            source.x+=int(travel*(.5f+.38f*std::sin(scene.candidate*1.71f+time*.011f)));source.w=wanted;
        } else {
            const int wanted=int(source.w/aspect),travel=source.h-wanted;
            source.y+=int(travel*(.5f+.38f*std::sin(scene.candidate*1.71f+time*.011f)));source.h=wanted;
        }
        const float tint=.10f+scene.tintStrength*.30f;
        SDL_Rect full{0,0,width,height};
        SDL_SetTextureColorMod(textures.backgrounds,sfCampaignTintChannel(155,accent.r,tint),
            sfCampaignTintChannel(165,accent.g,tint),sfCampaignTintChannel(190,accent.b,tint));
        SDL_RenderCopy(renderer,textures.backgrounds,&source,&full);
        SDL_SetTextureColorMod(textures.backgrounds,255,255,255);
    }
    if (textures.planets) {
        const auto source=sfAtlasRect(textures.planets,sfScenicPlanetAtlasIndex(scene.planetSlot),11,5);
        const float diameter=std::min(width,height)*(.34f+.012f*(scene.basePlanet%7));
        const float x=width*(.5f+.28f*std::sin(scene.candidate*1.37f+time*.024f));
        const float y=height*(scene.baseBackdrop%2 ? .77f+.045f*std::sin(time*.019f+scene.basePlanet) : .23f+.045f*std::sin(time*.019f+scene.basePlanet));
        int twidth,theight;SDL_QueryTexture(textures.planets,nullptr,nullptr,&twidth,&theight);
        std::array<SDL_Vertex,66> vertices{};std::array<int,192> indices{};
        const float uc=(source.x+source.w*.5f)/twidth,vc=(source.y+source.h*.5f)/theight;
        const float planetTint=.10f+scene.tintStrength;
        const SDL_Color planetColor{sfCampaignTintChannel(180,accent.r,planetTint),
            sfCampaignTintChannel(190,accent.g,planetTint),sfCampaignTintChannel(220,accent.b,planetTint),190};
        vertices[0]={{x,y},planetColor,{uc,vc}};
        for (int i=0;i<=64;++i) {
            const float a=i*2*float(PI)/64,rotation=time*.009f;
            vertices[i+1]={{x+std::cos(a)*diameter*.5f,y+std::sin(a)*diameter*.5f},planetColor,
                {uc+std::cos(a+rotation)*source.w*.405f/twidth,vc+std::sin(a+rotation)*source.w*.405f/theight}};
            if (i<64) {indices[i*3]=0;indices[i*3+1]=i+1;indices[i*3+2]=i+2;}
        }
        SDL_SetTextureBlendMode(textures.planets,SDL_BLENDMODE_BLEND);
        SDL_RenderGeometry(renderer,textures.planets,vertices.data(),int(vertices.size()),indices.data(),int(indices.size()));
        sfUiCircle(renderer,int(x),int(y),int(diameter*.5f),
            sfCampaignTintChannel(70,accent.r,.12f),sfCampaignTintChannel(105,accent.g,.12f),sfCampaignTintChannel(145,accent.b,.12f));
    }
    std::uint32_t seed=scene.starSeed^std::uint32_t(encounter)*2654435761u;
    for (int i=0;i<70;++i) {
        seed=seed*1664525u+1013904223u;const int x=seed%std::max(1,width);
        seed=seed*1664525u+1013904223u;const int y=seed%std::max(1,height);
        const auto alpha=Uint8(90+65*(1+std::sin(time*.7f+i)));
        SDL_SetRenderDrawColor(renderer,sfCampaignTintChannel(180,accent.r,.08f),
            sfCampaignTintChannel(215,accent.g,.08f),sfCampaignTintChannel(255,accent.b,.08f),alpha);
        SDL_RenderDrawPoint(renderer,x,y);
    }
}

static SDL_Rect sfMirrorRect180(SDL_Rect rect,int width,int height)
{
    rect.x=width-rect.x-rect.w;rect.y=height-rect.y-rect.h;return rect;
}
static SDL_Color sfHealthColor(float ratio)
{
    ratio=std::clamp(ratio,0.0f,1.0f);
    return {Uint8(235*(1-ratio)+45*ratio),Uint8(45*(1-ratio)+225*ratio),70,255};
}
static void sfCoopText180(SDL_Renderer *renderer,int width,int height,int x,int y,
                          const std::string &value,int maxWidth,int preferred,SDL_Color color)
{
    const std::string text=sfDisplayText(value);
    int outputWidth,outputHeight;SDL_GetRendererOutputSize(renderer,&outputWidth,&outputHeight);
    const int scale=std::max(1,std::min({preferred,maxWidth/std::max(1,int(text.size())*6),
                                        std::max(1,outputHeight/330)}));
    SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);
    int cursor=x;
    for(const char ch:text) {
        const uint8_t *glyph=sfUiGlyph(ch);
        for(int row=0;row<7;++row) for(int col=0;col<5;++col) if(glyph[row]&(1u<<(4-col))) {
            SDL_Rect px{cursor+col*scale,y+row*scale,scale,scale};
            px=sfMirrorRect180(px,width,height);SDL_RenderFillRect(renderer,&px);
        }
        cursor+=6*scale;
    }
}
static int sfHudBarHeight(int width) {return std::max(10,width/60);}
static int sfBossHudBarHeight(int width) {return std::max(5,width/110);}
static Uint8 sfBossHudBackgroundAlpha() {return 88;}
static Uint8 sfBossHudFillAlpha() {return 188;}
static Uint8 sfBossHudMirrorAlpha() {return 76;}
static SDL_Color sfBossEnergyColor() {return {205,175,255,255};}
static SDL_Color sfBossKineticColor() {return {255,184,66,255};}
static SDL_Rect sfPlayerPvRect(int owner,int width,int height)
{
    const int margin=std::max(8,width/30),w=int(width*.43f),h=sfHudBarHeight(width);
    SDL_Rect base{margin,int(height*.925f),w,h};
    return owner==0 ? sfMirrorRect180(base,width,height) : base;
}
static SDL_Rect sfPlayerEnergyRect(int owner,int width,int height)
{
    const int margin=std::max(8,width/30),w=int(width*.43f),h=sfHudBarHeight(width);
    SDL_Rect base{margin,int(height*.963f),w,h};
    return owner==0 ? sfMirrorRect180(base,width,height) : base;
}
static SDL_Rect sfBossHudRectAt(int index,int width,int height,bool upper=false)
{
    const int margin=std::max(8,width/30);
    const int w=std::max(48,int(width*.28f)),h=sfBossHudBarHeight(width);
    const int gap=std::max(2,h/3);
    // Canonical object lives bottom-left for the lower player. The upper-player
    // instance is its exact 180-degree geometric mirror.
    SDL_Rect base{margin,int(height*.835f)+index*(h+gap),w,h};
    return upper ? sfMirrorRect180(base,width,height) : base;
}
static SDL_Rect sfBossHudLabelRect(bool upper,int width,int height)
{
    const SDL_Rect top=sfBossHudRectAt(0,width,height,false);
    const int labelH=std::max(10,sfBossHudBarHeight(width)*2);
    SDL_Rect base{top.x,std::max(2,top.y-labelH-4),top.w,labelH};
    return upper ? sfMirrorRect180(base,width,height) : base;
}
static SDL_Rect sfBossHudMirrorBand(SDL_Rect rect,bool lower)
{
    const int bandH=std::max(1,rect.h/4);
    const int inset=rect.h>=4 ? 1 : 0;
    const int y=lower ? std::max(rect.y+rect.h/2,rect.y+rect.h-bandH-inset) : rect.y+inset;
    return {rect.x,y,rect.w,bandH};
}
static SDL_Rect sfBossLifeRect(bool upper,int width,int height)
{
    return sfBossHudRectAt(1,width,height,upper);
}
static SDL_Rect sfBossEnergyRect(bool upper,int width,int height)
{
    return sfBossHudRectAt(0,width,height,upper);
}
static SDL_Rect sfBossKineticRect(bool upper,int width,int height)
{
    return sfBossHudRectAt(2,width,height,upper);
}
static SDL_Rect sfBossHudValueRect(SDL_Rect rect,float ratio,bool upper)
{
    ratio=std::clamp(ratio,0.0f,1.0f);
    SDL_Rect value=rect;value.w=std::max(0,int(rect.w*ratio));
    if(upper) value.x=rect.x+rect.w-value.w;
    return value;
}
static void sfDrawRatioBar(SDL_Renderer *renderer,SDL_Rect rect,float ratio,SDL_Color fill,bool reverse=false)
{
    ratio=std::clamp(ratio,0.0f,1.0f);
    SDL_SetRenderDrawColor(renderer,24,28,40,235);SDL_RenderFillRect(renderer,&rect);
    SDL_Rect value=rect;value.w=std::max(0,int(rect.w*ratio));
    if(reverse) value.x=rect.x+rect.w-value.w;
    SDL_SetRenderDrawColor(renderer,fill.r,fill.g,fill.b,fill.a);SDL_RenderFillRect(renderer,&value);
}
static void sfDrawBossRatioBar(SDL_Renderer *renderer,SDL_Rect rect,float ratio,SDL_Color fill,bool upper=false)
{
    SDL_BlendMode oldBlend;SDL_GetRenderDrawBlendMode(renderer,&oldBlend);
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer,24,28,40,sfBossHudBackgroundAlpha());SDL_RenderFillRect(renderer,&rect);
    SDL_Rect value=sfBossHudValueRect(rect,ratio,upper);
    SDL_SetRenderDrawColor(renderer,fill.r,fill.g,fill.b,sfBossHudFillAlpha());SDL_RenderFillRect(renderer,&value);

    // The gloss is rotated with the HUD too: bright edge on top for the lower
    // player, bright edge on bottom for the 180-degree upper-player copy.
    if(value.w>0) {
        SDL_Rect top=sfBossHudMirrorBand(value,false);
        SDL_Rect bottom=sfBossHudMirrorBand(value,true);
        SDL_SetRenderDrawColor(renderer,255,255,255,upper ? Uint8(sfBossHudMirrorAlpha()/3) : sfBossHudMirrorAlpha());
        SDL_RenderFillRect(renderer,&top);
        SDL_SetRenderDrawColor(renderer,255,255,255,upper ? sfBossHudMirrorAlpha() : Uint8(sfBossHudMirrorAlpha()/3));
        SDL_RenderFillRect(renderer,&bottom);
    }
    SDL_SetRenderDrawBlendMode(renderer,oldBlend);
}

static void sfCoopDrawArena(SDL_Renderer *renderer,int width,int height)
{
    auto &textures=sfCoopTextures(renderer);
    sfDrawCampaignSpace(renderer,sfCoop.encounter,sfCoop.time+sfCoop.phaseTime,width,height);
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    for (const auto &beam : sfCoop.beams) {
        const bool active=beam.age>=beam.warning;
        const float dx=std::cos(beam.angle),dy=std::sin(beam.angle);
        SDL_SetRenderDrawColor(renderer,255,active ? 210 : 80,active ? 150 : 90,active ? 235 : 150);
        const int thickness=active ? std::max(3,int(width*.027f)) : 2;
        for (int i=-thickness/2;i<=thickness/2;++i)
            SDL_RenderDrawLine(renderer,int(beam.origin.x-dy*i),int(beam.origin.y+dx*i),
                               int(beam.origin.x+dx*height*2-dy*i),int(beam.origin.y+dy*height*2+dx*i));
    }
    for (const auto &wave : sfCoop.waves) {
        const int radius=int(wave.age<.7f ? sfCoopBossRadius()+wave.age*25 : wave.radius);
        for (int i=0;i<3;++i) sfUiCircle(renderer,int(wave.origin.x),int(wave.origin.y),radius+i,230,80,190);
    }
    for (const auto *rock : sa1) {
        if (rock->pv<=0) continue;
        SDL_Rect dest{int(rock->x-rock->w*.5f),int(rock->y-rock->h*.5f),int(rock->w),int(rock->h)};
        const int index=rock->name.size()==2 ? std::clamp(rock->name[1]-'1',0,3) : 0;
        if (textures.rocks[index]) SDL_RenderCopyEx(renderer,textures.rocks[index],nullptr,&dest,
            rock->rand0+sfCoop.time*rock->as*rock->asign,nullptr,rock->flip);
    }
    for (const auto *dust : particules) if (dust->pv>0) sfCoopDisc(renderer,dust->x,dust->y,2.5f,{125,235,255,220});
    for (const auto *dust : particulesr) if (dust->pv>0) sfCoopDisc(renderer,dust->x,dust->y,2.0f,{255,130,70,200});
    if(textures.impact || textures.explosion) for(const auto *effect:explos) {
        if(effect->pv<=0 || effect->w<=0 || effect->h<=0) continue;
        const bool smokeKind=effect->name=="pous" || effect->name=="tj2" || effect->name=="tj1";
        bool weakShieldNearby=false;
        for(int owner=0;owner<2;++owner) {
            const auto *ship=sfCoopShip(owner);
            if(!ship || ship->pv<=0 || sfShieldFraction(ship->nrj)>=.35f) continue;
            if(vlong(effect->x-ship->x,effect->y-ship->y)<sfCoopShipRadius()*2.2f) {
                weakShieldNearby=true;break;
            }
        }
        SDL_Texture *fx=(smokeKind && !weakShieldNearby) ? textures.impact : textures.explosion;
        if(!fx) fx=textures.explosion ? textures.explosion : textures.impact;
        if(!fx) continue;
        SDL_Rect rect{int(effect->x-effect->w*.5f),int(effect->y-effect->h*.5f),int(effect->w),int(effect->h)};
        SDL_RenderCopyEx(renderer,fx,nullptr,&rect,sfCoop.time*effect->as*effect->asign*5,nullptr,effect->flip);
    }
    for (const auto &shot : sfCoop.shots) {
        SDL_Color color=shot.owner==0 ? SDL_Color{255,175,75,255} : shot.owner==1 ? SDL_Color{100,220,255,255} : SDL_Color{255,70,140,255};
        if (shot.kind==2) color=shot.age<1.2f ? SDL_Color{255,200,75,190} : SDL_Color{255,70,50,255};
        if (shot.kind==4 && textures.missile) {
            const int w=std::max(8,int(shot.radius*1.55f)),h=std::max(18,int(shot.radius*4.8f));
            SDL_Rect rect{int(shot.position.x-w*.5f),int(shot.position.y-h*.5f),w,h};
            const float speed=std::max(1.0f,vlong(shot.velocity.vx,shot.velocity.vy));
            const float ux=shot.velocity.vx/speed,uy=shot.velocity.vy/speed;
            const double angle=std::atan2(shot.velocity.vy,shot.velocity.vx)*180.0/PI+90.0;
            // Classic missile language without spawning gameplay red dust: hot
            // exhaust beads trail the same missilebb.png body.
            for(int i=1;i<=4;++i) {
                const float trail=shot.radius*(1.5f+i*1.05f);
                const float rr=shot.radius*(.85f-.12f*i);
                sfCoopDisc(renderer,shot.position.x-ux*trail,shot.position.y-uy*trail,std::max(1.0f,rr),
                           {255,Uint8(std::max(70,205-i*30)),45,Uint8(180-i*26)});
            }
            sfCoopDisc(renderer,shot.position.x,shot.position.y,shot.radius*1.65f,{color.r,color.g,color.b,58});
            SDL_RenderCopyEx(renderer,textures.missile,nullptr,&rect,angle,nullptr,SDL_FLIP_NONE);
        } else if(shot.kind==0 && shot.owner>=0) {
            SDL_Texture *orb=shot.owner==0 ? textures.orbOrange : textures.orbBlue;
            if(orb) {
                const int side=std::max(7,int(shot.radius*2.55f));
                SDL_Rect rect{int(shot.position.x-side*.5f),int(shot.position.y-side*.5f),side,side};
                SDL_RenderCopy(renderer,orb,nullptr,&rect);
            } else {
                sfCoopDisc(renderer,shot.position.x,shot.position.y,shot.radius,color);
            }
        } else {
            sfCoopDisc(renderer,shot.position.x,shot.position.y,shot.radius*1.7f,{color.r,color.g,color.b,65});
            sfCoopDisc(renderer,shot.position.x,shot.position.y,shot.radius,color);
        }
    }
    const float death=sfCoop.phase==SfCoopPhase::Dying ? std::clamp(sfCoop.phaseTime/1.6f,0.0f,1.0f) :
        (sfCoop.phase==SfCoopPhase::Name || sfCoop.phase==SfCoopPhase::Saved ? 1.0f : 0.0f);
    if (sfCoop.warning>0 && death==0) {
        const int radius=int(sfCoopBossRadius()*(1.1f+.14f*sfCoop.warning));
        sfUiCircle(renderer,int(sfCoop.position.x),int(sfCoop.position.y),radius,255,120,90);
        sfUiCircle(renderer,int(sfCoop.position.x),int(sfCoop.position.y),radius+2,255,170,100);
    }
    if(death==0) {
        const int fieldRadius=int(sfCoopBossRadius()*SF_COOP_BOSS_KINETIC_FIELD_RADIUS_SCALE);
        const Uint8 reserveAlpha=sfCoopBossKineticVisibilityAlpha(sfCoop.bossKineticReserve);
        sfCoopCircleAlpha(renderer,int(sfCoop.position.x),int(sfCoop.position.y),fieldRadius,
                          145,95,215,Uint8(std::max(8,int(reserveAlpha*.34f))));
        // Quiet, continuous centre -> outside kinetic waves. A stressed reserve
        // is easier to read, while a healthy field remains almost transparent.
        for(int ring=0;ring<3;++ring) {
            float p=std::fmod(sfCoop.time*.31f+ring/3.0f,1.0f);
            if(p<0) p+=1.0f;
            const float eased=p*p*(3.0f-2.0f*p);
            const int radius=std::max(1,int(fieldRadius*eased));
            const Uint8 alpha=Uint8(std::clamp(float(reserveAlpha)*(1.0f-.62f*p),5.0f,76.0f));
            sfCoopCircleAlpha(renderer,int(sfCoop.position.x),int(sfCoop.position.y),radius,
                              205,160,255,alpha);
        }
        if(sfCoop.bossKineticFlash>0) {
            const float progress=1.0f-sfCoop.bossKineticFlash/SF_COOP_BOSS_KINETIC_RING_DURATION;
            const int radius=int(sfCoopBossKineticRingRadius(sfCoop.bossKineticFlash,float(fieldRadius)));
            const float impactBase=std::min(float(SF_COOP_BOSS_KINETIC_RING_ALPHA),float(reserveAlpha)+18.0f);
            const Uint8 alpha=Uint8(std::clamp(impactBase*(1.0f-progress*.62f),10.0f,76.0f));
            sfCoopCircleAlpha(renderer,int(sfCoop.position.x),int(sfCoop.position.y),radius,
                              255,150,235,alpha);
            sfCoopCircleAlpha(renderer,int(sfCoop.position.x),int(sfCoop.position.y),radius+2,
                              220,185,255,Uint8(std::max(7,int(alpha*.55f))));
        }
    }
    sfDrawEncounterBoss(renderer,textures.bosses,sfCoop.encounter,sfCoop.position,sfCoopBossRadius(),
               sfCoop.time+sfCoop.phaseTime,sfCoop.hit,death);
    if (death>0 && death<1) for (int i=0;i<40;++i) {
        const float angle=i*2.39996f,radius=sfCoopBossRadius()*(.5f+death*(1.5f+(i%4)*.4f));
        sfCoopDisc(renderer,sfCoop.position.x+std::cos(angle)*radius,sfCoop.position.y+std::sin(angle)*radius,
                   std::max(1.0f,(1-death)*7),{255,Uint8(90+i%120),100,Uint8(255*(1-death))});
    }
    for (int owner=0;owner<2;++owner) {
        const auto *ship=sfCoopShip(owner);SDL_Texture *texture=owner==0 ? textures.orange : textures.blue;
        const int size=int(std::min(sfArenaW,sfArenaH)*.15f);
        SDL_Rect rect{int(ship->x-ship->w*.5f),int(ship->y-ship->h*.5f),int(ship->w),int(ship->h)};
        if (texture) {
            SDL_SetTextureColorMod(texture,ship->pv>0 ? 255 : 95,ship->pv>0 ? 255 : 95,ship->pv>0 ? 255 : 95);
            SDL_SetTextureAlphaMod(texture,sfCoop.invulnerable[owner]>0 && int(sfCoop.time*12)%2 ? 140 : 255);
            SDL_RenderCopyEx(renderer,texture,nullptr,&rect,sfCoop.time*(owner==0 ? 17 : -17),nullptr,SDL_FLIP_NONE);
            SDL_SetTextureColorMod(texture,255,255,255);SDL_SetTextureAlphaMod(texture,255);
        }
        if (ship->pv<=0) {
            sfCoopText(renderer,int(ship->x-size*.6f),int(ship->y+size*.6f),sfCoop.revives>0 ? "SECOURIR 2 S" : "HORS COMBAT",int(size*1.4f),2,{255,150,150,255});
            sfUiCircle(renderer,int(ship->x),int(ship->y),int(sfCoopShipRadius()*1.8f),230,100,100);
        }
    }
    sfDrawTacticalEffects(renderer);
    sfDrawKineticEffects(renderer);
    if (sfCoop.bonusLife>0 && textures.bonus) {
        const int side=int(std::min(sfArenaW,sfArenaH)*.085f);
        SDL_Rect rect{int(sfCoop.bonusPosition.x-side*.5f),int(sfCoop.bonusPosition.y-side*.5f),side,side};
        SDL_RenderCopyEx(renderer,textures.bonus,nullptr,&rect,5*std::sin(sfCoop.time*2),nullptr,SDL_FLIP_NONE);
    }
    if (sfCoop.turretTime>0) sfCoopText(renderer,width/4,int(height*.875f),
        "TOURELLES : "+std::to_string(int(std::ceil(sfCoop.turretTime)))+" S",width/2,2,{120,255,150,255});
    const int margin=std::max(8,width/30);
    sfCoopText(renderer,margin,int(height*.025f),"COMBAT "+std::to_string(sfCoop.encounter+1)+" / 200 - "+sfDifficultyNames[sfDifficultyIndex(sfCoop.encounter)],int(width*.45f),std::max(2,width/220),{255,190,120,255});
    sfCoopText(renderer,margin,int(height*.05f),sfCoopProfile().name,int(width*.45f),std::max(2,width/260));
    sfCoopButton(renderer,{int(width*.81f),int(height*.47f),int(width*.16f),int(height*.062f)},"PAUSE");

    const float bossRatio=std::clamp(sfCoop.health/sfCoopProfile().health,0.0f,1.0f);
    const SDL_Color bossColor=sfHealthColor(bossRatio);
    const SDL_Color bossEnergyColor=sfBossEnergyColor(),bossKineticColor=sfBossKineticColor();

    // Lower player: canonical compact object bottom-left.
    const SDL_Rect bossLabelLower=sfBossHudLabelRect(false,width,height);
    sfCoopText(renderer,bossLabelLower.x,bossLabelLower.y,"BOSS",bossLabelLower.w,2,{232,220,255,220});
    sfDrawBossRatioBar(renderer,sfBossEnergyRect(false,width,height),sfCoop.bossEnergyReserve,bossEnergyColor,false);
    sfDrawBossRatioBar(renderer,sfBossLifeRect(false,width,height),bossRatio,bossColor,false);
    sfDrawBossRatioBar(renderer,sfBossKineticRect(false,width,height),sfCoop.bossKineticReserve,bossKineticColor,false);

    // Upper player: same object transformed by 180 degrees. sfCoopText180
    // receives the lower logical coordinates and performs the glyph rotation.
    sfCoopText180(renderer,width,height,bossLabelLower.x,bossLabelLower.y,"BOSS",bossLabelLower.w,2,{232,220,255,220});
    sfDrawBossRatioBar(renderer,sfBossEnergyRect(true,width,height),sfCoop.bossEnergyReserve,bossEnergyColor,true);
    sfDrawBossRatioBar(renderer,sfBossLifeRect(true,width,height),bossRatio,bossColor,true);
    sfDrawBossRatioBar(renderer,sfBossKineticRect(true,width,height),sfCoop.bossKineticReserve,bossKineticColor,true);

    for (int owner=0;owner<2;++owner) {
        const auto *ship=sfCoopShip(owner);const bool upper=owner==0;
        const SDL_Color team=upper ? SDL_Color{255,180,95,255} : SDL_Color{100,210,255,255};
        const float pvRatio=std::clamp(ship->pv/1000.0f,0.0f,1.0f);
        const float energyRatio=sfKineticEnergyFraction(ship->nrj);
        sfDrawRatioBar(renderer,sfPlayerPvRect(owner,width,height),pvRatio,sfHealthColor(pvRatio),upper);
        sfDrawRatioBar(renderer,sfPlayerEnergyRect(owner,width,height),energyRatio,team,upper);
        const int logicalX=margin,logicalW=int(width*.43f);
        const int nameY=int(height*.895f),pvY=int(height*.908f),energyY=int(height*.946f);
        const std::string name=upper ? (sfActiveMode==SF_COOP_AI ? "ORION IA" : "PILOTE ORANGE") : "PILOTE BLEU";
        if(upper) {
            sfCoopText180(renderer,width,height,logicalX,nameY,name,logicalW,std::max(2,width/280),team);
            sfCoopText180(renderer,width,height,logicalX,pvY,"PV",logicalW,std::max(2,width/300),sfHealthColor(pvRatio));
            sfCoopText180(renderer,width,height,logicalX,energyY,"ENERGIE",logicalW,std::max(2,width/300),team);
        } else {
            sfCoopText(renderer,logicalX,nameY,name,logicalW,std::max(2,width/280),team);
            sfCoopText(renderer,logicalX,pvY,"PV",logicalW,std::max(2,width/300),sfHealthColor(pvRatio));
            sfCoopText(renderer,logicalX,energyY,"ENERGIE",logicalW,std::max(2,width/300),team);
        }
    }
}

static SDL_Rect sfCampaignCard(int index,int width,int height)
{
    const int columns=height>=width ? 2 : 5,rows=10/columns;
    const int cellWidth=int(width*.9f)/columns,cellHeight=int(height*.58f)/rows;
    return {int(width*.05f)+(index%columns)*cellWidth,int(height*.205f)+(index/columns)*cellHeight,cellWidth-6,cellHeight-5};
}
static void sfCampaignDrawSelect(SDL_Renderer *renderer)
{
    sfLoadCampaign();int width,height;SDL_GetRendererOutputSize(renderer,&width,&height);
    if (width<=0 || height<=0) return;
    sfCampaignPage=std::clamp(sfCampaignPage,0,19);
    const int first=sfCampaignPage*10,difficulty=first/50;
    const float time=SDL_GetTicks64()*.001f;
    sfDrawCampaignSpace(renderer,sfCampaignSave.selected,time,width,height);
    sfCoopCentered(renderer,width,int(height*.025f),"CAMPAGNE COOPERATIVE",std::max(2,std::min(width/150,height/170)));
    sfCoopCentered(renderer,width,int(height*.078f),std::to_string(sfCampaignSave.cleared)+" VICTOIRES SUR 200",std::max(2,std::min(width/240,height/250)));
    for(int d=0;d<4;++d) sfCoopButton(renderer,{int(width*(.04f+d*.235f)),int(height*.127f),int(width*.215f),int(height*.055f)},
        std::to_string(d+1)+" "+sfDifficultyNames[d],d==difficulty);
    auto &textures=sfCoopTextures(renderer);
    for (int i=0;i<10;++i) {
        const int encounter=first+i;
        const auto rect=sfCampaignCard(i,width,height);const bool unlocked=encounter<=sfCampaignSave.cleared;
        sfUiPanel(renderer,rect,5,14,28,unlocked ? 80 : 35,unlocked ? 175 : 55,unlocked ? 185 : 70);
        const float radius=std::max(1.0f,std::min(float(rect.w),float(rect.h-36))*.27f);
        sfDrawEncounterBoss(renderer,textures.bosses,encounter,tupl(rect.x+rect.w*.5f,rect.y+(rect.h-30)*.5f),radius,time);
        sfCoopText(renderer,rect.x+6,rect.y+rect.h-29,"BOSS "+std::to_string(sfBossIndex(encounter)+1)+" - N"+std::to_string(difficulty+1),rect.w-12,2);
        sfCoopText(renderer,rect.x+6,rect.y+rect.h-10,encounter<sfCampaignSave.cleared ? "VICTOIRE" : unlocked ? "JOUER" : "VERROUILLE",rect.w-12,1,
                   unlocked ? SDL_Color{140,240,180,255} : SDL_Color{190,190,200,255});
    }
    sfCoopButton(renderer,{int(width*.05f),int(height*.80f),int(width*.25f),int(height*.045f)},"PRECEDENT");
    sfCoopCentered(renderer,width,int(height*.81f),"BOSS "+std::to_string(first%50+1)+" A "+std::to_string(first%50+10),2);
    sfCoopButton(renderer,{int(width*.70f),int(height*.80f),int(width*.25f),int(height*.045f)},"SUIVANT");
    sfCoopButton(renderer,{int(width*.08f),int(height*.86f),int(width*.84f),int(height*.065f)},sfCampaignSave.pending ? "INSCRIRE LA VICTOIRE EN ATTENTE" : "REPRENDRE LA CAMPAGNE");
    sfCoopButton(renderer,{int(width*.3f),int(height*.94f),int(width*.4f),int(height*.045f)},"ACCUEIL");
}

static void sfCampaignDrawHall(SDL_Renderer *renderer)
{
    sfLoadCampaign();int width,height;SDL_GetRendererOutputSize(renderer,&width,&height);
    if (width<=0 || height<=0) return;
    sfDrawCampaignSpace(renderer,199,SDL_GetTicks64()*.001f,width,height);
    sfCoopCentered(renderer,width,int(height*.035f),"HALL OF FAME",std::max(3,width/130),{255,220,125,255});
    sfCoopCentered(renderer,width,int(height*.105f),"LES EQUIPES IMMORTALISEES",std::max(2,width/250));
    std::vector<SfFameEntry> entries=sfCampaignSave.fame;
    std::stable_sort(entries.begin(),entries.end(),[](const auto &a,const auto &b){
        return a.boss!=b.boss ? a.boss>b.boss : a.score!=b.score ? a.score>b.score : a.seconds<b.seconds;
    });
    const int perPage=height>=width ? 5 : 2,pages=std::max(1,int(entries.size()+perPage-1)/perPage);
    sfFamePage=std::clamp(sfFamePage,0,pages-1);
    if (entries.empty()) {
        sfCoopCentered(renderer,width,int(height*.38f),"VOTRE LEGENDE COMMENCE ICI",std::max(2,width/230));
        sfCoopCentered(renderer,width,int(height*.46f),"BATTEZ UN BOSS EN COOP",std::max(2,width/260));
    }
    for (int row=0;row<perPage;++row) {
        const int index=sfFamePage*perPage+row;if (index>=int(entries.size())) break;
        const auto &entry=entries[index];const int y=int(height*(.19f+row*(.57f/perPage)));
        SDL_Rect panel{int(width*.07f),y,int(width*.86f),int(height*.52f/perPage)};
        sfUiPanel(renderer,panel,7,16,34,90,125,150);
        const int left=panel.x+12,w=panel.w-24,scale=std::max(2,std::min(width/240,panel.h/30));
        sfCoopText(renderer,left,y+8,std::to_string(index+1)+". "+entry.names[2],w,scale+1,{255,215,125,255});
        sfCoopText(renderer,left,y+10+9*(scale+1),entry.names[0]+" + "+entry.names[1],w,scale);
        sfCoopText(renderer,left,y+14+18*(scale+1),"COMBAT "+std::to_string(entry.boss)+" / 200  SCORE "+std::to_string(entry.score)+"  "+std::to_string(entry.seconds)+" S",w,scale,{120,220,220,255});
    }
    if (!sfCampaignStorageError.empty()) sfCoopCentered(renderer,width,int(height*.79f),sfCampaignStorageError,2,{255,130,110,255});
    else sfCoopCentered(renderer,width,int(height*.79f),"MEMOIRE DE CE TELEPHONE - "+std::to_string(sfCampaignSave.fame.size())+" VICTOIRES",2);
    sfCoopButton(renderer,{int(width*.07f),int(height*.84f),int(width*.22f),int(height*.055f)},"PRECEDENT");
    sfCoopCentered(renderer,width,int(height*.855f),std::to_string(sfFamePage+1)+" / "+std::to_string(pages),2);
    sfCoopButton(renderer,{int(width*.71f),int(height*.84f),int(width*.22f),int(height*.055f)},"SUIVANT");
    sfCoopButton(renderer,{int(width*.25f),int(height*.925f),int(width*.5f),int(height*.06f)},"ACCUEIL");
}

static void sfCoopDrawOverlay(SDL_Renderer *renderer,int width,int height)
{
    if (sfCoop.phase==SfCoopPhase::Combat || sfCoop.phase==SfCoopPhase::Dying) return;
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);SDL_SetRenderDrawColor(renderer,2,6,18,205);
    SDL_Rect dim{0,0,width,height};SDL_RenderFillRect(renderer,&dim);
    if (sfCoop.phase==SfCoopPhase::Name) {
        sfCoopCentered(renderer,width,int(height*.04f),sfCoop.encounter==199 ? "LES 200 AFFRONTEMENTS SONT VAINCUS" : "VICTOIRE COOPERATIVE",std::max(3,width/190),{255,215,125,255});
        sfCoopCentered(renderer,width,int(height*.09f),"GRAVEZ VOTRE EQUIPE DANS L HISTOIRE",std::max(2,width/300));
        const std::array<const char*,3> labels{{"PILOTE ORANGE","PILOTE BLEU","NOM DE L EQUIPE"}};
        for (int i=0;i<3;++i) {
            const int y=int(height*(.15f+i*.115f));
            sfCoopText(renderer,int(width*.09f),y,labels[i],int(width*.8f),std::max(2,width/300));
            SDL_Rect field{int(width*.08f),y+int(height*.027f),int(width*.84f),int(height*.064f)};
            sfUiPanel(renderer,field,8,25,45,i==sfCoop.nameField ? 100 : 45,i==sfCoop.nameField ? 240 : 100,180);
            std::string value=sfCoop.names[i];
            if (i==sfCoop.nameField && !sfCoop.composition.empty()) value+=" "+sfCoop.composition;
            sfCoopText(renderer,field.x+10,field.y+(field.h-21)/2,value.empty() ? "TOUCHER POUR SAISIR" : value,field.w-20,std::max(2,width/230));
        }
        if (!sfCoop.error.empty()) sfCoopCentered(renderer,width,int(height*.505f),sfCoop.error,2,{255,140,120,255});
        else sfCoopCentered(renderer,width,int(height*.505f),"24 CARACTERES PAR NOM",2);
        sfCoopButton(renderer,{int(width*.08f),int(height*.54f),int(width*.84f),int(height*.068f)},"IMMORTALISER LA VICTOIRE");
    } else if (sfCoop.phase==SfCoopPhase::Intro) {
        sfCoopCentered(renderer,width,int(height*.32f),"COMBAT "+std::to_string(sfCoop.encounter+1)+" / 200 - "+sfDifficultyNames[sfDifficultyIndex(sfCoop.encounter)],std::max(3,width/150),{255,210,125,255});
        sfCoopCentered(renderer,width,int(height*.40f),sfCoopProfile().name,std::max(3,width/190));
        sfCoopCentered(renderer,width,int(height*.51f),sfBossHints[sfCoopProfile().family],std::max(2,width/320));
        sfCoopCentered(renderer,width,int(height*.60f),"1 DOIGT : PILOTER - 2E DOIGT : TAP POUR TIRER",std::max(2,width/340));
        sfCoopCentered(renderer,width,int(height*.67f),"MINERAIS = ENERGIE - PROXIMITE = SECOURS",std::max(2,width/350));
    } else {
        const bool saved=sfCoop.phase==SfCoopPhase::Saved,paused=sfCoop.phase==SfCoopPhase::Paused;
        sfCoopCentered(renderer,width,int(height*.31f),saved ? "VICTOIRE IMMORTALISEE" : paused ? "PAUSE COOPERATIVE" : "EQUIPE HORS COMBAT",std::max(3,width/190),{255,220,130,255});
        if (saved) sfCoopCentered(renderer,width,int(height*.42f),sfCampaignSave.names[2],std::max(3,width/200));
        sfCoopButton(renderer,{int(width*.13f),int(height*.53f),int(width*.74f),int(height*.09f)},saved ? (sfCoop.encounter==199 ? "VOIR LE HALL OF FAME" : "BOSS SUIVANT") : paused ? "REPRENDRE" : "REESSAYER CE BOSS");
        sfCoopButton(renderer,{int(width*.23f),int(height*.68f),int(width*.54f),int(height*.08f)},"CHOISIR UNE MISSION");
        sfCoopButton(renderer,{int(width*.3f),int(height*.81f),int(width*.4f),int(height*.07f)},"ACCUEIL");
    }
}

static bool sfCampaignFrame(SDL_Renderer *renderer)
{
    if (!sfIsCoop() || sfUiScreen!=SF_UI_GAME) return false;
    const float dt=sfFrameDt;
    if (sfCoop.phase==SfCoopPhase::Combat) {
        const int steps=std::max(1,int(std::ceil(dt*90)));
        for (int i=0;i<steps;++i) sfCoopTick(dt/steps);
    } else if (sfCoop.phase==SfCoopPhase::Intro || sfCoop.phase==SfCoopPhase::Dying) {
        sfCoop.phaseTime+=dt;
        if (sfCoop.phase==SfCoopPhase::Intro && sfCoop.phaseTime>=3) {sfCoop.phase=SfCoopPhase::Combat;sfCoop.phaseTime=0;}
        else if (sfCoop.phase==SfCoopPhase::Dying && sfCoop.phaseTime>=1.6f) {sfCoop.phase=SfCoopPhase::Name;sfCoop.phaseTime=0;}
    }
    int width,height;SDL_GetRendererOutputSize(renderer,&width,&height);
    if (width>0 && height>0) {sfCoopDrawArena(renderer,width,height);sfCoopDrawOverlay(renderer,width,height);}
    return true;
}

static void sfCoopFocusName(int field)
{
    sfCoop.nameField=std::clamp(field,0,2);sfCoop.keyboard=true;sfCoop.composition.clear();
    SDL_Rect area{int(sfArenaW*.08f),int(sfArenaH*(.177f+field*.115f)),int(sfArenaW*.84f),int(sfArenaH*.064f)};
    SDL_SetTextInputRect(&area);SDL_StartTextInput();
}
static bool sfCoopSaveNames()
{
    for (int i=0;i<3;++i) if (sfCleanName(sfCoop.names[i]).empty()) {
        sfCoop.error="RENSEIGNEZ LES DEUX NOMS ET L EQUIPE";sfCoopFocusName(i);return false;
    }
    if (!sfRecordCampaignVictory(sfCoop.names)) {
        sfCoop.error=sfCampaignStorageError.empty() ? "ECHEC SAUVEGARDE - REESSAYER" : sfCampaignStorageError;return false;
    }
    SDL_StopTextInput();sfCoop.keyboard=false;sfCoop.phase=SfCoopPhase::Saved;sfCoop.error.clear();return true;
}
static void sfCoopRequestHome(int screen=SF_UI_HOME)
{
    sfCampaignSuspend();sfFixLaunchPending.store(false);sfFixRequestedScreen.store(screen);
}
static bool sfCampaignHandleEvent(SDL_Event *event)
{
    const int screen=sfFixRequestedScreen.load();
    const bool game=sfIsCoop() && screen==SF_UI_GAME;
    if (event->type==SDL_APP_DIDENTERBACKGROUND && game) {sfCampaignSuspend();return false;}
    if (event->type==SDL_KEYDOWN && (event->key.keysym.sym==SDLK_ESCAPE || event->key.keysym.sym==SDLK_AC_BACK)) {
        if (game) {
            if (sfCoop.phase==SfCoopPhase::Name && sfCoop.keyboard) {SDL_StopTextInput();sfCoop.keyboard=false;}
            else if (sfCoop.phase==SfCoopPhase::Combat) sfCampaignSuspend();
            else sfCoopRequestHome();
            event->type=SDL_USEREVENT;return true;
        }
        if (screen==SF_UI_HALL || screen==SF_UI_CAMPAIGN) {sfCoopRequestHome();event->type=SDL_USEREVENT;return true;}
    }
    if (game && sfCoop.phase==SfCoopPhase::Name) {
        if (event->type==SDL_TEXTINPUT) {
            auto &name=sfCoop.names[sfCoop.nameField];
            std::string combined=name+event->text.text;
            int characters=0;size_t end=0;
            for (;end<combined.size();++end) if ((static_cast<unsigned char>(combined[end])&0xc0)!=0x80 && ++characters>24) break;
            name=combined.substr(0,std::min(end,size_t(96)));sfCoop.error.clear();sfCoop.composition.clear();
        } else if (event->type==SDL_TEXTEDITING) sfCoop.composition=event->edit.text;
        else if (event->type==SDL_KEYDOWN) {
            auto &name=sfCoop.names[sfCoop.nameField];
            if (event->key.keysym.sym==SDLK_BACKSPACE && !name.empty()) {
                size_t index=name.size()-1;while (index>0 && (static_cast<unsigned char>(name[index])&0xc0)==0x80) --index;name.erase(index);
            } else if (event->key.keysym.sym==SDLK_TAB) sfCoopFocusName((sfCoop.nameField+1)%3);
            else if (event->key.keysym.sym==SDLK_RETURN) {if (sfCoop.nameField<2) sfCoopFocusName(sfCoop.nameField+1);else sfCoopSaveNames();}
        }
        if (event->type==SDL_TEXTINPUT || event->type==SDL_TEXTEDITING || event->type==SDL_KEYDOWN) {event->type=SDL_USEREVENT;return true;}
    }
    const bool touch=event->type==SDL_FINGERDOWN || event->type==SDL_FINGERMOTION || event->type==SDL_FINGERUP;
    if (!touch) return false;
    const float x=event->tfinger.x,y=event->tfinger.y;const auto finger=event->tfinger.fingerId;
    if (screen==SF_UI_HOME && event->type==SDL_FINGERDOWN) {
        if (y>=.73f && y<=.85f && x>=.5f) {sfLoadCampaign();sfFixRequestedScreen.store(SF_UI_HALL);}
        else if (y>=.61f && y<=.715f && (sfSelectedMode==SF_COOP_LOCAL || sfSelectedMode==SF_COOP_AI)) {
            sfLoadCampaign();sfCampaignPage=sfCampaignSave.selected/10;sfFixRequestedScreen.store(SF_UI_CAMPAIGN);
        } else return false;
        sfFixConsumedFingers.insert(finger);event->type=SDL_USEREVENT;return true;
    }
    if (screen==SF_UI_HALL || screen==SF_UI_CAMPAIGN) {
        if (event->type==SDL_FINGERDOWN) {
            sfFixConsumedFingers.insert(finger);
            if (screen==SF_UI_HALL) {
                if (y>.915f) sfCoopRequestHome();
                else if (y>=.825f && y<=.91f) {if (x<.35f) sfFamePage=std::max(0,sfFamePage-1);else if (x>.65f) ++sfFamePage;}
            } else if (y>.935f) sfCoopRequestHome();
            else if (y>=.127f && y<=.182f && x>=.04f && x<.96f) {
                const int d=std::clamp(int((x-.04f)/.235f),0,3);sfCampaignPage=d*5+sfCampaignPage%5;
            } else if (y>=.80f && y<=.85f) {
                const int d=sfCampaignPage/5,p=sfCampaignPage%5;
                if(x<.31f) sfCampaignPage=d*5+std::max(0,p-1);
                else if(x>.69f) sfCampaignPage=d*5+std::min(4,p+1);
            } else {
                int selected=-1;
                if (y>=.85f && y<=.93f) selected=sfCampaignSave.selected;
                else for (int i=0;i<10;++i) {
                    const auto rect=sfCampaignCard(i,int(sfArenaW),int(sfArenaH));
                    if (x*sfArenaW>=rect.x && x*sfArenaW<rect.x+rect.w && y*sfArenaH>=rect.y && y*sfArenaH<rect.y+rect.h) {selected=sfCampaignPage*10+i;break;}
                }
                if (selected>=0 && (selected<=sfCampaignSave.cleared || sfCampaignSave.pending)) {
                    sfCampaignSave.selected=std::clamp(selected,0,199);
                    sfFixLaunchPending.store(true);
                }
            }
        }
        event->type=SDL_USEREVENT;return true;
    }
    if (!game) return false;
    if (sfCoop.phase!=SfCoopPhase::Combat) {
        if (event->type==SDL_FINGERDOWN) {
            sfFixConsumedFingers.insert(finger);
            if (sfCoop.phase==SfCoopPhase::Name) {
                if (y>=.54f && y<=.63f) sfCoopSaveNames();
                else for (int field=0;field<3;++field) if (y>=.15f+field*.115f && y<.26f+field*.115f) sfCoopFocusName(field);
            } else if (sfCoop.phase==SfCoopPhase::Saved || sfCoop.phase==SfCoopPhase::Defeat || sfCoop.phase==SfCoopPhase::Paused) {
                if (y>=.53f && y<=.64f) {
                    if (sfCoop.phase==SfCoopPhase::Paused) sfCoop.phase=SfCoopPhase::Combat;
                    else if (sfCoop.phase==SfCoopPhase::Saved && sfCoop.encounter==199) sfCoopRequestHome(SF_UI_HALL);
                    else {if (sfCoop.phase==SfCoopPhase::Defeat) sfCampaignSave.selected=sfCoop.encounter;sfFixLaunchPending.store(true);}
                } else if (y>=.68f && y<=.78f) sfCoopRequestHome(SF_UI_CAMPAIGN);
                else if (y>=.80f) sfCoopRequestHome();
            }
        }
        event->type=SDL_USEREVENT;return true;
    }
    if (event->type==SDL_FINGERDOWN && x>.79f && y<.10f) {
        sfCampaignSuspend();sfFixConsumedFingers.insert(finger);event->type=SDL_USEREVENT;return true;
    }
    if (event->type==SDL_FINGERUP) {
        auto binding=sfCoop.fireFingers.find(finger);
        if(binding!=sfCoop.fireFingers.end()) {
            const int owner=binding->second;
            const bool purge=sfKineticSurgeRelease(owner);
            if(purge) {sfKineticAudioRelease(owner);sfKineticPurgeAsteroids(owner);}
            else {sfKineticAudioCancel(owner);sfCoopFire(owner);}
            sfCoop.fireFingers.erase(binding);
        }
    }
    if (event->type==SDL_FINGERDOWN) {
        bool assigned=sfCoop.fireFingers.count(finger)>0;
        for (const auto &control : sfCoop.controls) assigned|=control.down && control.finger==finger;
        const int owner=y<.5f ? 0 : 1;
        auto &control=sfCoop.controls[owner];
        if (!assigned && !(owner==0 && sfActiveMode==SF_COOP_AI) && sfCoopShip(owner)->pv>0) {
            if (!control.down) {control.down=true;control.finger=finger;}
            else if (std::none_of(sfCoop.fireFingers.begin(),sfCoop.fireFingers.end(),
                       [owner](const auto &binding){return binding.second==owner;})) {
                sfCoop.fireFingers[finger]=owner;sfKineticSurgePress(owner);sfKineticAudioStartCharge(owner);
            }
        }
    }
    for (auto &control : sfCoop.controls) if (control.finger==finger) {
        control.target=tupl(std::clamp(x,0.0f,1.0f)*sfArenaW,std::clamp(y,0.0f,1.0f)*sfArenaH);
        if (event->type==SDL_FINGERUP) {control.down=false;control.finger=-1;}
    }
    event->type=SDL_USEREVENT;return true;
}
