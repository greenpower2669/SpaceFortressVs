# Space Fortress — Android build

This directory wraps the historical C++/SDL2 game in a modern Android project.
The original `src/main.cpp` is preserved. Guarded build-time corrections are
applied to its Android copy; shared vector helpers include numeric bug fixes.

## Toolchain

- Android Gradle Plugin 9.4.0
- Gradle 9.6.0
- compileSdk / targetSdk 36 (Android 16)
- NDK 28.2.13676358
- CMake 3.22.1
- arm64-v8a
- SDL2 2.32.10
- SDL2_image 2.8.12
- SDL2_mixer 2.8.2
- Python 3 (asset preparation and validation)

The SDL dependencies are fetched by `scripts/fetch-sdl.sh` and are not committed.
`scripts/prepare-assets.py` assembles the APK assets at `resources/assets/`,
matching the historical C++ paths. It validates PNG chunks, compression and
scanlines. The two known corrupt heart/burst transports use verified historical
art; new unknown corruption fails the build. Incomplete scenic text packs are
excluded from the APK.

## GitHub Actions

`.github/workflows/android-build.yml` builds and publishes as workflow artifacts:

- a debug APK, directly installable on Android;
- a debug AAB for build validation;
- an unsigned release AAB.

Before building Android, CI runs `scripts/test-regressions.sh`: image validation,
real SDL software-renderer lifecycle/input/vector tests with UBSan, and a syntax
check of the same generated game source compiled by the Android build.

See [the September regression audit](../docs/regression-audit-2026-09-12.md)
for the corrections and the remaining device checks.

A stable Play Store release will need the final application id/version and an upload
keystore. Those signing values should be added later as GitHub Actions secrets rather
than committed to the repository.

Current provisional application id: `com.greenpower2669.spacefortressvs`.
