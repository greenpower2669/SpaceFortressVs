#pragma once
// Same kinetic charge threshold, mining range and widening cone from the
// historical COOP code. Only the *axis* is now freely aimed at 360 degrees.
#include "kinetic_shield.hpp"
#include <algorithm>
#include <cmath>

namespace sfsolo {
struct AimVector {
    float x=0,y=-1;
};
inline AimVector directionTo(float shipX,float shipY,float targetX,float targetY,
                              AimVector fallback={0,-1}){
    const float dx=targetX-shipX,dy=targetY-shipY;
    const float length=std::hypot(dx,dy);
    if(length<=.0001f)return fallback;
    return {dx/length,dy/length};
}
inline bool inAimedCanonicalCone(float shipX,float shipY,float pointX,float pointY,
                                AimVector axis,float range,float padding=0){
    if(range<=0)return false;
    const float dx=pointX-shipX,dy=pointY-shipY;
    const float forward=dx*axis.x+dy*axis.y;
    if(forward<=0)return false;
    if(std::hypot(dx,dy)>range+std::max(0.0f,padding))return false;
    const float side=std::abs(dx*axis.y-dy*axis.x);
    return side<=sfKineticSurgeConeHalfWidth(forward)+
                 std::max(0.0f,padding);
}
inline float coneRangeForShip(float shipRadius) {
    return 2.0f*shipRadius*sfKineticSurgeMiningRangeDiameters();
}
} // namespace sfsolo
