#pragma once
// BONUS CORE-012. Session-owned SOLO data: no SDL, sprite or FabLib globals.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

namespace sfsolo::bonus {
enum class Kind:std::uint8_t {Health,Energy,Stealth,Missiles,Prison,Ice,Lava,Meteor,None};
inline constexpr bool isOrb(Kind k){return k>=Kind::Prison && k<=Kind::Meteor;}
inline constexpr unsigned salvoSize=3,grantCharges=3,maxCharges=6;
inline constexpr double stealthSeconds=5,missileSeconds=3,prisonSeconds=2,
    iceSeconds=4,lavaSeconds=4,pickupSeconds=45;
inline constexpr float lavaDps=24,meteorDamage=120,orbDamage=12;
inline constexpr std::size_t maxPickups=24;
inline const char *name(Kind k){
    constexpr const char *names[]={"PV","ENERGIE","FURTIF","MISSILES","PRISON","GLACE","LAVE","METEORE","AUCUN"};
    const auto n=unsigned(k);return names[n<9?n:8];
}
struct Pickup {std::uint32_t id;Kind kind;float x,y,radius;double expires;};
struct State {
    double time=0;
    Kind orb=Kind::None;
    unsigned charges=0;
    std::vector<Pickup> pickups;
    std::array<double,8> until{};
    unsigned dropped=0,encounter=0;
    std::uint32_t nextId=1;

    double remaining(Kind k) const {
        return unsigned(k)<until.size()?std::max(0.0,until[unsigned(k)]-time):0;
    }
    bool stealth() const {return remaining(Kind::Stealth)>0;}
    bool missiles() const {return remaining(Kind::Missiles)>0;}
    void grant(Kind k,float &health,float &heat){
        if(health<=0 || !std::isfinite(health))return;
        switch(k){
        case Kind::Health:health=std::clamp(health+350.0f,0.0f,1000.0f);break;
        case Kind::Energy:heat=std::clamp((std::isfinite(heat)?heat:50.0f)-25.0f,0.0f,50.0f);break;
        case Kind::Stealth:until[unsigned(k)]=time+stealthSeconds;break;
        case Kind::Missiles:until[unsigned(k)]=time+missileSeconds;break;
        default:if(isOrb(k)){charges=orb==k?std::min(maxCharges,charges+grantCharges):grantCharges;orb=k;}break;
        }
    }
    bool consumeSalvo(bool bossAvailable){
        if(!bossAvailable || missiles() || !charges || !isOrb(orb))return false;
        --charges;return true;
    }
    void hitBoss(Kind k){
        if(k==Kind::Prison)until[unsigned(k)]=time+prisonSeconds;
        if(k==Kind::Ice)until[unsigned(k)]=time+iceSeconds;
        if(k==Kind::Lava)until[unsigned(k)]=time+lavaSeconds;
    }
    float bossSpeedScale() const {
        return remaining(Kind::Prison)>0?0.0f:remaining(Kind::Ice)>0?.4f:1.0f;
    }
    // Integrate the exact active portion, including a frame crossing expiry.
    float advance(double dt){
        if(!std::isfinite(dt)||dt<=0)return 0;
        const float damage=float(std::min(dt,remaining(Kind::Lava))*lavaDps);
        time+=dt;
        pickups.erase(std::remove_if(pickups.begin(),pickups.end(),[&](const Pickup &p){return p.expires<=time;}),pickups.end());
        return damage;
    }
    void clearBoss(){
        for(Kind k:{Kind::Prison,Kind::Ice,Kind::Lava})until[unsigned(k)]=0;
    }
    void beginEncounter(){clearBoss();dropped=0;++encounter;}
    std::vector<Kind> drops(float health,float maximum){
        std::vector<Kind> result;
        if(!std::isfinite(health)||!std::isfinite(maximum)||maximum<=0)return result;
        constexpr float thresholds[]={.75f,.50f,.25f,0};
        constexpr Kind loot[][3]={{Kind::Health,Kind::Energy,Kind::Prison},
            {Kind::Stealth,Kind::Missiles,Kind::Ice},{Kind::Lava,Kind::Meteor,Kind::Health},
            {Kind::Energy,Kind::Stealth,Kind::Missiles}};
        for(unsigned i=0;i<4;++i)if(!(dropped&(1u<<i)) && health<=maximum*thresholds[i]){
            dropped|=1u<<i;result.insert(result.end(),std::begin(loot[i]),std::end(loot[i]));
        }
        return result;
    }
    bool addPickup(Kind k,float x,float y,float radius){
        if(k==Kind::None || unsigned(k)>unsigned(Kind::None) || pickups.size()>=maxPickups ||
           !std::isfinite(x)||!std::isfinite(y)||!std::isfinite(radius)||radius<=0)return false;
        pickups.push_back({nextId++,k,x,y,radius,time+pickupSeconds});return true;
    }
    unsigned collect(float x,float y,float shipRadius,float &health,float &heat){
        if(health<=0||!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(shipRadius)||shipRadius<0)return 0;
        unsigned collected=0;
        pickups.erase(std::remove_if(pickups.begin(),pickups.end(),[&](const Pickup &p){
            if(p.expires<=time)return true;
            if(std::hypot(x-p.x,y-p.y)>shipRadius+p.radius)return false;
            grant(p.kind,health,heat);++collected;return true;
        }),pickups.end());
        return collected;
    }
    void reset(){*this=State{};}
};
} // namespace sfsolo::bonus
