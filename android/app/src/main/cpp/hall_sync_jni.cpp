#include <jni.h>
#include <SDL2/SDL.h>
#include <codecvt>
#include <locale>
#include <string>
#include <utility>

#include "game_mode.hpp"
#include "hall_sync_runtime.hpp"

namespace {
JavaVM *gVm=nullptr;
jclass gClientClass=nullptr;
jmethodID gSubmitMethod=nullptr;
jmethodID gSyncPageMethod=nullptr;
jmethodID gGameVersionMethod=nullptr;
jmethodID gConfiguredMethod=nullptr;

SfHallSyncError errorFromJava(jint code)
{
    switch(code){
        case 1:return SfHallSyncError::Offline;
        case 2:return SfHallSyncError::Timeout;
        case 3:return SfHallSyncError::Auth;
        case 4:return SfHallSyncError::Server;
        case 5:return SfHallSyncError::Protocol;
        case 6:return SfHallSyncError::NotConfigured;
        default:return SfHallSyncError::Protocol;
    }
}

JNIEnv *currentEnv(bool &attached)
{
    attached=false;
    if(!gVm) return nullptr;
    JNIEnv *env=nullptr;
    const jint result=gVm->GetEnv(reinterpret_cast<void **>(&env),JNI_VERSION_1_6);
    if(result==JNI_OK) return env;
    if(result!=JNI_EDETACHED) return nullptr;
    if(gVm->AttachCurrentThread(&env,nullptr)!=JNI_OK) return nullptr;
    attached=true;
    return env;
}

void detachIfNeeded(bool attached)
{
    if(attached && gVm) gVm->DetachCurrentThread();
}

jstring toJavaString(JNIEnv *env,const std::string &value)
{
    if(!env) return nullptr;
    try{
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
        const std::u16string utf16=convert.from_bytes(value);
        return env->NewString(reinterpret_cast<const jchar *>(utf16.data()),static_cast<jsize>(utf16.size()));
    }catch(...){
        return nullptr;
    }
}

bool fromJavaString(JNIEnv *env,jstring value,std::string &out)
{
    if(!env || !value) return false;
    const jchar *chars=env->GetStringChars(value,nullptr);
    if(!chars || env->ExceptionCheck()){
        if(env->ExceptionCheck()) env->ExceptionClear();
        return false;
    }
    const jsize length=env->GetStringLength(value);
    try{
        std::u16string utf16(reinterpret_cast<const char16_t *>(chars),static_cast<size_t>(length));
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
        out=convert.to_bytes(utf16);
    }catch(...){
        env->ReleaseStringChars(value,chars);
        return false;
    }
    env->ReleaseStringChars(value,chars);
    return true;
}

void clearJavaException(JNIEnv *env)
{
    if(env && env->ExceptionCheck()) env->ExceptionClear();
}

void javaSubmit(const SfHallUploadRequest &request)
{
    bool attached=false;
    JNIEnv *env=currentEnv(attached);
    if(!env || !gClientClass || !gSubmitMethod){
        detachIfNeeded(attached);
        sfHallTransportUploadFailed(request.submissionId,SfHallSyncError::NotConfigured);
        return;
    }
    jstring submission=toJavaString(env,request.submissionId);
    jstring player=toJavaString(env,request.playerName);
    jstring pilot0=toJavaString(env,request.pilots[0]);
    jstring pilot1=toJavaString(env,request.pilots[1]);
    jstring difficulty=toJavaString(env,request.difficulty);
    jstring version=toJavaString(env,request.gameVersion);
    jstring mode=toJavaString(env,request.mode);
    if(!submission || !player || !pilot0 || !pilot1 || !difficulty || !version || !mode){
        clearJavaException(env);
        for(jobject ref:{static_cast<jobject>(submission),static_cast<jobject>(player),static_cast<jobject>(pilot0),static_cast<jobject>(pilot1),static_cast<jobject>(difficulty),static_cast<jobject>(version),static_cast<jobject>(mode)}) if(ref) env->DeleteLocalRef(ref);
        detachIfNeeded(attached);
        sfHallTransportUploadFailed(request.submissionId,SfHallSyncError::Protocol);
        return;
    }
    env->CallStaticVoidMethod(gClientClass,gSubmitMethod,
        submission,player,pilot0,pilot1,
        static_cast<jint>(request.boss),difficulty,static_cast<jlong>(request.durationMs),
        static_cast<jint>(request.points),static_cast<jint>(request.stars),version,
        static_cast<jlong>(request.completedAtEpochSeconds),mode,static_cast<jint>(request.encounter));
    const bool failed=env->ExceptionCheck();
    clearJavaException(env);
    for(jobject ref:{static_cast<jobject>(submission),static_cast<jobject>(player),static_cast<jobject>(pilot0),static_cast<jobject>(pilot1),static_cast<jobject>(difficulty),static_cast<jobject>(version),static_cast<jobject>(mode)}) env->DeleteLocalRef(ref);
    detachIfNeeded(attached);
    if(failed) sfHallTransportUploadFailed(request.submissionId,SfHallSyncError::Protocol);
}

void javaSyncPage(uint64_t cycleId,const std::string &cursor,int limit)
{
    bool attached=false;
    JNIEnv *env=currentEnv(attached);
    if(!env || !gClientClass || !gSyncPageMethod){
        detachIfNeeded(attached);
        sfHallTransportPageFailed(cycleId,cursor,SfHallSyncError::NotConfigured);
        return;
    }
    jstring javaCursor=toJavaString(env,cursor);
    if(!javaCursor){
        clearJavaException(env);detachIfNeeded(attached);
        sfHallTransportPageFailed(cycleId,cursor,SfHallSyncError::Protocol);return;
    }
    env->CallStaticVoidMethod(gClientClass,gSyncPageMethod,static_cast<jlong>(cycleId),javaCursor,static_cast<jint>(limit));
    const bool failed=env->ExceptionCheck();
    clearJavaException(env);env->DeleteLocalRef(javaCursor);detachIfNeeded(attached);
    if(failed) sfHallTransportPageFailed(cycleId,cursor,SfHallSyncError::Protocol);
}
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm,void *)
{
    gVm=vm;
    JNIEnv *env=nullptr;
    if(!vm || vm->GetEnv(reinterpret_cast<void **>(&env),JNI_VERSION_1_6)!=JNI_OK) return JNI_VERSION_1_6;
    jclass local=env->FindClass("com/greenpower2669/spacefortressvs/HallOfFameSyncClient");
    if(!local){clearJavaException(env);sfHallInstallTransport({});return JNI_VERSION_1_6;}
    gClientClass=static_cast<jclass>(env->NewGlobalRef(local));
    env->DeleteLocalRef(local);
    if(!gClientClass){clearJavaException(env);sfHallInstallTransport({});return JNI_VERSION_1_6;}

    gSubmitMethod=env->GetStaticMethodID(gClientClass,"submit","(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;JIILjava/lang/String;JLjava/lang/String;I)V");
    gSyncPageMethod=env->GetStaticMethodID(gClientClass,"syncPage","(JLjava/lang/String;I)V");
    gGameVersionMethod=env->GetStaticMethodID(gClientClass,"gameVersion","()Ljava/lang/String;");
    gConfiguredMethod=env->GetStaticMethodID(gClientClass,"isUploadConfigured","()Z");
    if(env->ExceptionCheck() || !gSubmitMethod || !gSyncPageMethod || !gGameVersionMethod || !gConfiguredMethod){
        clearJavaException(env);sfHallInstallTransport({});return JNI_VERSION_1_6;
    }

    jstring version=static_cast<jstring>(env->CallStaticObjectMethod(gClientClass,gGameVersionMethod));
    std::string nativeVersion;
    if(!env->ExceptionCheck() && fromJavaString(env,version,nativeVersion) && !nativeVersion.empty()) sfHallSetGameVersion(nativeVersion);
    clearJavaException(env);
    if(version) env->DeleteLocalRef(version);

    const jboolean configured=env->CallStaticBooleanMethod(gClientClass,gConfiguredMethod);
    if(env->ExceptionCheck()) clearJavaException(env);

    SfHallTransport transport;
    if(configured==JNI_TRUE) transport.submit=[](const SfHallUploadRequest &request){javaSubmit(request);};
    transport.syncPage=[](uint64_t cycleId,const std::string &cursor,int limit){javaSyncPage(cycleId,cursor,limit);};
    sfHallInstallTransport(std::move(transport));
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm,void *)
{
    JNIEnv *env=nullptr;
    if(vm && vm->GetEnv(reinterpret_cast<void **>(&env),JNI_VERSION_1_6)==JNI_OK && env && gClientClass) env->DeleteGlobalRef(gClientClass);
    gClientClass=nullptr;gVm=nullptr;gSubmitMethod=nullptr;gSyncPageMethod=nullptr;gGameVersionMethod=nullptr;gConfiguredMethod=nullptr;
    sfHallInstallTransport({});
}

extern "C" JNIEXPORT void JNICALL
Java_com_greenpower2669_spacefortressvs_HallOfFameSyncClient_nativeUploadAccepted(JNIEnv *env,jclass,jstring submission,jstring serverId)
{
    std::string nativeSubmission,nativeServer;
    if(!fromJavaString(env,submission,nativeSubmission) || !fromJavaString(env,serverId,nativeServer)) return;
    sfHallTransportUploadAccepted(nativeSubmission,nativeServer);
}

extern "C" JNIEXPORT void JNICALL
Java_com_greenpower2669_spacefortressvs_HallOfFameSyncClient_nativeUploadFailed(JNIEnv *env,jclass,jstring submission,jint errorCode)
{
    std::string nativeSubmission;
    if(!fromJavaString(env,submission,nativeSubmission)) return;
    sfHallTransportUploadFailed(nativeSubmission,errorFromJava(errorCode));
}

extern "C" JNIEXPORT void JNICALL
Java_com_greenpower2669_spacefortressvs_HallOfFameSyncClient_nativeRemoteEntry(JNIEnv *env,jclass,
    jlong cycleId,jstring id,jstring submissionId,jstring playerName,jstring pilot0,jstring pilot1,
    jint boss,jstring difficulty,jlong durationMs,jint points,jint stars,jstring completedAt,jstring mode,jint encounter,jint serverRank)
{
    SfHallRemoteEntry entry;
    if(!fromJavaString(env,id,entry.id) || !fromJavaString(env,playerName,entry.playerName) ||
       !fromJavaString(env,pilot0,entry.pilots[0]) || !fromJavaString(env,pilot1,entry.pilots[1]) ||
       !fromJavaString(env,difficulty,entry.difficulty) || !fromJavaString(env,completedAt,entry.completedAt) ||
       !fromJavaString(env,mode,entry.mode)) return;
    if(submissionId && !fromJavaString(env,submissionId,entry.submissionId)) return;
    entry.boss=static_cast<int>(boss);entry.durationMs=static_cast<long long>(durationMs);
    entry.points=static_cast<int>(points);entry.stars=static_cast<int>(stars);
    entry.encounter=static_cast<int>(encounter);entry.serverRank=static_cast<int>(serverRank);
    sfHallTransportRemoteEntry(static_cast<uint64_t>(cycleId),entry);
}

extern "C" JNIEXPORT void JNICALL
Java_com_greenpower2669_spacefortressvs_HallOfFameSyncClient_nativePageDone(JNIEnv *env,jclass,jlong cycleId,jstring requestedCursor,jstring nextCursor,jboolean hasMore)
{
    std::string requested,next;
    if(!fromJavaString(env,requestedCursor,requested) || !fromJavaString(env,nextCursor,next)) return;
    sfHallTransportPageDone(static_cast<uint64_t>(cycleId),requested,next,hasMore==JNI_TRUE);
}

extern "C" JNIEXPORT void JNICALL
Java_com_greenpower2669_spacefortressvs_HallOfFameSyncClient_nativePageFailed(JNIEnv *env,jclass,jlong cycleId,jstring requestedCursor,jint errorCode)
{
    std::string requested;
    if(!fromJavaString(env,requestedCursor,requested)) return;
    sfHallTransportPageFailed(static_cast<uint64_t>(cycleId),requested,errorFromJava(errorCode));
}
