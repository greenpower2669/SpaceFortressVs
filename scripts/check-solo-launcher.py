#!/usr/bin/env python3
"""Preflight: diagnostic APK starts historical selector, hidden hub uses 20 taps.

The real manifest merge/build is additionally verified by Android Gradle.
"""
from pathlib import Path
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[1]
APP = ROOT / "android/app/src"
A = "{http://schemas.android.com/apk/res/android}"
T = "{http://schemas.android.com/tools}"
LEGACY = "com.greenpower2669.spacefortressvs.SpaceFortressActivity"
SOLO = "com.greenpower2669.spacefortressvs.SpaceFortressSoloActivity"
HUB = "com.greenpower2669.spacefortressvs.SpaceFortressHubActivity"
SOLO_GAME = "com.greenpower2669.spacefortressvs.SpaceFortressSoloGameActivity"
COOP_SOLO = "com.greenpower2669.spacefortressvs.SpaceFortressCoopSoloActivity"

def activities(path):
    document = ET.parse(path)
    return {item.get(A+"name"): item
            for item in document.findall("./application/activity")}

def launcher(activity):
    return any(
        any(a.get(A+"name") == "android.intent.action.MAIN"
            for a in filt.findall("action"))
        and any(c.get(A+"name") == "android.intent.category.LAUNCHER"
                for c in filt.findall("category"))
        for filt in activity.findall("intent-filter"))

main = activities(APP / "main/AndroidManifest.xml")
debug = activities(APP / "debug/AndroidManifest.xml")
assert SOLO_GAME in main and not launcher(main[SOLO_GAME])
assert main[SOLO_GAME].get(A+"exported")=="false"
assert main[SOLO_GAME].get(A+"process")==":sologame"
assert LEGACY in main and launcher(main[LEGACY]), "release launcher changed"
assert ET.parse(APP / "debug/AndroidManifest.xml").getroot().find(
    "./application").get(A+"name", "").endswith(".SpaceFortressDebugApplication"), (
    "debug application does not register crash journal")
assert debug[SOLO].get(A+"process") == ":solo", (
    "SDL SOLO must launch in a separate native process")
assert SOLO not in main and HUB not in main and COOP_SOLO not in main, (
    "diagnostic activity leaked into release")
assert debug[COOP_SOLO].get(A+"process")==":coopsolo"
assert set(debug) == {LEGACY, SOLO, COOP_SOLO, HUB}, "unexpected debug activities"
assert debug[LEGACY].get(T+"node") == "replace", "original launcher not replaced"
assert launcher(debug[LEGACY]), "debug must launch genuine SOLO/COOP/DUEL mode selector"
assert not launcher(debug[SOLO]) and not launcher(debug[COOP_SOLO])
assert not launcher(debug[HUB]) and debug[HUB].get(A+"exported")=="false", "hub must be hidden"
assert not launcher(debug[HUB]) and SOLO_GAME not in debug, "SOLO launcher cannot replace selector"
java = (APP / "debug/java/com/greenpower2669/spacefortressvs/SpaceFortressHubActivity.java").read_text()
assert "SpaceFortressActivity.class" in java and "SpaceFortressSoloActivity.class" in java
assert "SpaceFortressCoopSoloActivity.class" in java
assert "EXPORTER JOURNAL DEBUG" in java and "ACTION_CREATE_DOCUMENT" in java
assert (APP / "debug/java/com/greenpower2669/spacefortressvs/SpaceFortressDebugLog.java").is_file()
assert (APP / "debug/java/com/greenpower2669/spacefortressvs/SpaceFortressDebugApplication.java").is_file()
native = (ROOT / "src/solo_android_diagnostics.hpp").read_text()
assert "SF_DIAGNOSTICS_DEBUG" in native
assert "spacefortress-native-" in native
gradle = (ROOT / "android/app/build.gradle").read_text()
assert "applicationIdSuffix '.diagnostic'" in gradle, (
    "debug crash diagnosis must not overwrite the signed original package")
gesture = (APP / "main/java/com/greenpower2669/spacefortressvs/SpaceFortressActivity.java").read_text()
assert "debugTapCount>=20" in gesture and "BuildConfig.DEBUG" in gesture
assert "SpaceFortressHubActivity" in gesture
assert debug[HUB].get(A+"label") == "SpaceFortress DIAG", (
    "hidden debug hub label must remain identifiable")


print("SOLO debug hub static checks OK; release launcher preserved")
