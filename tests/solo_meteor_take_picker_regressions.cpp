#include "../src/solo_meteor_take_picker.hpp"
#include <cassert>
#include <array>
int main(){
    for(unsigned seed=1;seed<40;++seed){
        sfsolo::MeteorTakePicker picker(seed);
        int previous=-1;
        std::array<int,4> hits{};
        for(int n=0;n<400;++n){
            const int i=picker.next();
            assert(i>=0&&i<4&&i!=previous);
            assert(picker.volume()>=95&&picker.volume()<=112);
            ++hits[size_t(i)];
            previous=i;
        }
        for(auto n:hits)assert(n==100);
    }
}
