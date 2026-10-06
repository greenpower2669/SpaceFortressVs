# brain.md — SpaceFortressVs

## État canonique vivant
- Base livrée : v1.4.0 sur `main`; aucun merge/release sans validation Fab.
- Branche réseau/correctifs pré-release : `feature/hall-of-fame-global-sync-v1`, base `778967f76fd5fa8184e60bfdc238482de6fe8950`.
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
- Validation téléphone 2026-10-06 : APK configuré testé par Fab, écran `SYNC OK` / `GLOBAL 1 + LOCAL 0`; chaîne runtime globale validée.

## Audit livraison SYNC NON CONFIGUREE — clos
- Cause racine prouvée : ancien artifact sans clé `11369888063` remis par erreur au téléphone.
- Bon artifact configuré : `11374640381`; APK SHA-256 `0ca76a074b8148578b95a5b10a1fd32fad41d1c27cb46454ff2cd15d2133ab87`.
- Le re-test du bon APK a donné `SYNC OK`; aucun patch réseau n’était requis.

## Mini-fix surcharge cinétique v2 — en validation CI
- Appui second doigt <0,30 s : pas de cercle irisé, puissance normale ×1; relâchement court reste un tir classique.
- 0,30–2,00 s : surcharge visible ×2; à 2 s état armé et vulnérable jusqu’au relâchement.
- Relâchement armé : purge + vague visuelle = diamètre 3,0× vaisseau; poussière blanche de purge reste au canon 100 %.
- Son `kinetic_release.wav` ré-authored comme déflagration EMP originale via `assets/sounds/kinetic_release_emp.b64`, décodée et contrôlée par `prepare-assets.py`.
- CLASSIQUE + COOP utilisent le même état de surcharge.
- RED TDD prouvé par workflow `37424970512`; GREEN complet et test téléphone encore attendus.

## Hall local — invariants
- Boss réel 1..200; Danger Boss HOME 1..9; ancien `danger=0` reste `DANGER INCONNU`.
- Points : `|boss*(danger-minutes)| + boss*(danger-minutes)`; tri points décroissant puis temps croissant.
- Sauvegarde campagne v3, v1/v2 toujours lisibles.

## Invariants permanents
- Progression campagne jamais modifiée par le global.
- Campagne, aide/tuto et Danger 9 restent protégés; le seul changement cinétique autorisé est le mini-fix v2 ci-dessus.
- Validation téléphone Fab avant merge/release.
