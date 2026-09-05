#ifndef SPACEFORTRESS_ANDROID_ASSET_COMPAT_HPP
#define SPACEFORTRESS_ANDROID_ASSET_COMPAT_HPP

#ifdef __ANDROID__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <android/log.h>
#include <cstring>

// Historical code keeps using "./resources/assets/...". Android normalizes the
// prefix here and redirects only compatibility/remaster assets at the boundary.
static const char *SpaceFortress_AssetPath(const char *path)
{
    if (!path) return path;

    while (path[0] == '.' && path[1] == '/') path += 2;

    // Android assets are case-sensitive. The historical source asks for
    // laser.ogg, while the repository contains Laser.ogg.
    if (std::strcmp(path, "resources/assets/sounds/laser.ogg") == 0)
        return "resources/assets/sounds/Laser.ogg";

    // Preserve team identity: player 1 is the warm/red side, player 2 blue.
    if (std::strcmp(path, "resources/assets/pict/so.png") == 0)
        return "resources/assets/pict/remaster/player_orange.png";
    if (std::strcmp(path, "resources/assets/pict/sb.png") == 0)
        return "resources/assets/pict/remaster/player_blue.png";

    // The gameplay background is now rendered live by remaster_runtime.hpp.
    // Keep all former static scenic layers transparent so no painted sun,
    // planet or shooting-star motif can cover the animated space field.
    if (std::strcmp(path, "resources/assets/pict/fond4hlz.png") == 0 ||
        std::strcmp(path, "resources/assets/pict/suno.png") == 0 ||
        std::strcmp(path, "resources/assets/pict/sunrcc.png") == 0 ||
        std::strcmp(path, "resources/assets/pict/marssoeur3.png") == 0 ||
        std::strcmp(path, "resources/assets/pict/jupsoeur4.png") == 0)
        return "resources/assets/pict/remaster/transparent.png";

    if (std::strcmp(path, "resources/assets/pict/rouage.png") == 0)
        return "resources/assets/pict/remaster/gear.png";

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

#ifdef Mix_LoadWAV
#undef Mix_LoadWAV
#endif

#define IMG_LoadTexture SpaceFortress_IMG_LoadTexture
#define Mix_LoadWAV SpaceFortress_Mix_LoadWAV
#define Mix_LoadMUS SpaceFortress_Mix_LoadMUS

#endif // __ANDROID__

#endif // SPACEFORTRESS_ANDROID_ASSET_COMPAT_HPP
