#pragma once
#include <array>
#include <vector>
#include <limits>
#include "ship_energy.hpp"

extern std::list<sprite*> entitiesj1, entitiesj2, burnsj1, burnsj2;
extern std::list<parts*> particules, particulesr;
extern std::list<eexpl*> explos;
extern bool tirjz, tirj1z, tirj2z;

// Tactical velocities are pixels/second. Legacy asteroids and projectiles use
// pixels per frame multiplied by k0; convert at that boundary only.
inline float sfArenaW = 780, sfArenaH = 1680;
inline float sfFrameDt = 1.0f / 60.0f;
inline Uint64 sfTacticsLastTick = 0;
inline Uint64 sfNextAsteroidId = 0;
constexpr int SF_TURRETS_PER_TEAM = 6;
constexpr int SF_TURRET_COUNT = SF_TURRETS_PER_TEAM*2;

struct SfMotionSample {
    tupl previous;
    tuplv velocity;
    bool valid = false;
    void observe(tupl position, float dt) {
        if (!valid || vlong(position.x-previous.x, position.y-previous.y) > sfArenaW*.4f) {
            velocity.set(0, 0); valid = true;
        } else {
            const float blend = 1.0f - std::exp(-dt/.12f);
            const float limit = sfArenaW*2.5f;
            velocity.vx += (std::clamp((position.x-previous.x)/dt, -limit, limit)-velocity.vx)*blend;
            velocity.vy += (std::clamp((position.y-previous.y)/dt, -limit, limit)-velocity.vy)*blend;
        }
        previous = position;
    }
};
inline std::array<SfMotionSample, 2> sfObserved;

static tupl sfPredictIntercept(tupl origin, tupl target, tuplv velocity, float speed,
                                float horizon = .8f)
{
    const float dx = target.x-origin.x, dy = target.y-origin.y;
    const float a = velocity.vx*velocity.vx + velocity.vy*velocity.vy - speed*speed;
    const float b = 2*(dx*velocity.vx + dy*velocity.vy), c = dx*dx + dy*dy;
    float time = vlong(dx,dy)/std::max(1.0f,speed);
    if (std::abs(a) < .001f) {
        if (b < -.001f) time = -c/b;
    } else {
        const float discriminant = b*b-4*a*c;
        if (discriminant >= 0) {
            const float root = std::sqrt(discriminant);
            const float t1 = (-b-root)/(2*a), t2 = (-b+root)/(2*a);
            if (t1>0 && t2>0) time = std::min(t1,t2);
            else if (t1>0) time = t1;
            else if (t2>0) time = t2;
        }
    }
    time = std::clamp(time,0.0f,horizon);
    return tupl(target.x+velocity.vx*time,target.y+velocity.vy*time);
}

struct SfVelocityGhost {
    tupl position;
    tuplv velocity;
    tupl intercept(tupl origin,float speed,float horizon=.9f) const {
        return sfPredictIntercept(origin,position,velocity,speed,horizon);
    }
};
static SfVelocityGhost sfShipGhost(int owner) {
    const auto *ship=owner==0 ? Spritej1 : Spritej2;
    return {tupl(ship->x,ship->y),sfObserved[owner].velocity};
}
static float sfMainShotSpeed(float heat) {
    return (60-sfShipHeat(heat+1))*.4f*60*sfArenaW/780;
}

static float sfAsteroidRisk(tupl position, tuplv velocity, float radius)
{
    float risk = 0;
    const float units = std::max(.05f,k0)/sfFrameDt;
    for (const auto *rock : sa1) {
        if (rock->pv<=0) continue;
        const float dx = rock->x-position.x, dy = rock->y-position.y;
        const float vx = rock->vx*units-velocity.vx, vy = rock->vy*units-velocity.vy;
        const float speed2 = vx*vx+vy*vy;
        const float time = speed2>.01f ? std::clamp(-(dx*vx+dy*vy)/speed2,.12f,.85f) : .12f;
        const float clearance = radius+std::max(rock->w,rock->h)*.52f+sfArenaW*.035f;
        const float separation = vlong(dx+vx*time,dy+vy*time);
        if (separation<clearance) risk += 1+(clearance-separation)/clearance*3;
    }
    return risk;
}

static tuplv sfProjectileVelocity(const sprite *shot)
{
    if (shot->defensiveShot || shot->shotOwner>=0)
        return tuplv(shot->shotVelocityX,shot->shotVelocityY);
    return tuplv(shot->vx*k0/sfFrameDt,shot->vy*k0/sfFrameDt);
}

static float sfEnemyShotRisk(tupl position,tuplv velocity,float radius)
{
    float risk=0;
    for (const auto *shot : entitiesj1) {
        if (shot->pv<=0) continue;
        const tuplv flight=sfProjectileVelocity(shot);
        const float dx=shot->x-position.x, dy=shot->y-position.y;
        const float vx=flight.vx-velocity.vx, vy=flight.vy-velocity.vy;
        const float speed2=vx*vx+vy*vy;
        const float time=speed2>.01f ? std::clamp(-(dx*vx+dy*vy)/speed2,0.0f,.65f) : 0;
        const float clearance=radius+std::max(shot->w,shot->h)*.5f+sfArenaW*.018f;
        const float distance=vlong(dx+vx*time,dy+vy*time);
        if (distance<clearance) risk+=1+(clearance-distance)/clearance*3;
    }
    return risk;
}

static tuplv sfAvoidAsteroids(tupl position, tupl goal, tuplv current, float radius,
                              float maxY=-1)
{
    if (maxY<0) maxY=sfArenaH*.48f-radius*.25f;
    enti steering;
    steering.xy = position;
    steering.azim(goal.x,goal.y); // fablib heading and vector conversion
    const float maxSpeed = sfArenaW*.42f;
    const float speed = std::min(maxSpeed,vlong(goal.x-position.x,goal.y-position.y)*3);
    const tuplv wanted(steering.v.vxt*speed,steering.v.vyt*speed);
    std::array<tuplv,19> candidates;
    candidates[0]=wanted; candidates[1]=current; candidates[2]=tuplv(0,0);
    for (int i=0;i<16;++i) {
        const float angle = i*2*float(PI)/16;
        candidates[i+3]=tuplv(std::cos(angle)*maxSpeed,std::sin(angle)*maxSpeed);
    }
    tuplv best = wanted;
    float bestScore = std::numeric_limits<float>::max();
    for (auto candidate : candidates) {
        const float ax=candidate.vx-current.vx, ay=candidate.vy-current.vy;
        const float change=vlong(ax,ay), maxChange=maxSpeed*7*sfFrameDt;
        if (change>maxChange) {
            candidate.vx=current.vx+ax/change*maxChange;
            candidate.vy=current.vy+ay/change*maxChange;
        }
        const float futureX=position.x+candidate.vx*.25f, futureY=position.y+candidate.vy*.25f;
        const float edgePenalty=(futureX<radius || futureX>sfArenaW-radius ||
            futureY<radius || futureY>maxY) ? 8.0f : 0.0f;
        const float score=sfAsteroidRisk(position,candidate,radius)*8+
            sfEnemyShotRisk(position,candidate,radius)*12+edgePenalty+
            vlong(candidate.vx-wanted.vx,candidate.vy-wanted.vy)/maxSpeed;
        if (score<bestScore) { bestScore=score; best=candidate; }
    }
    return best;
}

enum class SfAiMode { Attack, Collect, Mine, RaidMine, Retreat };
struct SfPilot {
    SfAiMode mode = SfAiMode::Attack;
    tupl goal, aim;
    tuplv velocity;
    float rethink=0, aligned=0, cooldown=.35f;
    float aggressiveFor=0, nextAggression=2.5f, raidCooldown=0, raidAge=0, enemyTime=0;
    Uint64 asteroidId=0;
};
inline SfPilot sfPilot;
inline std::array<float,2> sfPickupGlow{};

static bool sfRoundActive()
{
    return sfUiScreen==SF_UI_GAME && !setgui && Spritej1 && Spritej2 &&
        Spritej1->pv>0 && Spritej2->pv>0 && loosej1->pv<=0 && loosej2->pv<=0;
}

// Owner 0 is the upper orange fortress; owner 1 is the lower blue fortress.
static sprite *sfMakeShot(int owner, bool defence=false)
{
    if (!sfRoundActive()) return nullptr;
    auto &shots=owner==0 ? entitiesj2 : entitiesj1;
    const auto weaponCount=std::count_if(shots.begin(),shots.end(),[defence](const sprite *s) {
        return s->defensiveShot==defence;
    });
    if (weaponCount>=(defence ? 72 : 50)) return nullptr;
    sprite *shot=owner==0 ? static_cast<sprite*>(new bj2) : static_cast<sprite*>(new bj1);
    shots.push_back(shot);
    (owner==0 ? tirj1 : tirj2)=int(shots.size());
    shot->idx=int(shots.size());
    shot->defensiveShot=defence;
    shot->shotOwner=owner;
    return shot;
}

static bool sfFireMain(int owner,const tupl *target=nullptr)
{
    sprite *ship=owner==0 ? Spritej1 : Spritej2;
    sprite *shot=sfMakeShot(owner);
    if (!shot) return false;
    sfAddShipHeat(ship,1);
    shot->x=ship->x; shot->y=ship->y;
    shot->vx=.5f*(rand()%3-1);
    shot->vy=(60-ship->nrj)*(owner==0 ? .4f : -.4f);
    shot->shotImpactHeat=std::clamp(shot->vy*shot->vy*.005f,0.0f,3.0f);
    shot->shotVelocityX=shot->vx*60*sfArenaW/780;
    shot->shotVelocityY=shot->vy*60*sfArenaW/780;
    if (target) {
        const float speed=std::abs(shot->shotVelocityY);
        const float angle=std::atan2(target->y-shot->y,target->x-shot->x);
        shot->shotVelocityX=std::cos(angle)*speed;shot->shotVelocityY=std::sin(angle)*speed;
    }
    if (ship->nrj<1.5f) {
        shot->name="miss"; sfAddShipHeat(ship,10);
        shot->shotVelocityX=0;
        shot->shotVelocityY=(owner==0 ? 1 : -1)*sfArenaW*.65f;
        shot->vy=shot->shotVelocityY*sfFrameDt/std::max(.05f,k0);
        shot->w=shot->sw*.5f; shot->vx=0; tirjz=true;
    } else (owner==0 ? tirj1z : tirj2z)=true;
    shot->vx=shot->shotVelocityX*sfFrameDt/std::max(.05f,k0);
    shot->vy=shot->shotVelocityY*sfFrameDt/std::max(.05f,k0);
    (owner==0 ? sfRmFlashJ1Start : sfRmFlashJ2Start)=std::max<Uint64>(1,SDL_GetTicks64());
    shot->startup();
    shot->shotFromX=shot->x; shot->shotFromY=shot->y;
    return true;
}

static void sfAdvanceProjectile(sprite *shot)
{
    if (shot->pv<=0) return;
    shot->shotFromX=shot->x; shot->shotFromY=shot->y;
    const bool missile=shot->shotOwner>=0 && shot->name=="miss";
    if (missile) {
        const float forward=shot->shotOwner==0 ? 1.0f : -1.0f;
        const sprite *target=shot->shotOwner==0 ? Spritej2 : Spritej1;
        // Bounded acceleration and lateral guidance in simulation time. The
        // old renderer multiplied vy every draw and also moved y a second time.
        const float speed=sfArenaW*std::min(1.5f,.65f+1.8f*(shot->shotAge+sfFrameDt*.5f));
        if (target && target->pv>0 && (target->y-shot->y)*forward>0) {
            const auto future=sfShipGhost(1-shot->shotOwner).intercept(tupl(shot->x,shot->y),speed,.6f);
            const float wanted=std::clamp((future.x-shot->x)*3,-sfArenaW*.35f,sfArenaW*.35f);
            shot->shotVelocityX+=std::clamp(wanted-shot->shotVelocityX,
                                          -sfArenaW*1.4f*sfFrameDt,sfArenaW*1.4f*sfFrameDt);
        }
        shot->shotVelocityY=forward*std::sqrt(std::max(0.0f,speed*speed-
                                                     shot->shotVelocityX*shot->shotVelocityX));
    }
    if (shot->defensiveShot || shot->shotOwner>=0) {
        shot->shotAge+=sfFrameDt;
        shot->vx=shot->shotVelocityX*sfFrameDt/std::max(.05f,k0);
        shot->vy=shot->shotVelocityY*sfFrameDt/std::max(.05f,k0);
    }
    shot->updatetir();
    // Legacy updatetir() computes its render bounds before movement. A fast
    // shot otherwise hits at a different position from the one being drawn.
    shot->startup();
    if (shot->shotOwner>=0 && shot->shotAge>(shot->defensiveShot || missile ? 4 : 10)) shot->pv=0;
}

static float sfShotHeat(const sprite *shot)
{
    // Weapon impact strength is independent of defence angle, display size,
    // missile acceleration and frame duration. Only the victim pays this cost.
    if (shot->defensiveShot) return 2.0f;
    if (shot->name=="miss") return 8.0f;
    if (shot->shotOwner>=0) return shot->shotImpactHeat;
    return std::clamp(shot->vy*shot->vy*.005f,0.0f,3.0f);
}

static bool sfShotCrosses(const sprite *target,const sprite *shot)
{
    if (shot->shotOwner<0 || shot->pv<=0 || target->pv<=0) return false;
    const float dx=shot->x-shot->shotFromX, dy=shot->y-shot->shotFromY;
    const float length2=dx*dx+dy*dy;
    const float time=length2>.001f ? std::clamp(((target->x-shot->shotFromX)*dx+
        (target->y-shot->shotFromY)*dy)/length2,0.0f,1.0f) : 0;
    // Same radius convention as legacy colee(), swept over the last movement.
    const float radius=(target->sh+shot->sh)/(1.7f*std::sqrt(1.7f));
    return vlong(shot->shotFromX+dx*time-target->x,shot->shotFromY+dy*time-target->y)<radius;
}

static SDL_Rect sfProjectileRect(const sprite *shot)
{
    const bool missile=shot->name=="miss";
    const int width=std::max(1,int(shot->w*(missile ? .5f : 1.0f)));
    const int height=std::max(1,int(shot->h*(missile ? 2.0f : 1.0f)));
    return SDL_Rect{int(std::lround(shot->x-width*.5f)),int(std::lround(shot->y-height*.5f)),width,height};
}

static double sfProjectileAngle(const sprite *shot)
{
    const auto velocity=sfProjectileVelocity(shot);
    return std::atan2(velocity.vy,velocity.vx)*180/PI+90;
}

static void sfDrawProjectiles(SDL_Renderer *renderer,const std::list<sprite*> &shots,
                              SDL_Texture *orb,SDL_Texture *missile)
{
    SDL_BlendMode blend;
    Uint8 red,green,blue,alpha;
    SDL_GetRenderDrawBlendMode(renderer,&blend);
    SDL_GetRenderDrawColor(renderer,&red,&green,&blue,&alpha);
    for (const auto *shot : shots) {
        if (shot->pv<=0) continue;
        const SDL_Rect rect=sfProjectileRect(shot);
        if (shot->name=="miss") {
            const auto velocity=sfProjectileVelocity(shot);
            const float speed=std::max(1.0f,vlong(velocity.vx,velocity.vy));
            const SDL_Point tail{int(shot->x-velocity.vx/speed*rect.h*.48f),
                                 int(shot->y-velocity.vy/speed*rect.h*.48f)};
            SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer,255,180,65,210);
            SDL_RenderDrawLine(renderer,tail.x,tail.y,
                int(tail.x-velocity.vx/speed*rect.h*.25f),int(tail.y-velocity.vy/speed*rect.h*.25f));
            SDL_RenderCopyEx(renderer,missile,nullptr,&rect,sfProjectileAngle(shot),nullptr,SDL_FLIP_NONE);
        } else SDL_RenderCopy(renderer,orb,nullptr,&rect);
    }
    SDL_SetRenderDrawBlendMode(renderer,blend);
    SDL_SetRenderDrawColor(renderer,red,green,blue,alpha);
}

static void sfBeginRetreat()
{
    sfPilot.mode=SfAiMode::Retreat; sfPilot.asteroidId=0;
    sfPilot.aggressiveFor=0; sfPilot.aligned=0; sfPilot.raidCooldown=8;
}

static sprite *sfFindRaidAsteroid()
{
    for (auto *rock : sa1)
        if (rock->pv>0 && rock->tacticalId==sfPilot.asteroidId && sfPilot.asteroidId) return rock;
    return nullptr;
}

static void sfRaidGoal(sprite *rock,float radius,float shotSpeed)
{
    const tuplv velocity(rock->vx*k0/sfFrameDt,rock->vy*k0/sfFrameDt);
    const float speed=std::max(1.0f,vlong(velocity.vx,velocity.vy));
    const float clearance=radius+std::max(rock->w,rock->h)*.52f+sfArenaW*.04f+speed*.18f;
    sfPilot.aim=sfPredictIntercept(tupl(Spritej1->x,Spritej1->y),tupl(rock->x,rock->y),velocity,shotSpeed,.35f);
    sfPilot.goal=tupl(rock->x+velocity.vx*.18f-velocity.vx/speed*clearance,
                     rock->y+velocity.vy*.18f-velocity.vy/speed*clearance);
}

static void sfThinkPilot()
{
    const tupl position(Spritej1->x,Spritej1->y);
    const float radius=std::max(Spritej1->w,Spritej1->h)*.43f;
    const float speed=sfMainShotSpeed(Spritej1->nrj);
    if (sfPilot.mode==SfAiMode::RaidMine) {
        sprite *rock=sfFindRaidAsteroid();
        if (!rock || rock->vy<=0 || sfPilot.raidAge>10 || sfPilot.enemyTime>1.0f ||
            Spritej1->y>sfArenaH*.57f || Spritej1->nrj>34 || Spritej1->pv<600 ||
            sfEnemyShotRisk(position,sfPilot.velocity,radius)>.5f) sfBeginRetreat();
        else { sfRaidGoal(rock,radius,speed); return; }
    }
    if (sfPilot.mode==SfAiMode::Retreat) {
        float best=std::numeric_limits<float>::max();
        for (float fraction : {.25f,.50f,.75f}) {
            const tupl goal(sfArenaW*fraction,sfArenaH*.29f);
            const float distance=std::max(1.0f,vlong(goal.x-position.x,goal.y-position.y));
            const tuplv velocity((goal.x-position.x)/distance*sfArenaW*.42f,
                                 (goal.y-position.y)/distance*sfArenaW*.42f);
            const float score=sfEnemyShotRisk(position,velocity,radius)*10+
                              sfAsteroidRisk(position,velocity,radius)*5+distance/sfArenaW;
            if (score<best) { best=score; sfPilot.goal=goal; }
        }
        if (Spritej1->y>sfArenaH*.34f) return;
        sfPilot.mode=SfAiMode::Attack;
    }
    if (sfPilot.aggressiveFor>0 && sfPilot.raidCooldown<=0 && Spritej1->nrj<23 && Spritej1->pv>=650) {
        sprite *chosen=nullptr; float best=std::numeric_limits<float>::max();
        for (auto *rock : sa1) {
            const float vy=rock->vy*k0/sfFrameDt;
            if (rock->pv<=0 || vy<sfArenaW*.025f || vy>sfArenaW*.32f ||
                std::abs(rock->vx)>rock->vy*.65f || rock->y<position.y+radius ||
                rock->y>sfArenaH*.57f || rock->x<radius*1.5f || rock->x>sfArenaW-radius*1.5f) continue;
            const float score=vlong(rock->x-position.x,rock->y-position.y);
            if (score<best) { chosen=rock; best=score; }
        }
        if (chosen) {
            if (!chosen->tacticalId) chosen->tacticalId=++sfNextAsteroidId;
            sfPilot.asteroidId=chosen->tacticalId; sfPilot.mode=SfAiMode::RaidMine;
            sfPilot.raidAge=sfPilot.enemyTime=0; sfRaidGoal(chosen,radius,speed); return;
        }
    }
    sfPilot.aim=sfShipGhost(1).intercept(position,speed);
    sfPilot.goal=tupl(sfPilot.aim.x,std::min(sfArenaH*(sfPilot.aggressiveFor>0 ? .36f : .29f),
                                            sfPilot.aim.y-sfArenaH*.28f));
    sfPilot.mode=SfAiMode::Attack;
    float bestDust=std::numeric_limits<float>::max();
    if (Spritej1->nrj>12 && (sfPilot.aggressiveFor<=0 || Spritej1->nrj>25)) {
        const int stride=std::max(1,int(particules.size())/96);
        int sample=0;
        for (const auto *dust : particules) {
            if (sample++%stride || dust->pv<=0 || dust->y>sfArenaH*.46f ||
                dust->x<radius || dust->x>sfArenaW-radius || dust->y<radius) continue;
            const float distance=vlong(dust->x-position.x,dust->y-position.y);
            if (distance>sfArenaH*.38f) continue;
            const tuplv velocity(vtan(dust->x-position.x,dust->y-position.y)*sfArenaW*.42f,
                                vtan(dust->y-position.y,dust->x-position.x)*sfArenaW*.42f);
            if (sfAsteroidRisk(position,velocity,radius)>.5f || sfEnemyShotRisk(position,velocity,radius)>.5f) continue;
            const float score=distance/std::max(.25f,std::min(1.0f,dust->pv*k0/600));
            if (score<bestDust) {
                bestDust=score; sfPilot.goal=tupl(dust->x,dust->y); sfPilot.mode=SfAiMode::Collect;
            }
        }
    }
    if (sfPilot.mode!=SfAiMode::Collect && Spritej1->nrj>18 && sfPilot.aggressiveFor<=0) {
        float bestRock=std::numeric_limits<float>::max();
        for (const auto *rock : sa1) {
            if (rock->pv<=0 || rock->y<radius*2 || rock->y>sfArenaH*.58f ||
                rock->x<radius || rock->x>sfArenaW-radius) continue;
            const float distance=vlong(rock->x-position.x,rock->y-position.y);
            if (distance<bestRock) {
                bestRock=distance;
                const float clearance=radius+std::max(rock->w,rock->h)*.6f+sfArenaW*.09f;
                sfPilot.aim=sfPredictIntercept(position,tupl(rock->x,rock->y),
                    tuplv(rock->vx*k0/sfFrameDt,rock->vy*k0/sfFrameDt),speed,.5f);
                sfPilot.goal=tupl(sfPilot.aim.x,rock->y-clearance); sfPilot.mode=SfAiMode::Mine;
            }
        }
    }
    sfPilot.goal.x=std::clamp(sfPilot.goal.x,radius,std::max(radius,sfArenaW-radius));
    sfPilot.goal.y=std::clamp(sfPilot.goal.y,radius,std::max(radius,sfArenaH*.45f));
}

static void sfUpdatePilot(float dt)
{
    if (!setia || !sfRoundActive()) { sfPilot.aligned=0; return; }
    sfPilot.rethink-=dt; sfPilot.cooldown-=dt;
    sfPilot.raidCooldown=std::max(0.0f,sfPilot.raidCooldown-dt);
    sfPilot.aggressiveFor=std::max(0.0f,sfPilot.aggressiveFor-dt); sfPilot.nextAggression-=dt;
    if (Spritej1->nrj>28 || Spritej1->pv<650) sfPilot.aggressiveFor=0;
    if (sfPilot.nextAggression<=0 && sfPilot.mode!=SfAiMode::Retreat) {
        sfPilot.nextAggression=4+rand()%4;
        if (Spritej1->nrj<23 && Spritej1->pv>=650 && rand()%3!=0) sfPilot.aggressiveFor=2.5f+(rand()%15)*.1f;
    }
    if (sfPilot.mode==SfAiMode::RaidMine) {
        sfPilot.raidAge+=dt;
        if (Spritej1->y>sfArenaH*.5f) sfPilot.enemyTime+=dt;
        const float radius=std::max(Spritej1->w,Spritej1->h)*.43f;
        if (sfEnemyShotRisk(tupl(Spritej1->x,Spritej1->y),sfPilot.velocity,radius)>.5f) {
            sfBeginRetreat(); sfPilot.rethink=0;
        }
    }
    if (sfPilot.rethink<=0) { sfThinkPilot(); sfPilot.rethink=.10f; }
    const float radius=std::max(Spritej1->w,Spritej1->h)*.43f;
    const tupl position(Spritej1->x,Spritej1->y);
    const bool excursion=sfPilot.mode==SfAiMode::RaidMine || sfPilot.mode==SfAiMode::Retreat;
    const float maxY=sfArenaH*(excursion ? .60f : .47f);
    sfPilot.velocity=sfAvoidAsteroids(position,sfPilot.goal,sfPilot.velocity,radius,maxY);
    Spritej1->x=std::clamp(position.x+sfPilot.velocity.vx*dt,radius,std::max(radius,sfArenaW-radius));
    Spritej1->y=std::clamp(position.y+sfPilot.velocity.vy*dt,radius,std::max(radius,maxY));
    iago->xy.setxy(Spritej1->x,Spritej1->y);
    iago->v.vx=sfPilot.velocity.vx; iago->v.vy=sfPilot.velocity.vy; iago->v.setvi();
    Spritej1->vx=Spritej1->vy=0; Spritej1->startup();
    const bool danger=sfAsteroidRisk(position,sfPilot.velocity,radius)>.6f ||
                      sfEnemyShotRisk(position,sfPilot.velocity,radius)>.6f;
    const bool aligned=std::abs(sfPilot.aim.x-Spritej1->x)<std::max(sfArenaW*.028f,Spritej2->w*.20f) &&
                       sfPilot.aim.y>Spritej1->y+radius;
    sfPilot.aligned=aligned && !danger ? sfPilot.aligned+dt : 0;
    const bool aggressive=sfPilot.aggressiveFor>0 || sfPilot.mode==SfAiMode::RaidMine;
    const auto mainShots=std::count_if(entitiesj2.begin(),entitiesj2.end(),[](const sprite *s){return !s->defensiveShot;});
    if (sfPilot.mode!=SfAiMode::Collect && sfPilot.mode!=SfAiMode::Retreat &&
        sfPilot.aligned>=(aggressive ? .10f : .16f) && sfPilot.cooldown<=0 &&
        Spritej1->nrj<43 && mainShots<12) {
        if (sfFireMain(0,&sfPilot.aim)) sfPilot.cooldown=aggressive ? .16f+Spritej1->nrj*.003f : .30f+Spritej1->nrj*.007f;
        sfPilot.aligned=0;
    }
}

static void sfCollectDust()
{
    if (!sfRoundActive()) return;
    for (auto *dust : particules) {
        if (dust->pv<=0) continue;
        sprite *winner=nullptr; int owner=-1;
        float nearest=std::numeric_limits<float>::max();
        for (int i=0;i<2;++i) {
            sprite *ship=i==0 ? Spritej1 : Spritej2;
            const float distance=vlong(dust->x-ship->x,dust->y-ship->y);
            if (distance<ship->h*.5f && distance<nearest) { nearest=distance; winner=ship; owner=i; }
        }
        if (winner) {
            const float value=std::clamp(dust->pv*k0/600,0.0f,1.0f);
            // nrj is depletion/heat: a lower value means MORE available energy.
            winner->nrj=sfShipHeat(sfShipHeat(winner->nrj)*.98f-.10f*value);
            winner->pv=std::min(1000.0f,winner->pv+.30f*value);
            dust->pv=0; sfPickupGlow[owner]=.65f;
        }
    }
}

struct SfTurret {
    float deploy=0, angle=0, cooldown=0, flash=0;
    bool alert=false;
    tupl virtualTarget; // Invisible prediction, never a renderable sprite.
};
inline std::array<SfTurret,SF_TURRET_COUNT> sfTurrets;

static float sfTurretRadius() { return std::clamp(sfArenaW*.031f,12.0f,42.0f); }

static tupl sfTurretBase(int index)
{
    const auto &t=sfTurrets[index];
    const float r=sfTurretRadius(), inset=r+8;
    const float ease=t.deploy*t.deploy*(3-2*t.deploy);
    const int slot=index%SF_TURRETS_PER_TEAM;
    const bool left=slot%2==0;
    const float ownY=slot<2 ? inset : slot<4 ? sfArenaH*.25f : sfArenaH*.5f-r*1.6f;
    const float y=index<SF_TURRETS_PER_TEAM ? ownY : sfArenaH-ownY;
    return tupl(left ? -r*2+(inset+r*2)*ease : sfArenaW+r*2-(inset+r*2)*ease,y);
}

static tupl sfTurretMuzzle(int index)
{
    const tupl base=sfTurretBase(index);
    const auto &t=sfTurrets[index];
    const float length=sfTurretRadius()*(1.85f-.23f*std::clamp(t.flash/.16f,0.0f,1.0f));
    return tupl(base.x+std::cos(t.angle)*length,base.y+std::sin(t.angle)*length);
}

static void sfUpdateTurrets(float dt)
{
    const bool active=sfRoundActive();
    for (int i=0;i<SF_TURRET_COUNT;++i) {
        auto &t=sfTurrets[i];
        const int owner=i/SF_TURRETS_PER_TEAM;
        const sprite *enemy=owner==0 ? Spritej2 : Spritej1;
        const float depth=owner==0 ? .5f-enemy->y/sfArenaH : enemy->y/sfArenaH-.5f;
        const float hullMargin=std::max(enemy->w,enemy->h)*.4f/sfArenaH;
        if (!active) t.alert=false;
        else if (depth>-.035f-hullMargin) t.alert=true;
        else if (depth<-.08f-hullMargin) t.alert=false;
        t.deploy=std::clamp(t.deploy+dt*(t.alert ? 2.2f : -1.3f),0.0f,1.0f);
        t.cooldown=std::max(0.0f,t.cooldown-dt); t.flash=std::max(0.0f,t.flash-dt);
        if (t.deploy<=0) continue;
        const tupl base=sfTurretBase(i);
        const tupl origin=sfTurretMuzzle(i);
        t.virtualTarget=sfShipGhost(1-owner).intercept(origin,sfArenaW*1.5f,.9f);
        const float desired=std::atan2(t.virtualTarget.y-base.y,t.virtualTarget.x-base.x);
        const float difference=std::remainder(desired-t.angle,2*float(PI));
        t.angle+=std::clamp(difference,-dt*5,dt*5);
        if (!t.alert || depth<=0 || t.deploy<.98f || std::abs(difference)>.12f || t.cooldown>0) continue;
        sprite *shot=sfMakeShot(owner,true);
        if (!shot) continue;
        t.flash=.16f;
        const tupl muzzle=sfTurretMuzzle(i);
        shot->x=muzzle.x; shot->y=muzzle.y;
        shot->shotVelocityX=std::cos(t.angle)*sfArenaW*1.5f;
        shot->shotVelocityY=std::sin(t.angle)*sfArenaW*1.5f;
        shot->shotFromX=shot->x; shot->shotFromY=shot->y;
        shot->vx=shot->shotVelocityX*sfFrameDt/std::max(.05f,k0);
        shot->vy=shot->shotVelocityY*sfFrameDt/std::max(.05f,k0);
        shot->w=shot->h=shot->sw=shot->sh=std::max(8.0f,sfArenaW*.022f); shot->startup();
        t.cooldown=.38f+(i%SF_TURRETS_PER_TEAM)*.035f;
        (owner==0 ? tirj1z : tirj2z)=true;
    }
}

static void sfTacticsReset()
{
    sfPilot=SfPilot{}; sfObserved={}; sfPickupGlow={}; sfTurrets={};
    sfSceneSeconds=0; sfTacticsLastTick=0;
    for (int i=0;i<SF_TURRET_COUNT;++i) {
        sfTurrets[i].angle=i<SF_TURRETS_PER_TEAM ? float(PI)*.5f : -float(PI)*.5f;
        sfTurrets[i].cooldown=(i%SF_TURRETS_PER_TEAM)*.06f;
    }
    for (auto *list : {&entitiesj1,&entitiesj2,&burnsj1,&burnsj2}) {
        for (auto *e : *list) delete e;
        list->clear();
    }
    for (auto *list : {&particules,&particulesr}) {
        for (auto *e : *list) delete e;
        list->clear();
    }
    for (auto *e : explos) delete e;
    explos.clear();
    tirj1=tirj2=0; tirjz=tirj1z=tirj2z=false;
    sfRmLastTirJ1=sfRmLastTirJ2=0; sfRmFlashJ1Start=sfRmFlashJ2Start=0;
}

static void sfTacticsBeginFrame(SDL_Renderer *renderer)
{
    sfFixApplyUiRequests();
    sfAddShipHeat(Spritej1,0); sfAddShipHeat(Spritej2,0);
    SDL_Rect viewport{}; SDL_RenderGetViewport(renderer,&viewport);
    sfArenaW=std::max(1,viewport.w); sfArenaH=std::max(1,viewport.h);
    const Uint64 now=SDL_GetTicks64();
    const Uint64 elapsed=sfTacticsLastTick ? now-sfTacticsLastTick : 16;
    sfTacticsLastTick=now;
    sfFrameDt=std::clamp(elapsed*.001f,.004f,.10f);
    if (elapsed>180 || sfUiScreen!=SF_UI_GAME) {
        sfObserved={}; sfPilot.velocity.set(0,0); sfPilot.aligned=0; sfPilot.rethink=0;
    }
    if (sfUiScreen==SF_UI_GAME) sfSceneSeconds+=sfFrameDt;
    if (!sfIsCoop()) {
        sfObserved[0].observe(tupl(Spritej1->x,Spritej1->y),sfFrameDt);
        sfObserved[1].observe(tupl(Spritej2->x,Spritej2->y),sfFrameDt);
    }
    for (auto &glow : sfPickupGlow) glow=std::max(0.0f,glow-sfFrameDt);
    if (!sfIsCoop()) {sfUpdatePilot(sfFrameDt);sfUpdateTurrets(sfFrameDt);}
}

static void sfTacticalRing(SDL_Renderer *renderer,tupl center,float radius,SDL_Color color)
{
    std::array<SDL_FPoint,49> points;
    for (int i=0;i<=48;++i) {
        const float angle=i*2*float(PI)/48;
        points[i]={center.x+std::cos(angle)*radius,center.y+std::sin(angle)*radius};
    }
    SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);
    SDL_RenderDrawLinesF(renderer,points.data(),int(points.size()));
}

static void sfCannonQuad(SDL_Renderer *renderer,tupl start,tupl end,float halfWidth,SDL_Color color)
{
    const float length=std::max(1.0f,vlong(end.x-start.x,end.y-start.y));
    const float nx=-(end.y-start.y)/length*halfWidth, ny=(end.x-start.x)/length*halfWidth;
    const SDL_Vertex vertices[]={{{start.x+nx,start.y+ny},color,{0,0}},
        {{end.x+nx,end.y+ny},color,{0,0}},{{end.x-nx,end.y-ny},color,{0,0}},
        {{start.x-nx,start.y-ny},color,{0,0}}};
    const int indices[]={0,1,2,0,2,3};
    SDL_RenderGeometry(renderer,nullptr,vertices,4,indices,6);
}

static void sfDrawTacticalEffects(SDL_Renderer *renderer)
{
    if (sfUiScreen!=SF_UI_GAME) return;
    SDL_BlendMode previous; SDL_GetRenderDrawBlendMode(renderer,&previous);
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    const float r=sfTurretRadius();
    for (int i=0;i<SF_TURRET_COUNT;++i) {
        const auto &t=sfTurrets[i];
        if (t.deploy<=0) continue;
        const tupl base=sfTurretBase(i), muzzle=sfTurretMuzzle(i);
        const SDL_Color team=i<SF_TURRETS_PER_TEAM ? SDL_Color{255,164,52,240} : SDL_Color{60,191,255,240};
        sfRmFilledCircle(renderer,int(base.x),int(base.y),int(r*1.2f),8,14,24,245);
        for (float scale : {1.12f,1.0f,.72f}) sfTacticalRing(renderer,base,r*scale,team);
        sfRmFilledCircle(renderer,int(base.x),int(base.y),int(r*.67f),72,89,105,255);
        sfCannonQuad(renderer,base,muzzle,r*.27f,{19,28,40,255});
        sfCannonQuad(renderer,base,muzzle,r*.15f,{150,168,181,255});
        sfTacticalRing(renderer,base,r*.30f,team);
        if (t.flash>0) {
            sfRmFilledCircle(renderer,int(muzzle.x),int(muzzle.y),int(r*.7f*t.flash/.16f),
                             team.r,team.g,team.b,130);
            sfRmFilledCircle(renderer,int(muzzle.x),int(muzzle.y),int(r*.28f),255,244,206,235);
        }
    }
    int sample=0;
    for (const auto *dust : particules) {
        if (dust->pv<=0 || sample++%4) continue;
        SDL_SetRenderDrawColor(renderer,255,220,140,180);
        SDL_RenderDrawLineF(renderer,dust->x-2,dust->y,dust->x+2,dust->y);
        SDL_RenderDrawLineF(renderer,dust->x,dust->y-2,dust->x,dust->y+2);
    }
    for (int i=0;i<2;++i) if (sfPickupGlow[i]>0) {
        const sprite *ship=i==0 ? Spritej1 : Spritej2;
        sfTacticalRing(renderer,tupl(ship->x,ship->y),ship->h*(.50f+(.65f-sfPickupGlow[i])*.15f),
            SDL_Color{140,255,205,Uint8(220*sfPickupGlow[i]/.65f)});
    }
    SDL_SetRenderDrawBlendMode(renderer,previous);
}
