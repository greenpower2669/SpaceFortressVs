#!/usr/bin/env python3
"""Preflight: the unified debug hub is opt-in, never a release change.

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
assert LEGACY in main and launcher(main[LEGACY]), "release launcher changed"
assert SOLO not in main and HUB not in main, "prototype leaked into release"
assert set(debug) == {LEGACY, SOLO, HUB}, "unexpected debug activities"
assert debug[LEGACY].get(T+"node") == "replace", "original launcher not replaced"
assert not launcher(debug[LEGACY]) and not launcher(debug[SOLO])
assert launcher(debug[HUB]), "single debug launcher missing"
java = (APP / "debug/java/com/greenpower2669/spacefortressvs/SpaceFortressHubActivity.java").read_text()
assert "SpaceFortressActivity.class" in java and "SpaceFortressSoloActivity.class" in java
print("SOLO debug hub static checks OK; release launcher preserved")
