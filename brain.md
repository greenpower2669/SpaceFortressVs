# brain.md — SpaceFortressVs

## État canonique vivant
- Version publiée : `v1.4.1`.
- Release commit/tag : `450423c41c4cef6c348f49af698767016a0528fd`.
- `main` a été aligné sur ce commit exact après publication.
- Release publique vérifiée : APK + AAB + build.json + SHA256SUMS présents.
- `src/main.cpp` historique reste strictement protégé, blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.

## Hall global sync v1 — livré
- Sync locale-first durable, UUID/payload stable, ack/pages/cache/snapshot global+local.
- HTTPS Java + JNI Android, GET `/sync` public, POST avec clé BuildConfig injectée par `SPACEFORTRESS_HOF_API_KEY`.
- Clé jeu jamais dans Git; aucune clé admin dans l'APK.
- Validation téléphone : `SYNC OK` / `GLOBAL 1 + LOCAL 0`.
- Incident `SYNC NON CONFIGUREE` clos : ancien artifact sans clé livré par erreur; le bon artifact configuré a validé la chaîne complète.

## Mini-fix surcharge cinétique v2 — livré
- Appui second doigt <0,30 s : pas de cercle irisé, puissance normale ×1, relâchement court = tir classique.
- 0,30–2,00 s : surcharge visible ×2.
- À 2 s : état armé, champ cinétique OFF jusqu’au relâchement.
- Relâchement armé : purge + vague visuelle sur 3,0 diamètres de vaisseau, avec son EMP original.
- CLASSIQUE + COOP partagent la même logique.
- TDD RED : `37424970512`.
- Code/asset final : `43326ec5ba92d40b2378b0877775bce28d21b1b4`.
- CI de préparation 1.4.1 : `37488788585` GREEN.
- CI sur `main` après merge : `37489724746` GREEN.
- CI de publication : `37490579073` GREEN, job `publish-release` SUCCESS.

## Release 1.4.1
- APK SHA-256 : `4d4f10f324a0b9929397b14f79fb36f4faae9b48a8a027f9ce4fdc864c014da8`.
- AAB SHA-256 : `5e20d2a5bae408237b6a25a06e87623809b4258f48392f745100dd77f34eb087`.
- Version Android : `1.4.1`, versionCode `11`.
- Signature APK différente de la référence v1.3.1 : ne pas désinstaller/effacer les données pour forcer une mise à jour de test.

## Invariants permanents
- Progression campagne jamais modifiée par le global.
- Campagne 200, aide/tuto, Danger 9 et poussières hors périmètre restent protégés.
- Toute prochaine évolution repart de `main` post-v1.4.1 avec validation Fab avant nouvelle Release.
