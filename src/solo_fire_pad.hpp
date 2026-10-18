#pragma once
#include <SDL2/SDL.h>
#include <cmath>
#include <algorithm>
#include <cstdint>

namespace sfsolo {
constexpr float stickPi=3.14159265358979323846f;
inline void neonRing(SDL_Renderer*r,float x,float y,float radius,SDL_Color c,bool dashed){
    SDL_SetRenderDrawColor(r,c.r,c.g,c.b,c.a);
    constexpr int segments=72;
    for(int j=0;j<segments;++j){
        if(dashed && j%8>=5)continue;
        const float a=j*(2.f*stickPi/segments),b=(j+1)*(2.f*stickPi/segments);
        SDL_RenderDrawLine(r,int(x+std::cos(a)*radius),int(y+std::sin(a)*radius),
                             int(x+std::cos(b)*radius),int(y+std::sin(b)*radius));
    }
}
inline void neonDisk(SDL_Renderer*r,float x,float y,float radius,SDL_Color color){
    SDL_SetRenderDrawColor(r,color.r,color.g,color.b,color.a);
    for(int dy=-int(radius);dy<=int(radius);++dy){
        const float half=std::sqrt(std::max(0.f,radius*radius-dy*dy));
        SDL_RenderDrawLine(r,int(x-half),int(y+dy),int(x+half),int(y+dy));
    }
}

// FirePad: hold at centre -> drag to aim, hold to fire in continuous bursts.
// Re-tapping centre without dragging fires at the last retained angle.
// The burst-rest gate exists ONLY here; historic COOP and DUEL never use it.
struct FirePad {
    struct Response {bool owned=false,fire=false;}; // .fire = press edge for instant first shot
    bool right=false;
    std::int64_t finger=-1;
    float dx=0,dy=-1,dragX=0,dragY=0;
    float burst=0,rest=0;
    static constexpr float padPi=stickPi;
    static constexpr float burstSeconds=1.25f;
    static constexpr float restSeconds=1.50f;
    float cx(int w)const {return right?w-std::max(92.f,w*.20f):std::max(92.f,w*.20f);}
    float cy(int h)const {return h-std::max(144.f,h*.17f);}
    float orbit(int w)const {return std::clamp(w*.135f,54.f,82.f);}
    bool active()const {return finger>=0;}
    void cancel(){finger=-1;dragX=dragY=0;burst=rest=0;}
    void set(float x,float y){
        const float len=std::hypot(x,y);
        if(len>.0001f){dx=x/len;dy=y/len;}
    }
    Response handle(int type,std::int64_t id,float x,float y,int w,int h){
        if(active()){
            if(id!=finger)return {};
            if(type==SDL_FINGERMOTION){
                dragX=x-cx(w);dragY=y-cy(h);
                if(std::hypot(dragX,dragY)>std::max(12.f,w*.025f))set(dragX,dragY);
                return {true,false};
            }
            if(type==SDL_FINGERUP){cancel();return {true,false};}
            return {true,false};
        }
        if(type!=SDL_FINGERDOWN)return {};
        // Entire circular touch target belongs to fire; never steal other stick.
        if(std::hypot(x-cx(w),y-cy(h))>orbit(w)+13.f)return {};
        finger=id;
        dragX=x-cx(w);dragY=y-cy(h);
        if(std::hypot(dragX,dragY)>std::max(12.f,w*.025f))set(dragX,dragY);
        burst=rest=0;
        return {true,true};
    }
    // Returns permission to attempt an actual shot. Canonical COOP fire cooldown
    // and energy availability still decide whether a round can be emitted.
    bool tick(float dt){
        if(!active()||dt<=0)return false;
        if(rest>0){
            rest=std::max(0.f,rest-dt);
            if(rest>0)return false;
            burst=0;
        }
        burst+=dt;
        if(burst>=burstSeconds){
            burst=0;rest=restSeconds;return false;
        }
        return true;
    }
    void draw(SDL_Renderer*r,int w,int h)const {
        if(!r)return;
        SDL_BlendMode old;SDL_GetRenderDrawBlendMode(r,&old);
        SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
        const float x=cx(w),y=cy(h),radius=orbit(w);
        // SOLO-028: no full-size tinted disk: the world behind is untouched.
        neonRing(r,x,y,radius,{82,225,241,112},true);
        neonDisk(r,x,y,radius*.36f,{64,219,233,34});
        neonRing(r,x,y,radius*.36f,{98,240,247,125},false);
        const float extent=radius*.81f,ex=x+dx*extent,ey=y+dy*extent;
        SDL_SetRenderDrawColor(r,101,244,250,170);
        SDL_RenderDrawLine(r,int(x),int(y),int(ex),int(ey));
        const float perpX=-dy,perpY=dx;
        SDL_RenderDrawLine(r,int(ex),int(ey),int(ex-dx*16+perpX*8),int(ey-dy*16+perpY*8));
        SDL_RenderDrawLine(r,int(ex),int(ey),int(ex-dx*16-perpX*8),int(ey-dy*16-perpY*8));
        if(rest>0)neonRing(r,x,y,radius*.92f,{245,170,102,140},false);
        SDL_SetRenderDrawBlendMode(r,old);
    }
};

// Accumulate INTENTION only, never physical speed or camera displacement.
// SOLO-032: directional-intent reserve. Inertia belongs to ShipMotion;
// this bounded reserve ONLY adjusts the target speed request in SOLO maze.
// The vector (anchorDx, anchorDy) reorients continuously. stableSeconds is
// its stored charge; bonus is a second, slower output low-pass. Neither can
// jump to zero on an ordinary turn, including a 180-degree reversal.
struct SteeringHoldBoost {
    float stableSeconds=0,anchorDx=0,anchorDy=0,bonus=0,recoverySeconds=0;
    bool latched=false;
    static constexpr float edgeStart=.68f,edgeFull=.94f,maximumBonus=.36f;
    static constexpr float maximumRequested=1.30f;
    static constexpr float chargeSeconds=5.f;
    static float smooth(float t){t=std::clamp(t,0.f,1.f);return t*t*(3.f-2.f*t);}
    void reset(){stableSeconds=anchorDx=anchorDy=bonus=recoverySeconds=0;latched=false;}
    void tick(float dt,float dx,float dy,float radial){
        if(!std::isfinite(dt)||dt<=0)return;
        if(!std::isfinite(dx)||!std::isfinite(dy)||!std::isfinite(radial)){
            reset();return;
        }
        dt=std::min(dt,.05f);
        const float norm=std::hypot(dx,dy);
        const bool headingValid=norm>.9f;
        const float x=headingValid?dx/norm:0.f;
        const float y=headingValid?dy/norm:0.f;
        // A new touch starts uncharged; reorientation while held retains
        // charge and tapers it rather than throwing it away.
        if(headingValid && !latched){
            anchorDx=x;anchorDy=y;latched=true;
        }
        const float alignment=(latched&&headingValid)?
            std::clamp(anchorDx*x+anchorDy*y,-1.f,1.f):0.f;
        const float edge=smooth((radial-edgeStart)/(edgeFull-edgeStart));
        // Preserve an anti-rebound recovery window during a real turn.
        // While the vector re-aligns the reserve DISCHARGES continuously;
        // it cannot instantly begin charging again the moment alignment
        // crosses a threshold. A broad 180-degree turn waits longer.
        if(headingValid && radial>=edgeStart && alignment<.94f)
            recoverySeconds=std::max(recoverySeconds,.45f+.30f*(1.f-alignment));
        recoverySeconds=std::max(0.f,recoverySeconds-dt);
        const bool charging=headingValid && radial>=edgeStart &&
                            alignment>=.987f && recoverySeconds<=0.f;
        if(charging){
            stableSeconds=std::min(chargeSeconds,stableSeconds+dt*1.05f);
        }else{
            const float turnPenalty=alignment<.55f?3.8f:alignment<.93f?2.8f:1.15f;
            const float outerPenalty=edge>.001f?turnPenalty:2.3f;
            stableSeconds=std::max(0.f,stableSeconds-dt*outerPenalty);
        }
        // Charging takes several seconds even after reaching the rim.
        // A separate slower output makes both acceleration AND braking
        // continuous, with finite dt-independent derivatives.
        const float wanted=maximumBonus*edge*smooth(stableSeconds/chargeSeconds);
        const float rate=wanted>bonus?1.00f:1.75f;
        bonus+=(wanted-bonus)*(-std::expm1(-rate*dt));
        bonus=std::clamp(bonus,0.f,maximumBonus);
        if(latched && headingValid){
            // Rotate the reserve heading by a bounded angle per second.
            // Unlike linear interpolation this safely crosses 180 degrees:
            // opposite vectors cannot collapse into a zero vector.
            const float delta=std::atan2(anchorDx*y-anchorDy*x,
                                         anchorDx*x+anchorDy*y);
            const float yaw=std::clamp(delta,-2.65f*dt,2.65f*dt);
            const float c=std::cos(yaw),sn=std::sin(yaw);
            const float priorX=anchorDx,priorY=anchorDy;
            anchorDx=priorX*c-priorY*sn;
            anchorDy=priorX*sn+priorY*c;
        }
    }
};
// Movement always uses the SAME physical ShipMotion code as touch-on-hull.
// Joystick only supplies a unit force intention; its direction is rebuilt
// against the actual ship coordinates on every fixed physics substep.
struct MovePad {
    bool right=true;
    std::int64_t finger=-1;
    float dx=0,dy=0,magnitude=0,travel=0;
    SteeringHoldBoost holdBoost;
    // 30% more finger travel, centred further inward to avoid screen clipping.
    float cx(int w)const {return right?w-std::max(105.f,w*.26f):std::max(105.f,w*.26f);}
    float cy(int h)const {return h-std::max(144.f,h*.17f);}
    // More physical finger travel means finer control, not a sudden full-speed jump.
    float orbit(int w)const {return std::clamp(w*.2314f,91.f,148.f);}
    bool active()const {return finger>=0;}
    void cancel(){finger=-1;dx=dy=magnitude=travel=0;holdBoost.reset();}
    void tick(float dt){if(active())holdBoost.tick(dt,dx,dy,travel);else holdBoost.reset();}
    float effectiveMagnitude()const {return std::clamp(magnitude+holdBoost.bonus,0.f,SteeringHoldBoost::maximumRequested);}
    float speedMultiplier()const {return std::clamp(effectiveMagnitude(),1.f,SteeringHoldBoost::maximumRequested);}
    // ShipMotion uses desiredSpeed=min(maxSpeed, distance*12). Match that
    // canonical law at the rim so full throw still reaches max speed while
    // early travel remains slow; difficulty/inertia stays owned by ShipMotion.
    static float steeringCurve(float t){
        t=std::clamp(t,0.f,1.f);
        return t*t*t*(.70f+.30f*t); // half-throw ~11%, soft low-speed flight
    }
    static float reachFromMagnitude(float amount,float arenaWidth,int dangerIndex){
        if(!(arenaWidth>0))return 0.f;
        const float difficulty=std::clamp(dangerIndex,0,8)/8.f;
        const float maxSpeed=arenaWidth*(1.7f+1.4f*difficulty);
        return (maxSpeed/12.f)*std::clamp(amount,0.f,SteeringHoldBoost::maximumRequested);
    }
    float virtualReach(float arenaWidth,int dangerIndex)const {
        return reachFromMagnitude(effectiveMagnitude(),arenaWidth,dangerIndex);
    }
    bool handle(int type,std::int64_t id,float x,float y,int w,int h){
        if(active()){
            if(id!=finger)return false;
            if(type==SDL_FINGERUP){cancel();return true;}
            if(type==SDL_FINGERMOTION){update(x,y,w,h);return true;}
            return true;
        }
        if(type!=SDL_FINGERDOWN ||
           std::hypot(x-cx(w),y-cy(h))>orbit(w)+13.f)return false;
        finger=id;update(x,y,w,h);return true;
    }
    void update(float x,float y,int w,int h){
        const float ox=x-cx(w),oy=y-cy(h),len=std::hypot(ox,oy);
        const float dead=std::max(13.f,w*.022f);
        if(len<=dead){dx=dy=magnitude=travel=0;holdBoost.reset();return;}
        const float dirX=ox/len,dirY=oy/len;
        // Mild direction filtering in the violet joystick; real turns keep
        // immediate control and trigger the stable-vector reset in tick().
        if(dx*dirX+dy*dirY>.970f){
            const float ax=.68f*dx+.32f*dirX,ay=.68f*dy+.32f*dirY;
            const float n=std::hypot(ax,ay);
            dx=ax/n;dy=ay/n;
        }else{dx=dirX;dy=dirY;}
        travel=std::clamp(len/orbit(w),0.f,1.f);
        const float linear=std::clamp((len-dead)/std::max(1.f,orbit(w)-dead),0.f,1.f);
        // Ease-in: quarter throw ~5%, half throw ~22%, full throw 100%.
        magnitude=steeringCurve(linear);
    }
    void draw(SDL_Renderer*r,int w,int h)const {
        if(!r)return;
        SDL_BlendMode old;SDL_GetRenderDrawBlendMode(r,&old);
        SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
        const float x=cx(w),y=cy(h),radius=orbit(w);
        // Only the minimal control outline/knob is drawn. No grey or dark
        // gameplay exclusion circle; the actual maze remains fully visible.
        neonRing(r,x,y,radius,{191,163,255,112},true);
        const float px=x+dx*travel*radius*.66f,py=y+dy*travel*radius*.66f;
        SDL_SetRenderDrawColor(r,203,178,255,125);
        SDL_RenderDrawLine(r,int(x),int(y),int(px),int(py));
        neonDisk(r,px,py,radius*.29f,{174,148,250,53});
        neonRing(r,px,py,radius*.29f,{203,180,255,146},false);
        SDL_SetRenderDrawBlendMode(r,old);
    }
};
// Finger-on-hull uses the same zero, progressive curve and physical intent
// as the violet stick, but with its origin at the INITIAL hull touch.
// The camera can shift indefinitely without changing an untouched command.
struct HullTouchPilot {
    bool down=false;
    float anchorX=0,anchorY=0,dx=0,dy=0,magnitude=0,travel=0;
    SteeringHoldBoost holdBoost;
    void cancel(){down=false;dx=dy=magnitude=travel=0;holdBoost.reset();}
    void begin(float x,float y){cancel();down=true;anchorX=x;anchorY=y;}
    void tick(float dt){if(down)holdBoost.tick(dt,dx,dy,travel);else holdBoost.reset();}
    float effectiveMagnitude()const {return std::clamp(magnitude+holdBoost.bonus,0.f,SteeringHoldBoost::maximumRequested);}
    float speedMultiplier()const {return std::clamp(effectiveMagnitude(),1.f,SteeringHoldBoost::maximumRequested);}
    // Smaller on-hull arc than the violet stick: finger stays nearer ship.
    float orbit(int screenWidth)const {return std::clamp(screenWidth*.125f,52.f,82.f);}
    void update(float x,float y,int screenWidth){
        if(!down)return;
        const float ox=x-anchorX,oy=y-anchorY,len=std::hypot(ox,oy);
        const float dead=std::max(13.f,screenWidth*.022f);
        if(len<=dead){dx=dy=magnitude=travel=0;holdBoost.reset();return;}
        dx=ox/len;dy=oy/len;
        const float radius=orbit(screenWidth);
        travel=std::clamp((len-dead)/std::max(1.f,radius-dead),0.f,1.f);
        magnitude=MovePad::steeringCurve(travel);
    }
    float virtualReach(float width,int danger)const {
        return MovePad::reachFromMagnitude(effectiveMagnitude(),width,danger);
    }
};
} // namespace sfsolo
