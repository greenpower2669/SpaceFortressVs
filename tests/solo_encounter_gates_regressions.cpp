#include "solo_encounter_gates.hpp"
#include <cassert>
#include <cstdio>
struct Minion{
    sfsolo::CoopWorldPoint anchor{},position{};
    int pack=-1;float health=180;
};
int main(){
    using namespace sfsolo;
    std::vector<Minion> e={{{200,200},{200,200},-1,180},
        {{800,200},{800,200},4,180},{{880,210},{880,210},4,180},
        {{1400,800},{1400,800},-1,180}};
    EncounterGates g;g.create(e,540);assert(g.zones.size()==3);
    assert(std::abs(g.zones[0].radius-540.f*.52f*1.5f)<.01f);
    const float base=std::max(540.f*.52f,
        std::hypot(e[1].anchor.x-840.f,e[1].anchor.y-205.f)+540.f*.32f);
    assert(std::abs(g.zones[1].radius-base*1.5f)<.01f);
    g.tick({2000,2000},e);assert(!g.closed());
    g.tick({210,200},e);assert(g.closed()&&g.closed()->members.size()==1);
    e[0].health=0;g.tick({210,200},e);assert(!g.closed());
    g.tick({810,215},e);assert(g.closed()&&g.closed()->members.size()==2);
    e[1].health=0;g.tick({810,215},e);assert(g.closed());
    e[2].health=0;g.tick({810,215},e);assert(!g.closed());
    g.tick({1400,800},e);assert(g.closed());
    puts("PASS local mini/horde locks and opening after last defeat");
}