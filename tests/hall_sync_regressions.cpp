#include <cassert>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
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

static void reset_runtime_for(const std::string &path,const SfHallSyncState &state={})
{
    sfHallRuntimeState=state;
    sfHallRuntimePath=path;
    sfHallRuntimeLoaded=true;
    sfHallRuntimeError.clear();
    sfHallLastError=SfHallSyncError::None;
    sfHallResetPageStageForTests();
    sfHallResetOrchestrationForTests();
}

static void upload_success_and_failures_preserve_pending_correctly()
{
    const std::string dir=temp_dir(), path=dir+"/hall-sync-v1.dat";
    SfHallSyncState initial;
    initial.locals.emplace(42,SfHallLocalRecord{42,"sub-42",SfHallUploadState::Pending,""});
    std::string error; assert(sfHallSyncSaveFile(path,initial,error));
    reset_runtime_for(path,initial);
    sfHallApplyUploadFailure("sub-42",SfHallSyncError::Offline);
    assert(sfHallRuntimeState.locals.at(42).state==SfHallUploadState::Pending);
    sfHallApplyUploadFailure("sub-42",SfHallSyncError::Server);
    assert(sfHallRuntimeState.locals.at(42).state==SfHallUploadState::Pending);
    sfHallApplyUploadFailure("sub-42",SfHallSyncError::Auth);
    assert(sfHallRuntimeState.locals.at(42).state==SfHallUploadState::Pending);
    sfHallApplyUploadSuccess("sub-42","srv-42");
    assert(sfHallRuntimeState.locals.at(42).state==SfHallUploadState::Acknowledged);
    assert(sfHallRuntimeState.locals.at(42).serverId=="srv-42");
    sfHallApplyUploadSuccess("sub-42","srv-42");
    assert(sfHallRuntimeState.locals.at(42).serverId=="srv-42");
    SfHallSyncState reloaded; assert(sfHallSyncLoadFile(path,reloaded,error));
    assert(reloaded.locals.at(42).state==SfHallUploadState::Acknowledged);
}

static void paged_sync_commits_cursor_only_after_durable_merge()
{
    const std::string dir=temp_dir(), path=dir+"/hall-sync-v1.dat";
    SfHallSyncState initial; initial.cursor="0";
    std::string error; assert(sfHallSyncSaveFile(path,initial,error));
    reset_runtime_for(path,initial);
    auto one=sample_remote();
    assert(sfHallBeginPage(7,"0"));
    assert(sfHallStageRemote(7,one));
    assert(sfHallCommitPage(7,"0","100",true));
    assert(sfHallRuntimeState.cursor=="100" && sfHallRuntimeState.globals.size()==1);
    assert(sfHallLastCommittedHasMore && sfHallLastCommittedNextCursor=="100");

    auto two=one; two.id="srv-2"; two.submissionId="sub-2"; two.serverRank=4;
    assert(sfHallBeginPage(7,"100"));
    assert(sfHallStageRemote(7,two));
    sfHallFailPage(7,"100",SfHallSyncError::Timeout);
    assert(sfHallRuntimeState.cursor=="100" && sfHallRuntimeState.globals.count("srv-2")==0);

    const std::string blocked=dir+"/blocked.dat";
    { std::ofstream out(blocked); out<<"SPACEFORTRESS_HALL_SYNC 99\nfuture\n"; }
    sfHallRuntimePath=blocked;
    assert(sfHallBeginPage(8,"100"));
    assert(sfHallStageRemote(8,two));
    assert(!sfHallCommitPage(8,"100","200",false));
    assert(sfHallRuntimeState.cursor=="100" && sfHallRuntimeState.globals.count("srv-2")==0);
}

static void stale_callbacks_and_invalid_remote_entries_are_rejected()
{
    const std::string dir=temp_dir(), path=dir+"/hall-sync-v1.dat";
    SfHallSyncState initial; initial.cursor="10";
    std::string error; assert(sfHallSyncSaveFile(path,initial,error));
    reset_runtime_for(path,initial);
    assert(sfHallBeginPage(50,"10"));
    auto valid=sample_remote();
    assert(!sfHallStageRemote(49,valid));
    assert(!sfHallCommitPage(49,"10","20",false));
    auto invalid=valid; invalid.id="";
    assert(!sfHallStageRemote(50,invalid));
    invalid=valid; invalid.stars=10;
    assert(!sfHallStageRemote(50,invalid));
    invalid=valid; invalid.points=-1;
    assert(!sfHallStageRemote(50,invalid));
    sfHallFailPage(50,"10",SfHallSyncError::Protocol);
    assert(sfHallRuntimeState.cursor=="10" && sfHallRuntimeState.globals.empty());
}

static void offline_snapshot_contains_global_and_local_without_duplicates()
{
    SfCampaignSave save=sample_campaign_with_victory(70,9);
    auto pending=sample_campaign_with_victory(71,1).fame.front(); pending.names[2]="LOCAL"; pending.seconds=20;
    auto legacy=sample_campaign_with_victory(72,0).fame.front(); legacy.names[2]="OLD";
    save.fame.push_back(pending); save.fame.push_back(legacy);
    const int clearedBefore=save.cleared, selectedBefore=save.selected;

    SfHallSyncState state;
    state.locals.emplace(70,SfHallLocalRecord{70,"sub-70",SfHallUploadState::Acknowledged,"srv-70"});
    state.locals.emplace(71,SfHallLocalRecord{71,"sub-71",SfHallUploadState::Pending,""});
    auto remote=sample_remote(); remote.id="srv-70";remote.submissionId="sub-70";remote.playerName="TEST1";
    state.globals.emplace(remote.id,remote);
    auto other=sample_remote(); other.id="srv-other";other.submissionId="sub-other";other.points=9999;other.serverRank=1;
    state.globals.emplace(other.id,other);

    const auto snapshot=sfHallBuildSnapshot(save,state);
    assert(save.cleared==clearedBefore && save.selected==selectedBefore);
    int srv70=0,localPending=0,legacyCount=0,globalOther=0;
    for(const auto &d:snapshot){
        if(d.serverId=="srv-70") ++srv70;
        if(d.localId==71 && d.pending && d.serverRank==0) ++localPending;
        if(d.localId==72 && d.danger==0 && d.difficulty=="DANGER INCONNU") ++legacyCount;
        if(d.serverId=="srv-other") ++globalOther;
    }
    assert(srv70==1 && localPending==1 && legacyCount==1 && globalOther==1);
    assert(snapshot.front().points>=snapshot.back().points);
}

static void automatic_orchestration_deduplicates_uploads_and_paginates_once()
{
    const std::string dir=temp_dir(),path=dir+"/hall-sync-v1.dat";
    SfHallSyncState initial;initial.cursor="0";
    std::string error;assert(sfHallSyncSaveFile(path,initial,error));
    reset_runtime_for(path,initial);
    sfCampaignSave=sample_campaign_with_victory(90,9);

    std::vector<std::string> uploads;
    std::vector<std::tuple<uint64_t,std::string,int>> pages;
    SfHallTransport transport;
    transport.submit=[&](const SfHallUploadRequest &r){uploads.push_back(r.submissionId);};
    transport.syncPage=[&](uint64_t cycle,const std::string &cursor,int limit){pages.emplace_back(cycle,cursor,limit);};
    sfHallInstallTransport(std::move(transport));

    sfHallSyncOnHallOpen();
    sfHallSyncOnHallOpen();
    assert(uploads.size()==1);
    assert(pages.size()==1);
    assert(std::get<1>(pages[0])=="0" && std::get<2>(pages[0])==100);
    const uint64_t cycle=std::get<0>(pages[0]);

    auto remote=sample_remote();remote.id="srv-90";remote.submissionId=uploads[0];remote.serverRank=1;
    assert(sfHallTransportRemoteEntry(cycle,remote));
    sfHallTransportPageDone(cycle,"0","100",true);
    assert(pages.size()==2 && std::get<1>(pages[1])=="100" && std::get<0>(pages[1])==cycle);
    sfHallTransportPageDone(cycle,"100","200",false);
    assert(!sfHallSyncCycleActive);
    assert(sfHallRuntimeState.cursor=="200");
}

static void post_victory_upload_retries_only_after_callback_clears_inflight()
{
    const std::string dir=temp_dir(),path=dir+"/hall-sync-v1.dat";
    SfHallSyncState initial;
    std::string error;assert(sfHallSyncSaveFile(path,initial,error));
    reset_runtime_for(path,initial);
    sfCampaignSave=sample_campaign_with_victory(91,9);

    std::vector<std::string> uploads;
    int pages=0;
    SfHallTransport transport;
    transport.submit=[&](const SfHallUploadRequest &r){uploads.push_back(r.submissionId);};
    transport.syncPage=[&](uint64_t,const std::string&,int){++pages;};
    sfHallInstallTransport(std::move(transport));

    sfHallSyncAfterLocalVictorySaved();
    sfHallSyncAfterLocalVictorySaved();
    assert(uploads.size()==1 && pages==0);
    const std::string submission=uploads.front();
    sfHallTransportUploadFailed(submission,SfHallSyncError::Offline);
    assert(sfHallRuntimeState.locals.at(91).state==SfHallUploadState::Pending);
    sfHallSyncAfterLocalVictorySaved();
    assert(uploads.size()==2 && uploads[1]==submission);
}

static void page_failure_stops_chain_without_advancing_cursor()
{
    const std::string dir=temp_dir(),path=dir+"/hall-sync-v1.dat";
    SfHallSyncState initial;initial.cursor="33";
    std::string error;assert(sfHallSyncSaveFile(path,initial,error));
    reset_runtime_for(path,initial);
    sfCampaignSave={};

    std::vector<std::tuple<uint64_t,std::string,int>> pages;
    SfHallTransport transport;
    transport.syncPage=[&](uint64_t cycle,const std::string &cursor,int limit){pages.emplace_back(cycle,cursor,limit);};
    sfHallInstallTransport(std::move(transport));
    sfHallSyncOnHallOpen();
    assert(pages.size()==1 && sfHallSyncCycleActive);
    const auto cycle=std::get<0>(pages.front());
    sfHallTransportPageFailed(cycle,"33",SfHallSyncError::Timeout);
    assert(!sfHallSyncCycleActive);
    assert(sfHallRuntimeState.cursor=="33");
    assert(sfHallLastError==SfHallSyncError::Timeout);
}

int main()
{
    sync_state_round_trips_v1();
    partial_or_invalid_state_never_replaces_valid_state();
    unknown_future_version_is_preserved_and_blocked();
    fresh_local_victory_becomes_pending_and_id_is_stable();
    danger_unknown_is_local_only_and_payload_maps_exactly();
    two_victories_never_share_submission_id();
    upload_success_and_failures_preserve_pending_correctly();
    paged_sync_commits_cursor_only_after_durable_merge();
    stale_callbacks_and_invalid_remote_entries_are_rejected();
    offline_snapshot_contains_global_and_local_without_duplicates();
    automatic_orchestration_deduplicates_uploads_and_paginates_once();
    post_victory_upload_retries_only_after_callback_clears_inflight();
    page_failure_stops_chain_without_advancing_cursor();
    return 0;
}
