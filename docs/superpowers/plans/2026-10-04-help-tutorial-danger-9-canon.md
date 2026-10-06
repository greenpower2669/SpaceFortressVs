# SpaceFortressVs Help / Tutorial / Danger 9 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a `?` help center on HOME and in-game, three player-selectable help depths plus an isolated step-by-step tutorial, and extend hostile non-kinetic danger damage from 5 to 9 named levels including classic AI shots.

**Architecture:** Keep `src/main.cpp` byte-for-byte historical. Extend the existing danger source of truth, add focused `help_runtime.hpp` and `tutorial_runtime.hpp` components, and route pause/resume/input through the existing Android remaster bridge. Classic hostile damage is applied before the shared shield resolver using projectile ownership/mode metadata already present; campaign/boss damage continues through its existing non-kinetic path.

**Tech Stack:** C++17, SDL2, SDL2_image, SDL2_mixer, existing generated Android compatibility source, native regression harness, CMake/Gradle Android build.

**Spec:** `docs/superpowers/specs/2026-10-04-help-tutorial-danger-9-canon-design.md`

## Global Constraints

- Work only on `feature/help-tutorial-danger-9-canon`, based on `main` SHA `703676aaa3ae16a0ef415802dbb9ea24a229e61c`.
- `src/main.cpp` remains byte-for-byte unchanged unless Fab separately approves an exception.
- Danger names and internal multipliers are exactly: `MOU DU GENOU` x1, `CHILL` x5, `ROCK N ROLL` x10, `DUR A CUIRE` x15, `MACHINE DE GUERRE` x20, `CA VA PIQUER` x25, `SANS PITIE` x30, `ENFER STELLAIRE` x35, `APOCALYPSE` x40.
- Default danger stays `ROCK N ROLL` / index 2. Coefficients are never displayed to players.
- This lot changes danger **damage only**: no danger-driven cadence, projectile speed, accuracy, anticipation or guidance changes.
- Danger never affects asteroids, kinetic charges or other kinetic paths.
- Help formats are `RAPIDE`, `DETAILLE`, `ANIME`; default is `ANIME`.
- `TUTORIEL` is separate from help depth and must not alter real campaign progression, saves, Hall of Fame or statistics.
- Opening help from a live game pauses real gameplay and must resume without resetting the match or leaking touch events.
- Reuse existing game assets and SDL drawings; add no pre-rendered tutorial videos in this lot.
- Preserve 1.4.0 invariants: campaign 200, `nrj=0` full / `nrj=50` exhausted, white dust recharge then heal, red dust visual only, kinetic v3, 2-second surge semantics, centre-to-outside kinetic waves.
- No merge to `main` and no release without a new explicit Fab approval.

## Review Focus

1. **Resume without reset:** opening `?` during duel or campaign then closing it must preserve PV, `nrj`, projectile/asteroid state and encounter state while preventing a stale `FINGERUP` from firing or moving a ship. Covered in Task 4 regression tests.
2. **Small/rotated screens:** HOME help entry, in-game `?`, page navigation and tutorial controls must remain hittable and readable in portrait and landscape. Covered in Tasks 3-5 rendering/input tests.
3. **Wrong owner multiplication:** human shots must never inherit the classic-AI danger multiplier, including local duel and coop modes. Covered in Task 2 ownership/mode matrix tests.
4. **Missing illustration asset:** animated help must degrade to SDL schematic/text without crashing or leaving a stale texture reference after renderer recreation. Covered in Task 3 texture fallback tests.
5. **Tutorial state contamination:** exiting or completing tutorial after arbitrary inputs must leave the real save/progression/Hall state byte-for-byte/logically unchanged. Covered in Task 5 snapshot tests.

---

### Task 1: Extend the danger source of truth to nine levels

**Files:**
- Modify: `src/boss_danger.hpp`
- Create: `tests/danger9_regressions.cpp`
- Modify: `scripts/test-regressions.sh`

**Interfaces:**
- Produces: `SF_BOSS_DANGER_COUNT == 9`, `SF_BOSS_DANGER_LEVELS[9]`, `SF_BOSS_DANGER_NAMES[9]`, default `sfBossDangerIndex == 2`, and existing `sfBossDangerMultiplier()`, `sfBossDangerName()`, `sfBossDangerNext()`, `sfBossDangerAdjust(int)` operating over all nine entries.
- Produces: `static float sfApplyHostileDanger(float rawDamage)` returning `rawDamage * sfBossDangerMultiplier()` for non-kinetic hostile callers.

- [ ] **Step 1: Write the failing danger table regression**

Create `tests/danger9_regressions.cpp` asserting exact count, names, multipliers, default index 2, forward wrap 8->0, bounded adjustment, and `sfApplyHostileDanger(2.0f)` at indexes 0/2/8 equals 2/20/80.

- [ ] **Step 2: Run the focused test and verify it fails**

Run: `g++ -std=c++17 -O1 -I src tests/danger9_regressions.cpp -o /tmp/sf-danger9 && /tmp/sf-danger9`
Expected: FAIL to compile or assert because only five levels exist and the helper is absent.

- [ ] **Step 3: Implement the nine-entry table and helper in `src/boss_danger.hpp`**

Keep the existing public function names so HOME/campaign callers do not need parallel danger state. Replace hard-coded `4/5` clamp/modulo constants with `SF_BOSS_DANGER_COUNT`.

- [ ] **Step 4: Run the focused test and verify it passes**

Run the command from Step 2.
Expected: PASS.

- [ ] **Step 5: Add the focused binary to `scripts/test-regressions.sh` and commit**

Commit message: `feat: extend boss danger to nine levels`

---

### Task 2: Apply danger to classic AI hostile non-kinetic damage only

**Files:**
- Modify: `src/tactical_runtime.hpp`
- Modify: `scripts/prepare-legacy-source.cmake`
- Create: `tests/classic_danger_regressions.cpp`
- Modify: `scripts/test-regressions.sh`

**Interfaces:**
- Consumes: Task 1 `sfApplyHostileDanger(float)` and existing `sprite::shotOwner`, `sfActiveMode`, `SF_DUEL_AI`.
- Produces: `static bool sfClassicAiHostileShot(const sprite *shot,const sprite *victim)`.
- Produces: `static float sfClassicIncomingNonKinetic(const sprite *shot,const sprite *victim,float rawDamage)`; only `SF_DUEL_AI` shots from owner 0 hitting `Spritej2` are multiplied, all other owner/mode/victim combinations return `rawDamage` unchanged.

- [ ] **Step 1: Write the failing classic ownership/mode regression**

Create cases covering `SF_DUEL_AI` owner 0 -> blue human (multiplied), owner 1 -> orange (unchanged), `SF_DUEL_LOCAL` both directions unchanged, coop modes unchanged, and kinetic/asteroid functions untouched by this helper.

- [ ] **Step 2: Run focused test and verify failure**

Compile against `src` plus the existing SDL test include setup used by native regressions.
Expected: FAIL because the classification/helper does not exist.

- [ ] **Step 3: Implement the classification/helper in `src/tactical_runtime.hpp`**

Use projectile ownership and active mode only; do not infer hostility from list names or projectile position.

- [ ] **Step 4: Route generated classic hit damage through the helper**

In `scripts/prepare-legacy-source.cmake`, update only the generated Android replacements for the `Spritej2` normal/missile hull impact path so raw non-kinetic damage is wrapped by `sfClassicIncomingNonKinetic(e,Spritej2,rawDamage)` **before** `sfApplyShieldImpact`. Leave `Spritej1` human/local paths and all asteroid/kinetic replacements unchanged.

- [ ] **Step 5: Verify generated source semantics**

Run: `cmake -DREPO_ROOT="$PWD" -P scripts/prepare-legacy-source.cmake`
Then grep the generated source for the new helper on hostile projectile paths and confirm no danger helper appears in asteroid replacements.

- [ ] **Step 6: Run focused plus legacy-field tests and commit**

Run the focused test, generated Android syntax check, and existing `legacy_field_regressions` path from `scripts/test-regressions.sh`.
Expected: PASS.

Commit message: `feat: scale classic AI hostile damage by danger`

---

### Task 3: Build the help model, content pages and animated renderer

**Files:**
- Create: `src/help_runtime.hpp`
- Modify: `src/start_ui.hpp`
- Create: `tests/help_tutorial_regressions.hpp`
- Modify: `tests/regressions.cpp`

**Interfaces:**
- Produces enum `SfHelpFormat { Quick, Detailed, Animated }` with default `Animated`.
- Produces help state containing current format, page index, hub/page state, return screen and whether the help was opened from a live game.
- Produces: `sfHelpOpen(int returnScreen)`, `sfHelpCloseRequest()`, `sfHelpHandleEvent(SDL_Event*)`, `sfHelpDraw(SDL_Renderer*)`, `sfHelpDrawGameButton(SDL_Renderer*)`, `sfHelpGameButtonRect(int width,int height)`.
- Consumes existing `sfUiText`, `sfUiPanel`, `sfUiCircle`, `sfUiBackground` primitives and existing game textures/assets when available.

- [ ] **Step 1: Write failing help model/navigation tests**

Assert default `Animated`, selection of Quick/Detailed/Animated, bounded page navigation, HOME return target, and that `TUTORIEL` is a separate action rather than a fourth `SfHelpFormat`.

- [ ] **Step 2: Write failing render/fallback tests**

Use SDL software renderers at 360x780 and 780x360. Assert `sfHelpGameButtonRect` stays on-screen with a minimum touch dimension, all three modes render non-background pixels, and an intentionally unavailable optional illustration still renders text/schematic without crashing.

- [ ] **Step 3: Run native regression binary and verify failure**

Run the existing native regression build command from `scripts/test-regressions.sh`.
Expected: compile failure because `help_runtime.hpp` interfaces do not exist.

- [ ] **Step 4: Implement `src/help_runtime.hpp`**

Keep content data-driven (page descriptors + render callbacks) so Quick/Detailed/Animated share facts rather than duplicating contradictory prose. Animated pages use existing vaisseau/boss/asteroid/dust/missile assets opportunistically, with SDL schematic fallback.

- [ ] **Step 5: Replace the old one-page `sfUiDrawHelp` ownership in `src/start_ui.hpp`**

Include the new help runtime after the existing lightweight UI primitives are defined. HOME `? AIDE` opens the new hub. Preserve existing HOME mode, danger, launch and Hall zones.

- [ ] **Step 6: Add renderer lifecycle cleanup**

Any help-owned textures are keyed per renderer and discarded from references when the renderer is destroyed/recreated, matching existing home/remaster texture ownership patterns.

- [ ] **Step 7: Run native regression binary and commit**

Expected: all existing tests plus new help model/render tests PASS.

Commit message: `feat: add multi-depth animated help center`

---

### Task 4: Open help from gameplay and resume the exact match safely

**Files:**
- Modify: `src/remaster_ai_fix.hpp`
- Modify: `src/help_runtime.hpp`
- Modify: `src/campaign_runtime.hpp` only if a narrow resume helper is required
- Modify: `tests/help_tutorial_regressions.hpp`
- Modify: `tests/regressions.cpp`

**Interfaces:**
- Consumes: Task 3 help state and in-game button rectangle.
- Produces: `sfFixResumeGamePending` (or equivalently named one-shot request) distinct from `sfFixLaunchPending`.
- Produces: a resume path that sets `setgui=false` and returns to `SF_UI_GAME` **without** `sfFixResetMatchState()` / campaign restart.
- Uses `sfCampaignSuspend()` to neutralize coop controls/surge when help is entered from campaign; resume restores the prior combat phase without advancing simulation while help was open.

- [ ] **Step 1: Write failing pause/resume state-preservation tests**

Create duel and coop fixtures with distinctive PV, `nrj`, projectile positions, asteroid identity/count and campaign time/health. Open help through the in-game `?`, apply many render/request cycles, close help, and assert those values did not advance or reset.

- [ ] **Step 2: Write failing stale-touch tests**

The finger that opens `?`, its motion and `FINGERUP` must all be converted to `SDL_USEREVENT`; after resume no shot count/control state changes until a fresh touch begins.

- [ ] **Step 3: Run native regressions and verify failure**

Expected: current bridge has no non-reset gameplay-help resume path.

- [ ] **Step 4: Implement in-game `?` interception and draw path**

Use one small edge button centered near the arena midline so it is reachable in portrait/landscape but does not overlap HOME gears. Consume only touches starting inside its rectangle.

- [ ] **Step 5: Implement non-reset resume request in `src/remaster_ai_fix.hpp`**

Keep launch/new-match and resume-existing-match as separate atomics/branches. Opening help sets GUI pause and resets active control fingers; closing from gameplay resumes without reseeding, clearing asteroids, restoring ships or restarting campaign.

- [ ] **Step 6: Verify duel + coop pause/resume tests and commit**

Expected: PASS with no simulation drift and no stale touch leakage.

Commit message: `feat: pause and resume live game through help`

---

### Task 5: Add isolated in-game tutorial modules

**Files:**
- Create: `src/tutorial_runtime.hpp`
- Modify: `src/help_runtime.hpp`
- Modify: `tests/help_tutorial_regressions.hpp`
- Modify: `tests/regressions.cpp`

**Interfaces:**
- Produces enum/step identifiers for: Move, Fire, Hud, Energy, HullShield, Dust, Kinetic, SurgeHold, SurgeReady, SurgeRelease, Danger.
- Produces lightweight `SfTutorialState` containing only tutorial-owned positions, PV/energy demo values, current step, progress and touch ownership.
- Produces: `sfTutorialStart(moduleOrAll)`, `sfTutorialHandleEvent(SDL_Event*)`, `sfTutorialTick(float dt)`, `sfTutorialDraw(SDL_Renderer*)`, `sfTutorialExit()`.
- Must not write `sfCampaignSave`, Hall of Fame, real `Spritej1/Spritej2` combat state, `sa1`, real projectile lists or real campaign encounter state.

- [ ] **Step 1: Write failing tutorial sequence tests**

For `TOUT FAIRE`, synthesize the required actions and assert the step order exactly matches the spec; for individual modules assert they start/end independently.

- [ ] **Step 2: Write failing isolation snapshot test**

Snapshot real ships, campaign save/progression, fame entries, encounter state, projectile/asteroid counts and danger selection. Run tutorial with representative inputs and ticks, exit, then assert every real-game snapshot value is unchanged except help/tutorial UI state.

- [ ] **Step 3: Write portrait/landscape tutorial render/input tests**

Assert highlighted target zones and NEXT/EXIT controls stay on-screen and touchable at 360x780 and 780x360.

- [ ] **Step 4: Run native regressions and verify failure**

Expected: tutorial interfaces absent.

- [ ] **Step 5: Implement isolated tutorial runtime**

Use tutorial-owned coordinates/state and existing rendering assets/primitives. The 2-second surge lesson uses tutorial-local timing but mirrors real semantics: charge, ready/vulnerable state, release/purge explanation.

- [ ] **Step 6: Integrate tutorial entry/exit with help hub**

`TUTORIEL` opens module selection / `TOUT FAIRE`; BACK exits module to help first, then help to the prior context.

- [ ] **Step 7: Run tutorial/help regressions and commit**

Expected: PASS and real state unchanged.

Commit message: `feat: add isolated guided gameplay tutorial`

---

### Task 6: Lock campaign behavior to the nine-level damage-only profile

**Files:**
- Modify: `src/campaign_runtime.hpp`
- Modify: `tests/difficulty_regressions.hpp`
- Modify: `tests/regressions.cpp`

**Interfaces:**
- Consumes: Task 1 `sfApplyHostileDanger(float)`.
- Keeps `sfCoopHurt(owner,damage)` as the non-kinetic boss damage entry point.

- [ ] **Step 1: Add failing campaign danger matrix test**

For indexes 0..8, apply a fixed non-kinetic boss hit with shield conditions held constant and assert incoming raw danger scaling matches x1..x40. Separately snapshot `sfCoopProfile().interval`, shot velocity and aim behavior at two danger indexes and assert they are unchanged by danger selection.

- [ ] **Step 2: Add kinetic exclusion regression**

Run an identical asteroid/explicit kinetic fixture at danger indexes 0 and 8 and assert identical kinetic result.

- [ ] **Step 3: Refactor `sfCoopHurt` to use `sfApplyHostileDanger`**

Do not change pattern cadence, projectile speeds or intercept calculations.

- [ ] **Step 4: Run campaign/difficulty/kinetic regressions and commit**

Expected: PASS.

Commit message: `test: lock nine-level campaign danger semantics`

---

### Task 7: Integrate Android generation and run the full regression/build chain

**Files:**
- Modify only as required: `scripts/prepare-legacy-source.cmake`, Android build glue/CMake inputs that enumerate new headers
- Modify: `scripts/test-regressions.sh` if focused test registration is incomplete

**Interfaces:**
- Consumes all Tasks 1-6.
- Produces generated Android source that compiles with the new help/tutorial/danger interfaces while `src/main.cpp` hash stays unchanged.

- [ ] **Step 1: Run `scripts/test-regressions.sh` from a clean temporary build context**

Expected: all Python, danger, native SDL/UBSan, generated Android syntax and legacy-field tests PASS.

- [ ] **Step 2: Verify historical source integrity**

Compare `src/main.cpp` blob/hash to the branch base and assert unchanged.

- [ ] **Step 3: Build Android APK + AAB through the existing workflow/build commands**

Expected: Gradle APK and AAB succeed; packaged assets include no new video payload.

- [ ] **Step 4: Inspect generated Android source**

Confirm classic AI hostile projectile damage uses the danger helper and asteroid/kinetic code does not.

- [ ] **Step 5: Commit any build-glue-only adjustments**

Commit message: `build: integrate help tutorial danger 9 on Android`

---

### Task 8: Compact living memories and prepare the phone-test handoff

**Files:**
- Modify: `brain.md`
- Modify: `brainmap.md`
- Modify: `debughistorical.md`
- Modify: `todo.md`
- Create: `docs/archive/2026-10-spacefortress-help-tutorial-danger9-history.md`
- Modify: `ordres-de-mission.md`

**Interfaces:**
- Living memories contain only current canon, architecture pointers, unresolved phone checks and the final verified SHA/workflow reference.
- Detailed implementation chronology, intermediate failures and build proofs go to the archive.

- [ ] **Step 1: Archive implementation/test history outside living memories**

Record task outcomes, important bug causes, test/build proof and any deliberate implementation deviations from the spec.

- [ ] **Step 2: Synchronize the four living memories compactly**

Add only: nine danger names/default/semantics, help/tutor entry points, branch/SHA, and phone-validation checklist. Remove completed intermediate TODOs.

- [ ] **Step 3: Update `ordres-de-mission.md` with the canonical mission status**

Preserve the rule: no merge `main` / no release before Fab phone validation and explicit approval.

- [ ] **Step 4: Run the full regression chain one final time on the exact documentation HEAD**

Expected: GREEN. If docs-only commits do not trigger/build code, record both the verified code SHA and final docs SHA unambiguously.

- [ ] **Step 5: Produce the phone-test handoff**

Report APK/AAB artifact names and exact SHA, with a concise phone checklist: nine danger names/default, HOME `?`, in-game pause/resume, three help modes, animated pages, tutorial isolation, classic IA damage scaling, coop damage scaling, kinetic unchanged.

Commit message: `docs: finalize help tutorial danger 9 handoff`

---

## Self-review result

- **Spec coverage:** all sections map to Tasks 1-8: danger table, classic IA scope, help depths/content, animated/fallback rendering, in-game pause/resume, tutorial isolation, campaign semantics, Android/full regression, compact memories.
- **Type/interface consistency:** danger helper has one name across classic/campaign tasks; help and tutorial have separate state; resume is explicitly separate from launch/reset.
- **No unauthorized behavior:** the plan contains no danger-driven cadence/speed/accuracy changes and keeps `ROCK N ROLL` as default.
- **Review-focus coverage:** each of the five high-risk cases has a named regression task.
- **Proportion:** implementation bodies are intentionally omitted; the plan fixes interfaces, exact semantics and verification commands without transcribing the code.
