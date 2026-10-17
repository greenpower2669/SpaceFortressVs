#pragma once
// SOLO-024: continuous, map-space organic Rock silhouette.
// Cached 8x8-cell RGBA chunks; no seam at tile boundaries, no map edits.
// The source paint is still ParoisModeSolo.png, never replaced or resized.
#include "solo_map_generator.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

namespace sfsolo {
struct SoloOrganicRims {
    static constexpr int chunkCells=8, texelsPerCell=48;
    static constexpr int textureSide=chunkCells*texelsPerCell;
    static constexpr int maxCached=24, themes=6;
    struct Chunk {
        SDL_Texture *texture=nullptr;
        int cx=-1,cy=-1,theme=-1;
        std::uint64_t signature=0;
        unsigned stamp=0;
    };
    std::array<Chunk,maxCached> cache{};
    std::array<SDL_Surface*,themes> painted{};
    unsigned tick=0;
    bool ready()const {for(auto *s:painted)if(!s)return false;return true;}
    static bool rock(const Map &map,int x,int y){
        return x>=0&&x<map.width&&y>=0&&y<map.height&&map.at(x,y)==Tile::Rock;
    }
    static float smooth(float x){
        x=std::clamp(x,0.f,1.f);return x*x*(3.f-2.f*x);
    }
    // Bilinear ROCK samples at cell centres smooth external corners over
    // multiple cells, unlike a 16-mask piece repeated at each tile border.
    static float field(const Map &map,float gx,float gy){
        const float ax=gx-.5f,ay=gy-.5f;
        const int ix=int(std::floor(ax)),iy=int(std::floor(ay));
        const float u=smooth(ax-ix),v=smooth(ay-iy);
        const float a=rock(map,ix,iy)?1.f:0.f;
        const float b=rock(map,ix+1,iy)?1.f:0.f;
        const float c=rock(map,ix,iy+1)?1.f:0.f;
        const float d=rock(map,ix+1,iy+1)?1.f:0.f;
        return (a+(b-a)*u)*(1-v)+(c+(d-c)*u)*v;
    }
    static float contourThreshold(float gx,float gy){
        // Low spatial frequencies in MAP coordinates ensure every adjoining
        // chunk gets an identical contour. No per-tile random phase reset.
        return .51f+.115f*std::sin(3.1f*gy+1.17f*std::sin(1.31f*gx))
            +.085f*std::sin(1.63f*gx-2.31f*gy)
            +.052f*std::sin(5.37f*gy+2.07f*gx);
    }
    static bool visibleRock(const Map &map,float gx,float gy){
        return field(map,gx,gy)>=contourThreshold(gx,gy);
    }
    void release(){
        for(auto &entry:cache){
            if(entry.texture)SDL_DestroyTexture(entry.texture);
            entry=Chunk{};
        }
        for(auto *&s:painted){if(s)SDL_FreeSurface(s);s=nullptr;}
        tick=0;
    }
    ~SoloOrganicRims(){release();}
    SoloOrganicRims()=default;
    SoloOrganicRims(const SoloOrganicRims&)=delete;
    SoloOrganicRims&operator=(const SoloOrganicRims&)=delete;
    bool load(SDL_Surface *sheet){
        release();
        if(!sheet)return false;
        for(int theme=0;theme<themes;++theme){
            auto *s=SDL_CreateRGBSurfaceWithFormat(0,200,58,32,SDL_PIXELFORMAT_RGBA32);
            if(!s){release();return false;}
            SDL_SetSurfaceBlendMode(sheet,SDL_BLENDMODE_NONE);
            const SDL_Rect crop{theme*256+30,126,200,58};
            if(SDL_BlitSurface(sheet,&crop,s,nullptr)!=0){
                SDL_FreeSurface(s);release();return false;
            }
            painted[size_t(theme)]=s;
        }
        return true;
    }
    static std::uint64_t signature(const Map &map,int cx,int cy){
        std::uint64_t code=1469598103934665603ULL;
        // 2 cell halo catches any neighbour whose smoothed field contributes.
        for(int y=cy*chunkCells-2;y<(cy+1)*chunkCells+2;++y)
            for(int x=cx*chunkCells-2;x<(cx+1)*chunkCells+2;++x){
                code^=rock(map,x,y)?1u:0u;
                code*=1099511628211ULL;
            }
        return code;
    }
    SDL_Texture *ensure(SDL_Renderer *r,const Map &map,int theme,int cx,int cy){
        if(!r||!ready()||theme<0||theme>=themes)return nullptr;
        const auto sig=signature(map,cx,cy);
        Chunk *slot=nullptr;
        for(auto &entry:cache)
            if(entry.texture&&entry.cx==cx&&entry.cy==cy&&entry.theme==theme){
                slot=&entry;break;
            }
        if(slot&&slot->signature==sig){slot->stamp=++tick;return slot->texture;}
        if(!slot){
            slot=&cache[0];
            for(auto &entry:cache){
                if(!entry.texture){slot=&entry;break;}
                if(entry.stamp<slot->stamp)slot=&entry;
            }
        }
        auto *surf=SDL_CreateRGBSurfaceWithFormat(
            0,textureSide,textureSide,32,SDL_PIXELFORMAT_RGBA32);
        if(!surf)return nullptr;
        // Generate pixels at world-fixed positions: texture across all tile
        // seams is sampled with ONE continuous phase, never repeats per cell.
        const SDL_Surface *paint=painted[size_t(theme)];
        for(int y=0;y<textureSide;++y){
            auto *row=reinterpret_cast<Uint32*>(static_cast<Uint8*>(surf->pixels)+y*surf->pitch);
            const float gy=cy*chunkCells+(y+.5f)/texelsPerCell;
            const int py=(int(std::floor(gy*58.f/.65f))%58+58)%58;
            const auto *paintRow=reinterpret_cast<const Uint32*>(
                static_cast<const Uint8*>(paint->pixels)+py*paint->pitch);
            for(int x=0;x<textureSide;++x){
                const float gx=cx*chunkCells+(x+.5f)/texelsPerCell;
                const float strength=field(map,gx,gy);
                if(strength<contourThreshold(gx,gy)){
                    row[x]=SDL_MapRGBA(surf->format,0,0,0,0);continue;
                }
                const int px=(int(std::floor(gx*200.f/2.f))%200+200)%200;
                Uint8 red,green,blue,alpha;
                SDL_GetRGBA(paintRow[px],paint->format,&red,&green,&blue,&alpha);
                // Subtle bevel along a LONG natural continuous contour.
                const float light=std::clamp(.76f+.32f*strength+
                    .11f*std::sin(2.4f*gx-1.5f*gy),.68f,1.17f);
                auto adjust=[light](Uint8 c){
                    return Uint8(std::clamp(int(c*light),0,255));
                };
                row[x]=SDL_MapRGBA(surf->format,adjust(red),adjust(green),adjust(blue),255);
            }
        }
        SDL_Texture *texture=SDL_CreateTextureFromSurface(r,surf);
        SDL_FreeSurface(surf);
        if(!texture)return nullptr; // preserve older slot on failure
        SDL_SetTextureBlendMode(texture,SDL_BLENDMODE_BLEND);
        SDL_SetTextureScaleMode(texture,SDL_ScaleModeLinear);
        if(slot->texture)SDL_DestroyTexture(slot->texture);
        slot->texture=texture;slot->cx=cx;slot->cy=cy;
        slot->theme=theme;slot->signature=sig;slot->stamp=++tick;
        return texture;
    }
    template<class World>
    void draw(SDL_Renderer *r,const World &w,int theme){
        if(!r||!ready()||!w.ready)return;
        const float c=w.pixelsPerCell,zoom=w.zoom;
        const int x0=std::max(0,int(std::floor(w.cameraLeft/c))/chunkCells);
        const int y0=std::max(0,int(std::floor(w.cameraTop/c))/chunkCells);
        const int x1=std::min((w.map.width+chunkCells-1)/chunkCells,
            (int(std::ceil((w.cameraLeft+w.visibleWidth())/c))+chunkCells-1)/chunkCells);
        const int y1=std::min((w.map.height+chunkCells-1)/chunkCells,
            (int(std::ceil((w.cameraTop+w.visibleHeight())/c))+chunkCells-1)/chunkCells);
        for(int cy=y0;cy<y1;++cy)for(int cx=x0;cx<x1;++cx){
            auto *tex=ensure(r,w.map,theme,cx,cy);
            if(!tex)continue;
            const auto origin=w.toScreen({cx*chunkCells*c,cy*chunkCells*c});
            const SDL_FRect dest{origin.x,origin.y,
                chunkCells*c*zoom,chunkCells*c*zoom};
            SDL_RenderCopyF(r,tex,nullptr,&dest);
        }
    }
};
} // namespace sfsolo
