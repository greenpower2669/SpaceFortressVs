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
static bool sfReadFame(std::istream &in,SfFameEntry &entry)
{
    if (!(in>>entry.id>>entry.boss>>entry.score>>entry.seconds>>entry.mode>>entry.date)) return false;
    for (auto &name : entry.names) if (!(in>>std::quoted(name)) || name.size()>96) return false;
    return entry.id>0 && entry.boss>=1 && entry.boss<=50 && entry.score>=0 && entry.seconds>=0 &&
           (entry.mode==SF_COOP_LOCAL || entry.mode==SF_COOP_AI);
}
static std::string sfEncodeCampaign(const SfCampaignSave &save)
{
    std::ostringstream out;
    out<<"SPACEFORTRESS_CAMPAIGN 1\n"<<save.cleared<<' '<<save.selected<<' '<<save.pending<<' '<<save.fame.size()<<'\n';
    for (const auto &name : save.names) out<<std::quoted(name)<<'\n';
    if (save.pending) sfWriteFame(out,save.victory);
    for (const auto &entry : save.fame) sfWriteFame(out,entry);
    return out.str();
}
static bool sfDecodeCampaign(std::istream &in,SfCampaignSave &save)
{
    SfCampaignSave candidate;
    std::string magic; int version,pending; size_t count;
    if (!(in>>magic>>version) || magic!="SPACEFORTRESS_CAMPAIGN" || version!=1) return false;
    if (!(in>>candidate.cleared>>candidate.selected>>pending>>count) || candidate.cleared<0 ||
        candidate.cleared>50 || candidate.selected<0 || candidate.selected>49 ||
        pending<0 || pending>1 || count>100000) return false;
    candidate.pending=pending;
    for (auto &name : candidate.names) if (!(in>>std::quoted(name)) || name.size()>96) return false;
    if (candidate.pending && !sfReadFame(in,candidate.victory)) return false;
    for (size_t i=0;i<count;++i) {
        SfFameEntry entry; if (!sfReadFame(in,entry)) return false;
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
static bool sfCampaignPathMayExist(const std::string &path)
{
    struct stat info{};
    return ::stat(path.c_str(),&info)==0 || errno!=ENOENT;
}
static void sfLoadCampaign()
{
    if (sfCampaignLoaded) return;
    sfCampaignLoaded=true;
    if (sfCampaignStoragePath.empty()) {
        char *path=SDL_GetPrefPath("greenpower2669","SpaceFortressVs");
        if (!path) {
            sfCampaignStorageError="SAUVEGARDE INDISPONIBLE"; sfCampaignStorageBlocked=true; return;
        }
        sfCampaignStoragePath=std::string(path)+"campaign-v1.dat"; SDL_free(path);
    }
    std::ifstream primary(sfCampaignStoragePath,std::ios::binary);
    if (primary) {
        if (sfDecodeCampaign(primary,sfCampaignSave)) return;
        if (sfReadCampaignFile(sfCampaignStoragePath+".bak",sfCampaignSave)) {
            sfCampaignStorageError="COPIE DE SECOURS RESTAUREE"; return;
        }
        // Keep unreadable/unknown formats intact: no silent reset of the hall.
        sfCampaignStorageError="SAUVEGARDE ILLISIBLE - FICHIER CONSERVE";
        sfCampaignStorageBlocked=true;
    } else if (sfReadCampaignFile(sfCampaignStoragePath+".bak",sfCampaignSave)) {
        sfCampaignStorageError="COPIE DE SECOURS RESTAUREE";
    } else if (sfCampaignPathMayExist(sfCampaignStoragePath) || sfCampaignPathMayExist(sfCampaignStoragePath+".bak")) {
        sfCampaignStorageError="SAUVEGARDE INACCESSIBLE - FICHIER CONSERVE";
        sfCampaignStorageBlocked=true;
    }
}

static bool sfSaveCampaign(const SfCampaignSave &candidate)
{
    sfLoadCampaign();
    if (sfCampaignStorageBlocked || sfCampaignStoragePath.empty()) return false;
    const std::string temporary=sfCampaignStoragePath+".tmp",bytes=sfEncodeCampaign(candidate);
    FILE *file=std::fopen(temporary.c_str(),"wb");
    if (!file) { sfCampaignStorageError="ECHEC SAUVEGARDE - REESSAYER"; return false; }
    bool ok=std::fwrite(bytes.data(),1,bytes.size(),file)==bytes.size();
    ok=std::fflush(file)==0 && ok;
    ok=::fsync(::fileno(file))==0 && ok;
    ok=std::fclose(file)==0 && ok;
    if (!ok) { std::remove(temporary.c_str()); sfCampaignStorageError="ECHEC ECRITURE - REESSAYER"; return false; }
    // Only copy a valid primary into the backup; a recovered file must not
    // replace the last readable backup with the corrupt primary.
    SfCampaignSave old;
    if (sfReadCampaignFile(sfCampaignStoragePath,old)) {
        std::ifstream in(sfCampaignStoragePath,std::ios::binary);
        std::ofstream backup(sfCampaignStoragePath+".bak",std::ios::binary|std::ios::trunc);
        backup<<in.rdbuf(); backup.flush();
        if (!backup) { sfCampaignStorageError="ECHEC COPIE - REESSAYER"; return false; }
    }
    if (std::rename(temporary.c_str(),sfCampaignStoragePath.c_str())!=0) {
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
