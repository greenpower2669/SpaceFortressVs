#include "../src/solo_renderer.hpp"
#include <cassert>
int main(){
 using namespace sfsolo;
 assert(tileColor(Tile::Lava).r==255);
 assert(tileColor(Tile::Finish).g==239);
 auto m=generate({});
 Session s(m);assert(s.start());
 SDL_Rect viewport{0,0,540,960};
 drawSession(nullptr,s,viewport); // renderer guard must not dereference
 assert(s.map.valid());
 assert(soloImpactFlashAlpha(s)==0);
 s.collisionCooldown=.45f;
 assert(soloImpactFlashAlpha(s)==160);
 s.collisionCooldown=.225f;
 assert(soloImpactFlashAlpha(s)==80);
 s.phase=Phase::Lost;
 assert(soloImpactFlashAlpha(s)==0);
 return 0;
}
