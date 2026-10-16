#pragma once
#include <SDL2/SDL.h>
#include <cmath>
#include <algorithm>
#include <cstdint>
namespace sfsolo {
// Eight discrete fire buttons (45 degrees) orbit a semi-transparent joystick.
// Centre release fires; a tap repeats the LAST nonzero aimed vector.
struct FirePad {
    struct Response {bool owned=false,fire=false;};
    bool right=false;
    std::int64_t finger=-1;
    bool stick=false;
    float dx=0,dy=-1,dragX=0,dragY=0;
    static constexpr float padPi=3.14159265358979323846f;
    float cx(int w)const {return right?w-std::max(100.f,w*.20f):std::max(100.f,w*.20f);}
    float cy(int h)const {return h-std::max(142.f,h*.17f);}
    float orbit(int w)const {return std::clamp(w*.15f,64.f,86.f);}
    void cancel(){finger=-1;stick=false;dragX=dragY=0;}
    void set(float x,float y){
        const float len=std::hypot(x,y);
        if(len>.001f){dx=x/len;dy=y/len;}
    }
    Response handle(int type,std::int64_t id,float x,float y,int w,int h){
        if(finger>=0){
            if(id!=finger)return {};
            if(type==SDL_FINGERMOTION&&stick){dragX=x-cx(w);dragY=y-cy(h);return {true,false};}
            if(type==SDL_FINGERUP){
                const bool shoot=stick;
                if(shoot&&std::hypot(dragX,dragY)>std::max(12.f,w*.025f))set(dragX,dragY);
                cancel();return {true,shoot};
            }
            return {true,false};
        }
        if(type!=SDL_FINGERDOWN)return {};
        const float px=cx(w),py=cy(h),r=orbit(w);
        for(int i=0;i<8;++i){
            const float a=i*padPi*.25f;
            if(std::hypot(x-px-r*std::cos(a),y-py-r*std::sin(a))<=std::max(19.f,w*.038f)){
                finger=id;stick=false;set(std::cos(a),std::sin(a));return {true,true};
            }
        }
        if(std::hypot(x-px,y-py)<=std::max(34.f,w*.068f)){
            finger=id;stick=true;dragX=dragY=0;return {true,false};
        }
        return {};
    }
    void draw(SDL_Renderer* renderer,int w,int h)const {
        if(!renderer)return;
        SDL_BlendMode old;SDL_GetRenderDrawBlendMode(renderer,&old);
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
        const float x=cx(w),y=cy(h),r=orbit(w);
        auto ring=[&](float px,float py,float radius,SDL_Color c){
            SDL_SetRenderDrawColor(renderer,c.r,c.g,c.b,c.a);
            for(int j=0;j<48;++j){
                const float a=j*padPi/24,b=(j+1)*padPi/24;
                SDL_RenderDrawLine(renderer,int(px+std::cos(a)*radius),int(py+std::sin(a)*radius),
                       int(px+std::cos(b)*radius),int(py+std::sin(b)*radius));
            }
        };
        ring(x,y,35,{120,220,255,170});
        ring(x,y,29,{120,220,255,70});
        for(int i=0;i<8;++i){
            const float a=i*padPi*.25f,bx=x+std::cos(a)*r,by=y+std::sin(a)*r;
            ring(bx,by,std::max(17.f,w*.032f),{140,220,255,170});
            SDL_SetRenderDrawColor(renderer,180,245,255,205);
            SDL_RenderDrawLine(renderer,int(bx-std::cos(a)*7),int(by-std::sin(a)*7),
                               int(bx+std::cos(a)*7),int(by+std::sin(a)*7));
        }
        const float dist=std::hypot(dragX,dragY);
        const float vx=stick&&dist>12?dragX/dist:dx,vy=stick&&dist>12?dragY/dist:dy;
        SDL_SetRenderDrawColor(renderer,230,245,255,225);
        SDL_RenderDrawLine(renderer,int(x),int(y),int(x+vx*28),int(y+vy*28));
        SDL_SetRenderDrawBlendMode(renderer,old);
    }
};
}
