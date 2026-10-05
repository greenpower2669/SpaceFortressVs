#include <cassert>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include "hall_sync.hpp"
#include "hall_sync_storage.hpp"
char *SDL_GetPrefPath(const char*,const char*);
void SDL_free(void*);
constexpr int SF_COOP_LOCAL=2,SF_COOP_AI=3;
#define SF_HALL_SYNC_NO_SDL 1
#include "hall_sync_runtime.hpp"

static std::string temp_dir()
{
    char pattern[]="/tmp/sf-hall-sync-XXXXXX";
    char *dir=::mkdtemp(pattern);
    assert(dir);
    return dir;
}

static SfHallRemoteEntry sample_remote()
{
    SfHallRemoteEntry e;
    e.id="srv-1";
    e.submissionId="sub-1";
    e.playerName="ÉQUIPE TEST";
    e.pilots={"ORION IA","FAB"};
    e.boss=200;
    e.difficulty="APOCALYPSE";
    e.durationMs=87000;
    e.points=1466;
    e.stars=9;
    e.completedAt="2026-10-05T16:21:59Z";
    e.mode="coop-ai";
    e.encounter=200;
    e.serverRank=3;
    return e;
}

static void sync_state_round_trips_v1()
{
    SfHallSyncState state;
    state.cursor="12345";
    state.lastSuccessfulSync=1791217319;
    state.locals.emplace(77,SfHallLocalRecord{77,"550e8400-e29b-41d4-a716-446655440000",SfHallUploadState::Acknowledged,"srv-1"});
    state.globals.emplace("srv-1",sample_remote());

    const std::string encoded=sfHallSyncEncode(state);
    std::istringstream input(encoded);
    SfHallSyncState decoded;
    assert(sfHallSyncDecode(input,decoded));
    assert(decoded.cursor==state.cursor);
    assert(decoded.lastSuccessfulSync==state.lastSuccessfulSync);
    assert(decoded.locals.size()==1);
    assert(decoded.locals.at(77).submissionId==state.locals.at(77).submissionId);
    assert(decoded.locals.at(77).state==SfHallUploadState::Acknowledged);
    assert(decoded.locals.at(77).serverId=="srv-1");
    assert(decoded.globals.size()==1);
    const auto &remote=decoded.globals.at("srv-1");
    assert(remote.playerName=="ÉQUIPE TEST");
    assert(remote.pilots[0]=="ORION IA" && remote.pilots[1]=="FAB");
    assert(remote.serverRank==3);
    assert(remote.stars==9 && remote.boss==200 && remote.encounter==200);
}

static void partial_or_invalid_state_never_replaces_valid_state()
{
    const std::string dir=temp_dir(), path=dir+"/hall-sync-v1.dat";
    SfHallSyncState original;
    original.cursor="88";
    original.locals.emplace(1,SfHallLocalRecord{1,"11111111-1111-4111-8111-111111111111",SfHallUploadState::Pending,""});
    std::string error;
    assert(sfHallSyncSaveFile(path,original,error));

    { std::ofstream broken(path+".tmp-garbage",std::ios::binary); broken<<"HALL_SYNC 1\nBROKEN"; }
    SfHallSyncState loaded;
    assert(sfHallSyncLoadFile(path,loaded,error));
    assert(loaded.cursor=="88");
    assert(loaded.locals.size()==1);

    SfHallSyncState invalid=original;
    invalid.cursor=std::string(5000,'x');
    assert(!sfHallSyncSaveFile(path,invalid,error));
    SfHallSyncState loadedAgain;
    assert(sfHallSyncLoadFile(path,loadedAgain,error));
    assert(loadedAgain.cursor=="88");
}

static void unknown_future_version_is_preserved_and_blocked()
{
    const std::string dir=temp_dir(), path=dir+"/hall-sync-v1.dat";
    { std::ofstream out(path,std::ios::binary); out<<"SPACEFORTRESS_HALL_SYNC 99\nfuture-data\n"; }
    SfHallSyncState state;
    std::string error;
    assert(!sfHallSyncLoadFile(path,state,error));
    assert(error.find("FORMAT INCONNU")!=std::string::npos);

    SfHallSyncState candidate;
    candidate.cursor="1";
    assert(!sfHallSyncSaveFile(path,candidate,error));
    std::ifstream in(path,std::ios::binary);
    std::string bytes((std::istreambuf_iterator<char>(in)),{});
    assert(bytes=="SPACEFORTRESS_HALL_SYNC 99\nfuture-data\n");
}


static SfCampaignSave sample_campaign_with_victory(uint64_t id,int danger=9)
{
    SfCampaignSave save;
    SfFameEntry e;
    e.id=id;e.boss=200;e.seconds=87;e.danger=danger;e.mode=SF_COOP_AI;e.date=1791217319;
    e.names={"ORION IA","FAB","TEST1"};
    save.fame.push_back(e);
    return save;
}

static void fresh_local_victory_becomes_pending_and_id_is_stable()
{
    auto save=sample_campaign_with_victory(42);
    SfHallSyncState state;
    int generated=0;
    auto factory=[&](uint64_t localId){++generated;return sfHallMakeSubmissionId(localId,0x1111222233334444ULL,0xaaaabbbbccccddddULL);};
    assert(sfHallReconcileLocal(save,state,factory));
    assert(state.locals.size()==1 && state.locals.at(42).state==SfHallUploadState::Pending);
    const auto first=state.locals.at(42).submissionId;
    assert(first.size()==36 && first[14]=='4' && (first[19]=='8'||first[19]=='9'||first[19]=='a'||first[19]=='b'));
    assert(!sfHallReconcileLocal(save,state,factory));
    assert(state.locals.size()==1 && state.locals.at(42).submissionId==first && generated==1);
}

static void danger_unknown_is_local_only_and_payload_maps_exactly()
{
    SfCampaignSave save=sample_campaign_with_victory(43,0);
    SfHallSyncState state;
    auto factory=[](uint64_t id){return sfHallMakeSubmissionId(id,1,2);};
    assert(!sfHallReconcileLocal(save,state,factory));
    assert(state.locals.empty());

    save=sample_campaign_with_victory(44,9);
    assert(sfHallReconcileLocal(save,state,factory));
    auto requests=sfHallPendingUploads(save,state,"1.4.0");
    assert(requests.size()==1);
    const auto &r=requests.front();
    assert(r.playerName=="TEST1");
    assert(r.pilots[0]=="ORION IA" && r.pilots[1]=="FAB");
    assert(r.boss==200 && r.encounter==200 && r.difficulty=="APOCALYPSE" && r.stars==9);
    assert(r.durationMs==87000 && r.points==sfFamePoints(200,9,87));
    assert(r.gameVersion=="1.4.0" && r.completedAtEpochSeconds==1791217319 && r.mode=="coop-ai");
}

static void two_victories_never_share_submission_id()
{
    auto save=sample_campaign_with_victory(50);
    auto other=sample_campaign_with_victory(51).fame.front();
    save.fame.push_back(other);
    SfHallSyncState state;
    auto factory=[](uint64_t id){return sfHallMakeSubmissionId(id,id*17,id*31);};
    assert(sfHallReconcileLocal(save,state,factory));
    assert(state.locals.size()==2);
    assert(state.locals.at(50).submissionId!=state.locals.at(51).submissionId);
}

int main()
{
    sync_state_round_trips_v1();
    partial_or_invalid_state_never_replaces_valid_state();
    unknown_future_version_is_preserved_and_blocked();
    fresh_local_victory_becomes_pending_and_id_is_stable();
    danger_unknown_is_local_only_and_payload_maps_exactly();
    two_victories_never_share_submission_id();
    return 0;
}
