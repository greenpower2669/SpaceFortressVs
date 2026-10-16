#pragma once
// Renderer-owned atlas slices shared by the prototype and authentic COOP SOLO.
#include "solo_scenery_atlas.hpp"
#include <SDL2/SDL_image.h>
#include <array>
#include <string>
namespace sfsolo {
struct SoloSceneryAssets {
    // Pre-sliced transparent sprites. Never draw a source atlas frame as a tile.
    std::array<std::array<SDL_Texture*,8>,6> wallParts{};
    std::array<std::array<SDL_Texture*,7>,5> environmentParts{};
    SDL_Renderer *renderer=nullptr;
    static SDL_Texture *load(SDL_Renderer *renderer,const char *file){
        // SDL_RWFromFile supports Android APK assets via SDL's asset manager.
        // The asset pack is the same one packaged for the historical modes.
        const std::string path=std::string("resources/assets/pict/")+file;
        SDL_Texture *texture=IMG_LoadTexture(renderer,path.c_str());
        if(texture)SDL_SetTextureBlendMode(texture,SDL_BLENDMODE_BLEND);
        return texture;
    }
    // A concept sheet is not an atlas of ready-to-render 1x1 tiles.
    // Pre-slice ONLY painted asset rectangles and key out the background
    // ONCE, before SDL creates textures (no per-frame expensive processing).
    static SDL_Texture *extractSprite(SDL_Renderer *r,SDL_Surface *sheet,
                                      SDL_Rect source,bool wall){
        if(!r||!sheet||source.x<0||source.y<0||
           source.x+source.w>sheet->w||source.y+source.h>sheet->h)return nullptr;
        const int width=128,height=wall?128:160;
        SDL_Surface *cut=SDL_CreateRGBSurfaceWithFormat(
            0,width,height,32,SDL_PIXELFORMAT_RGBA32);
        if(!cut)return nullptr;
        SDL_SetSurfaceBlendMode(sheet,SDL_BLENDMODE_NONE);
        SDL_Rect target{0,0,width,height};
        if(SDL_BlitScaled(sheet,&source,cut,&target)!=0){
            SDL_FreeSurface(cut);return nullptr;
        }
        SDL_LockSurface(cut);
        for(int y=0;y<height;++y){
            auto *pixels=reinterpret_cast<Uint32*>(
                static_cast<Uint8*>(cut->pixels)+y*cut->pitch);
            for(int x=0;x<width;++x){
                Uint8 red,green,blue,alpha;
                SDL_GetRGBA(pixels[x],cut->format,&red,&green,&blue,&alpha);
                // Source panel navy/black is semitransparent. Preserve actual
                // dark rock instead of using the old destructive RGB key.
                if(wall){
                    const float coverage=std::clamp((float(alpha)-164.0f)/75.0f,
                                                     0.0f,1.0f);
                    alpha=Uint8(alpha*coverage);
                }
                // Feather crop edges: energy clouds never have hard clipped
                // square borders, even when adjacent atlas objects overlap.
                const int edge=std::min({x,y,width-x-1,height-y-1});
                const float fade=std::clamp(float(edge)/(wall?3.0f:11.0f),
                                             0.0f,1.0f);
                alpha=Uint8(alpha*fade);
                pixels[x]=SDL_MapRGBA(cut->format,red,green,blue,alpha);
            }
        }
        SDL_UnlockSurface(cut);
        SDL_Texture *result=SDL_CreateTextureFromSurface(r,cut);
        SDL_FreeSurface(cut);
        if(result)SDL_SetTextureBlendMode(result,SDL_BLENDMODE_BLEND);
        return result;
    }
    void buildScenery(SDL_Renderer *r){
        const std::string base="resources/assets/pict/solo/";
        SDL_Surface *walls=IMG_Load((base+"ParoisModeSolo.png").c_str());
        SDL_Surface *effects=IMG_Load((base+"EnvironnementsModeSolo.png").c_str());
        if(walls){
            SDL_Surface *rgba=SDL_ConvertSurfaceFormat(
                walls,SDL_PIXELFORMAT_RGBA32,0);SDL_FreeSurface(walls);
            if(rgba){
                for(int theme=0;theme<6;++theme)
                    for(int index=0;index<8;++index)
                        wallParts[theme][index]=extractSprite(
                            r,rgba,soloWallArtBox(theme,index),true);
                SDL_FreeSurface(rgba);
            }
        }
        if(effects){
            SDL_Surface *rgba=SDL_ConvertSurfaceFormat(
                effects,SDL_PIXELFORMAT_RGBA32,0);SDL_FreeSurface(effects);
            if(rgba){
                for(int theme=0;theme<5;++theme)
                    for(int variant=0;variant<7;++variant)
                        environmentParts[theme][variant]=extractSprite(
                            r,rgba,soloEnvironmentArtBox(theme,variant),false);
                SDL_FreeSurface(rgba);
            }
        }
        SDL_Log("SOLO cut scenery: rock=%d lava=%d energy=%d ice=%d",
           wallParts[0][0]!=nullptr,wallParts[1][0]!=nullptr,
           environmentParts[0][0]!=nullptr,environmentParts[3][0]!=nullptr);
    }
    SDL_Texture *wall(int theme,int index) const {
        return wallParts[std::clamp(theme,0,5)][std::clamp(index,0,7)];
    }
    SDL_Texture *deco(int atlasRow,int index) const {
        return environmentParts[std::clamp(atlasRow,0,4)]
                               [std::clamp(index,0,6)];
    }
    explicit SoloSceneryAssets(SDL_Renderer *r):renderer(r){if(r)buildScenery(r);}
    void release(){
        for(auto &row:wallParts)for(auto *&p:row){if(p)SDL_DestroyTexture(p);p=nullptr;}
        for(auto &row:environmentParts)for(auto *&p:row){if(p)SDL_DestroyTexture(p);p=nullptr;}
    }
    void reload(){release();if(renderer)buildScenery(renderer);}
    ~SoloSceneryAssets(){release();}
    SoloSceneryAssets(const SoloSceneryAssets&)=delete;
    SoloSceneryAssets& operator=(const SoloSceneryAssets&)=delete;
};
} // namespace sfsolo
