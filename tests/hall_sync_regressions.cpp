#include <cassert>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include "hall_sync.hpp"
#include "hall_sync_storage.hpp"

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

int main()
{
    sync_state_round_trips_v1();
    partial_or_invalid_state_never_replaces_valid_state();
    unknown_future_version_is_preserved_and_blocked();
    return 0;
}
