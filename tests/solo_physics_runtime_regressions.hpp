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
    // Same physical pilot in the boss arena: bounded acceleration, reversal, coast.
    sfCoop.bossPresent=false;Spritej2->x=sfArenaW*.4f;Spritej2->y=sfArenaH*.6f;
    c.down=true;c.target=tupl(sfArenaW,Spritej2->y);sfCoopMovePlayers(.05f);
    assert(c.velocity.vx>0 && c.velocity.vx<sfArenaW*.3f);
    const float arenaX=Spritej2->x,arenaV=c.velocity.vx;c.down=false;sfCoopMovePlayers(.01f);
    assert(Spritej2->x>arenaX && c.velocity.vx>0 && c.velocity.vx<arenaV);
    c.down=true;c.target.x=0;sfCoopMovePlayers(.005f);assert(c.velocity.vx>0);
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
    // Crossing an entire deposit in one 60 Hz step still stops at contact.
    a=sfTestDeposit(1000,3000,30);b=sfTestDeposit(1200,3000,30);a->vx=500;
    sfSoloRockMove(a);assert(a->x<=1140.1f && (b->vx>0 || b->pv<=0));
    for(auto *r:sa1)if(r->pv>0)assert(world.canOccupy({r->x,r->y},sfSoloRockRadius(r)));
    sfFixResetAsteroidField();
    // A diagonal wall slide endpoint is not a safe chord for rock/object CCD.
    std::fill(world.map.tiles.begin(),world.map.tiles.end(),sfsolo::Tile::Empty);
    const float cell=world.pixelsPerCell;
    world.map.tiles[10*world.map.width+10]=sfsolo::Tile::Rock;
    a=sfTestDeposit(9.5f*cell,9.5f*cell,10);b=sfTestDeposit(11*cell+40,10.5f*cell,30);
    a->vx=a->vy=2*cell;sfSoloRockMove(a);
    assert(a->pv<=0 || world.canOccupy({a->x,a->y},10));
    for(auto *r:sa1)if(r->pv>0)assert(world.canOccupy({r->x,r->y},sfSoloRockRadius(r)));
    sfFixResetAsteroidField();
    // In open space retain six Outer debris and the actual shield solution.
    std::fill(world.map.tiles.begin(),world.map.tiles.end(),sfsolo::Tile::Empty);
    float speed[2]{};
    for(int strong=0;strong<2;++strong){
        a=sfTestDeposit(2000,4000,60);a->vx=5;
        SfKineticSolution solution{};solution.rawDamage=100;solution.residualDamage=strong?10:80;
        solution.relativeSpeed=300;solution.dissipationFraction=strong?.9f:.2f;
        sfObserved[1].velocity.vx=sfObserved[1].velocity.vy=0;
        assert(sfKineticFragmentRock(a,Spritej2,1,SfKineticLayer::Outer,solution));
        int children=0;for(auto *r:sa1)if(r->pv>0){
            ++children;assert(r->kineticStage==1 && world.canOccupy({r->x,r->y},sfSoloRockRadius(r)));
            speed[strong]=std::max(speed[strong],std::hypot(r->vx,r->vy));
        }
        assert(children==6);sfFixResetAsteroidField();
    }
    assert(speed[1]<speed[0]*.5f);
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
static void testSoloHuntersNavigateRealMaze(){
    for(int danger=0;danger<9;++danger){
        sfsolo::CoopSoloWorldBridge world;sfPhysicsWorld(world);
        sfsolo::MapOptions options;assert(world.create(options,{0,0,780,1680},780));
        sfsolo::LevelFlow level;assert(level.create(world,sfCoopSoloPickupHull()));
        const auto start=world.start();sfsolo::CoopWorldPoint goal{};bool detour=false;
        for(const auto at:level.minibossAnchors){
            if(std::hypot(at.x-level.lair.x,at.y-level.lair.y)<world.combatWidth*2)continue;
            if(sfsolo::mazeRoute(world,start,at,25).size()>2){goal=at;detour=true;break;}
        }
        assert(detour);
        sfBossDangerIndex=danger;
        Spritej2->x=level.lair.x;Spritej2->y=level.lair.y; // cannot reveal a new position through walls
        SfSoloMiniboss m;m.position=m.anchor=tupl(start.x,start.y);m.lastSeen=tupl(goal.x,goal.y);
        m.radius=25;m.role=1;m.memory=2+5.f*danger/8;m.alerted=true;sfCoop.soloMinibosses.push_back(m);
        float travel=0;tupl previous=m.position;
        for(int frame=0;frame<180;++frame){
            sfSoloMinibossTick(1.f/60);const auto &hunter=sfCoop.soloMinibosses.front();
            assert(world.canOccupy({hunter.position.x,hunter.position.y},hunter.radius));
            assert(world.firstWallImpact({previous.x,previous.y},{hunter.position.x,hunter.position.y},hunter.radius)>=1);
            assert(hunter.lastSeen.x==goal.x && hunter.lastSeen.y==goal.y);
            travel+=vlong(hunter.position.x-previous.x,hunter.position.y-previous.y);previous=hunter.position;
        }
        assert(travel>100); // actual traversal, not just a planned BFS path
    }
}
static void testSoloPhysics017(){
    testSoloPhysicalShipAndCamera();testSoloStableDepositsAndSound();testSoloPackPerceptionAndSearch();testSoloHuntersNavigateRealMaze();
    setupCampaign();puts("PASS: SOLO-017 real ship/walls/camera, resting rocks/mining/audio and bounded pack search");
}
