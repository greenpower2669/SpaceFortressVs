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
- Task 8 en validation : CI doit tester natif + Java, construire sans clé, scanner le secret, puis construire l’APK/AAB avec le secret GitHub seulement s’il est configuré.
- Clé jeu : jamais dans Git; aucune clé admin dans l'APK.

## Hall local — invariants
- Boss réel 1..200; Danger Boss HOME 1..9; ancien `danger=0` reste `DANGER INCONNU`.
- Points : `|boss*(danger-minutes)| + boss*(danger-minutes)`; tri points décroissant puis temps croissant.
- Sauvegarde campagne v3, v1/v2 toujours lisibles.

## Invariants permanents
- Progression campagne jamais modifiée par le global.
- Campagne, cinétique, poussières, aide/tuto et Danger 9 restent protégés.
- Validation finale téléphone Fab; aucun merge/release automatique.
