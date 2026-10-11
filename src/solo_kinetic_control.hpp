#pragma once
// SOLO's temporary touch/charge bridge reuses the actual COOP/DUEL kinetic
// surge state machine, cone geometry and difficulty calibration.
// It does not replace the historical ship/weapon/FX runtime. Final integration
// must still bind the canonical COOP ship through solo_canonical_adapter.hpp.
#include "solo_session.hpp"
#include "kinetic_shield.hpp"
#include "solo_vector_aim.hpp"
#include <algorithm>
#include <cmath>

namespace sfsolo {
struct KineticSurgeControl {
    // The SOLO ship starts at the bottom and faces up: canonical owner 1.
    static constexpr int owner=1;
    bool pressed=false;
    // Short visible plasma afterglow AFTER releasing the charged cone, so
    // the blast does not vanish in the same frame as the finger lift.
    float burstSeconds=0.0f,burstOriginX=0.0f,burstOriginY=0.0f;
    AimVector burstAxis{0,-1};
    float targetX=0,targetY=0;
    bool hasAim=false;
    AimVector axis{0,-1};
    // SOLO-only experiment converting canonical arena-fraction melt rate
    // into tile visual mass. The shared COOP rates stay untouched.
    static constexpr float SOLO_TILE_REFERENCE_ARENA_FRACTION=.08f;

    void aimAt(const Session &s,float x,float y){
        targetX=x;targetY=y;hasAim=true;
        axis=directionTo(s.pilot.x,s.pilot.y,x,y,axis);
    }
    AimVector direction(const Session &s) const {
        return hasAim ? directionTo(s.pilot.x,s.pilot.y,targetX,targetY,axis) : axis;
    }

    void press(){
        if(pressed)return;
        sfKineticSurgePress(owner);
        pressed=true;
    }
    void advance(float dt){
        dt=std::clamp(dt,0.0f,.05f);
        burstSeconds=std::max(0.0f,burstSeconds-dt);
        if(!pressed)return;
        sfKineticAdvanceSurges(dt);
    }
    // Active from canonical 0.20 s, including after READY at 2 s.
    // All asteroids inside the moving cone erode simultaneously.
    void advance(Session &s,float dt){
        advance(dt);
        if(!pressed||!sfKineticSuperchargeActive(owner)||dt<=0.0f||
           (s.phase!=Phase::Flying&&s.phase!=Phase::BossFight))return;
        const float range=coneRangeForShip(s.shipRadius);
        if(range<=0.0f)return;
        const AimVector dir=direction(s);
        const int left=std::max(0,int(std::floor(s.pilot.x-range-1)));
        const int right=std::min(s.map.width-1,int(std::ceil(s.pilot.x+range+1)));
        const int top=std::max(0,int(std::floor(s.pilot.y-range-1)));
        const int bottom=std::min(s.map.height-1,int(std::ceil(s.pilot.y+range+1)));
        for(int y=top;y<=bottom;++y)for(int x=left;x<=right;++x){
            if(s.asteroidIntegrityAt(x,y)<=0.0f)continue;
            const float cx=x+.5f,cy=y+.5f;
            if(!inAimedCanonicalCone(s.pilot.x,s.pilot.y,cx,cy,dir,range,.15f))
                continue;
            const float distance=std::clamp(
                std::hypot(cx-s.pilot.x,cy-s.pilot.y)/range,0.0f,1.0f);
            const float loss=sfKineticMiningArenaFractionPerSecond(distance)*
                            dt/SOLO_TILE_REFERENCE_ARENA_FRACTION;
            s.erodeAsteroid(x,y,loss);
        }
    }
    void cancel(){
        pressed=false;
        burstSeconds=0;
        sfKineticSurgeCancel(owner);
    }
    // At 2s the final sound/FX fires; no instant deletion of intact rocks.
    // True COOP energy, shield and minage FX remain for later integration.
    bool release(Session &s){
        if(!pressed)return false;
        pressed=false;
        const bool charged=sfKineticSurgeRelease(owner);
        if(!charged)return false;
        if(s.phase!=Phase::Flying && s.phase!=Phase::BossFight)return true;
        burstSeconds=.42f;
        burstOriginX=s.pilot.x;burstOriginY=s.pilot.y;
        burstAxis=direction(s);
        if(s.canonicalShipActive){
            // Same historical sfKineticPurgeAsteroids rule: radial burst,
            // radius = ship diameter * BLAST_DIAMETER * .5, independent of
            // mining cone Danger scale; walls are NOT destructible.
            const float radius=2.0f*s.shipRadius*
                  SF_KINETIC_SURGE_BLAST_DIAMETER*.5f;
            const int minX=std::max(0,int(std::floor(s.pilot.x-radius-1)));
            const int maxX=std::min(s.map.width-1,int(std::ceil(s.pilot.x+radius+1)));
            const int minY=std::max(0,int(std::floor(s.pilot.y-radius-1)));
            const int maxY=std::min(s.map.height-1,int(std::ceil(s.pilot.y+radius+1)));
            int purged=0;
            for(int y=minY;y<=maxY;++y)for(int x=minX;x<=maxX;++x){
                if(s.asteroidIntegrityAt(x,y)<=0)continue;
                // Half of a tile models the original asteroid sprite radius.
                if(std::hypot(x+.5f-s.pilot.x,y+.5f-s.pilot.y)
                                      >radius+.5f)continue;
                if(s.erodeAsteroid(x,y,1.0f))++purged;
            }
            if(purged>0)
                sfKineticTriggerWave(owner,
                    SF_KINETIC_SURGE_BLAST_DIAMETER*.5f,
                    1.0f,s.energyFraction());
        }
        return true;
    }
};
} // namespace sfsolo
