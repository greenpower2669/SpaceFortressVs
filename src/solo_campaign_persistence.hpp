#pragma once
// Explicit durable SOLO campaign lifecycle. Does not alter DUEL/COOP saves.
#include "solo_campaign_controller.hpp"
#include "solo_progression_storage.hpp"

namespace sfsolo {
inline bool restoreCampaign(CampaignController &campaign,const std::string &path){
    if(campaign.active)return false; // Never replace progression mid-flight.
    Progression restored;
    if(!loadProgression(path,restored))return false;
    campaign.progression=restored;
    if(!selectable(campaign.progression,campaign.selection))
        campaign.selection={};
    return true;
}
// Save before publishing a victory to the local Hall or discarding the run.
// On I/O failure, the session stays available and the progression rolls back.
inline bool finishAndSave(CampaignController &campaign,const std::string &path,
                          const std::string &player,const std::string &submissionId,
                          int points){
    if(!campaign.active || campaign.active->phase!=Phase::Won ||
       !campaign.active->bossDefeated || !campaign.active->reachedFinish ||
       campaign.active->pilot.health<=0)return false;
    const auto entry=makeSoloHallEntry(*campaign.active,player,submissionId,
                                      campaign.launchedSelection.difficulty,points);
    if(!entry.valid() || entry.world!=campaign.launchedSelection.world ||
       entry.stage!=campaign.launchedSelection.stage)return false;
    Progression updated=campaign.progression;
    if(!updated.complete(campaign.launchedSelection.world,campaign.launchedSelection.stage,true))
        return false;
    if(!saveProgression(path,updated))return false;
    campaign.progression=updated;
    campaign.localHall.record(entry);
    campaign.active.reset();
    return true;
}
// Same durable commit order for the new runtime, without instantiating Session.
inline bool finishAndSave(CampaignController &campaign,const RunResult &run,
                          const std::string &path,const std::string &player,
                          const std::string &submissionId,int points){
    if(campaign.active)return false;
    const auto entry=makeSoloHallEntry(run,player,submissionId,points);
    if(!entry.valid() || run.world!=campaign.launchedSelection.world ||
       run.stage!=campaign.launchedSelection.stage ||
       run.difficulty!=campaign.launchedSelection.difficulty)return false;
    Progression updated=campaign.progression;
    if(!updated.complete(run.world,run.stage,true) || !saveProgression(path,updated))return false;
    campaign.progression=updated;campaign.localHall.record(entry);return true;
}
} // namespace sfsolo
