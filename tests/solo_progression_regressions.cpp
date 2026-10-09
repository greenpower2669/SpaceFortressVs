#include "../src/solo_progression.hpp"
#include <cassert>
int main(){
 using namespace sfsolo;
 Progression p;
 assert(p.unlocked(1,1)&&!p.unlocked(1,2)&&!p.unlocked(2,1));
 assert(!p.complete(1,2,true)&&!p.complete(1,1,false));
 for(unsigned i=1;i<=20;i++){
     assert(p.unlocked(1,i));
     assert(p.complete(1,i,true));
 }
 assert(p.unlocked(2,1)&&p.unlocked(1,1));
 assert(p.complete(1,1,true)); // replay
 auto saved=p.serialize();
 Progression restored;assert(restored.deserialize(saved));
 assert(restored.unlocked(2,1)&&restored.isCompleted(1,20));
 assert(!restored.deserialize("SFSOLO1:FFFFFFFF"));
 assert(restored.unlocked(2,1)); // invalid data must not overwrite state
 assert(!restored.deserialize("SFSOLO1:00000005:00000000:00000000:00000000:00000000:00000000"));
 assert(restored.unlocked(2,1)); // cannot inject skipped stages
 for(unsigned w=2;w<=6;w++)
     for(unsigned i=1;i<=20;i++)assert(restored.complete(w,i,true));
 assert(restored.highestUnlockedWorld()==6);
 assert(!restored.unlocked(7,1));
}
