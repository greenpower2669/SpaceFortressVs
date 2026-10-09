#include "../src/solo_tutorial_challenges.hpp"
#include <cassert>
int main(){
 using namespace sfsolo;
 TutorialProgress t;
 assert(!t.onEvent(TutorialEvent::PassedGate));
 for(unsigned i=0;i<8;i++){
     assert(static_cast<unsigned>(t.challenge)==i);
     assert(t.start());
     assert(!t.onEvent(static_cast<TutorialEvent>((i+1)%8)));
     assert(t.mistakes==1);
     t.tick(.5f);assert(t.elapsed==.05f);
     for(unsigned k=0;k<t.required();k++)assert(t.onEvent(TutorialProgress::expected(t.challenge)));
     assert(t.state==TutorialState::Completed);
     assert(t.leaderboardPoints()==0);
     if(i<7)assert(t.next());else assert(t.finished() && !t.next());
 }
 t.replay(TutorialChallenge::Slalom);
 assert(t.challenge==TutorialChallenge::Slalom && t.progress==0);
 assert(!t.finished() && t.leaderboardPoints()==0);
}
