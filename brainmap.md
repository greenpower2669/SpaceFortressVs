# brainmap.md — SpaceFortressVs

## Reprise rapide
- Branche : `feature/hall-of-fame-global-sync-v1`.
- Spec : `docs/superpowers/specs/2026-10-05-hall-of-fame-global-sync-design.md`.
- Plan : `docs/superpowers/plans/2026-10-05-hall-of-fame-global-sync.md`.

## Flux réseau vivant
- Victoire -> fame locale durable -> reconcile -> UUID stable -> pending.
- Upload 201/200 -> ack + serverId durable; erreurs -> pending conservé.
- `/sync` -> stage page -> validation -> merge durable -> cursor seulement après save.
- Snapshot offline -> global cache + locaux, dédup par serverId puis submissionId.

## Fichiers
- `src/hall_sync.hpp` : modèle/codec + validation remote.
- `src/hall_sync_storage.hpp` : persistance atomique.
- `src/hall_sync_runtime.hpp` : réconciliation, payload, ack/errors, pages, snapshot.
- `tests/hall_sync_regressions.cpp` : TDD sync.
- Futurs : transport/orchestration, Java HTTPS, JNI, rendu Hall.

## Protections
- `src/main.cpp` lecture seule; save campagne séparée du réseau.
- `danger=0` non envoyé; global ne modifie jamais progression.
- Clé jeu absente de Git; injection build uniquement.
