#include "../src/solo_bonus_core.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <limits>
#include <vector>

using sfsolo::bonus::Kind;
using sfsolo::bonus::State;

static bool close(double a,double b,double tolerance=1e-5){
    return std::abs(a-b)<=tolerance;
}

// These tests express Fab's BONUS CORE-012 contract, independently of the
// runtime adapter. Actual historical fire/collisions are tested separately.
static void restoresBoundedResourcesWithoutRevival(){
    State s;float hp=100,heat=40;
    s.grant(Kind::Health,hp,heat);
    assert(hp==450 && heat==40);
    hp=900;s.grant(Kind::Health,hp,heat);assert(hp==1000);
    s.grant(Kind::Health,hp,heat);assert(hp==1000);
    s.grant(Kind::Energy,hp,heat);assert(heat==15 && hp==1000);
    s.grant(Kind::Energy,hp,heat);assert(heat==0);
    s.grant(Kind::Energy,hp,heat);assert(heat==0);
    hp=0;s.grant(Kind::Health,hp,heat);assert(hp==0);
    hp=-10;s.grant(Kind::Health,hp,heat);assert(hp<=0);
}

static void timedPowersExpireAndRefreshWithoutAddition(){
    State s;float hp=1000,heat=30;
    s.grant(Kind::Stealth,hp,heat);
    s.grant(Kind::Missiles,hp,heat);
    assert(s.stealth() && s.missiles());
    assert(close(s.remaining(Kind::Stealth),5));
    assert(close(s.remaining(Kind::Missiles),3));
    s.advance(2);
    assert(close(s.remaining(Kind::Stealth),3));
    assert(close(s.remaining(Kind::Missiles),1));
    s.grant(Kind::Stealth,hp,heat);
    s.grant(Kind::Missiles,hp,heat);
    assert(close(s.remaining(Kind::Stealth),5));
    assert(close(s.remaining(Kind::Missiles),3));
    s.advance(3);
    assert(s.stealth() && !s.missiles());
    assert(close(s.remaining(Kind::Stealth),2));
    assert(s.remaining(Kind::Missiles)==0);
    s.advance(2);
    assert(!s.stealth() && !s.missiles());
    assert(s.remaining(Kind::Stealth)==0);
    assert(hp==1000 && heat==30);
}

static void noClockAdvanceMeansNoExpiryAndInvalidStepsAreIgnored(){
    State s;float hp=1000,heat=0;
    s.grant(Kind::Stealth,hp,heat);s.hitBoss(Kind::Lava);
    for(double dt:{0.0,-1.0,std::numeric_limits<double>::infinity(),
                   std::numeric_limits<double>::quiet_NaN()}){
        assert(s.advance(dt)==0);
        assert(s.time==0 && s.remaining(Kind::Stealth)==5);
    }
    // A paused application simply does not call advance: queries are pure.
    for(int i=0;i<100;++i)assert(s.stealth() && s.time==0);
}

static void orbInventoryHasOneConsumptionPerAcceptedSalvo(){
    for(Kind kind:{Kind::Prison,Kind::Ice,Kind::Lava,Kind::Meteor}){
        State s;float hp=1000,heat=37;
        s.grant(kind,hp,heat);assert(s.orb==kind && s.charges==3);
        assert(!s.consumeSalvo(false) && s.charges==3);
        s.grant(kind,hp,heat);assert(s.charges==6);
        s.grant(kind,hp,heat);assert(s.charges==6);
        s.grant(Kind::Missiles,hp,heat);
        assert(!s.consumeSalvo(true) && s.charges==6);
        s.advance(3);
        for(unsigned n=6;n>0;--n){
            assert(s.consumeSalvo(true));assert(s.charges==n-1);
        }
        assert(!s.consumeSalvo(true));
        assert(hp==1000 && heat==37); // expenditure belongs to real fire
    }
    State s;float hp=1000,heat=0;
    s.grant(Kind::Prison,hp,heat);s.grant(Kind::Prison,hp,heat);
    s.grant(Kind::Ice,hp,heat);
    assert(s.orb==Kind::Ice && s.charges==3);
    s.grant(Kind::Stealth,hp,heat);
    assert(s.orb==Kind::Ice && s.charges==3 && s.stealth());
}

static void bossStatusesCombineAndExpireIndependently(){
    State s;
    assert(s.bossSpeedScale()==1);
    s.hitBoss(Kind::Ice);assert(close(s.bossSpeedScale(),.4));
    s.hitBoss(Kind::Prison);assert(s.bossSpeedScale()==0);
    s.advance(2);assert(close(s.bossSpeedScale(),.4));
    s.advance(2);assert(s.bossSpeedScale()==1);
    s.hitBoss(Kind::Prison);s.advance(1);s.hitBoss(Kind::Prison);
    s.advance(1);assert(s.bossSpeedScale()==0);
    s.advance(1);assert(s.bossSpeedScale()==1);
    s.hitBoss(Kind::Meteor);assert(s.bossSpeedScale()==1);
}

static void lavaDamageUsesOnlyActiveTimeAndNeverStacksDps(){
    State single;
    single.hitBoss(Kind::Lava);
    assert(close(single.advance(10),96)); // 4 seconds, not the whole frame
    assert(single.advance(1)==0);
    State refreshed;
    refreshed.hitBoss(Kind::Lava);
    assert(close(refreshed.advance(1),24));
    refreshed.hitBoss(Kind::Lava);
    assert(close(refreshed.advance(4),96)); // refresh, never 48 damage/s
    assert(refreshed.advance(.1)==0);
    for(int hz:{30,60,120}){
        State partitioned;partitioned.hitBoss(Kind::Lava);
        double damage=0;
        for(int frame=0;frame<hz*5;++frame)damage+=partitioned.advance(1.0/hz);
        assert(close(damage,96,1e-3));
    }
}

static void dropsCrossEveryThresholdOncePerEncounter(){
    State s;s.beginEncounter();
    assert(s.drops(1000,1000).empty());
    assert(s.drops(751,1000).empty());
    assert(s.drops(750,1000)==std::vector<Kind>({Kind::Health,Kind::Energy,Kind::Prison}));
    assert(s.drops(749,1000).empty());
    assert(s.drops(1000,1000).empty()); // healing cannot rearm the threshold
    assert(s.drops(750,1000).empty());
    assert(s.drops(500,1000)==std::vector<Kind>({Kind::Stealth,Kind::Missiles,Kind::Ice}));
    assert(s.drops(250,1000)==std::vector<Kind>({Kind::Lava,Kind::Meteor,Kind::Health}));
    assert(s.drops(0,1000)==std::vector<Kind>({Kind::Energy,Kind::Stealth,Kind::Missiles}));
    assert(s.drops(-1,1000).empty());
    s.beginEncounter();
    const auto simultaneous=s.drops(0,1000);
    assert(simultaneous.size()==12);
    assert(s.drops(0,1000).empty());
    s.beginEncounter();
    assert(s.drops(0,0).empty());
    assert(s.drops(0,-1).empty());
    assert(s.drops(0,std::numeric_limits<float>::quiet_NaN()).empty());
    assert(s.drops(std::numeric_limits<float>::quiet_NaN(),1000).empty());
    assert(s.drops(0,1000).size()==12); // invalid measurements consume nothing
}

static void contactCollectsOnceAndExpiredPickupsCannotBeCollected(){
    State s;float hp=100,heat=50;
    assert(s.addPickup(Kind::Health,100,100,10));
    assert(s.addPickup(Kind::Energy,160,100,10));
    assert(s.collect(70,100,15,hp,heat)==0);
    assert(s.collect(75,100,15,hp,heat)==1); // touching circle edges
    assert(hp==450 && heat==50 && s.pickups.size()==1);
    assert(s.collect(100,100,15,hp,heat)==0);
    assert(s.collect(160,100,15,hp,heat)==1);
    assert(hp==450 && heat==25 && s.pickups.empty());
    assert(s.addPickup(Kind::Health,100,100,10));
    s.advance(45);
    assert(s.collect(100,100,15,hp,heat)==0 && hp==450);
    assert(s.pickups.empty());
}

static void boundedPickupStorageAndFullSessionReset(){
    State s;float hp=1000,heat=0;
    for(int i=0;i<24;++i)assert(s.addPickup(Kind::Energy,float(i*20),100,5));
    assert(!s.addPickup(Kind::Energy,700,100,5));
    assert(s.pickups.size()==24);
    for(std::size_t i=0;i<s.pickups.size();++i)
        for(std::size_t j=i+1;j<s.pickups.size();++j)
            assert(s.pickups[i].id!=s.pickups[j].id);
    s.grant(Kind::Prison,hp,heat);s.grant(Kind::Missiles,hp,heat);
    s.grant(Kind::Stealth,hp,heat);s.hitBoss(Kind::Prison);s.hitBoss(Kind::Lava);
    s.advance(1);s.reset();
    assert(s.time==0 && s.pickups.empty() && s.charges==0 && s.orb==Kind::None);
    assert(!s.stealth() && !s.missiles() && s.bossSpeedScale()==1);
    assert(s.advance(1)==0 && s.drops(0,1000).size()==12);
}

int main(){
    restoresBoundedResourcesWithoutRevival();
    timedPowersExpireAndRefreshWithoutAddition();
    noClockAdvanceMeansNoExpiryAndInvalidStepsAreIgnored();
    orbInventoryHasOneConsumptionPerAcceptedSalvo();
    bossStatusesCombineAndExpireIndependently();
    lavaDamageUsesOnlyActiveTimeAndNeverStacksDps();
    dropsCrossEveryThresholdOncePerEncounter();
    contactCollectsOnceAndExpiredPickupsCannotBeCollected();
    boundedPickupStorageAndFullSessionReset();
    std::puts("PASS: SOLO BONUS CORE-012 resource bounds, exact timers, inventory, statuses, drops, contacts and lifecycle");
}
