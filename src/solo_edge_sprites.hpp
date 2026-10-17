#pragma once
// SOLO-023: sixteen wall-neighbour profiles x three deterministic variations.
// RGBA atlas is generated offline; no texture uploads or PNG reads per frame.
// Mask chips carve visible organic contours INSIDE Rock cells only.
// Collision map and all physical/screen coordinate rules remain unchanged.
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <array>
#include <algorithm>
#include <cmath>

namespace sfsolo {
struct SoloEdgeSprites {
    static constexpr int tileSize=128,profiles=16,variants=3,themes=6;
    static constexpr int count=themes*profiles*variants;
    std::array<SDL_Texture*,count> tiles{};
    SoloEdgeSprites()=default;
    SoloEdgeSprites(const SoloEdgeSprites&)=delete;
    SoloEdgeSprites& operator=(const SoloEdgeSprites&)=delete;
    ~SoloEdgeSprites(){release();}
    static int index(int theme,int neighbours,int variant){
        return (std::clamp(theme,0,themes-1)*profiles+
               std::clamp(neighbours,0,profiles-1))*variants+
               std::clamp(variant,0,variants-1);
    }
    SDL_Texture *tile(int theme,int neighbours,int variant) const {
        return tiles[size_t(index(theme,neighbours,variant))];
    }
    bool ready()const {
        for(const auto *texture:tiles)if(!texture)return false;
        return true;
    }
    void release(){
        for(auto *&texture:tiles){if(texture)SDL_DestroyTexture(texture);texture=nullptr;}
    }
    bool load(SDL_Renderer *renderer,SDL_Surface *sourceSheet){
        release();
        if(!renderer||!sourceSheet)return false;
        auto *raw=IMG_Load("resources/assets/pict/solo/HabillageRebordsModeSolo.png");
        if(!raw){SDL_Log("SOLO_023 missing edge atlas: %s",IMG_GetError());return false;}
        auto *mask=SDL_ConvertSurfaceFormat(raw,SDL_PIXELFORMAT_RGBA32,0);
        SDL_FreeSurface(raw);
        auto *wall=SDL_ConvertSurfaceFormat(sourceSheet,SDL_PIXELFORMAT_RGBA32,0);
        if(!mask||!wall||mask->w!=tileSize*12||mask->h!=tileSize*4||
           wall->w<themes*256||wall->h<184){
            if(mask)SDL_FreeSurface(mask);
            if(wall)SDL_FreeSurface(wall);
            SDL_Log("SOLO_023 bad atlas/sheet dimensions");
            return false;
        }
        SDL_LockSurface(mask);SDL_LockSurface(wall);
        bool success=true;
        for(int theme=0;theme<themes&&success;++theme)
            for(int shape=0;shape<profiles&&success;++shape)
                for(int variant=0;variant<variants&&success;++variant){
                    auto *surface=SDL_CreateRGBSurfaceWithFormat(
                        0,tileSize,tileSize,32,SDL_PIXELFORMAT_RGBA32);
                    if(!surface){success=false;break;}
                    SDL_LockSurface(surface);
                    const int atlasX=(4*variant+shape%4)*tileSize;
                    const int atlasY=(shape/4)*tileSize;
                    for(int y=0;y<tileSize;++y){
                        auto *dst=reinterpret_cast<Uint32*>(
                            static_cast<Uint8*>(surface->pixels)+y*surface->pitch);
                        const auto *maskRow=reinterpret_cast<const Uint32*>(
                            static_cast<const Uint8*>(mask->pixels)+(atlasY+y)*mask->pitch);
                        const int texY=126+(int(float(y)*58.0f/(.65f*tileSize))+
                                             variant*17)%58;
                        const auto *wallRow=reinterpret_cast<const Uint32*>(
                            static_cast<const Uint8*>(wall->pixels)+texY*wall->pitch);
                        for(int x=0;x<tileSize;++x){
                            Uint8 r,g,b,a;
                            SDL_GetRGBA(maskRow[atlasX+x],mask->format,&r,&g,&b,&a);
                            // Atlas provides the carved silhouette and rock ridge
                            // luminance. Keep the colors of the ORIGINAL wall art.
                            if(!a){dst[x]=SDL_MapRGBA(surface->format,0,0,0,0);continue;}
                            const int texX=theme*256+30+
                                (int(float(x)*200.f/(2.f*tileSize))+variant*41)%200;
                            Uint8 wr,wg,wb,wa;
                            SDL_GetRGBA(wallRow[texX],wall->format,&wr,&wg,&wb,&wa);
                            const float shade=.68f+.52f*(float(r)/255.f);
                            const auto c=[shade](Uint8 v)->Uint8{
                                return Uint8(std::clamp(int(v*shade),0,255));
                            };
                            dst[x]=SDL_MapRGBA(surface->format,c(wr),c(wg),c(wb),255);
                        }
                    }
                    SDL_UnlockSurface(surface);
                    SDL_Texture *tex=SDL_CreateTextureFromSurface(renderer,surface);
                    SDL_FreeSurface(surface);
                    if(!tex){success=false;break;}
                    SDL_SetTextureBlendMode(tex,SDL_BLENDMODE_BLEND);
                    SDL_SetTextureScaleMode(tex,SDL_ScaleModeNearest);
                    tiles[size_t(index(theme,shape,variant))]=tex;
                }
        SDL_UnlockSurface(wall);SDL_UnlockSurface(mask);
        SDL_FreeSurface(wall);SDL_FreeSurface(mask);
        if(!success)release();
        SDL_Log("SOLO_023 edge tiles=%d ready=%d",count,int(success));
        return success;
    }
    // Triangulated geometry matches the exact neighbouring cell boundaries at
    // non-integer zoom/camera coordinates; no rounded SDL_RenderCopyF seams.
    static void drawCell(SDL_Renderer *renderer,SDL_Texture *texture,SDL_FRect d){
        if(!renderer||!texture)return;
        const SDL_Color white{255,255,255,255};
        const float x=d.x,y=d.y,u=d.x+d.w,v=d.y+d.h;
        const SDL_Vertex points[5]={
            {{x,y},white,{0,0}},{{u,y},white,{1,0}},
            {{u,v},white,{1,1}},{{x,v},white,{0,1}},
            {{(x+u)*.5f,(y+v)*.5f},white,{.5f,.5f}}
        };
        constexpr int indices[12]={0,1,4,1,2,4,2,3,4,3,0,4};
        SDL_RenderGeometry(renderer,texture,points,5,indices,12);
    }
};
} // namespace sfsolo
