#pragma once
// SOLO interactive tutorial challenge logic; no changes to canonical COOP gameplay.
#include <algorithm>
#include <array>
#include <cstdint>

namespace sfsolo {
enum class TutorialChallenge : uint8_t {
    Slalom, TargetPractice, ReadGauges, MineAsteroid,
    KineticShield, Supercharge, RescueAlly, MiniBoss, Count
};
enum class TutorialEvent : uint8_t {
    PassedGate, HitTarget, InspectedGauges, MinedDust,
    BlockedHit, ReleasedSurge, RepairedAlly, DefeatedMiniBoss
};
enum class TutorialState : uint8_t { Waiting, Running, Completed };
struct TutorialProgress {
    TutorialChallenge challenge=TutorialChallenge::Slalom;
    TutorialState state=TutorialState::Waiting;
    unsigned progress=0;
    unsigned mistakes=0;
    float elapsed=0;
    static constexpr std::array<unsigned,8> targets{{3,3,1,2,2,1,1,1}};
    bool start(){
        if(state!=TutorialState::Waiting)return false;
        state=TutorialState::Running;return true;
    }
    static TutorialEvent expected(TutorialChallenge c){
        return static_cast<TutorialEvent>(static_cast<uint8_t>(c));
    }
    unsigned required() const {return targets[static_cast<unsigned>(challenge)];}
    bool onEvent(TutorialEvent event){
        if(state!=TutorialState::Running)return false;
        if(event!=expected(challenge)){++mistakes;return false;}
        progress=std::min(progress+1,required());
        if(progress==required())state=TutorialState::Completed;
        return true;
    }
    void tick(float dt){
        if(state==TutorialState::Running)
            elapsed+=std::clamp(dt,0.0f,.05f);
    }
    bool next(){
        if(state!=TutorialState::Completed ||
           challenge==TutorialChallenge::MiniBoss)return false;
        challenge=static_cast<TutorialChallenge>(static_cast<unsigned>(challenge)+1);
        state=TutorialState::Waiting;progress=0;mistakes=0;elapsed=0;
        return true;
    }
    void replay(TutorialChallenge c){
        if(static_cast<unsigned>(c)>=static_cast<unsigned>(TutorialChallenge::Count))return;
        challenge=c;state=TutorialState::Waiting;
        progress=mistakes=0;elapsed=0;
    }
    bool finished() const {
        return challenge==TutorialChallenge::MiniBoss && state==TutorialState::Completed;
    }
    // Tutorials never contribute to public Hall of Fame scores.
    int leaderboardPoints() const {return 0;}
};
} // namespace sfsolo
