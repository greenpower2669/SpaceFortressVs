#pragma once
// SOLO-035 — craters cut into the EXISTING painted rock, not a second texture.
// Deterministic world-space relief, independent of chunk, camera, zoom or theme.
// Only RGB lighting is changed; no tile, collision mask or alpha changes.
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace sfsolo {
struct SoloCraterRelief {
    float shade=1.f, hollow=0.f, rim=0.f;

    static float smooth(float v) {
        v=std::clamp(v,0.f,1.f);
        return v*v*(3.f-2.f*v);
    }
    static std::uint32_t hash(std::uint32_t v) {
        v^=v>>16;
        v*=0x7feb352du;
        v^=v>>15;
        v*=0x846ca68bu;
        return v^(v>>16);
    }
    static float fraction(std::uint32_t v) {
        return float(v&0xffffu)/65535.f;
    }
    // Neighbouring lattice cells are visited so a crater remains continuous
    // even when its center is in a different 8x8 texture chunk.
    // Two independently jittered scales give isolated broad impacts and a
    // separate field of small impacts, rather than a repeating stamp atlas.
    static void layer(float x,float y,float spacing,std::uint32_t seed,
                      float &dark,float &bright,float &hollows,float &rims) {
        const int ix=int(std::floor(x/spacing));
        const int iy=int(std::floor(y/spacing));
        for(int cy=iy-1;cy<=iy+1;++cy)
            for(int cx=ix-1;cx<=ix+1;++cx) {
                std::uint32_t h=hash(std::uint32_t(cx)*0x9e3779b9u
                      ^ std::uint32_t(cy)*0x85ebca6bu ^ seed);
                if((h&255u)>159u)continue; // Empty patches, not a regular grid.
                const float jx=.16f+.68f*fraction(h=hash(h+0x51ed270bu));
                const float jy=.16f+.68f*fraction(h=hash(h+0x3c6ef372u));
                const float radius=spacing*(.115f+.205f*
                    fraction(h=hash(h+0xa54ff53au)));
                const float eccentricity=.78f+.44f*
                    fraction(h=hash(h+0x510e527fu));
                const float strength=.68f+.32f*
                    fraction(h=hash(h+0x1f83d9abu));
                const float dx=x-(cx+jx)*spacing;
                const float dy=y-(cy+jy)*spacing;
                if(std::abs(dx)>radius*1.4f || std::abs(dy)>radius*1.6f)
                    continue;
                const float distance=std::sqrt(
                    (dx*eccentricity)*(dx*eccentricity)+
                    (dy/eccentricity)*(dy/eccentricity))/radius;
                // Mineral-fractured rim: gently deformed in world space.
                const float rough=distance*(1.f+
                    .075f*std::sin(4.7f*x-3.8f*y)+
                    .048f*std::sin(9.1f*x+6.3f*y));
                if(rough>1.25f)continue;
                const float hollow=1.f-smooth((rough-.16f)/.74f);
                const float rim=smooth((rough-.69f)/.19f)*
                           (1.f-smooth((rough-1.01f)/.19f));
                // Light arrives from upper-left like the existing cliff lip.
                // The underlying lithic grain is retained even in the crater.
                const float facing=std::clamp(.72f-.28f*(dx+dy)/radius,
                                            .22f,1.12f);
                dark+=strength*(.235f*hollow+.060f*rim*(1.12f-facing));
                bright+=strength*.19f*rim*facing;
                hollows+=hollow*strength;
                rims+=rim*strength;
            }
    }
    static SoloCraterRelief at(float gx,float gy,float rockDepth) {
        // Craters disappear GRADUALLY into the cliff face; this is crucial at
        // the flat lower traverse and along all rock/opening transitions.
        const float blend=smooth((rockDepth-.105f)/.265f);
        if(blend<=0.f)return {};
        // Same world coordinates and source material for every theme.
        const float x=gx+.048f*std::sin(2.17f*gy+.79f*gx);
        const float y=gy+.048f*std::sin(1.81f*gx-2.63f*gy);
        float dark=0.f,bright=0.f,hollows=0.f,rims=0.f;
        layer(x,y,6.4f,0x915fd2c3u,dark,bright,hollows,rims);
        layer(x,y,2.6f,0x2fba74d1u,dark,bright,hollows,rims);
        return {std::clamp(1.f+blend*(bright-dark),.62f,1.23f),
                blend*hollows,blend*rims};
    }
};
} // namespace sfsolo
