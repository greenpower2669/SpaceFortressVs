#include "../src/solo_audio.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <cassert>
int main(){
    // Headless CI can open a dummy speaker, but must load the SAME actual
    // WAV/OGG bytes that Android packages from assets/sounds.
    SDL_setenv("SDL_AUDIODRIVER","dummy",1);
    assert(SDL_Init(SDL_INIT_AUDIO)==0);
    {
        sfsolo::SoloSounds audio;
        assert(audio.opened);
        assert(audio.cues[size_t(sfsolo::SoundCue::Shot)]!=nullptr);
        assert(audio.cues[size_t(sfsolo::SoundCue::Impact)]!=nullptr);
        assert(audio.cues[size_t(sfsolo::SoundCue::Charge)]!=nullptr);
        assert(audio.cues[size_t(sfsolo::SoundCue::Ready)]!=nullptr);
        assert(audio.cues[size_t(sfsolo::SoundCue::Plasma)]!=nullptr);
        audio.play(sfsolo::SoundCue::Shot);
        audio.play(sfsolo::SoundCue::Impact);
        audio.play(sfsolo::SoundCue::Charge);
        audio.play(sfsolo::SoundCue::Ready);
        audio.play(sfsolo::SoundCue::Plasma);
    }
    SDL_Quit();
}
