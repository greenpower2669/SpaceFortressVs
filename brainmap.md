# brainmap.md — SpaceFortressVs

## Reprise rapide
- Branche : `feature/hall-of-fame-global-sync-v1`.
- Spec : `docs/superpowers/specs/2026-10-05-hall-of-fame-global-sync-design.md`.
- Plan : `docs/superpowers/plans/2026-10-05-hall-of-fame-global-sync.md`.
- Base réseau : `778967f76fd5fa8184e60bfdc238482de6fe8950`.

## Flux cible
Victoire locale durable -> reconciliation sync -> `submissionId` stable -> upload async -> ack 201/200.
Ouverture Hall -> affichage immédiat local+cache -> retry pending -> `/sync?cursor=...&limit=100` -> merge durable -> commit cursor -> page suivante.

## Fichiers sync
- `src/hall_sync.hpp` : modèle/codec sync.
- `src/hall_sync_storage.hpp` : persistance atomique séparée.
- `src/hall_sync_runtime.hpp` : UUID stable, reconciliation, projection du payload.
- `tests/hall_sync_regressions.cpp` : contrats TDD sync.
- Futurs : transport/orchestration, Java HTTPS, JNI, rendu fusionné.

## Protections
- `src/main.cpp` lecture seule.
- Campaign save v1/v2/v3 indépendant du réseau.
- `danger=0` non envoyé.
- Clé de jeu absente de Git; injection build uniquement.
