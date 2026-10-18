package com.greenpower2669.spacefortressvs;

import android.content.Intent;
import android.os.SystemClock;
import android.view.MotionEvent;

/** Internal game entry, reached only through the real native mode selector. */
public final class SpaceFortressSoloGameActivity extends SpaceFortressActivity {
    // SOLO-026 (debug APK only): a hidden twenty-tap LEFT-RIGHT sequence
    // in the upper side bands opens the old diagnostic hub. The user-facing
    // launcher now enters the real SOLO directly. Production ignores this.
    private int debugTapCount=0;
    private int debugTapSide=-1;
    private long debugTapLastMs=0;
    private boolean debugTapCaptured=false;

    private int debugTopSide(float x,float y){
        final android.view.View view=getWindow().getDecorView();
        final float width=view.getWidth(),height=view.getHeight();
        if(width<=0||height<=0||y<height*.065f||y>height*.145f)return -1;
        if(x<width*.25f)return 0;
        if(x>width*.75f)return 1;
        return -1;
    }

    @Override public boolean dispatchTouchEvent(MotionEvent event){
        if(!BuildConfig.DEBUG)return super.dispatchTouchEvent(event);
        final int action=event.getActionMasked();
        if(action==MotionEvent.ACTION_DOWN){
            debugTapSide=event.getPointerCount()==1?
                debugTopSide(event.getX(),event.getY()):-1;
            debugTapCaptured=debugTapSide>=0;
            if(debugTapCaptured)return true; // do not accidentally fire a weapon
            debugTapCount=0;
        }
        if(debugTapCaptured){
            if(action==MotionEvent.ACTION_UP){
                final long now=SystemClock.elapsedRealtime();
                final int actual=debugTopSide(event.getX(),event.getY());
                if(actual!=debugTapSide || now-debugTapLastMs>30000L)
                    debugTapCount=0;
                if(actual==debugTapSide){
                    // Strict LEFT, RIGHT, ... LEFT, RIGHT: 20 separate taps.
                    debugTapCount=actual==(debugTapCount&1)?
                        debugTapCount+1:(actual==0?1:0);
                    debugTapLastMs=now;
                    if(debugTapCount>=20){
                        debugTapCount=0;
                        SpaceFortressDebugGate.openHub(this);
                    }
                }
                debugTapCaptured=false;
            } else if(action==MotionEvent.ACTION_CANCEL ||
                      action==MotionEvent.ACTION_POINTER_DOWN){
                debugTapCaptured=false;
                debugTapCount=0;
            }
            return true;
        }
        return super.dispatchTouchEvent(event);
    }

    @Override
    protected String[] getArguments() {
        int danger = Math.max(0, Math.min(8, getIntent().getIntExtra("bossDanger", 2)));
        return new String[] {"--spacefortress-solo-game", "--boss-danger=" + danger};
    }
}

/** Android debug hub is never loaded in production builds. */
final class SpaceFortressDebugGate {
    private SpaceFortressDebugGate(){}
    static void openHub(SpaceFortressSoloGameActivity activity){
        if(!BuildConfig.DEBUG)return;
        Intent intent=new Intent();
        intent.setClassName(activity,
            "com.greenpower2669.spacefortressvs.SpaceFortressHubActivity");
        try { activity.startActivity(intent); }
        catch (android.content.ActivityNotFoundException missing) {
            android.util.Log.w("SpaceFortress", "Debug hub not installed", missing);
        }
    }
}
