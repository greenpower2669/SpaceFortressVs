#include "../src/solo_map_generator.hpp"
#include <cassert>
int main(){
 using namespace sfsolo;
 MapOptions o;
 Map a=generate(o),b=generate(o);
 assert(a.valid() && a.tiles==b.tiles && a.id==b.id);
 assert(a.at(a.width/2,a.height-3)==Tile::Start);
 assert(a.at(a.width/2,2)==Tile::Finish);
 assert(a.at(a.width/2,9)==Tile::Boss);
 assert(hasVerticalSafeCorridor(a));
 for(unsigned world=1;world<=12;world++)
   for(unsigned stage=1;stage<=20;stage++)
     for(unsigned difficulty=1;difficulty<=9;difficulty++){
       o.world=world;o.stage=stage;o.difficulty=difficulty;
       Map m=generate(o);
       assert(m.valid() && hasVerticalSafeCorridor(m));
     }
 o.width=12;assert(!generate(o).valid());
 o.width=54;o.stage=21;assert(!generate(o).valid());
 assert(coopStyleBase(0,100,10)==650);
}
