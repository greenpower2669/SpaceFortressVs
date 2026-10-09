#pragma once
// Portable SOLO progression. Storage is explicit: no filesystem access during
// game loop, no change to legacy campaign or Hall of Fame.
#include <array>
#include <cstdint>
#include <string>
#include <algorithm>

namespace sfsolo {
struct Progression {
    static constexpr unsigned stagesPerWorld=20;
    static constexpr unsigned initialWorlds=6;
    std::array<uint32_t,initialWorlds> completed{{}};
    unsigned highestUnlockedWorld() const {
        for(unsigned w=1;w<initialWorlds;w++)
            if(completed[w-1] != ((1u<<stagesPerWorld)-1u))return w;
        return initialWorlds;
    }
    bool unlocked(unsigned world,unsigned stage) const {
        if(world<1||world>initialWorlds||stage<1||stage>stagesPerWorld)return false;
        if(world>1 && completed[world-2]!=((1u<<stagesPerWorld)-1u))return false;
        if(stage==1)return true;
        return (completed[world-1] & (1u<<(stage-2)))!=0;
    }
    bool complete(unsigned world,unsigned stage,bool won){
        if(!won||!unlocked(world,stage))return false;
        completed[world-1] |= 1u<<(stage-1);
        return true;
    }
    bool isCompleted(unsigned world,unsigned stage) const {
        return world>=1&&world<=initialWorlds&&stage>=1&&stage<=stagesPerWorld&&
               (completed[world-1]&(1u<<(stage-1)))!=0;
    }
    std::string serialize() const {
        std::string s="SFSOLO1";
        for(uint32_t mask:completed){
            s.push_back(':');
            for(int shift=28;shift>=0;shift-=4){
                const unsigned nibble=(mask>>shift)&15u;
                s.push_back("0123456789ABCDEF"[nibble]);
            }
        }
        return s;
    }
    bool deserialize(const std::string &s){
        if(s.size()!=7+initialWorlds*9||s.compare(0,7,"SFSOLO1")!=0)return false;
        std::array<uint32_t,initialWorlds> parsed{};
        for(unsigned w=0;w<initialWorlds;w++){
            if(s[7+w*9]!=':')return false;
            uint32_t mask=0;
            for(unsigned n=0;n<8;n++){
                const char c=s[8+w*9+n];
                const size_t pos=std::string("0123456789ABCDEF").find(c);
                if(pos==std::string::npos)return false;
                mask=(mask<<4)|uint32_t(pos);
            }
            if(mask & ~((1u<<stagesPerWorld)-1u))return false;
            parsed[w]=mask;
        }
        // Each completed level must have all preceding levels completed.
        // Reject forged saves with holes before checking world progression.
        for(uint32_t mask:parsed){
            if(mask && (mask & (mask+1u))!=0u)return false;
        }
        // Reject saves that unlock future worlds without completing predecessors.
        for(unsigned w=1;w<initialWorlds;w++)
            if(parsed[w] && parsed[w-1]!=((1u<<stagesPerWorld)-1u))return false;
        completed=parsed;
        return true;
    }
};
} // namespace sfsolo
