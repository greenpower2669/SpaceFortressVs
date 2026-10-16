#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace sfsolo {
// Selection is an explicit short hull tap. Once selected the next two touches
// belong to zoom from DOWN until both UPs: no speculative weapon/charge events.
struct ZoomGesture {
    using Id=std::int64_t;
    bool selected=false,candidate=false,pinching=false;
    Id first=-1,second=-1;
    float ax=0,ay=0,bx=0,by=0,startX=0,startY=0,startTime=0,startDistance=0,startZoom=1,zoom=1;
    void reset(){*this=ZoomGesture{};}
    bool owns(Id id)const{return id>=0&&(id==first||id==second);}
    bool down(Id id,float x,float y,float time,float shipX,float shipY,float radius){
        if(!selected){
            if(first>=0||std::hypot(x-shipX,y-shipY)>radius)return false;
            candidate=true;first=id;ax=startX=x;ay=startY=y;startTime=time;return true;
        }
        if(first<0&&!pinching){first=id;ax=x;ay=y;startTime=time;return true;}
        if(second<0&&id!=first&&!pinching){
            second=id;bx=x;by=y;startDistance=std::max(12.f,std::hypot(ax-bx,ay-by));
            startZoom=zoom;pinching=true;return true;
        }
        return true; // extra selected touches are never interpreted as weapons
    }
    bool motion(Id id,float x,float y,float){
        if(!owns(id))return selected;
        if(id==first){ax=x;ay=y;}else{bx=x;by=y;}
        if(candidate&&std::hypot(x-startX,y-startY)>18.f){candidate=false;first=-1;return false;}
        if(pinching&&first>=0&&second>=0)
            zoom=std::clamp(startZoom*std::hypot(ax-bx,ay-by)/startDistance,1.f,2.f);
        return true;
    }
    bool up(Id id,float x,float y,float time){
        if(!owns(id))return selected;
        const bool selecting=candidate;
        if(candidate && time-startTime<=.25f && std::hypot(x-startX,y-startY)<=18.f)selected=true;
        candidate=false;
        if(id==first)first=-1;else second=-1;
        if(first<0&&second<0){
            if(!pinching && !selecting)selected=false;
            pinching=false;
        }
        return true;
    }
};
}
