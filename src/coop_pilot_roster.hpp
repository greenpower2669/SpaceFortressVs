#pragma once
// One physical participant is not the same as one alive and one dead.
// Default bitmask preserves both historical COOP players exactly.
#include <cstdint>
struct SfCoopPilotRoster {
    std::uint8_t activeMask=0x03;
    bool active(int owner) const {
        return owner>=0&&owner<2&&(activeMask&(1u<<owner))!=0;
    }
    int count() const {return int(active(0))+int(active(1));}
    bool anyAlive(float pv0,float pv1) const {
        return (active(0)&&pv0>0)||(active(1)&&pv1>0);
    }
    bool allDefeated(float pv0,float pv1) const {
        return !anyAlive(pv0,pv1);
    }
    static SfCoopPilotRoster single(int owner){
        SfCoopPilotRoster result;
        result.activeMask=owner==0?1u:owner==1?2u:0u;
        return result;
    }
};
