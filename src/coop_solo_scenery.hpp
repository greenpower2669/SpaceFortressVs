#pragma once
// PARALLAX-016: presentation only. The bridge owns physics and the camera.
#include "coop_solo_world_bridge.hpp"
#include "solo_scenery_assets.hpp"
#include <array>
#include <cmath>
namespace sfsolo {
struct CoopSoloScenery {
    static constexpr float spaceDepth=.08f,mineralDepth=.34f;
    // SOLO-021: the source strip is 200x58 pixels. The prior repeat
    // (4.5 x 2.0 cells) enlarged it ~2x horizontally / ~3x vertically
    // at native zoom, visibly softening the walls before pinch-zoom.
    // ~2 x 0.65 cells retains near-native texel density at zoom 1.
    static constexpr float wallRepeatCellsX=2.0f,wallRepeatCellsY=.65f;
    SDL_Renderer *renderer=nullptr;
    SoloSceneryAssets art;
    SDL_Texture *space=nullptr,*mineral=nullptr;
    std::array<SDL_Texture*,6> material{};
    explicit CoopSoloScenery(SDL_Renderer *r):renderer(r),art(r){load();}
    CoopSoloScenery(const CoopSoloScenery&)=delete;
    CoopSoloScenery& operator=(const CoopSoloScenery&)=delete;
    ~CoopSoloScenery(){release();}
    bool ready() const {
        if(!space||!mineral)return false;
        for(auto *p:material)if(!p)return false;
        for(const auto &row:art.environmentParts)for(auto *p:row)if(!p)return false;
        return true;
    }
    void load(){
        if(!renderer)return;
        space=SoloSceneryAssets::load(renderer,"fond4hlow.png");
        mineral=SoloSceneryAssets::load(renderer,"solo/FondsParallaxeModeSolo.png");
        auto *sheet=IMG_Load("resources/assets/pict/solo/ParoisModeSolo.png");
        if(sheet){
            SDL_SetSurfaceBlendMode(sheet,SDL_BLENDMODE_NONE);
            for(int theme=0;theme<6;++theme){
                // Painted core only: no titles or concept-sheet panels.
                auto *cut=SDL_CreateRGBSurfaceWithFormat(0,200,58,32,SDL_PIXELFORMAT_RGBA32);
                if(!cut)continue;
                SDL_Rect source{theme*256+30,126,200,58};
                if(SDL_BlitSurface(sheet,&source,cut,nullptr)==0)
                    material[theme]=SDL_CreateTextureFromSurface(renderer,cut);
                SDL_FreeSurface(cut);
                if(material[theme]){
                    SDL_SetTextureBlendMode(material[theme],SDL_BLENDMODE_NONE);
                    // Nearest only on opaque foreground walls: the background
                    // planes intentionally keep their existing linear filtering.
                    SDL_SetTextureScaleMode(material[theme],SDL_ScaleModeNearest);
                }
            }
            SDL_FreeSurface(sheet);
        }
        for(auto *p:{space,mineral})if(p)SDL_SetTextureScaleMode(p,SDL_ScaleModeLinear);
        for(const auto &row:art.environmentParts)for(auto *p:row)
            if(p)SDL_SetTextureScaleMode(p,SDL_ScaleModeLinear);
        SDL_Log("SOLO_PARALLAX_016 textures=%d depths=0.08/0.34/1.00",int(ready()));
    }
    void release(){
        for(auto **p:{&space,&mineral}){if(*p)SDL_DestroyTexture(*p);*p=nullptr;}
        for(auto *&p:material){if(p)SDL_DestroyTexture(p);p=nullptr;}
        art.release();
    }
    void reload(){release();art.reload();load();}
    // Preserve logical scale/target, including the bounded lair capture.
    struct DrawState {
        SDL_Renderer *r;SDL_Rect viewport{},clip{};SDL_bool clipping;
        SDL_BlendMode blend;Uint8 red,green,blue,alpha;
        explicit DrawState(SDL_Renderer *value):r(value),clipping(SDL_RenderIsClipEnabled(r)){
            SDL_RenderGetViewport(r,&viewport);SDL_RenderGetClipRect(r,&clip);
            SDL_GetRenderDrawColor(r,&red,&green,&blue,&alpha);
            SDL_GetRenderDrawBlendMode(r,&blend);SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
        }
        ~DrawState(){
            SDL_RenderSetViewport(r,&viewport);SDL_RenderSetClipRect(r,clipping?&clip:nullptr);
            SDL_SetRenderDrawColor(r,red,green,blue,alpha);SDL_SetRenderDrawBlendMode(r,blend);
        }
    };
    static void colorRect(SDL_Renderer *r,SDL_FRect rect,SDL_Color c){
        SDL_SetRenderDrawColor(r,c.r,c.g,c.b,c.a);SDL_RenderFillRectF(r,&rect);
    }
    // Center fan avoids SDL software's rectangle-to-copy shortcut, which can
    // truncate cropped rows. No integer scissor per cell: adjacent fragments
    // share the same scaled boundary, even during high-density lair capture.
    static void tiled(SDL_Renderer *r,SDL_Texture *texture,SDL_FRect area,
                      float offsetX,float offsetY,float tileW,float tileH,
                      SDL_Color tint={255,255,255,255}){
        if(!texture||tileW<=0||tileH<=0)return;
        const int x0=int(std::floor(offsetX/tileW)),y0=int(std::floor(offsetY/tileH));
        const int x1=int(std::ceil((offsetX+area.w)/tileW)),y1=int(std::ceil((offsetY+area.h)/tileH));
        for(int y=y0;y<y1;++y)for(int x=x0;x<x1;++x){
            const float left=std::max(offsetX,x*tileW),top=std::max(offsetY,y*tileH);
            const float right=std::min(offsetX+area.w,(x+1)*tileW);
            const float bottom=std::min(offsetY+area.h,(y+1)*tileH);
            if(right<=left||bottom<=top)continue;
            float u0=std::clamp((left-x*tileW)/tileW,0.0f,1.0f);
            float u1=std::clamp((right-x*tileW)/tileW,0.0f,1.0f);
            float v0=std::clamp((top-y*tileH)/tileH,0.0f,1.0f);
            float v1=std::clamp((bottom-y*tileH)/tileH,0.0f,1.0f);
            if(x%2){u0=1-u0;u1=1-u1;}if(y%2){v0=1-v0;v1=1-v1;}
            const float sx=area.x+left-offsetX,sy=area.y+top-offsetY;
            const float ex=area.x+right-offsetX,ey=area.y+bottom-offsetY;
            const SDL_Vertex vertices[5]={{{sx,sy},tint,{u0,v0}},{{ex,sy},tint,{u1,v0}},
                {{ex,ey},tint,{u1,v1}},{{sx,ey},tint,{u0,v1}},
                {{(sx+ex)*.5f,(sy+ey)*.5f},tint,{(u0+u1)*.5f,(v0+v1)*.5f}}};
            constexpr int indices[12]={0,1,4,1,2,4,2,3,4,3,0,4};
            SDL_RenderGeometry(r,texture,vertices,5,indices,12);
        }
    }
    void drawBackground(SDL_Renderer *r,const CoopSoloWorldBridge &world,bool openings=true) const {
        if(!r||!world.ready)return;
        DrawState saved(r);
        const SDL_FRect screen{float(world.screen.x),float(world.screen.y),float(world.screen.w),float(world.screen.h)};
        colorRect(r,screen,{4,8,19,255});const float width=world.combatWidth*2.5f*world.zoom;
        tiled(r,space,screen,world.cameraLeft*spaceDepth*world.zoom,world.cameraTop*spaceDepth*world.zoom,
            width,width*1795.0f/1024.0f,{155,170,205,255});
        if(openings)tiled(r,mineral,screen,world.cameraLeft*mineralDepth*world.zoom,
            world.cameraTop*mineralDepth*world.zoom,width,width,{66,76,94,255});
    }
    static unsigned hash(int x,int y){
        unsigned value=unsigned(x)*0x9e3779b1u^unsigned(y)*0x85ebca6bu;
        value^=value>>16;value*=0x7feb352du;return value^(value>>15);
    }
    static int worldTheme(unsigned world){
        constexpr int themes[6]={0,4,1,2,3,5};return themes[(std::max(1u,world)-1)%6];
    }
    void drawWalls(SDL_Renderer *r,const CoopSoloWorldBridge &world,float seconds,bool opaqueOnly=false) const {
        if(!r||!world.ready)return;
        DrawState saved(r);const auto &map=world.map;const float physicalCell=world.pixelsPerCell,cell=physicalCell*world.zoom;
        const int x0=std::max(0,int(std::floor(world.cameraLeft/physicalCell)));
        const int y0=std::max(0,int(std::floor(world.cameraTop/physicalCell)));
        const int x1=std::min(map.width,int(std::ceil((world.cameraLeft+world.visibleWidth())/physicalCell)));
        const int y1=std::min(map.height,int(std::ceil((world.cameraTop+world.visibleHeight())/physicalCell)));
        const int theme=worldTheme(map.world);
        for(int y=y0;y<y1;++y)for(int x=x0;x<x1;++x){
            const auto at=world.toScreen({x*physicalCell,y*physicalCell});const SDL_FRect rect{at.x,at.y,cell,cell};
            const auto tile=map.at(x,y);
            if(opaqueOnly && tile!=Tile::Rock)continue;
            if(tile==Tile::Rock){
                colorRect(r,rect,{57,59,69,255});
                tiled(r,material[theme],rect,x*cell,y*cell,
                      cell*wallRepeatCellsX,cell*wallRepeatCellsY,
                      {195,200,214,255});
                // Exposed bevels stay inside the actual physical wall mass.
                // More legible lithic edges: narrower four-step bevel avoids
                // a long translucent gradient that reads as blurred geometry.
                constexpr int steps=4;const float depth=cell*.10f;
                for(int step=0;step<steps;++step){
                    const float inset=depth*step/steps,band=depth/steps;
                    const Uint8 alpha=Uint8(100*(1.0f-float(step)/steps));
                    if(map.at(x,y-1)!=Tile::Rock)
                        colorRect(r,{at.x,at.y+inset,cell,band},{155,168,190,Uint8(alpha*.55f)});
                    if(map.at(x-1,y)!=Tile::Rock)
                        colorRect(r,{at.x+inset,at.y,band,cell},{155,168,190,Uint8(alpha*.45f)});
                    if(map.at(x+1,y)!=Tile::Rock)
                        colorRect(r,{at.x+cell-inset-band,at.y,band,cell},{5,7,15,alpha});
                    if(map.at(x,y+1)!=Tile::Rock)
                        colorRect(r,{at.x,at.y+cell-inset-band,cell,band},{5,7,15,alpha});
                }
            }else if(tile==Tile::Lava||tile==Tile::Beam||tile==Tile::IceAsteroid){
                // Existing hazard decoration only; no new physical/minable actor.
                const int row=tile==Tile::Lava?1:tile==Tile::Beam?0:3;
                auto *texture=art.deco(row,int(hash(x,y)%7));
                const float pulse=.92f+.08f*std::sin(seconds*1.8f+float(hash(x,y)%61));
                const float size=cell*(tile==Tile::IceAsteroid?.65f:1.0f)*pulse;
                const SDL_FRect dst{at.x+(cell-size)*.5f,at.y+(cell-size)*.5f,size,size};
                if(texture){
                    SDL_SetTextureColorMod(texture,255,255,255);SDL_SetTextureAlphaMod(texture,185);
                    SDL_RenderCopyF(r,texture,nullptr,&dst);
                }
            }else if(tile==Tile::Trigger||tile==Tile::Finish||tile==Tile::Boss){
                const SDL_Color c=tile==Tile::Trigger?SDL_Color{65,205,176,70}:
                    tile==Tile::Finish?SDL_Color{90,240,125,65}:SDL_Color{245,110,210,60};
                colorRect(r,{at.x+cell*.2f,at.y+cell*.2f,cell*.6f,cell*.6f},c);
            }
        }
    }
    void draw(SDL_Renderer *r,const CoopSoloWorldBridge &world,float seconds) const {
        drawBackground(r,world);drawWalls(r,world,seconds);
    }
};
} // namespace sfsolo
