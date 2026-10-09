#pragma once
// SOLO-specific score records: never reinterpret legacy DUEL/COOP hall entries.
// Remote payload transport is intentionally separate until server schema audit.
#include "solo_session.hpp"
#include <algorithm>
#include <cstdint>
#include <map>
#include <string>
#include <cmath>
#include <tuple>

namespace sfsolo {
struct SoloHallEntry {
    std::string submissionId,playerName,mapId;
    unsigned mapVersion=0,world=0,stage=0,difficulty=0;
    unsigned enemiesDefeated=0,enemiesAvailable=0;
    uint64_t durationMs=0;
    int points=0;
    float damageTaken=0;
    bool bossDefeated=false,reachedFinish=false,alive=false;
    bool valid() const {
        return std::isfinite(damageTaken) && !std::isnan(damageTaken) &&
            !submissionId.empty()&&submissionId.size()<=128 &&
            !playerName.empty()&&playerName.size()<=192 &&
            !mapId.empty()&&mapId.size()<=256 &&
            mapVersion>0&&world>=1&&stage>=1&&stage<=20&&
            difficulty>=1&&difficulty<=9&&enemiesDefeated<=enemiesAvailable &&
            durationMs>0&&durationMs<=86400000ULL&&
            points>=0&&damageTaken>=0&&damageTaken<=1000000 &&
            bossDefeated&&reachedFinish&&alive;
    }
};
inline SoloHallEntry makeSoloHallEntry(const Session &session,
    const std::string &player,const std::string &submissionId,
    unsigned difficulty,int points){
    SoloHallEntry e;
    if(session.phase!=Phase::Won || !session.bossDefeated ||
       !session.reachedFinish || session.pilot.health<=0 ||
       !std::isfinite(session.seconds) || !std::isfinite(session.pilot.damageTaken) ||
       points<0 || session.enemiesDefeated>session.enemiesAvailable)return e;
    e.submissionId=submissionId;e.playerName=player;e.mapId=session.map.id;
    e.mapVersion=session.map.version;e.world=session.map.world;
    e.stage=session.map.stage;e.difficulty=difficulty;
    e.enemiesDefeated=session.enemiesDefeated;
    e.enemiesAvailable=session.enemiesAvailable;
    e.durationMs=static_cast<uint64_t>(std::max(0.0f,session.seconds)*1000.0f);
    e.points=points;e.damageTaken=session.pilot.damageTaken;
    e.bossDefeated=session.bossDefeated;
    e.reachedFinish=session.reachedFinish;e.alive=session.pilot.health>0;
    return e;
}
struct SoloHallLocal {
    // One best result per player / map / difficulty; equal scores favour faster times.
    std::map<std::tuple<std::string,std::string,unsigned>,SoloHallEntry> best;
    bool record(const SoloHallEntry &entry){
        if(!entry.valid())return false;
        const auto key=std::make_tuple(entry.playerName,entry.mapId,entry.difficulty);
        const auto it=best.find(key);
        if(it!=best.end() && (it->second.points>entry.points ||
           (it->second.points==entry.points && it->second.durationMs<=entry.durationMs)))
            return false;
        best[key]=entry;return true;
    }
};
} // namespace sfsolo
