#pragma once
#include "boss_catalog.hpp"
#include "campaign_save.hpp"

enum class SfCoopPhase { Intro,Combat,Dying,Name,Saved,Defeat,Paused };
struct SfCoopShot {
    tupl position,previous;tuplv velocity;
    float radius=5,damage=10,age=0,life=8,phase=0;
    int owner=-1,kind=0;
};
struct SfCoopBeam {tupl origin;float angle=0,age=0,warning=.9f;};
struct SfCoopWave {tupl origin;float age=0,radius=0;};
struct SfCoopRock {tupl position;tuplv velocity;float radius=20,health=25;};
struct SfCoopDust {tupl position;tuplv velocity;float life=10;};
struct SfCoopControl {SDL_FingerID finger=-1;tupl target;bool down=false;tuplv velocity;};
struct SfCoopState {
    SfCoopPhase phase=SfCoopPhase::Intro;
    int boss=0,volley=0,revives=3,phaseNumber=0,nameField=0;
    float time=0,phaseTime=0,health=900,attack=2,warning=0,hit=0,rockTimer=3;
    tupl position;SfMotionSample motion;
    std::array<SfCoopControl,2> controls{};
    std::array<float,2> cooldown{},invulnerable{},reviveProgress{};
    std::vector<SfCoopShot> shots;std::vector<SfCoopBeam> beams;
    std::vector<SfCoopWave> waves;std::vector<SfCoopRock> rocks;std::vector<SfCoopDust> dust;
    std::array<std::string,3> names{};
    bool keyboard=false,pendingSaved=false,soundBoss=false,soundHit=false;
    std::array<bool,2> soundShot{};
    std::string composition,error;
};
inline SfCoopState sfCoop;
inline int sfFamePage=0;
struct SfDuelShipStyle {float w,h,sw,sh;bool animated;};
inline std::array<SfDuelShipStyle,2> sfDuelShipStyles{};
inline bool sfDuelShipStylesSaved=false;

static sprite *sfCoopShip(int owner) {return owner==0 ? Spritej1 : Spritej2;}
static const SfBossProfile &sfCoopProfile() {return sfBossCatalog()[sfCoop.boss];}
static float sfCoopBossRadius() {return std::min(sfArenaW,sfArenaH)*(.11f+.00065f*sfCoop.boss);}
static float sfCoopShipRadius() {return std::min(sfArenaW,sfArenaH)*.053f;}
static SfVelocityGhost sfBossGhost() {return {sfCoop.position,sfCoop.motion.velocity};}
static float sfSegmentDistance(tupl a,tupl b,tupl point)
{
    const float dx=b.x-a.x,dy=b.y-a.y,length=dx*dx+dy*dy;
    const float t=length>.0001f ? std::clamp(((point.x-a.x)*dx+(point.y-a.y)*dy)/length,0.0f,1.0f) : 0;
    return vlong(a.x+dx*t-point.x,a.y+dy*t-point.y);
}
static tuplv sfUnitVelocity(tupl origin,tupl target,float speed)
{
    const float length=vlong(target.x-origin.x,target.y-origin.y);
    if (length<.0001f) return tuplv(0,0);
    enti heading;heading.xy=origin;heading.azim(target.x,target.y);
    return tuplv(heading.v.vxt*speed,heading.v.vyt*speed);
}
static void sfCoopEmit(tupl origin,float angle,float speed,int owner,float damage,int kind=0)
{
    if (sfCoop.shots.size()>=600) return;
    SfCoopShot s;s.position=s.previous=origin;s.velocity=tuplv(std::cos(angle)*speed,std::sin(angle)*speed);
    s.owner=owner;s.damage=damage;s.kind=kind;s.phase=angle;
    s.radius=std::min(sfArenaW,sfArenaH)*(owner<0 ? .009f : .006f);
    s.life=kind==2 ? 5 : 8;sfCoop.shots.push_back(s);
    if (owner>=0) sfCoop.soundShot[owner]=true;
}
static void sfCoopHurt(int owner,float damage)
{
    auto *ship=sfCoopShip(owner);
    if (ship->pv<=0 || sfCoop.invulnerable[owner]>0 || sfCoop.phase!=SfCoopPhase::Combat) return;
    ship->pv=std::max(0.0f,ship->pv-damage);sfAddShipHeat(ship,2);
    sfCoop.invulnerable[owner]=.38f;sfCoop.soundHit=true;
    if (ship->pv<=0) {sfCoop.controls[owner].down=false;sfCoop.controls[owner].velocity.set(0,0);}
}
static void sfCoopPattern(int pattern)
{
    sfCoop.soundBoss=true;
    const auto &boss=sfCoopProfile();const float speed=std::min(sfArenaW,sfArenaH)*boss.shotSpeed;
    const int tier=boss.tier,count=3+tier;
    const float rotation=sfCoop.volley*.37f+boss.index*.11f;
    if (pattern==3) {
        for (int owner=0;owner<2;++owner) if (sfCoopShip(owner)->pv>0) {
            const auto aim=sfShipGhost(owner).intercept(sfCoop.position,speed,.6f);
            sfCoop.beams.push_back({sfCoop.position,std::atan2(aim.y-sfCoop.position.y,aim.x-sfCoop.position.x),0,1-tier*.05f});
        }
    } else if (pattern==8) sfCoop.waves.push_back({sfCoop.position,0,0});
    else if (pattern==2 || pattern==5) {
        const int rays=pattern==2 ? 12+tier*2 : 4,gap=sfCoop.volley%rays;
        for (int i=0;i<rays;++i) {
            if (pattern==2 && (i==gap || i==(gap+1)%rays || i==(gap+rays/2)%rays)) continue;
            sfCoopEmit(sfCoop.position,rotation+i*2*float(PI)/rays,speed,-1,boss.damage);
        }
    } else if (pattern==1) {
        for (int arm=0;arm<4;++arm) for (int i=0;i<2+tier;++i)
            sfCoopEmit(sfCoop.position,rotation+arm*float(PI)*.5f+i*.12f,speed*(.8f+i*.06f),-1,boss.damage);
    } else if (pattern==4) {
        for (int i=0;i<4+tier;++i) {
            const float a=rotation+i*2*float(PI)/(4+tier);
            sfCoopEmit(tupl(sfCoop.position.x+std::cos(a)*sfCoopBossRadius(),sfCoop.position.y+std::sin(a)*sfCoopBossRadius()),
                       a,speed*.32f,-1,boss.damage*1.5f,2);
        }
    } else for (int owner=0;owner<2;++owner) if (sfCoopShip(owner)->pv>0) {
        tupl origin=sfCoop.position;
        if (pattern==9) origin.x+=sfCoopBossRadius()*std::sin(rotation+owner*float(PI))*1.3f;
        const auto aim=sfShipGhost(owner).intercept(origin,speed,.7f);
        const float a=std::atan2(aim.y-origin.y,aim.x-origin.x);
        for (int i=0;i<count;++i) sfCoopEmit(origin,a+(i-(count-1)*.5f)*(.12f+tier*.01f),speed,-1,boss.damage,
                                           pattern==6 ? 1 : pattern==7 ? 3 : 0);
    }
}
static float sfCoopRisk(tupl position,tuplv velocity)
{
    float risk=0;const float radius=sfCoopShipRadius();
    for (const auto &shot : sfCoop.shots) if (shot.owner<0 && shot.life>0) {
        const float dx=shot.position.x-position.x,dy=shot.position.y-position.y;
        const float vx=shot.velocity.vx-velocity.vx,vy=shot.velocity.vy-velocity.vy,speed=vx*vx+vy*vy;
        const float t=speed>.001f ? std::clamp(-(dx*vx+dy*vy)/speed,0.0f,.7f) : 0;
        const float clearance=radius+shot.radius+sfArenaW*.026f,distance=vlong(dx+vx*t,dy+vy*t);
        if (distance<clearance) risk+=1+(clearance-distance)/clearance*3;
    }
    for (const auto &b : sfCoop.beams) {
        const float range=std::max(sfArenaW,sfArenaH)*2;
        if (sfSegmentDistance(b.origin,tupl(b.origin.x+std::cos(b.angle)*range,b.origin.y+std::sin(b.angle)*range),
                              tupl(position.x+velocity.vx*.3f,position.y+velocity.vy*.3f))<radius+sfArenaW*.025f) risk+=4;
    }
    for (const auto &w : sfCoop.waves) if (w.age>.25f) {
        const float futureRadius=std::max(0.0f,w.age+.35f-.7f)*std::min(sfArenaW,sfArenaH)*.48f;
        if (std::abs(vlong(position.x+velocity.vx*.35f-w.origin.x,position.y+velocity.vy*.35f-w.origin.y)-futureRadius)<radius+sfArenaW*.03f) risk+=3;
    }
    for (const auto &r : sfCoop.rocks)
        if (vlong(position.x+velocity.vx*.4f-r.position.x-r.velocity.vx*.4f,
                  position.y+velocity.vy*.4f-r.position.y-r.velocity.vy*.4f)<radius+r.radius+sfArenaW*.02f) risk+=3;
    return risk;
}
static tuplv sfCoopAiVelocity(float dt)
{
    const tupl position(Spritej1->x,Spritej1->y);
    const auto aim=sfBossGhost().intercept(position,sfArenaW*1.4f,.9f);
    tupl goal(aim.x+sfArenaW*.16f*std::sin(sfCoop.time*.38f),aim.y-sfArenaH*.26f);
    if (Spritej2->pv<=0 && sfCoop.revives>0) goal=tupl(Spritej2->x,Spritej2->y);
    else if (Spritej1->nrj>22) {
        float best=std::numeric_limits<float>::max();
        for (const auto &dust : sfCoop.dust) {
            const float distance=vlong(dust.position.x-position.x,dust.position.y-position.y);
            if (distance<best && sfCoopRisk(dust.position,tuplv(0,0))<.1f) {best=distance;goal=dust.position;}
        }
    }
    const float maxSpeed=std::min(sfArenaW,sfArenaH)*.58f;
    goal.x=std::clamp(goal.x,sfArenaW*.12f,sfArenaW*.88f);goal.y=std::clamp(goal.y,sfArenaH*.12f,sfArenaH*.88f);
    const tuplv wanted=sfUnitVelocity(position,goal,std::min(maxSpeed,vlong(goal.x-position.x,goal.y-position.y)*3));
    tuplv chosen=wanted;float best=std::numeric_limits<float>::max();
    for (int i=-2;i<16;++i) {
        const float angle=i*2*float(PI)/16;
        const tuplv candidate=i==-2 ? wanted : i==-1 ? tuplv(0,0) : tuplv(std::cos(angle)*maxSpeed,std::sin(angle)*maxSpeed);
        const tupl future(position.x+candidate.vx*.35f,position.y+candidate.vy*.35f);
        const bool edge=future.x<sfArenaW*.07f || future.x>sfArenaW*.93f || future.y<sfArenaH*.11f || future.y>sfArenaH*.89f;
        const float near=vlong(future.x-sfCoop.position.x,future.y-sfCoop.position.y)<sfCoopBossRadius()+sfCoopShipRadius()*1.7f ? 8 : 0;
        const float score=sfCoopRisk(position,candidate)*10+(edge ? 12 : 0)+near+
                          vlong(candidate.vx-wanted.vx,candidate.vy-wanted.vy)/maxSpeed;
        if (score<best) {best=score;chosen=candidate;}
    }
    const auto old=sfCoop.controls[0].velocity;const float blend=1-std::exp(-dt*9);
    return tuplv(old.vx+(chosen.vx-old.vx)*blend,old.vy+(chosen.vy-old.vy)*blend);
}
static void sfCoopMovePlayers(float dt)
{
    const float radius=sfCoopShipRadius();
    for (int owner=0;owner<2;++owner) {
        auto *ship=sfCoopShip(owner);auto &c=sfCoop.controls[owner];
        sfCoop.invulnerable[owner]=std::max(0.0f,sfCoop.invulnerable[owner]-dt);
        if (ship->pv<=0) continue;
        if (owner==0 && sfActiveMode==SF_COOP_AI) c.velocity=sfCoopAiVelocity(dt);
        else if (c.down) c.velocity=sfUnitVelocity(tupl(ship->x,ship->y),c.target,
            std::min(std::min(sfArenaW,sfArenaH)*1.2f,vlong(c.target.x-ship->x,c.target.y-ship->y)*11));
        else {const float decay=std::exp(-dt*8);c.velocity.vx*=decay;c.velocity.vy*=decay;}
        ship->x=std::clamp(ship->x+c.velocity.vx*dt,radius,sfArenaW-radius);
        ship->y=std::clamp(ship->y+c.velocity.vy*dt,sfArenaH*.105f,sfArenaH*.895f);
        const float collision=sfCoopBossRadius()*.72f+radius;
        if (vlong(ship->x-sfCoop.position.x,ship->y-sfCoop.position.y)<collision) {
            auto push=sfUnitVelocity(sfCoop.position,tupl(ship->x,ship->y),collision);
            if (push.vx==0 && push.vy==0) push.vy=owner==0 ? -collision : collision;
            ship->x=std::clamp(sfCoop.position.x+push.vx,radius,sfArenaW-radius);
            ship->y=std::clamp(sfCoop.position.y+push.vy,sfArenaH*.105f,sfArenaH*.895f);
            sfCoopHurt(owner,30+sfCoop.boss);
        }
        ship->vx=ship->vy=0;ship->startup();sfObserved[owner].observe(tupl(ship->x,ship->y),dt);
        sfAddShipHeat(ship,-4.5f*dt);sfCoop.cooldown[owner]-=dt;
        if (ship->pv>0 && sfCoop.cooldown[owner]<=0) {
            const float speed=sfArenaW*(1.5f-ship->nrj*.014f);
            const auto aim=sfBossGhost().intercept(tupl(ship->x,ship->y),speed,.9f);
            const float a=std::atan2(aim.y-ship->y,aim.x-ship->x);
            sfCoopEmit(tupl(ship->x+std::cos(a)*radius,ship->y+std::sin(a)*radius),a,speed,owner,12);
            sfAddShipHeat(ship,1.8f);sfCoop.cooldown[owner]=.19f+ship->nrj*.004f;
        }
    }
    for (int owner=0;owner<2;++owner) if (sfCoopShip(owner)->pv<=0) {
        auto *down=sfCoopShip(owner);const auto *ally=sfCoopShip(1-owner);
        if (ally->pv>0 && sfCoop.revives>0 && vlong(ally->x-down->x,ally->y-down->y)<sfArenaW*.16f) {
            sfCoop.reviveProgress[owner]+=dt;
            if (sfCoop.reviveProgress[owner]>=2) {
                down->pv=450;down->nrj=15;--sfCoop.revives;sfCoop.invulnerable[owner]=2;
                sfCoop.reviveProgress[owner]=0;sfObserved[owner].valid=false;
            }
        } else sfCoop.reviveProgress[owner]=0;
    }
}
static void sfCoopDefences(float dt)
{
    for (int i=0;i<SF_TURRET_COUNT;++i) {
        auto &t=sfTurrets[i];t.deploy=std::min(1.0f,t.deploy+dt*1.8f);
        t.cooldown-=dt;t.flash=std::max(0.0f,t.flash-dt);
        const auto base=sfTurretBase(i);
        t.virtualTarget=sfBossGhost().intercept(sfTurretMuzzle(i),sfArenaW*1.25f,.9f);
        const float desired=std::atan2(t.virtualTarget.y-base.y,t.virtualTarget.x-base.x);
        const float delta=std::remainder(desired-t.angle,2*float(PI));t.angle+=std::clamp(delta,-dt*5,dt*5);
        if (t.deploy>=.99f && t.cooldown<=0 && std::abs(delta)<.1f) {
            t.flash=.16f;sfCoopEmit(sfTurretMuzzle(i),t.angle,sfArenaW*1.25f,i/SF_TURRETS_PER_TEAM,2.5f);
            t.cooldown=1.6f+(i%SF_TURRETS_PER_TEAM)*.12f;
        }
    }
}
static void sfCoopProjectiles(float dt)
{
    std::vector<SfCoopShot> bursts;
    for (auto &s : sfCoop.shots) {
        s.previous=s.position;s.age+=dt;s.life-=dt;
        if (s.kind==1 && s.age<.75f) {
            const int target=Spritej1->pv<=0 ? 1 : Spritej2->pv<=0 ? 0 :
                (vlong(s.position.x-Spritej1->x,s.position.y-Spritej1->y)<vlong(s.position.x-Spritej2->x,s.position.y-Spritej2->y) ? 0 : 1);
            const float speed=vlong(s.velocity.vx,s.velocity.vy);
            const auto aim=sfShipGhost(target).intercept(s.position,speed,.35f);
            s.phase+=std::clamp(std::remainder(std::atan2(aim.y-s.position.y,aim.x-s.position.x)-s.phase,2*float(PI)),-dt*.8f,dt*.8f);
            s.velocity=tuplv(std::cos(s.phase)*speed,std::sin(s.phase)*speed);
        }
        if (s.kind==2) {
            s.velocity.vx*=std::exp(-dt*1.4f);s.velocity.vy*=std::exp(-dt*1.4f);
            if (s.age>1.2f && s.life>0) for (int owner=0;owner<2;++owner) {
                const auto *ship=sfCoopShip(owner);
                if (ship->pv>0 && vlong(ship->x-s.position.x,ship->y-s.position.y)<sfArenaW*.13f) {
                    s.life=0;
                    for (int ray=0;ray<8;++ray) {
                        SfCoopShot b;b.position=b.previous=s.position;const float a=ray*float(PI)*.25f;
                        b.velocity=tuplv(std::cos(a)*sfArenaW*.34f,std::sin(a)*sfArenaW*.34f);
                        b.damage=s.damage;b.radius=sfArenaW*.008f;bursts.push_back(b);
                    }
                    break;
                }
            }
        }
        const float wobble=s.kind==3 ? std::sin(s.age*8)*sfArenaW*.10f : 0;
        s.position.x+=(s.velocity.vx-std::sin(s.phase)*wobble)*dt;
        s.position.y+=(s.velocity.vy+std::cos(s.phase)*wobble)*dt;
        if (s.life<=0) continue;
        if (s.owner>=0) {
            for (auto &rock : sfCoop.rocks) if (rock.health>0 && sfSegmentDistance(s.previous,s.position,rock.position)<rock.radius+s.radius) {
                rock.health-=s.damage;s.life=0;break;
            }
            if (s.life>0 && sfSegmentDistance(s.previous,s.position,sfCoop.position)<sfCoopBossRadius()*.75f+s.radius) {
                sfCoop.health=std::max(0.0f,sfCoop.health-s.damage);s.life=0;sfCoop.hit=.1f;
            }
        } else if (s.kind!=2 || s.age>1.2f) for (int owner=0;owner<2;++owner) {
            const auto *ship=sfCoopShip(owner);
            if (ship->pv>0 && sfSegmentDistance(s.previous,s.position,tupl(ship->x,ship->y))<sfCoopShipRadius()+s.radius) {
                sfCoopHurt(owner,s.damage);s.life=0;break;
            }
        }
        if (s.position.x<-50 || s.position.x>sfArenaW+50 || s.position.y<-50 || s.position.y>sfArenaH+50) s.life=0;
    }
    sfCoop.shots.erase(std::remove_if(sfCoop.shots.begin(),sfCoop.shots.end(),[](const auto &s){return s.life<=0;}),sfCoop.shots.end());
    for (const auto &s : bursts) if (sfCoop.shots.size()<600) sfCoop.shots.push_back(s);
    for (auto &b : sfCoop.beams) {
        b.age+=dt;
        if (b.age>=b.warning && b.age<b.warning+.5f) {
            const float range=std::max(sfArenaW,sfArenaH)*2;
            const tupl end(b.origin.x+std::cos(b.angle)*range,b.origin.y+std::sin(b.angle)*range);
            for (int owner=0;owner<2;++owner) {
                auto *ship=sfCoopShip(owner);
                if (sfSegmentDistance(b.origin,end,tupl(ship->x,ship->y))<sfCoopShipRadius()+sfArenaW*.016f)
                    sfCoopHurt(owner,sfCoopProfile().damage*1.6f);
            }
        }
    }
    sfCoop.beams.erase(std::remove_if(sfCoop.beams.begin(),sfCoop.beams.end(),[](const auto &b){return b.age>b.warning+.5f;}),sfCoop.beams.end());
    for (auto &w : sfCoop.waves) {
        const float previous=w.radius;w.age+=dt;w.radius=std::max(0.0f,w.age-.7f)*std::min(sfArenaW,sfArenaH)*.48f;
        if (w.age>.7f) for (int owner=0;owner<2;++owner) {
            const auto *ship=sfCoopShip(owner);const float d=vlong(ship->x-w.origin.x,ship->y-w.origin.y);
            if (d+sfCoopShipRadius()>=previous && d-sfCoopShipRadius()<=w.radius) sfCoopHurt(owner,sfCoopProfile().damage*1.3f);
        }
    }
    sfCoop.waves.erase(std::remove_if(sfCoop.waves.begin(),sfCoop.waves.end(),[](const auto &w){return w.radius>std::max(sfArenaW,sfArenaH)*1.2f;}),sfCoop.waves.end());
}
static void sfCoopResources(float dt)
{
    sfCoop.rockTimer-=dt;
    if (sfCoop.rockTimer<=0 && sfCoop.rocks.size()<10) {
        sfCoop.rockTimer=3.3f;const int n=int(sfCoop.time/3.3f);
        sfCoop.rocks.push_back({tupl(n%2 ? sfArenaW+35 : -35,sfArenaH*(.24f+.13f*(n%5))),
            tuplv((n%2 ? -1 : 1)*sfArenaW*.07f,std::sin(float(n))*sfArenaW*.025f),sfArenaW*(.025f+.004f*(n%3)),25});
    }
    for (auto &r : sfCoop.rocks) {
        r.position.x+=r.velocity.vx*dt;r.position.y+=r.velocity.vy*dt;
        if (r.health>0) for (int owner=0;owner<2;++owner) {
            const auto *s=sfCoopShip(owner);
            if (s->pv>0 && vlong(s->x-r.position.x,s->y-r.position.y)<sfCoopShipRadius()+r.radius) {sfCoopHurt(owner,16);r.health=0;break;}
        }
        if (r.health<=0) for (int i=0;i<7;++i) {
            const float a=i*2*float(PI)/7;
            sfCoop.dust.push_back({r.position,tuplv(std::cos(a)*25,std::sin(a)*25),10});
        }
    }
    sfCoop.rocks.erase(std::remove_if(sfCoop.rocks.begin(),sfCoop.rocks.end(),[](const auto &r){return r.health<=0 || r.position.x<-100 || r.position.x>sfArenaW+100;}),sfCoop.rocks.end());
    for (auto &d : sfCoop.dust) {
        d.life-=dt;d.position.x+=d.velocity.vx*dt;d.position.y+=d.velocity.vy*dt;
        int nearest=-1;float distance=sfArenaW*.12f;
        for (int owner=0;owner<2;++owner) {
            const auto *s=sfCoopShip(owner);const float gap=vlong(s->x-d.position.x,s->y-d.position.y);
            if (s->pv>0 && gap<distance) {nearest=owner;distance=gap;}
        }
        if (nearest>=0) {
            auto *s=sfCoopShip(nearest);
            if (distance<sfCoopShipRadius()) {sfAddShipHeat(s,-4);s->pv=std::min(1000.0f,s->pv+8);d.life=0;}
            else {
                const auto velocity=sfUnitVelocity(d.position,tupl(s->x,s->y),sfArenaW*.4f);
                d.position.x+=velocity.vx*dt;d.position.y+=velocity.vy*dt;
            }
        }
    }
    sfCoop.dust.erase(std::remove_if(sfCoop.dust.begin(),sfCoop.dust.end(),[](const auto &d){return d.life<=0;}),sfCoop.dust.end());
}
static void sfCoopWin()
{
    if (sfCoop.phase!=SfCoopPhase::Combat) return;
    sfCoop.phase=SfCoopPhase::Dying;sfCoop.phaseTime=0;
    sfCoop.shots.clear();sfCoop.beams.clear();sfCoop.waves.clear();
    auto next=sfCampaignSave;next.cleared=std::max(next.cleared,sfCoop.boss+1);
    next.selected=std::min(49,sfCoop.boss+1);next.pending=true;next.victory={};
    next.victory.id=(uint64_t(std::time(nullptr))<<24)^(SDL_GetPerformanceCounter()&0xffffffu);
    for (const auto &e : next.fame) next.victory.id=std::max(next.victory.id,e.id+1);
    if (!next.victory.id) next.victory.id=1;
    next.victory.boss=sfCoop.boss+1;next.victory.mode=sfActiveMode;
    next.victory.seconds=int(sfCoop.time);next.victory.date=std::time(nullptr);
    next.victory.score=std::max(0,int((sfCoop.boss+1)*500+(Spritej1->pv+Spritej2->pv)*2-sfCoop.time*5));
    sfCoop.pendingSaved=sfSaveCampaign(next);
    if (!sfCoop.pendingSaved) {sfCampaignSave=next;sfCoop.error=sfCampaignStorageError;}
    sfCoop.names=next.names;
    if (sfActiveMode==SF_COOP_AI && sfCoop.names[0].empty()) sfCoop.names[0]="ORION IA";
}
static void sfCoopTick(float dt)
{
    if (sfCoop.phase!=SfCoopPhase::Combat || dt<=0) return;
    sfCoop.time+=dt;sfCoop.hit=std::max(0.0f,sfCoop.hit-dt);
    const auto &boss=sfCoopProfile();const float f=.35f+boss.family*.023f+boss.tier*.018f;
    sfCoop.position=tupl(sfArenaW*(.5f+.19f*std::sin(sfCoop.time*f+boss.index*.3f)),sfArenaH*(.5f+.055f*std::sin(sfCoop.time*f*.71f)));
    sfCoop.motion.observe(sfCoop.position,dt);
    sfCoop.phaseNumber=sfCoop.health>boss.health*.65f ? 0 : sfCoop.health>boss.health*.3f ? 1 : 2;
    sfCoopMovePlayers(dt);sfCoopDefences(dt);sfCoop.attack-=dt;
    sfCoop.warning=sfCoop.attack<.65f ? 1-sfCoop.attack/.65f : 0;
    if (sfCoop.attack<=0) {
        int pattern=boss.family;
        if (sfCoop.phaseNumber>0 && sfCoop.volley%2) pattern=(boss.family+boss.tier+sfCoop.phaseNumber+1)%10;
        sfCoopPattern(pattern);sfCoop.attack=boss.interval*(sfCoop.phaseNumber==2 ? .82f : 1);++sfCoop.volley;
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
        for (int owner=0;owner<2;++owner) {const auto *s=sfCoopShip(owner);sfDuelShipStyles[owner]={s->w,s->h,s->sw,s->sh,s->animated};}
        sfDuelShipStylesSaved=true;
    }
    sfCoop.boss=std::clamp(sfCampaignSave.selected,0,std::min(49,sfCampaignSave.cleared));
    sfCoop.health=sfCoopProfile().health;sfCoop.position=tupl(sfArenaW*.5f,sfArenaH*.5f);
    for (int owner=0;owner<2;++owner) {
        auto *s=sfCoopShip(owner);const float size=std::min(sfArenaW,sfArenaH)*.15f;
        s->setxywh(sfArenaW*(owner==0 ? .35f : .65f),sfArenaH*(owner==0 ? .23f : .77f),size,size);
        s->pv=1000;s->nrj=0;s->animated=false;sfFixResetSpriteHistory(s);
        sfCoop.controls[owner].target=tupl(s->x,s->y);
    }
    sfObserved={};
    if (sfCampaignSave.pending) {
        sfCoop.boss=sfCampaignSave.victory.boss-1;sfCoop.phase=SfCoopPhase::Name;
        sfCoop.names=sfCampaignSave.names;sfCoop.pendingSaved=true;sfCoop.health=0;
        if (sfCampaignSave.victory.mode==SF_COOP_AI && sfCoop.names[0].empty()) sfCoop.names[0]="ORION IA";
    }
}
static void sfCampaignRestoreDuelShips()
{
    if (!sfDuelShipStylesSaved) return;
    for (int owner=0;owner<2;++owner) {
        auto *s=sfCoopShip(owner);const auto &v=sfDuelShipStyles[owner];
        s->w=v.w;s->h=v.h;s->sw=v.sw;s->sh=v.sh;s->animated=v.animated;s->startup();
    }
    sfDuelShipStylesSaved=false;
}
static void sfCampaignSuspend()
{
    for (auto &c : sfCoop.controls) {c.down=false;c.finger=-1;c.velocity.set(0,0);}
    sfObserved={};sfCoop.motion.valid=false;
    if (sfCoop.phase==SfCoopPhase::Combat) sfCoop.phase=SfCoopPhase::Paused;
    if (sfCoop.keyboard) {SDL_StopTextInput();sfCoop.keyboard=false;}
}
static void sfCoopPlaySounds(Mix_Chunk *orange,Mix_Chunk *blue,Mix_Chunk *boss,Mix_Chunk *hit)
{
    static Uint64 last=0;const Uint64 now=SDL_GetTicks64();if (now-last<120) return;last=now;
    for (int owner=0;owner<2;++owner) if (sfCoop.soundShot[owner]) {
        auto *sound=owner==0 ? orange : blue;if (sound) Mix_PlayChannel(4+owner,sound,0);sfCoop.soundShot[owner]=false;
    }
    if (sfCoop.soundBoss && boss) Mix_PlayChannel(6,boss,0);
    if (sfCoop.soundHit && hit) Mix_PlayChannel(7,hit,0);
    sfCoop.soundBoss=sfCoop.soundHit=false;
}
#include "campaign_scene.hpp"
#include "campaign_controls.hpp"
