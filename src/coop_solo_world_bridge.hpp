#pragma once
// SOLO-COOP-WORLD-002: one physical COOP ship will own the WORLD position.
// This is a view/collision adapter, NOT another physics integrator or pilot.
// Reference combat distance = historical arena width, NEVER maze width.
#include "solo_map_generator.hpp"
#include "solo_viewport.hpp"
#include <algorithm>
#include <cmath>
#include <optional>
#include <SDL2/SDL.h>

namespace sfsolo {
struct CoopWorldPoint { float x=0,y=0; };
struct CoopSoloWorldBridge {
    Map map;
    SDL_Rect screen{};
    float combatWidth=1.0f;
    float pixelsPerCell=1.0f;
    float cameraLeft=0,cameraTop=0;
    bool ready=false;
    bool create(MapOptions options,SDL_Rect output,float historicCombatWidth){
        // Never change the original COOP arena, nrj/damage or projectile
        // scales when the map becomes 54x160 or larger.
        ready=false;
        map=generate(options);
        if(!map.valid()||output.w<=0||output.h<=0||
           !std::isfinite(historicCombatWidth)||historicCombatWidth<=0)return false;
        screen=output;
        combatWidth=historicCombatWidth;
        pixelsPerCell=historicCombatWidth/(SOLO_REFERENCE_COLUMNS/SOLO_MAP_ZOOM);
        cameraLeft=cameraTop=0;
        return ready=true;
    }
    float worldWidth() const {return map.width*pixelsPerCell;}
    float worldHeight() const {return map.height*pixelsPerCell;}
    float visibleWidth() const {return float(screen.w);}
    float visibleHeight() const {return float(screen.h);}
    CoopWorldPoint start() const {
        for(int y=0;y<map.height;++y)for(int x=0;x<map.width;++x)
            if(map.at(x,y)==Tile::Start)
                return {(x+.5f)*pixelsPerCell,(y+.5f)*pixelsPerCell};
        return {};
    }
    void centerOn(CoopWorldPoint ship){
        cameraLeft=soloClampCamera(ship.x-visibleWidth()*.5f,worldWidth(),visibleWidth());
        cameraTop=soloClampCamera(ship.y-visibleHeight()*.5f,worldHeight(),visibleHeight());
    }
    CoopWorldPoint toScreen(CoopWorldPoint world) const {
        return {screen.x+world.x-cameraLeft,screen.y+world.y-cameraTop};
    }
    CoopWorldPoint toWorld(CoopWorldPoint pixel) const {
        return {pixel.x-screen.x+cameraLeft,pixel.y-screen.y+cameraTop};
    }
    bool canOccupy(CoopWorldPoint world,float radius) const {
        if(!ready || radius<0 || !std::isfinite(world.x)||!std::isfinite(world.y))return false;
        // Circle-vs-tile AABB: the ship's HULL, not only its centre,
        // decides whether a wall is passable. Rock is never passable.
        const float left=(world.x-radius)/pixelsPerCell;
        const float right=(world.x+radius)/pixelsPerCell;
        const float top=(world.y-radius)/pixelsPerCell;
        const float bottom=(world.y+radius)/pixelsPerCell;
        for(int iy=int(std::floor(top));iy<=int(std::floor(bottom));++iy)
            for(int ix=int(std::floor(left));ix<=int(std::floor(right));++ix){
                if(map.at(ix,iy)!=Tile::Rock)continue;
                const float minX=ix*pixelsPerCell,maxX=minX+pixelsPerCell;
                const float minY=iy*pixelsPerCell,maxY=minY+pixelsPerCell;
                const float nearestX=std::clamp(world.x,minX,maxX);
                const float nearestY=std::clamp(world.y,minY,maxY);
                const float dx=world.x-nearestX,dy=world.y-nearestY;
                if(dx*dx+dy*dy<radius*radius)return false;
            }
        return true;
    }
    CoopWorldPoint moveWithoutTunnelling(CoopWorldPoint from,CoopWorldPoint to,
                                         float hullRadius) const {
        if(!canOccupy(from,hullRadius))return from;
        const float distance=std::hypot(to.x-from.x,to.y-from.y);
        const float step=std::max(.5f,pixelsPerCell*.20f);
        const int count=std::clamp(int(std::ceil(distance/step)),1,200);
        CoopWorldPoint cursor=from;
        for(int i=1;i<=count;++i){
            const float t=float(i)/count;
            const CoopWorldPoint next{from.x+(to.x-from.x)*t,
                                      from.y+(to.y-from.y)*t};
            if(canOccupy(next,hullRadius)){cursor=next;continue;}
            // Slide along either free axis, but NEVER tunnel through a rock.
            const CoopWorldPoint slideX{next.x,cursor.y};
            const CoopWorldPoint slideY{cursor.x,next.y};
            if(canOccupy(slideX,hullRadius))cursor=slideX;
            else if(canOccupy(slideY,hullRadius))cursor=slideY;
            else break;
        }
        return cursor;
    }

    // Draw only the visible part of the immutable real SOLO map, in SCREEN
    // coordinates, with connected wall masses and subtle inner cracks.
    void draw(SDL_Renderer *r) const {
        if(!r||!ready)return;
        const int x0=std::max(0,int(std::floor(cameraLeft/pixelsPerCell))-1);
        const int x1=std::min(map.width,int(std::ceil((cameraLeft+visibleWidth())/pixelsPerCell))+1);
        const int y0=std::max(0,int(std::floor(cameraTop/pixelsPerCell))-1);
        const int y1=std::min(map.height,int(std::ceil((cameraTop+visibleHeight())/pixelsPerCell))+1);
        SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
        for(int y=y0;y<y1;++y)for(int x=x0;x<x1;++x){
            const auto t=map.at(x,y);
            if(t!=Tile::Rock && t!=Tile::Lava && t!=Tile::Beam
               && t!=Tile::Trigger && t!=Tile::Boss && t!=Tile::Finish)
                continue;
            SDL_Rect rect{
                int(std::floor((x*pixelsPerCell-cameraLeft)+screen.x)),
                int(std::floor((y*pixelsPerCell-cameraTop)+screen.y)),
                int(std::ceil(pixelsPerCell))+1,int(std::ceil(pixelsPerCell))+1};
            if(t==Tile::Rock){
                const unsigned noise=(unsigned(x)*83492791u)^(unsigned(y)*2971215073u);
                const Uint8 c=Uint8(45+(noise%19u));
                SDL_SetRenderDrawColor(r,c,Uint8(c-4),Uint8(c+10),255);
                SDL_RenderFillRect(r,&rect);
                // Only mark a contour where Rock meets traversable space:
                // adjacent wall cells read as ONE continuous rock mass.
                SDL_SetRenderDrawColor(r,117,111,129,125);
                if(map.at(x,y-1)!=Tile::Rock)
                    SDL_RenderDrawLine(r,rect.x,rect.y,rect.x+rect.w,rect.y);
                if(map.at(x-1,y)!=Tile::Rock)
                    SDL_RenderDrawLine(r,rect.x,rect.y,rect.x,rect.y+rect.h);
                if(map.at(x+1,y)!=Tile::Rock)
                    SDL_RenderDrawLine(r,rect.x+rect.w-1,rect.y,rect.x+rect.w-1,rect.y+rect.h);
                if(map.at(x,y+1)!=Tile::Rock)
                    SDL_RenderDrawLine(r,rect.x,rect.y+rect.h-1,rect.x+rect.w,rect.y+rect.h-1);
            }else{
                SDL_Color color=t==Tile::Lava?SDL_Color{205,58,19,65}:
                    t==Tile::Beam?SDL_Color{110,60,240,65}:
                    t==Tile::Trigger?SDL_Color{65,205,176,105}:
                    t==Tile::Finish?SDL_Color{90,240,125,90}:
                    SDL_Color{245,110,210,90};
                SDL_SetRenderDrawColor(r,color.r,color.g,color.b,color.a);
                SDL_RenderFillRect(r,&rect);
            }
        }
    }
    void panIfShipActuallyMoved(
        CoopWorldPoint ship,CoopWorldPoint displacement,float dt,
        std::optional<CoopWorldPoint> fingerScreen=std::nullopt){
        if(!ready||dt<=0||!std::isfinite(dt))return;
        const auto onscreen=toScreen(ship);
        // Exact existing SOLO concentric law: 0 / 0.1->2 / 2.1->6 / 6.
        // User arbitration about 4.32 vs 3.10 visible widths/s is still
        // pending; preserve the already agreed radial speed, do not retune it.
        const auto pan=soloCirclePan(onscreen.x,onscreen.y,screen,
                                     visibleWidth()/pixelsPerCell);
        float desiredX=pan.vx*pixelsPerCell*std::min(dt,.05f);
        float desiredY=pan.vy*pixelsPerCell*std::min(dt,.05f);
        if(fingerScreen){
            // The CAMERA must never steal the ship's remaining movement
            // BEFORE the ship has reached the held fingertip. Previously the
            // maximum radial pan outran the original COOP 3.10*W/s pilot,
            // and was clamped to its whole displacement; its screen position
            // consequently froze BEFORE reaching the finger forever.
            //
            // Camera movement is limited to the *excess* of ship movement
            // beyond the finger, per axis. Once caught up, the camera follows
            // the ship without separating it from the same stationary touch.
            const float excessX=onscreen.x-fingerScreen->x;
            const float excessY=onscreen.y-fingerScreen->y;
            if(desiredX>0)desiredX=std::min(desiredX,std::max(0.0f,excessX));
            else if(desiredX<0)desiredX=std::max(desiredX,std::min(0.0f,excessX));
            if(desiredY>0)desiredY=std::min(desiredY,std::max(0.0f,excessY));
            else if(desiredY<0)desiredY=std::max(desiredY,std::min(0.0f,excessY));
        }
        auto clampToMotion=[](float wanted,float moved){
            return wanted>0?std::min(wanted,std::max(0.f,moved)):
                   wanted<0?std::max(wanted,std::min(0.f,moved)):0.f;
        };
        cameraLeft=soloClampCamera(cameraLeft+clampToMotion(desiredX,displacement.x),
                                   worldWidth(),visibleWidth());
        cameraTop=soloClampCamera(cameraTop+clampToMotion(desiredY,displacement.y),
                                  worldHeight(),visibleHeight());
    }
};
} // namespace sfsolo
