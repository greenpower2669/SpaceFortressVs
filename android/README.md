# Space Fortress — Android build

This directory wraps the historical C++/SDL2 game in a modern Android project.
The gameplay source under `src/` is kept as-is.

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

The SDL dependencies are fetched by `scripts/fetch-sdl.sh` and are not committed.
The original `assets/` directory is copied at build time to the APK asset path
`resources/assets/`, matching the historical paths used by the C++ code.

## GitHub Actions

`.github/workflows/android-build.yml` builds:

- a debug APK, directly installable on Android;
- a debug AAB for build validation;
- an unsigned release AAB.

A stable Play Store release will need the final application id/version and an upload
keystore. Those signing values should be added later as GitHub Actions secrets rather
than committed to the repository.

Current provisional application id: `com.greenpower2669.spacefortressvs`.
