#pragma once
#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <ctime>
#include <cstdint>
#include <unistd.h>
#include <sys/stat.h>
#include <cerrno>

struct SfFameEntry {
    uint64_t id=0;
    int boss=0,score=0,seconds=0,mode=SF_COOP_LOCAL;
    long long date=0;
    std::array<std::string,3> names{};
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
    std::string out;int count=0;
    for (unsigned char c : input) {
        if (c<32 || c==127) continue;
        if ((c&0xc0)!=0x80 && ++count>24) break;
        if (out.size()>=96) break;
        out.push_back(char(c));
    }
    const auto first=out.find_first_not_of(' '),last=out.find_last_not_of(' ');
    return first==std::string::npos ? "" : out.substr(first,last-first+1);
}
static void sfWriteFame(std::ostream &out,const SfFameEntry &e)
{
    out<<e.id<<' '<<e.boss<<' '<<e.score<<' '<<e.seconds<<' '<<e.mode<<' '<<e.date;
    for (const auto &n : e.names) out<<' '<<std::quoted(n);
    out<<'\n';
}
static bool sfReadFame(std::istream &in,SfFameEntry &e)
{
    if (!(in>>e.id>>e.boss>>e.score>>e.seconds>>e.mode>>e.date)) return false;
    for (auto &n : e.names) if (!(in>>std::quoted(n)) || n.size()>96) return false;
    return e.id>0 && e.id<UINT64_MAX && e.boss>=1 && e.boss<=50 && e.score>=0 && e.seconds>=0 &&
           (e.mode==SF_COOP_LOCAL || e.mode==SF_COOP_AI);
}
static std::string sfEncodeCampaign(const SfCampaignSave &s)
{
    std::ostringstream out;
    out<<"SPACEFORTRESS_CAMPAIGN 1\n"<<s.cleared<<' '<<s.selected<<' '<<s.pending<<' '<<s.fame.size()<<'\n';
    for (const auto &n : s.names) out<<std::quoted(n)<<'\n';
    if (s.pending) sfWriteFame(out,s.victory);
    for (const auto &e : s.fame) sfWriteFame(out,e);
    return out.str();
}
static bool sfDecodeCampaign(std::istream &in,SfCampaignSave &out)
{
    SfCampaignSave s;std::string magic;int version,pending;size_t count;
    if (!(in>>magic>>version) || magic!="SPACEFORTRESS_CAMPAIGN" || version!=1) return false;
    if (!(in>>s.cleared>>s.selected>>pending>>count) || s.cleared<0 || s.cleared>50 ||
        s.selected<0 || s.selected>49 || pending<0 || pending>1 || count>100000) return false;
    for (auto &n : s.names) if (!(in>>std::quoted(n)) || n.size()>96) return false;
    s.pending=pending;
    if (s.pending && !sfReadFame(in,s.victory)) return false;
    for (size_t i=0;i<count;++i) {SfFameEntry e;if (!sfReadFame(in,e)) return false;s.fame.push_back(e);}
    in>>std::ws;if (!in.eof()) return false;
    out=std::move(s);return true;
}
static bool sfReadCampaignFile(const std::string &path,SfCampaignSave &save)
{
    std::ifstream in(path,std::ios::binary);return in && sfDecodeCampaign(in,save);
}
static bool sfCampaignPathMayExist(const std::string &path)
{
    struct stat info{};return ::stat(path.c_str(),&info)==0 || errno!=ENOENT;
}
static void sfLoadCampaign()
{
    if (sfCampaignLoaded) return;
    sfCampaignLoaded=true;
    if (sfCampaignStoragePath.empty()) {
        char *path=SDL_GetPrefPath("greenpower2669","SpaceFortressVs");
        if (!path) {sfCampaignStorageError="SAUVEGARDE INDISPONIBLE";sfCampaignStorageBlocked=true;return;}
        sfCampaignStoragePath=std::string(path)+"campaign-v1.dat";SDL_free(path);
    }
    if (sfReadCampaignFile(sfCampaignStoragePath,sfCampaignSave)) return;
    if (sfReadCampaignFile(sfCampaignStoragePath+".bak",sfCampaignSave)) {
        sfCampaignStorageError="COPIE DE SECOURS RESTAUREE";return;
    }
    if (sfCampaignPathMayExist(sfCampaignStoragePath) || sfCampaignPathMayExist(sfCampaignStoragePath+".bak")) {
        sfCampaignStorageError="SAUVEGARDE ILLISIBLE - FICHIER CONSERVE";sfCampaignStorageBlocked=true;
    }
}
static bool sfCampaignAtomicWrite(const std::string &path,const std::string &bytes)
{
    const std::string temp=path+".tmp";
    FILE *file=std::fopen(temp.c_str(),"wb");
    if (!file) return false;
    bool ok=std::fwrite(bytes.data(),1,bytes.size(),file)==bytes.size();
    ok=std::fflush(file)==0 && ok;ok=::fsync(::fileno(file))==0 && ok;ok=std::fclose(file)==0 && ok;
    if (!ok || std::rename(temp.c_str(),path.c_str())!=0) {std::remove(temp.c_str());return false;}
    return true;
}
static bool sfSaveCampaign(const SfCampaignSave &candidate)
{
    sfLoadCampaign();
    if (sfCampaignStorageBlocked || sfCampaignStoragePath.empty()) return false;
    SfCampaignSave old;
    if (sfReadCampaignFile(sfCampaignStoragePath,old) &&
        !sfCampaignAtomicWrite(sfCampaignStoragePath+".bak",sfEncodeCampaign(old))) {
        sfCampaignStorageError="ECHEC COPIE - REESSAYER";return false;
    }
    if (!sfCampaignAtomicWrite(sfCampaignStoragePath,sfEncodeCampaign(candidate))) {
        sfCampaignStorageError="ECHEC SAUVEGARDE - REESSAYER";return false;
    }
    sfCampaignSave=candidate;sfCampaignStorageError.clear();return true;
}
static bool sfRecordCampaignVictory(std::array<std::string,3> names)
{
    if (!sfCampaignSave.pending) return false;
    for (auto &n : names) {n=sfCleanName(n);if (n.empty()) return false;}
    auto next=sfCampaignSave;next.names=names;next.victory.names=names;
    const auto found=std::find_if(next.fame.begin(),next.fame.end(),[&](const auto &e){return e.id==next.victory.id;});
    if (found==next.fame.end()) next.fame.push_back(next.victory);
    next.pending=false;return sfSaveCampaign(next);
}
