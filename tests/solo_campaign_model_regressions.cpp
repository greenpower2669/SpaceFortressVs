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
 Physics physics;Pilot pilot;physics.step(pilot,0,-1,.05f);
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
