package com.greenpower2669.spacefortressvs;

/**
 * Debug-only manifest entry. Launches the SOLO prototype without changing the
 * historical activity, menus, saves or default entry point.
 * This is NOT the final canonical COOP-powered SOLO gameplay.
 */
public class SpaceFortressSoloActivity extends SpaceFortressActivity {
    @Override
    protected String[] getArguments() {
        return new String[] { "--spacefortress-solo-prototype" };
    }
}
