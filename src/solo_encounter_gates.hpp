#pragma once
#include "coop_solo_world_bridge.hpp"
#include <algorithm>
#include <cmath>
#include <vector>
namespace sfsolo {
// Combat portions, NOT the boss lair. One lock per lone mini or entire horde.
struct EncounterGate {
    int pack=-1;
    std::vector<unsigned> members;
    CoopWorldPoint centre{};
    float radius=0;
    bool cleared=false;
};
struct EncounterGates {
    std::vector<EncounterGate> zones;
    int active=-1;
    float width=1;
    void reset(){zones.clear();active=-1;}
    template<class Boss> void create(const std::vector<Boss> &enemies,float worldCombatWidth){
        reset();width=std::max(1.f,worldCombatWidth);
        for(unsigned i=0;i<enemies.size();++i){
            auto found=std::find_if(zones.begin(),zones.end(),[&](const EncounterGate &z){
                return enemies[i].pack>=0 && enemies[i].pack==z.pack;
            });
            if(found==zones.end()){zones.push_back({});found=zones.end()-1;found->pack=enemies[i].pack;}
            found->members.push_back(i);
        }
        for(auto &z:zones){
            for(unsigned i:z.members){
                z.centre.x+=enemies[i].anchor.x;
                z.centre.y+=enemies[i].anchor.y;
            }
            z.centre.x/=float(z.members.size());z.centre.y/=float(z.members.size());
            z.radius=width*.52f;
            for(unsigned i:z.members)
                z.radius=std::max(z.radius,std::hypot(enemies[i].anchor.x-z.centre.x,
                    enemies[i].anchor.y-z.centre.y)+width*.32f);
        }
    }
    template<class Boss> void tick(CoopWorldPoint player,const std::vector<Boss> &enemies){
        if(active>=0){
            EncounterGate &z=zones[size_t(active)];
            bool lives=false;
            for(unsigned i:z.members)if(i<enemies.size()&&enemies[i].health>0)lives=true;
            if(!lives){z.cleared=true;active=-1;}
            return;
        }
        float nearest=width*.31f;int chosen=-1;
        for(unsigned j=0;j<zones.size();++j){
            const auto &z=zones[j];if(z.cleared)continue;
            for(unsigned i:z.members)if(i<enemies.size()&&enemies[i].health>0){
                const float d=std::hypot(player.x-enemies[i].position.x,
                                         player.y-enemies[i].position.y);
                if(d<nearest){nearest=d;chosen=int(j);}
            }
        }
        if(chosen<0)return;
        active=chosen;
        auto &z=zones[size_t(active)];
        z.radius=std::max(z.radius,std::hypot(player.x-z.centre.x,
                                              player.y-z.centre.y)+width*.20f);
        for(unsigned i:z.members)if(i<enemies.size()&&enemies[i].health>0)
            z.radius=std::max(z.radius,std::hypot(enemies[i].position.x-z.centre.x,
                enemies[i].position.y-z.centre.y)+width*.20f);
    }
    const EncounterGate *closed()const{
        return active<0?nullptr:&zones[size_t(active)];
    }
};
}
