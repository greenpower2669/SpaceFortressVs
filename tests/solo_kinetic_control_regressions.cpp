#include "../src/solo_kinetic_control.hpp"
#include <cassert>
#include <initializer_list>

static void hold(sfsolo::KineticSurgeControl &c,sfsolo::Session &s,int n){
    for(int i=0;i<n;++i)c.advance(s,.05f);
}
int main(){
    using namespace sfsolo;
    sfBossDangerIndex=0;
    Session s(generate({}));assert(s.start());
    const int x=int(s.pilot.x),y=int(s.pilot.y);
    const auto place=[&](int rx,int ry,Tile tile=Tile::Asteroid){
        const size_t id=size_t(ry)*s.map.width+size_t(rx);
        s.map.tiles[id]=tile;s.asteroidIntegrity[id]=1.0f;
    };
    place(x,y-1);place(x,y+1);
    KineticSurgeControl c;
    c.press();hold(c,s,3); // below 0.2 sec must not mine
    assert(s.asteroidIntegrityAt(x,y-1)==1);
    hold(c,s,7);
    assert(!sfKineticSurges[1].charged);
    const float small=s.asteroidIntegrityAt(x,y-1);
    assert(small>0&&small<1);
    assert(s.asteroidIntegrityAt(x,y+1)==1);
    assert(!c.release(s));
    assert(s.asteroidIntegrityAt(x,y-1)==small);
    c.press();hold(c,s,42); // ready, still mining
    assert(sfKineticSurges[1].charged);
    assert(s.asteroidIntegrityAt(x,y-1)<small);
    hold(c,s,250); // keep holding AFTER ready
    assert(s.map.at(x,y-1)==Tile::Empty);
    assert(s.map.at(x,y+1)==Tile::Asteroid);
    assert(s.dust.size()>=8);
    assert(c.release(s)&&c.burstSeconds>.4f);
    assert(!c.release(s));
    for(int i=0;i<10;++i)c.advance(.05f);
    assert(c.burstSeconds==0);
    place(x,y-1);
    c.press();hold(c,s,10);
    const float partial=s.asteroidIntegrityAt(x,y-1);
    assert(partial>0&&partial<1);
    c.cancel();
    assert(!c.release(s)&&s.asteroidIntegrityAt(x,y-1)==partial);
    sfBossDangerIndex=8;
    c.press();hold(c,s,200);
    assert(c.release(s));
    assert(s.asteroidIntegrityAt(x,y-1)==partial);
    sfBossDangerIndex=0;
    const auto harvest=[&](int rx,int ry,float tx,float ty){
        place(rx,ry);
        c.aimAt(s,tx,ty);c.press();hold(c,s,320);
        assert(s.map.at(rx,ry)==Tile::Empty);
        assert(c.release(s));
    };
    harvest(x,y+1,s.pilot.x,s.pilot.y+15);
    harvest(x-1,y,s.pilot.x-15,s.pilot.y);
    harvest(x+1,y,s.pilot.x+15,s.pilot.y);
    place(x,y-1,Tile::IceAsteroid);
    c.aimAt(s,s.pilot.x,s.pilot.y-15);c.press();hold(c,s,320);
    assert(s.map.at(x,y-1)==Tile::Empty&&c.release(s));
    bool icy=false;for(const auto &p:s.dust)icy=icy||p.icy;
    assert(icy);
    Session m(generate({}));assert(m.start());m.shipRadius=1.2f;
    const int mx=int(m.pilot.x),my=int(m.pilot.y);
    for(int dx:{-1,1}){
        const size_t i=size_t(my-2)*m.map.width+size_t(mx+dx);
        m.map.tiles[i]=Tile::Asteroid;m.asteroidIntegrity[i]=1;
    }
    KineticSurgeControl both;
    both.aimAt(m,m.pilot.x,m.pilot.y-30);
    both.press();hold(both,m,10);
    assert(m.asteroidIntegrityAt(mx-1,my-2)<1);
    assert(m.asteroidIntegrityAt(mx+1,my-2)<1);
    both.cancel();
    // Canonical release creates the SAME radial purge as COOP, including
    // behind the ship; ordinary 2-second holding stays aimed and 360-degree.
    Session canonical(generate({}));assert(canonical.start());
    canonical.canonicalShipActive=true;
    const int bx=int(canonical.pilot.x),by=int(canonical.pilot.y);
    const size_t rear=size_t(by+1)*canonical.map.width+bx;
    canonical.map.tiles[rear]=Tile::Asteroid;
    canonical.asteroidIntegrity[rear]=1.0f;
    KineticSurgeControl release;
    release.aimAt(canonical,canonical.pilot.x,canonical.pilot.y-10);
    release.press();
    for(int i=0;i<41;++i)release.advance(canonical,.05f);
    assert(canonical.map.at(bx,by+1)==Tile::Asteroid);
    assert(release.release(canonical));
    assert(canonical.map.at(bx,by+1)==Tile::Empty);
    assert(!canonical.dust.empty());
    assert(!sfKineticWaves.empty());
    sfKineticWaves.clear();
    sfBossDangerIndex=2;
}
