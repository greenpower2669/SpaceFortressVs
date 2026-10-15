package com.greenpower2669.spacefortressvs;

import org.libsdl.app.SDLActivity;
import android.content.Intent;
import android.widget.Toast;
import java.util.concurrent.atomic.AtomicBoolean;

public class SpaceFortressActivity extends SDLActivity {
    private final AtomicBoolean soloLaunching = new AtomicBoolean(false);

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
