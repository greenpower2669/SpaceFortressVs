#pragma once
// SOLO audio uses the SAME shipped sound files as classic SpaceFortress,
// without touching the historical game loop or changing its mixer ownership.
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <array>
#include <string>
namespace sfsolo {
enum class SoundCue { Shot=0, Impact=1, Charge=2, Ready=3, Plasma=4 };
struct SoloSounds {
    bool audioInitialized=false,opened=false;
    std::array<Mix_Chunk*,5> cues{};
    Mix_Music *music=nullptr;
    static Mix_Chunk* load(const char *filename){
        const std::string path=std::string("resources/assets/sounds/")+filename;
        Mix_Chunk *chunk=Mix_LoadWAV(path.c_str());
        if(!chunk)SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO,
            "SOLO audio missing %s: %s",filename,Mix_GetError());
        return chunk;
    }
    SoloSounds(){
        if(!(SDL_WasInit(SDL_INIT_AUDIO)&SDL_INIT_AUDIO)){
            if(SDL_InitSubSystem(SDL_INIT_AUDIO)!=0){
                SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO,
                    "SOLO optional audio unavailable: %s",SDL_GetError());
                return;
            }
            audioInitialized=true;
        }
        Mix_Init(MIX_INIT_OGG);
        if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048)!=0){
            SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO,
                "SOLO mixer unavailable: %s",Mix_GetError());
            return;
        }
        opened=true;
        Mix_AllocateChannels(12);
        Mix_Volume(-1,MIX_MAX_VOLUME/4);
        cues[size_t(SoundCue::Shot)]=load("laser2.ogg");
        cues[size_t(SoundCue::Impact)]=load("explo1.ogg");
        cues[size_t(SoundCue::Charge)]=load("kinetic_charge.wav");
        cues[size_t(SoundCue::Ready)]=load("kinetic_ready.wav");
        cues[size_t(SoundCue::Plasma)]=load("kinetic_release.wav");
        // Same original in-game music, quiet enough to hear weapon feedback.
        SDL_RWops *rw=SDL_RWFromFile(
            "resources/assets/sounds/music.ogg","rb");
        if(rw)music=Mix_LoadMUS_RW(rw,1);
        if(music){
            Mix_VolumeMusic(MIX_MAX_VOLUME/6);
            Mix_PlayMusic(music,-1);
        }
        SDL_Log("SOLO audio cues: shot=%d impact=%d charge=%d ready=%d plasma=%d music=%d",
            cues[0]!=nullptr,cues[1]!=nullptr,cues[2]!=nullptr,
            cues[3]!=nullptr,cues[4]!=nullptr,music!=nullptr);
    }
    void play(SoundCue cue){
        Mix_Chunk *chunk=cues[size_t(cue)];
        if(opened&&chunk)Mix_PlayChannel(-1,chunk,0);
    }
    ~SoloSounds(){
        if(opened)Mix_HaltChannel(-1);
        if(music){Mix_HaltMusic();Mix_FreeMusic(music);}
        for(auto *chunk:cues)if(chunk)Mix_FreeChunk(chunk);
        if(opened)Mix_CloseAudio();
        Mix_Quit();
        if(audioInitialized)SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
    SoloSounds(const SoloSounds&)=delete;
    SoloSounds&operator=(const SoloSounds&)=delete;
};
} // namespace sfsolo
