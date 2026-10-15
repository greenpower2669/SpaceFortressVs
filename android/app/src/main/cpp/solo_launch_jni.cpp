// SDL supplies the thread-local JNI environment; no FabLib, no JNI_OnLoad.
#include <jni.h>
#include <SDL.h>
#include <SDL_system.h>
extern "C" bool sfLaunchSoloGame(int danger) {
    auto *env=static_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());
    auto activity=static_cast<jobject>(SDL_AndroidGetActivity());
    if(!env || !activity)return false;
    jclass type=env->GetObjectClass(activity);
    jmethodID method=type?env->GetMethodID(type,"launchSoloGame","(I)Z"):nullptr;
    bool started=false;
    if(method)started=env->CallBooleanMethod(activity,method,jint(danger))==JNI_TRUE;
    if(env->ExceptionCheck()){env->ExceptionClear();started=false;}
    if(type)env->DeleteLocalRef(type);
    env->DeleteLocalRef(activity);
    return started;
}
