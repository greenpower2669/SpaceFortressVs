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
 return 0;
}
