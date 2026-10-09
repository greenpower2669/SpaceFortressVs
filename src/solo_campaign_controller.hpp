#pragma once
// SOLO-only navigation -> session -> validated victory -> progression.
// No legacy game mode or remote Hall of Fame state is modified.
#include "solo_stage_selection.hpp"
#include "solo_hall.hpp"
#include <memory>

namespace sfsolo {
struct CampaignController {
    Progression progression;
    StageSelection selection;
    std::unique_ptr<Session> active;
    SoloHallLocal localHall;
    bool launch(){
        Map map=mapForSelection(progression,selection);
        if(!map.valid())return false;
        auto candidate=std::make_unique<Session>(std::move(map));
        if(!candidate->start())return false;
        active=std::move(candidate);
        return true;
    }
    void abandon(){active.reset();}
    // Only completed, validated runs can unlock the next level or set records.
    bool finish(const std::string &player,const std::string &submissionId,
                int points){
        if(!active || active->phase!=Phase::Won || !active->bossDefeated ||
           !active->reachedFinish || active->pilot.health<=0)return false;
        const auto entry=makeSoloHallEntry(*active,player,submissionId,
                                          selection.difficulty,points);
        if(!entry.valid() || entry.world!=selection.world ||
           entry.stage!=selection.stage)return false;
        if(!progression.complete(selection.world,selection.stage,true))return false;
        localHall.record(entry); // A lower replay score does not overwrite a PB.
        active.reset();
        return true;
    }
    bool next(){return advanceStage(progression,selection);}
};
} // namespace sfsolo
