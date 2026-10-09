#include "../src/solo_hall.hpp"
#include <cassert>
#include <limits>
int main(){
 using namespace sfsolo;
 Session s(generate({}));
 assert(s.start());
 auto rejected=makeSoloHallEntry(s,"Fab","solo-1",9,1000);
 assert(!rejected.valid());
 s.phase=Phase::BossFight;s.defeatBoss();
 s.phase=Phase::Won;s.reachedFinish=true;s.seconds=42.0f;
 s.enemiesDefeated=2;
 auto e=makeSoloHallEntry(s,"Fab","solo-2",9,1200);
 assert(e.valid());
 assert(e.world==1&&e.stage==1&&e.difficulty==9);
 SoloHallLocal hall;
 assert(hall.record(e));
 assert(!hall.record(e));
 auto lower=e;lower.submissionId="solo-3";lower.points=900;
 assert(!hall.record(lower));
 auto faster=e;faster.submissionId="solo-4";faster.durationMs=40000;
 assert(hall.record(faster));
 auto different=e;different.difficulty=8;different.submissionId="solo-5";
 assert(hall.record(different));
 assert(hall.best.size()==2);
 auto invalid=e;invalid.bossDefeated=false;
 assert(!hall.record(invalid));
 s.pilot.damageTaken=std::numeric_limits<float>::quiet_NaN();
 assert(!makeSoloHallEntry(s,"Fab","solo-6",9,1200).valid());
}
