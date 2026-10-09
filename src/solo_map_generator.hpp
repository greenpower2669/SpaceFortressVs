#pragma once
#include "solo_campaign_model.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace sfsolo {
// Fixed integer PRNG: the same world/stage/difficulty yields the same authored
// logical map on Android and desktop. No wall clock, rand() or device state.
struct Rng {
    uint32_t state;
    explicit Rng(uint32_t seed):state(seed ? seed : 0x9e3779b9u){}
    uint32_t next() {
        uint32_t x=state;
        x^=x<<13;x^=x>>17;x^=x<<5;
        return state=x;
    }
};
struct MapOptions {
    unsigned world=1,stage=1,difficulty=1;
    int width=54,height=160; // logical 9:16 viewport; stages may extend vertically
    uint32_t campaignSeed=0x5f2026u;
};
inline Map generate(const MapOptions &o) {
    Map m;
    if(o.world==0 || o.stage==0 || o.stage>20 || o.difficulty<1 ||
       o.difficulty>9 || o.width<12 || o.width>512 ||
       o.height<32 || o.height>8192) return m;
    m.world=o.world;m.stage=o.stage;m.version=1;
    m.id="W"+std::to_string(o.world)+"-S"+std::to_string(o.stage)+
         "-D"+std::to_string(o.difficulty)+"-V1";
    m.width=o.width;m.height=o.height;
    m.tiles.assign(size_t(m.width)*size_t(m.height),Tile::Empty);
    Rng rng(o.campaignSeed ^ (o.world*0x85ebca6bu) ^
            (o.stage*0xc2b2ae35u) ^ (o.difficulty*0x27d4eb2fu));
    const int mid=m.width/2;
    auto put=[&](int x,int y,Tile t){m.tiles[size_t(y)*size_t(m.width)+size_t(x)]=t;};
    // Vertical progression: start at bottom, finish at top.
    // Preserve a navigable 5-cell central corridor until physics validation.
    for(int y=0;y<m.height;y++){
        const int inset=2+int(rng.next()%3u);
        for(int x=0;x<inset;x++){put(x,y,Tile::Rock);put(m.width-1-x,y,Tile::Rock);}
        if(y>8 && y<m.height-9 && y%7==0){
            const int side=int(rng.next()%2u);
            const int x=side ? mid+4+int(rng.next()%uint32_t(mid-8)):
                               3+int(rng.next()%uint32_t(mid-8));
            const unsigned roll=rng.next()%10u;
            Tile obstacle=roll<2 ? Tile::Lava : roll<4 ? Tile::Beam :
                          roll<7 ? Tile::Asteroid : Tile::Enemy;
            // Fewer recovery asteroids on the highest difficulty.
            if(obstacle==Tile::Asteroid && o.difficulty>=8 &&
               rng.next()%3u!=0u) obstacle=Tile::Enemy;
            put(x,y,obstacle);
        }
    }
    // An encounter wave can be followed by a deterministic recovery asteroid.
    for(int y=m.height-18;y>14;y-=32){
        put(mid+5,y,Tile::Enemy);
        if(o.difficulty<8 || (rng.next()%3u==0u))
            put(mid-5,y-5,Tile::Asteroid);
    }
    // Boss before finish; its runtime encounter must be defeated to exit.
    put(mid,m.height>30 ? 9 : 5,Tile::Boss);
    put(mid,m.height-3,Tile::Start);
    put(mid,2,Tile::Finish);
    return m;
}
inline bool hasVerticalSafeCorridor(const Map &m) {
    if(!m.valid())return false;
    const int x=m.width/2;
    for(int y=3;y<m.height-3;y++){
        const Tile t=m.at(x,y);
        if(t!=Tile::Empty && t!=Tile::Boss) return false;
    }
    return true;
}
} // namespace sfsolo
