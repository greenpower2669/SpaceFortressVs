#pragma once
// SOLO-025 - World-continuous domain warping and sparse mineral-blue pockets.
#include <algorithm>
#include <cmath>
namespace sfsolo {
struct RockMaterialSample {
    float sourceX=0,sourceY=0,altX=0,altY=0;
    float shade=1,altWeight=0,blueWeight=0;
};
inline RockMaterialSample soloRockMaterialAt(float x,float y){
    const float u=x+.36f*std::sin(y*1.17f+x*.23f)
                   +.17f*std::sin(y*3.47f-x*.73f);
    const float v=y+.28f*std::sin(x*1.51f-y*.41f)
                   +.13f*std::sin(x*3.09f+y*.67f);
    const float macro=.5f+.28f*std::sin(x*.39f+y*.24f)
                          +.21f*std::sin(x*.19f-y*.53f+1.2f);
    const float alt=std::clamp((macro-.43f)*.65f,0.f,.36f);
    const float shade=std::clamp(.86f+.105f*std::sin(x*.51f-y*.27f)
                           +.075f*std::sin(y*.84f+x*.32f+2.4f),.68f,1.07f);
    const float vein=.5f+.30f*std::sin(x*.59f+y*.21f+1.8f)
                          +.24f*std::sin(y*.34f-x*.77f);
    const float keep=std::clamp((vein-.80f)/.17f,0.f,1.f);
    return {u,v,v*.78f+u*.42f+3.7f,u*.91f-v*.36f-2.6f,shade,alt,keep};
}
}
