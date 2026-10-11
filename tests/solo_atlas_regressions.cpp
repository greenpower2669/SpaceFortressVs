#include "../src/solo_renderer.hpp"
#include <cassert>
#include <cmath>
#include <array>

static bool eq(sfsolo::SoloWallShape a,sfsolo::SoloWallShape b){return a==b;}
int main(){
    using namespace sfsolo;
    const std::array<SoloWallShape,16> expected{{
        SoloWallShape::Isolated, SoloWallShape::End,
        SoloWallShape::End, SoloWallShape::Corner,
        SoloWallShape::End, SoloWallShape::Vertical,
        SoloWallShape::Corner, SoloWallShape::Tee,
        SoloWallShape::End, SoloWallShape::Corner,
        SoloWallShape::Horizontal, SoloWallShape::Tee,
        SoloWallShape::Corner, SoloWallShape::Tee,
        SoloWallShape::Tee, SoloWallShape::Cross
    }};
    for(unsigned mask=0;mask<16;++mask){
        auto a=soloWallPresentation(uint8_t(mask),44,97);
        assert(eq(a.shape,expected[mask]));
        assert(a.rotation%90==0);
        assert(a.variant<=2);
        assert(soloWallPresentation(uint8_t(mask),44,97).rotation==a.rotation);
        assert(soloWallPresentation(uint8_t(mask),44,97).variant==a.variant);
        assert(soloWallSpriteIndex(a)>=0&&soloWallSpriteIndex(a)<8);
    }
    assert(soloWallPresentation(12,2,8).rotation==0); // actual art opens SW
    assert(soloWallPresentation(9,2,8).rotation==90);
    assert(soloWallPresentation(3,2,8).rotation==180);
    assert(soloWallPresentation(6,2,8).rotation==270);
    assert(soloWallPresentation(1,2,8).rotation==90);  // vertical cap
    assert(soloWallPresentation(2,2,8).rotation==180); // horizontal cap
    assert(soloWallPresentation(5,2,8).shape==SoloWallShape::Vertical);
    assert(soloWallPresentation(10,2,8).shape==SoloWallShape::Horizontal);
    assert(soloWallPresentation(15,2,8).shape==SoloWallShape::Cross);
    assert(soloDecoPhase(24,19,2)==soloDecoPhase(24,19,2));
    assert(soloWorldDecorHash(44,97,13u)==soloWorldDecorHash(44,97,13u));

    Session s(generate({}));assert(s.start());
    // Isolate the test geometry from the procedural maze.
    const int x=20,y=55;
    for(int yy=y-2;yy<=y+2;++yy)
      for(int xx=x-2;xx<=x+2;++xx)
        s.map.tiles[size_t(yy)*s.map.width+xx]=Tile::Empty;
    s.map.tiles[size_t(y)*s.map.width+x]=Tile::Rock;
    assert(soloWallNeighbors(s,x,y)==0);
    s.map.tiles[size_t(y)*s.map.width+x+1]=Tile::Lava;
    s.map.tiles[size_t(y+1)*s.map.width+x]=Tile::Rock;
    assert(soloWallNeighbors(s,x,y)==(WallEast|WallSouth));
    assert(soloWallPresentation(s,x,y).shape==SoloWallShape::Corner);
    s.map.tiles[size_t(y-1)*s.map.width+x]=Tile::Beam;
    assert(soloWallPresentation(s,x,y).shape==SoloWallShape::Tee);
    s.map.tiles[size_t(y)*s.map.width+x-1]=Tile::Rock;
    assert(soloWallPresentation(s,x,y).shape==SoloWallShape::Cross);
    assert(s.map.at(x,y)==Tile::Rock); // VISUAL logic must NEVER mutate collision map

    for(int theme=0;theme<6;++theme){
      for(int part=0;part<8;++part){
        SDL_Rect r=soloWallArtBox(theme,part);
        assert(r.w>0&&r.h>0&&r.x>=0&&r.y>=0);
        assert(r.x+r.w<=1536&&r.y+r.h<=1024);
        assert(r.x>=256*theme&&r.x+r.w<=256*(theme+1));
        assert(r.y>=98&&r.y+r.h<=480); // no titles/labels/preview examples
      }
    }
    for(int theme=0;theme<5;++theme)for(int frame=0;frame<7;++frame){
        SDL_Rect r=soloEnvironmentArtBox(theme,frame);
        assert(r.x>=0&&r.y>=0&&r.w>0&&r.h>0);
        assert(r.x+r.w<=1122&&r.y+r.h<=1402);
    }
    assert(soloEnvironmentArtTheme(2)==0);
    assert(soloEnvironmentArtTheme(1)==1);
    assert(soloEnvironmentArtTheme(4)==3);
    assert(soloEnvironmentArtTheme(5)==4);
    return 0;
}
