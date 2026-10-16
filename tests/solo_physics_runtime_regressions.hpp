#pragma once
#include "solo_maze_path.hpp"
static void sfPhysicsWorld(sfsolo::CoopSoloWorldBridge &world){
    sfBonusTestSetup(false);sfBonusTestWorld(world);sfCoop.soloGame=true;
    sfCoop.soloWorldResolve=+[](void *c,tupl from,tupl to,float r){
        const auto p=static_cast<sfsolo::CoopSoloWorldBridge*>(c)->moveWithoutTunnelling({from.x,from.y},{to.x,to.y},r);return tupl(p.x,p.y);
    };
    sfCoop.soloEventVisible=+[](void *c,tupl p){return static_cast<sfsolo::CoopSoloWorldBridge*>(c)->directView({Spritej2->x,Spritej2->y},{p.x,p.y});};
    sfCoop.soloRoute=+[](void *c,tupl a,tupl b,float r,int role,std::vector<tupl>&out){
        out.clear();for(auto p:sfsolo::mazeRoute(*static_cast<sfsolo::CoopSoloWorldBridge*>(c),{a.x,a.y},{b.x,b.y},r,role))out.emplace_back(p.x,p.y);
    };
    world.centerOn({Spritej2->x,Spritej2->y});
}
static sprite *sfTestDeposit(float x,float y,float radius){
    auto *r=new sprite;r->setxywh(x,y,radius*2,radius*2);r->sw=r->w;r->sh=r->h;
    r->pv=1;r->vx=r->vy=0;r->as=0;r->timer=r->tout=r->rand0=0;r->name="a1";r->startup();sa1.push_back(r);return r;
}
static void testSoloPhysicalShipAndCamera(){
    sfsolo::CoopSoloWorldBridge world;sfPhysicsWorld(world);sfBossDangerIndex=8;
    auto &c=sfCoop.controls[1];c.down=true;c.target=tupl(10000,1300);
    for(int i=0;i<100;++i)sfCoopMovePlayers(.01f);
    const float x=Spritej2->x,v=c.velocity.vx;c.down=false;
    sfCoopMovePlayers(.05f);assert(Spritej2->x>x && c.velocity.vx>0 && c.velocity.vx<v);
    world.followPhysicalShip({Spritej2->x,Spritej2->y},{Spritej2->x-x,0},.05f);
    auto screen=world.toScreen({Spritej2->x,Spritej2->y});assert(screen.x>0 && screen.x<world.screen.w);
    const float cell=world.pixelsPerCell;const int wall=int(Spritej2->x/cell)+3;
    for(int y=0;y<world.map.height;++y)world.map.tiles[y*world.map.width+wall]=sfsolo::Tile::Rock;
    c.down=true;c.target=tupl(100000,1300);c.velocity.vx=100000;
    sfCoopMovePlayers(.1f);assert(Spritej2->x<wall*cell && c.velocity.vx==0);
    assert(world.canOccupy({Spritej2->x,Spritej2->y},sfCoopSoloPickupHull()));
    sfCoopSoloEnterFinalArena();assert(c.velocity.vx==0 && c.velocity.vy==0 && !c.down);
}
static void testSoloStableDepositsAndSound(){
    sfsolo::CoopSoloWorldBridge world;sfPhysicsWorld(world);
    auto *a=sfTestDeposit(1000,3000,30),*b=sfTestDeposit(1059,3000,30);
    SfSoloFieldScope scope({true,true,&world,sfCoop.soloWorldResolve,sfCoop.soloWorldCanOccupy,sfCoop.soloWorldFirstImpact,sfCoop.soloEventVisible});
    for(int i=0;i<600;++i)sfLegacyFieldStep(sfCoopAsteroidHurt,2,&sfCoop.soloKineticAxes,world.worldWidth(),world.worldHeight());
    assert(sa1.size()==2 && a->x==1000 && b->x==1059 && a->vx==0 && b->vx==0);
    sfFieldCollisionSound=false;a->vx=1;sfSoloRockPair(a,b);assert(b->vx>0 && !sfFieldCollisionSound);
    assert(a->pv>0 && b->pv>0); // low energy: no spontaneous destruction
    const float size=a->w;sprite shot;shot.setxywh(a->x,a->y,8,8);shot.pv=1;
    sfFieldMiningSound=false;sfMineAsteroid(a,&shot);assert(a->w<size && !particules.empty() && !sfFieldMiningSound);
    // Visible event, then same event occluded by one opaque wall.
    Spritej2->x=1000;Spritej2->y=2500;world.centerOn({1000,2500});
    assert(sfFieldAudible(1000,2700));
    const int row=int(2600/world.pixelsPerCell);
    for(int x=0;x<world.map.width;++x)world.map.tiles[row*world.map.width+x]=sfsolo::Tile::Rock;
    assert(!sfFieldAudible(1000,2700));assert(!sfFieldAudible(1000,10000));
    sfCoop.soundHit=false;sfCoop.invulnerable[1]=0;sfCoopAsteroidHurt(1,10);assert(sfCoop.soundHit);
    // Mobile asteroid sweeps and rebounds, not rock->ast's arena boundary.
    a->x=1000;a->y=row*world.pixelsPerCell-sfSoloRockRadius(a)-2;a->vx=0;a->vy=5;
    sfSoloRockMove(a);assert(a->vy<0 && world.canOccupy({a->x,a->y},sfSoloRockRadius(a)));
    const auto x=a->x,y=a->y;a->vx=a->vy=0;for(int i=0;i<100;++i)sfSoloRockMove(a);assert(a->x==x && a->y==y);
    sfFixResetAsteroidField();
}
static void testSoloPackPerceptionAndSearch(){
    sfsolo::CoopSoloWorldBridge world;sfPhysicsWorld(world);sfBossDangerIndex=8;
    Spritej2->x=600;Spritej2->y=1500;
    for(int i=0;i<3;++i){SfSoloMiniboss m;m.position=m.anchor=tupl(300+i*100,1400);m.radius=25;m.role=i+1;m.pack=1;sfCoop.soloMinibosses.push_back(m);}
    sfSoloMinibossTick(.02f);for(const auto&m:sfCoop.soloMinibosses)assert(m.alerted && m.memory>0);
    Spritej2->x=6000;Spritej2->y=10000;
    for(int i=0;i<100;++i)sfSoloMinibossTick(.02f);
    for(const auto&m:sfCoop.soloMinibosses){assert(m.lastSeen.x==600 && m.lastSeen.y==1500);assert(world.canOccupy({m.position.x,m.position.y},m.radius));}
    for(int i=0;i<600;++i)sfSoloMinibossTick(.02f);
    for(const auto&m:sfCoop.soloMinibosses)assert(!m.alerted && !m.searching && m.memory==0);
    for(auto &m:sfCoop.soloMinibosses)sfSoloMinibossDamage(m,1000);
    assert(sfCoop.soloBonusPending.empty() && sfCoop.soloBonus.pickups.empty());
}
static void testSoloPhysics017(){
    testSoloPhysicalShipAndCamera();testSoloStableDepositsAndSound();testSoloPackPerceptionAndSearch();
    setupCampaign();puts("PASS: SOLO-017 real ship/walls/camera, resting rocks/mining/audio and bounded pack search");
}
