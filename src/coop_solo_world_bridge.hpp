#pragma once
// SOLO-COOP-WORLD-002: one physical COOP ship will own the WORLD position.
// This is a view/collision adapter, NOT another physics integrator or pilot.
// Reference combat distance = historical arena width, NEVER maze width.
#include "solo_map_generator.hpp"
#include "solo_viewport.hpp"
#include <algorithm>
#include <cmath>
#include <optional>
#include <queue>
#include <limits>
#include <SDL2/SDL.h>

namespace sfsolo {
struct CoopWorldPoint { float x=0,y=0; };
struct CoopBounceMotion {
    CoopWorldPoint position{},velocity{},normal{};
    bool collided=false;
};
struct CoopSoloWorldBridge {
    Map map;
    SDL_Rect screen{};
    float combatWidth=1.0f;
    float pixelsPerCell=1.0f;
    float cameraLeft=0,cameraTop=0,zoom=1;
    // Presentation-only offset: moves parallax without shifting any world tile.
    float backgroundDriftX=0,backgroundDriftY=0;
    bool ready=false;
    bool localGateClosed=false;
    CoopWorldPoint localGateCentre{};
    float localGateRadius=0;
    std::vector<CoopWorldPoint> pickupBatchCells; // rebuilt once per reward batch
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
        cameraLeft=cameraTop=0;zoom=1;
        backgroundDriftX=backgroundDriftY=0;
        localGateClosed=false;localGateRadius=0;
        return ready=true;
    }
    float worldWidth() const {return map.width*pixelsPerCell;}
    float worldHeight() const {return map.height*pixelsPerCell;}
    float visibleWidth() const {return float(screen.w)/zoom;}
    float visibleHeight() const {return float(screen.h)/zoom;}
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
        return {screen.x+(world.x-cameraLeft)*zoom,screen.y+(world.y-cameraTop)*zoom};
    }
    CoopWorldPoint toWorld(CoopWorldPoint pixel) const {
        return {(pixel.x-screen.x)/zoom+cameraLeft,(pixel.y-screen.y)/zoom+cameraTop};
    }
    // The held first fingertip has TWO meanings: a precise on-screen ship
    // position, and (outside the inner circular dead zone) a persistent world
    // exploration direction. Merely aiming the canonical COOP pilot at the
    // fingertip remains a world-space intention, with an outward lead in the
    // circular cruise zones. This lead and the physical camera speed use the
    // combat reference width, so zoom never retunes the pilot's world speed.
    CoopWorldPoint heldFingerPilotTarget(CoopWorldPoint finger) const {
        const auto exact=toWorld(finger);
        if(!ready)return exact;
        const auto zone=soloSoftCirclePan(finger.x,finger.y,screen,
                                       combatWidth/pixelsPerCell);
        if(zone.speed<=0.0f)return exact;
        const float vectorLength=std::hypot(zone.vx,zone.vy);
        if(vectorLength<=.0001f)return exact;
        // ShipMotion approaches distance*12 before its physical speed cap.
        const float desiredCameraPxPerSecond=vectorLength*pixelsPerCell;
        const float lead=desiredCameraPxPerSecond/12.0f;
        return {exact.x+zone.vx/vectorLength*lead,
                exact.y+zone.vy/vectorLength*lead};
    }
    // Swept disc, conservatively expanded tile boxes at corners. Unlike a
    // sampled endpoint this cannot tunnel through a wall on a long frame.
    float firstWallImpact(CoopWorldPoint from,CoopWorldPoint to,float radius) const {
        if(!ready || radius<=0 || !std::isfinite(radius) ||
           !std::isfinite(from.x)||!std::isfinite(from.y)||
           !std::isfinite(to.x)||!std::isfinite(to.y))return 0;
        float first=1;
        const float dx=to.x-from.x,dy=to.y-from.y;
        const int left=std::max(-1,int(std::floor((std::min(from.x,to.x)-radius)/pixelsPerCell)));
        const int right=std::min(map.width,int(std::floor((std::max(from.x,to.x)+radius)/pixelsPerCell)));
        const int top=std::max(-1,int(std::floor((std::min(from.y,to.y)-radius)/pixelsPerCell)));
        const int bottom=std::min(map.height,int(std::floor((std::max(from.y,to.y)+radius)/pixelsPerCell)));
        if(from.x<radius || from.y<radius || from.x>worldWidth()-radius || from.y>worldHeight()-radius)return 0;
        for(int y=top;y<=bottom;++y)for(int x=left;x<=right;++x){
            if(map.at(x,y)!=Tile::Rock)continue;
            float enter=0,leave=1;
            const auto slab=[&](float p,float d,float low,float high){
                if(std::abs(d)<1e-7f)return p>low && p<high;
                float a=(low-p)/d,b=(high-p)/d;if(a>b)std::swap(a,b);
                enter=std::max(enter,a);leave=std::min(leave,b);return enter<leave;
            };
            if(slab(from.x,dx,x*pixelsPerCell-radius,(x+1)*pixelsPerCell+radius) &&
               slab(from.y,dy,y*pixelsPerCell-radius,(y+1)*pixelsPerCell+radius))first=std::min(first,enter);
        }
        return first;
    }
    // Loot must belong to the component the physical ship can reach. Nearest
    // empty tile alone would deposit rewards across an impassable wall.
    std::vector<CoopWorldPoint> reachablePickupCells(CoopWorldPoint ship,float clearance) const {
        std::vector<CoopWorldPoint> cells;
        if(!ready || clearance<=0 || !canOccupy(ship,clearance))return cells;
        const int sx=int(ship.x/pixelsPerCell),sy=int(ship.y/pixelsPerCell);
        if(sx<0||sy<0||sx>=map.width||sy>=map.height)return cells;
        const auto centre=[&](int x,int y){return CoopWorldPoint{(x+.5f)*pixelsPerCell,(y+.5f)*pixelsPerCell};};
        if(firstWallImpact(ship,centre(sx,sy),clearance)<1)return cells;
        std::vector<unsigned char> seen(std::size_t(map.width)*map.height,0);
        std::queue<int> pending;pending.push(sy*map.width+sx);seen[pending.front()]=1;
        while(!pending.empty()){
            const int i=pending.front();pending.pop();const int x=i%map.width,y=i/map.width;
            const auto at=centre(x,y);const auto tile=map.at(x,y);
            if(tile!=Tile::Lava && tile!=Tile::Beam)cells.push_back(at);
            constexpr int ox[]={1,-1,0,0},oy[]={0,0,1,-1};
            for(int n=0;n<4;++n){
                const int nx=x+ox[n],ny=y+oy[n];
                if(nx<0||ny<0||nx>=map.width||ny>=map.height)continue;
                const int j=ny*map.width+nx;if(seen[j])continue;
                const auto next=centre(nx,ny);
                if(firstWallImpact(at,next,clearance)<1)continue;
                seen[j]=1;pending.push(j);
            }
        }
        return cells;
    }
    static bool selectPickup(const std::vector<CoopWorldPoint> &cells,CoopWorldPoint desired,
                             const std::vector<CoopWorldPoint> &occupied,float separation,CoopWorldPoint &out){
        float best=std::numeric_limits<float>::max();bool found=false;
        for(const auto at:cells){
            const float dx=at.x-desired.x,dy=at.y-desired.y,d=dx*dx+dy*dy;
            if(d>=best)continue;
            bool overlap=false;
            for(const auto p:occupied)if(std::hypot(at.x-p.x,at.y-p.y)<separation){overlap=true;break;}
            if(!overlap){best=d;out=at;found=true;}
        }
        return found;
    }
    bool reachablePickup(CoopWorldPoint ship,CoopWorldPoint desired,float clearance,CoopWorldPoint &out) const {
        return selectPickup(reachablePickupCells(ship,clearance),desired,{},0,out);
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
        const float dx=to.x-from.x,dy=to.y-from.y;
        const float length=std::hypot(dx,dy);
        const float hit=firstWallImpact(from,to,hullRadius);
        if(hit>=1 && canOccupy(to,hullRadius))return to;
        const float fraction=std::max(0.f,hit-.01f/std::max(1.f,length));
        CoopWorldPoint cursor{from.x+dx*fraction,from.y+dy*fraction};
        // Resolve the remaining tangent motion with another sweep, never a
        // long endpoint sample. Even a frame crossing >200 tiles is safe.
        for(int axis=0;axis<2;++axis){
            CoopWorldPoint target=cursor;
            if(axis==0)target.x=to.x;else target.y=to.y;
            const float distance=std::hypot(target.x-cursor.x,target.y-cursor.y);
            const float t=firstWallImpact(cursor,target,hullRadius);
            const float safe=t>=1?1:std::max(0.f,t-.01f/std::max(1.f,distance));
            const CoopWorldPoint accepted{cursor.x+(target.x-cursor.x)*safe,cursor.y+(target.y-cursor.y)*safe};
            if(canOccupy(accepted,hullRadius))cursor=accepted;
        }
        return cursor;
    }

    // SOLO ONLY: carry the velocity through the first swept contact.
    // Unlike moveWithoutTunnelling(), this response restores a fraction
    // of normal momentum and integrates the frame's unused time.
    CoopBounceMotion moveWithBounce(CoopWorldPoint from,CoopWorldPoint desired,
                                    CoopWorldPoint velocity,float radius,float dt) const {
        CoopBounceMotion result{from,velocity,{0,0},false};
        if(!ready||radius<=0||!std::isfinite(dt)||dt<=0)return result;
        if(!canOccupy(result.position,radius)){
            // Recover a *nearby* valid centre after tiny numeric overlap,
            // without jumping a passage or crossing an opaque wall.
            bool recovered=false;
            const float step=std::max(.02f,std::min(radius*.15f,pixelsPerCell*.075f));
            for(int ring=1;ring<=12&&!recovered;++ring){
                for(int ray=0;ray<16;++ray){
                    const float angle=float(ray)*6.28318530718f/16.f;
                    const CoopWorldPoint candidate{from.x+std::cos(angle)*ring*step,
                                                   from.y+std::sin(angle)*ring*step};
                    if(canOccupy(candidate,radius)){
                        result.position=candidate;recovered=true;break;
                    }
                }
            }
            if(!recovered){result.velocity={0,0};result.collided=true;return result;}
        }
        float remaining=dt;
        for(int attempt=0;attempt<4&&remaining>1e-6f;++attempt){
            const CoopWorldPoint start=result.position;
            const CoopWorldPoint goal=attempt==0?desired:
                CoopWorldPoint{start.x+result.velocity.x*remaining,
                               start.y+result.velocity.y*remaining};
            const float dx=goal.x-start.x,dy=goal.y-start.y;
            const float distance=std::hypot(dx,dy);
            if(distance<1e-5f)break;
            const float t=firstWallImpact(start,goal,radius);
            if(t>=1 && canOccupy(goal,radius)){result.position=goal;break;}
            result.collided=true;
            const float hit=std::clamp(t,0.f,1.f);
            const CoopWorldPoint contact{start.x+dx*hit,start.y+dy*hit};
            const float safe=std::max(0.f,hit-.05f/std::max(1.f,distance));
            result.position={start.x+dx*safe,start.y+dy*safe};
            // Recover a geometric contact normal from the nearest solid tile.
            CoopWorldPoint normal{-dx/distance,-dy/distance};
            float nearestDistance=std::numeric_limits<float>::infinity();
            const int xmin=int(std::floor((contact.x-radius)/pixelsPerCell))-1;
            const int xmax=int(std::floor((contact.x+radius)/pixelsPerCell))+1;
            const int ymin=int(std::floor((contact.y-radius)/pixelsPerCell))-1;
            const int ymax=int(std::floor((contact.y+radius)/pixelsPerCell))+1;
            for(int iy=ymin;iy<=ymax;++iy)for(int ix=xmin;ix<=xmax;++ix){
                if(map.at(ix,iy)!=Tile::Rock)continue;
                const float px=std::clamp(contact.x,ix*pixelsPerCell,(ix+1)*pixelsPerCell);
                const float py=std::clamp(contact.y,iy*pixelsPerCell,(iy+1)*pixelsPerCell);
                const float nx=contact.x-px,ny=contact.y-py;
                const float length=std::hypot(nx,ny);
                if(length>1e-5f && length<nearestDistance){
                    const float ux=nx/length,uy=ny/length;
                    if(result.velocity.x*ux+result.velocity.y*uy<0){
                        normal={ux,uy};nearestDistance=length;
                    }
                }
            }
            const float vn=result.velocity.x*normal.x+result.velocity.y*normal.y;
            result.normal=normal;
            if(vn<0){
                const float restitution=(-vn>combatWidth*.018f)? .46f:0.f;
                // A slight tangential loss stops endless corner vibrations.
                result.velocity.x=(result.velocity.x-vn*normal.x)*.94f-vn*restitution*normal.x;
                result.velocity.y=(result.velocity.y-vn*normal.y)*.94f-vn*restitution*normal.y;
            }else{
                // Conservative expanded AABBs occasionally report an edge
                // while already moving away; do not reverse that movement.
                result.position=moveWithoutTunnelling(start,goal,radius);
                break;
            }
            const CoopWorldPoint separated{
                result.position.x+normal.x*.055f,result.position.y+normal.y*.055f};
            if(canOccupy(separated,radius))result.position=separated;
            remaining*=1.f-hit;
            if(std::hypot(result.velocity.x,result.velocity.y)<combatWidth*.001f)break;
        }
        if(!canOccupy(result.position,radius))result.position=from;
        // A sealed LOCAL encounter portion reflects the player inward.
        // No world map mutation, no effect on projectiles or the boss lair.
        if(localGateClosed && localGateRadius>radius &&
           std::hypot(from.x-localGateCentre.x,from.y-localGateCentre.y)<
               localGateRadius-radius){
            const float nx=result.position.x-localGateCentre.x;
            const float ny=result.position.y-localGateCentre.y;
            const float length=std::hypot(nx,ny);
            const float limit=localGateRadius-radius;
            if(length>limit){
                const float ux=nx/length,uy=ny/length;
                const CoopWorldPoint goal{localGateCentre.x+ux*(limit-.1f),
                                           localGateCentre.y+uy*(limit-.1f)};
                result.position=moveWithoutTunnelling(from,goal,radius);
                const float normal=result.velocity.x*ux+result.velocity.y*uy;
                if(normal>0){
                    result.velocity.x=(result.velocity.x-normal*ux)*.94f-normal*.46f*ux;
                    result.velocity.y=(result.velocity.y-normal*uy)*.94f-normal*.46f*uy;
                }
                result.normal={-ux,-uy};result.collided=true;
            }
        }
        return result;
    }

    void setZoom(float value,CoopWorldPoint anchor){
        const auto pixel=toScreen(anchor);zoom=std::clamp(value,1.f,2.f);
        cameraLeft=soloClampCamera(anchor.x-(pixel.x-screen.x)/zoom,worldWidth(),visibleWidth());
        cameraTop=soloClampCamera(anchor.y-(pixel.y-screen.y)/zoom,worldHeight(),visibleHeight());
    }
    bool directView(CoopWorldPoint observer,CoopWorldPoint event)const{
        const auto p=toScreen(event);
        return p.x>=screen.x && p.x<screen.x+screen.w && p.y>=screen.y && p.y<screen.y+screen.h &&
            firstWallImpact(observer,event,.1f)>=1;
    }
    void updateBackgroundOnly(CoopWorldPoint ship,CoopWorldPoint displacement,float dt){
        if(!ready||dt<=0||!std::isfinite(dt)||
           !std::isfinite(displacement.x)||!std::isfinite(displacement.y))return;
        // SOLO-024: in the 1/2-to-1 diameter inner ring, only the scenery
        // moves; the camera, rocks, enemies and collision map do NOT.
        // Across 1-to-2 diameters smoothly crossfade extra background drift
        // into the existing physical-camera pan of SOLO-022.
        const auto px=toScreen(ship);
        const float gain=soloBackgroundOnlyGain(px.x,px.y,screen);
        backgroundDriftX+=displacement.x*gain;
        backgroundDriftY+=displacement.y*gain;
    }
    void followPhysicalShip(CoopWorldPoint ship,CoopWorldPoint displacement,float dt){
        updateBackgroundOnly(ship,displacement,dt);
        panIfShipActuallyMoved(ship,displacement,dt);
        // Safety envelope follows the physical hull, never the touch. It also
        // works after release, at max zoom, and when a long frame hits a wall.
        const float mx=visibleWidth()*.14f,my=visibleHeight()*.16f;
        cameraLeft=soloClampCamera(std::clamp(cameraLeft,ship.x-visibleWidth()+mx,ship.x-mx),worldWidth(),visibleWidth());
        cameraTop=soloClampCamera(std::clamp(cameraTop,ship.y-visibleHeight()+my,ship.y-my),worldHeight(),visibleHeight());
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
        // SOLO-022: very slow additional 1->2 diameter scroll band,
        // progressing through SOLO-020 without stopping; max remains 6.
        // User arbitration about 4.32 vs 3.10 visible widths/s is still
        // pending; preserve the already agreed radial speed, do not retune it.
        const auto pan=soloSoftCirclePan(onscreen.x,onscreen.y,screen,
                                     combatWidth/pixelsPerCell);
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
