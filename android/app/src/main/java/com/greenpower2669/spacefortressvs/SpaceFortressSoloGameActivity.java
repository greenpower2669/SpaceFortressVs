package com.greenpower2669.spacefortressvs;

/** Internal game entry, reached only through the real native mode selector. */
public final class SpaceFortressSoloGameActivity extends SpaceFortressActivity {
    @Override
    protected String[] getArguments() {
        int danger = Math.max(0, Math.min(8, getIntent().getIntExtra("bossDanger", 2)));
        return new String[] {"--spacefortress-solo-game", "--boss-danger=" + danger};
    }
}
