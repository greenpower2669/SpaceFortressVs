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

// Movement always uses the SAME physical ShipMotion code as touch-on-hull.
// Joystick only supplies a unit force intention; its direction is rebuilt
// against the actual ship coordinates on every fixed physics substep.
struct MovePad {
    bool right=true;
    std::int64_t finger=-1;
    float dx=0,dy=0,magnitude=0,travel=0;
    float cx(int w)const {return right?w-std::max(92.f,w*.20f):std::max(92.f,w*.20f);}
    float cy(int h)const {return h-std::max(144.f,h*.17f);}
    // More physical finger travel means finer control, not a sudden full-speed jump.
    float orbit(int w)const {return std::clamp(w*.178f,70.f,114.f);}
    bool active()const {return finger>=0;}
    void cancel(){finger=-1;dx=dy=magnitude=travel=0;}
    // ShipMotion uses desiredSpeed=min(maxSpeed, distance*12). Match that
    // canonical law at the rim so full throw still reaches max speed while
    // early travel remains slow; difficulty/inertia stays owned by ShipMotion.
    float virtualReach(float arenaWidth,int dangerIndex)const {
        if(!(arenaWidth>0))return 0.f;
        const float difficulty=std::clamp(dangerIndex,0,8)/8.f;
        const float maxSpeed=arenaWidth*(1.7f+1.4f*difficulty);
        return (maxSpeed/12.f)*magnitude;
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
        if(len<=dead){dx=dy=magnitude=travel=0;return;}
        dx=ox/len;dy=oy/len;
        travel=std::clamp(len/orbit(w),0.f,1.f);
        const float linear=std::clamp((len-dead)/std::max(1.f,orbit(w)-dead),0.f,1.f);
        // Ease-in: quarter throw ~5%, half throw ~22%, full throw 100%.
        magnitude=linear*linear*(.75f+.25f*linear);
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
} // namespace sfsolo
