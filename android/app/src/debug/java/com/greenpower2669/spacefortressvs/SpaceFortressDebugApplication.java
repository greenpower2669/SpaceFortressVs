package com.greenpower2669.spacefortressvs;

import android.app.Activity;
import android.app.Application;
import android.os.Bundle;

/**
 * Opt-in journal only for APK debug; Play Store release has no journal hooks.
 * Logs activity transitions so an abrupt native crash can be correlated with
 * Android's own ApplicationExitInfo at the next startup.
 */
public final class SpaceFortressDebugApplication extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        SpaceFortressDebugLog.record(this,"APP_START build=solo-debug-journal-v1");
        final Thread.UncaughtExceptionHandler previous=
                Thread.getDefaultUncaughtExceptionHandler();
        Thread.setDefaultUncaughtExceptionHandler((thread,error)->{
            SpaceFortressDebugLog.recordCrash(this,thread,error);
            if(previous!=null)previous.uncaughtException(thread,error);
            else {
                android.os.Process.killProcess(android.os.Process.myPid());
                System.exit(10);
            }
        });
        registerActivityLifecycleCallbacks(new ActivityLifecycleCallbacks(){
            @Override public void onActivityCreated(Activity activity,Bundle state){
                SpaceFortressDebugLog.record(activity,
                        "ACTIVITY_CREATED "+activity.getClass().getSimpleName());
            }
            @Override public void onActivityStarted(Activity activity){
                SpaceFortressDebugLog.record(activity,
                        "ACTIVITY_STARTED "+activity.getClass().getSimpleName());
            }
            @Override public void onActivityResumed(Activity activity){
                SpaceFortressDebugLog.record(activity,
                        "ACTIVITY_RESUMED "+activity.getClass().getSimpleName());
            }
            @Override public void onActivityPaused(Activity activity){
                SpaceFortressDebugLog.record(activity,
                        "ACTIVITY_PAUSED "+activity.getClass().getSimpleName());
            }
            @Override public void onActivityStopped(Activity activity){
                SpaceFortressDebugLog.record(activity,
                        "ACTIVITY_STOPPED "+activity.getClass().getSimpleName());
            }
            @Override public void onActivitySaveInstanceState(
                    Activity activity,Bundle state){}
            @Override public void onActivityDestroyed(Activity activity){
                SpaceFortressDebugLog.record(activity,
                        "ACTIVITY_DESTROYED "+activity.getClass().getSimpleName());
            }
        });
    }
}
