#pragma once
static void testNoHumanAutofire()
{
    setupCampaign();
    for(int i=0;i<120;++i) sfCoopMovePlayers(1.0f/60);
    assert(sfCoop.shots.empty());
    auto a=sfCoopFinger(SDL_FINGERDOWN,1001,.3f,.2f);
    auto b=sfCoopFinger(SDL_FINGERDOWN,1002,.7f,.8f);
    sfFixHandleEvent(&a);sfFixHandleEvent(&b);
    for(int i=0;i<20;++i) sfCoopMovePlayers(1.0f/60);
    assert(sfCoop.shots.empty());
    a=sfCoopFinger(SDL_FINGERDOWN,1003,.4f,.2f);
    b=sfCoopFinger(SDL_FINGERDOWN,1004,.6f,.8f);
    sfFixHandleEvent(&a);sfFixHandleEvent(&b);
    assert(sfCoop.shots.size()==2);
    assert(sfCoop.shots[0].kind==4 && sfCoop.shots[1].kind==4);
    assert(Spritej1->nrj==11 && Spritej2->nrj==11);
    for(int i=0;i<120;++i) sfCoopMovePlayers(1.0f/60);
    assert(sfCoop.shots.size()==2); // Holding or moving the firing finger never repeats.
    a=sfCoopFinger(SDL_FINGERMOTION,1003,.8f,.8f);sfFixHandleEvent(&a);
    assert(sfCoop.controls[0].finger==1001 && sfCoop.controls[1].finger==1002);
    a=sfCoopFinger(SDL_FINGERUP,1003,.8f,.8f);sfFixHandleEvent(&a);
    a=sfCoopFinger(SDL_FINGERDOWN,1003,.4f,.2f);sfFixHandleEvent(&a);
    assert(sfCoop.shots.size()==3 && sfCoop.shots.back().kind==0);
    assert(std::abs(sfCoop.shots.back().velocity.vx)<.001f);
    SDL_Event pause{};pause.type=SDL_APP_DIDENTERBACKGROUND;sfFixHandleEvent(&pause);
    assert(!sfCoop.controls[0].down && !sfCoop.controls[1].down);
    std::puts("PASS: two pilots/four fingers, tap-only shots, full-energy missiles and pause cancellation");
}
static void testPassiveCoopTurrets()
{
    setupCampaign();
    for(int i=0;i<180;++i) sfCoopDefences(1.0f/60);
    assert(sfCoop.shots.empty());
    for(const auto &t:sfTurrets) assert(t.deploy==0);
    std::puts("PASS: cooperative turrets remain inactive without a collected bonus");
}

static void testCollectedBonusAndShield()
{
    setupCampaign();sfCoop.bonusTimer=0;sfCoopBonus(.01f);
    assert(sfCoop.bonusLife>0 && sfCoop.turretTime==0);
    sfCoop.bonusPosition=tupl(Spritej2->x,Spritej2->y);sfCoop.bonusVelocity.set(0,0);
    sfCoopBonus(.01f);assert(sfCoop.bonusLife==0 && sfCoop.turretTime==14);
    for(int i=0;i<180;++i) sfCoopDefences(1.0f/60);
    assert(!sfCoop.shots.empty() && Spritej1->nrj==0 && Spritej2->nrj==0);
    assert(sfTurrets[0].energy<100);
    sfCoop.shots.clear();sfCoop.turretTime=.001f;sfCoopDefences(.01f);
    for(int i=0;i<180;++i) sfCoopDefences(1.0f/60);
    assert(sfCoop.shots.empty());
    for(auto *ship:{Spritej1,Spritej2}) ship->pv=1000;
    Spritej1->nrj=0;Spritej2->nrj=50;
    sfCoopHurt(0,100);sfCoopHurt(1,100);
    assert(Spritej1->pv==1000 && Spritej2->pv==900);
    assert(Spritej1->nrj>0 && Spritej2->nrj==50);
    std::puts("PASS: random floating bonus, collection, independent turret energy, expiration and reserve-dependent shield");
}

static void testRealCoopField()
{
    setupCampaign();
    sfCoopTick(1.0f/60);
    assert(!sa1.empty());
    const auto *rock=sa1.back();const auto x=rock->x,y=rock->y;
    sfCoopTick(1.0f/60);
    assert(rock->x!=x || rock->y!=y);
    std::puts("PASS: cooperative tick advances the actual historical asteroid list");
}
