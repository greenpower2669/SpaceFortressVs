#pragma once
// SOLO's temporary touch/charge bridge reuses the actual COOP/DUEL kinetic
// surge state machine, cone geometry and difficulty calibration.
// It does not replace the historical ship/weapon/FX runtime. Final integration
// must still bind the canonical COOP ship through solo_canonical_adapter.hpp.
#include "solo_session.hpp"
#include "kinetic_shield.hpp"
#include <algorithm>
#include <cmath>

namespace sfsolo {
struct KineticSurgeControl {
    // The SOLO ship starts at the bottom and faces up: canonical owner 1.
    static constexpr int owner=1;
    bool pressed=false;

    void press(){
        if(pressed)return;
        sfKineticSurgePress(owner);
        pressed=true;
    }
    void advance(float dt){
        if(!pressed)return;
        sfKineticAdvanceSurges(std::clamp(dt,0.0f,.05f));
    }
    void cancel(){
        pressed=false;
        sfKineticSurgeCancel(owner);
    }
    // Return true when the canonical 2s surge was released; do NOT also fire.
    // The only world interaction here is harvesting nearby asteroid tiles.
    // Real particle suction, damage, sound and FX still belong to COOP.
    bool release(Session &s){
        if(!pressed)return false;
        pressed=false;
        const bool charged=sfKineticSurgeRelease(owner);
        if(!charged)return false;
        if(s.phase!=Phase::Flying && s.phase!=Phase::BossFight)return true;
        const float range=2.0f*s.shipRadius*
                          sfKineticSurgeMiningRangeDiameters();
        const int left=std::max(0,int(std::floor(s.pilot.x-range-1)));
        const int right=std::min(s.map.width-1,int(std::ceil(s.pilot.x+range+1)));
        const int top=std::max(0,int(std::floor(s.pilot.y-range-1)));
        const int bottom=std::min(s.map.height-1,int(std::ceil(s.pilot.y+range+1)));
        for(int y=top;y<=bottom;++y)for(int x=left;x<=right;++x){
            const size_t index=size_t(y)*size_t(s.map.width)+size_t(x);
            if(s.map.tiles[index]!=Tile::Asteroid)continue;
            if(sfKineticSurgeConeContains(owner,s.pilot.x,s.pilot.y,
                                          x+.5f,y+.5f,range,.15f)){
                s.map.tiles[index]=Tile::Empty;
            }
        }
        return true;
    }
};
} // namespace sfsolo
