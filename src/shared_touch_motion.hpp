#pragma once
// Exactly the bounded vector-to-target interpolation used by SpaceFortress
// classic human touch control; kept in one pure header for CLASSIC and SOLO.
#include <algorithm>
#include <cmath>
struct SfTouchMovementDelta {float x=0,y=0;};
inline SfTouchMovementDelta sfTouchMovementDelta(float dx,float dy,
                                                  float maxSpeed,float dt) {
    const float distance=std::hypot(dx,dy);
    if(distance<=.5f)return {dx,dy};
    const float step=std::min(distance,std::max(0.0f,maxSpeed)*std::max(0.0f,dt));
    return {dx/distance*step,dy/distance*step};
}
