#include "../src/solo_renderer.hpp"
#include <cassert>
int main(){
    using namespace sfsolo;
    Session s(generate({}));assert(s.start());
    for(int theme=0;theme<6;++theme){
        for(int y=0;y<9;++y)for(int x=0;x<9;++x){
            const auto sample=soloWallAtlasSample(theme,x,y);
            assert(sample.x>=theme*256&&sample.x+sample.w<=(theme+1)*256);
            assert(sample.y>=65&&sample.y+sample.h<=218);
            const auto fx=soloEnvironmentAtlasSample(theme,x,y);
            assert(fx.x>=0&&fx.x+fx.w<=1122);
            assert(fx.y>=0&&fx.y+fx.h<=1402);
        }
    }
    s.map.tiles[size_t(20)*s.map.width+20]=Tile::Lava;
    assert(soloWallTheme(s,20,20)==1);
    s.map.tiles[size_t(20)*s.map.width+20]=Tile::Empty;
    s.map.tiles[size_t(20)*s.map.width+20]=Tile::Beam;
    assert(soloWallTheme(s,20,20)==2);
    s.map.tiles[size_t(20)*s.map.width+20]=Tile::Empty;
    s.map.tiles[size_t(20)*s.map.width+20]=Tile::IceAsteroid;
    assert(soloWallTheme(s,20,20)==4);
}
