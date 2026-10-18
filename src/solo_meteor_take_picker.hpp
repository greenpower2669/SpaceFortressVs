#pragma once
// SOLO-025: independent four-take shuffle bag for real meteor-on-rock WAVs.
// Avoids immediate repeats and avoids exhausting one take before the others.
// Stateless audio effect code only calls next() when an actual impact plays.
#include <array>
#include <cstdint>
#include <algorithm>
namespace sfsolo {
struct MeteorTakePicker {
    std::array<int,4> order{{0,1,2,3}};
    int cursor=4,last=-1;
    std::uint32_t seed=0x6d2b79f5u;
    explicit MeteorTakePicker(std::uint32_t value=0x6d2b79f5u):seed(value?value:1){}
    std::uint32_t random(){
        seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return seed;
    }
    int next(){
        if(cursor>=4){
            order={{0,1,2,3}};
            for(int i=3;i>0;--i){
                const int j=int(random()%std::uint32_t(i+1));
                std::swap(order[size_t(i)],order[size_t(j)]);
            }
            if(order[0]==last)std::swap(order[0],order[1]);
            cursor=0;
        }
        const int result=order[size_t(cursor++)];
        last=result;
        return result;
    }
    int volume(){
        return 112-int(random()%18u); // vary mildly but never mute impact
    }
};
}
