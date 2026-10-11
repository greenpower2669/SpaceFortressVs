#include "../src/coop_solo_charge_audio.hpp"
#include <cassert>
int main(){
    SfCoopSoloChargeCueGate sound;
    SfKineticSurgeState surge{};
    assert(!sound.startWhenVisible(surge)); // absent press
    surge.held=true;
    surge.heldSeconds=0.0f;
    assert(!sound.startWhenVisible(surge)); // first tap = ordinary shot, SILENT
    surge.heldSeconds=.1f;
    assert(!sound.startWhenVisible(surge));
    surge.heldSeconds=.199f;
    assert(!sound.startWhenVisible(surge));
    surge.heldSeconds=SF_KINETIC_SURGE_VISIBLE_DELAY_SECONDS;
    assert(sound.startWhenVisible(surge));
    assert(!sound.startWhenVisible(surge)); // exactly once per hold
    surge.heldSeconds=2.0f;surge.charged=true;
    assert(!sound.startWhenVisible(surge)); // READY is separate, COOP owns it
    surge.held=false;
    assert(!sound.startWhenVisible(surge)); // release resets
    surge.held=true;surge.heldSeconds=.19f;surge.charged=false;
    assert(!sound.startWhenVisible(surge));
    surge.heldSeconds=.21f;assert(sound.startWhenVisible(surge));
    sound.reset(); // Android pause or lost fingers
    surge.heldSeconds=0;assert(!sound.startWhenVisible(surge));
    return 0;
}
