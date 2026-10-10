#include "../src/solo_campaign_model.hpp"
#include <cassert>
#include <cmath>
int main(){
 using namespace sfsolo;
 Map m; m.id="WORLD-01-01";m.width=3;m.height=4;m.tiles.assign(12,Tile::Empty);
 m.tiles[10]=Tile::Start;m.tiles[1]=Tile::Finish;
 assert(m.valid());assert(m.at(-1,0)==Tile::Rock);
 assert(fromRgb(0xff,0x80,0)==Tile::Lava);
 assert(fromRgb(0x12,0x34,0x56)==Tile::Rock);
 Physics physics;Pilot pilot;
 assert(SOLO_REACTIVITY_MULTIPLIER==4.0f);
 assert(physics.thrust==20.0f && physics.maxSpeed==28.0f);
 Physics old;old.thrust=5.0f;old.maxSpeed=7.0f;
 Pilot quick,slow;
 for(int i=0;i<12;++i){physics.step(quick,0,-1,.05f);old.step(slow,0,-1,.05f);}
 assert(std::abs(quick.y/slow.y-4.0f)<.02f); // 4x travel, no teleport
 assert(std::abs(quick.vy/slow.vy-4.0f)<.02f); // 4x response
 const float before=quick.vy;
 physics.step(quick,0,0,.05f);
 assert(std::abs(quick.vy)<std::abs(before)*.95f); // active braking
 physics.step(pilot,0,-1,.05f);
 assert(pilot.vy<0 && pilot.y<0);
 float oldSpeed=std::abs(pilot.vy);physics.step(pilot,0,0,.05f);
 assert(std::abs(pilot.vy)<oldSpeed);
 ScoreInput s;s.alive=true;s.reachedFinish=true;s.bossDefeated=true;
 s.enemiesAvailable=10;s.enemiesDefeated=10;s.coopCombatPoints=1000;
 int clean=score(s);s.damageTaken=50;assert(score(s)<clean);
 s.bossDefeated=false;assert(score(s)==0);
 assert(worldUnlocked(1,{}));assert(!worldUnlocked(2,{}));
 assert(worldUnlocked(2,{true}));
}
