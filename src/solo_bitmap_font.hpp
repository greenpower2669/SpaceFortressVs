#pragma once
// Built-in 5x7 pixel lettering for the SOLO debug selector.
// No external font files or new SDL_ttf runtime dependency.
#include <SDL2/SDL.h>
#include <algorithm>
#include <array>
#include <string>

namespace sfsolo {
inline std::array<unsigned char,7> soloGlyph(char c) {
    switch(c) {
        case 'A':return {14,17,17,31,17,17,17};
        case 'B':return {30,17,17,30,17,17,30};
        case 'C':return {14,17,16,16,16,17,14};
        case 'D':return {30,17,17,17,17,17,30};
        case 'E':return {31,16,16,30,16,16,31};
        case 'F':return {31,16,16,30,16,16,16};
        case 'G':return {14,17,16,23,17,17,15};
        case 'H':return {17,17,17,31,17,17,17};
        case 'I':return {31,4,4,4,4,4,31};
        case 'J':return {7,2,2,2,18,18,12};
        case 'K':return {17,18,20,24,20,18,17};
        case 'L':return {16,16,16,16,16,16,31};
        case 'M':return {17,27,21,21,17,17,17};
        case 'N':return {17,25,21,19,17,17,17};
        case 'O':return {14,17,17,17,17,17,14};
        case 'P':return {30,17,17,30,16,16,16};
        case 'Q':return {14,17,17,17,21,18,13};
        case 'R':return {30,17,17,30,20,18,17};
        case 'S':return {15,16,16,14,1,1,30};
        case 'T':return {31,4,4,4,4,4,4};
        case 'U':return {17,17,17,17,17,17,14};
        case 'V':return {17,17,17,17,17,10,4};
        case 'W':return {17,17,17,21,21,21,10};
        case 'X':return {17,17,10,4,10,17,17};
        case 'Y':return {17,17,10,4,4,4,4};
        case 'Z':return {31,1,2,4,8,16,31};
        case '0':return {14,17,19,21,25,17,14};
        case '1':return {4,12,4,4,4,4,14};
        case '2':return {14,17,1,2,4,8,31};
        case '3':return {30,1,1,14,1,1,30};
        case '4':return {2,6,10,18,31,2,2};
        case '5':return {31,16,16,30,1,1,30};
        case '6':return {14,16,16,30,17,17,14};
        case '7':return {31,1,2,4,8,8,8};
        case '8':return {14,17,17,14,17,17,14};
        case '9':return {14,17,17,15,1,1,14};
        case '/':return {1,1,2,4,8,16,16};
        case '-':return {0,0,0,31,0,0,0};
        case ':':return {0,4,4,0,4,4,0};
        case '.':return {0,0,0,0,0,12,12};
        case '+':return {0,4,4,31,4,4,0};
        case '!':return {4,4,4,4,4,0,4};
        case '?':return {14,17,1,2,4,0,4};
        case ' ':return {0,0,0,0,0,0,0};
        default:return {31,17,21,21,21,17,31};
    }
}
inline int soloTextWidth(const std::string &s,int pixel) {
    return int(s.size())*6*std::max(1,pixel);
}
inline void soloText(SDL_Renderer *r,const std::string &s,int centerX,int y,
                     int pixel,SDL_Color color) {
    if(!r || pixel<=0)return;
    const int x0=centerX-soloTextWidth(s,pixel)/2;
    SDL_SetRenderDrawColor(r,color.r,color.g,color.b,color.a);
    for(size_t i=0;i<s.size();++i){
        const auto glyph=soloGlyph(s[i]);
        for(int gy=0;gy<7;++gy)for(int gx=0;gx<5;++gx) {
            if((glyph[size_t(gy)]>>(4-gx))&1u) {
                SDL_Rect p{x0+int(i)*6*pixel+gx*pixel,y+gy*pixel,pixel,pixel};
                SDL_RenderFillRect(r,&p);
            }
        }
    }
}
inline void soloLabel(SDL_Renderer *r,const std::string &text,SDL_Rect box,
                      SDL_Color color,int desiredPixels) {
    if(!r||box.w<=0||box.h<=0)return;
    const int px=std::max(1,std::min({desiredPixels,
        box.w/std::max(1,int(text.size())*6),box.h/8}));
    soloText(r,text,box.x+box.w/2,box.y+(box.h-7*px)/2,px,color);
}
} // namespace sfsolo
