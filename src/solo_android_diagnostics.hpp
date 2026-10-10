#pragma once
// Native Android diagnostics for DEBUG APKs only. No network, no permissions.
// This file is intentionally a no-op in desktop and Android RELEASE builds.

#if defined(__ANDROID__) && defined(SF_DIAGNOSTICS_DEBUG)
#include <SDL2/SDL_system.h>
#include <android/log.h>
#include <cstdio>
#include <ctime>
#include <string>

inline void sfDebugNativeLog(const char *mode,const char *event) {
    const char *safeMode=mode ? mode : "unknown";
    const char *safeEvent=event ? event : "unknown";
    __android_log_print(ANDROID_LOG_INFO,"SpaceFortressDebug",
                        "%s: %.200s",safeMode,safeEvent);
    const char *root=SDL_AndroidGetInternalStoragePath();
    if(!root || !*root)return;
    const std::string basename=std::string("spacefortress-native-")+
                               ((std::string(safeMode)=="solo")?"solo":"classic")+
                               ".log";
    const std::string path=std::string(root)+"/"+basename;
    if(FILE *check=std::fopen(path.c_str(),"rb")){
        std::fseek(check,0,SEEK_END);
        long size=std::ftell(check);
        std::fclose(check);
        if(size>256*1024){
            if(FILE *reset=std::fopen(path.c_str(),"wb")){
                std::fputs("Journal natif limite a 256 Ko, historique tronque.\n",reset);
                std::fclose(reset);
            }
        }
    }
    if(FILE *out=std::fopen(path.c_str(),"ab")){
        std::fprintf(out,"%lld %s %s\n",
                     static_cast<long long>(std::time(nullptr)),
                     safeMode,safeEvent);
        std::fflush(out);
        std::fclose(out);
    }
}
#else
inline void sfDebugNativeLog(const char *,const char *) {}
#endif
