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
 assert(s.phase==Phase::Won);
 Session loss(m);assert(loss.start());loss.damage(150);
 assert(loss.phase==Phase::Lost);loss.repair(200);
 assert(loss.pilot.health==0 && loss.pilot.damageTaken==150);
}
