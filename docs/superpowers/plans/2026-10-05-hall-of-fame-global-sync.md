# Hall of Fame Global Sync Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add local-first automatic upload and incremental global Hall of Fame synchronization to SpaceFortressVs while keeping campaign progression independent, offline-safe, and fully playable without network configuration.

**Architecture:** Keep campaign save v3 unchanged. Add a separate native C++ sync model/storage/orchestrator, a narrow injected transport interface, and an Android Java HTTPS adapter reached through JNI. The Hall renderer consumes a merged local + cached-global snapshot; network work is asynchronous and never owns or mutates campaign progression.

**Tech Stack:** C++17, SDL2 application storage, POSIX atomic file writes, JNI, Java/Android `HttpsURLConnection`, `org.json`, Gradle/BuildConfig, JUnit 4 test-only dependencies, existing shell/C++ regression suite.

**Spec:** `docs/superpowers/specs/2026-10-05-hall-of-fame-global-sync-design.md`

## Global Constraints

- Branch: `feature/hall-of-fame-global-sync-v1`; base commit `778967f76fd5fa8184e60bfdc238482de6fe8950`.
- `src/main.cpp` must remain byte-for-byte unchanged.
- No merge to `main` and no Release without Fab's explicit order.
- Campaign save v1/v2/v3 readability and existing Hall Danger 1..9 behavior remain intact.
- Legacy local Hall entries with `danger=0` stay local-only and remain `DANGER INCONNU`.
- Remote synchronization must never mutate `cleared`, `selected`, pending campaign victory, or any other personal progression field.
- A local victory is durably recorded before it is eligible for network submission.
- Offline Hall must show both the last valid global cache and all local phone scores.
- Retry must reuse the same stable `submissionId` for a local victory.
- HTTP 201 and idempotent HTTP 200 are success; network errors, 5xx, 401/403, malformed protocol, and shutdown never delete pending data.
- The sync cursor advances only after the received page has been durably merged.
- Only one pagination cycle may be active at once.
- No libcurl and no new native JSON dependency.
- `SPACEFORTRESS_HOF_API_KEY` is read only from the Android build environment; the raw key must never enter Git, tests, docs, memory files, or logs.
- A build with no API key must compile, run, save locally, and display `SYNC NON CONFIGUREE` without attempting authenticated upload.
- No admin credential or admin feature is part of the APK.
- Every code commit must keep `brain.md`, `brainmap.md`, `debughistorical.md`, and `todo.md` synchronized per Fab-Copilot.

## Review Focus

- **Crash between campaign save and queue creation:** reopening the Hall must reconcile the durable local victory into exactly one pending sync record; pinned in Task 2.
- **Callback reordering/stale async work:** a late callback from an older page/cycle must not advance the current cursor or overwrite newer state; pinned in Task 3.
- **Remote values outside local expectations:** invalid boss/stars/duration/points or malformed IDs must be rejected without poisoning the durable cache; pinned in Task 3.
- **Repeated Hall opens / rapid UI navigation:** only one sync cycle and one upload per `submissionId` may be in flight; pinned in Task 4.
- **Secret handling during build/test:** missing key is supported, fake keys are used in tests, and repository scans must prove no supplied raw credential was committed; pinned in Task 5 and Task 7.

---

### Task 1: Native sync state and crash-safe persistence

**Files:**
- Create: `src/hall_sync.hpp`
- Create: `src/hall_sync_storage.hpp`
- Create: `tests/hall_sync_regressions.cpp`
- Modify: `scripts/test-regressions.sh`
- Modify with concise state only: `brain.md`, `brainmap.md`, `debughistorical.md`, `todo.md`

**Interfaces:**
- Produces `enum class SfHallUploadState { Pending, Acknowledged };`.
- Produces `struct SfHallLocalRecord { uint64_t localId; std::string submissionId; SfHallUploadState state; std::string serverId; };`.
- Produces `struct SfHallRemoteEntry` with `id`, optional `submissionId`, `playerName`, two pilot names, `boss`, `difficulty`, `durationMs`, `points`, `stars`, `completedAt`, `mode`, `encounter`, and optional integer `serverRank` (`0` means absent).
- Produces `struct SfHallSyncState { std::string cursor; std::map<uint64_t,SfHallLocalRecord> locals; std::map<std::string,SfHallRemoteEntry> globals; long long lastSuccessfulSync; };`.
- Produces pure codec `std::string sfHallSyncEncode(const SfHallSyncState&)` and `bool sfHallSyncDecode(std::istream&, SfHallSyncState&)`.
- Produces durable storage `bool sfHallSyncLoadFile(const std::string&, SfHallSyncState&, std::string&)` and `bool sfHallSyncSaveFile(const std::string&, const SfHallSyncState&, std::string&)`.

- [ ] **Step 1: Write RED persistence tests**

Add tests named `sync_state_round_trips_v1`, `partial_or_invalid_state_never_replaces_valid_state`, and `unknown_future_version_is_preserved_and_blocked`. Assert all local mappings, cursor, global entries, Unicode names, ranks, and last-sync timestamp round-trip exactly; invalid temporary/primary data must not erase the last valid file.

- [ ] **Step 2: Register and run only the new test binary**

Run:
`g++ -std=c++17 -O1 -I src tests/hall_sync_regressions.cpp -o /tmp/hall-sync && /tmp/hall-sync`

Expected: FAIL because `hall_sync.hpp` / storage interfaces do not exist.

- [ ] **Step 3: Implement the minimal sync-state types and version-1 text codec**

Use quoted UTF-8 strings and strict field/range validation. Keep network state completely outside `SfCampaignSave`.

- [ ] **Step 4: Implement atomic sync-state persistence**

Follow campaign-save safety: temporary file, flush/fsync, rename, directory fsync, preservation of an earlier valid state on failure, and explicit refusal to overwrite unknown future format.

- [ ] **Step 5: Re-run the focused tests**

Expected: all Task 1 tests PASS.

- [ ] **Step 6: Add the new binary to `scripts/test-regressions.sh` and run the full suite**

Run: `bash scripts/test-regressions.sh`
Expected: existing suite + `hall_sync_regressions` PASS.

- [ ] **Step 7: Synchronize the four living Fab-Copilot files and commit**

Commit message: `feat: add durable Hall sync state`

---

### Task 2: Stable submission IDs, reconciliation, and upload request model

**Files:**
- Modify: `src/hall_sync.hpp`
- Create: `src/hall_sync_runtime.hpp`
- Modify: `tests/hall_sync_regressions.cpp`
- Modify: `src/campaign_runtime.hpp` only at the post-`sfRecordCampaignVictory` integration point
- Modify with concise state only: `brain.md`, `brainmap.md`, `debughistorical.md`, `todo.md`

**Interfaces:**
- Consumes `SfCampaignSave`, `SfFameEntry`, `sfFamePoints`, and Task 1 state/storage.
- Produces `struct SfHallUploadRequest` containing exactly: `submissionId`, `playerName`, `pilots[2]`, `boss`, `difficulty`, `durationMs`, `points`, `stars`, `gameVersion`, `completedAtEpochSeconds`, `mode`, `encounter`.
- Produces `std::string sfHallMakeSubmissionId(uint64_t localId, uint64_t entropyA, uint64_t entropyB)` returning a UUID-v4-formatted string.
- Produces `bool sfHallReconcileLocal(const SfCampaignSave&, SfHallSyncState&, SfHallIdFactory)` where the injectable factory exists only to make tests deterministic.
- Produces `std::vector<SfHallUploadRequest> sfHallPendingUploads(const SfCampaignSave&, const SfHallSyncState&, const std::string& gameVersion)`.
- Produces runtime hook `void sfHallSyncAfterLocalVictorySaved()`; it runs only after `sfRecordCampaignVictory(...)` succeeds.

- [ ] **Step 1: Write RED reconciliation tests**

Cover: a fresh local victory becomes pending; a retry/restart reuses the exact same `submissionId`; two local IDs never share one submission ID; `danger=0` entries are not queued; the payload maps names, real boss/encounter, danger name, milliseconds, canonical points, stars, version, epoch completion time, and `coop-ai`/`coop-local` exactly.

- [ ] **Step 2: Add the crash-window Review Focus test**

Start from a campaign save containing a victory but an empty sync file, reconcile twice, and assert one persistent local record with one unchanged submission ID.

- [ ] **Step 3: Run focused tests and confirm RED**

Expected: FAIL on missing reconciliation/request APIs.

- [ ] **Step 4: Implement UUID creation and reconciliation**

Generate once, persist before upload eligibility, and never regenerate for an existing durable local victory ID.

- [ ] **Step 5: Implement upload request projection**

Do not serialize JSON in C++; keep the request typed for the Java adapter.

- [ ] **Step 6: Hook the existing successful name/save path**

Immediately after `sfRecordCampaignVictory(sfCoop.names)` returns true in `sfCoopSaveNames()`, call `sfHallSyncAfterLocalVictorySaved()`. The existing campaign save remains the first durable operation and its success semantics remain unchanged.

- [ ] **Step 7: Run focused + full regression tests**

Expected: all PASS and campaign format tests unchanged.

- [ ] **Step 8: Synchronize living files and commit**

Commit message: `feat: queue Hall victories after local save`

---

### Task 3: Upload results, paged sync staging, cache merge, and offline snapshot

**Files:**
- Modify: `src/hall_sync.hpp`
- Modify: `src/hall_sync_runtime.hpp`
- Modify: `tests/hall_sync_regressions.cpp`
- Modify with concise state only: `brain.md`, `brainmap.md`, `debughistorical.md`, `todo.md`

**Interfaces:**
- Produces `enum class SfHallSyncError { None, Offline, Timeout, Auth, Server, Protocol, NotConfigured };`.
- Produces `void sfHallApplyUploadSuccess(const std::string& submissionId, const std::string& serverId)`; both HTTP 201 and idempotent 200 call this same native path.
- Produces `void sfHallApplyUploadFailure(const std::string& submissionId, SfHallSyncError)`; pending state is retained.
- Produces page staging APIs `bool sfHallBeginPage(uint64_t cycleId, const std::string& requestedCursor)`, `bool sfHallStageRemote(uint64_t cycleId, const SfHallRemoteEntry&)`, `bool sfHallCommitPage(uint64_t cycleId, const std::string& requestedCursor, const std::string& nextCursor, bool hasMore)`, and `void sfHallFailPage(uint64_t cycleId, const std::string& requestedCursor, SfHallSyncError)`.
- Produces `struct SfHallDisplayEntry` and `std::vector<SfHallDisplayEntry> sfHallBuildSnapshot(const SfCampaignSave&, const SfHallSyncState&)`.
- Snapshot entries expose local/global origin, pending state, remote rank when known, and display-ready boss/danger/time/points/name values.

- [ ] **Step 1: Write RED upload-result tests**

Assert 201-equivalent and already-received-equivalent success acknowledge exactly once; Offline, Timeout, 500/Server, Auth 401/403, and Protocol errors leave the submission pending and preserve its ID.

- [ ] **Step 2: Write RED page/cursor tests**

Cover one page, multiple pages, page-save failure, interruption between pages, duplicate remote server IDs, and restart from the last committed cursor. Assert cursor changes only after durable cache save.

- [ ] **Step 3: Add stale-callback and invalid-remote Review Focus tests**

A callback with an old `cycleId` or mismatched requested cursor must be ignored. Reject remote entries with empty server ID, boss/encounter outside 1..200, stars outside 1..9, negative duration/points, or invalid required names without modifying cache/cursor.

- [ ] **Step 4: Write RED merged-snapshot tests**

Assert global cache + local scores are simultaneously present offline; acknowledged local/global pairs dedupe by server ID, then by submission ID when available; unresolved local pending remains visible once; remote data does not mutate any campaign progression field; score ordering stays points-desc/time-asc with stable exact ties.

- [ ] **Step 5: Run focused tests and confirm RED**

Expected: FAIL on missing page/result/snapshot interfaces.

- [ ] **Step 6: Implement upload result transitions, staged page commit, and merge**

Guard all runtime mutable state used by network callbacks with a mutex. Stage an entire page in memory, validate it, merge into a copy, save the copy durably, then and only then publish state/cursor.

- [ ] **Step 7: Implement merged snapshot**

Remote points are server-provided; local points use `sfFamePoints`. Remote rank is metadata only; local pending entries have no authoritative server rank.

- [ ] **Step 8: Run focused + full regression tests**

Expected: all PASS.

- [ ] **Step 9: Synchronize living files and commit**

Commit message: `feat: merge global and local Hall scores`

---

### Task 4: Transport abstraction and automatic sync-cycle orchestration

**Files:**
- Create: `src/hall_sync_transport.hpp`
- Modify: `src/hall_sync_runtime.hpp`
- Modify: `src/campaign_runtime.hpp` at Hall entry points only
- Modify: `tests/hall_sync_regressions.cpp`
- Modify with concise state only: `brain.md`, `brainmap.md`, `debughistorical.md`, `todo.md`

**Interfaces:**
- Produces `struct SfHallTransport { std::function<void(const SfHallUploadRequest&)> submit; std::function<void(uint64_t cycleId,const std::string& cursor,int limit)> syncPage; };`.
- Produces `void sfHallInstallTransport(SfHallTransport)`; an empty transport means local-only / not configured.
- Produces `void sfHallSyncOnHallOpen()` and `void sfHallSyncAfterLocalVictorySaved()`.
- Produces callbacks `sfHallTransportUploadAccepted(...)`, `sfHallTransportUploadFailed(...)`, `sfHallTransportRemoteEntry(...)`, `sfHallTransportPageDone(...)`, `sfHallTransportPageFailed(...)` used later by JNI.
- Runtime tracks in-flight submission IDs and one active page cycle; callbacks clear only their own matching work.

- [ ] **Step 1: Write RED fake-transport orchestration tests**

Use lambdas as a deterministic fake transport. Assert post-victory starts at most one upload for each pending ID; Hall open first reconciles/queues uploads then requests sync from the committed cursor with limit 100; `hasMore=true` schedules exactly the next page; failure stops the chain without losing state.

- [ ] **Step 2: Add rapid-open Review Focus test**

Call `sfHallSyncOnHallOpen()` repeatedly before callbacks complete. Assert one active pagination chain and no duplicate in-flight upload for the same submission ID.

- [ ] **Step 3: Run focused tests and confirm RED**

Expected: FAIL on missing transport/orchestration APIs.

- [ ] **Step 4: Implement event-driven orchestration**

No timer loop. Retry opportunities are only post-victory, Hall open, and next-page continuation.

- [ ] **Step 5: Hook Hall entry without networking in the renderer**

Trigger `sfHallSyncOnHallOpen()` when HOME transitions to `SF_UI_HALL` and when campaign completion requests `SF_UI_HALL`. Keep `sfCampaignDrawHall()` free of HTTP/JNI calls.

- [ ] **Step 6: Run focused + full tests**

Expected: all PASS.

- [ ] **Step 7: Synchronize living files and commit**

Commit message: `feat: orchestrate automatic Hall synchronization`

---

### Task 5: Android protocol, HTTPS client, build-time game key, and Java tests

**Files:**
- Create: `android/app/src/main/java/com/greenpower2669/spacefortressvs/HallOfFameSyncProtocol.java`
- Create: `android/app/src/main/java/com/greenpower2669/spacefortressvs/HallOfFameSyncClient.java`
- Create: `android/app/src/test/java/com/greenpower2669/spacefortressvs/HallOfFameSyncProtocolTest.java`
- Modify: `android/app/build.gradle`
- Modify: `android/app/src/main/AndroidManifest.xml`
- Modify with concise state only: `brain.md`, `brainmap.md`, `debughistorical.md`, `todo.md`

**Interfaces:**
- `HallOfFameSyncProtocol.Upload` mirrors `SfHallUploadRequest`.
- `static String encodeUpload(Upload)` emits the server JSON fields exactly: `submissionId`, `playerName`, `pilots`, `boss`, `difficulty`, `durationMs`, `points`, `stars`, `gameVersion`, `completedAt`, `mode`, `encounter`.
- `static UploadReply decodeUploadReply(int httpCode,String body)` recognizes 201 accepted and 200 already-received; other status classes remain failures.
- `static SyncPage decodeSyncPage(String body)` returns typed entries, `nextCursor`, `hasMore`; optional integer `rank` maps to `serverRank`, absent rank maps to 0.
- `HallOfFameSyncClient.submit(...)` and `HallOfFameSyncClient.syncPage(...)` enqueue work on one background executor and never block the SDL thread.
- Client uses public base URL `https://fab-hall-of-fame.gnrationsia.chatgpt.site`; POST uses the game key, GET `/sync` is public.

- [ ] **Step 1: Write RED protocol unit tests**

Use only fake key `test-key`. Assert exact request field mapping, UTC RFC3339 completion timestamp, Authorization value construction, 201/200 classification, 401/403/5xx classification, sync entries/cursor/hasMore parsing, Unicode names, optional rank, and malformed JSON rejection.

- [ ] **Step 2: Add test-only dependencies and run Java unit tests**

Add `testImplementation 'junit:junit:4.13.2'` and `testImplementation 'org.json:json:20240303'`; these are test-only. Run:
`gradle -p android :app:testDebugUnitTest --tests com.greenpower2669.spacefortressvs.HallOfFameSyncProtocolTest`

Expected: RED before implementation, then PASS after Steps 3-4.

- [ ] **Step 3: Implement the pure Java protocol adapter**

Product code uses Android's `org.json`; no native JSON dependency is added. Never log the Authorization header or key.

- [ ] **Step 4: Implement asynchronous `HttpsURLConnection` client**

Use finite connect/read timeouts, close streams/connections on every path, classify DNS/offline/timeout separately from HTTP status, and invoke native callbacks with decoded typed values only.

- [ ] **Step 5: Add Internet permission and build configuration**

Add `<uses-permission android:name="android.permission.INTERNET" />`. Enable BuildConfig generation if needed. Read `System.getenv('SPACEFORTRESS_HOF_API_KEY') ?: ''` at Gradle configuration and inject `BuildConfig.HOF_API_KEY` without any `println`/logging. `BuildConfig.VERSION_NAME` supplies `gameVersion`.

- [ ] **Step 6: Add missing-key and secret-handling Review Focus tests**

Assert empty BuildConfig key causes `SYNC NON CONFIGUREE` behavior at the native boundary and no POST attempt. Repository tests use only `test-key`; no real secret literal is introduced.

- [ ] **Step 7: Run Java tests and Android Java compilation**

Run:
`gradle -p android :app:testDebugUnitTest :app:compileDebugJavaWithJavac`

Expected: PASS with or without `SPACEFORTRESS_HOF_API_KEY` set.

- [ ] **Step 8: Synchronize living files and commit**

Commit message: `feat: add Android Hall HTTPS client`

---

### Task 6: JNI bridge and Android transport installation

**Files:**
- Create: `android/app/src/main/cpp/hall_sync_jni.cpp`
- Modify: `android/app/src/main/cpp/CMakeLists.txt`
- Modify: `src/hall_sync_runtime.hpp` only if callback declarations need final wiring
- Modify: `tests/hall_sync_regressions.cpp` for host no-op/fake transport compile coverage
- Modify with concise state only: `brain.md`, `brainmap.md`, `debughistorical.md`, `todo.md`

**Interfaces:**
- `JNI_OnLoad` caches the app class/method IDs and installs an `SfHallTransport` whose submit/page lambdas call Java static async methods.
- Java declares native callbacks for upload success/failure, remote entry delivery, page completion, and page failure.
- JNI converts Java strings/values to the typed native callback interfaces from Task 4; it does not parse JSON or alter campaign state.
- Host builds never require JNI: the existing fake/empty transport remains valid when `hall_sync_jni.cpp` is not compiled.

- [ ] **Step 1: Write/extend host tests proving the native core works with no JNI installed**

Expected: local save, Hall snapshot, and fake transport tests remain PASS on Linux.

- [ ] **Step 2: Add `hall_sync_jni.cpp` to Android `main` shared library sources and run an Android compile**

Run: `gradle -p android :app:assembleDebug`
Expected initially: compile/link failures until JNI methods are implemented.

- [ ] **Step 3: Implement native-to-Java calls**

Cache global class reference and method IDs at load; every string conversion checks exceptions/nulls and reports Protocol/NotConfigured rather than crashing gameplay.

- [ ] **Step 4: Implement Java-to-native callbacks**

Use `cycleId` and requested cursor on page callbacks so stale callbacks are rejected by Task 3 logic.

- [ ] **Step 5: Run focused native tests, Java tests, and debug APK build**

Expected: all PASS; APK links `hall_sync_jni.cpp` only on Android.

- [ ] **Step 6: Synchronize living files and commit**

Commit message: `feat: bridge Hall sync through JNI`

---

### Task 7: Hall presentation of global cache + local phone scores

**Files:**
- Modify: `src/hall_of_fame_runtime.hpp`
- Modify: `tests/hall_sync_regressions.cpp`
- Modify with concise state only: `brain.md`, `brainmap.md`, `debughistorical.md`, `todo.md`

**Interfaces:**
- Consumes `sfHallBuildSnapshot(...)` from Task 3 and a read-only status snapshot from Task 4.
- Existing star drawing remains authoritative for 1..9 stars.
- Presentation marks unresolved local uploads as `LOCAL / EN ATTENTE`; synchronized/global entries are displayed once; status line may show `SYNC OK`, `SYNC EN ATTENTE`, `HORS LIGNE`, `AUTH SYNC`, `ERREUR PROTOCOLE`, or `SYNC NON CONFIGUREE`.

- [ ] **Step 1: Write RED presentation-model assertions**

Assert the display snapshot includes cached global + local offline, does not duplicate acknowledged entries, preserves 1..9 stars and `DANGER INCONNU` local legacy entries, and never assigns authoritative server rank to pending local entries.

- [ ] **Step 2: Update Hall renderer to consume the merged snapshot**

Keep panel layout/paging behavior and canonical points/time ordering. Do not perform network work from drawing code.

- [ ] **Step 3: Run focused and full regression tests**

Run: `bash scripts/test-regressions.sh`
Expected: PASS.

- [ ] **Step 4: Synchronize living files and commit**

Commit message: `feat: display global and local Hall entries together`

---

### Task 8: End-to-end verification, secret scan, Android packaging, and documentation

**Files:**
- Modify append-only: `ordres-de-mission.md`
- Modify/compact: `brain.md`, `brainmap.md`, `debughistorical.md`, `todo.md`
- Create archival detail only if needed under `docs/archive/`
- No gameplay source change unless a verification failure demonstrates one.

**Interfaces:**
- No new product API. This task verifies the branch and records exact evidence.

- [ ] **Step 1: Run the complete regression suite fresh**

Run: `bash scripts/test-regressions.sh`
Expected: all PASS.

- [ ] **Step 2: Run Java transport tests fresh**

Run: `gradle -p android :app:testDebugUnitTest --tests com.greenpower2669.spacefortressvs.HallOfFameSyncProtocolTest`
Expected: PASS.

- [ ] **Step 3: Verify historical source protection**

Compare `src/main.cpp` blob/SHA against base `778967f76fd5fa8184e60bfdc238482de6fe8950`; expected byte-for-byte unchanged.

- [ ] **Step 4: Verify secret hygiene**

Scan tracked files and commit diff for credential-like literals and confirm no raw supplied API key appears. Confirm only the environment-variable name `SPACEFORTRESS_HOF_API_KEY` and fake test key are tracked. Never print the real key during this check.

- [ ] **Step 5: Build without a key**

Run debug build with `SPACEFORTRESS_HOF_API_KEY` unset. Expected: APK builds and local-only sync path is valid/non-fatal.

- [ ] **Step 6: Build phone-test APK/AAB with the key supplied only through protected build environment**

Use the existing Android CI/package workflow or an equivalent protected environment that does not echo the secret. Expected: regression, APK, AAB, verify/package GREEN; publish-release remains skipped.

- [ ] **Step 7: Inspect artifact metadata and compute APK SHA-256**

Record workflow ID, code SHA, artifact ID/digest, APK filename and APK SHA-256. Do not publish a Release.

- [ ] **Step 8: Update mission/memory documentation compactly and commit**

Append the sync mission result to `ordres-de-mission.md`; keep the four living memory files concise and archive verbose diagnostics. Commit message: `docs: record Hall global sync validation`.

- [ ] **Step 9: Final branch review**

Confirm: no main merge, no Release, `src/main.cpp` unchanged, no credential committed, all tests green, configured APK available for Fab phone validation.
