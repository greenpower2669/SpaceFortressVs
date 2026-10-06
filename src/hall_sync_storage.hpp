#pragma once

#include "hall_sync.hpp"
#include <cerrno>
#include <cstdio>
#include <fcntl.h>
#include <fstream>
#include <iterator>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

enum class SfHallSyncFileState { Missing, Valid, Invalid, Unknown, Inaccessible };

static bool sfHallSyncDirectoryFsync(const std::string &path)
{
    const auto slash=path.find_last_of('/');
    const std::string directory=slash==std::string::npos ? "." : slash==0 ? "/" : path.substr(0,slash);
    const int fd=::open(directory.c_str(),O_RDONLY|O_DIRECTORY);
    if (fd<0) return false;
    const bool ok=::fsync(fd)==0;
    ::close(fd);
    return ok;
}

static bool sfHallSyncWriteTemp(const std::string &prefix,const std::string &bytes,std::string &outPath)
{
    outPath=prefix+"XXXXXX";
    const int fd=::mkstemp(outPath.data());
    if (fd<0) return false;
    FILE *file=::fdopen(fd,"wb");
    if (!file) {::close(fd);std::remove(outPath.c_str());return false;}
    bool ok=std::fwrite(bytes.data(),1,bytes.size(),file)==bytes.size();
    ok=std::fflush(file)==0 && ok;
    ok=::fsync(::fileno(file))==0 && ok;
    ok=std::fclose(file)==0 && ok;
    if (!ok) std::remove(outPath.c_str());
    return ok;
}

static SfHallSyncFileState sfHallSyncInspect(const std::string &path,SfHallSyncState &state,std::string &bytes)
{
    struct stat info{};
    if (::stat(path.c_str(),&info)!=0) return errno==ENOENT ? SfHallSyncFileState::Missing : SfHallSyncFileState::Inaccessible;
    if (!S_ISREG(info.st_mode)) return SfHallSyncFileState::Inaccessible;
    std::ifstream in(path,std::ios::binary);
    if (!in) return SfHallSyncFileState::Inaccessible;
    bytes.assign(std::istreambuf_iterator<char>(in),{});
    if (in.bad()) return SfHallSyncFileState::Inaccessible;
    std::istringstream decoded(bytes);
    if (sfHallSyncDecode(decoded,state)) return SfHallSyncFileState::Valid;
    std::istringstream header(bytes); std::string magic; int version=0;
    if (header>>magic>>version && magic=="SPACEFORTRESS_HALL_SYNC" && version!=SF_HALL_SYNC_FORMAT_VERSION)
        return SfHallSyncFileState::Unknown;
    return SfHallSyncFileState::Invalid;
}

static bool sfHallSyncAtomicWrite(const std::string &path,const std::string &bytes)
{
    std::string temporary;
    if (!sfHallSyncWriteTemp(path+".tmp-",bytes,temporary)) return false;
    if (std::rename(temporary.c_str(),path.c_str())!=0) {std::remove(temporary.c_str());return false;}
    return sfHallSyncDirectoryFsync(path);
}

static bool sfHallSyncLoadFile(const std::string &path,SfHallSyncState &state,std::string &error)
{
    error.clear();
    SfHallSyncState decoded; std::string bytes;
    switch (sfHallSyncInspect(path,decoded,bytes)) {
        case SfHallSyncFileState::Valid: state=std::move(decoded); return true;
        case SfHallSyncFileState::Missing: state=SfHallSyncState{}; return true;
        case SfHallSyncFileState::Unknown: error="FORMAT INCONNU - SYNC CONSERVEE"; return false;
        case SfHallSyncFileState::Inaccessible: error="SYNC INACCESSIBLE - FICHIER CONSERVE"; return false;
        case SfHallSyncFileState::Invalid: error="SYNC ILLISIBLE - FICHIER CONSERVE"; return false;
    }
    return false;
}

static bool sfHallSyncSaveFile(const std::string &path,const SfHallSyncState &state,std::string &error)
{
    error.clear();
    const std::string encoded=sfHallSyncEncode(state);
    if (encoded.empty()) {error="SYNC DONNEES INVALIDES";return false;}

    SfHallSyncState current; std::string currentBytes;
    const auto fileState=sfHallSyncInspect(path,current,currentBytes);
    if (fileState==SfHallSyncFileState::Unknown) {error="FORMAT INCONNU - SYNC CONSERVEE";return false;}
    if (fileState==SfHallSyncFileState::Inaccessible) {error="SYNC INACCESSIBLE - FICHIER CONSERVE";return false;}
    if (fileState==SfHallSyncFileState::Invalid) {error="SYNC ILLISIBLE - FICHIER CONSERVE";return false;}

    if (fileState==SfHallSyncFileState::Valid) {
        if (!sfHallSyncAtomicWrite(path+".bak",currentBytes)) {error="ECHEC COPIE SYNC";return false;}
    }
    if (!sfHallSyncAtomicWrite(path,encoded)) {error="ECHEC SAUVEGARDE SYNC";return false;}
    return true;
}
