#include "../src/solo_campaign_persistence.hpp"
#include <cassert>
#include <cstdio>
int main(){
 using namespace sfsolo;
 const std::string path="solo-durable-test.save";
 std::remove(path.c_str());
 CampaignController campaign;
 assert(campaign.launch());
 assert(!restoreCampaign(campaign,path));
 assert(!finishAndSave(campaign,path,"Fab","early",900));
 assert(!campaign.progression.isCompleted(1,1));
 campaign.active->phase=Phase::BossFight;
 campaign.active->defeatBoss();
 campaign.active->phase=Phase::Won;
 campaign.active->reachedFinish=true;
 campaign.active->seconds=15;
 assert(!finishAndSave(campaign,"","Fab","io-failure",900));
 assert(campaign.active);
 assert(!campaign.progression.isCompleted(1,1));
 assert(campaign.localHall.best.empty());
 campaign.selection.difficulty=9; // Changing the menu cannot reclassify the run.
 assert(finishAndSave(campaign,path,"Fab","success",900));
 assert(!campaign.active);
 assert(campaign.progression.isCompleted(1,1));
 assert(campaign.localHall.best.size()==1);
 assert(campaign.localHall.best.begin()->second.difficulty==1);
 CampaignController second;
 assert(restoreCampaign(second,path));
 assert(second.progression.isCompleted(1,1));
 assert(second.next());
 assert(second.selection.stage==2);
 assert(second.launch());
 assert(!restoreCampaign(second,path));
 second.abandon();
 std::remove(path.c_str());
 std::remove((path+".tmp").c_str());
}
