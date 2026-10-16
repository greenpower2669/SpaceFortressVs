#pragma once
// Shared artwork coordinates only: no prototype simulation dependency.
#include <SDL2/SDL.h>
#include <algorithm>
namespace sfsolo {
// Source boxes are inside artwork only: NEVER include the headings, labels,
// demonstration rows, or panel borders of the 1536x1024 concept sheet.
inline SDL_Rect soloWallArtBox(int theme,int index){
    const int col=std::clamp(theme,0,5)*256;
    switch(index){
        case 0:return {col+19,101,112,101}; // continuous horizontal, variant A
        case 1:return {col+79,101,112,101}; // horizontal B
        case 2:return {col+136,101,106,101}; // horizontal C
        case 3:return {col+20,252,69,117}; // real vertical segment
        case 4:return {col+108,252,84,117}; // inner corner
        case 5:return {col+200,254,53,114}; // outer corner
        case 6:return {col+120,416,77,51}; // junction boulder
        case 7:return {col+20,414,88,54}; // terminal/rock cap
        default:return {col+20,414,88,54};
    }
}
inline SDL_Rect soloEnvironmentArtBox(int theme,int variant){
    // Original 1122x1402 atlas: seven independent objects per themed row.
    constexpr int tops[5]={12,292,575,852,1121};
    const int row=std::clamp(theme,0,4);
    const int v=std::clamp(variant,0,6);
    return {10+v*157,tops[row]+13,138,223};
}
inline int soloEnvironmentArtTheme(int wallTheme){
    // wall: rock lava energy dust ice smoke -> atlas rows energy lava rock ice smoke
    switch(wallTheme){
        case 1:return 1;
        case 2:return 0;
        case 4:return 3;
        case 5:return 4;
        default:return 2;
    }
}
} // namespace sfsolo
