package com.greenpower2669.spacefortressvs;

import org.libsdl.app.SDLActivity;

public class SpaceFortressActivity extends SDLActivity {
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
