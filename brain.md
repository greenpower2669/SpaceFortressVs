# brain.md — SpaceFortressVs

## État canonique vivant
- Base livrée : v1.4.0 sur `main`; aucun merge/release sans validation Fab.
- Branche réseau : `feature/hall-of-fame-global-sync-v1`, base `778967f76fd5fa8184e60bfdc238482de6fe8950`.
- Hall Danger Boss réel validé : `47ada1e859e38ec5f09ae5104a0575eb25f08544`, workflow `37332234356` GREEN.
- `src/main.cpp` historique reste strictement protégé.

## Hall global sync v1
- Task 1 : état sync séparé + persistance atomique `hall-sync-v1.dat`.
- Task 2 : UUID stable par victoire, réconciliation locale, payload typé; `danger=0` local-only.
- Task 3 : ack upload idempotent, erreurs non destructives, pages stagées, cache/cursor durable, rejet callbacks obsolètes, snapshot global+local dédupliqué.
- Hooks automatiques post-victoire/Hall restent à finaliser en Task 4.
- Clé jeu : jamais dans Git; injection build via `SPACEFORTRESS_HOF_API_KEY`; aucune clé admin dans l'APK.

## Hall local — invariants
- Boss réel 1..200; Danger Boss HOME 1..9; ancien `danger=0` reste `DANGER INCONNU`.
- Points : `|boss*(danger-minutes)| + boss*(danger-minutes)`; tri points décroissant puis temps croissant.
- Sauvegarde campagne v3, v1/v2 toujours lisibles.

## Invariants permanents
- Progression campagne jamais modifiée par le global.
- Campagne, cinétique, poussières, aide/tuto et Danger 9 restent protégés.
- Validation finale téléphone Fab; aucun merge/release automatique.
