#pragma once
// The SOLO world now uses the actual classic SpaceFortress fortress and
// existing photo-realistic space/rock assets. Colored blocks are fallbacks
// only if an asset cannot load; final COOP rendering integration stays next.
#include "solo_session.hpp"
#include "solo_viewport.hpp"
#include "solo_combat.hpp"
#include "solo_kinetic_control.hpp"
#include "solo_bitmap_font.hpp"
#include "solo_scenery.hpp"
#include <array>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <algorithm>
#include <cmath>
#include <string>

namespace sfsolo {
inline SDL_Color tileColor(Tile tile){
    switch(tile){
        case Tile::Empty:return {9,13,28,255};
        case Tile::Rock:return {64,78,101,255};
        case Tile::Lava:return {255,93,32,255};
        case Tile::IceAsteroid:return {212,242,255,255};
        case Tile::Asteroid:return {170,181,195,255};
        case Tile::Beam:return {145,73,236,255};
        case Tile::Enemy:return {240,82,67,255};
        case Tile::Boss:return {255,47,176,255};
        case Tile::Start:return {55,135,255,255};
        case Tile::Finish:return {54,239,112,255};
        case Tile::Trigger:return {255,227,115,255};
    }
    return {0,0,0,255};
}
inline void fill(SDL_Renderer *r,SDL_Rect rect,SDL_Color c){
    SDL_SetRenderDrawColor(r,c.r,c.g,c.b,c.a);
    SDL_RenderFillRect(r,&rect);
}
struct SoloAssets {
    SDL_Texture *ship=nullptr,*enemy=nullptr,*rock=nullptr,*space=nullptr;
    SDL_Texture *projectile=nullptr,*missile=nullptr,*energy=nullptr;
    SDL_Texture *plasma=nullptr,*spark=nullptr,*explosion=nullptr,*shield=nullptr;
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
    explicit SoloAssets(SDL_Renderer *r):renderer(r){
        if(!renderer)return;
        ship=load(r,"sb.png");
        enemy=load(r,"so.png");
        rock=load(r,"ro1.png");
        space=load(r,"fond4hlow.png");
        projectile=load(r,"orbeb.png");
        missile=load(r,"missilebb.png");
        energy=load(r,"pousb.png");
        plasma=load(r,"energie.png");
        spark=load(r,"pousc.png");
        explosion=load(r,"explo.png");
        shield=load(r,"shield.png");
        buildScenery(r);
        SDL_Log("SOLO original assets: ship=%d enemy=%d rock=%d space=%d",
                ship!=nullptr,enemy!=nullptr,rock!=nullptr,space!=nullptr);
    }
    ~SoloAssets(){
        for(SDL_Texture *p:{ship,enemy,rock,space,projectile,missile,energy,
                            plasma,spark,explosion,shield})
            if(p)SDL_DestroyTexture(p);
        for(const auto &row:wallParts)for(SDL_Texture *p:row)
            if(p)SDL_DestroyTexture(p);
        for(const auto &row:environmentParts)for(SDL_Texture *p:row)
            if(p)SDL_DestroyTexture(p);
    }
    SoloAssets(const SoloAssets&)=delete;
    SoloAssets& operator=(const SoloAssets&)=delete;
};
inline void soloCopy(SDL_Renderer *r,SDL_Texture *texture,SDL_Rect destination,
                     SDL_Color tint={255,255,255,255},double angle=0){
    if(!texture)return;
    SDL_SetTextureColorMod(texture,tint.r,tint.g,tint.b);
    SDL_SetTextureAlphaMod(texture,tint.a);
    SDL_RenderCopyEx(r,texture,nullptr,&destination,angle,nullptr,SDL_FLIP_NONE);
}
// Tile signatures drive visual themes without modifying their physics.
inline int soloWallTheme(const Session &s,int x,int y){
    for(int dy=-2;dy<=2;++dy)for(int dx=-2;dx<=2;++dx){
        const auto near=s.map.at(x+dx,y+dy);
        if(near==Tile::Lava)return 1;
        if(near==Tile::Beam)return 2;
        if(near==Tile::IceAsteroid)return 4;
    }
    const unsigned variant=unsigned(x*92821u+y*68917u);
    return variant%19u==0u?3:(variant%31u==0u?5:0);
}
// Paint real silhouette cut-outs, not a crop from the full presentation.
inline void soloDrawCutSprite(SDL_Renderer *r,SDL_Texture *sprite,
                              SDL_Rect dst,double rotation=0,Uint8 alpha=255,
                              SDL_RendererFlip flip=SDL_FLIP_NONE){
    if(!sprite)return;
    SDL_SetTextureAlphaMod(sprite,alpha);
    SDL_SetTextureColorMod(sprite,255,255,255);
    SDL_RenderCopyEx(r,sprite,nullptr,&dst,rotation,nullptr,flip);
}
inline int soloWallSpriteIndex(SoloWallPresentation presentation){
    switch(presentation.shape){
        case SoloWallShape::Horizontal:return int(presentation.variant); // 3 originals
        case SoloWallShape::Vertical:return 3;  // real vertical sample
        case SoloWallShape::Corner:return 4;    // real L corner
        case SoloWallShape::Tee:return 6;       // rock junction
        case SoloWallShape::Cross:return 6;
        case SoloWallShape::End:return 7;
        default:return 7;
    }
}
inline void soloDrawAmbience(SDL_Renderer *r,const Session &s,
                             const SoloViewport &camera,SoloAssets *assets,
                             int x,int y,int theme,int cx,int cy,float cell,
                             bool anchoredToWall){
    if(!assets)return;
    const unsigned h=soloWorldDecorHash(x,y,uint32_t(theme)+0xD336u);
    const int atlasTheme=soloEnvironmentArtTheme(theme);
    // A rock wall is NOT a screen for large floating thumbnails:
    // decorate sparingly, except actually energetic/lava surfaces.
    const bool active=theme==1||theme==2||theme==4||theme==5;
    if(!active && (h%29u)!=0u)return;
    if(active && (h%4u)>1u && anchoredToWall)return;
    const int frame=int(h%7u); // stable *identity*, not an animation frame
    SDL_Texture *sprite=assets->deco(atlasTheme,frame);
    if(!sprite)return;
    const float phase=soloDecoPhase(x,y,theme);
    const float t=s.seconds;
    // NO random frame changes: soft brightness/scale/float animation only.
    const float pulse=std::sin(t*(theme==5?.93f:theme==1?1.26f:1.73f)+phase);
    const float opacity=std::clamp((active?.33f:.17f)+pulse*.10f,.07f,.48f);
    const float relativeSize=(anchoredToWall?.53f:.74f)*
                              (1.0f+.035f*pulse);
    const int width=std::max(8,int(cell*relativeSize*.82f));
    const int height=std::max(8,int(cell*relativeSize));
    const int floatY=theme==5?int(std::sin(t*.56f+phase)*cell*.055f):0;
    soloDrawCutSprite(r,sprite,{cx-width/2,cy-height/2+floatY,width,height},
                      0,Uint8(255*opacity));
}
// Original shield art is an impact FLASH, not a simulated COOP shield.
inline Uint8 soloImpactFlashAlpha(const Session &s){
    if(s.phase==Phase::Lost||s.phase==Phase::Won)return 0;
    return Uint8(160.0f*std::clamp(s.collisionCooldown/.45f,0.0f,1.0f));
}
inline void drawSession(SDL_Renderer *r,const Session &s,SDL_Rect viewport,
                        SoloAssets *assets=nullptr){
    if(!r||!s.map.valid()||viewport.w<=0||viewport.h<=0)return;
    fill(r,viewport,{3,8,23,255});
    const SoloViewport camera=soloViewport(s,viewport);
    const float cell=camera.cell;
    const int firstX=std::max(0,int(std::floor(camera.left)));
    const int lastX=std::min(s.map.width,int(std::ceil(camera.left+camera.visibleColumns()))+1);
    const int firstY=std::max(0,int(std::floor(camera.top)));
    const int lastY=std::min(s.map.height,int(std::ceil(camera.top+camera.visibleRows()))+1);
    if(assets&&assets->space){
        int sw=0,sh=0;SDL_QueryTexture(assets->space,nullptr,nullptr,&sw,&sh);
        // Scenic parallax tracks the real camera, not an arbitrary forced
        // vertical autoscroll; going backwards restores previously seen skies.
        if(sw>0&&sh>0){
            const int cropW=sw,cropH=std::max(16,std::min(sh,int(sw*viewport.h/
                                    float(std::max(1,viewport.w)))));
            const int maxTop=std::max(0,sh-cropH);
            const float progress=std::clamp(camera.top/float(std::max(1,s.map.height)),
                                            0.0f,1.0f);
            SDL_Rect source{0,int(progress*maxTop),cropW,cropH};
            SDL_SetTextureColorMod(assets->space,125,144,177);
            SDL_SetTextureAlphaMod(assets->space,155);
            SDL_RenderCopy(r,assets->space,&source,&viewport);
        }
    }
    SDL_SetRenderDrawColor(r,90,128,176,255);
    for(int y=firstY;y<lastY;++y)for(int x=firstX;x<lastX;++x){
        const unsigned hash=unsigned(x*92821u+y*68917u+2167u);
        if(hash%53u==0u){
            SDL_RenderDrawPoint(r,camera.pixelX(x+.23f),camera.pixelY(y+.64f));
        }
    }
    const float now=s.seconds;
    for(int y=firstY;y<lastY;++y)for(int x=firstX;x<lastX;++x){
        const Tile t=s.map.at(x,y);
        if(t==Tile::Empty)continue;
        const int px=camera.pixelX(float(x)),py=camera.pixelY(float(y));
        const SDL_Rect square{px,py,std::max(2,int(cell)+1),std::max(2,int(cell)+1)};
        const int cx=camera.pixelX(x+.5f),cy=camera.pixelY(y+.5f);
        const int radius=std::max(6,int(cell*.40f));
        const unsigned hash=unsigned(x*22343u+y*65519u);
        if(t==Tile::Rock || t==Tile::Lava || t==Tile::Beam){
            const int theme=t==Tile::Lava?1:t==Tile::Beam?2:
                                      soloWallTheme(s,x,y);
            const auto selection=soloWallPresentation(s,x,y);
            const int which=soloWallSpriteIndex(selection);
            SDL_Texture *piece=assets?assets->wall(theme,which):nullptr;
            if(piece){
                // Exact stable orientation from wall adjacency N/E/S/W.
                // VERTICAL strips and corner geometry are genuinely different
                // images, not stretched horizontal source sheet rectangles.
                const SDL_Rect dst{px-2,py-2,int(cell)+5,int(cell)+5};
                const SDL_RendererFlip mirror=(selection.variant==2u)?
                    SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE;
                soloDrawCutSprite(r,piece,dst,selection.rotation,255,mirror);
                // Decorations have no collision and do not determine wall
                // orientation, color, or stable body texture.
                soloDrawAmbience(r,s,camera,assets,x,y,theme,cx,cy,cell,true);
            }else{
                fill(r,square,tileColor(t));
                if(assets&&assets->rock)soloCopy(r,assets->rock,square);
            }
            continue;
        }
        if(t==Tile::Asteroid || t==Tile::IceAsteroid){
            // Actual rock sprite shrinks as the held cone melts its mass.
            const int reduced=std::max(2,int(radius*s.asteroidIntegrityAt(x,y)));
            // Distinct transparent asteroid/ice cut-out, never a vignette.
            SDL_Texture *shape=assets?assets->deco(
                t==Tile::IceAsteroid?3:2,
                int(soloWorldDecorHash(x,y,0xA511u)%7u)):nullptr;
            if(shape){
                soloDrawCutSprite(r,shape,
                    {cx-reduced,cy-reduced,reduced*2,reduced*2});
            }else if(assets&&assets->rock){
                const SDL_Color tint=t==Tile::IceAsteroid?
                    SDL_Color{158,233,255,255}:SDL_Color{215,218,229,255};
                soloCopy(r,assets->rock,{cx-reduced,cy-reduced,reduced*2,reduced*2},
                         tint,double(hash%8u)*45.0);
            }else{
                fill(r,{cx-reduced,cy-reduced,reduced*2,reduced*2},tileColor(t));
            }
            if(t==Tile::IceAsteroid){
                SDL_SetRenderDrawColor(r,220,253,255,255);
                SDL_RenderDrawLine(r,cx-reduced,cy,cx+reduced,cy);
                SDL_RenderDrawLine(r,cx,cy-reduced,cx,cy+reduced);
            }
            continue;
        }
        if(t==Tile::Enemy||t==Tile::Boss){
            const int size=t==Tile::Boss?int(cell*2.0f):int(cell*.95f);
            const SDL_Rect dest{cx-size/2,cy-size/2,size,size};
            if(assets&&assets->enemy)soloCopy(r,assets->enemy,dest,
                t==Tile::Boss?SDL_Color{255,116,176,255}:
                              SDL_Color{255,192,126,255},
                t==Tile::Boss?std::sin(now*2)*4:0);
            else fill(r,dest,tileColor(t));
            continue;
        }
        if(t==Tile::Trigger){
            const int beat=std::max(3,int((.27f+std::sin(now*3.3f)*.06f)*cell));
            fill(r,{cx-beat,cy-beat,beat*2,beat*2},{40,50,75,255});
            SDL_SetRenderDrawColor(r,255,232,116,255);
            SDL_RenderDrawLine(r,cx,cy-beat*2,cx+beat*2,cy);
            SDL_RenderDrawLine(r,cx+beat*2,cy,cx,cy+beat*2);
            SDL_RenderDrawLine(r,cx,cy+beat*2,cx-beat*2,cy);
            SDL_RenderDrawLine(r,cx-beat*2,cy,cx,cy-beat*2);
            continue;
        }
        if(t==Tile::Finish){
            SDL_SetRenderDrawColor(r,s.missionReady()?100:220,
                                  s.missionReady()?255:110,166,255);
            const int rr=int(cell*1.25f);
            SDL_Rect exit{cx-rr,cy-rr,rr*2,rr*2};
            SDL_RenderDrawRect(r,&exit);
        }
    }
    for(const auto &particle:s.dust){
        const int dx=camera.pixelX(particle.x),dy=camera.pixelY(particle.y);
        if(dx<viewport.x||dx>=viewport.x+viewport.w||
           dy<viewport.y||dy>=viewport.y+viewport.h)continue;
        const int diameter=std::max(6,int(cell*.22f));
        if(assets&&assets->energy)soloCopy(r,assets->energy,
            {dx-diameter/2,dy-diameter/2,diameter,diameter},
            particle.icy?SDL_Color{185,244,255,220}:
                         SDL_Color{250,238,200,220});
        else fill(r,{dx-2,dy-2,5,5},{230,253,255,255});
    }
    const int shipX=camera.pixelX(s.pilot.x);
    const int shipY=camera.pixelY(s.pilot.y);
    // Graphic ship size intentionally remains EXACTLY the old one: zoom x3
    // applies to terrain only. The pilot's world collision radius is intact.
    const int radius=soloShipRadiusPixels(viewport);
    const int pulse=std::max(1,int(float(viewport.w)/18.0f*.09f));
    SDL_SetRenderDrawColor(r,58,204,250,255);
    for(int k=0;k<2;++k){
        SDL_Rect glow{shipX-radius-pulse-k*2,shipY-radius-pulse-k*2,
                     2*(radius+pulse+k*2),2*(radius+pulse+k*2)};
        SDL_RenderDrawRect(r,&glow);
    }
    // Movement trail uses ship velocity, never camera-pan velocity.
    const float speed=std::hypot(s.pilot.vx,s.pilot.vy);
    if(assets&&assets->spark&&speed>.35f){
        const float vx=s.pilot.vx/speed,vy=s.pilot.vy/speed;
        const int tailX=shipX-int(vx*radius*1.2f);
        const int tailY=shipY-int(vy*radius*1.2f);
        const int size=std::max(5,radius*3/4);
        soloCopy(r,assets->spark,{tailX-size/2,tailY-size/2,size,size},
                 {109,218,255,210},s.seconds*110.0);
    }
    if(s.canonicalShipActive&&assets&&assets->shield&&s.energyFraction()>.001f){
        const int rr=std::max(radius+2,int(radius*1.7f));
        soloCopy(r,assets->shield,{shipX-rr,shipY-rr,2*rr,2*rr},
                 {134,209,255,Uint8(52+75*s.energyFraction())});
    }
    if(assets&&assets->ship)soloCopy(r,assets->ship,
        {shipX-radius,shipY-radius,radius*2,radius*2});
    else{
        SDL_SetRenderDrawColor(r,92,237,255,255);
        SDL_RenderDrawLine(r,shipX,shipY-radius,shipX-radius,shipY+radius);
        SDL_RenderDrawLine(r,shipX-radius,shipY+radius,shipX+radius,shipY+radius);
        SDL_RenderDrawLine(r,shipX+radius,shipY+radius,shipX,shipY-radius);
    }
    if(s.canonicalShipActive){
        // Shared kinetic wave timing and radius (not an arbitrary pulse).
        SDL_BlendMode original;SDL_GetRenderDrawBlendMode(r,&original);
        SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
        for(const auto &wave:sfKineticWaves){
            if(wave.owner!=1||!sfKineticWaveAlive(wave))continue;
            const float distance=sfKineticWaveRadiusAt(
                wave,s.shipRadius*2.0f,wave.age)*camera.cell;
            if(distance<1.0f)continue;
            constexpr int edges=36;
            const Uint8 alpha=Uint8(170.0f*std::max(0.0f,
                1.0f-sfKineticWaveProgressAt(wave,wave.age)));
            SDL_SetRenderDrawColor(r,94,204,255,alpha);
            for(int i=0;i<edges;++i){
                const float a=i*6.2831853f/edges,b=(i+1)*6.2831853f/edges;
                SDL_RenderDrawLine(r,shipX+int(std::cos(a)*distance),
                    shipY+int(std::sin(a)*distance),
                    shipX+int(std::cos(b)*distance),
                    shipY+int(std::sin(b)*distance));
            }
        }
        SDL_SetRenderDrawBlendMode(r,original);
    }
    const Uint8 impactAlpha=soloImpactFlashAlpha(s);
    if(impactAlpha && assets&&assets->shield){
        const int shell=radius*2;
        soloCopy(r,assets->shield,
                 {shipX-shell,shipY-shell,shell*2,shell*2},
                 {170,225,255,impactAlpha});
    }
    if(s.slowActive()){
        SDL_SetRenderDrawColor(r,s.iceActive()?209:255,s.iceActive()?247:125,
                              s.iceActive()?255:62,255);
        SDL_Rect halo{shipX-radius-8,shipY-radius-8,2*radius+16,2*radius+16};
        SDL_RenderDrawRect(r,&halo);
    }
    const int hudY=viewport.y+std::max(58,viewport.h/19);
    const int barW=std::max(80,viewport.w*34/100);
    fill(r,{viewport.x+14,hudY,barW,13},{94,36,54,255});
    fill(r,{viewport.x+14,hudY,int(barW*std::clamp(s.pilot.health/100,0.0f,1.0f)),13},
         {90,231,147,255});
    if(s.canonicalShipActive){
        fill(r,{viewport.x+14,hudY+17,barW,10},{24,27,62,228});
        fill(r,{viewport.x+14,hudY+17,
                int(barW*std::clamp(s.energyFraction(),0.0f,1.0f)),10},
             s.energyFraction()<.10f?SDL_Color{255,68,42,255}:
             SDL_Color{165,128,255,255});
    }
    const std::string goal="RELAIS "+std::to_string(s.relaysCollected)+"/"+
                            std::to_string(s.relaysNeeded);
    soloLabel(r,goal,{viewport.x+12,hudY+30,barW+28,34},
              {255,229,129,255},std::max(2,viewport.w/175));
    if(s.slowActive()){
        const SDL_Rect tag{viewport.x+12,hudY+54,
                           std::max(140,viewport.w*2/5),44};
        fill(r,tag,s.iceActive()?SDL_Color{26,97,140,255}:
                                  SDL_Color{156,66,27,255});
        soloLabel(r,s.iceActive()?"GLACE":"LAVE",tag,
                  {255,255,255,255},std::max(3,viewport.w/170));
    }
    // Exploration map: corridors stay revealed when traversed backwards.
    const int mw=std::max(77,viewport.w/5),mh=std::max(128,viewport.h/5);
    SDL_Rect mini{viewport.x+viewport.w-mw-10,hudY,mw,mh};
    fill(r,mini,{15,22,40,240});
    const float mx=float(mw-4)/s.map.width,my=float(mh-4)/s.map.height;
    SDL_SetRenderDrawColor(r,75,108,146,255);
    for(int y=0;y<s.map.height;y+=2)for(int x=0;x<s.map.width;x+=2){
        if(s.explored.empty() || !s.explored[size_t(y)*s.map.width+x])continue;
        const Tile tile=s.map.at(x,y);
        if(tile==Tile::Rock)continue;
        SDL_Rect dot{mini.x+2+int(x*mx),mini.y+2+int(y*my),
                     std::max(1,int(mx*2)),std::max(1,int(my*2))};
        SDL_RenderFillRect(r,&dot);
    }
    const auto p=miniMapPosition(s);
    fill(r,{mini.x+int(p.x*mw)-3,mini.y+int(p.y*mh)-3,8,8},
         {104,240,255,255});
    for(int y=0;y<s.map.height;++y)for(int x=0;x<s.map.width;++x){
        if(s.map.at(x,y)!=Tile::Trigger)continue;
        if(s.explored.empty()||!s.explored[size_t(y)*s.map.width+x])continue;
        fill(r,{mini.x+int(float(x)/s.map.width*mw)-2,
                mini.y+int(float(y)/s.map.height*mh)-2,5,5},
             {255,222,100,255});
    }
}
inline void drawMissionOverlay(SDL_Renderer *r,const Session &s,
                               SDL_Rect viewport) {
    if(!r || s.phase!=Phase::Lost)return;
    const int wide=std::max(200,viewport.w*85/100);
    const SDL_Rect modal{viewport.x+(viewport.w-wide)/2,
        viewport.y+viewport.h*42/100,wide,viewport.h*17/100};
    fill(r,modal,{19,25,45,250});
    SDL_SetRenderDrawColor(r,255,111,111,255);
    SDL_RenderDrawRect(r,&modal);
    soloLabel(r,"VAISSEAU DETRUIT",
              {modal.x+10,modal.y+10,modal.w-20,modal.h/3},
              {255,186,171,255},std::max(3,viewport.w/120));
    soloLabel(r,"TAP = RECOMMENCER",
              {modal.x+10,modal.y+modal.h/2,modal.w-20,modal.h/3},
              {255,255,255,255},std::max(3,viewport.w/135));
}
inline void drawKineticCharge(SDL_Renderer *r,const Session &s,SDL_Rect viewport,
                              const KineticSurgeControl &kinetic,
                              SoloAssets *assets=nullptr){
    if(!r||viewport.w<=0||viewport.h<=0)return;
    const auto &surge=sfKineticSurges[1];
    const bool charging=surge.held;
    const bool pulse=kinetic.burstSeconds>0;
    if(!charging && !pulse)return;
    if(charging){
        const float progress=std::clamp(
            surge.heldSeconds/SF_KINETIC_SURGE_HOLD_SECONDS,0.0f,1.0f);
        const int barW=std::max(70,viewport.w/2);
        const int barH=std::max(12,viewport.h/65);
        const int barX=viewport.x+(viewport.w-barW)/2;
        const int barY=viewport.y+viewport.h-barH-60;
        fill(r,{barX-3,barY-3,barW+6,barH+6},{255,255,255,255});
        fill(r,{barX,barY,barW,barH},{25,35,57,255});
        fill(r,{barX,barY,int(barW*progress),barH},
             surge.charged?SDL_Color{103,255,166,255}:
                           SDL_Color{255,216,95,255});
    }
    if(!sfKineticSurgeVisible(1) && !pulse)return;
    const SoloViewport camera=soloViewport(s,viewport);
    const float originX=pulse?kinetic.burstOriginX:s.pilot.x;
    const float originY=pulse?kinetic.burstOriginY:s.pilot.y;
    const int px=camera.pixelX(originX),py=camera.pixelY(originY);
    const AimVector d=pulse?kinetic.burstAxis:kinetic.direction(s);
    const float range=coneRangeForShip(s.shipRadius);
    const float half=sfKineticSurgeConeHalfWidth(range);
    const int forwardX=int(range*camera.cell*d.x);
    const int forwardY=int(range*camera.cell*d.y);
    const int sideX=int(half*camera.cell*-d.y);
    const int sideY=int(half*camera.cell*d.x);
    const int ex=px+forwardX,ey=py+forwardY;
    const float opacity=pulse?std::clamp(kinetic.burstSeconds/.42f,0.0f,1.0f):.8f;

    // Real original plasma/dust assets make a volumetric-looking 360-degree
    // blast rather than the old three-line outline. The *hit footprint* is
    // still supplied by exactly the canonical kinetic_shield.hpp geometry.
    for(int along=2;along<21;++along){
        const float fraction=float(along)/21.0f;
        const float forward=range*fraction;
        const float width=sfKineticSurgeConeHalfWidth(forward);
        for(int strand=-2;strand<=2;++strand){
            const float offset=float(strand)/2.2f;
            const float travel=forward+std::sin(s.seconds*21+along*7+strand)*.018f;
            const float wx=originX+d.x*travel-d.y*width*offset;
            const float wy=originY+d.y*travel+d.x*width*offset;
            const int sx=camera.pixelX(wx),sy=camera.pixelY(wy);
            const int diameter=std::max(4,int(camera.cell*
                  (.17f+.11f*(1.0f-fraction))));
            const Uint8 alpha=Uint8(200*opacity*(.65f+.35f*(1.f-fraction)));
            if(assets && (assets->plasma||assets->spark)){
                SDL_Texture *texture=(along+strand)%3?
                                       assets->plasma:assets->spark;
                if(!texture)texture=assets->plasma?
                                       assets->plasma:assets->spark;
                soloCopy(r,texture,{sx-diameter/2,sy-diameter/2,
                                   diameter,diameter},
                         {145,236,255,alpha});
            }else{
                fill(r,{sx-2,sy-2,5,5},{139,231,255,alpha});
            }
        }
    }
    SDL_SetRenderDrawColor(r,131,235,255,Uint8(235*opacity));
    SDL_BlendMode previous=SDL_BLENDMODE_NONE;
    SDL_GetRenderDrawBlendMode(r,&previous);
    SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
    SDL_RenderDrawLine(r,px,py,ex+sideX,ey+sideY);
    SDL_RenderDrawLine(r,px,py,ex-sideX,ey-sideY);
    SDL_RenderDrawLine(r,ex-sideX,ey-sideY,ex+sideX,ey+sideY);
    SDL_SetRenderDrawBlendMode(r,previous);
}
inline void drawCombat(SDL_Renderer *r,const Session &s,const Combat &combat,
                       SDL_Rect viewport,SoloAssets *assets=nullptr){
    if(!r||!s.map.valid()||viewport.w<=0||viewport.h<=0)return;
    const SoloViewport camera=soloViewport(s,viewport);
    for(const auto &shot:combat.shots){
        const int px=camera.pixelX(shot.x),py=camera.pixelY(shot.y);
        if(px<viewport.x||px>=viewport.x+viewport.w||
           py<viewport.y||py>=viewport.y+viewport.h)continue;
        const int d=std::max(7,int(camera.cell*(shot.missile?.57f:.35f)));
        SDL_Texture *sprite=assets?
            (shot.missile?assets->missile:assets->projectile):nullptr;
        if(sprite)
            soloCopy(r,sprite,{px-d/2,py-d/2,d,d},{255,255,255,255},
                     shot.missile?
                     std::atan2(shot.vy,shot.vx)*180.0/3.14159265+90.0:0.0);
        else fill(r,{px-d/2,py-d/2,d,d},{255,236,114,255});
    }
    for(const auto &hit:combat.impacts){
        const int px=camera.pixelX(hit.x),py=camera.pixelY(hit.y);
        if(px<viewport.x-80||px>viewport.x+viewport.w+80||
           py<viewport.y-80||py>viewport.y+viewport.h+80)continue;
        const float progress=1.0f-std::clamp(hit.life/.36f,0.0f,1.0f);
        const int diameter=std::max(10,int(camera.cell*(.5f+progress*.85f)));
        if(assets&&assets->explosion)
            soloCopy(r,assets->explosion,
                     {px-diameter/2,py-diameter/2,diameter,diameter},
                     hit.frozen?SDL_Color{154,237,255,
                         Uint8(230*(1-progress))}:
                         SDL_Color{255,165,98,Uint8(230*(1-progress))});
        else{
            SDL_Rect blast{px-diameter/2,py-diameter/2,diameter,diameter};
            SDL_SetRenderDrawColor(r,255,hit.frozen?220:130,
                                   hit.frozen?255:70,Uint8(220*(1-progress)));
            SDL_RenderDrawRect(r,&blast);
        }
    }
    if(combat.bossHealth<combat.bossMaxHealth&&!s.bossDefeated){
        const int width=std::max(1,viewport.w/2);
        const int x=viewport.x+(viewport.w-width)/2;
        const int y=viewport.y+std::max(62,viewport.h/19)+26;
        fill(r,{x,y,width,11},{67,26,57,255});
        fill(r,{x,y,int(width*std::clamp(combat.bossHealth/
             std::max(1.0f,combat.bossMaxHealth),0.0f,1.0f)),11},
             {255,77,170,255});
    }
}
} // namespace sfsolo
