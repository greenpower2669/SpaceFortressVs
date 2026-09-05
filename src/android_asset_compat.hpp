#ifndef SPACEFORTRESS_ANDROID_ASSET_COMPAT_HPP
#define SPACEFORTRESS_ANDROID_ASSET_COMPAT_HPP

#ifdef __ANDROID__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <android/log.h>

// The historical game uses paths such as "./resources/assets/pict/so.png".
// Android APK assets are rooted at assets/, and SDL's Android RWops fallback
// expects an asset-relative path. Keep the old source untouched and normalize
// only at the Android boundary.
static const char *SpaceFortress_AssetPath(const char *path)
{
    if (!path) return path;
    while (path[0] == '.' && path[1] == '/') path += 2;
    return path;
}

static SDL_RWops *SpaceFortress_OpenAsset(const char *path)
{
    const char *normalized = SpaceFortress_AssetPath(path);
    SDL_RWops *rw = SDL_RWFromFile(normalized, "rb");
    if (!rw) {
        __android_log_print(ANDROID_LOG_ERROR, "SpaceFortress",
                            "Asset open failed: %s -> %s : %s",
                            path ? path : "(null)",
                            normalized ? normalized : "(null)",
                            SDL_GetError());
    }
    return rw;
}

static SDL_Texture *SpaceFortress_IMG_LoadTexture(SDL_Renderer *renderer,
                                                   const char *path)
{
    SDL_RWops *rw = SpaceFortress_OpenAsset(path);
    if (!rw) return NULL;

    SDL_Texture *texture = IMG_LoadTexture_RW(renderer, rw, 1);
    if (!texture) {
        __android_log_print(ANDROID_LOG_ERROR, "SpaceFortress",
                            "Texture decode failed: %s : %s",
                            SpaceFortress_AssetPath(path), IMG_GetError());
    }
    return texture;
}

static Mix_Chunk *SpaceFortress_Mix_LoadWAV(const char *path)
{
    SDL_RWops *rw = SpaceFortress_OpenAsset(path);
    if (!rw) return NULL;

    Mix_Chunk *chunk = Mix_LoadWAV_RW(rw, 1);
    if (!chunk) {
        __android_log_print(ANDROID_LOG_ERROR, "SpaceFortress",
                            "Sound decode failed: %s : %s",
                            SpaceFortress_AssetPath(path), Mix_GetError());
    }
    return chunk;
}

static Mix_Music *SpaceFortress_Mix_LoadMUS(const char *path)
{
    SDL_RWops *rw = SpaceFortress_OpenAsset(path);
    if (!rw) return NULL;

    Mix_Music *music = Mix_LoadMUS_RW(rw, 1);
    if (!music) {
        __android_log_print(ANDROID_LOG_ERROR, "SpaceFortress",
                            "Music decode failed: %s : %s",
                            SpaceFortress_AssetPath(path), Mix_GetError());
    }
    return music;
}

// Mix_LoadWAV is a convenience macro in SDL_mixer, so replace it explicitly.
#ifdef Mix_LoadWAV
#undef Mix_LoadWAV
#endif

#define IMG_LoadTexture SpaceFortress_IMG_LoadTexture
#define Mix_LoadWAV SpaceFortress_Mix_LoadWAV
#define Mix_LoadMUS SpaceFortress_Mix_LoadMUS

#endif // __ANDROID__

#endif // SPACEFORTRESS_ANDROID_ASSET_COMPAT_HPP
