#pragma once
#include <SDL2/SDL.h>
#include <array>
#include <algorithm>
#include <cmath>
namespace sfsolo {
// One bounded GPU snapshot; no readback, no CPU image warping.
struct LairTransition {
    SDL_Texture *snapshot=nullptr,*previousTarget=nullptr;
    SDL_Renderer *renderer=nullptr;
    SDL_Rect previousViewport{},previousClip{};
    float previousScaleX=1,previousScaleY=1,focusX=.5f,focusY=.5f;
    bool clipEnabled=false,capturing=false;
    int width=0,height=0;
    ~LairTransition(){release();}
    void release(){if(snapshot)SDL_DestroyTexture(snapshot);snapshot=nullptr;capturing=false;}
    bool beginCapture(SDL_Renderer *r,int w,int h,float x,float y){
        release();renderer=r;width=w;height=h;
        focusX=std::clamp(x/float(w),0.0f,1.0f);focusY=std::clamp(y/float(h),0.0f,1.0f);
        if(!SDL_RenderTargetSupported(r))return false;
        const float scale=std::min(1.0f,2048.0f/std::max(w,h));
        snapshot=SDL_CreateTexture(r,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,
                                   std::max(1,int(w*scale)),std::max(1,int(h*scale)));
        if(!snapshot)return false;
        SDL_SetTextureBlendMode(snapshot,SDL_BLENDMODE_BLEND);
        previousTarget=SDL_GetRenderTarget(r);SDL_RenderGetViewport(r,&previousViewport);
        SDL_RenderGetScale(r,&previousScaleX,&previousScaleY);
        clipEnabled=SDL_RenderIsClipEnabled(r);SDL_RenderGetClipRect(r,&previousClip);
        if(SDL_SetRenderTarget(r,snapshot)!=0){release();return false;}
        SDL_RenderSetViewport(r,nullptr);SDL_RenderSetClipRect(r,nullptr);
        SDL_RenderSetScale(r,scale,scale);capturing=true;return true;
    }
    void endCapture(){
        if(!capturing)return;
        SDL_SetRenderTarget(renderer,previousTarget);
        SDL_RenderSetViewport(renderer,&previousViewport);
        SDL_RenderSetScale(renderer,previousScaleX,previousScaleY);
        SDL_RenderSetClipRect(renderer,clipEnabled?&previousClip:nullptr);capturing=false;
    }
    void draw(SDL_Renderer *r,int w,int h,float seconds){
        const float t=std::clamp(seconds,0.0f,1.0f);
        const float fade=std::clamp((t-.55f)/.45f,0.0f,1.0f);
        if(!snapshot){
            // SDL renderer without render targets: continuous fade, no hard cut.
            const float darkness=t<.65f?t/.65f:(1-t)/.35f;
            SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(r,12,5,32,Uint8(255*std::clamp(darkness,0.0f,1.0f)));
            SDL_RenderFillRect(r,nullptr);return;
        }
        constexpr int cols=16,rows=24;
        std::array<SDL_Vertex,(cols+1)*(rows+1)> vertices{};
        std::array<int,cols*rows*6> indices{};
        const float zoom=1+2.8f*t*t;
        const float cx=w*(focusX+(.5f-focusX)*t),cy=h*(focusY+(.5f-focusY)*t);
        for(int y=0;y<=rows;++y)for(int x=0;x<=cols;++x){
            const float u=float(x)/cols,v=float(y)/rows;
            const float dx=(u-focusX)*w,dy=(v-focusY)*h;
            const float radius=std::hypot(dx,dy)/std::max(1,std::min(w,h));
            const float ripple=1+.035f*std::sin(radius*22-t*13)*std::sin(t*3.14159265f);
            vertices[y*(cols+1)+x]={{cx+dx*zoom*ripple,cy+dy*zoom*ripple},
                {255,255,255,Uint8(255*(1-fade))},{u,v}};
        }
        int k=0;
        for(int y=0;y<rows;++y)for(int x=0;x<cols;++x){
            const int a=y*(cols+1)+x,b=a+1,c=a+cols+1,d=c+1;
            for(int i:{a,b,c,b,d,c})indices[k++]=i;
        }
        if(SDL_RenderGeometry(r,snapshot,vertices.data(),int(vertices.size()),indices.data(),k)!=0){
            const SDL_FRect dest{cx-focusX*w*zoom,cy-focusY*h*zoom,w*zoom,h*zoom};
            SDL_SetTextureAlphaMod(snapshot,Uint8(255*(1-fade)));
            SDL_RenderCopyF(r,snapshot,nullptr,&dest);SDL_SetTextureAlphaMod(snapshot,255);
        }
    }
};
}
