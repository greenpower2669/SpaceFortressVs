# debughistorical.md — SpaceFortressVs

## Pièges prouvés / à préserver
- Hall : ne jamais confondre difficulté campagne 4 niveaux et Danger Boss HOME 1..9.
- Anciennes entrées v1/v2 : `danger=0` = inconnu; ne jamais l'inventer ni l'envoyer.
- Sync : save campagne et état réseau séparés; global ne modifie jamais `cleared/selected/pending`.
- Sync : `submissionId` immuable après création; retry réutilise l'identité.
- Sync : page vide + `hasMore=true` doit persister `nextCursor` puis continuer.
- Sync : callback vieux cycle/cursor ignoré; erreur réseau/auth/5xx conserve pending/cache/cursor.
- Clés : aucune vraie clé de jeu dans Git/logs/tests/mémoires; aucune clé admin dans l'APK.
- Livraison Actions : plusieurs artifacts peuvent avoir le même nom; toujours identifier tentative/ID et vérifier SHA-256.
- Incident 2026-10-06 `SYNC NON CONFIGUREE` : ancien APK sans clé remis au téléphone; code innocent. Le bon APK configuré a donné `SYNC OK`.
- `src/main.cpp` historique ne doit jamais être modifié; blob canonique `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- Surcharge v2 : champ normal max 2,0 diamètres; seul le blast armé à 2 s purge/affiche 3,0 diamètres.
- Avant 0,30 s : seconde touche visuellement classique et sans multiplicateur ×2.
- À 2 s : conserver la fenêtre vulnérable champ OFF jusqu’au relâchement.
- EMP : transport base64 contrôlé par checksum puis asset Android `kinetic_release.wav`.
- Incident checksum EMP : échec CI dû au checksum de transport, pas au gameplay; correctif minimal au SHA `43326ec5ba92d40b2378b0877775bce28d21b1b4`.

## Clôture v1.4.1 — 2026-10-06
- Fab a validé physiquement le lot puis autorisé explicitement `release et merge main`.
- `v1.4.0` existait déjà : incrément propre vers `v1.4.1` / versionCode 11, sans écraser l’ancienne Release.
- PR #5 mergée vers `main`, puis Release `v1.4.1` publiée.
- Workflow publication `37490579073` : build Android GREEN + `publish-release` SUCCESS.
- Release commit/tag : `450423c41c4cef6c348f49af698767016a0528fd`.
- `main` aligné ensuite sur ce commit exact, sans différence de fichiers.
- APK SHA-256 : `4d4f10f324a0b9929397b14f79fb36f4faae9b48a8a027f9ce4fdc864c014da8`.
- AAB SHA-256 : `5e20d2a5bae408237b6a25a06e87623809b4258f48392f745100dd77f34eb087`.
- Ne jamais remplacer les assets de cette Release par d’autres bytes sous le même tag/version.
