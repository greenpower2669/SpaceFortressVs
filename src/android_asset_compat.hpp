#ifndef SPACEFORTRESS_ANDROID_ASSET_COMPAT_HPP
#define SPACEFORTRESS_ANDROID_ASSET_COMPAT_HPP

#ifdef __ANDROID__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <android/log.h>
#include <cstring>

// Texture identities used by the final visual layer. Keeping these at the
// Android loading boundary lets us add aura/scale treatment without touching
// the historical renderer or main.cpp.
static SDL_Texture *SpaceFortressSunTexture = NULL;
static SDL_Texture *SpaceFortressPlanetTexture = NULL;

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

    // Restore the historical rich galaxy background unchanged. Scenic bodies
    // stay separate and use the modern round assets.
    if (std::strcmp(path, "resources/assets/pict/suno.png") == 0)
        return "resources/assets/pict/remaster/sun.png";
    if (std::strcmp(path, "resources/assets/pict/sunrcc.png") == 0)
        return "resources/assets/pict/remaster/transparent.png";
    if (std::strcmp(path, "resources/assets/pict/marssoeur3.png") == 0)
        return "resources/assets/pict/remaster/transparent.png";
    if (std::strcmp(path, "resources/assets/pict/jupsoeur4.png") == 0)
        return "resources/assets/pict/remaster/planet.png";

    // Active gameplay asteroids are aa1..aa4.
    if (std::strcmp(path, "resources/assets/pict/aa1.png") == 0)
        return "resources/assets/pict/remaster/asteroid1.png";
    if (std::strcmp(path, "resources/assets/pict/aa2.png") == 0)
        return "resources/assets/pict/remaster/asteroid2.png";
    if (std::strcmp(path, "resources/assets/pict/aa3.png") == 0)
        return "resources/assets/pict/remaster/asteroid3.png";
    if (std::strcmp(path, "resources/assets/pict/aa4.png") == 0)
        return "resources/assets/pict/remaster/asteroid4.png";

    // Dormant background-asteroid slots, kept modern if re-enabled later.
    if (std::strcmp(path, "resources/assets/pict/ast.png") == 0)
        return "resources/assets/pict/remaster/asteroid1.png";
    if (std::strcmp(path, "resources/assets/pict/ast2.png") == 0)
        return "resources/assets/pict/remaster/asteroid2.png";
    if (std::strcmp(path, "resources/assets/pict/ast3.png") == 0)
        return "resources/assets/pict/remaster/asteroid3.png";
    if (std::strcmp(path, "resources/assets/pict/ast4.png") == 0)
        return "resources/assets/pict/remaster/asteroid4.png";
    if (std::strcmp(path, "resources/assets/pict/ast5.png") == 0)
        return "resources/assets/pict/remaster/asteroid2.png";

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
    const char *normalized = SpaceFortress_AssetPath(path);
    SDL_RWops *rw = SpaceFortress_OpenAsset(path);
    if (!rw) return NULL;

    SDL_Texture *texture = IMG_LoadTexture_RW(renderer, rw, 1);
    if (!texture) {
        __android_log_print(ANDROID_LOG_ERROR, "SpaceFortress",
                            "Texture decode failed: %s : %s",
                            normalized ? normalized : "(null)", IMG_GetError());
        return NULL;
    }

    if (normalized && std::strcmp(normalized,
            "resources/assets/pict/remaster/sun.png") == 0)
        SpaceFortressSunTexture = texture;
    if (normalized && std::strcmp(normalized,
            "resources/assets/pict/remaster/planet.png") == 0)
        SpaceFortressPlanetTexture = texture;

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
