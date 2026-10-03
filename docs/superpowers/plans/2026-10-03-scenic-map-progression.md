# Scenic Map Progression Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Share campaign scenery with classic mode, prevent classic scene repetition for a full 100-scene bag, and make campaign scenery, boss colour and boss travel evolve continuously across all 200 encounters.

**Architecture:** Add a small pure `scenic_mix.hpp` model that defines 20 base map identities and deterministically composes 100 candidate scenes from mixed backdrop/planet/filter components. Classic mode owns a shuffled 100-entry bag and chooses a new scene on each game entry. Campaign rendering consumes the encounter number (0..199), not only boss index (0..49), so visual colour and movement progress continuously through all 200 fights. Rendering remains SDL-side; `src/main.cpp` stays untouched.

**Tech Stack:** C++17, SDL2 / SDL_image, existing regression harness, GitHub Actions Android build.

**Spec:** Fab request in conversation, 2026-10-03: coop maps in classic, 100-candidate no-repeat memory, mixes from 20 initial maps, subtle planet/boss RGB filters, blue→green→red difficulty colour and increasingly edge-reaching boss movement.

## Global Constraints

- Work only on `fix/gameplay-campaign-200`.
- Do not merge `main` and do not publish a release.
- `src/main.cpp` must remain byte-identical.
- Keep existing 50 boss identities and 4 historical tentacle-count tiers.
- Scene colour is continuous over encounter 1..200; do not make four abrupt colour jumps.
- Classic random selection uses a 100-candidate bag without replacement; after all 100 have appeared, refill to 100% eligibility and reshuffle.
- Scene mixes are assembled from 20 stable base identities and may combine one map's backdrop with another map's planet/filter family.
- Planet and boss tint must remain subtle, preserving source artwork and alpha.
- Boss travel starts centre-biased and progressively approaches arena edges at high encounters while remaining inside safe bounds.
- Synchronize `brain.md`, `brainmap.md`, `debughistorical.md`, `todo.md`, and `ordres-de-mission.md` in the final lot.

## Review Focus

1. Re-entering classic 100 times must not repeat a scene ID before the bag resets.
2. Scene 100/200 must not accidentally reuse encounter 0/50 colours because of `%50` boss indexing.
3. Texture colour/alpha/blend modulation must always be restored after scenic/boss rendering.
4. Extreme boss movement must remain inside the playable arena and not create unavoidable contact at the edge.
5. Renderer recreation must reload classic campaign atlas textures safely.

---

### Task 1: Pure scene model and no-repeat bag

**Files:**
- Create: `src/scenic_mix.hpp`
- Test: `tests/difficulty_regressions.hpp`

**Interfaces:**
- Produces `SfScenicProfile sfScenicProfile(int candidateOrEncounter)`.
- Produces `SDL_Color sfProgressColor(int encounter)` only when SDL colour is available at call site, or an SDL-independent RGB struct converted by renderers.
- Produces `float sfBossTravelProgress(int encounter)`.
- Produces `SfScenicBag` / `sfScenicBagNext(...)` for 100 no-repeat candidates.

- [ ] Write tests proving 100 unique draws, full refill after exhaustion, stable 20-base mixing, continuous blue→green→red progression, and monotonic boss travel.
- [ ] Run focused regression and confirm RED because the model does not exist.
- [ ] Implement minimal pure model.
- [ ] Run focused regression and confirm GREEN.

### Task 2: Campaign uses full 0..199 scenic progression

**Files:**
- Modify: `src/campaign_runtime.hpp`
- Modify: `src/boss_difficulty_visuals.hpp`
- Test: `tests/difficulty_regressions.hpp`

**Interfaces:**
- `sfDrawCampaignSpace` consumes encounter 0..199.
- `sfCoopBossPosition` uses continuous travel progress.
- `sfDrawEncounterBoss` applies a subtle RGB correction toward encounter progress colour while preserving 0/4/8/20 tentacle counts.

- [ ] Write tests proving encounter 0, 50, 100, 150 and 199 have distinct scene/filter progression and increasing movement span.
- [ ] Verify RED on current code.
- [ ] Route full encounter into campaign scenery and add mixed atlas selections/tints.
- [ ] Add subtle boss tint and continuous aura colour.
- [ ] Replace four-step boss movement span with safe continuous progression.
- [ ] Run full regressions and confirm GREEN.

### Task 3: Classic consumes campaign scene atlas with 100-scene memory

**Files:**
- Modify: `src/remaster_runtime.hpp`
- Test: `tests/restoration_regressions.hpp`

**Interfaces:**
- On transition into `SF_UI_GAME`, classic chooses exactly one new candidate from its `SfScenicBag`.
- The selected candidate stays stable until leaving/re-entering gameplay.
- Classic draws campaign nebula/planet atlas layers behind its existing parallax stars and optional classic scenic accents.

- [ ] Write tests for scene change only at game-entry transition and bag uniqueness.
- [ ] Verify RED on current code.
- [ ] Add renderer-owned campaign atlas textures and safe recreation.
- [ ] Draw mixed campaign backdrop/planet using the current classic scene.
- [ ] Keep legacy/classic overlays subtle and non-blocking.
- [ ] Run full regressions and confirm GREEN.

### Task 4: FAB-Copilot sync and Android verification

**Files:**
- Modify: `brain.md`
- Modify: `brainmap.md`
- Modify: `debughistorical.md`
- Modify: `todo.md`
- Modify: `ordres-de-mission.md`

- [ ] Record the 20-base / 100-bag / 200-progress architecture and exact test evidence.
- [ ] Assert `src/main.cpp` blob SHA is unchanged from `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- [ ] Run complete native regression suite.
- [ ] Run Android `assembleDebug` + `bundleRelease` through normal CI.
- [ ] Verify APK/AAB packaging and report exact code SHA, run ID, artifact ID and APK SHA-256.
