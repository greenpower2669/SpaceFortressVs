# SpaceFortressVs — Hall of Fame global sync design

Date: 2026-10-05
Status: design for Fab review
Branch: `feature/hall-of-fame-global-sync-v1`
Base: `778967f76fd5fa8184e60bfdc238482de6fe8950`

## 1. Goal

Add a global Hall of Fame synchronization layer to SpaceFortressVs without changing historical gameplay or personal campaign progression.

The feature is **local-first**:

- a victory is saved locally before any network action;
- network failure can never prevent saving, playing, opening the Hall, or continuing the campaign;
- after every victory, the game automatically attempts to upload the score;
- when the Hall opens, the game automatically retries pending uploads and then downloads global updates;
- offline, the Hall shows both the last known global cache and local scores from the phone;
- remote data never changes `cleared`, `selected`, pending victory state, or any other local progression field.

`src/main.cpp` remains byte-for-byte unchanged.

## 2. Existing invariants preserved

The current local Hall contract remains canonical:

- boss is the real encounter number 1..200;
- Boss Danger HOME is 1..9 and is distinct from campaign four-tier difficulty;
- `MOU DU GENOU` = 1 star and `APOCALYPSE` = 9 stars;
- legacy entries without persisted danger remain `danger=0` / `DANGER INCONNU` and are never guessed;
- points use Fab's formula:
  `|boss * (danger - minutes)| + boss * (danger - minutes)`;
- ranking is points descending, then shorter duration;
- campaign save v1/v2/v3 compatibility remains intact.

The sync subsystem must not alter kinetic shield behavior, dust behavior, Danger Boss damage rules, campaign combat, help/tutorial, or the historical classic source.

## 3. Server contract

Game id: `spacefortressvs`.

### Submit a victory

`POST /api/v1/games/spacefortressvs/scores`

Headers:

- `Content-Type: application/json`
- `Authorization: Bearer <game key>`

Payload semantics:

- `submissionId`: stable UUID for one local victory; reused on every retry;
- `playerName`: local team/player display name (`SfFameEntry.names[2]`);
- `pilots`: the two local pilot names (`names[0]`, `names[1]`);
- `boss`: real boss/encounter 1..200;
- `difficulty`: canonical Boss Danger HOME name;
- `durationMs`: local recorded duration converted to milliseconds;
- `points`: points computed by the canonical local formula;
- `stars`: actual Boss Danger 1..9;
- `gameVersion`: packaged game version;
- `completedAt`: local victory timestamp encoded as UTC RFC3339;
- `mode`: `coop-ai` or `coop-local`;
- `encounter`: same real 1..200 encounter identity used by the campaign.

A new accepted submission returns HTTP 201. Re-sending the same `submissionId` returns HTTP 200 with `already_received` semantics and must be treated as success, not as a duplicate.

### Incremental synchronization

`GET /api/v1/games/spacefortressvs/sync?cursor=<cursor>&limit=100`

The response contains:

- `entries`;
- `nextCursor`;
- `hasMore`.

Entries are merged by server `id`. The cursor advances only after the received page and its entries have been saved durably.

If the published OpenAPI exposes the server ranking position under a transport-specific JSON field name, the Android adapter maps that field into the internal `serverRank` property. The native Hall does not depend on the external JSON field name.

## 4. Architecture decision

Use a **native C++ sync state + Android Java HTTPS transport**.

Do not add libcurl.

### Native responsibilities

A dedicated native subsystem owns:

- stable local submission identity;
- pending upload state;
- durable global cache;
- sync cursor;
- merge/deduplication rules;
- Hall snapshot exposed to the renderer;
- reconciliation between campaign fame entries and sync metadata.

### Android responsibilities

A small Java client owns:

- HTTPS requests;
- Bearer header injection;
- JSON serialization/deserialization;
- background execution outside the SDL render/game thread;
- mapping network responses into typed JNI callbacks.

Java does not own campaign progression and does not rewrite the campaign save.

### JNI boundary

The JNI contract is deliberately narrow.

Native -> Java:

- submit one typed victory;
- request one sync page for a cursor;
- cancel/ignore stale work when the activity is shutting down.

Java -> Native callbacks:

- upload accepted/already-received;
- upload failed with classified HTTP/network status;
- one decoded global entry;
- sync page completed with `nextCursor` and `hasMore`;
- sync page failed.

The Java adapter parses JSON so the C++ game does not need a general JSON library.

## 5. Persistence model

Do **not** extend the campaign save merely to support the network layer. Keep campaign progression and synchronization as separate durable files.

Introduce a versioned sync-state file under the same SDL application preference directory, for example `hall-sync-v1.dat`.

It stores only synchronization metadata:

- sync format version;
- last committed cursor;
- local victory id -> stable `submissionId` mapping;
- upload state (`pending` or acknowledged);
- acknowledged server id when known;
- global cached entries keyed by server id;
- server rank metadata when supplied by the server;
- last successful sync timestamp for display/status only.

Writes must use the same safety philosophy as campaign persistence: encode, validate, write temporary file, fsync, rename, and preserve the previous valid state on failure.

No API key is persisted in this file.

## 6. Reconciliation and crash safety

A crash must not create a permanently missing score.

Whenever the Hall opens, and after a local victory is durably recorded, the sync subsystem scans local fame entries.

For every local fame entry with valid Boss Danger 1..9:

1. if a sync record already exists for its durable local victory id, reuse its existing `submissionId`;
2. otherwise generate one stable UUID, persist the mapping, and mark it pending;
3. never replace that UUID on retry.

This reconciliation closes the crash window between campaign-save success and network-queue creation.

Legacy local entries with `danger=0` stay visible locally but are not uploaded because their difficulty/stars cannot be reconstructed honestly.

## 7. Automatic runtime flow

### After each victory

1. Complete the existing local campaign-save path first.
2. Reconcile that local fame entry into sync state.
3. If Android networking is configured and no upload for that entry is already in flight, start upload asynchronously.
4. On HTTP 201: mark acknowledged and store returned server identity.
5. On HTTP 200 `already_received`: mark acknowledged identically.
6. On network failure, timeout, HTTP 5xx, malformed response, or app shutdown: retain pending state unchanged.
7. On HTTP 401/403: retain pending state, expose a non-destructive sync configuration/authentication status, and do not spin in a rapid retry loop.

### When the Hall opens

1. Render immediately from durable local fame + durable global cache; do not wait for network.
2. Reconcile local fame into the pending queue.
3. Attempt pending uploads.
4. Start GET sync from the committed cursor.
5. Decode and stage one page.
6. Durably merge that page by server id.
7. Only then commit `nextCursor`.
8. If `hasMore=true`, request the next page.
9. Stop cleanly on any error; keep the last committed cache/cursor intact.

Only one Hall sync cycle may be active at a time. Opening the Hall repeatedly must not create concurrent pagination chains.

## 8. Hall merge and display

The renderer receives one merged presentation snapshot containing:

- cached global entries;
- local entries from this phone.

Deduplication order:

1. if a local sync record has an acknowledged server id, merge local + global by that server id;
2. otherwise compare stable `submissionId` when a corresponding remote submission identity is available;
3. unresolved local pending entries remain separate and visibly local/pending.

A server-synchronized score is shown once.

The presentation sort keeps the canonical score ordering. A remote server rank, when supplied, is retained as server metadata. A local unsent score must not pretend to have an authoritative server rank; it is labelled as local/pending until acknowledged and observed globally.

Offline behavior is explicit:

- keep displaying the last valid global cache;
- keep displaying local phone scores;
- keep pending scores queued;
- show a compact status such as `HORS LIGNE`/`SYNC EN ATTENTE` without blocking the Hall.

## 9. Security and build configuration

The game key is not committed to Git.

Repository rules:

- no raw key in C++, Java, Gradle files, tests, documentation, memories, logs, or artifacts intended as documentation;
- no admin credential in the APK under any circumstance;
- future admin access is outside this APK feature.

For Android builds, Gradle reads `SPACEFORTRESS_HOF_API_KEY` from the build environment and injects it into the generated Android build configuration. The Java network adapter reads that generated value.

Fab accepts that a game key embedded in a shipped APK can theoretically be extracted. This is an accepted v1 risk, but it does not justify storing the key in source control.

A build without the environment variable must still compile and run. In that configuration the sync subsystem remains local-only and reports a non-fatal `SYNC NON CONFIGUREE` state rather than failing the game.

A phone-test APK with real synchronization requires the build environment to supply the game key without printing it in CI logs.

## 10. Android integration

Required Android changes are isolated outside historical game code:

- add Internet permission to `AndroidManifest.xml`;
- add a dedicated Java Hall sync client beside `SpaceFortressActivity`;
- keep `SpaceFortressActivity` minimal, adding only lifecycle/bridge setup if required;
- use `HttpsURLConnection` or the Android/Java standard HTTPS stack; no new external networking dependency;
- perform all network I/O on a background executor;
- callbacks into native code carry decoded typed values only.

Native Android calls are guarded by `__ANDROID__` so host-side regression tests can run with a deterministic fake transport.

## 11. Retry policy

Retries are event-driven, not a tight background polling loop.

Retry opportunities:

- immediately after a new local victory;
- when the Hall opens;
- when a sync cycle continues to the next page.

No sub-minute autonomous retry timer is required for v1.

Failures are classified:

- offline/DNS/timeout: leave pending and retry on the next normal opportunity;
- HTTP 5xx: leave pending and retry later;
- HTTP 401/403: leave pending and show auth/configuration status;
- invalid JSON/protocol mismatch: preserve all local/cache state and show sync protocol error;
- HTTP 201/200-idempotent: acknowledge exactly once locally.

## 12. Test strategy

Implementation follows TDD.

### Native RED tests first

Cover at minimum:

1. local victory reconciles to pending;
2. `submissionId` is generated once;
3. retry reuses the same `submissionId`;
4. 201 acknowledges;
5. 200 `already_received` acknowledges;
6. offline leaves pending;
7. HTTP 500 leaves pending;
8. 401/403 leave pending without data loss;
9. single-page sync;
10. multi-page sync;
11. cursor commits only after durable page save;
12. interruption between pages preserves the previous cursor;
13. merge by server id;
14. local + global deduplicate after acknowledgement;
15. global cache remains visible offline;
16. local scores remain visible offline;
17. remote cache cannot mutate campaign progression;
18. v1/v2/v3 campaign saves remain readable;
19. Boss Danger 1..9 remains intact;
20. danger-unknown legacy entries remain local-only;
21. exact ties remain deterministic/stable;
22. sync state recovery after invalid/partial temp write.

### Transport tests

The Android adapter is tested against deterministic mocked HTTP results for:

- request JSON field mapping;
- Authorization header presence without logging its value;
- 201 / 200 / 401 / 403 / 5xx classification;
- sync JSON decoding;
- pagination callback values;
- malformed JSON.

### Full verification

Run `scripts/test-regressions.sh`, then the Android APK+AAB build/package pipeline. Verify `src/main.cpp` unchanged against the base commit.

## 13. Proposed implementation units

Names may be adjusted to existing conventions, but responsibilities stay isolated:

- `src/hall_sync.hpp` — pure native model, reconciliation, queue, merge, cursor state;
- `src/hall_sync_storage.hpp` — atomic sync-state persistence;
- `src/hall_sync_android.hpp` — JNI-facing native adapter guarded for Android;
- `android/app/src/main/java/com/greenpower2669/spacefortressvs/HallOfFameSyncClient.java` — HTTPS + JSON transport;
- `tests/hall_sync_regressions.cpp` — pure deterministic native sync tests;
- Android transport test source if the current Gradle test layout supports it;
- minimal integration changes in the current Hall/campaign runtime and Android manifest/build configuration.

The existing local Hall renderer remains the rendering authority; it consumes the merged snapshot rather than becoming a network client itself.

## 14. Non-goals for v1

Do not add:

- admin UI or admin credentials;
- remote editing/deleting scores;
- login/accounts;
- remote campaign save/progression;
- background Android service;
- push notifications;
- libcurl or a new JSON library in native code;
- automatic correction of legacy `DANGER INCONNU` entries;
- merge to `main` or release publication.

## 15. Acceptance criteria

The design is complete when an implementation can demonstrate all of the following:

- a new victory is safely local before any network request;
- a stable `submissionId` survives retries and app restarts;
- successful/repeated server submissions do not duplicate the score;
- Hall open automatically retries pending scores and paginates global sync;
- offline Hall simultaneously shows last global cache and local scores;
- synchronization cannot modify personal campaign progression;
- API key is absent from Git history and source files;
- a build without the key remains playable/local-only;
- a correctly configured build can synchronize automatically;
- all regression tests pass;
- APK and AAB package successfully;
- `src/main.cpp` is unchanged;
- no merge to `main` and no Release occur without Fab's explicit order.
