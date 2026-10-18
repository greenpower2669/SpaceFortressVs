package com.greenpower2669.spacefortressvs;

import org.libsdl.app.SDLActivity;
import android.content.Intent;
import android.os.SystemClock;
import android.view.MotionEvent;
import android.widget.Toast;
import java.util.concurrent.atomic.AtomicBoolean;

public class SpaceFortressActivity extends SDLActivity {
    private final AtomicBoolean soloLaunching = new AtomicBoolean(false);

    // SOLO-027: the real classic mode selector is the debug APK launcher.
    // A private twenty-tap L/R upper-corner gesture opens the debug hub
    // from either the selector or SOLO, while normal touch remains SDL-owned.
    private int debugTapCount=0;
    private int debugTapSide=-1;
    private long debugTapLastMs=0;
    private boolean debugTapCaptured=false;

    private int debugTopSide(float x,float y) {
        final android.view.View root=getWindow().getDecorView();
        final float w=root.getWidth(),h=root.getHeight();
        if(w<=0||h<=0||y<0||y>h*.056f)return -1;
        if(x<w*.25f)return 0;
        if(x>w*.75f)return 1;
        return -1;
    }

    @Override public boolean dispatchTouchEvent(MotionEvent event) {
        if(!BuildConfig.DEBUG)return super.dispatchTouchEvent(event);
        final int action=event.getActionMasked();
        if(action==MotionEvent.ACTION_DOWN){
            debugTapSide=event.getPointerCount()==1?
                debugTopSide(event.getX(),event.getY()):-1;
            debugTapCaptured=debugTapSide>=0;
            if(debugTapCaptured)return true;
            debugTapCount=0;
        }
        if(debugTapCaptured){
            if(action==MotionEvent.ACTION_UP){
                final long now=SystemClock.elapsedRealtime();
                final int side=debugTopSide(event.getX(),event.getY());
                if(side!=debugTapSide || now-debugTapLastMs>30000L)
                    debugTapCount=0;
                if(side==debugTapSide){
                    debugTapCount=side==(debugTapCount&1)?
                        debugTapCount+1:(side==0?1:0);
                    debugTapLastMs=now;
                    if(debugTapCount>=20){
                        debugTapCount=0;
                        Intent intent=new Intent();
                        intent.setClassName(this,
                            "com.greenpower2669.spacefortressvs.SpaceFortressHubActivity");
                        try { startActivity(intent); }
                        catch (android.content.ActivityNotFoundException missing) {
                            android.util.Log.w("SpaceFortress","Debug hub unavailable",missing);
                        }
                    }
                }
                debugTapCaptured=false;
            }else if(action==MotionEvent.ACTION_CANCEL ||
                     action==MotionEvent.ACTION_POINTER_DOWN){
                debugTapCaptured=false;debugTapCount=0;
            }
            return true;
        }
        return super.dispatchTouchEvent(event);
    }

    // Called by the native selector. SDL remains owned by its original process.
    public boolean launchSoloGame(int danger) {
        if (!soloLaunching.compareAndSet(false, true)) return true;
        runOnUiThread(() -> {
            try {
                Intent intent = new Intent(this, SpaceFortressSoloGameActivity.class);
                intent.putExtra("bossDanger", Math.max(0, Math.min(8, danger)));
                startActivity(intent);
            } catch (RuntimeException error) {
                soloLaunching.set(false);
                Toast.makeText(this, "Impossible de lancer SOLO", Toast.LENGTH_LONG).show();
            }
        });
        return true;
    }

    @Override
    protected void onResume() {
        super.onResume();
        soloLaunching.set(false);
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
            "SDL2_image",
            "SDL2_mixer",
            "main"
        };
    }

    @Override
    protected String getMainSharedObject() {
        return getApplicationInfo().nativeLibraryDir + "/libmain.so";
    }

    @Override
    protected String getMainFunction() {
        return "SpaceFortressMain";
    }
}
