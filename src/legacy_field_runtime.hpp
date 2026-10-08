#pragma once

// One bounded, fixed-step owner for Fab's sprite field. Asteroids now meet a
// shared two-layer kinetic field before the historical energy shield/hull path.
static void sfFieldImpact(sprite *a,sprite *b,bool rocks=false)
{
    if(explos.size()>=256) return;
    explos.push_back(new eexpl("pous",xymr(a->x,b->x,a->w,b->w),xymr(a->y,b->y,a->h,b->h),
        rocks ? (a->h+a->w+b->h+b->w)/2 : sfArenaH/40,
        rocks ? (a->h+a->w+b->h+b->w)/8 : (a->h+a->w)/4,
        (a->vx+b->vx)*.1f,(a->vy+b->vy)*.1f));
}
static void sfMineAsteroid(sprite *rock,sprite *shot)
{
    if (rock->pv<=0 || shot->pv<=0) return;
    partsforiw(rock,shot);
    sfFieldMiningSound=true;
    sfFieldImpact(rock,shot);
    const float cm=sfArenaH/1000.0f;
    rock->w=std::max(0.0f,rock->w-cm);rock->h=std::max(0.0f,rock->h-cm);
    rock->sw=rock->w;rock->sh=rock->h;rock->startup();shot->pv=0;
    if (rock->w<sfArenaH/100) rock->pv=0;
}
static float sfKineticSegmentDistance(float ax,float ay,float bx,float by,float px,float py)
{
    const float dx=bx-ax,dy=by-ay,length=dx*dx+dy*dy;
    const float t=length>.0001f ? std::clamp(((px-ax)*dx+(py-ay)*dy)/length,0.0f,1.0f) : 0;
    return vlong(ax+dx*t-px,ay+dy*t-py);
}
static float sfKineticShipDiameter(const sprite *ship)
{
    return std::max(1.0f,std::max({ship->sw,ship->sh,ship->w,ship->h}));
}

static constexpr float SF_KINETIC_SURGE_MINING_VISUAL_INTERVAL=.24f;
static std::array<float,2> sfKineticSurgeMiningVisualClock{{0,0}};

static bool sfKineticSurgeMiningActive(int owner)
{
    return sfKineticSuctionVisible(owner);
}
static float sfKineticSurgeNoseY(int owner,const sprite *ship,float diameter)
{
    return ship->y+(owner==0 ? 1.0f : -1.0f)*diameter*.42f;
}
static void sfKineticAttractWhiteDust(int owner,float dt)
{
    auto *ship=owner==0 ? Spritej1 : Spritej2;
    if(!ship || ship->pv<=0 || dt<=0 || !sfKineticSurgeMiningActive(owner)) return;
    const float diameter=sfKineticShipDiameter(ship);
    const float range=diameter*sfKineticSurgeDustRangeDiameters();
    const float noseY=sfKineticSurgeNoseY(owner,ship,diameter);
    const float pull=1.0f-std::exp(-8.0f*dt);
    for(auto *dust:particules) {
        if(!dust || dust->pv<=0) continue;
        if(!sfKineticSurgeConeContains(owner,ship->x,noseY,dust->x,dust->y,range)) continue;
        const float dx=ship->x-dust->x,dy=ship->y-dust->y;
        dust->x+=dx*pull;dust->y+=dy*pull;
        dust->vx=dx*12.0f;dust->vy=dy*12.0f;
    }
}
static void sfKineticTideVisual(sprite *rock,sprite *ship,float diameter)
{
    if(!rock || !ship || particules.size()+29>=1000) return;
    const float dx=ship->x-rock->x,dy=ship->y-rock->y,d=std::max(1.0f,vlong(dx,dy));
    const float rr=std::max(rock->w,rock->h)*.42f;
    sprite tideProbe;
    const float probe=std::max(2.0f,diameter*.035f);
    tideProbe.setxywh(rock->x+dx/d*rr,rock->y+dy/d*rr,probe,probe);
    tideProbe.pv=1;tideProbe.vx=dx/d*.12f;tideProbe.vy=dy/d*.12f;tideProbe.startup();
    partsforiw(rock,&tideProbe);
    sfFieldImpact(rock,&tideProbe);
}
static void sfKineticSurgeMineAsteroids(float dt)
{
    if(dt<=0) return;
    for(int owner=0;owner<2;++owner) {
        if(!sfKineticSurgeMiningActive(owner)) {
            sfKineticSurgeMiningVisualClock[owner]=0;
            continue;
        }
        auto *ship=owner==0 ? Spritej1 : Spritej2;
        if(!ship || ship->pv<=0) continue;
        const float diameter=sfKineticShipDiameter(ship);
        const float range=diameter*sfKineticSurgeMiningRangeDiameters();
        const float noseY=sfKineticSurgeNoseY(owner,ship,diameter);
        sfKineticSurgeMiningVisualClock[owner]+=dt;
        const bool visualPulse=sfKineticSurgeMiningVisualClock[owner]>=SF_KINETIC_SURGE_MINING_VISUAL_INTERVAL;
        if(visualPulse) sfKineticSurgeMiningVisualClock[owner]=std::fmod(sfKineticSurgeMiningVisualClock[owner],SF_KINETIC_SURGE_MINING_VISUAL_INTERVAL);
        bool minedAny=false;
        for(auto *rock:sa1) {
            if(!rock || rock->pv<=0) continue;
            const float rockRadius=std::max(rock->w,rock->h)*.5f;
            const float centerDistance=vlong(rock->x-ship->x,rock->y-noseY);
            const float surface=std::max(0.0f,centerDistance-rockRadius);
            if(surface>range) continue;
            if(!sfKineticSurgeConeContains(owner,ship->x,noseY,rock->x,rock->y,range,rockRadius)) continue;
            const float normalized=std::clamp(surface/std::max(1.0f,range),0.0f,1.0f);
            const float shrink=sfArenaH*sfKineticMiningArenaFractionPerSecond(normalized)*dt;
            if(shrink<=0) continue;
            rock->w=std::max(0.0f,rock->w-shrink);rock->h=std::max(0.0f,rock->h-shrink);
            rock->sw=rock->w;rock->sh=rock->h;rock->startup();
            minedAny=true;
            if(visualPulse) sfKineticTideVisual(rock,ship,diameter);
            if(rock->w<sfArenaH/100.0f || rock->h<sfArenaH/100.0f) rock->pv=0;
        }
        if(minedAny && visualPulse) sfFieldMiningSound=true;
        sfKineticAttractWhiteDust(owner,dt);
    }
}
static SfKineticSolution sfKineticRockSolution(const sprite *rock,const sprite *ship,int owner)
{
    const float dx=rock->x-ship->x,dy=rock->y-ship->y,d=std::max(1.0f,vlong(dx,dy));
    const float area=std::max(1.0f,rock->w*rock->h);
    const float massRelative=std::clamp(sfKineticMassFactorFromArea(area,sfArenaH),0.0f,1.0f);
    return sfResolveKinetic(SF_KINETIC_ASTEROID_MAX_HULL_DAMAGE,massRelative,
        {rock->vx*60.0f,rock->vy*60.0f},{sfObserved[owner].velocity.vx,sfObserved[owner].velocity.vy},
        dx/d,dy/d,sfKineticReferenceSpeed(sfArenaW));
}

enum class SfKineticDustCause { SurgePurge, AsteroidCollision, KineticField };
struct SfKineticRedDustFlashColor { int r=255,g=220,b=45,a=255; };
struct SfKineticRedDustResponse {
    SfKineticDustMotion motion{};
    bool consumed=false;
    float flashStrength=0;
};
struct SfKineticDustFlash {
    float x=0,y=0,age=0,duration=.14f,intensity=1,progress=0;
};
static std::vector<SfKineticDustFlash> sfKineticDustFlashes;

static float sfKineticWhiteDustYieldFraction(SfKineticDustCause cause)
{
    return cause==SfKineticDustCause::SurgePurge ? 1.0f : .10f;
}
static int sfKineticWhiteDustParticleCount(float asteroidArea,float arenaHeight,SfKineticDustCause cause)
{
    const float area=std::max(0.0f,asteroidArea);
    if(area<=0) return 0;
    const float referenceYield=29.0f*area/sfKineticReferenceArea(arenaHeight);
    return std::max(1,int(std::lround(referenceYield*sfKineticWhiteDustYieldFraction(cause))));
}
static SfKineticRedDustFlashColor sfKineticRedDustFlashColor(float progress,float intensity)
{
    progress=std::clamp(progress,0.0f,1.0f);
    intensity=std::clamp(intensity,0.0f,1.0f);
    float g,b;
    if(progress<.5f) {
        const float t=progress*2.0f;
        g=220.0f+(120.0f-220.0f)*t;
        b=45.0f +(18.0f-45.0f)*t;
    } else {
        const float t=(progress-.5f)*2.0f;
        g=120.0f+(28.0f-120.0f)*t;
        b=18.0f +(28.0f-18.0f)*t;
    }
    SfKineticRedDustFlashColor out;
    out.r=int(190.0f+65.0f*intensity);
    out.g=int(g*(.55f+.45f*intensity));
    out.b=int(b*(.55f+.45f*intensity));
    out.a=int(120.0f+135.0f*intensity);
    return out;
}
static SfKineticRedDustResponse sfKineticRespondRedDust(float vx,float vy,float normalX,float normalY,
                                                         float strength,float variation,float arenaWidth)
{
    SfKineticRedDustResponse out;
    out.motion=sfKineticRespondDust(false,vx,vy,normalX,normalY,strength,variation,arenaWidth);
    if(!out.motion.deflected) return out;
    variation=std::clamp(variation,0.0f,1.0f);
    out.flashStrength=std::clamp(.62f+.38f*strength,0.0f,1.0f);
    // Canon: most red matter is consumed; a small deterministic fraction survives.
    out.consumed=variation<.78f;
    return out;
}
static void sfKineticRecordDustFlash(float x,float y,float intensity,float progress)
{
    if(sfKineticDustFlashes.size()>=96) sfKineticDustFlashes.erase(sfKineticDustFlashes.begin());
    SfKineticDustFlash flash;flash.x=x;flash.y=y;flash.intensity=std::clamp(intensity,0.0f,1.0f);
    flash.progress=std::clamp(progress,0.0f,1.0f);sfKineticDustFlashes.push_back(flash);
}
static void sfKineticAdvanceDustFlashes(float dt)
{
    for(auto &flash:sfKineticDustFlashes) flash.age+=std::max(0.0f,dt);
    sfKineticDustFlashes.erase(std::remove_if(sfKineticDustFlashes.begin(),sfKineticDustFlashes.end(),
        [](const auto &flash){return flash.age>=flash.duration;}),sfKineticDustFlashes.end());
}
static void sfKineticRedImpactVisual(float x,float y,float intensity)
{
    if(explos.size()>=256) return;
    const float size=std::max(2.0f,sfArenaH*(.0038f+.0022f*std::clamp(intensity,0.0f,1.0f)));
    explos.push_back(new eexpl("pous",x,y,size,std::max(2.0f,size*.34f),0,0));
}
static int sfKineticEmitWhiteDust(const sprite *rock,SfKineticDustCause cause,float incidentVx,float incidentVy)
{
    if(!rock) return 0;
    const int wanted=sfKineticWhiteDustParticleCount(std::max(0.0f,rock->w*rock->h),sfArenaH,cause);
    const bool stationary=cause==SfKineticDustCause::SurgePurge;
    const float speed=vlong(incidentVx,incidentVy);
    const float tangentX=speed>.0001f ? -incidentVy/speed : 0.0f;
    const float tangentY=speed>.0001f ?  incidentVx/speed : 0.0f;
    const float baseScale=1000.0f; // parts::update converts its velocity back with a 0.001 factor.
    int emitted=0;
    for(int i=0;i<wanted && particules.size()<1000;++i) {
        const float phase=wanted>1 ? float(i)/float(wanted-1) : .5f;
        const float angle=(i+.5f)*2.0f*float(PI)/std::max(1,wanted);
        const float radius=.16f*std::max(rock->w,rock->h)*(stationary ? .42f : .22f);
        auto *dust=new parts(rock->x+std::cos(angle)*radius,rock->y+std::sin(angle)*radius);
        dust->pv=600;
        if(stationary) {
            dust->vx=0;dust->vy=0;
        } else {
            const float spread=(phase-.5f)*.24f*speed*baseScale;
            dust->vx=incidentVx*baseScale+tangentX*spread;
            dust->vy=incidentVy*baseScale+tangentY*spread;
        }
        particules.push_back(dust);++emitted;
    }
    return emitted;
}
static bool sfKineticDestroyAsteroid(sprite *rock,SfKineticDustCause cause)
{
    if(!rock || rock->pv<=0) return false;
    const float vx=rock->vx,vy=rock->vy;
    const int emitted=sfKineticEmitWhiteDust(rock,cause,vx,vy);
    rock->pv=0;
    SDL_Log("KINETIC_WHITE_DUST cause=%d emitted=%d area=%.3f incident=(%.3f,%.3f)",
        int(cause),emitted,rock->w*rock->h,vx,vy);
    return true;
}
static void sfKineticEmitRedDust(int owner,int count,float ring)
{
    if(owner<0 || owner>1 || count<=0) return;
    auto *ship=owner==0 ? Spritej1 : Spritej2;
    for(int i=0;i<count && particulesr.size()<1000;++i) {
        const float a=(i+.5f)*2*float(PI)/count+owner*.41f;
        auto *dust=new parts(ship->x+std::cos(a)*ring,ship->y+std::sin(a)*ring);
        dust->pv=600;
        dust->vx=std::cos(a)*sfArenaW*.72f;
        dust->vy=std::sin(a)*sfArenaW*.72f;
        particulesr.push_back(dust);
    }
}
static int sfKineticPurgeAsteroids(int owner)
{
    if(owner<0 || owner>1) return 0;
    auto *ship=owner==0 ? Spritej1 : Spritej2;
    if(!ship || ship->pv<=0) return 0;
    const float diameter=sfKineticShipDiameter(ship);
    const float radius=diameter*SF_KINETIC_SURGE_BLAST_DIAMETER*.5f;
    int purged=0;
    for(auto *rock:sa1) {
        if(!rock || rock->pv<=0) continue;
        const float rockRadius=std::max(rock->w,rock->h)*.5f;
        if(vlong(rock->x-ship->x,rock->y-ship->y)>radius+rockRadius) continue;
        sfFieldImpact(rock,ship);
        if(sfKineticDestroyAsteroid(rock,SfKineticDustCause::SurgePurge)) ++purged;
    }
    if(purged>0) {
        sfKineticTriggerWave(owner,SF_KINETIC_SURGE_BLAST_DIAMETER*.5f,1.0f,sfKineticEnergyFraction(ship->nrj));
        sfKineticEmitRedDust(owner,std::min(12,3+purged),diameter*.46f);
        SDL_Log("KINETIC_PURGE owner=%d asteroids=%d maxRadius=%.3f",owner,purged,radius);
    }
    return purged;
}
static void sfKineticUpdateEffects(float dt)
{
    if(dt<=0) return;
    for(auto &wave:sfKineticWaves) {
        if(wave.owner<0 || wave.owner>1) continue;
        auto *ship=wave.owner==0 ? Spritej1 : Spritej2;
        if(!ship || ship->pv<=0) continue;
        const float diameter=sfKineticShipDiameter(ship);
        const float previousRadius=sfKineticWaveRadiusAt(wave,diameter,wave.age);
        const float nextAge=std::min(wave.duration,wave.age+dt);
        const float currentRadius=sfKineticWaveRadiusAt(wave,diameter,nextAge);
        const float shellMin=std::max(0.0f,std::min(previousRadius,currentRadius)-diameter*.055f);
        const float shellMax=std::max(previousRadius,currentRadius)+diameter*.055f;
        const float progress=sfKineticWaveProgressAt(wave,nextAge);
        if(!wave.loggedMid && progress>=.50f) {
            SDL_Log("KINETIC_WAVE owner=%d start=0 currentRadius=%.3f maxRadius=%.3f impactStrength=%.3f",
                wave.owner,currentRadius,diameter*wave.maxRadiusShipDiameters,wave.strength);
            wave.loggedMid=true;
        }
        int index=0;
        for(auto *dust:particulesr) {
            if(!dust || dust->pv<=0) {++index;continue;}
            const float dx=dust->x-ship->x,dy=dust->y-ship->y,d=std::max(1.0f,vlong(dx,dy));
            if(d<shellMin || d>shellMax) {++index;continue;}
            const float beforeVx=dust->vx,beforeVy=dust->vy;
            const float variation=.5f+.5f*std::sin(index*1.73f+wave.serial*.61f+d*.019f);
            const auto response=sfKineticRespondRedDust(dust->vx,dust->vy,dx/d,dy/d,wave.strength,variation,sfArenaW);
            const auto color=sfKineticRedDustFlashColor(progress,response.flashStrength);
            (void)color;
            sfKineticRecordDustFlash(dust->x,dust->y,response.flashStrength,progress);
            sfKineticRedImpactVisual(dust->x,dust->y,response.flashStrength);
            if(response.consumed) {
                dust->pv=0;
            } else {
                dust->vx=response.motion.vx;dust->vy=response.motion.vy;
                dust->pv=std::max(dust->pv,560.0f/std::max(.001f,k0));
                const float wobble=std::sin(wave.serial*.77f+index*1.31f+progress*18.0f)*diameter*.018f*wave.strength;
                dust->x+=(-dy/d)*wobble;dust->y+=(dx/d)*wobble;
            }
            if(!wave.loggedRed) {
                SDL_Log("KINETIC_DUST type=red consumed=%s flash=(%d,%d,%d) vibrated=%s deflected=%s velocityBefore=(%.3f,%.3f) velocityAfter=(%.3f,%.3f)",
                    response.consumed?"true":"false",color.r,color.g,color.b,
                    response.motion.vibrated?"true":"false",response.motion.deflected?"true":"false",
                    beforeVx,beforeVy,response.motion.vx,response.motion.vy);
                wave.loggedRed=true;
            }
            ++index;
        }
        if(!wave.loggedWhite) for(auto *dust:particules) {
            if(!dust || dust->pv<=0) continue;
            const float d=vlong(dust->x-ship->x,dust->y-ship->y);
            if(d<shellMin || d>shellMax) continue;
            SDL_Log("KINETIC_DUST type=white vibrated=false deflected=false velocityBefore=(%.3f,%.3f) velocityAfter=(%.3f,%.3f)",
                dust->vx,dust->vy,dust->vx,dust->vy);
            wave.loggedWhite=true;break;
        }
    }
    sfKineticAdvanceWaves(dt);
    sfKineticAdvanceDustFlashes(dt);
}
static bool sfKineticFragmentRock(sprite *rock,const sprite *ship,int owner,SfKineticLayer layer,
                        const SfKineticSolution &solution)
{
    if(!rock || rock->pv<=0 || !ship) return false;
    const int count=layer==SfKineticLayer::Outer ? 6 : 4;
    const int nextStage=layer==SfKineticLayer::Outer ? 1 : 2;
    const float scale=1.0f/std::sqrt(float(count));
    const float smallest=sfArenaH/260.0f;
    if(std::max(rock->w,rock->h)*scale<smallest)
        return sfKineticDestroyAsteroid(rock,SfKineticDustCause::KineticField);
    if(sa1.size()+count>200) return false;
    const float shipVx=sfObserved[owner].velocity.vx,shipVy=sfObserved[owner].velocity.vy;
    const float relX=rock->vx*60.0f-shipVx,relY=rock->vy*60.0f-shipVy;
    const float baseAngle=std::atan2(relY,relX);
    const float retained=std::sqrt(std::clamp(solution.residualDamage/std::max(.001f,solution.rawDamage),.02f,1.0f));
    const int penetrators=std::max(1,int(std::ceil(count*std::clamp(1.0f-solution.dissipationFraction,0.05f,.55f))));
    for(int i=0;i<count;++i) {
        auto *fragment=new sprite;
        const float fw=std::max(2.0f,rock->w*scale),fh=std::max(2.0f,rock->h*scale);
        fragment->setxywh(rock->x,rock->y,fw,fh);fragment->sw=fw;fragment->sh=fh;
        fragment->pv=1;fragment->name=rock->name;fragment->timer=1;fragment->kineticStage=nextStage;
        float angle;
        if(i<penetrators) angle=baseAngle+(i-(penetrators-1)*.5f)*.16f;
        else {
            const float side=(i&1) ? 1.0f : -1.0f;
            angle=baseAngle+side*(.72f+.25f*(i-penetrators));
        }
        const float relSpeed=solution.relativeSpeed*retained*(i<penetrators ? .82f : .62f);
        fragment->vx=(shipVx+std::cos(angle)*relSpeed)/60.0f;
        fragment->vy=(shipVy+std::sin(angle)*relSpeed)/60.0f;
        fragment->startup();
        if(i==0) SDL_Log("KINETIC_DUST type=debris vibrated=false deflected=true velocityBefore=(%.3f,%.3f) velocityAfter=(%.3f,%.3f)",
            rock->vx,rock->vy,fragment->vx,fragment->vy);
        sa1.push_back(fragment);
    }
    return sfKineticDestroyAsteroid(rock,SfKineticDustCause::KineticField);
}
static bool sfKineticTryLayer(sprite *rock,sprite *ship,int owner,SfKineticLayer layer,
                    const SfKineticSolution &raw,bool *interacted=nullptr)
{
    if(interacted) *interacted=false;
    const auto solved=sfApplyKineticLayer(raw,layer,sfKineticEnergyFraction(ship->nrj),sfKineticSurgePower(owner));
    if(solved.dissipationFraction<=.001f) return false;
    if(interacted) *interacted=true;
    sfAddShipHeat(ship,solved.energyCost);
    const float strength=std::clamp(.28f+solved.dissipationFraction*.72f,0.0f,1.0f);
    sfKineticTriggerWave(owner,raw.maxRadiusShipDiameters,strength,sfKineticEnergyFraction(ship->nrj));
    const float diameter=sfKineticShipDiameter(ship);
    sfKineticEmitRedDust(owner,layer==SfKineticLayer::Outer ? 7 : 4,diameter*.46f);
    SDL_Log("KINETIC_IMPACT owner=%d massFactor=%.5f relativeSpeed=%.3f impactSpeed=%.3f rawDamage=%.5f residualDamage=%.5f selectedRange=%d maxRadius=%.3f energyCost=%.8f",
        owner,raw.massFactor,raw.relativeSpeed,raw.impactSpeed,raw.rawDamage,solved.residualDamage,
        raw.selectedRange,diameter*raw.maxRadiusShipDiameters,solved.energyCost);
    SDL_Log("KINETIC_WAVE owner=%d start=0 currentRadius=0 maxRadius=%.3f impactStrength=%.3f",
        owner,diameter*raw.maxRadiusShipDiameters,strength);
    if(sfKineticFragmentRock(rock,ship,owner,layer,solved)) return true;
    // Population cap fallback: preserve matter, shed kinetic speed and deflect it sideways.
    const float dx=rock->x-ship->x,dy=rock->y-ship->y,d=std::max(1.0f,vlong(dx,dy));
    const float retained=std::sqrt(std::clamp(solved.residualDamage/std::max(.001f,solved.rawDamage),.02f,1.0f));
    const float wx=rock->vx*retained,wy=rock->vy*retained;
    rock->vx=(-dy/d)*vlong(wx,wy)*(owner ? -1.0f : 1.0f);rock->vy=(dx/d)*vlong(wx,wy)*(owner ? -1.0f : 1.0f);
    rock->kineticStage=layer==SfKineticLayer::Outer ? 1 : 2;
    return false;
}

static void sfLegacyFieldStep(void (*hurt)(int,float))
{
    if (sa1.empty()) {setastswall();setasts(2);}
    else if (sa1.size()<8) setasts(2);
    std::vector<sprite*> rocks(sa1.begin(),sa1.end());
    for (auto *rock:rocks) {
        if (rock->pv<=0) continue;
        const float fromX=rock->x,fromY=rock->y;
        rock->ast();
        rock->tout=inxy(rock) ? 0 : rock->tout+1;
        if (rock->tout>60*30) rock->pv=0;
        for (int owner=0;owner<2 && rock->pv>0;++owner) {
            auto *ship=owner==0 ? Spritej1 : Spritej2;
            if (ship->pv<=0) continue;
            const float diameter=sfKineticShipDiameter(ship),rockRadius=std::max(rock->w,rock->h)*.5f;
            const auto raw=sfKineticRockSolution(rock,ship,owner);
            const float travelled=sfKineticSegmentDistance(fromX,fromY,rock->x,rock->y,ship->x,ship->y);
            const float outer=diameter*raw.maxRadiusShipDiameters+rockRadius;
            const float inner=diameter*sfKineticInnerRadiusShipDiameters(raw)+rockRadius;
            bool interacted=false;
            if(rock->kineticStage==0 && raw.suggestedLayer==SfKineticLayer::Outer && travelled<=outer) {
                rock->kineticStage=1;
                if(sfKineticTryLayer(rock,ship,owner,SfKineticLayer::Outer,raw,&interacted)) continue;
                if(interacted) continue;
            }
            if(rock->pv<=0) continue;
            interacted=false;
            if(rock->kineticStage<2 && raw.selectedRange>0 && travelled<=inner) {
                rock->kineticStage=2;
                if(sfKineticTryLayer(rock,ship,owner,SfKineticLayer::Inner,raw,&interacted)) continue;
                if(interacted) continue;
            }
            if(rock->pv<=0) continue;
            const float hullRadius=std::max(ship->sw,ship->sh)*.52f+rockRadius;
            const bool hullHit=colee(ship,rock) || travelled<=hullRadius;
            if(!hullHit) continue;
            sfFieldCollisionSound=true;sfFieldImpact(rock,ship);
            sfKineticEmitRedDust(owner,3,diameter*.45f);
            SDL_Log("KINETIC_IMPACT owner=%d massFactor=%.5f relativeSpeed=%.3f impactSpeed=%.3f rawDamage=%.5f residualDamage=%.5f selectedRange=%d maxRadius=%.3f energyCost=0",
                owner,raw.massFactor,raw.relativeSpeed,raw.impactSpeed,raw.rawDamage,raw.rawDamage,
                raw.selectedRange,diameter*raw.maxRadiusShipDiameters);
            if (hurt) hurt(owner,raw.rawDamage);
            else ship->pv=std::max(0.0f,ship->pv-sfApplyShieldImpact(ship,raw.rawDamage));
            rock->pv=0;
        }
    }
    for(size_t i=0;i<rocks.size();++i) {
        auto *a=rocks[i];
        for(size_t j=i+1;j<rocks.size() && a->pv>0;++j) {
            auto *b=rocks[j];
            if (b->pv<=0 || a->timer || b->timer || !(inxy(a)||inxy(b)) || !colee(a,b)) continue;
            sfFieldCollisionSound=true;sfFieldImpact(a,b,true);
            const float h1=std::sqrt(a->w*a->h),h2=std::sqrt(b->w*b->h);
            if (std::min(h1,h2)>0 && std::max(h1,h2)/std::min(h1,h2)<1.05f) {
                for(int burst=0;burst<4 && sa1.size()+4<=200;++burst) eclats(a,b);
                sfKineticDestroyAsteroid(a,SfKineticDustCause::AsteroidCollision);
                sfKineticDestroyAsteroid(b,SfKineticDustCause::AsteroidCollision);
            } else {
                auto *large=h1>h2 ? a : b;auto *small=h1>h2 ? b : a;
                large->w+=small->w*.01f;large->h+=small->h*.01f;
                large->sw=large->w;large->sh=large->h;
                sfKineticDestroyAsteroid(small,SfKineticDustCause::AsteroidCollision);
            }
        }
    }
    if (!hurt) for(auto *list:{&entitiesj1,&entitiesj2}) for(auto *shot:*list) {
        if (shot->pv<=0) continue;
        for(auto *rock:rocks) if(rock->pv>0 && (colee(shot,rock)||sfShotCrosses(rock,shot))) {
            sfMineAsteroid(rock,shot);break;
        }
    }
    sfKineticSurgeMineAsteroids(1.0f/60.0f);
    for(auto i=sa1.begin();i!=sa1.end();) {
        if ((*i)->pv<=0) {delete *i;i=sa1.erase(i);} else ++i;
    }
    incra1=int(sa1.size());
    SpaceFortressPruneParticles(particules,1000);SpaceFortressPruneParticles(particulesr,1000);
    sfKineticUpdateEffects(1.0f/60.0f);
}

static void sfLegacyFieldFrame(float dt,void (*hurt)(int,float))
{
    const int oldW=W,oldH=H,oldWidth=WIDTH,oldHeight=HEIGHT;const float oldK=k0;
    W=WIDTH=std::max(1,int(sfArenaW));H=HEIGHT=std::max(1,int(sfArenaH));k0=1;
    sfFieldRemainder+=std::clamp(dt,0.0f,.1f);
    while(sfFieldRemainder+1e-6f>=1.0f/60) {
        sfFieldRemainder=std::max(0.0f,sfFieldRemainder-1.0f/60);sfLegacyFieldStep(hurt);
        if (hurt) {
            for(auto *p:particules) p->update();for(auto *p:particulesr) p->update();sfCollectDust();
            for(auto i=explos.begin();i!=explos.end();) {
                (*i)->upexpl();if((*i)->pv<=0) {delete *i;i=explos.erase(i);} else ++i;
            }
        }
    }
    W=oldW;H=oldH;WIDTH=oldWidth;HEIGHT=oldHeight;k0=oldK;
}
