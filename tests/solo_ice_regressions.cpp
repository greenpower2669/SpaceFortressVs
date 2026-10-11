#include "../src/solo_session.hpp"
#include <cassert>
#include <cmath>

int main(){
    using namespace sfsolo;
    const Map m=generate({});
    assert(m.valid() && m.version==3);
    assert(m.id.find("-MAZE-V3")!=std::string::npos);
    bool iceTile=false;
    for(const auto tile:m.tiles)if(tile==Tile::IceAsteroid)iceTile=true;
    assert(iceTile); // first stage always teaches the ice mechanic

    // Regression for high-speed tunneling: beginning and endpoint are not the
    // obstacle cell, but the swept path crosses y=80 while travelling upward.
    Session frozen(m);
    assert(frozen.start());
    const int x=int(frozen.pilot.x);
    frozen.pilot.y=81.2f;
    frozen.pilot.vx=0.0f;frozen.pilot.vy=-28.0f;
    frozen.cameraY=frozen.pilot.y;
    frozen.map.tiles[size_t(80)*m.width+x]=Tile::IceAsteroid;
    frozen.map.tiles[size_t(81)*m.width+x]=Tile::Empty;
    frozen.map.tiles[size_t(79)*m.width+x]=Tile::Empty;
    frozen.step(0.0f,0.0f,.05f);
    assert(frozen.iceActive() && frozen.iceSeconds>2.4f);
    assert(frozen.pilot.health==95.0f);
    const float previousIce=frozen.iceSeconds;
    // Move to the known central safe corridor and test 1/4 acceleration.
    frozen.pilot.y=74.5f;frozen.pilot.vx=0;frozen.pilot.vy=0;
    frozen.step(0.0f,-1.0f,.05f);
    const float iceVelocity=frozen.pilot.vy;
    assert(iceVelocity<0 && frozen.iceSeconds<previousIce);
    Session normal(m);
    assert(normal.start());
    normal.pilot.y=74.5f;normal.pilot.vx=0;normal.pilot.vy=0;
    normal.step(0.0f,-1.0f,.05f);
    assert(normal.pilot.vy<iceVelocity); // normal steering responds faster
    assert(std::abs(normal.pilot.vy/iceVelocity-4.0f)<.02f);
    frozen.map.tiles[size_t(80)*m.width+x]=Tile::Empty;
    frozen.pilot.vx=frozen.pilot.vy=0;
    for(int n=0;n<55;++n)frozen.step(0,0,.05f);
    assert(!frozen.iceActive()); // the malus is temporary

    Session rock(m);assert(rock.start());
    rock.pilot.y=81.2f;rock.pilot.vx=0;rock.pilot.vy=-28;
    rock.map.tiles[size_t(80)*m.width+x]=Tile::Rock;
    rock.map.tiles[size_t(81)*m.width+x]=Tile::Empty;
    rock.map.tiles[size_t(79)*m.width+x]=Tile::Empty;
    rock.step(0,0,.05f);
    assert(rock.pilot.y>=81.0f);
    assert(rock.pilot.vy==0.0f && rock.pilot.health==92.0f);

    // Actual diagonal motion along a vertical wall: contact in X must
    // never cancel free Y movement, even at high speed in a narrow corridor.
    Session slide(m);assert(slide.start());
    for(int yy=80;yy<=82;++yy)
        for(int xx=x-1;xx<=x+2;++xx)
            slide.map.tiles[size_t(yy)*m.width+xx]=Tile::Empty;
    slide.pilot.x=x+.90f;slide.pilot.y=81.50f;
    slide.map.tiles[size_t(81)*m.width+(x+1)]=Tile::Rock;
    slide.pilot.vx=10.0f;slide.pilot.vy=-3.0f;
    slide.step(0,0,.05f);
    assert(slide.pilot.x<x+1.0f);
    assert(slide.pilot.y<81.5f); // still glides upward
    assert(slide.pilot.vx==0.0f);
    assert(slide.pilot.vy<0.0f);
    assert(slide.pilot.health==92.0f);
    // Horizontal travel survives contact against an upper rock.
    Session slideY(m);assert(slideY.start());
    for(int yy=79;yy<=82;++yy)
        for(int xx=x-1;xx<=x+2;++xx)
            slideY.map.tiles[size_t(yy)*m.width+xx]=Tile::Empty;
    slideY.pilot.x=x+.50f;slideY.pilot.y=81.10f;
    slideY.map.tiles[size_t(80)*m.width+x]=Tile::Rock;
    slideY.pilot.vx=3.0f;slideY.pilot.vy=-10.0f;
    slideY.step(0,0,.05f);
    assert(slideY.pilot.y>=81.0f);
    assert(slideY.pilot.x>x+.50f); // still glides right
    assert(slideY.pilot.vy==0.0f);
    assert(slideY.pilot.vx>0.0f);
    assert(slideY.pilot.health==92.0f);
}
