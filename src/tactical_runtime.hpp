#pragma once
#include <array>
#include <vector>
#include <limits>

extern std::list<sprite*> entitiesj1, entitiesj2, burnsj1, burnsj2;
extern std::list<parts*> particules, particulesr;
extern std::list<eexpl*> explos;
extern bool tirjz, tirj1z, tirj2z;

// Tactical velocities are pixels/second. Legacy asteroids and projectiles use
// pixels per frame multiplied by k0; convert at that boundary only.
inline float sfArenaW = 780, sfArenaH = 1680;
inline float sfFrameDt = 1.0f / 60.0f;
inline Uint64 sfTacticsLastTick = 0;

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

static tuplv sfAvoidAsteroids(tupl position, tupl goal, tuplv current, float radius)
{
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
            futureY<radius || futureY>sfArenaH*.48f-radius*.25f) ? 8.0f : 0.0f;
        const float score=sfAsteroidRisk(position,candidate,radius)*8+edgePenalty+
            vlong(candidate.vx-wanted.vx,candidate.vy-wanted.vy)/maxSpeed;
        if (score<bestScore) { bestScore=score; best=candidate; }
    }
    return best;
}

enum class SfAiMode { Attack, Collect, Mine };
struct SfPilot {
    SfAiMode mode = SfAiMode::Attack;
    tupl goal, aim;
    tuplv velocity;
    float rethink=0, aligned=0, cooldown=.35f;
};
inline SfPilot sfPilot;
inline std::array<float,2> sfPickupGlow{};

static bool sfRoundActive()
{
    return sfUiScreen==SF_UI_GAME && !setgui && Spritej1 && Spritej2 &&
        Spritej1->pv>0 && Spritej2->pv>0 && loosej1->pv<=0 && loosej2->pv<=0;
}

// Owner 0 is the upper orange fortress; owner 1 is the lower blue fortress.
static sprite *sfMakeShot(int owner)
{
    if (!sfRoundActive()) return nullptr;
    auto &shots=owner==0 ? entitiesj2 : entitiesj1;
    if (shots.size()>=50) return nullptr;
    sprite *shot=owner==0 ? static_cast<sprite*>(new bj2) : static_cast<sprite*>(new bj1);
    shots.push_back(shot);
    (owner==0 ? tirj1 : tirj2)=int(shots.size());
    shot->idx=int(shots.size());
    return shot;
}

static bool sfFireMain(int owner)
{
    sprite *ship=owner==0 ? Spritej1 : Spritej2;
    sprite *shot=sfMakeShot(owner);
    if (!shot) return false;
    ship->nrj=std::clamp(ship->nrj+1,0.0f,50.0f);
    shot->x=ship->x; shot->y=ship->y;
    shot->vx=.5f*(rand()%3-1);
    shot->vy=(60-ship->nrj)*(owner==0 ? .4f : -.4f);
    if (ship->nrj<1.5f) {
        shot->name="miss"; ship->nrj+=10;
        shot->vy*=.01f; shot->h*=2; shot->vx=0; tirjz=true;
    } else (owner==0 ? tirj1z : tirj2z)=true;
    (owner==0 ? sfRmFlashJ1Start : sfRmFlashJ2Start)=std::max<Uint64>(1,SDL_GetTicks64());
    shot->startup();
    return true;
}

static void sfThinkPilot()
{
    const tupl position(Spritej1->x,Spritej1->y);
    const float radius=std::max(Spritej1->w,Spritej1->h)*.43f;
    const float speed=(60-std::clamp(Spritej1->nrj+1,0.0f,50.0f))*.4f*
        std::max(.05f,k0)/sfFrameDt;
    sfPilot.aim=sfPredictIntercept(position,tupl(Spritej2->x,Spritej2->y),sfObserved[1].velocity,speed);
    sfPilot.goal=tupl(sfPilot.aim.x,std::min(sfArenaH*.29f,Spritej2->y-sfArenaH*.28f));
    sfPilot.mode=SfAiMode::Attack;
    float bestDust=std::numeric_limits<float>::max();
    if (Spritej1->nrj>12) {
        const int stride=std::max(1,int(particules.size())/96);
        int sample=0;
        for (const auto *dust : particules) {
            if (sample++%stride || dust->pv<=0 || dust->y>sfArenaH*.46f ||
                dust->x<radius || dust->x>sfArenaW-radius || dust->y<radius) continue;
            const float distance=vlong(dust->x-position.x,dust->y-position.y);
            if (distance>sfArenaH*.38f) continue;
            const tuplv velocity(vtan(dust->x-position.x,dust->y-position.y)*sfArenaW*.42f,
                                vtan(dust->y-position.y,dust->x-position.x)*sfArenaW*.42f);
            if (sfAsteroidRisk(position,velocity,radius)>.5f) continue;
            const float score=distance/std::max(.25f,std::min(1.0f,dust->pv*k0/600));
            if (score<bestDust) {
                bestDust=score; sfPilot.goal=tupl(dust->x,dust->y); sfPilot.mode=SfAiMode::Collect;
            }
        }
    }
    if (sfPilot.mode!=SfAiMode::Collect && Spritej1->nrj>18) {
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
    if (sfPilot.rethink<=0) { sfThinkPilot(); sfPilot.rethink=.10f; }
    const float radius=std::max(Spritej1->w,Spritej1->h)*.43f;
    const tupl position(Spritej1->x,Spritej1->y);
    sfPilot.velocity=sfAvoidAsteroids(position,sfPilot.goal,sfPilot.velocity,radius);
    Spritej1->x=std::clamp(position.x+sfPilot.velocity.vx*dt,radius,std::max(radius,sfArenaW-radius));
    Spritej1->y=std::clamp(position.y+sfPilot.velocity.vy*dt,radius,std::max(radius,sfArenaH*.47f));
    iago->xy.setxy(Spritej1->x,Spritej1->y);
    iago->v.vx=sfPilot.velocity.vx; iago->v.vy=sfPilot.velocity.vy; iago->v.setvi();
    Spritej1->vx=Spritej1->vy=0; Spritej1->startup();
    const bool danger=sfAsteroidRisk(position,sfPilot.velocity,radius)>.6f;
    const bool aligned=std::abs(sfPilot.aim.x-Spritej1->x)<std::max(sfArenaW*.028f,Spritej2->w*.20f) &&
                       sfPilot.aim.y>Spritej1->y+radius;
    sfPilot.aligned=aligned && !danger ? sfPilot.aligned+dt : 0;
    if (sfPilot.mode!=SfAiMode::Collect && sfPilot.aligned>=.16f && sfPilot.cooldown<=0 &&
        Spritej1->nrj<43 && entitiesj2.size()<12) {
        if (sfFireMain(0)) sfPilot.cooldown=.30f+Spritej1->nrj*.007f;
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
            winner->nrj=std::max(0.0f,winner->nrj*.98f-.10f*value);
            winner->pv=std::min(1000.0f,winner->pv+.30f*value);
            dust->pv=0; sfPickupGlow[owner]=.65f;
        }
    }
}

struct SfTurret {
    float deploy=0, angle=0, cooldown=0, flash=0;
    bool alert=false;
};
inline std::array<SfTurret,6> sfTurrets;

static float sfTurretRadius() { return std::clamp(sfArenaW*.031f,12.0f,42.0f); }

static tupl sfTurretBase(int index)
{
    const auto &t=sfTurrets[index];
    const float r=sfTurretRadius(), inset=r+8;
    const float ease=t.deploy*t.deploy*(3-2*t.deploy);
    const bool left=index==0 || index==1 || index==4;
    const float y=index==0 || index==3 ? sfArenaH*.5f :
                  index<3 ? inset : sfArenaH-inset;
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
    for (int i=0;i<6;++i) {
        auto &t=sfTurrets[i];
        const int owner=i/3;
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
        const tupl aim=sfPredictIntercept(base,tupl(enemy->x,enemy->y),sfObserved[1-owner].velocity,
                                          sfArenaW*.95f,.6f);
        const float desired=std::atan2(aim.y-base.y,aim.x-base.x);
        const float difference=std::remainder(desired-t.angle,2*float(PI));
        t.angle+=std::clamp(difference,-dt*4,dt*4);
        if (!t.alert || depth<=0 || t.deploy<.98f || std::abs(difference)>.12f || t.cooldown>0) continue;
        sprite *shot=sfMakeShot(owner);
        if (!shot) continue;
        t.flash=.16f;
        const tupl muzzle=sfTurretMuzzle(i);
        shot->x=muzzle.x; shot->y=muzzle.y;
        const float speed=sfArenaW*.95f*sfFrameDt/std::max(.05f,k0);
        shot->vx=std::cos(t.angle)*speed; shot->vy=std::sin(t.angle)*speed;
        shot->w=shot->h=std::max(8.0f,sfArenaW*.022f); shot->startup();
        t.cooldown=.64f+(i%3)*.10f;
        (owner==0 ? tirj1z : tirj2z)=true;
    }
}

static void sfTacticsReset()
{
    sfPilot=SfPilot{}; sfObserved={}; sfPickupGlow={}; sfTurrets={};
    sfSceneSeconds=0; sfTacticsLastTick=0;
    for (int i=0;i<6;++i) {
        sfTurrets[i].angle=i<3 ? float(PI)*.5f : -float(PI)*.5f;
        sfTurrets[i].cooldown=(i%3)*.10f;
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
    sfObserved[0].observe(tupl(Spritej1->x,Spritej1->y),sfFrameDt);
    sfObserved[1].observe(tupl(Spritej2->x,Spritej2->y),sfFrameDt);
    for (auto &glow : sfPickupGlow) glow=std::max(0.0f,glow-sfFrameDt);
    sfUpdatePilot(sfFrameDt); sfUpdateTurrets(sfFrameDt);
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
    for (int i=0;i<6;++i) {
        const auto &t=sfTurrets[i];
        if (t.deploy<=0) continue;
        const tupl base=sfTurretBase(i), muzzle=sfTurretMuzzle(i);
        const SDL_Color team=i<3 ? SDL_Color{255,164,52,240} : SDL_Color{60,191,255,240};
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
