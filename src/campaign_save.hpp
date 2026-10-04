#pragma once
#include <array>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <ctime>
#include <unistd.h>
#include <sys/stat.h>
#include <cerrno>
#include <fcntl.h>

struct SfFameEntry {
    uint64_t id=0;
    int boss=0,score=0,seconds=0,mode=SF_COOP_LOCAL;
    long long date=0;
    std::array<std::string,3> names{}; // orange, blue, team; retain UTF-8
};
struct SfCampaignSave {
    int cleared=0,selected=0;
    std::array<std::string,3> names{};
    std::vector<SfFameEntry> fame;
    bool pending=false;
    SfFameEntry victory;
};
inline SfCampaignSave sfCampaignSave;
inline bool sfCampaignLoaded=false,sfCampaignStorageBlocked=false;
inline std::string sfCampaignStoragePath,sfCampaignStorageError;

static std::string sfCleanName(const std::string &input)
{
    std::string out; int characters=0;
    for (unsigned char c : input) {
        if (c<32 || c==127) continue;
        if ((c&0xc0)!=0x80 && ++characters>24) break;
        if (out.size()>=96) break;
        out.push_back(char(c));
    }
    const auto first=out.find_first_not_of(' '),last=out.find_last_not_of(' ');
    return first==std::string::npos ? "" : out.substr(first,last-first+1);
}

static void sfWriteFame(std::ostream &out,const SfFameEntry &entry)
{
    out<<entry.id<<' '<<entry.boss<<' '<<entry.score<<' '<<entry.seconds<<' '
       <<entry.mode<<' '<<entry.date;
    for (const auto &name : entry.names) out<<' '<<std::quoted(name);
    out<<'\n';
}
static bool sfReadFame(std::istream &in,SfFameEntry &entry,int limit=200)
{
    if (!(in>>entry.id>>entry.boss>>entry.score>>entry.seconds>>entry.mode>>entry.date)) return false;
    for (auto &name : entry.names) if (!(in>>std::quoted(name)) || name.size()>96) return false;
    return entry.id>0 && entry.boss>=1 && entry.boss<=limit && entry.score>=0 && entry.seconds>=0 &&
           (entry.mode==SF_COOP_LOCAL || entry.mode==SF_COOP_AI);
}
static std::string sfEncodeCampaign(const SfCampaignSave &save)
{
    std::ostringstream out;
    out<<"SPACEFORTRESS_CAMPAIGN 2\n"<<save.cleared<<' '<<save.selected<<' '<<save.pending<<' '<<save.fame.size()<<'\n';
    for (const auto &name : save.names) out<<std::quoted(name)<<'\n';
    if (save.pending) sfWriteFame(out,save.victory);
    for (const auto &entry : save.fame) sfWriteFame(out,entry);
    return out.str();
}
static bool sfDecodeCampaign(std::istream &in,SfCampaignSave &save)
{
    SfCampaignSave candidate;
    std::string magic; int version,pending; size_t count;
    if (!(in>>magic>>version) || magic!="SPACEFORTRESS_CAMPAIGN" || (version!=1 && version!=2)) return false;
    const int limit=version==1 ? 50 : 200;
    if (!(in>>candidate.cleared>>candidate.selected>>pending>>count) || candidate.cleared<0 ||
        candidate.cleared>limit || candidate.selected<0 || candidate.selected>=limit ||
        pending<0 || pending>1 || count>100000) return false;
    candidate.pending=pending;
    for (auto &name : candidate.names) if (!(in>>std::quoted(name)) || name.size()>96) return false;
    if (candidate.pending && !sfReadFame(in,candidate.victory,limit)) return false;
    for (size_t i=0;i<count;++i) {
        SfFameEntry entry; if (!sfReadFame(in,entry,limit)) return false;
        candidate.fame.push_back(entry);
    }
    in>>std::ws;
    if (!in.eof()) return false;
    save=std::move(candidate); return true;
}

static bool sfReadCampaignFile(const std::string &path,SfCampaignSave &save)
{
    std::ifstream in(path,std::ios::binary);
    return in && sfDecodeCampaign(in,save);
}
enum class SfSaveFileState { Missing, Valid, Invalid, Unknown, Inaccessible };
static SfSaveFileState sfInspectCampaign(const std::string &path,SfCampaignSave &save,std::string &bytes)
{
    struct stat info{};
    if (::stat(path.c_str(),&info)!=0)
        return errno==ENOENT ? SfSaveFileState::Missing : SfSaveFileState::Inaccessible;
    if (!S_ISREG(info.st_mode)) return SfSaveFileState::Inaccessible;
    std::ifstream file(path,std::ios::binary);
    if (!file) return SfSaveFileState::Inaccessible;
    bytes.assign(std::istreambuf_iterator<char>(file),{});
    if (file.bad()) return SfSaveFileState::Inaccessible;
    std::istringstream input(bytes);
    if (sfDecodeCampaign(input,save)) return SfSaveFileState::Valid;
    std::istringstream header(bytes);std::string magic;int version=0;
    if (header>>magic>>version && magic=="SPACEFORTRESS_CAMPAIGN" && version!=1 && version!=2)
        return SfSaveFileState::Unknown;
    return SfSaveFileState::Invalid;
}

static bool sfSyncCampaignDirectory(const std::string &path)
{
    const auto slash=path.find_last_of('/');
    const auto directory=slash==std::string::npos ? "." : slash==0 ? "/" : path.substr(0,slash);
    const int fd=::open(directory.c_str(),O_RDONLY|O_DIRECTORY);
    if (fd<0) return false;
    const bool ok=::fsync(fd)==0;::close(fd);return ok;
}

static bool sfCampaignTemporary(const std::string &prefix,const std::string &bytes,std::string &path)
{
    path=prefix+"XXXXXX";
    const int fd=::mkstemp(path.data());
    if (fd<0) return false;
    FILE *file=::fdopen(fd,"wb");
    if (!file) {::close(fd);std::remove(path.c_str());return false;}
    bool ok=std::fwrite(bytes.data(),1,bytes.size(),file)==bytes.size();
    ok=std::fflush(file)==0 && ok;
    ok=::fsync(::fileno(file))==0 && ok;
    ok=std::fclose(file)==0 && ok;
    if (!ok) std::remove(path.c_str());
    return ok;
}

static bool sfAtomicCampaignWrite(const std::string &path,const std::string &bytes)
{
    std::string temporary;
    if (!sfCampaignTemporary(path+".tmp-",bytes,temporary)) return false;
    if (std::rename(temporary.c_str(),path.c_str())!=0) {
        std::remove(temporary.c_str());return false;
    }
    return sfSyncCampaignDirectory(path);
}

static bool sfPreserveCampaignBytes(const std::string &path,const std::string &bytes)
{
    // A unique durable copy keeps the original bytes even when repair succeeds.
    // Never truncate an earlier recovery, and never archive an unknown format
    // as a pretext to overwrite it: those are blocked by the caller.
    std::string preserved;
    return sfCampaignTemporary(path+".preserved-",bytes,preserved) && sfSyncCampaignDirectory(path);
}

static void sfLoadCampaign()
{
    if (sfCampaignLoaded) return;
    sfCampaignLoaded=true;
    sfCampaignStorageBlocked=false;sfCampaignStorageError.clear();
    if (sfCampaignStoragePath.empty()) {
        char *path=SDL_GetPrefPath("greenpower2669","SpaceFortressVs");
        if (!path) {
            sfCampaignStorageError="SAUVEGARDE INDISPONIBLE"; sfCampaignStorageBlocked=true; return;
        }
        sfCampaignStoragePath=std::string(path)+"campaign-v1.dat"; SDL_free(path);
    }
    SfCampaignSave primary,backup;std::string bytes,backupBytes;
    const auto state=sfInspectCampaign(sfCampaignStoragePath,primary,bytes);
    if (state==SfSaveFileState::Valid) {sfCampaignSave=std::move(primary);return;}
    const auto backupState=sfInspectCampaign(sfCampaignStoragePath+".bak",backup,backupBytes);
    if (backupState==SfSaveFileState::Valid) sfCampaignSave=std::move(backup);
    if (state==SfSaveFileState::Unknown || backupState==SfSaveFileState::Unknown) {
        sfCampaignStorageError="FORMAT INCONNU - FICHIERS CONSERVES";sfCampaignStorageBlocked=true;
    } else if (state==SfSaveFileState::Inaccessible) {
        sfCampaignStorageError="SAUVEGARDE INACCESSIBLE - FICHIER CONSERVE";sfCampaignStorageBlocked=true;
    } else if (backupState==SfSaveFileState::Valid) {
        sfCampaignStorageError="COPIE DE SECOURS RESTAUREE";
    } else if (state!=SfSaveFileState::Missing || backupState!=SfSaveFileState::Missing) {
        sfCampaignStorageError="SAUVEGARDE ILLISIBLE - FICHIERS CONSERVES";sfCampaignStorageBlocked=true;
    }
}

static bool sfSaveCampaign(const SfCampaignSave &candidate)
{
    sfLoadCampaign();
    if (sfCampaignStorageBlocked || sfCampaignStoragePath.empty()) return false;
    const std::string bytes=sfEncodeCampaign(candidate),backupPath=sfCampaignStoragePath+".bak";
    SfCampaignSave decoded;std::istringstream validation(bytes);
    if (!sfDecodeCampaign(validation,decoded)) {
        sfCampaignStorageError="DONNEES INVALIDES - FICHIERS CONSERVES";return false;
    }
    // Recheck on every write: a file can be replaced after the initial load.
    SfCampaignSave primary,backup;std::string oldBytes,backupBytes;
    const auto state=sfInspectCampaign(sfCampaignStoragePath,primary,oldBytes);
    const auto backupState=sfInspectCampaign(backupPath,backup,backupBytes);
    for (const auto current : {state,backupState}) {
        if (current==SfSaveFileState::Unknown || current==SfSaveFileState::Inaccessible) {
            sfCampaignStorageError="SAUVEGARDE PROTEGEE - FICHIERS CONSERVES";
            sfCampaignStorageBlocked=true;return false;
        }
    }
    if ((state==SfSaveFileState::Invalid && !sfPreserveCampaignBytes(sfCampaignStoragePath,oldBytes)) ||
        (backupState==SfSaveFileState::Invalid && !sfPreserveCampaignBytes(backupPath,backupBytes))) {
        sfCampaignStorageError="ECHEC COPIE DE RECUPERATION - FICHIERS CONSERVES";return false;
    }
    // Preserve the complete original v1 bytes independently of the rotating
    // backup before migration. Scores, IDs, names and pending victory map 1:1.
    for (const auto &source : {std::make_pair(state,oldBytes),std::make_pair(backupState,backupBytes)}) {
        if (source.first==SfSaveFileState::Valid && source.second.rfind("SPACEFORTRESS_CAMPAIGN 1\n",0)==0 &&
            !sfPreserveCampaignBytes(sfCampaignStoragePath+".v1-original",source.second)) {
            sfCampaignStorageError="ECHEC ARCHIVE V1 - FICHIERS CONSERVES";return false;
        }
    }
    if (state==SfSaveFileState::Valid && !sfAtomicCampaignWrite(backupPath,oldBytes)) {
        sfCampaignStorageError="ECHEC COPIE - REESSAYER";return false;
    }
    if (!sfAtomicCampaignWrite(sfCampaignStoragePath,bytes)) {
        sfCampaignStorageError="ECHEC SAUVEGARDE - REESSAYER"; return false;
    }
    sfCampaignSave=candidate; sfCampaignStorageError.clear(); return true;
}

static bool sfRecordCampaignVictory(std::array<std::string,3> names)
{
    if (!sfCampaignSave.pending) return false;
    for (auto &name : names) { name=sfCleanName(name); if (name.empty()) return false; }
    auto next=sfCampaignSave;
    next.names=names; next.victory.names=names;
    const auto found=std::find_if(next.fame.begin(),next.fame.end(),[&](const SfFameEntry &e){return e.id==next.victory.id;});
    if (found==next.fame.end()) next.fame.push_back(next.victory);
    next.pending=false;
    return sfSaveCampaign(next);
}
