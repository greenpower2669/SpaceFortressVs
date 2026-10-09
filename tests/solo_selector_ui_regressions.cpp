#include "../src/solo_selector_ui.hpp"
#include <cassert>
int main(){
 using namespace sfsolo;
 Progression progress;
 Selector ui;
 assert(ui.touch(.5f,.9f,progress)==SelectAction::Play);
 assert(ui.touch(.9f,.26f,progress)==SelectAction::None);
 assert(ui.selection.world==1);
 assert(ui.touch(.9f,.46f,progress)==SelectAction::None);
 assert(ui.selection.stage==1);
 assert(ui.touch(.9f,.66f,progress)==SelectAction::None);
 assert(ui.selection.difficulty==2);
 assert(ui.touch(.2f,.07f,progress)==SelectAction::Back);
 assert(ui.touch(-.2f,.9f,progress)==SelectAction::None);
 assert(ui.touch(.2f,1.1f,progress)==SelectAction::None);
 assert(progress.complete(1,1,true));
 ui.touch(.9f,.46f,progress);
 assert(ui.selection.stage==2);
 assert(ui.touch(.5f,.9f,progress)==SelectAction::Play);
}
