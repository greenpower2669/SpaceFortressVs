#include "../src/solo_combat.hpp"
#include <cassert>
int main(){
 using namespace sfsolo;
 Session s(generate({}));
 assert(s.start());
 Combat combat;
 // Put an enemy and boss in a clear firing lane directly ahead.
 const int x=int(s.pilot.x),y=int(s.pilot.y);
 assert(y>=5 && y<s.map.height);
 for(int j=y-5;j<=y;j++)s.map.tiles[size_t(j)*s.map.width+x]=Tile::Empty;
 s.map.tiles[size_t(y-2)*s.map.width+x]=Tile::Enemy;
 s.map.tiles[size_t(y-5)*s.map.width+x]=Tile::Boss;
 s.enemiesAvailable=1;s.enemiesDefeated=0;
 assert(combat.fire(s,x+.5f,y-5.5f));
 assert(!combat.fire(s,x+.5f,y-5.5f)); // cooldown
 for(int i=0;i<10;i++)combat.step(s,.05f);
 assert(s.enemiesDefeated==1);
 assert(s.map.at(x,y-2)==Tile::Empty);
 assert(combat.fire(s,x+.5f,y-5.5f));
 for(int i=0;i<10;i++)combat.step(s,.05f);
 assert(combat.bossHealth<100);
 for(int n=0;n<9;n++){
   assert(combat.fire(s,x+.5f,y-5.5f));
   for(int i=0;i<10;i++)combat.step(s,.05f);
 }
 assert(s.bossDefeated);
 assert(s.map.at(x,y-5)==Tile::Empty);
 assert(combat.bossHealth==0);
}
