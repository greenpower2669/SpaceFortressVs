#include "../src/solo_canonical_adapter.hpp"
#include <cassert>
#include <cmath>
struct FakeShip {
 sfsolo::CanonicalSnapshot ship;
 sfsolo::CanonicalInput last;
 int ticks=0,collisions=0;
 float impact=0;
};
static void tick(void *p,const sfsolo::CanonicalInput &input){
 auto &f=*static_cast<FakeShip*>(p);
 f.last=input;++f.ticks;
 f.ship.x+=input.x*input.dt;
}
static sfsolo::CanonicalSnapshot snapshot(void *p){
 return static_cast<FakeShip*>(p)->ship;
}
static void collision(void *p,float amount){
 auto &f=*static_cast<FakeShip*>(p);
 ++f.collisions;f.impact+=amount;
}
int main(){
 using namespace sfsolo;
 Session session(generate({}));
 assert(session.start());
 FakeShip fake;
 fake.ship.x=session.pilot.x;
 fake.ship.y=session.pilot.y;
 CanonicalAdapter adapter{{&fake,tick,snapshot,collision}};
 assert(adapter.available());
 adapter.step(session,{5.0f,0.0f,1.0f,true,false});
 assert(fake.ticks==1);
 assert(std::abs(fake.last.x-1.0f)<.001f);
 assert(std::abs(fake.last.dt-.05f)<.001f);
 assert(fake.last.fire);
 assert(std::abs(session.pilot.x-fake.ship.x)<.001f);
 const int x=int(session.pilot.x),y=int(session.pilot.y);
 session.map.tiles[size_t(y)*session.map.width+x]=Tile::Asteroid;
 adapter.step(session,{0,0,.01f,false,false});
 assert(fake.collisions==1 && fake.impact==5);
 adapter.step(session,{0,0,.01f,false,false});
 assert(fake.collisions==1);
}
