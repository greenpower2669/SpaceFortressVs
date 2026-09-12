#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <string>

// Load native SDL's platform definitions first. Only game shims take their
// Android path; rendering and image decoding still use the real SDL libraries.
#define __ANDROID__ 1
#include "t.hpp"
#include "th2.h"

bool setgui = true, setia = false, sdlstarted = true;
float k0 = 1;
float tw = 780, th = 1680;
int tirj1 = 0, tirj2 = 0, incra1 = 0;
std::list<sprite*> sa1;
sprite *Spritej1 = new sprite, *Spritej2 = new sprite;
sprite *loosej1 = new sprite, *loosej2 = new sprite;
sprite *rouage1 = new sprite, *rouage2 = new sprite, *Suiveur = new sprite;
enti *iago = new enti, *iago1 = new enti, *iacalc = new enti, *iatake = new enti;

static SDL_Event finger(Uint32 type, SDL_FingerID id, float x, float y)
{
    SDL_Event e{};
    e.tfinger.type = type; e.tfinger.fingerId = id;
    e.tfinger.x = x; e.tfinger.y = y;
    return e;
}

static void testVectors()
{
    vecteurs v;
    v.setvi();
    assert(std::isfinite(v.angle) && v.force == 0 && v.vxt == 0 && v.vyt == 0);
    v.vx = 3; v.vy = 4; v.setvi();
    assert(std::abs(v.force - 5) < 0.0001f);
    assert(std::abs(v.angle - 53.1301f) < 0.001f);
    v.vx = 0; v.vy = 0; v.setvi();
    assert(std::isfinite(v.angle) && v.force == 0);
    vecteurs distinct(3, 4);
    assert(distinct.vx == 3 && distinct.vy == 4);
    assert(std::isfinite(conv360(1.0000001f, 0)));
    assert(std::isfinite(conv360(0, -1.0000001f)));
    std::puts("PASS: stationary, moving and rounded vector inputs stay finite");
}

static void testLegacyCrashes()
{
    for (int bound : {0, -1, 1, 10, 50, 500}) {
        for (int attempt = 0; attempt < 128; ++attempt) {
            const int choice = SpaceFortressRandomBelow(bound);
            assert(choice >= 0 && choice < (bound > 0 ? bound : 1));
        }
    }
    std::list<parts*> particles;
    for (int index = 0; index < 6; ++index) {
        auto *particle = new parts(0, 0);
        particle->pv = (index == 0 || index == 2 || index == 5) ? 0 : 100;
        particles.push_back(particle);
    }
    SpaceFortressPruneParticles(particles, 1000);
    assert(particles.size() == 3);
    for (auto *particle : particles) assert(particle->pv == 100);
    SpaceFortressPruneParticles(particles, 2);
    assert(particles.size() == 2);
    for (auto *particle : particles) particle->pv = 0;
    SpaceFortressPruneParticles(particles, 2);
    assert(particles.empty());
    SpaceFortressPruneParticles(particles, 2);
    std::puts("PASS: zero-energy IA shots and removal of first/middle/last/all particles");
}

static void testInput()
{
    sfFixRequestedScreen.store(SF_UI_HOME);
    sfFixConsumedFingers.clear();
    auto first = finger(SDL_FINGERDOWN, 11, .5f, .60f);
    sfFixHandleEvent(&first);
    auto second = finger(SDL_FINGERDOWN, 12, .5f, .30f);
    sfFixHandleEvent(&second);
    sfFixApplyUiRequests();
    assert(sfUiScreen == SF_UI_GAME && !setgui);
    for (auto id : {11, 12}) {
        auto motion = finger(SDL_FINGERMOTION, id, .5f, .8f);
        sfFixHandleEvent(&motion); assert(motion.type == SDL_USEREVENT);
        auto up = finger(SDL_FINGERUP, id, .5f, .8f);
        sfFixHandleEvent(&up); assert(up.type == SDL_USEREVENT);
    }
    assert(sfFixConsumedFingers.empty());
    SDL_Event back{}; back.type = SDL_KEYDOWN; back.key.keysym.sym = SDLK_AC_BACK;
    Spritej1->ctrl = Spritej2->ctrl = true;
    sfFixHandleEvent(&back); sfFixApplyUiRequests();
    assert(back.type == SDL_USEREVENT && sfUiScreen == SF_UI_HOME && setgui);
    assert(!Spritej1->ctrl && !Spritej2->ctrl);
    // Returning immediately after PLAY must cancel its deferred launch.
    auto launch = finger(SDL_FINGERDOWN, 13, .5f, .60f);
    sfFixHandleEvent(&launch);
    back.type = SDL_KEYDOWN; sfFixHandleEvent(&back); sfFixApplyUiRequests();
    assert(sfUiScreen == SF_UI_HOME);
    sfFixRequestedScreen.store(SF_UI_GAME);
    rouage1->x = .2f * tw; rouage1->y = .2f * th * 1.09f;
    rouage2->x = .8f * tw; rouage2->y = .8f * th * 1.09f;
    rouage1->w = rouage1->h = rouage2->w = rouage2->h = 40;
    auto gear = finger(SDL_FINGERDOWN, 21, .2f, .2f);
    sfFixHandleEvent(&gear); assert(sfFixGear1Down);
    auto up = finger(SDL_FINGERUP, 21, .2f, .2f);
    sfFixHandleEvent(&up); assert(!sfFixGear1Down && up.type == SDL_USEREVENT);
    gear = finger(SDL_FINGERDOWN, 21, .2f, .2f); sfFixHandleEvent(&gear);
    SDL_Event paused{}; paused.type = SDL_APP_DIDENTERBACKGROUND;
    Spritej2->ctrl = true; Spritej2->id = 42; Spritej2->vx = 7;
    sfFixHandleEvent(&paused);
    sfFixApplyUiRequests();
    assert(!sfFixGear1Down && sfFixConsumedFingers.empty());
    assert(!Spritej2->ctrl && Spritej2->id == 100 && Spritej2->vx == 0);
    auto other = finger(SDL_FINGERDOWN, 22, .8f, .8f);
    sfFixHandleEvent(&other); assert(sfFixRequestedScreen.load() == SF_UI_GAME);
    gear = finger(SDL_FINGERDOWN, 23, .2f, .2f); sfFixHandleEvent(&gear);
    assert(sfFixRequestedScreen.load() == SF_UI_HOME);
    std::puts("PASS: Android Back, deferred launch, multitouch and gear cancellation");
}

static void testTextures()
{
    auto *surface = SDL_CreateRGBSurfaceWithFormat(0, 256, 256, 32, SDL_PIXELFORMAT_RGBA32);
    auto *renderer = SDL_CreateSoftwareRenderer(surface);
    auto *otherSurface = SDL_CreateRGBSurfaceWithFormat(0, 32, 32, 32, SDL_PIXELFORMAT_RGBA32);
    auto *otherRenderer = SDL_CreateSoftwareRenderer(otherSurface);
    assert(surface && renderer && otherSurface && otherRenderer);
    auto *otherSun = IMG_LoadTexture(otherRenderer, "resources/assets/pict/suno.png");
    assert(otherSun);
    for (int cycle = 0; cycle < 12; ++cycle) {
        auto *sun = IMG_LoadTexture(renderer, "./resources/assets/pict/suno.png");
        assert(sun && SpaceFortress_IsSunTexture(sun));
        auto *jupiter = IMG_LoadTexture(renderer, "./resources/assets/pict/jupsoeur4.png");
        assert(jupiter && SpaceFortress_IsPlanetTexture(jupiter));
        auto *gear = IMG_LoadTexture(renderer, "resources/assets/pict/rouage.png");
        assert(gear);
        sfUiScreen = SF_UI_GAME;
        rouage1->x = 40; rouage1->y = 40; rouage2->x = 200; rouage2->y = 200;
        SDL_Rect dst{20, 20, 40, 40};
        assert(SDL_RenderCopy(renderer, gear, nullptr, &dst) == 0);
        sfRmEnsureTextures(renderer);
        assert(sfFinalGearOrangeTexture && sfRmMuzzleStrip);
        SDL_DestroyRenderer(renderer);
        assert(!SpaceFortress_TextureIn(SpaceFortressSunTextures, sun));
        assert(!SpaceFortress_TextureIn(SpaceFortressPlanetTextures, jupiter));
        assert(!sfFinalGearOrangeTexture && !sfRmMuzzleStrip);
        assert(SpaceFortress_IsSunTexture(otherSun));
        renderer = SDL_CreateSoftwareRenderer(surface); assert(renderer);
    }
    // Corrupt a mapped file only inside the disposable assembled test directory.
    const char *name = "resources/assets/pict/remaster/heart_blue.png";
    std::ifstream input(name, std::ios::binary);
    std::string bytes((std::istreambuf_iterator<char>(input)), {}); input.close();
    { std::ofstream damaged(name, std::ios::binary); damaged << "broken PNG"; }
    auto *fallback = IMG_LoadTexture(renderer, "./resources/assets/pict/coeurbl.png");
    assert(fallback);
    SDL_DestroyTexture(fallback);
    { std::ofstream restore(name, std::ios::binary); restore.write(bytes.data(), bytes.size()); }
    SDL_DestroyRenderer(renderer); SDL_DestroyRenderer(otherRenderer);
    SDL_FreeSurface(surface); SDL_FreeSurface(otherSurface);
    assert(SpaceFortressSunTextures.empty());
    assert(SpaceFortressPlanetTextures.empty());
    std::puts("PASS: 12 renderer recreations, isolated texture ownership and corrupt-image fallback");
}

static void testScenicRendering()
{
    const int oldW = WIDTH, oldH = HEIGHT;
    const char *paths[] = {"resources/assets/pict/suno.png",
                          "resources/assets/pict/jupsoeur4.png"};
    const SDL_Point screens[] = {{720, 1560}, {1080, 2340}, {640, 360}};
    sfUiScreen = SF_UI_GAME;
    for (const auto &screen : screens) {
        auto *surface = SDL_CreateRGBSurfaceWithFormat(0, screen.x, screen.y, 32,
                                                       SDL_PIXELFORMAT_RGBA32);
        auto *renderer = SDL_CreateSoftwareRenderer(surface);
        assert(surface && renderer);
        // Reproduce the mismatch from main.cpp: world bounds exceed display.
        WIDTH = screen.x * 11 / 10; HEIGHT = screen.y * 11 / 10;
        for (int index = 0; index < 2; ++index) {
            auto *texture = IMG_LoadTexture(renderer, paths[index]);
            assert(texture);
            int w = 0, h = 0;
            SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
            assert(w == 768 && h == 768); // Reject the old cropped thumbnails.
            SDL_ScaleMode scale{}; SDL_BlendMode blend{};
            assert(SDL_GetTextureScaleMode(texture, &scale) == 0);
            assert(SDL_GetTextureBlendMode(texture, &blend) == 0);
            assert(scale == SDL_ScaleModeLinear && blend == SDL_BLENDMODE_BLEND);
            const SDL_Rect positions[] = {
                {-HEIGHT, -HEIGHT, HEIGHT, HEIGHT},
                {HEIGHT / 2, HEIGHT / 2, HEIGHT, HEIGHT}};
            for (const auto &position : positions) {
                SDL_SetRenderDrawColor(renderer, 11, 19, 31, 255);
                SDL_RenderClear(renderer);
                assert(SDL_RenderCopy(renderer, texture, nullptr, &position) == 0);
                std::vector<Uint8> pixels(screen.x * screen.y * 4);
                assert(SDL_RenderReadPixels(renderer, nullptr, SDL_PIXELFORMAT_RGBA32,
                    pixels.data(), screen.x * 4) == 0);
                int minX = screen.x, minY = screen.y, maxX = -1, maxY = -1, count = 0;
                for (int y = 0; y < screen.y; ++y) {
                    for (int x = 0; x < screen.x; ++x) {
                        const auto *p = &pixels[(y * screen.x + x) * 4];
                        if (std::abs(int(p[0]) - 11) + std::abs(int(p[1]) - 19) +
                            std::abs(int(p[2]) - 31) <= 24) continue;
                        minX = std::min(minX, x); maxX = std::max(maxX, x);
                        minY = std::min(minY, y); maxY = std::max(maxY, y); ++count;
                    }
                }
                assert(count > 1000);
                // Test rendered pixels, not only the helper's rectangle: a
                // clipped planet or a square source cannot pass these checks.
                assert(minX > 2 && minY > 2 && maxX < screen.x - 3 && maxY < screen.y - 3);
                const int bw = maxX - minX + 1, bh = maxY - minY + 1;
                assert(std::abs(bw - bh) < std::max(bw, bh) / 10);
                const float coverage = float(count) / (bw * bh);
                assert(coverage > .60f && coverage < .87f);
            }
        }
        SDL_DestroyRenderer(renderer); SDL_FreeSurface(surface);
    }
    WIDTH = oldW; HEIGHT = oldH;
    std::puts("PASS: full round Sun/Jupiter pixels inside portrait and landscape screens");
}

static void writeScenicPreview(const char *path)
{
    if (!path) return;
    const int width = 709, height = 1536;
    auto *surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
    auto *renderer = SDL_CreateSoftwareRenderer(surface);
    assert(surface && renderer);
    sfUiScreen = SF_UI_GAME;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    auto *galaxy = IMG_LoadTexture(renderer, "resources/assets/pict/fond4hlz.png");
    auto *sun = IMG_LoadTexture(renderer, "resources/assets/pict/suno.png");
    auto *jupiter = IMG_LoadTexture(renderer, "resources/assets/pict/jupsoeur4.png");
    assert(galaxy && sun && jupiter);
    SDL_Rect background{0, 0, width * 11 / 10, height * 11 / 10};
    SDL_Rect solar{-330, -145, 840, 840}, jovian{840, 840, 1680, 1680};
    assert(SDL_RenderCopy(renderer, galaxy, nullptr, &background) == 0);
    assert(SDL_RenderCopy(renderer, sun, nullptr, &solar) == 0);
    assert(SDL_RenderCopy(renderer, jupiter, nullptr, &jovian) == 0);
    SDL_RenderFlush(renderer);
    assert(IMG_SavePNG(surface, path) == 0);
    SDL_DestroyRenderer(renderer); SDL_FreeSurface(surface);
}

int main()
{
    assert(SDL_Init(SDL_INIT_TIMER) == 0);
    testVectors(); testLegacyCrashes(); testInput(); testTextures(); testScenicRendering();
    writeScenicPreview(std::getenv("SPACEFORTRESS_SCENIC_PREVIEW"));
    delete Spritej1; delete Spritej2; delete loosej1; delete loosej2;
    delete rouage1; delete rouage2; delete Suiveur;
    delete iago; delete iago1; delete iacalc; delete iatake;
    SDL_Quit();
}
