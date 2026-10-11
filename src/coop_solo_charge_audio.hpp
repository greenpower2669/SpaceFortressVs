#pragma once
// SOLO diagnostic audio gate. The historical kinetic phase transition is at
// 0.20 s, NEVER when the second finger initially goes down. READY at 2.00 s
// stays owned by sfKineticAudioUpdate() in the real COOP tick.
#include "kinetic_shield.hpp"

struct SfCoopSoloChargeCueGate {
    bool chargeStarted=false;
    void reset() {chargeStarted=false;}
    bool startWhenVisible(const SfKineticSurgeState &surge){
        if(!surge.held){reset();return false;}
        if(chargeStarted || surge.heldSeconds<SF_KINETIC_SURGE_VISIBLE_DELAY_SECONDS)
            return false;
        chargeStarted=true;return true;
    }
};
