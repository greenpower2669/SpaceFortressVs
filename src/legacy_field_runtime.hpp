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
static SfKineticSolution sfKineticRockSolution(const sprite *rock,const sprite *ship,int owner)
{
    const float dx=rock->x-ship->x,dy=rock->y-ship->y,d=std::max(1.0f,vlong(dx,dy));
    const float area=std::max(1.0f,rock->w*rock->h);
    const float refArea=sfKineticReferenceArea(sfArenaH);
    return sfResolveKinetic(refArea*.05f,sfKineticMassFactorFromArea(area,sfArenaH),
        {rock->vx*60.0f,rock->vy*60.0f},{sfObserved[owner].velocity.vx,sfObserved[owner].velocity.vy},
        dx/d,dy/d,sfKineticReferenceSpeed(sfArenaW));
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
static void sfKineticUpdateEffects(float dt)
{
    for(int owner=0;owner<2;++owner) {
        auto &pulse=sfKineticPulses[owner];pulse.phase+=dt*34.0f;
        pulse.outer=std::max(0.0f,pulse.outer-dt/0.42f);
        pulse.inner=std::max(0.0f,pulse.inner-dt/0.34f);
        const auto *ship=owner==0 ? Spritej1 : Spritej2;
        if(!ship || ship->pv<=0) continue;
        const float diameter=sfKineticShipDiameter(ship);
        for(auto *dust:particulesr) {
  if(!dust || dust->pv<=0) continue;
  const float dx=dust->x-ship->x,dy=dust->y-ship->y,d=std::max(1.0f,vlong(dx,dy));
  float strength=0;
  if(pulse.outer>0 && std::abs(d-diameter*SF_KINETIC_OUTER_RADIUS_DIAMETERS)<diameter*.10f) strength=pulse.outer;
  if(pulse.inner>0 && std::abs(d-diameter*SF_KINETIC_INNER_RADIUS_DIAMETERS)<diameter*.09f) strength=std::max(strength,pulse.inner);
  if(strength<=0) continue;
  const float wobble=std::sin(pulse.phase+d*.08f)*diameter*.018f*strength;
  dust->x+=(-dy/d)*wobble;dust->y+=(dx/d)*wobble;
        }
    }
}
static bool sfKineticFragmentRock(sprite *rock,const sprite *ship,int owner,SfKineticLayer layer,
                        const SfKineticSolution &solution)
{
    if(!rock || rock->pv<=0 || !ship) return false;
    const int count=layer==SfKineticLayer::Outer ? 6 : 4;
    const int nextStage=layer==SfKineticLayer::Outer ? 1 : 2;
    const float scale=1.0f/std::sqrt(float(count));
    const float smallest=sfArenaH/260.0f;
    if(std::max(rock->w,rock->h)*scale<smallest) {rock->pv=0;return true;}
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
        fragment->startup();sa1.push_back(fragment);
    }
    rock->pv=0;return true;
}
static bool sfKineticTryLayer(sprite *rock,sprite *ship,int owner,SfKineticLayer layer,
                    const SfKineticSolution &raw,float damageMultiplier)
{
    const auto solved=sfApplyKineticLayer(raw,layer,sfKineticEnergyFraction(ship->nrj),damageMultiplier);
    if(solved.dissipationFraction<=.001f) return false;
    sfAddShipHeat(ship,solved.heatCost);
    sfKineticTriggerPulse(owner,layer,std::clamp(.35f+solved.dissipationFraction,0.0f,1.0f));
    const float diameter=sfKineticShipDiameter(ship);
    sfKineticEmitRedDust(owner,layer==SfKineticLayer::Outer ? 7 : 4,
        diameter*(layer==SfKineticLayer::Outer ? SF_KINETIC_OUTER_RADIUS_DIAMETERS : SF_KINETIC_INNER_RADIUS_DIAMETERS));
    if(sfKineticFragmentRock(rock,ship,owner,layer,solved)) return true;
    // Population cap fallback: preserve the object but shed kinetic speed and
    // turn it sideways rather than deleting matter or tunnelling through.
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
  const float outer=diameter*SF_KINETIC_OUTER_RADIUS_DIAMETERS+rockRadius;
  const float inner=diameter*SF_KINETIC_INNER_RADIUS_DIAMETERS+rockRadius;
  const auto raw=sfKineticRockSolution(rock,ship,owner);
  const float multiplier=hurt ? SF_KINETIC_COOP_DAMAGE_MULTIPLIER : 1.0f;
  if(rock->kineticStage==0 && raw.suggestedLayer==SfKineticLayer::Outer &&
     sfKineticSegmentDistance(fromX,fromY,rock->x,rock->y,ship->x,ship->y)<=outer) {
      rock->kineticStage=1;
      if(sfKineticTryLayer(rock,ship,owner,SfKineticLayer::Outer,raw,multiplier)) continue;
  }
  if(rock->pv<=0) continue;
  if(rock->kineticStage<2 && raw.suggestedLayer!=SfKineticLayer::None &&
     sfKineticSegmentDistance(fromX,fromY,rock->x,rock->y,ship->x,ship->y)<=inner) {
      rock->kineticStage=2;
      if(sfKineticTryLayer(rock,ship,owner,SfKineticLayer::Inner,raw,multiplier)) continue;
  }
  if(rock->pv<=0) continue;
  const float hullRadius=std::max(ship->sw,ship->sh)*.52f+rockRadius;
  const bool hullHit=colee(ship,rock) ||
      sfKineticSegmentDistance(fromX,fromY,rock->x,rock->y,ship->x,ship->y)<=hullRadius;
  if(!hullHit) continue;
  sfFieldCollisionSound=true;sfFieldImpact(rock,ship);
  sfKineticTriggerPulse(owner,SfKineticLayer::Inner,.42f);
  sfKineticEmitRedDust(owner,3,diameter*.62f);
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
  partsforiw(a,b);sfFieldCollisionSound=true;sfFieldImpact(a,b,true);
  const float h1=std::sqrt(a->w*a->h),h2=std::sqrt(b->w*b->h);
  if (std::min(h1,h2)>0 && std::max(h1,h2)/std::min(h1,h2)<1.05f) {
      for(int burst=0;burst<4 && sa1.size()+4<=200;++burst) eclats(a,b);
      a->pv=b->pv=0;
  } else {
      auto *large=h1>h2 ? a : b;auto *small=h1>h2 ? b : a;
      large->w+=small->w*.01f;large->h+=small->h*.01f;
      large->sw=large->w;large->sh=large->h;small->pv=0;
  }
        }
    }
    if (!hurt) for(auto *list:{&entitiesj1,&entitiesj2}) for(auto *shot:*list) {
        if (shot->pv<=0) continue;
        for(auto *rock:rocks) if(rock->pv>0 && (colee(shot,rock)||sfShotCrosses(rock,shot))) {
  sfMineAsteroid(rock,shot);break;
        }
    }
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
