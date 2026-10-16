#pragma once
// SOLO visual topology: walls are ALWAYS the original collidable map cells.
// Deterministic selection based on four adjacent solid tiles; no RNG per draw.
#include "solo_session.hpp"
#include "solo_scenery_atlas.hpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cstdint>

namespace sfsolo {
enum class SoloWallShape : uint8_t {
    Isolated=0, Horizontal=1, Vertical=2, Corner=3,
    Tee=4, Cross=5, End=6
};
enum SoloWallDir : uint8_t {
    WallNorth=1, WallEast=2, WallSouth=4, WallWest=8
};
inline bool soloIsWall(Tile t){
    return t==Tile::Rock||t==Tile::Lava||t==Tile::Beam;
}
inline uint8_t soloWallNeighbors(const Session &s,int x,int y){
    uint8_t mask=0;
    if(soloIsWall(s.map.at(x,y-1)))mask|=WallNorth;
    if(soloIsWall(s.map.at(x+1,y)))mask|=WallEast;
    if(soloIsWall(s.map.at(x,y+1)))mask|=WallSouth;
    if(soloIsWall(s.map.at(x-1,y)))mask|=WallWest;
    return mask;
}
struct SoloWallPresentation {
    SoloWallShape shape=SoloWallShape::Isolated;
    int rotation=0;  // degrees clockwise, exact multiple of 90
    uint8_t variant=0; // stable, tied to WORLD coordinate
};
inline uint32_t soloWorldDecorHash(int x,int y,uint32_t salt=0){
    uint32_t h=uint32_t(x)*0x9e3779b1u ^ uint32_t(y)*0x85ebca6bu ^ salt;
    h^=h>>16;h*=0x7feb352du;h^=h>>15;h*=0x846ca68bu;h^=h>>16;
    return h;
}
inline SoloWallPresentation soloWallPresentation(uint8_t rawMask,int x,int y){
    const uint8_t mask=rawMask&15;
    SoloWallPresentation out;
    out.variant=uint8_t(soloWorldDecorHash(x,y,0xAC101u)%3u);
    switch(mask){
        case 0: out.shape=SoloWallShape::Isolated;break;
        // Authored end-cap is horizontal: rotate 90° for vertical walls.
        case 1: case 4: out.shape=SoloWallShape::End;out.rotation=mask==1?90:270;break;
        case 2: case 8: out.shape=SoloWallShape::End;out.rotation=mask==2?180:0;break;
        case 5: out.shape=SoloWallShape::Vertical;break;
        case 10:out.shape=SoloWallShape::Horizontal;break;
        // Authored L: stone points LEFT and DOWN (shape ┐).
        // The connectivity mask therefore starts at West|South = 12.
        case 12:out.shape=SoloWallShape::Corner;out.rotation=0;break;
        case 9: out.shape=SoloWallShape::Corner;out.rotation=90;break;
        case 3: out.shape=SoloWallShape::Corner;out.rotation=180;break;
        case 6: out.shape=SoloWallShape::Corner;out.rotation=270;break;
        // T has three neighbors: rotate toward the missing side.
        case 7: out.shape=SoloWallShape::Tee;out.rotation=0;break;
        case 14:out.shape=SoloWallShape::Tee;out.rotation=90;break;
        case 13:out.shape=SoloWallShape::Tee;out.rotation=180;break;
        case 11:out.shape=SoloWallShape::Tee;out.rotation=270;break;
        case 15:out.shape=SoloWallShape::Cross;break;
    }
    return out;
}
inline SoloWallPresentation soloWallPresentation(const Session &s,int x,int y){
    return soloWallPresentation(soloWallNeighbors(s,x,y),x,y);
}
inline float soloDecoPhase(int x,int y,int type){
    return float(soloWorldDecorHash(x,y,uint32_t(type)+0xF35Du)%6283u)*.001f;
}
} // namespace sfsolo
