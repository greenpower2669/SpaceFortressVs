#include "../src/solo_stage_selection.hpp"
#include <cassert>
int main(){
 using namespace sfsolo;
 Progression p;
 StageSelection current;
 assert(selectable(p,current));
 assert(!selectStage(p,current,2,1,1));
 assert(!selectStage(p,current,1,2,1));
 assert(!selectStage(p,current,1,1,0));
 assert(!selectStage(p,current,1,1,10));
 assert(current.world==1&&current.stage==1&&current.difficulty==1);
 assert(mapForSelection(p,current).valid());
 assert(!advanceStage(p,current));
 for(unsigned stage=1;stage<=20;stage++){
   assert(p.complete(1,stage,true));
   if(stage<20)assert(selectStage(p,current,1,stage+1,9));
 }
 assert(selectStage(p,current,1,20,9));
 assert(advanceStage(p,current));
 assert(current.world==2&&current.stage==1&&current.difficulty==9);
 assert(mapForSelection(p,current).valid());
 assert(!selectStage(p,current,3,1,9));
}
