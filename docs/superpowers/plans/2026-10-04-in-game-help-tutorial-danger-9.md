# In-Game Help, Tutorial and Danger 9 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a complete in-game help centre and isolated playable tutorial, expand danger to nine profiles with `FIN DU MONDE` as the default, and make classic adverse AI fire use danger-scaled damage/cadence/speed and predictive straight-line aiming while preserving all 1.4.0 kinetic and save invariants.

**Architecture:** Keep three responsibilities separate: `boss_danger.hpp` owns one immutable nine-profile danger model; a pure ballistic helper computes launch-time prediction without touching gameplay state; help/tutorial each own their own UI/sandbox state. Classic integration stays in the tactical/generated-Android compatibility layers so `src/main.cpp` remains byte-identical. Campaign consumes the same danger profile only on non-kinetic hostile paths.

**Tech Stack:** C++17, SDL2, SDL2_image, SDL2_mixer, existing native SDL/UBSan regression harness, Python integration guards, CMake-generated Android compatibility source, Gradle Android APK/AAB build.

**Spec:** `docs/superpowers/specs/2026-10-04-in-game-help-tutorial-danger-9-design.md`

## Global Constraints

- Work only on `feature/in-game-help-tutorial-danger-9` until Fab validates the phone build.
- Do not merge `main` and do not publish a release without a new explicit Fab authorization.
- Keep `src/main.cpp` byte-identical to blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- Every behavior change is TDD: write focused regression, run and observe RED for the intended reason, implement minimally, rerun GREEN, then run relevant neighboring regressions.
- Danger never modifies asteroids, kinetic shield resolution, explicit boss kinetic charge, or kinetic balance v3.
- Red dust remains visual-only; white dust remains energy-first then hull healing; `nrj=0` remains full and `nrj=50` exhausted.
- Ordinary shots may be aimed predictively only at creation. Their velocity must never follow the target after launch. Existing explicitly guided missiles may keep their historical guidance.
- Danger must never alter a human player's ordinary shot damage, speed, cadence or aim.
- The help/tutorial must use large touch targets and high-contrast bitmap text and must remain usable in portrait and landscape.
- Opening help from a live game freezes real gameplay time/state. Help and tutorial touches must never leak to the historical controller.
- Tutorial state must not write any live ship/entity list, campaign save, Hall of Fame, statistics or progression.
- Synchronize `brain.md`, `brainmap.md`, `debughistorical.md`, `todo.md` and append the active contract/status to `ordres-de-mission.md` before final build verification.

## Review Focus

1. **Touch isolation:** open help while one or more fingers are active, navigate, close it, and prove no stale `FINGERUP`, shot, surge or movement reaches gameplay.
2. **Ballistic edge cases:** zero projectile speed, impossible interception, teleports/extreme target velocity and out-of-range danger index must stay finite and bounded.
3. **Straight-shot invariant:** move the target after an ordinary AI projectile is created and prove its velocity vector is unchanged; only explicit missiles may guide.
4. **Kinetic isolation:** compare asteroid and explicit boss-charge results at danger index 0 and 8 and prove identical kinetic output.
5. **UI/render lifecycle:** portrait/landscape/small viewport, renderer recreation and missing help asset fallback must stay readable and crash-free.

---

### Task 1: Nine-profile danger source of truth

**Files:**
- Modify: `src/boss_danger.hpp`
- Create: `tests/danger_profile_regressions.cpp`
- Modify: `scripts/test-regressions.sh`

**Interfaces:**

```cpp
struct SfDangerProfile {
    const char *name;
    float damageMultiplier;
    float cadenceMultiplier;
    float projectileSpeedMultiplier;
    float leadFraction;
    float maxAngularErrorDegrees;
};

static const SfDangerProfile &sfBossDangerProfile();
static const SfDangerProfile &sfBossDangerProfileAt(int index);
static float sfBossDangerMultiplier();
static float sfBossDangerCadenceMultiplier();
static float sfBossDangerProjectileSpeedMultiplier();
static float sfBossDangerLeadFraction();
static float sfBossDangerMaxAngularErrorDegrees();
```

- [ ] Add a focused test asserting exactly nine profiles, exact names/order and the exact values from the approved spec.
- [ ] Assert default index is `8`, `sfBossDangerNext()` wraps `8 -> 0`, and `sfBossDangerAdjust()` clamps to `0..8`.
- [ ] Assert corrupted negative/oversized index normalizes safely before indexing.
- [ ] Run the focused test and confirm RED against the current five-level implementation.
- [ ] Replace parallel 5-element arrays with one `constexpr` profile table while preserving `sfBossDangerMultiplier()` and `sfBossDangerName()` compatibility accessors.
- [ ] Set default to `FIN DU MONDE`, index 8.
- [ ] Rerun focused test GREEN and then `python3 tests/test_balance_v3_integration.py`; update the integration guard to expect all nine names/default without exposing coefficients on HOME.
- [ ] Commit the task with its tests.

### Task 2: Pure launch-time danger ballistics

**Files:**
- Create: `src/danger_ballistics.hpp`
- Create: `tests/danger_ballistics_regressions.cpp`
- Modify: `scripts/test-regressions.sh`

**Interfaces:**

```cpp
struct SfDangerAimInput {
    float originX, originY;
    float targetX, targetY;
    float targetVx, targetVy;
    float projectileSpeed;
    float horizon;
};
struct SfDangerAimPoint { float x, y; };

static SfDangerAimPoint sfDangerAim(const SfDangerAimInput &input,
                                    const SfDangerProfile &profile,
                                    std::uint32_t shotSeed);
```

- [ ] Test a stationary target: aim stays on target apart from the profile's bounded launch error.
- [ ] Test a lateral moving target: predicted X leads in the motion direction.
- [ ] For identical geometry/seed, assert lead magnitude increases monotonically from profile 0 to 8.
- [ ] Assert launch error is deterministic for the same seed and never exceeds `maxAngularErrorDegrees`.
- [ ] Assert zero speed, impossible intercept and extreme finite velocity return finite coordinates and fall back safely toward current target.
- [ ] Run focused test RED because helper does not exist.
- [ ] Implement the first positive quadratic intercept, clamp to requested horizon, blend current target→intercept using `leadFraction`, then rotate the launch ray by deterministic bounded signed error.
- [ ] Rerun focused test GREEN.
- [ ] Commit the task.

### Task 3: Classic adverse AI integrates danger while ordinary fire stays straight

**Files:**
- Modify: `src/tactical_runtime.hpp` around `sfFireMain`, `sfThinkPilot`, `sfUpdatePilot` and projectile helpers.
- Modify: `scripts/prepare-legacy-source.cmake` only at generated Android damage call sites.
- Modify: `tests/tactics_regressions.hpp`
- Modify: `tests/feedback_regressions.hpp` if needed for impact assertions.
- Modify: `scripts/test-regressions.sh`

**Interfaces:**

```cpp
static bool sfClassicAdverseAiShot(const sprite *shot);
static float sfClassicAiShotSpeed(float heat);
static float sfClassicAiCooldown(float historicalCooldown);
static float sfClassicIncomingShotDamage(const sprite *shot, float baseDamage);
static tupl sfClassicAiAim(tupl origin, const SfVelocityGhost &target,
                           float projectileSpeed, std::uint32_t shotSeed);
```

- [ ] RED: same heat at danger 0 vs 8 gives ordinary AI launch-speed ratio `1.80`, while owner-1 human launch speed is unchanged.
- [ ] RED: historical AI cooldown divided by profile cadence gives ratio `2.50` between index 0 and 8 for the same historical cooldown.
- [ ] RED: moving human target produces stronger lead at higher danger; stationary target remains approximately direct.
- [ ] RED: after creating an ordinary AI shot, move/accelerate target and call `sfAdvanceProjectile`; assert `shotVelocityX/Y` remain exactly constant.
- [ ] RED: existing guided missile test still proves guidance can change only the missile vector.
- [ ] RED: `sfClassicIncomingShotDamage` multiplies only adverse owner-0 AI projectiles in `SF_DUEL_AI`; owner-1 human shots and duel-local shots remain unchanged.
- [ ] Implement AI-only speed scaling before ordinary shot launch. Do not apply profile speed to guided missile acceleration, because the approved speed factor is for ordinary projectiles.
- [ ] Replace attack-mode `sfPilot.aim` with `sfClassicAiAim`; leave asteroid mining/raid targeting logic on its existing prediction model.
- [ ] Divide the post-fire AI cooldown by `sfBossDangerCadenceMultiplier()`; preserve all existing energy/heat conditions and no-human-autofire rules.
- [ ] In `prepare-legacy-source.cmake`, wrap normal/missile incoming damage with `sfClassicIncomingShotDamage(e, baseDamage)` where projectile `e` is in scope. Do not touch generated asteroid damage replacement.
- [ ] Rerun tactics/feedback focused tests GREEN, then syntax-generate the Android compatibility source and verify it compiles.
- [ ] Assert `src/main.cpp` blob is still `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- [ ] Commit the task.

### Task 4: Coop/campaign consumes cadence, speed and lead without contaminating kinetics

**Files:**
- Modify: `src/campaign_runtime.hpp` around `sfCoopPattern`, `sfCoopTick`, aimed hostile patterns and existing `sfCoopHurt`.
- Modify: `tests/campaign_regressions.hpp`
- Modify: `tests/difficulty_regressions.hpp`

**Behavior:**
- Hostile non-kinetic projectile launch speed = historical speed × danger projectile-speed factor.
- Boss attack interval = historical interval / danger cadence factor.
- Aimed patterns use the same pure danger aim helper; geometric radial/spiral patterns retain their angles.
- Existing `sfCoopHurt()` and ordinary contact keep the damage multiplier exactly once.
- Asteroid and explicit boss-charge kinetic paths remain untouched.

- [ ] RED: same boss/pattern at danger 0 vs 8 shows hostile ordinary projectile speed ratio `1.80`.
- [ ] RED: same boss/phase attack reset at 0 vs 8 shows interval ratio `2.50`.
- [ ] RED: aimed pattern on moving ship leads farther at danger 8 than danger 0, then emitted ordinary shot vector remains fixed after launch.
- [ ] RED: non-kinetic damage uses ×1 at index 0 and ×40 at index 8 exactly once.
- [ ] RED: run identical asteroid impact and identical explicit kinetic boss charge at danger 0/8 and assert identical raw/residual kinetic outcomes.
- [ ] Implement speed/cadence/lead changes only in applicable non-kinetic paths.
- [ ] Rerun campaign/difficulty/kinetic regressions GREEN.
- [ ] Commit the task.

### Task 5: Help-centre state, text content and navigation

**Files:**
- Create: `src/help_runtime.hpp`
- Modify: `src/start_ui.hpp` minimally for screen IDs, forward callbacks and HOME entry.
- Modify: `src/th2.h` to include the new runtime in an order visible to the final input/presentation bridge.
- Create: `tests/help_regressions.hpp`
- Modify: `tests/regressions.cpp`

**Interfaces:**

```cpp
enum class SfHelpMode { Menu, Quick, Detailed, Animated };
struct SfHelpState {
    SfHelpMode mode;
    int page;
    int returnScreen;
    bool openedFromGame;
};
inline SfHelpState sfHelp;

static void sfHelpOpen(int returnScreen);
static void sfHelpClose();
static bool sfHelpHandleEvent(SDL_Event *event);
static void sfHelpDraw(SDL_Renderer *renderer);
static bool sfHelpOwnsScreen(int screen);
```

- [ ] RED: HOME `?` opens Help Menu with four entries RAPIDE/DÉTAILLÉ/ANIMÉ/TUTORIEL.
- [ ] RED: Quick has the eight approved topics; Detailed contains all approved rule/HUD/energy/kinetic/boss/strategy topics.
- [ ] RED: page previous/next clamps correctly; BACK from a page returns to centre, BACK from centre returns to origin screen.
- [ ] RED: help text never exposes `×40` or other numeric danger coefficients to the HOME/help player-facing selector explanation; it describes named danger progression instead.
- [ ] Implement content as static page descriptors rather than embedding another giant conditional in `start_ui.hpp`.
- [ ] Keep all touch targets normalized/large enough for portrait and landscape.
- [ ] Rerun help navigation tests GREEN.
- [ ] Commit the task.

### Task 6: Animated help reuses real game art with procedural diagrams

**Files:**
- Extend: `src/help_runtime.hpp` or split to `src/help_visuals.hpp` if visual code would make the state/content file hard to reason about.
- Modify: `src/remaster_lifecycle_fix.hpp` to clear help texture references before renderer destruction.
- Modify: `tests/help_regressions.hpp`

**Interfaces:**

```cpp
struct SfHelpTextures {
    SDL_Renderer *renderer;
    SDL_Texture *orange;
    SDL_Texture *blue;
    SDL_Texture *asteroid;
    SDL_Texture *missile;
    SDL_Texture *bosses;
};
static SfHelpTextures &sfHelpTextures(SDL_Renderer *renderer);
static void sfHelpForgetRenderer(SDL_Renderer *renderer);
static void sfHelpDrawAnimatedScene(SDL_Renderer *renderer, int page, float seconds);
```

- [ ] RED render tests on software renderer in 709×1536 and 1536×709: each animated topic produces visible non-background pixels and stable navigation controls.
- [ ] RED missing-texture/fallback test: force one optional texture null and prove SDL primitives/text still render without crash.
- [ ] RED renderer recreation: create/destroy/recreate repeatedly and prove help cache no longer owns references to the old renderer.
- [ ] Implement scenes for predictive straight shot, wave centre→outside, white dust energy→hull, 2 s surge states, and direct-vs-predictive dodge.
- [ ] Use only existing repository art and SDL primitives; add no video asset.
- [ ] Rerun help render/lifecycle tests GREEN.
- [ ] Commit the task.

### Task 7: Isolated playable tutorial sandbox

**Files:**
- Create: `src/tutorial_runtime.hpp`
- Modify: `src/help_runtime.hpp` to launch modules / `TOUT FAIRE`.
- Modify: `src/th2.h` include order as needed.
- Create: `tests/tutorial_regressions.hpp`
- Modify: `tests/regressions.cpp`

**Interfaces:**

```cpp
enum class SfTutorialModule {
    Movement, Shooting, EnergyDust, HullShield,
    Kinetic, Surge, Dodge, Lead, All
};
struct SfTutorialState {
    SfTutorialModule module;
    int step;
    float time;
    bool complete;
    // only tutorial-local ship/shot/rock/dust state
};
inline SfTutorialState sfTutorial;

static void sfTutorialStart(SfTutorialModule module);
static void sfTutorialReset();
static bool sfTutorialHandleEvent(SDL_Event *event);
static void sfTutorialTick(float dt);
static void sfTutorialDraw(SDL_Renderer *renderer);
static bool sfTutorialComplete();
```

- [ ] RED: each of eight modules starts independently and advances only when its intended player action occurs.
- [ ] RED: `All` advances through all modules in canonical order and completes.
- [ ] RED: surge exercise distinguishes <2.00 s cancel, 2.00 s ready/vulnerable state and charged release/purge demonstration.
- [ ] RED isolation snapshot: save real ship values, `sa1`, projectile/dust list sizes, `sfCampaignSave` fields and Hall of Fame count; run tutorial events/ticks; assert every live value unchanged afterward.
- [ ] Implement tutorial-local structs; do not store pointers to `Spritej1/2`, live asteroid lists, `sfCoop` or mutable campaign save.
- [ ] Reuse pure math/constants where safe, but duplicate presentation state rather than borrowing mutable gameplay objects.
- [ ] Rerun tutorial + save/campaign regressions GREEN.
- [ ] Commit the task.

### Task 8: In-game `?`, complete pause and touch-safe resume

**Files:**
- Modify: `src/start_ui.hpp` to draw a small high-contrast gameplay `?` button before presentation.
- Modify: `src/remaster_ai_fix.hpp` to intercept that button before gears/gameplay touch routing.
- Modify: `src/help_runtime.hpp` for game-return context.
- Modify: `src/campaign_runtime.hpp` only if a dedicated pause/resume helper is needed without resetting combat state.
- Modify: `tests/help_regressions.hpp`
- Modify: `tests/regressions.cpp`

**Interfaces:**

```cpp
static SDL_FRect sfHelpGameplayButtonRect(int width, int height);
static bool sfHelpGameplayButtonHit(float normalizedX, float normalizedY);
static void sfHelpSuspendLiveGame();
static void sfHelpResumeLiveGame();
```

**Placement:** use a normalized top-right help zone approximately `x=.86..96, y=.07..14`, adjusted by actual viewport tests so it does not overlap coop PAUSE, HOME danger, gear hit zones or critical HUD labels.

- [ ] RED: opening help from active duel stores return=GAME, requests non-game help screen and freezes `sfSceneSeconds`, projectile positions, pilot cooldown and asteroid positions across repeated presentation/tick opportunities.
- [ ] RED: opening from active coop freezes `sfCoop.time`, attack timer, shot positions and world resources. Active control fingers are neutralized and no cooldown advances.
- [ ] RED: a finger that opens help remains consumed through MOTION/UP; closing help does not fire a projectile or trigger a surge.
- [ ] RED: close returns to GAME without `sfFixLaunchPending`, match reset or campaign restart; next normal dt advances exactly once.
- [ ] RED: BACK hierarchy is page→help centre→original GAME/HOME.
- [ ] Implement the button drawing/hit test and event routing before gameplay gear/touch logic.
- [ ] For coop, preserve combat state while clearing active input ownership. If phase must become `Paused`, store/restore only the prior phase; do not reset world, timers or progression.
- [ ] Rerun input/help/no-autofire/campaign pause tests GREEN.
- [ ] Commit the task.

### Task 9: Integration guards, living memories and fresh Android artifact

**Files:**
- Modify: `tests/regressions.cpp` to include/call new help/tutorial tests.
- Modify: `scripts/test-regressions.sh` to compile/run danger-profile and danger-ballistics focused tests before full SDL suite.
- Modify: `tests/test_balance_v3_integration.py` and/or add `tests/test_help_danger_integration.py` for architecture wiring guards.
- Modify: `brain.md`
- Modify: `brainmap.md`
- Modify: `debughistorical.md`
- Modify: `todo.md`
- Modify: `ordres-de-mission.md`
- Modify: `.github/workflows/android-build.yml` only if necessary to allow this feature branch to run the normal Android workflow; do not alter packaging/release semantics.

- [ ] Add integration guards proving all nine names/default, help/tutorial headers are wired, classic generated damage uses the danger helper, and no danger helper appears in kinetic asteroid/charge paths.
- [ ] Run targeted danger profile test GREEN.
- [ ] Run targeted ballistic test GREEN.
- [ ] Run `bash scripts/test-regressions.sh` and require the entire historical/native/generated-source suite GREEN.
- [ ] Verify `git hash-object src/main.cpp` is exactly `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- [ ] Synchronize all four living memories and append this mission/result to `ordres-de-mission.md`, recording exact focused/full test evidence.
- [ ] Trigger a fresh Android workflow from the exact final product SHA (workflow_dispatch if branch is not in push filters).
- [ ] Verify regression step, `assembleDebug`, `bundleRelease`, release-file naming/checksums and uploaded APK/AAB artifact all GREEN.
- [ ] Record exact code SHA, workflow run/job IDs, artifact ID/name and APK SHA-256 in the living memories.
- [ ] Stop for Fab phone validation. Do not merge main and do not publish a release.

## Definition of Done

The implementation is ready to hand to Fab only when all nine tasks are GREEN, `src/main.cpp` retains its historical blob, the new APK/AAB comes from the exact documented SHA, help/tutor input cannot leak into gameplay, tutorial isolation is proven, ordinary classic AI shots stay straight after launch, and danger index 0 vs 8 changes only the approved non-kinetic difficulty dimensions.