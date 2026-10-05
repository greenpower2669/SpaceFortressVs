# brain.md — SpaceFortressVs

## État canonique vivant
- Base livrée : v1.4.0 sur `main`; aucun merge/release sans validation Fab.
- Branche réseau : `feature/hall-of-fame-global-sync-v1`, base `778967f76fd5fa8184e60bfdc238482de6fe8950`.
- Hall Danger Boss réel validé : `47ada1e859e38ec5f09ae5104a0575eb25f08544`, workflow `37332234356` GREEN.
- `src/main.cpp` historique reste strictement protégé.

## Hall global sync v1
- Tasks 1–3 : état sync durable, UUID/payload, ack/pages/cache/snapshot global+local.
- Task 4 : transport injecté + anti-doublon in-flight + cycle paginé unique; victoire et entrée Hall déclenchent automatiquement la sync depuis la frontière d’événements, jamais depuis le renderer.
- Task 5 : protocole Java typé + HTTPS asynchrone, GET `/sync` public, POST avec clé BuildConfig injectée par `SPACEFORTRESS_HOF_API_KEY`, permission Internet, tests JSON sans vraie clé.
- Task 6 : JNI Android relie transport natif ↔ Java sans parser JSON côté C++; callbacks gardent cycle/cursor et erreurs typées; build hôte reste sans JNI.
- Task 7 : Hall lit uniquement un snapshot global-cache + local, déduplique serveur/téléphone, garde les pending visibles et affiche le statut de sync sans lancer de réseau depuis le rendu.
- Task 8 : workflow `37372279557`, tentative 2, GREEN au HEAD code `c48d7b1af04139ef9e6f8bd3c28fbbfff616759b`; artifact configuré `11374640381`.
- Clé jeu : jamais dans Git; aucune clé admin dans l'APK.

## Audit téléphone SYNC NON CONFIGUREE — 2026-10-06
- Le code et la tentative 2 configurée ne sont pas la cause du symptôme.
- Cause racine prouvée : l’APK précédemment remis à Fab était l’ancien artifact sans clé (artifact `11369888063`, APK SHA-256 `d7a24c1f1f50b569f45bb79cf2ad75ef2be834cd9d8ed5a95b38fe6edb0c72fe`).
- Le bon artifact de la tentative 2 est `11374640381`; son APK SHA-256 est `0ca76a074b8148578b95a5b10a1fd32fad41d1c27cb46454ff2cd15d2133ab87` et contient une configuration HOF non vide sans exposer la valeur.
- Aucun correctif code ni rebuild n’est requis avant re-test téléphone avec cet APK exact.
- Validation réelle POST/GET serveur reste à faire par vraie victoire Fab avec l’APK configuré.

## Hall local — invariants
- Boss réel 1..200; Danger Boss HOME 1..9; ancien `danger=0` reste `DANGER INCONNU`.
- Points : `|boss*(danger-minutes)| + boss*(danger-minutes)`; tri points décroissant puis temps croissant.
- Sauvegarde campagne v3, v1/v2 toujours lisibles.

## Invariants permanents
- Progression campagne jamais modifiée par le global.
- Campagne, cinétique, poussières, aide/tuto et Danger 9 restent protégés.
- Validation finale téléphone Fab; aucun merge/release automatique.
