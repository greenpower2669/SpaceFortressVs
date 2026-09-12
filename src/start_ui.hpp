#ifndef SPACEFORTRESS_START_UI_HPP
#define SPACEFORTRESS_START_UI_HPP

// Lightweight start/help UI for SpaceFortressVs.
//
// Goal: add a modern entry screen without touching the historical gameplay
// loop or adding SDL_ttf / new image assets. The UI is drawn procedurally with
// SDL2 and hooks only SDL_WaitEvent + SDL_RenderPresent.
//
// The existing "two gears" gesture is intentionally left unchanged.
// The old code still toggles setgui; this shim converts that toggle into a
// return to the new home screen at presentation time.

#include <SDL2/SDL.h>
#include <cmath>
#include <cstring>
#include <cstdint>

// Defined by main.cpp after the includes.
extern bool setgui;
extern bool setia;
extern sprite *Spritej1;
extern sprite *Spritej2;
extern sprite *loosej1;
extern sprite *loosej2;

enum SpaceFortressUiScreen {
    SF_UI_HOME = 0,
    SF_UI_HELP = 1,
    SF_UI_GAME = 2
};

static int sfUiScreen = SF_UI_HOME;
static bool sfUiConsumeFinger = false;
static SDL_FingerID sfUiConsumedFinger = 0;

// -----------------------------------------------------------------------------
// Tiny 5x7 bitmap font. No SDL_ttf dependency and no font asset to package.
// Each row uses the low 5 bits, left to right.
// -----------------------------------------------------------------------------

static const uint8_t *sfUiGlyph(char c)
{
    static const uint8_t BLANK[7] = {0,0,0,0,0,0,0};
    static const uint8_t A[7] = {14,17,17,31,17,17,17};
    static const uint8_t B[7] = {30,17,17,30,17,17,30};
    static const uint8_t C[7] = {14,17,16,16,16,17,14};
    static const uint8_t D[7] = {30,17,17,17,17,17,30};
    static const uint8_t E[7] = {31,16,16,30,16,16,31};
    static const uint8_t F[7] = {31,16,16,30,16,16,16};
    static const uint8_t G[7] = {14,17,16,23,17,17,14};
    static const uint8_t H[7] = {17,17,17,31,17,17,17};
    static const uint8_t I[7] = {31,4,4,4,4,4,31};
    static const uint8_t J[7] = {7,2,2,2,18,18,12};
    static const uint8_t K[7] = {17,18,20,24,20,18,17};
    static const uint8_t L[7] = {16,16,16,16,16,16,31};
    static const uint8_t M[7] = {17,27,21,21,17,17,17};
    static const uint8_t N[7] = {17,25,21,19,17,17,17};
    static const uint8_t O[7] = {14,17,17,17,17,17,14};
    static const uint8_t P[7] = {30,17,17,30,16,16,16};
    static const uint8_t Q[7] = {14,17,17,17,21,18,13};
    static const uint8_t R[7] = {30,17,17,30,20,18,17};
    static const uint8_t S[7] = {15,16,16,14,1,1,30};
    static const uint8_t T[7] = {31,4,4,4,4,4,4};
    static const uint8_t U[7] = {17,17,17,17,17,17,14};
    static const uint8_t V[7] = {17,17,17,17,17,10,4};
    static const uint8_t W[7] = {17,17,17,21,21,21,10};
    static const uint8_t X[7] = {17,17,10,4,10,17,17};
    static const uint8_t Y[7] = {17,17,10,4,4,4,4};
    static const uint8_t Z[7] = {31,1,2,4,8,16,31};

    static const uint8_t N0[7] = {14,17,19,21,25,17,14};
    static const uint8_t N1[7] = {4,12,4,4,4,4,14};
    static const uint8_t N2[7] = {14,17,1,2,4,8,31};
    static const uint8_t N3[7] = {30,1,1,14,1,1,30};
    static const uint8_t N4[7] = {2,6,10,18,31,2,2};
    static const uint8_t N5[7] = {31,16,16,30,1,1,30};
    static const uint8_t N6[7] = {14,16,16,30,17,17,14};
    static const uint8_t N7[7] = {31,1,2,4,8,8,8};
    static const uint8_t N8[7] = {14,17,17,14,17,17,14};
    static const uint8_t N9[7] = {14,17,17,15,1,1,14};

    static const uint8_t QUESTION[7] = {14,17,1,2,4,0,4};
    static const uint8_t COLON[7] = {0,4,4,0,4,4,0};
    static const uint8_t PLUS[7] = {0,4,4,31,4,4,0};
    static const uint8_t MINUS[7] = {0,0,0,31,0,0,0};
    static const uint8_t DOT[7] = {0,0,0,0,0,6,6};

    switch (c) {
        case 'A': return A; case 'B': return B; case 'C': return C;
        case 'D': return D; case 'E': return E; case 'F': return F;
        case 'G': return G; case 'H': return H; case 'I': return I;
        case 'J': return J; case 'K': return K; case 'L': return L;
        case 'M': return M; case 'N': return N; case 'O': return O;
        case 'P': return P; case 'Q': return Q; case 'R': return R;
        case 'S': return S; case 'T': return T; case 'U': return U;
        case 'V': return V; case 'W': return W; case 'X': return X;
        case 'Y': return Y; case 'Z': return Z;
        case '0': return N0; case '1': return N1; case '2': return N2;
        case '3': return N3; case '4': return N4; case '5': return N5;
        case '6': return N6; case '7': return N7; case '8': return N8;
        case '9': return N9;
        case '?': return QUESTION;
        case ':': return COLON;
        case '+': return PLUS;
        case '-': return MINUS;
        case '.': return DOT;
        default: return BLANK;
    }
}

static int sfUiTextWidth(const char *text, int scale)
{
    if (!text) return 0;
    const int n = static_cast<int>(std::strlen(text));
    return n > 0 ? (n * 6 - 1) * scale : 0;
}

static void sfUiText(SDL_Renderer *renderer, int x, int y, const char *text,
                     int scale, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255)
{
    if (!renderer || !text || scale < 1) return;
    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    int cursor = x;
    for (const char *p = text; *p; ++p) {
        const uint8_t *glyph = sfUiGlyph(*p);
        for (int row = 0; row < 7; ++row) {
            for (int col = 0; col < 5; ++col) {
                if (glyph[row] & (1u << (4 - col))) {
                    SDL_Rect px = { cursor + col * scale, y + row * scale,
                                    scale, scale };
                    SDL_RenderFillRect(renderer, &px);
                }
            }
        }
        cursor += 6 * scale;
    }
}

static void sfUiCenteredText(SDL_Renderer *renderer, int width, int y,
                             const char *text, int scale,
                             Uint8 r, Uint8 g, Uint8 b)
{
    sfUiText(renderer, (width - sfUiTextWidth(text, scale)) / 2, y,
             text, scale, r, g, b);
}

static void sfUiPanel(SDL_Renderer *renderer, SDL_Rect rect,
                      Uint8 fr, Uint8 fg, Uint8 fb,
                      Uint8 br, Uint8 bg, Uint8 bb)
{
    SDL_SetRenderDrawColor(renderer, fr, fg, fb, 255);
    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, br, bg, bb, 255);
    for (int i = 0; i < 3; ++i) {
        SDL_Rect o = {rect.x + i, rect.y + i, rect.w - i * 2, rect.h - i * 2};
        if (o.w > 0 && o.h > 0) SDL_RenderDrawRect(renderer, &o);
    }
}

static void sfUiCircle(SDL_Renderer *renderer, int cx, int cy, int radius,
                       Uint8 r, Uint8 g, Uint8 b)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        SDL_RenderDrawPoint(renderer, cx + x, cy + y);
        SDL_RenderDrawPoint(renderer, cx + y, cy + x);
        SDL_RenderDrawPoint(renderer, cx - y, cy + x);
        SDL_RenderDrawPoint(renderer, cx - x, cy + y);
        SDL_RenderDrawPoint(renderer, cx - x, cy - y);
        SDL_RenderDrawPoint(renderer, cx - y, cy - x);
        SDL_RenderDrawPoint(renderer, cx + y, cy - x);
        SDL_RenderDrawPoint(renderer, cx + x, cy - y);

        if (err <= 0) {
            ++y;
            err += 2 * y + 1;
        }
        if (err > 0) {
            --x;
            err -= 2 * x + 1;
        }
    }
}

static void sfUiGear(SDL_Renderer *renderer, int cx, int cy, int radius)
{
    sfUiCircle(renderer, cx, cy, radius, 185, 225, 245);
    sfUiCircle(renderer, cx, cy, radius - 1, 185, 225, 245);
    sfUiCircle(renderer, cx, cy, radius / 3, 185, 225, 245);

    for (int i = 0; i < 8; ++i) {
        const double a = i * 3.14159265358979323846 / 4.0;
        const int x1 = cx + static_cast<int>(std::cos(a) * (radius - 3));
        const int y1 = cy + static_cast<int>(std::sin(a) * (radius - 3));
        const int x2 = cx + static_cast<int>(std::cos(a) * (radius + 8));
        const int y2 = cy + static_cast<int>(std::sin(a) * (radius + 8));
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}

static void sfUiBackground(SDL_Renderer *renderer, int width, int height)
{
    SDL_SetRenderDrawColor(renderer, 3, 8, 20, 255);
    SDL_RenderClear(renderer);

    // Deterministic star field: zero asset loading, stable on every frame.
    uint32_t seed = 0x53F04A91u;
    SDL_SetRenderDrawColor(renderer, 215, 235, 255, 255);
    for (int i = 0; i < 125; ++i) {
        seed = seed * 1664525u + 1013904223u;
        const int x = static_cast<int>(seed % static_cast<uint32_t>(width));
        seed = seed * 1664525u + 1013904223u;
        const int y = static_cast<int>(seed % static_cast<uint32_t>(height));
        SDL_RenderDrawPoint(renderer, x, y);
        if ((i % 13) == 0) SDL_RenderDrawPoint(renderer, x + 1, y);
    }

    // A simple orbital motif.
    const int pcx = width / 2;
    const int pcy = static_cast<int>(height * 0.19f);
    const int pr = width / 10;
    sfUiCircle(renderer, pcx, pcy, pr, 80, 185, 235);
    sfUiCircle(renderer, pcx, pcy, pr - 1, 80, 185, 235);

    SDL_SetRenderDrawColor(renderer, 125, 205, 245, 255);
    SDL_Rect orbit = {pcx - pr * 2, pcy - pr / 2, pr * 4, pr};
    SDL_RenderDrawRect(renderer, &orbit);

    // Moving planet: visual cue for the selectable mode.
    const double t = SDL_GetTicks() * 0.0012;
    const int moonX = pcx + static_cast<int>(std::cos(t) * pr * 2.0);
    const int moonY = pcy + static_cast<int>(std::sin(t) * pr * 0.5);
    sfUiCircle(renderer, moonX, moonY, width / 45, 220, 115, 220);
}

static void sfUiDrawHome(SDL_Renderer *renderer)
{
    int width = 0, height = 0;
    SDL_GetRendererOutputSize(renderer, &width, &height);
    if (width <= 0 || height <= 0) return;

    sfUiBackground(renderer, width, height);

    const int base = (width / 260) > 2 ? (width / 260) : 2;
    const int titleScale = base * 2;
    const int buttonScale = base + 1;

    sfUiCenteredText(renderer, width, static_cast<int>(height * 0.055f),
                     "SPACE FORTRESS", titleScale, 235, 250, 255);
    sfUiCenteredText(renderer, width, static_cast<int>(height * 0.115f),
                     "DUEL ORBITAL", base, 105, 200, 240);

    sfUiCenteredText(renderer, width, static_cast<int>(height * 0.355f),
                     "TOUCHEZ LE MODE POUR LE CHANGER", base,
                     190, 215, 235);

    SDL_Rect mode = {
        static_cast<int>(width * 0.10f), static_cast<int>(height * 0.40f),
        static_cast<int>(width * 0.80f), static_cast<int>(height * 0.105f)
    };
    sfUiPanel(renderer, mode, 7, 18, 42,
              setia ? 225 : 70, setia ? 105 : 200, setia ? 215 : 245);

    const char *modeText = setia ? "MODE : 1 JOUEUR + IA" : "MODE : 2 JOUEURS";
    sfUiCenteredText(renderer, width,
                     mode.y + (mode.h - 7 * buttonScale) / 2,
                     modeText, buttonScale, 238, 248, 255);

    SDL_Rect start = {
        static_cast<int>(width * 0.10f), static_cast<int>(height * 0.56f),
        static_cast<int>(width * 0.80f), static_cast<int>(height * 0.115f)
    };
    sfUiPanel(renderer, start, 5, 39, 36, 80, 235, 185);
    sfUiCenteredText(renderer, width,
                     start.y + (start.h - 7 * buttonScale) / 2,
                     "LANCER LA PARTIE", buttonScale, 240, 255, 250);

    SDL_Rect help = {
        static_cast<int>(width * 0.27f), static_cast<int>(height * 0.735f),
        static_cast<int>(width * 0.46f), static_cast<int>(height * 0.09f)
    };
    sfUiPanel(renderer, help, 14, 18, 46, 185, 195, 240);
    sfUiCenteredText(renderer, width,
                     help.y + (help.h - 7 * buttonScale) / 2,
                     "?  AIDE", buttonScale, 240, 245, 255);

    const int gy = static_cast<int>(height * 0.90f);
    sfUiGear(renderer, static_cast<int>(width * 0.31f), gy, width / 28);
    sfUiGear(renderer, static_cast<int>(width * 0.69f), gy, width / 28);
    sfUiCenteredText(renderer, width, static_cast<int>(height * 0.858f),
                     "EN PARTIE", base, 125, 185, 225);
    sfUiCenteredText(renderer, width, static_cast<int>(height * 0.947f),
                     "DEUX ENGRENAGES = ACCUEIL", base, 205, 225, 240);
}

static void sfUiHelpLine(SDL_Renderer *renderer, int width, int y,
                         const char *number, const char *line1,
                         const char *line2, int scale)
{
    const int left = static_cast<int>(width * 0.08f);
    sfUiText(renderer, left, y, number, scale + 1, 80, 210, 245);
    sfUiText(renderer, left + 10 * scale, y, line1, scale, 230, 240, 250);
    if (line2 && *line2) {
        sfUiText(renderer, left + 10 * scale, y + 10 * scale,
                 line2, scale, 230, 240, 250);
    }
}

static void sfUiDrawHelp(SDL_Renderer *renderer)
{
    int width = 0, height = 0;
    SDL_GetRendererOutputSize(renderer, &width, &height);
    if (width <= 0 || height <= 0) return;

    sfUiBackground(renderer, width, height);

    const int base = (width / 300) > 2 ? (width / 300) : 2;
    const int titleScale = base * 2;

    sfUiCenteredText(renderer, width, static_cast<int>(height * 0.055f),
                     "COMMENT JOUER", titleScale, 235, 250, 255);

    int y = static_cast<int>(height * 0.17f);
    const int step = static_cast<int>(height * 0.115f);
    sfUiHelpLine(renderer, width, y, "1",
                 "CHAQUE JOUEUR CONTROLE", "SA MOITIE DE L ECRAN", base);
    y += step;
    sfUiHelpLine(renderer, width, y, "2",
                 "GLISSEZ POUR DEPLACER", "VOTRE VAISSEAU", base);
    y += step;
    sfUiHelpLine(renderer, width, y, "3",
                 "AVEC UN AUTRE DOIGT", "TAPOTEZ POUR TIRER", base);
    y += step;
    sfUiHelpLine(renderer, width, y, "4",
                 "LE MODE SE CHOISIT", "DEPUIS L ACCUEIL", base);
    y += step;
    sfUiHelpLine(renderer, width, y, "5",
                 "DEUX ENGRENAGES", "RAMENENT A L ACCUEIL", base);

    const int gy = static_cast<int>(height * 0.77f);
    sfUiGear(renderer, static_cast<int>(width * 0.33f), gy, width / 25);
    sfUiGear(renderer, static_cast<int>(width * 0.67f), gy, width / 25);
    sfUiCenteredText(renderer, width, static_cast<int>(height * 0.82f),
                     "GESTE DISCRET POUR GARDER L IMMERSION",
                     base, 165, 205, 230);

    SDL_Rect back = {
        static_cast<int>(width * 0.23f), static_cast<int>(height * 0.885f),
        static_cast<int>(width * 0.54f), static_cast<int>(height * 0.08f)
    };
    sfUiPanel(renderer, back, 5, 39, 36, 80, 235, 185);
    sfUiCenteredText(renderer, width,
                     back.y + (back.h - 7 * (base + 1)) / 2,
                     "RETOUR", base + 1, 240, 255, 250);
}

static void sfUiStartMatch()
{
    // Keep the historical game engine intact; only normalize the two ships'
    // obvious start state so the menu behaves like a real "new match" entry.
    if (Spritej1) {
        Spritej1->x = WIDTH * 0.25f;
        Spritej1->y = HEIGHT * 0.15f;
        Spritej1->pv = 1000;
        Spritej1->nrj = 0;
        Spritej1->ctrl = false;
        Spritej1->id = 100;
    }
    if (Spritej2) {
        Spritej2->x = WIDTH * 0.70f;
        Spritej2->y = HEIGHT * 0.75f;
        Spritej2->pv = 1000;
        Spritej2->nrj = 0;
        Spritej2->ctrl = false;
        Spritej2->id = 100;
    }
    if (loosej1) loosej1->pv = 0;
    if (loosej2) loosej2->pv = 0;

    setgui = false;
    sfUiScreen = SF_UI_GAME;
}

static void sfUiHandleFingerDown(const SDL_TouchFingerEvent &finger)
{
    const float x = finger.x;
    const float y = finger.y;
    (void)x; // full-width buttons for now

    if (sfUiScreen == SF_UI_HOME) {
        if (y >= 0.39f && y <= 0.52f) {
            setia = !setia;
        } else if (y >= 0.545f && y <= 0.69f) {
            sfUiStartMatch();
        } else if (y >= 0.71f && y <= 0.85f) {
            sfUiScreen = SF_UI_HELP;
        }
    } else if (sfUiScreen == SF_UI_HELP) {
        if (y >= 0.86f) sfUiScreen = SF_UI_HOME;
    }
}

// Wrappers are defined BEFORE the macros below, so calls inside these functions
// still target the real SDL functions.
static int SpaceFortressUi_WaitEvent(SDL_Event *event)
{
    const int result = SDL_WaitEvent(event);
    if (!result || !event) return result;

    // Consume the remainder of a touch that began on the menu, even if that
    // touch launched the game. This prevents a stray FINGERUP from reaching
    // the historical controller.
    if (sfUiConsumeFinger &&
        (event->type == SDL_FINGERMOTION || event->type == SDL_FINGERUP) &&
        event->tfinger.fingerId == sfUiConsumedFinger) {
        if (event->type == SDL_FINGERUP) sfUiConsumeFinger = false;
        event->type = SDL_USEREVENT;
        return result;
    }

    if (sfUiScreen != SF_UI_GAME) {
        if (event->type == SDL_FINGERDOWN) {
            sfUiConsumedFinger = event->tfinger.fingerId;
            sfUiConsumeFinger = true;
            sfUiHandleFingerDown(event->tfinger);
            event->type = SDL_USEREVENT;
        } else if (event->type == SDL_FINGERMOTION ||
                   event->type == SDL_FINGERUP) {
            event->type = SDL_USEREVENT;
        } else if (event->type == SDL_KEYDOWN &&
                   (event->key.keysym.sym == SDLK_ESCAPE
#ifdef SDLK_AC_BACK
                    || event->key.keysym.sym == SDLK_AC_BACK
#endif
                   )) {
            if (sfUiScreen == SF_UI_HELP) sfUiScreen = SF_UI_HOME;
            event->type = SDL_USEREVENT;
        }
    } else if (event->type == SDL_KEYDOWN &&
               (event->key.keysym.sym == SDLK_ESCAPE
#ifdef SDLK_AC_BACK
                || event->key.keysym.sym == SDLK_AC_BACK
#endif
               )) {
        // Android/back or Escape: same destination as the hidden gear gesture.
        sfUiScreen = SF_UI_HOME;
        setgui = false;
        event->type = SDL_USEREVENT;
    }

    return result;
}

static void SpaceFortressUi_RenderPresent(SDL_Renderer *renderer)
{
    if (!renderer) return;

    // Existing gear code toggles setgui. We do not change its detection logic;
    // we only reinterpret the successful gesture as "return to home".
    if (sfUiScreen == SF_UI_GAME && setgui) {
        sfUiScreen = SF_UI_HOME;
        setgui = false;
    }

    if (sfUiScreen == SF_UI_HOME) {
        sfUiDrawHome(renderer);
    } else if (sfUiScreen == SF_UI_HELP) {
        sfUiDrawHelp(renderer);
    }

    SDL_RenderPresent(renderer);
}

// Hook only two SDL calls used by the historical main.cpp.
#define SDL_WaitEvent SpaceFortressUi_WaitEvent
#define SDL_RenderPresent SpaceFortressUi_RenderPresent

#endif // SPACEFORTRESS_START_UI_HPP
