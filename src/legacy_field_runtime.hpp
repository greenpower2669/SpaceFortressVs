#pragma once

// One bounded, fixed-step owner for Fab's sprite field. The original ast(),
// collisions, fragmentation and ore emitters are used in BOTH game modes.
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

static void sfLegacyFieldStep(void (*hurt)(int,float))
{
    if (sa1.empty()) {setastswall();setasts(2);}
    else if (sa1.size()<8) setasts(2);
    // Snapshot: children join the next tick. Never rescan a growing list N
    // times or use the population counter as an object's unique identity.
    std::vector<sprite*> rocks(sa1.begin(),sa1.end());
    for (auto *rock:rocks) {
        if (rock->pv<=0) continue;
        rock->ast();
        rock->tout=inxy(rock) ? 0 : rock->tout+1;
        if (rock->tout>60*30) rock->pv=0;
        for (int owner=0;owner<2 && rock->pv>0;++owner) {
            auto *ship=owner==0 ? Spritej1 : Spritej2;
            if (ship->pv<=0 || !colee(ship,rock)) continue;
            partsforired(rock,ship);
            sfFieldCollisionSound=true;
            sfFieldImpact(rock,ship);
            const float area=rock->w*rock->h;
            if (hurt) hurt(owner,area*.05f);
            else {
                ship->pv=std::max(0.0f,ship->pv-ship->nrj*ship->nrj*area*.00002f);
                sfAddShipHeat(ship,area*.0001f);
            }
            rock->pv=0;
        }
    }
    for(size_t i=0;i<rocks.size();++i) {
        auto *a=rocks[i];
        for(size_t j=i+1;j<rocks.size() && a->pv>0;++j) {
            auto *b=rocks[j];
            if (b->pv<=0 || a->timer || b->timer || !(inxy(a)||inxy(b)) || !colee(a,b)) continue;
            partsforiw(a,b);
            sfFieldCollisionSound=true;
            sfFieldImpact(a,b,true);
            const float h1=std::sqrt(a->w*a->h),h2=std::sqrt(b->w*b->h);
            if (std::min(h1,h2)>0 && std::max(h1,h2)/std::min(h1,h2)<1.05f) {
                // Historical four bursts, bounded so frame rate cannot shrink
                // the population limit or kill only off-screen arrivals.
                for(int burst=0;burst<4 && sa1.size()+4<=200;++burst) eclats(a,b);
                a->pv=b->pv=0;
            } else {
                auto *large=h1>h2 ? a : b;auto *small=h1>h2 ? b : a;
                large->w+=small->w*.01f;large->h+=small->h*.01f;
                large->sw=large->w;large->sh=large->h;small->pv=0;
            }
        }
    }
    // Ordinary duel guns/turrets retain their historical sprite projectiles.
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
    SpaceFortressPruneParticles(particules,1000);
    SpaceFortressPruneParticles(particulesr,1000);
}

static void sfLegacyFieldFrame(float dt,void (*hurt)(int,float))
{
    const int oldW=W,oldH=H,oldWidth=WIDTH,oldHeight=HEIGHT;
    const float oldK=k0;
    W=WIDTH=std::max(1,int(sfArenaW));H=HEIGHT=std::max(1,int(sfArenaH));k0=1;
    sfFieldRemainder+=std::clamp(dt,0.0f,.1f);
    while(sfFieldRemainder+1e-6f>=1.0f/60) {
        sfFieldRemainder=std::max(0.0f,sfFieldRemainder-1.0f/60);
        sfLegacyFieldStep(hurt);
        if (hurt) { // Duel's renderer already owns its particle animation.
            for(auto *p:particules) p->update();
            for(auto *p:particulesr) p->update();
            sfCollectDust();
            for(auto i=explos.begin();i!=explos.end();) {
                (*i)->upexpl();
                if((*i)->pv<=0) {delete *i;i=explos.erase(i);} else ++i;
            }
        }
    }
    W=oldW;H=oldH;WIDTH=oldWidth;HEIGHT=oldHeight;k0=oldK;
}
