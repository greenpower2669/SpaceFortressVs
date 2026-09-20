#pragma once
#include "boss_catalog.hpp"
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
struct SfCoopRock { tupl position; tuplv velocity; float radius=20,health=25; };
struct SfCoopDust { tupl position; tuplv velocity; float life=10; };
struct SfCoopControl { SDL_FingerID finger=-1; tupl target; bool down=false; tuplv velocity; };
struct SfCoopState {
    SfCoopPhase phase=SfCoopPhase::Intro;
    int boss=0,volley=0,revives=3,phaseNumber=0;
    float time=0,phaseTime=0,health=900,attack=1,warning=0,hit=0,rockTimer=3;
    tupl position;
    SfMotionSample motion;
    std::array<SfCoopControl,2> controls{};
    std::array<float,2> cooldown{},invulnerable{},reviveProgress{};
    std::vector<SfCoopShot> shots;
    std::vector<SfCoopBeam> beams;
    std::vector<SfCoopWave> waves;
    std::vector<SfCoopRock> rocks;
    std::vector<SfCoopDust> dust;
    std::array<std::string,3> names{};
    int nameField=0;
    bool keyboard=false,pendingSaved=false;
    std::string composition,error;
    std::array<bool,2> soundShot{};
    bool soundBoss=false,soundHit=false;
};
inline SfCoopState sfCoop;
inline int sfFamePage=0;
struct SfCampaignTextures {
    SDL_Renderer *renderer=nullptr;
    SDL_Texture *bosses=nullptr,*planets=nullptr,*backgrounds=nullptr;
    SDL_Texture *orange=nullptr,*blue=nullptr,*rock=nullptr;
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
static const SfBossProfile &sfCoopProfile() { return sfBossCatalog()[sfCoop.boss]; }
static float sfCoopBossRadius() { return std::min(sfArenaW,sfArenaH)*(.11f+.00065f*sfCoop.boss); }
static float sfCoopShipRadius() { return std::min(sfArenaW,sfArenaH)*.053f; }
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
static void sfCoopHurt(int owner,float damage)
{
    auto *ship=sfCoopShip(owner);
    if (ship->pv<=0 || sfCoop.invulnerable[owner]>0 || sfCoop.phase!=SfCoopPhase::Combat) return;
    ship->pv=std::max(0.0f,ship->pv-damage);
    sfAddShipHeat(ship,2); sfCoop.invulnerable[owner]=.38f;
    sfCoop.soundHit=true;
    if (ship->pv<=0) { sfCoop.controls[owner].down=false; sfCoop.controls[owner].velocity.set(0,0); }
}

static void sfCoopPattern(int pattern)
{
    sfCoop.soundBoss=true;
    const auto &boss=sfCoopProfile();
    const float speed=sfArenaW*boss.shotSpeed;
    const int tier=boss.tier,count=3+tier;
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
            const tupl aim=sfShipGhost(owner).intercept(origin,speed,.7f);
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
    for (const auto &rock : sfCoop.rocks) {
        if (vlong(position.x+velocity.vx*.4f-rock.position.x-rock.velocity.vx*.4f,
                  position.y+velocity.vy*.4f-rock.position.y-rock.velocity.vy*.4f)<radius+rock.radius+sfArenaW*.02f) risk+=3;
    }
    return risk;
}

static tuplv sfCoopAiVelocity(float dt)
{
    auto *ship=Spritej1;
    const tupl position(ship->x,ship->y);
    const auto aim=sfBossGhost().intercept(position,sfArenaW*1.4f,.9f);
    tupl goal(aim.x+sfArenaW*.16f*std::sin(sfCoop.time*.38f),aim.y-sfArenaH*.26f);
    if (Spritej2->pv<=0 && sfCoop.revives>0) goal=tupl(Spritej2->x,Spritej2->y);
    else if (ship->nrj>22 && !sfCoop.dust.empty()) {
        float best=std::numeric_limits<float>::max();
        for (const auto &dust : sfCoop.dust) {
            const float distance=vlong(dust.position.x-position.x,dust.position.y-position.y);
            if (distance<best && sfCoopRisk(dust.position,tuplv(0,0))<.1f) {best=distance;goal=dust.position;}
        }
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
        const float score=sfCoopRisk(position,candidate)*10+(edge ? 12 : 0)+nearBoss+
                          vlong(candidate.vx-wanted.vx,candidate.vy-wanted.vy)/maxSpeed;
        if (score<best) {best=score;chosen=candidate;}
    }
    const auto old=sfCoop.controls[0].velocity;
    const float blend=1-std::exp(-dt*9);
    return tuplv(old.vx+(chosen.vx-old.vx)*blend,old.vy+(chosen.vy-old.vy)*blend);
}

static void sfCoopMovePlayers(float dt)
{
    const float radius=sfCoopShipRadius();
    for (int owner=0;owner<2;++owner) {
        auto *ship=sfCoopShip(owner); auto &control=sfCoop.controls[owner];
        ship->vib(sfCoop.time);
        sfCoop.invulnerable[owner]=std::max(0.0f,sfCoop.invulnerable[owner]-dt);
        if (ship->pv<=0) continue;
        const tupl previous(ship->x,ship->y);
        if (owner==0 && sfActiveMode==SF_COOP_AI) control.velocity=sfCoopAiVelocity(dt);
        else if (control.down) {
            const float distance=vlong(control.target.x-ship->x,control.target.y-ship->y);
            control.velocity=sfUnitVelocity(previous,control.target,std::min(sfArenaW*1.2f,distance*11));
        } else { const float decay=std::exp(-dt*8);control.velocity.vx*=decay;control.velocity.vy*=decay; }
        ship->x=std::clamp(ship->x+control.velocity.vx*dt,radius,sfArenaW-radius);
        ship->y=std::clamp(ship->y+control.velocity.vy*dt,sfArenaH*.105f,sfArenaH*.895f);
        ship->vx=ship->vy=0; ship->startup();
        sfObserved[owner].observe(tupl(ship->x,ship->y),dt);
        sfAddShipHeat(ship,-4.5f*dt);
        const float collision=sfCoopBossRadius()*.72f+radius;
        const float distance=vlong(ship->x-sfCoop.position.x,ship->y-sfCoop.position.y);
        if (distance<collision) {
            const tuplv push=sfUnitVelocity(sfCoop.position,tupl(ship->x,ship->y),collision);
            ship->x=sfCoop.position.x+push.vx; ship->y=sfCoop.position.y+push.vy;
            sfCoopHurt(owner,30+sfCoop.boss); ship->startup();
        }
        sfCoop.cooldown[owner]-=dt;
        if (sfCoop.cooldown[owner]<=0) {
            const float speed=sfArenaW*(1.5f-ship->nrj*.014f);
            // Humans keep the historical camp axis. Only Orion aims at the
            // velocity ghost; kind 0 still flies straight after emission.
            float angle=(owner==0 ? 1 : -1)*float(PI)*.5f;
            if (owner==0 && sfActiveMode==SF_COOP_AI) {
                const tupl aim=sfBossGhost().intercept(tupl(ship->x,ship->y),speed,.9f);
                angle=std::atan2(aim.y-ship->y,aim.x-ship->x);
            }
            const tupl muzzle(ship->x+std::cos(angle)*radius,ship->y+std::sin(angle)*radius);
            sfCoopEmit(muzzle,angle,speed,owner,12);
            sfAddShipHeat(ship,1.8f);
            sfCoop.cooldown[owner]=.19f+ship->nrj*.004f;
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
    for (int i=0;i<SF_TURRET_COUNT;++i) {
        auto &turret=sfTurrets[i];
        turret.deploy=std::min(1.0f,turret.deploy+dt*1.8f);
        turret.cooldown-=dt; turret.flash=std::max(0.0f,turret.flash-dt);
        const tupl base=sfTurretBase(i);
        turret.virtualTarget=sfBossGhost().intercept(sfTurretMuzzle(i),sfArenaW*1.25f,.9f);
        const float desired=std::atan2(turret.virtualTarget.y-base.y,turret.virtualTarget.x-base.x);
        const float delta=std::remainder(desired-turret.angle,2*float(PI));
        turret.angle+=std::clamp(delta,-dt*5,dt*5);
        if (turret.deploy>=.99f && turret.cooldown<=0 && std::abs(delta)<.1f) {
            turret.flash=.16f;
            sfCoopEmit(sfTurretMuzzle(i),turret.angle,sfArenaW*1.25f,i/SF_TURRETS_PER_TEAM,2.5f);
            turret.cooldown=1.6f+(i%SF_TURRETS_PER_TEAM)*.12f;
        }
    }
}

static void sfCoopProjectiles(float dt)
{
    std::vector<SfCoopShot> bursts;
    for (auto &shot : sfCoop.shots) {
        shot.previous=shot.position; shot.age+=dt; shot.life-=dt;
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
            for (auto &rock : sfCoop.rocks) if (rock.health>0 &&
                sfSegmentDistance(shot.previous,shot.position,rock.position)<rock.radius+shot.radius) {
                rock.health-=shot.damage;shot.life=0;break;
            }
            if (shot.life>0 && sfSegmentDistance(shot.previous,shot.position,sfCoop.position)<sfCoopBossRadius()*.75f+shot.radius) {
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
}

static void sfCoopResources(float dt)
{
    sfCoop.rockTimer-=dt;
    if (sfCoop.rockTimer<=0 && sfCoop.rocks.size()<10) {
        sfCoop.rockTimer=3.3f;
        const int index=int(sfCoop.time/3.3f);
        SfCoopRock rock;
        rock.position=tupl(index%2 ? sfArenaW+35 : -35,sfArenaH*(.24f+.13f*(index%5)));
        rock.velocity=tuplv((index%2 ? -1 : 1)*sfArenaW*.07f,std::sin(float(index))*sfArenaW*.025f);
        rock.radius=sfArenaW*(.025f+.004f*(index%3));sfCoop.rocks.push_back(rock);
    }
    for (auto &rock : sfCoop.rocks) {
        rock.position.x+=rock.velocity.vx*dt;rock.position.y+=rock.velocity.vy*dt;
        if (rock.health>0) for (int owner=0;owner<2;++owner) {
            const auto *ship=sfCoopShip(owner);
            if (ship->pv>0 && vlong(ship->x-rock.position.x,ship->y-rock.position.y)<sfCoopShipRadius()+rock.radius) {
                sfCoopHurt(owner,16);rock.health=0;break;
            }
        }
        if (rock.health<=0) {
            for (int i=0;i<7;++i) {
                const float angle=i*2*float(PI)/7;
                sfCoop.dust.push_back({rock.position,tuplv(std::cos(angle)*25,std::sin(angle)*25),10});
            }
        }
    }
    sfCoop.rocks.erase(std::remove_if(sfCoop.rocks.begin(),sfCoop.rocks.end(),[](const auto &r){return r.health<=0 || r.position.x<-100 || r.position.x>sfArenaW+100;}),sfCoop.rocks.end());
    for (auto &dust : sfCoop.dust) {
        dust.life-=dt;dust.position.x+=dust.velocity.vx*dt;dust.position.y+=dust.velocity.vy*dt;
        int nearest=-1;float distance=sfArenaW*.12f;
        for (int owner=0;owner<2;++owner) {
            const auto *ship=sfCoopShip(owner);const float d=vlong(ship->x-dust.position.x,ship->y-dust.position.y);
            if (ship->pv>0 && d<distance) {nearest=owner;distance=d;}
        }
        if (nearest>=0) {
            auto *ship=sfCoopShip(nearest);
            dust.position.x+=(ship->x-dust.position.x)*std::min(1.0f,dt*7);
            dust.position.y+=(ship->y-dust.position.y)*std::min(1.0f,dt*7);
            if (distance<sfCoopShipRadius()) {
                sfAddShipHeat(ship,-4);ship->pv=std::min(1000.0f,ship->pv+8);dust.life=0;
            }
        }
    }
    sfCoop.dust.erase(std::remove_if(sfCoop.dust.begin(),sfCoop.dust.end(),[](const auto &d){return d.life<=0;}),sfCoop.dust.end());
}

static void sfCoopWin()
{
    sfCoop.phase=SfCoopPhase::Dying;sfCoop.phaseTime=0;
    sfCoop.shots.clear();sfCoop.beams.clear();sfCoop.waves.clear();
    auto next=sfCampaignSave;next.cleared=std::max(next.cleared,sfCoop.boss+1);
    next.selected=std::min(49,sfCoop.boss+1);next.pending=true;
    next.victory={};next.victory.id=(uint64_t(std::time(nullptr))<<24)^(SDL_GetPerformanceCounter()&0xffffffu);
    for (const auto &entry : next.fame) next.victory.id=std::max(next.victory.id,entry.id+1);
    if (!next.victory.id) next.victory.id=1;
    next.victory.boss=sfCoop.boss+1;next.victory.mode=sfActiveMode;
    next.victory.seconds=int(sfCoop.time);next.victory.date=std::time(nullptr);
    next.victory.score=std::max(0,int((sfCoop.boss+1)*500+(Spritej1->pv+Spritej2->pv)*2-sfCoop.time*5));
    sfCoop.pendingSaved=sfSaveCampaign(next);
    if (!sfCoop.pendingSaved) { sfCampaignSave=next;sfCoop.error=sfCampaignStorageError; }
    sfCoop.names=sfCampaignSave.names;
    if (sfActiveMode==SF_COOP_AI && sfCoop.names[0].empty()) sfCoop.names[0]="ORION IA";
}

static void sfCoopTick(float dt)
{
    if (sfCoop.phase!=SfCoopPhase::Combat) return;
    sfCoop.time+=dt;sfCoop.hit=std::max(0.0f,sfCoop.hit-dt);
    const auto &boss=sfCoopProfile();
    const float frequency=.35f+boss.family*.023f+boss.tier*.018f;
    sfCoop.position=tupl(sfArenaW*(.5f+.19f*std::sin(sfCoop.time*frequency+boss.index*.3f)),
                         sfArenaH*(.5f+.055f*std::sin(sfCoop.time*frequency*.71f)));
    sfCoop.motion.observe(sfCoop.position,dt);
    sfCoop.phaseNumber=sfCoop.health>boss.health*.65f ? 0 : sfCoop.health>boss.health*.3f ? 1 : 2;
    sfCoopMovePlayers(dt);sfCoopDefences(dt);
    sfCoop.attack-=dt;
    sfCoop.warning=sfCoop.attack<.65f ? 1-sfCoop.attack/.65f : 0;
    if (sfCoop.attack<=0) {
        int pattern=boss.family;
        if (sfCoop.phaseNumber>0 && sfCoop.volley%2) pattern=(boss.family+boss.tier+sfCoop.phaseNumber+1)%10;
        sfCoopPattern(pattern);
        sfCoop.attack=boss.interval*(sfCoop.phaseNumber==2 ? .82f : 1.0f);
        ++sfCoop.volley;
    }
    sfCoopProjectiles(dt);sfCoopResources(dt);
    if (sfCoop.health<=0 && (Spritej1->pv>0 || Spritej2->pv>0)) sfCoopWin();
    else if (Spritej1->pv<=0 && Spritej2->pv<=0) {
        sfCoop.phase=SfCoopPhase::Defeat;sfCoop.phaseTime=0;
        sfCoop.shots.clear();sfCoop.beams.clear();sfCoop.waves.clear();
    }
}

static void sfCampaignStart()
{
    sfLoadCampaign();sfCoop=SfCoopState{};
    if (!sfDuelShipStylesSaved) {
        for (int owner=0;owner<2;++owner) {
            const auto *s=sfCoopShip(owner);
            sfDuelShipStyles[owner]={s->w,s->h,s->sw,s->sh,s->speed,s->frames,s->frame,s->animated,s->boundedBreathing};
        }
        sfDuelShipStylesSaved=true;
    }
    sfCoop.boss=std::clamp(sfCampaignSave.selected,0,std::min(49,sfCampaignSave.cleared));
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
        sfCoop.boss=sfCampaignSave.victory.boss-1;sfCoop.phase=SfCoopPhase::Name;
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
static void sfCoopPlaySounds(Mix_Chunk *orange,Mix_Chunk *blue,Mix_Chunk *boss,Mix_Chunk *hit)
{
    static Uint64 last=0;const Uint64 now=SDL_GetTicks64();
    if (now-last<120) return;last=now;
    for (int owner=0;owner<2;++owner) if (sfCoop.soundShot[owner]) {
        Mix_Chunk *sound=owner==0 ? orange : blue;
        if (sound) Mix_PlayChannel(4+owner,sound,0);sfCoop.soundShot[owner]=false;
    }
    if (sfCoop.soundBoss && boss) Mix_PlayChannel(6,boss,0);
    if (sfCoop.soundHit && hit) Mix_PlayChannel(7,hit,0);
    sfCoop.soundBoss=sfCoop.soundHit=false;
}
static void sfCampaignSuspend()
{
    for (auto &control : sfCoop.controls) {control.down=false;control.finger=-1;control.velocity.set(0,0);}
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
    entry.rock=IMG_LoadTexture(renderer,"resources/assets/pict/aa1.png");
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
static SDL_Rect sfBossAtlasRect(SDL_Texture *texture,int index)
{
    // Measured gutters: the painted sheet is not a mathematically uniform grid.
    // Use its actual cells for both animated meshes and mission thumbnails.
    constexpr int columns[]{0,177,343,503,672,846,1025,1202,1392,1578,1774};
    constexpr int rows[]{0,156,332,511,693,887};
    int width,height;SDL_QueryTexture(texture,nullptr,nullptr,&width,&height);
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
static std::string sfDisplayText(const std::string &input)
{
    // Display Latin names with the existing high-contrast font; originals are
    // retained losslessly in the save. Unsupported scripts use a visible '?'.
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
                                              float hit=0,float dying=0)
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
        vertex.color={255,Uint8(hit>0 ? 195 : 255),Uint8(hit>0 ? 165 : 255),Uint8(255*(1-std::clamp(dying,0.0f,1.0f)))};
        vertex.tex_coord={(src.x+(.005f+u*.99f)*src.w)/twidth,(src.y+(.005f+v*.99f)*src.h)/theight};
    }
    return vertices;
}
static void sfDrawBoss(SDL_Renderer *renderer,SDL_Texture *atlas,int boss,tupl centre,float radius,float time,float hit=0,float dying=0)
{
    if (!atlas) {sfCoopDisc(renderer,centre.x,centre.y,radius*.6f,{180,60,230,220});return;}
    const auto vertices=sfBossVertices(atlas,boss,centre,radius,time,hit,dying);
    std::array<int,384> indices{};int offset=0;
    for (int y=0;y<8;++y) for (int x=0;x<8;++x) {
        const int a=y*9+x;
        for (int index : {a,a+1,a+9,a+1,a+10,a+9}) indices[offset++]=index;
    }
    SDL_SetTextureBlendMode(atlas,SDL_BLENDMODE_BLEND);
    SDL_RenderGeometry(renderer,atlas,vertices.data(),int(vertices.size()),indices.data(),int(indices.size()));
}

static void sfDrawCampaignSpace(SDL_Renderer *renderer,int boss,float time,int width,int height)
{
    auto &textures=sfCoopTextures(renderer);const auto &profile=sfBossCatalog()[boss];
    SDL_SetRenderDrawColor(renderer,2,5,16,255);SDL_RenderClear(renderer);
    if (textures.backgrounds) {
        auto source=sfAtlasRect(textures.backgrounds,profile.backdrop,3,2);
        const float aspect=float(width)/height;
        if (aspect<1) {
            const int wanted=int(source.h*aspect),travel=source.w-wanted;
            source.x+=int(travel*(.5f+.38f*std::sin(boss*1.71f+time*.011f)));source.w=wanted;
        } else {
            const int wanted=int(source.w/aspect),travel=source.h-wanted;
            source.y+=int(travel*(.5f+.38f*std::sin(boss*1.71f+time*.011f)));source.h=wanted;
        }
        SDL_Rect full{0,0,width,height};SDL_SetTextureColorMod(textures.backgrounds,155,165,190);
        SDL_RenderCopy(renderer,textures.backgrounds,&source,&full);
        SDL_SetTextureColorMod(textures.backgrounds,255,255,255);
    }
    if (textures.planets) {
        const auto source=sfAtlasRect(textures.planets,profile.planet,11,5);
        const float diameter=std::min(width,height)*(.36f+.012f*(boss%7));
        const float x=width*(.5f+.28f*std::sin(boss*1.37f+time*.024f));
        const float y=height*(boss%2 ? .77f+.045f*std::sin(time*.019f+boss) : .23f+.045f*std::sin(time*.019f+boss));
        int twidth,theight;SDL_QueryTexture(textures.planets,nullptr,nullptr,&twidth,&theight);
        std::array<SDL_Vertex,66> vertices{};std::array<int,192> indices{};
        const float uc=(source.x+source.w*.5f)/twidth,vc=(source.y+source.h*.5f)/theight;
        vertices[0]={{x,y},{180,190,220,190},{uc,vc}};
        // A circular source mask guarantees complete discs despite the atlas's
        // rectangular cells. Each encounter samples a distinct planet surface.
        for (int i=0;i<=64;++i) {
            const float a=i*2*float(PI)/64,rotation=time*.009f;
            vertices[i+1]={{x+std::cos(a)*diameter*.5f,y+std::sin(a)*diameter*.5f},{180,190,220,190},
                {uc+std::cos(a+rotation)*source.w*.405f/twidth,vc+std::sin(a+rotation)*source.w*.405f/theight}};
            if (i<64) {indices[i*3]=0;indices[i*3+1]=i+1;indices[i*3+2]=i+2;}
        }
        SDL_SetTextureBlendMode(textures.planets,SDL_BLENDMODE_BLEND);
        SDL_RenderGeometry(renderer,textures.planets,vertices.data(),int(vertices.size()),indices.data(),int(indices.size()));
        sfUiCircle(renderer,int(x),int(y),int(diameter*.5f),70,105,145);
    }
    uint32_t seed=0x1968ab12u+boss*71539u;
    for (int i=0;i<70;++i) {
        seed=seed*1664525u+1013904223u;const int x=seed%std::max(1,width);
        seed=seed*1664525u+1013904223u;const int y=seed%std::max(1,height);
        const auto alpha=Uint8(90+65*(1+std::sin(time*.7f+i)));
        SDL_SetRenderDrawColor(renderer,180,215,255,alpha);SDL_RenderDrawPoint(renderer,x,y);
    }
}

static void sfCoopDrawArena(SDL_Renderer *renderer,int width,int height)
{
    auto &textures=sfCoopTextures(renderer);
    sfDrawCampaignSpace(renderer,sfCoop.boss,sfCoop.time+sfCoop.phaseTime,width,height);
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
    for (const auto &rock : sfCoop.rocks) {
        SDL_Rect dest{int(rock.position.x-rock.radius),int(rock.position.y-rock.radius),int(rock.radius*2),int(rock.radius*2)};
        if (textures.rock) SDL_RenderCopyEx(renderer,textures.rock,nullptr,&dest,sfCoop.time*17,nullptr,SDL_FLIP_NONE);
    }
    for (const auto &dust : sfCoop.dust) sfCoopDisc(renderer,dust.position.x,dust.position.y,2.5f,{125,235,255,220});
    for (const auto &shot : sfCoop.shots) {
        SDL_Color color=shot.owner==0 ? SDL_Color{255,175,75,255} : shot.owner==1 ? SDL_Color{100,220,255,255} : SDL_Color{255,70,140,255};
        if (shot.kind==2) color=shot.age<1.2f ? SDL_Color{255,200,75,190} : SDL_Color{255,70,50,255};
        sfCoopDisc(renderer,shot.position.x,shot.position.y,shot.radius*1.7f,{color.r,color.g,color.b,65});
        sfCoopDisc(renderer,shot.position.x,shot.position.y,shot.radius,color);
    }
    const float death=sfCoop.phase==SfCoopPhase::Dying ? std::clamp(sfCoop.phaseTime/1.6f,0.0f,1.0f) :
        (sfCoop.phase==SfCoopPhase::Name || sfCoop.phase==SfCoopPhase::Saved ? 1.0f : 0.0f);
    if (sfCoop.warning>0 && death==0) {
        const int radius=int(sfCoopBossRadius()*(1.1f+.14f*sfCoop.warning));
        sfUiCircle(renderer,int(sfCoop.position.x),int(sfCoop.position.y),radius,255,120,90);
        sfUiCircle(renderer,int(sfCoop.position.x),int(sfCoop.position.y),radius+2,255,170,100);
    }
    sfDrawBoss(renderer,textures.bosses,sfCoop.boss,sfCoop.position,sfCoopBossRadius(),
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
    const int margin=std::max(8,width/30),barWidth=int(width*.58f),barHeight=std::max(7,width/80);
    sfCoopText(renderer,margin,int(height*.025f),"BOSS "+std::to_string(sfCoop.boss+1)+" / 50",width*.7f,std::max(2,width/220),{255,190,120,255});
    sfCoopText(renderer,margin,int(height*.05f),sfCoopProfile().name,int(width*.72f),std::max(2,width/260));
    SDL_Rect bossBar{margin,int(height*.079f),barWidth,barHeight};
    SDL_SetRenderDrawColor(renderer,30,28,45,235);SDL_RenderFillRect(renderer,&bossBar);
    bossBar.w=int(barWidth*std::clamp(sfCoop.health/sfCoopProfile().health,0.0f,1.0f));
    SDL_SetRenderDrawColor(renderer,230,65,125,255);SDL_RenderFillRect(renderer,&bossBar);
    sfCoopButton(renderer,{int(width*.81f),int(height*.02f),int(width*.16f),int(height*.062f)},"PAUSE");
    for (int owner=0;owner<2;++owner) {
        const auto *ship=sfCoopShip(owner);const int x=owner==0 ? margin : width/2+margin/2;
        const int w=int(width*.43f),y=int(height*.935f);
        const SDL_Color color=owner==0 ? SDL_Color{255,180,95,255} : SDL_Color{100,210,255,255};
        sfCoopText(renderer,x,int(height*.908f),owner==0 ? (sfActiveMode==SF_COOP_AI ? "ORION IA" : "PILOTE ORANGE") : "PILOTE BLEU",w,std::max(2,width/280),color);
        SDL_Rect bar{x,y,w,barHeight};SDL_SetRenderDrawColor(renderer,30,40,60,220);SDL_RenderFillRect(renderer,&bar);
        bar.w=int(w*std::clamp(ship->pv/1000,0.0f,1.0f));SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,255);SDL_RenderFillRect(renderer,&bar);
        sfCoopText(renderer,x,y+barHeight+6,"ENERGIE "+std::to_string(int(std::lround(100*(1-sfShipHeat(ship->nrj)/50))))+"%",w,std::max(2,width/300),color);
    }
}

static SDL_Rect sfCampaignCard(int index,int width,int height)
{
    const int columns=height>=width ? 5 : 10,rows=50/columns;
    const int cellWidth=int(width*.9f)/columns,cellHeight=int(height*.66f)/rows;
    return {int(width*.05f)+(index%columns)*cellWidth,int(height*.16f)+(index/columns)*cellHeight,cellWidth-4,cellHeight-4};
}
static void sfCampaignDrawSelect(SDL_Renderer *renderer)
{
    sfLoadCampaign();int width,height;SDL_GetRendererOutputSize(renderer,&width,&height);
    if (width<=0 || height<=0) return;
    sfDrawCampaignSpace(renderer,sfCampaignSave.selected,SDL_GetTicks64()*.001f,width,height);
    sfCoopCentered(renderer,width,int(height*.035f),"CAMPAGNE COOPERATIVE",std::max(2,width/150));
    sfCoopCentered(renderer,width,int(height*.095f),std::to_string(sfCampaignSave.cleared)+" VICTOIRES SUR 50",std::max(2,width/240));
    auto &textures=sfCoopTextures(renderer);
    for (int i=0;i<50;++i) {
        const auto rect=sfCampaignCard(i,width,height);const bool unlocked=i<=sfCampaignSave.cleared;
        sfUiPanel(renderer,rect,5,14,28,unlocked ? 80 : 35,unlocked ? 175 : 55,unlocked ? 185 : 70);
        if (textures.bosses) {
            const auto source=sfBossAtlasRect(textures.bosses,i);
            const int size=std::min(rect.w-6,rect.h-18);
            SDL_Rect spriteRect{rect.x+(rect.w-size)/2,rect.y+3,size,size};
            SDL_SetTextureColorMod(textures.bosses,unlocked ? 255 : 45,unlocked ? 255 : 45,unlocked ? 255 : 60);
            SDL_RenderCopy(renderer,textures.bosses,&source,&spriteRect);
        }
        sfCoopText(renderer,rect.x+5,rect.y+rect.h-15,std::to_string(i+1)+(i<sfCampaignSave.cleared ? " OK" : unlocked ? " GO" : " -"),rect.w-8,2);
    }
    if (textures.bosses) SDL_SetTextureColorMod(textures.bosses,255,255,255);
    sfCoopButton(renderer,{int(width*.08f),int(height*.86f),int(width*.84f),int(height*.065f)},sfCampaignSave.pending ? "INSCRIRE LA VICTOIRE EN ATTENTE" : "REPRENDRE LA CAMPAGNE");
    sfCoopButton(renderer,{int(width*.3f),int(height*.94f),int(width*.4f),int(height*.045f)},"ACCUEIL");
}

static void sfCampaignDrawHall(SDL_Renderer *renderer)
{
    sfLoadCampaign();int width,height;SDL_GetRendererOutputSize(renderer,&width,&height);
    if (width<=0 || height<=0) return;
    sfDrawCampaignSpace(renderer,49,SDL_GetTicks64()*.001f,width,height);
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
        sfCoopText(renderer,left,y+14+18*(scale+1),"BOSS "+std::to_string(entry.boss)+" / 50  SCORE "+std::to_string(entry.score)+"  "+std::to_string(entry.seconds)+" S",w,scale,{120,220,220,255});
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
        sfCoopCentered(renderer,width,int(height*.04f),sfCoop.boss==49 ? "LES 50 BOSS SONT VAINCUS" : "VICTOIRE COOPERATIVE",std::max(3,width/190),{255,215,125,255});
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
        sfCoopCentered(renderer,width,int(height*.32f),"BOSS "+std::to_string(sfCoop.boss+1)+" / 50",std::max(3,width/150),{255,210,125,255});
        sfCoopCentered(renderer,width,int(height*.40f),sfCoopProfile().name,std::max(3,width/190));
        sfCoopCentered(renderer,width,int(height*.51f),sfBossHints[sfCoopProfile().family],std::max(2,width/320));
        sfCoopCentered(renderer,width,int(height*.60f),"GLISSEZ POUR PILOTER - TIR AUTOMATIQUE",std::max(2,width/340));
        sfCoopCentered(renderer,width,int(height*.67f),"MINERAIS = ENERGIE - PROXIMITE = SECOURS",std::max(2,width/350));
    } else {
        const bool saved=sfCoop.phase==SfCoopPhase::Saved,paused=sfCoop.phase==SfCoopPhase::Paused;
        sfCoopCentered(renderer,width,int(height*.31f),saved ? "VICTOIRE IMMORTALISEE" : paused ? "PAUSE COOPERATIVE" : "EQUIPE HORS COMBAT",std::max(3,width/190),{255,220,130,255});
        if (saved) sfCoopCentered(renderer,width,int(height*.42f),sfCampaignSave.names[2],std::max(3,width/200));
        sfCoopButton(renderer,{int(width*.13f),int(height*.53f),int(width*.74f),int(height*.09f)},saved ? (sfCoop.boss==49 ? "VOIR LE HALL OF FAME" : "BOSS SUIVANT") : paused ? "REPRENDRE" : "REESSAYER CE BOSS");
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
            // Preserve spaces while typing; trim only on validation.
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
        if (y>=.71f && y<=.85f && x>=.5f) {sfLoadCampaign();sfFixRequestedScreen.store(SF_UI_HALL);}
        else if (y>=.545f && y<=.69f && (sfSelectedMode==SF_COOP_LOCAL || sfSelectedMode==SF_COOP_AI)) {
            sfLoadCampaign();sfFixRequestedScreen.store(SF_UI_CAMPAIGN);
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
            else {
                int selected=-1;
                if (y>=.85f && y<=.93f) selected=sfCampaignSave.selected;
                else for (int i=0;i<50;++i) {
                    const auto rect=sfCampaignCard(i,int(sfArenaW),int(sfArenaH));
                    if (x*sfArenaW>=rect.x && x*sfArenaW<rect.x+rect.w && y*sfArenaH>=rect.y && y*sfArenaH<rect.y+rect.h) {selected=i;break;}
                }
                if (selected>=0 && (selected<=sfCampaignSave.cleared || sfCampaignSave.pending)) {
                    sfCampaignSave.selected=std::clamp(selected,0,49);
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
                    else if (sfCoop.phase==SfCoopPhase::Saved && sfCoop.boss==49) sfCoopRequestHome(SF_UI_HALL);
                    else {if (sfCoop.phase==SfCoopPhase::Defeat) sfCampaignSave.selected=sfCoop.boss;sfFixLaunchPending.store(true);}
                } else if (y>=.68f && y<=.78f) sfCoopRequestHome(SF_UI_CAMPAIGN);
                else if (y>=.80f) sfCoopRequestHome();
            }
        }
        event->type=SDL_USEREVENT;return true;
    }
    if (event->type==SDL_FINGERDOWN && x>.79f && y<.10f) {
        sfCampaignSuspend();sfFixConsumedFingers.insert(finger);event->type=SDL_USEREVENT;return true;
    }
    if (event->type==SDL_FINGERDOWN) {
        const int owner=y<.5f ? 0 : 1;
        if (!(owner==0 && sfActiveMode==SF_COOP_AI) && !sfCoop.controls[owner].down && sfCoopShip(owner)->pv>0) {
            sfCoop.controls[owner].down=true;sfCoop.controls[owner].finger=finger;
        }
    }
    for (auto &control : sfCoop.controls) if (control.finger==finger) {
        control.target=tupl(std::clamp(x,0.0f,1.0f)*sfArenaW,std::clamp(y,0.0f,1.0f)*sfArenaH);
        if (event->type==SDL_FINGERUP) {control.down=false;control.finger=-1;}
    }
    event->type=SDL_USEREVENT;return true;
}
