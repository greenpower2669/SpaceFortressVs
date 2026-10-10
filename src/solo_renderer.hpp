#pragma once
// The SOLO world now uses the actual classic SpaceFortress fortress and
// existing photo-realistic space/rock assets. Colored blocks are fallbacks
// only if an asset cannot load; final COOP rendering integration stays next.
#include "solo_session.hpp"
#include "solo_viewport.hpp"
#include "solo_combat.hpp"
#include "solo_kinetic_control.hpp"
#include "solo_bitmap_font.hpp"
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
        case Tile::Lava:return {239,93,32,255};
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
    SDL_Texture *projectile=nullptr,*energy=nullptr;
    SDL_Renderer *renderer=nullptr;
    static SDL_Texture *load(SDL_Renderer *renderer,const char *file){
        // SDL_RWFromFile supports Android APK assets via SDL's asset manager.
        // The asset pack is the same one packaged for the historical modes.
        const std::string path=std::string("resources/assets/pict/")+file;
        SDL_Texture *texture=IMG_LoadTexture(renderer,path.c_str());
        if(texture)SDL_SetTextureBlendMode(texture,SDL_BLENDMODE_BLEND);
        return texture;
    }
    explicit SoloAssets(SDL_Renderer *r):renderer(r){
        if(!renderer)return;
        ship=load(r,"sb.png");
        enemy=load(r,"so.png");
        rock=load(r,"ro1.png");
        space=load(r,"fond4hlow.png");
        projectile=load(r,"tirj1.png");
        energy=load(r,"pousb.png");
        SDL_Log("SOLO original assets: ship=%d enemy=%d rock=%d space=%d",
                ship!=nullptr,enemy!=nullptr,rock!=nullptr,space!=nullptr);
    }
    ~SoloAssets(){
        for(SDL_Texture *p:{ship,enemy,rock,space,projectile,energy})
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
            SDL_Color base=t==Tile::Rock?SDL_Color{37,48,69,255}:
                t==Tile::Lava?SDL_Color{111,31,20,255}:
                              SDL_Color{37,25,82,255};
            fill(r,square,base);
            if(assets&&assets->rock){
                const SDL_Color tint=t==Tile::Rock?
                    SDL_Color{uint8_t(138+hash%50),uint8_t(145+hash%55),
                              uint8_t(160+hash%60),255}:
                    t==Tile::Lava?SDL_Color{255,113,58,255}:
                                  SDL_Color{144,124,255,255};
                soloCopy(r,assets->rock,
                         {px-int(cell*.13f),py-int(cell*.13f),
                          int(cell*1.26f),int(cell*1.26f)},
                         tint,double(hash%8u)*45.0);
            }
            if(t==Tile::Lava||t==Tile::Beam){
                SDL_SetRenderDrawColor(r,
                    t==Tile::Lava?255:185,t==Tile::Lava?153:112,
                    t==Tile::Lava?44:255,255);
                const int offset=int(std::sin(now*3+x+y)*cell*.12f);
                SDL_RenderDrawLine(r,px+radius/2,py+radius/2,
                                   px+int(cell*.82f),py+int(cell*.63f)+offset);
            }
            continue;
        }
        if(t==Tile::Asteroid || t==Tile::IceAsteroid){
            if(assets&&assets->rock){
                const SDL_Color tint=t==Tile::IceAsteroid?
                    SDL_Color{158,233,255,255}:SDL_Color{215,218,229,255};
                soloCopy(r,assets->rock,{cx-radius,cy-radius,radius*2,radius*2},
                         tint,double(hash%8u)*45.0);
            }else{
                fill(r,{cx-radius,cy-radius,radius*2,radius*2},tileColor(t));
            }
            if(t==Tile::IceAsteroid){
                SDL_SetRenderDrawColor(r,220,253,255,255);
                SDL_RenderDrawLine(r,cx-radius,cy,cx+radius,cy);
                SDL_RenderDrawLine(r,cx,cy-radius,cx,cy+radius);
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
    const int shipX=camera.pixelX(s.pilot.x);
    const int shipY=camera.pixelY(s.pilot.y);
    const int radius=std::max(17,int(cell*.90f));
    const int pulse=std::max(1,int(cell*.09f));
    SDL_SetRenderDrawColor(r,58,204,250,255);
    for(int k=0;k<2;++k){
        SDL_Rect glow{shipX-radius-pulse-k*2,shipY-radius-pulse-k*2,
                     2*(radius+pulse+k*2),2*(radius+pulse+k*2)};
        SDL_RenderDrawRect(r,&glow);
    }
    if(assets&&assets->ship)soloCopy(r,assets->ship,
        {shipX-radius,shipY-radius,radius*2,radius*2});
    else{
        SDL_SetRenderDrawColor(r,92,237,255,255);
        SDL_RenderDrawLine(r,shipX,shipY-radius,shipX-radius,shipY+radius);
        SDL_RenderDrawLine(r,shipX-radius,shipY+radius,shipX+radius,shipY+radius);
        SDL_RenderDrawLine(r,shipX+radius,shipY+radius,shipX,shipY-radius);
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
    const std::string goal="RELAIS "+std::to_string(s.relaysCollected)+"/"+
                            std::to_string(s.relaysNeeded);
    soloLabel(r,goal,{viewport.x+12,hudY+18,barW+28,34},
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
inline void drawKineticCharge(SDL_Renderer *r,const Session &s,SDL_Rect viewport,
                              const KineticSurgeControl &kinetic){
    if(!r||viewport.w<=0||viewport.h<=0||!sfKineticSurges[1].held)return;
    const auto &surge=sfKineticSurges[1];
    const float progress=std::clamp(
        surge.heldSeconds/SF_KINETIC_SURGE_HOLD_SECONDS,0.0f,1.0f);
    const int barW=std::max(70,viewport.w/2);
    const int barH=std::max(12,viewport.h/65);
    const int barX=viewport.x+(viewport.w-barW)/2;
    const int barY=viewport.y+viewport.h-barH-60;
    fill(r,{barX-3,barY-3,barW+6,barH+6},{255,255,255,255});
    fill(r,{barX,barY,barW,barH},{25,35,57,255});
    fill(r,{barX,barY,int(barW*progress),barH},surge.charged?
         SDL_Color{103,255,166,255}:SDL_Color{255,216,95,255});
    if(!sfKineticSurgeVisible(1))return;
    const SoloViewport camera=soloViewport(s,viewport);
    const int px=camera.pixelX(s.pilot.x),py=camera.pixelY(s.pilot.y);
    const AimVector d=kinetic.direction(s);
    const float range=coneRangeForShip(s.shipRadius);
    const float half=sfKineticSurgeConeHalfWidth(range);
    const int forwardX=int(range*camera.cell*d.x),forwardY=int(range*camera.cell*d.y);
    const int sideX=int(half*camera.cell*-d.y),sideY=int(half*camera.cell*d.x);
    const int ex=px+forwardX,ey=py+forwardY;
    SDL_SetRenderDrawColor(r,135,238,255,255);
    SDL_RenderDrawLine(r,px,py,ex+sideX,ey+sideY);
    SDL_RenderDrawLine(r,px,py,ex-sideX,ey-sideY);
    SDL_RenderDrawLine(r,ex-sideX,ey-sideY,ex+sideX,ey+sideY);
}
inline void drawCombat(SDL_Renderer *r,const Session &s,const Combat &combat,
                       SDL_Rect viewport,SoloAssets *assets=nullptr){
    if(!r||!s.map.valid()||viewport.w<=0||viewport.h<=0)return;
    const SoloViewport camera=soloViewport(s,viewport);
    for(const auto &shot:combat.shots){
        const int px=camera.pixelX(shot.x),py=camera.pixelY(shot.y);
        if(px<viewport.x||px>=viewport.x+viewport.w||
           py<viewport.y||py>=viewport.y+viewport.h)continue;
        const int d=std::max(7,int(camera.cell*.35f));
        if(assets&&assets->projectile)
            soloCopy(r,assets->projectile,{px-d/2,py-d/2,d,d},
                     {235,247,255,255},
                     std::atan2(shot.vy,shot.vx)*180.0/3.14159265+90.0);
        else fill(r,{px-d/2,py-d/2,d,d},{255,236,114,255});
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
