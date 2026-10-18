#pragma once
#include <algorithm>
#include <cmath>

namespace sfsolo {
// SOLO-026 only. Walls use the normal component of velocity, never the
// tangential component: sliding is safe, a fast head-on crash is costly.
// The call site owns the contact cooldown and writes the real COOP resources.
struct WallPenalty {float hull=0,energy=0;};
inline WallPenalty wallPenalty(float vx,float vy,float nx,float ny,float combatWidth){
    if(!(combatWidth>0)||!std::isfinite(vx)||!std::isfinite(vy)||
       !std::isfinite(nx)||!std::isfinite(ny))return {};
    const float normalLength=std::hypot(nx,ny);
    if(normalLength<=.0001f)return {};
    const float closing=std::max(0.0f,-(vx*nx+vy*ny)/normalLength);
    const float ratio=closing/combatWidth;
    if(ratio<=.25f)return {}; // touching or sliding is not a new crash
    const float severity=std::clamp((ratio-.25f)/2.25f,0.0f,1.0f);
    return {10.0f+110.0f*severity*severity,2.0f+16.0f*severity};
}
} // namespace sfsolo
