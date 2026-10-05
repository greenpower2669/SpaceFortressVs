#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <iomanip>
#include <istream>
#include <map>
#include <ostream>
#include <sstream>
#include <string>

static constexpr int SF_HALL_SYNC_FORMAT_VERSION=1;

enum class SfHallUploadState { Pending=0, Acknowledged=1 };

struct SfHallLocalRecord {
    uint64_t localId=0;
    std::string submissionId;
    SfHallUploadState state=SfHallUploadState::Pending;
    std::string serverId;
};

struct SfHallRemoteEntry {
    std::string id;
    std::string submissionId;
    std::string playerName;
    std::array<std::string,2> pilots{};
    int boss=0;
    std::string difficulty;
    long long durationMs=0;
    int points=0;
    int stars=0;
    std::string completedAt;
    std::string mode;
    int encounter=0;
    int serverRank=0;
};

struct SfHallSyncState {
    std::string cursor="0";
    std::map<uint64_t,SfHallLocalRecord> locals;
    std::map<std::string,SfHallRemoteEntry> globals;
    long long lastSuccessfulSync=0;
};

static bool sfHallSafeToken(const std::string &value,size_t max=256)
{
    return value.size()<=max && std::none_of(value.begin(),value.end(),[](unsigned char c){return c==0;});
}

static bool sfHallRemoteValid(const SfHallRemoteEntry &e)
{
    return !e.id.empty() && sfHallSafeToken(e.id) && sfHallSafeToken(e.submissionId) &&
           sfHallSafeToken(e.playerName,192) && sfHallSafeToken(e.pilots[0],192) && sfHallSafeToken(e.pilots[1],192) &&
           e.boss>=1 && e.boss<=200 && sfHallSafeToken(e.difficulty,96) && e.durationMs>=0 &&
           e.points>=0 && e.stars>=1 && e.stars<=9 && sfHallSafeToken(e.completedAt,96) &&
           sfHallSafeToken(e.mode,64) && e.encounter>=1 && e.encounter<=200 && e.serverRank>=0;
}

static bool sfHallLocalValid(const SfHallLocalRecord &r)
{
    return r.localId>0 && !r.submissionId.empty() && sfHallSafeToken(r.submissionId,128) &&
           sfHallSafeToken(r.serverId,256) &&
           (r.state==SfHallUploadState::Pending || r.state==SfHallUploadState::Acknowledged);
}

static std::string sfHallSyncEncode(const SfHallSyncState &state)
{
    if (!sfHallSafeToken(state.cursor,1024) || state.locals.size()>100000 || state.globals.size()>100000 || state.lastSuccessfulSync<0)
        return {};
    for (const auto &[id,r] : state.locals) if (id!=r.localId || !sfHallLocalValid(r)) return {};
    for (const auto &[id,e] : state.globals) if (id!=e.id || !sfHallRemoteValid(e)) return {};

    std::ostringstream out;
    out<<"SPACEFORTRESS_HALL_SYNC "<<SF_HALL_SYNC_FORMAT_VERSION<<'\n';
    out<<std::quoted(state.cursor)<<' '<<state.lastSuccessfulSync<<' '<<state.locals.size()<<' '<<state.globals.size()<<'\n';
    for (const auto &[id,r] : state.locals) {
        out<<r.localId<<' '<<int(r.state)<<' '<<std::quoted(r.submissionId)<<' '<<std::quoted(r.serverId)<<'\n';
    }
    for (const auto &[id,e] : state.globals) {
        out<<std::quoted(e.id)<<' '<<std::quoted(e.submissionId)<<' '<<std::quoted(e.playerName)<<' '
           <<std::quoted(e.pilots[0])<<' '<<std::quoted(e.pilots[1])<<' '
           <<e.boss<<' '<<std::quoted(e.difficulty)<<' '<<e.durationMs<<' '<<e.points<<' '<<e.stars<<' '
           <<std::quoted(e.completedAt)<<' '<<std::quoted(e.mode)<<' '<<e.encounter<<' '<<e.serverRank<<'\n';
    }
    return out.str();
}

static bool sfHallSyncDecode(std::istream &in,SfHallSyncState &state)
{
    SfHallSyncState candidate;
    std::string magic;
    int version=0;
    size_t localCount=0,globalCount=0;
    if (!(in>>magic>>version) || magic!="SPACEFORTRESS_HALL_SYNC" || version!=SF_HALL_SYNC_FORMAT_VERSION) return false;
    if (!(in>>std::quoted(candidate.cursor)>>candidate.lastSuccessfulSync>>localCount>>globalCount)) return false;
    if (!sfHallSafeToken(candidate.cursor,1024) || candidate.lastSuccessfulSync<0 || localCount>100000 || globalCount>100000) return false;
    for (size_t i=0;i<localCount;++i) {
        SfHallLocalRecord r; int rawState=-1;
        if (!(in>>r.localId>>rawState>>std::quoted(r.submissionId)>>std::quoted(r.serverId))) return false;
        if (rawState==0) r.state=SfHallUploadState::Pending;
        else if (rawState==1) r.state=SfHallUploadState::Acknowledged;
        else return false;
        if (!sfHallLocalValid(r) || !candidate.locals.emplace(r.localId,r).second) return false;
    }
    for (size_t i=0;i<globalCount;++i) {
        SfHallRemoteEntry e;
        if (!(in>>std::quoted(e.id)>>std::quoted(e.submissionId)>>std::quoted(e.playerName)
              >>std::quoted(e.pilots[0])>>std::quoted(e.pilots[1])>>e.boss>>std::quoted(e.difficulty)
              >>e.durationMs>>e.points>>e.stars>>std::quoted(e.completedAt)>>std::quoted(e.mode)>>e.encounter>>e.serverRank)) return false;
        if (!sfHallRemoteValid(e) || !candidate.globals.emplace(e.id,e).second) return false;
    }
    in>>std::ws;
    if (!in.eof()) return false;
    state=std::move(candidate);
    return true;
}
