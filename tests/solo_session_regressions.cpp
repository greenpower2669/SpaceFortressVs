#include "../src/solo_session.hpp"
#include <cassert>
#include <cmath>
int main(){
 using namespace sfsolo;
 auto m=generate({});
 Session s(m);
 assert(s.start());assert(s.phase==Phase::Flying);
 assert(s.pilot.y>m.height/2);
 assert(s.pilot.health==100);
 s.damage(20);assert(s.pilot.health==80 && s.pilot.damageTaken==20);
 s.repair(15);assert(s.pilot.health==95 && s.pilot.damageTaken==20);
 s.step(0,-1,.05f);assert(s.seconds>0);
 auto p=miniMapPosition(s);assert(p.x>=0 && p.x<=1 && p.y>=0 && p.y<=1);
 s.pilot.x=m.width/2+.5f;s.pilot.y=9.5f;s.step(0,0,.01f);
 assert(s.phase==Phase::BossFight);
 s.defeatBoss();assert(s.bossDefeated && s.phase==Phase::Flying);
 s.pilot.y=2.5f;s.step(0,0,.01f);
 assert(s.phase!=Phase::Won); // finish locked until the maze relays are found
 assert(s.relaysNeeded==3);
 s.relaysCollected=s.relaysNeeded;s.step(0,0,.01f);
 assert(s.phase==Phase::Won);
 // Ordinary dust is visible and collectible; only ice applies the malus.
 Session particles(m);assert(particles.start());
 particles.scatterMining(particles.pilot.x+4,particles.pilot.y);
 assert(particles.dust.size()==8);
 for(const auto &p:particles.dust)assert(!p.icy);
 particles.advanceDust(.02f);
 assert(!particles.iceActive());
 particles.scatterIce(particles.pilot.x+4,particles.pilot.y);
 assert(particles.dust.size()==16);
 unsigned frozen=0;
 for(const auto &p:particles.dust)frozen+=p.icy?1u:0u;
 assert(frozen==8);
 for(int i=0;i<30;++i)
     particles.scatterMining(particles.pilot.x+4,particles.pilot.y);
 assert(particles.dust.size()<=128);
 Session collected(m);assert(collected.start());
 collected.scatterIce(collected.pilot.x,collected.pilot.y);
 collected.advanceDust(.01f);
 assert(collected.iceActive() && collected.dust.empty());
 Session ordinary(m);assert(ordinary.start());
 ordinary.scatterMining(ordinary.pilot.x,ordinary.pilot.y);
 ordinary.advanceDust(.01f);
 assert(!ordinary.iceActive() && ordinary.dust.empty());
 Session loss(m);assert(loss.start());loss.damage(150);
 assert(loss.phase==Phase::Lost);loss.repair(200);
 assert(loss.pilot.health==0 && loss.pilot.damageTaken==150);
}
