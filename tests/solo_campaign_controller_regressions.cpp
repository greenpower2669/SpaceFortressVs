#include "../src/solo_campaign_controller.hpp"
#include <cassert>
int main(){
 using namespace sfsolo;
 CampaignController c;
 assert(!c.next());
 assert(c.launch());
 assert(!c.finish("Fab","early",500));
 assert(!c.progression.isCompleted(1,1));
 assert(c.localHall.best.empty());
 assert(c.active);
 c.active->phase=Phase::BossFight;
 c.active->defeatBoss();
 c.active->reachedFinish=true;
 c.active->phase=Phase::Won;
 c.active->seconds=42;
 assert(c.finish("Fab","victory-1",1200));
 assert(c.progression.isCompleted(1,1));
 assert(c.localHall.best.size()==1);
 assert(!c.active);
 assert(c.next());
 assert(c.selection.stage==2);
 assert(c.launch());
 c.abandon();
 assert(!c.active);
 assert(!c.progression.isCompleted(1,2));
 assert(!c.finish("Fab","abandoned",9999));
}
