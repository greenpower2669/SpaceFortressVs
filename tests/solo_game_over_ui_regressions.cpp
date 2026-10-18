#include "solo_game_over_ui.hpp"
#include <cassert>
#include <cstdio>
int main(){
    const int w=540,h=960;
    const auto panel=sfsolo::GameOverUi::panel(w,h);
    const auto button=sfsolo::GameOverUi::menuButton(w,h);
    assert(button.x>panel.x && button.y>panel.y);
    assert(button.x+button.w<panel.x+panel.w);
    assert(button.y+button.h<panel.y+panel.h);
    assert(sfsolo::GameOverUi::hitMenu(button.x+button.w*.5f,
                                           button.y+button.h*.5f,w,h));
    assert(!sfsolo::GameOverUi::hitMenu(button.x-1.f,button.y,w,h));
    assert(!sfsolo::GameOverUi::hitMenu(button.x+button.w+1.f,button.y,w,h));
    assert(!sfsolo::GameOverUi::hitMenu(0,0,w,h));
    assert(!sfsolo::GameOverUi::hitMenu(0,0,0,0));
    puts("PASS SOLO-028 Game Over return-to-historical-menu hitbox");
}
