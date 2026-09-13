#pragma once

static void testEnergyFeedback()
{
    // Reproduce the screenshot's runaway HUD: at heat 200 the historical blue
    // marker fell several icon heights below the gun. Neither team may escape
    // its two anchors, regardless of damage or the icon's animated dimensions.
    for (const auto screen : {SDL_Point{709,1536},SDL_Point{720,1560},SDL_Point{640,360}}) {
        const int worldW=screen.x*1.089f, worldH=screen.y*1.0769f;
        const int size=worldH/18;
        for (int owner=0;owner<2;++owner) {
            const int x=int(worldW*(owner==0 ? .05f : .75f));
            SDL_Rect gun{x,int(worldH/(owner==0 ? 2.4f : 1.9f)),size,size};
            SDL_Rect full{x,int(worldH/(owner==0 ? 2.2f : 2.1f)),size+4,size-2};
            const float fullCentre=full.y+full.h*.5f, gunCentre=gun.y+gun.h*.5f;
            for (float heat : {-20.0f,0.0f,12.5f,25.0f,50.0f,200.0f,
                               std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()}) {
                const auto marker=sfEnergyMarkerRect(heat,full,gun);
                const float centre=marker.y+marker.h*.5f;
                assert(centre>=std::min(fullCentre,gunCentre)-1 && centre<=std::max(fullCentre,gunCentre)+1);
                assert(marker.x>=0 && marker.x+marker.w<screen.x && marker.y+marker.h<screen.y);
                if (heat<=0) assert(marker.x==full.x && marker.y==full.y);
                if (heat>=50) assert(std::abs(centre-gunCentre)<=.5f);
            }
        }
    }
    setupTactics(); Spritej1->nrj=Spritej2->nrj=0;
    sprite missile; missile.name="miss"; missile.vy=2000;
    assert(sfShotHeat(&missile)==8);
    for (int hit=0;hit<40;++hit) {
        sfAddShipHeat(Spritej2,sfShotHeat(&missile));
        assert(Spritej2->nrj<=50 && Spritej1->nrj==0);
    }
    assert(Spritej2->nrj==50);
    Spritej2->update(); assert(Spritej2->nrj<50);
    sfAddShipHeat(Spritej2,1000); assert(Spritej2->nrj==50);
    sfAddShipHeat(Spritej2,-1000); assert(Spritej2->nrj==0);
    for (auto *ship : {Spritej1,Spritej2}) { ship->nrj=200; ship->pv=0; }
    sfFixResetMatchState();
    assert(Spritej1->nrj==0 && Spritej2->nrj==0 && Spritej1->pv==1000 && Spritej2->pv==1000);
    std::puts("PASS: full/empty/overloaded energy HUD stays at shield/gun anchors, bounded impact heat and full reserve after restart");
}

static void testProjectileFeedback()
{
    float referenceTravel=0;
    for (int owner=0;owner<2;++owner) for (int fps : {30,60,120}) for (float legacyScale : {.5f,1.0f,2.0f}) {
        setupTactics(); setia=false; sfFrameDt=1.0f/fps; k0=legacyScale;
        sprite *ship=owner==0 ? Spritej1 : Spritej2;
        ship->nrj=0; assert(sfFireMain(owner));
        auto *shot=(owner==0 ? entitiesj2 : entitiesj1).front();
        assert(shot->name=="miss" && shot->shotOwner==owner);
        const float start=shot->y;
        for (int frame=0;frame<fps/2;++frame) {
            sfAdvanceProjectile(shot);
            const auto velocity=sfProjectileVelocity(shot);
            assert(velocity.vy*(owner==0 ? 1 : -1)>0);
            assert(vlong(velocity.vx,velocity.vy)<=sfArenaW*1.5f+.01f);
            assert(sfShotHeat(shot)==8);
            assert(std::abs(shot->xm+shot->w*.5f-shot->x)<.001f);
            assert(std::abs(shot->ym+shot->h*.5f-shot->y)<.001f);
            const auto rect=sfProjectileRect(shot);
            assert(std::abs(rect.x+rect.w*.5f-shot->x)<=.501f);
            assert(std::abs(rect.y+rect.h*.5f-shot->y)<=.501f);
        }
        const float travel=std::abs(shot->y-start);
        if (referenceTravel==0) referenceTravel=travel;
        assert(std::abs(travel-referenceTravel)<1);
        // Both directions must detect a crossing between frames.
        shot->setxywh(390,840,8,8); shot->shotVelocityX=0;
        sfFrameDt=.1f; sfAdvanceProjectile(shot);
        sprite target; target.setxywh(390,(shot->y+shot->shotFromY)*.5f,8,8);
        assert(std::abs(target.y-shot->y)>20 && sfShotCrosses(&target,shot));
        // Expired shots cannot hit again.
        shot->pv=0; assert(!sfShotCrosses(&target,shot));
    }

    // Horizontal/vertical defensive impacts have the same energy cost.
    setupTactics(); auto *shot=sfMakeShot(0,true);
    shot->shotVelocityX=1170; shot->shotVelocityY=0;
    const float heat=sfShotHeat(shot);
    shot->shotVelocityX=0; shot->shotVelocityY=1170;
    assert(heat==sfShotHeat(shot) && heat==2);

    // Draw the real assets repeatedly. Rendering cannot steer, accelerate,
    // spend energy, move collision bounds or emit collectible particles.
    setupTactics(); Spritej2->nrj=0; assert(sfFireMain(1));
    shot=entitiesj1.front(); sfAdvanceProjectile(shot);
    const float x=shot->x,y=shot->y,vx=shot->vx,vy=shot->vy,age=shot->shotAge;
    const float reserve=Spritej2->nrj;
    auto *surface=SDL_CreateRGBSurfaceWithFormat(0,780,1680,32,SDL_PIXELFORMAT_RGBA32);
    auto *renderer=SDL_CreateSoftwareRenderer(surface); assert(renderer);
    auto *orb=IMG_LoadTexture(renderer,IMG_PATHtj1);
    auto *missile=IMG_LoadTexture(renderer,IMG_PATHmiss); assert(orb && missile);
    for (int draw=0;draw<8;++draw) sfDrawProjectiles(renderer,entitiesj1,orb,missile);
    assert(shot->x==x && shot->y==y && shot->vx==vx && shot->vy==vy && shot->shotAge==age);
    assert(Spritej2->nrj==reserve && particules.empty() && particulesr.empty());
    SDL_RenderFlush(renderer);
    const Uint32 pixel=static_cast<Uint32*>(surface->pixels)[int(shot->y)*surface->w+int(shot->x)];
    assert(pixel!=0);
    SDL_DestroyRenderer(renderer); SDL_FreeSurface(surface);
    std::puts("PASS: mirrored bounded missiles at 30/60/120fps, current render/collision positions, swept hits and side-effect-free rendering");
}

static void writeFeedbackPreview(const char *path)
{
    if (!path) return;
    setupTactics(); setia=false; sfArenaW=709; sfArenaH=1536;
    const int worldW=772,worldH=1654,size=worldH/18;
    auto *surface=SDL_CreateRGBSurfaceWithFormat(0,709,1536,32,SDL_PIXELFORMAT_RGBA32);
    auto *renderer=SDL_CreateSoftwareRenderer(surface); assert(renderer);
    auto draw=[&](const char *asset,SDL_Rect rect) {
        auto *texture=IMG_LoadTexture(renderer,asset); assert(texture);
        SDL_RenderCopy(renderer,texture,nullptr,&rect);
    };
    draw("resources/assets/pict/fond4hlz.png",{0,0,worldW,worldH});
    draw("resources/assets/pict/suno.png",{-240,-110,800,800});
    draw("resources/assets/pict/jupsoeur4.png",{850,850,1680,1680});
    draw("resources/assets/pict/remaster/player_orange.png",{310,260,190,190});
    draw("resources/assets/pict/remaster/player_blue.png",{415,1190,190,190});
    for (int owner=0;owner<2;++owner) {
        const int x=int(worldW*(owner==0 ? .05f : .75f));
        SDL_Rect gun{x,int(worldH/(owner==0 ? 2.4f : 1.9f)),size,size};
        SDL_Rect full{x,int(worldH/(owner==0 ? 2.2f : 2.1f)),size,size};
        draw(owner==0 ? IMG_PATHgunrl : IMG_PATHgunbl,gun);
        draw(owner==0 ? "resources/assets/pict/remaster/shield_red.png" :
                        "resources/assets/pict/remaster/shield_blue.png",full);
        const float heat=std::getenv("SPACEFORTRESS_HUD_EMPTY") ? 200 : 0;
        draw("resources/assets/pict/ecl.png",sfEnergyMarkerRect(heat,full,gun));
    }
    Spritej1->setxywh(280,560,100,100); Spritej2->setxywh(460,1100,100,100);
    for (int owner=0;owner<2;++owner) {
        (owner==0 ? Spritej1 : Spritej2)->nrj=0; assert(sfFireMain(owner));
    }
    for (int i=0;i<8;++i) {
        sfAdvanceProjectile(entitiesj1.front()); sfAdvanceProjectile(entitiesj2.front());
    }
    auto *orb=IMG_LoadTexture(renderer,IMG_PATHtj1);
    auto *missile=IMG_LoadTexture(renderer,IMG_PATHmiss);
    sfDrawProjectiles(renderer,entitiesj1,orb,missile);
    sfDrawProjectiles(renderer,entitiesj2,orb,missile);
    SDL_RenderFlush(renderer); assert(IMG_SavePNG(surface,path)==0);
    SDL_DestroyRenderer(renderer); SDL_FreeSurface(surface);
}
