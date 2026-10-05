#pragma once

#include "hall_sync_storage.hpp"
#include "campaign_save.hpp"
#include "hall_of_fame.hpp"
#include "boss_danger.hpp"
#include <array>
#include <chrono>
#include <cstdint>
#include <functional>
#include <random>
#include <string>
#include <vector>
#ifndef SF_HALL_SYNC_NO_SDL
#include <SDL2/SDL.h>
#endif

struct SfHallUploadRequest {
    std::string submissionId;
    std::string playerName;
    std::array<std::string,2> pilots{};
    int boss=0;
    std::string difficulty;
    long long durationMs=0;
    int points=0;
    int stars=0;
    std::string gameVersion;
    long long completedAtEpochSeconds=0;
    std::string mode;
    int encounter=0;
};

using SfHallIdFactory=std::function<std::string(uint64_t)>;

static uint64_t sfHallMix64(uint64_t x)
{
    x+=0x9e3779b97f4a7c15ULL;
    x=(x^(x>>30))*0xbf58476d1ce4e5b9ULL;
    x=(x^(x>>27))*0x94d049bb133111ebULL;
    return x^(x>>31);
}

static std::string sfHallMakeSubmissionId(uint64_t localId,uint64_t entropyA,uint64_t entropyB)
{
    uint64_t a=sfHallMix64(localId^entropyA),b=sfHallMix64((localId<<1)^entropyB^a);
    unsigned char bytes[16];
    for(int i=0;i<8;++i) bytes[i]=static_cast<unsigned char>((a>>(56-i*8))&0xff);
    for(int i=0;i<8;++i) bytes[8+i]=static_cast<unsigned char>((b>>(56-i*8))&0xff);
    bytes[6]=static_cast<unsigned char>((bytes[6]&0x0f)|0x40);
    bytes[8]=static_cast<unsigned char>((bytes[8]&0x3f)|0x80);
    static const char hex[]="0123456789abcdef";
    std::string out;out.reserve(36);
    for(int i=0;i<16;++i){
        if(i==4||i==6||i==8||i==10) out.push_back('-');
        out.push_back(hex[bytes[i]>>4]);out.push_back(hex[bytes[i]&15]);
    }
    return out;
}

static bool sfHallReconcileLocal(const SfCampaignSave &save,SfHallSyncState &state,const SfHallIdFactory &factory)
{
    bool changed=false;
    for(const auto &entry:save.fame){
        if(entry.id==0 || entry.danger<1 || entry.danger>9) continue;
        if(state.locals.find(entry.id)!=state.locals.end()) continue;
        std::string id=factory(entry.id);
        if(id.empty()) continue;
        bool collision=false;
        for(const auto &[existingId,record]:state.locals) if(record.submissionId==id){collision=true;break;}
        if(collision) id=sfHallMakeSubmissionId(entry.id,entry.id^0x51f15eULL,entry.id^0xa11ceULL);
        state.locals.emplace(entry.id,SfHallLocalRecord{entry.id,id,SfHallUploadState::Pending,{}});
        changed=true;
    }
    return changed;
}

static std::vector<SfHallUploadRequest> sfHallPendingUploads(const SfCampaignSave &save,const SfHallSyncState &state,const std::string &gameVersion)
{
    std::vector<SfHallUploadRequest> out;
    for(const auto &entry:save.fame){
        const auto found=state.locals.find(entry.id);
        if(found==state.locals.end() || found->second.state!=SfHallUploadState::Pending || entry.danger<1 || entry.danger>9) continue;
        SfHallUploadRequest r;
        r.submissionId=found->second.submissionId;r.playerName=entry.names[2];
        r.pilots={entry.names[0],entry.names[1]};r.boss=entry.boss;r.encounter=entry.boss;
        r.difficulty=SF_BOSS_DANGER_NAMES[entry.danger-1];r.durationMs=static_cast<long long>(entry.seconds)*1000LL;
        r.points=sfFamePoints(entry.boss,entry.danger,entry.seconds);r.stars=entry.danger;r.gameVersion=gameVersion;
        r.completedAtEpochSeconds=entry.date;r.mode=entry.mode==SF_COOP_AI ? "coop-ai" : "coop-local";
        out.push_back(std::move(r));
    }
    return out;
}

inline SfHallSyncState sfHallRuntimeState;
inline bool sfHallRuntimeLoaded=false;
inline std::string sfHallRuntimePath;
inline std::string sfHallRuntimeError;

static std::string sfHallDefaultSubmissionId(uint64_t localId)
{
    const auto now=static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::random_device rd;
    const uint64_t extra=(static_cast<uint64_t>(rd())<<32)^static_cast<uint64_t>(rd());
    return sfHallMakeSubmissionId(localId,now,extra);
}

static bool sfHallRuntimeResolvePath()
{
    if(!sfHallRuntimePath.empty()) return true;
#ifdef SF_HALL_SYNC_NO_SDL
    return false;
#else
    char *base=SDL_GetPrefPath("greenpower2669","SpaceFortressVs");
    if(!base){sfHallRuntimeError="SYNC STOCKAGE INDISPONIBLE";return false;}
    sfHallRuntimePath=std::string(base)+"hall-sync-v1.dat";SDL_free(base);return true;
#endif
}

static bool sfHallRuntimeLoad()
{
    if(sfHallRuntimeLoaded) return true;
    if(!sfHallRuntimeResolvePath()) return false;
    sfHallRuntimeLoaded=true;
    return sfHallSyncLoadFile(sfHallRuntimePath,sfHallRuntimeState,sfHallRuntimeError);
}

static bool sfHallRuntimeReconcileAndSave()
{
    if(!sfHallRuntimeLoad()) return false;
    auto next=sfHallRuntimeState;
    if(!sfHallReconcileLocal(sfCampaignSave,next,sfHallDefaultSubmissionId)) return true;
    if(!sfHallSyncSaveFile(sfHallRuntimePath,next,sfHallRuntimeError)) return false;
    sfHallRuntimeState=std::move(next);return true;
}

static void sfHallSyncAfterLocalVictorySaved()
{
    sfHallRuntimeReconcileAndSave();
}
