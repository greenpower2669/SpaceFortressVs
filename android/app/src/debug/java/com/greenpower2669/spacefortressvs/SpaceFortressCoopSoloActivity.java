package com.greenpower2669.spacefortressvs;

/**
 * Debug-only: creates a new independent Android :coopsolo process and runs
 * the HISTORICAL COOP engine with one active pilot. The experimental original
 * SOLO labyrinth and public CLASSIC/COOP launchers remain unchanged.
 */
public final class SpaceFortressCoopSoloActivity extends SpaceFortressActivity {
    @Override
    protected String[] getArguments() {
        return new String[] { "--spacefortress-coop-solo-arena" };
    }
}
