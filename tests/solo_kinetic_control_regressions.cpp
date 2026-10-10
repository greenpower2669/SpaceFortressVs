#include "../src/solo_kinetic_control.hpp"
#include <cassert>
int main(){
    using namespace sfsolo;
    sfBossDangerIndex=0; // difficulty one: maximum canonical cone reach.
    Session s(generate({}));
    assert(s.start());
    const int x=int(s.pilot.x), y=int(s.pilot.y);
    assert(s.map.at(x,y-1)==Tile::Empty);
    s.map.tiles[size_t(y-1)*s.map.width+x]=Tile::Asteroid;
    s.map.tiles[size_t(y+1)*s.map.width+x]=Tile::Asteroid;
    KineticSurgeControl control;
    control.press();
    for(int n=0;n<10;++n)control.advance(.05f);
    assert(!sfKineticSurges[1].charged);
    assert(!control.release(s)); // short press fires rather than mining.
    assert(s.map.at(x,y-1)==Tile::Asteroid);
    control.press();
    for(int n=0;n<41;++n)control.advance(.05f);
    assert(sfKineticSurges[1].charged);
    assert(control.release(s));
    assert(s.map.at(x,y-1)==Tile::Empty); // forward/up, canonical cone.
    assert(s.map.at(x,y+1)==Tile::Asteroid); // never harvest backwards.
    assert(!control.release(s)); // one release, one blast.
    s.map.tiles[size_t(y-1)*s.map.width+x]=Tile::Asteroid;
    control.press();
    for(int n=0;n<41;++n)control.advance(.05f);
    control.cancel(); // app/menu interruption must not purge.
    assert(!control.release(s));
    assert(s.map.at(x,y-1)==Tile::Asteroid);
    sfBossDangerIndex=8; // Apocalypse: much shorter canonical cone.
    control.press();
    for(int n=0;n<41;++n)control.advance(.05f);
    assert(control.release(s));
    assert(s.map.at(x,y-1)==Tile::Asteroid);
    sfBossDangerIndex=2;
}
