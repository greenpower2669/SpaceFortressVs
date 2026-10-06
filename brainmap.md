# brainmap.md — SpaceFortressVs

## Reprise rapide
- Canon actuel : `main` après Release `v1.4.1`.
- Commit Release : `450423c41c4cef6c348f49af698767016a0528fd`.
- `src/main.cpp` lecture seule, blob protégé `835059a0ecfe0f74708068b3259cad5db1cdb579`.

## Hall global — livré et validé
- Victoire -> Hall local durable -> reconcile -> UUID stable -> pending -> upload auto.
- Upload 201/200 -> ack + serverId durable; erreurs -> pending conservé.
- Ouverture Hall -> retry pending + pagination `/sync` depuis cursor durable.
- Hall -> snapshot global cache + local, dédup serverId/submissionId.
- Téléphone : `SYNC OK`, `GLOBAL 1 + LOCAL 0`.
- Secret injecté uniquement au build via `SPACEFORTRESS_HOF_API_KEY`; jamais dans Git.

## Surcharge cinétique v2 — livrée
- 0–<0,30 s : comportement classique, aucun cercle irisé.
- 0,30–<2,00 s : surcharge ×2 visible.
- >=2,00 s : armé, champ OFF jusqu’au relâchement.
- Relâchement armé : blast/purge 3,0× + son EMP.
- Champ normal reste maximum 2,0 diamètres.
- CLASSIQUE + COOP utilisent le même état.

## Preuves release
- Préparation 1.4.1 : workflow `37488788585` GREEN.
- `main` après merge : workflow `37489724746` GREEN.
- Publication : workflow `37490579073` GREEN, `publish-release` SUCCESS.
- APK : `4d4f10f324a0b9929397b14f79fb36f4faae9b48a8a027f9ce4fdc864c014da8`.
- AAB : `5e20d2a5bae408237b6a25a06e87623809b4258f48392f745100dd77f34eb087`.

## Protection de reprise
- Ne jamais écraser `v1.4.1` ni ses assets.
- Toute nouvelle mission doit partir du `main` courant et créer une branche dédiée.
- Aucun changement Hall/campagne/cinétique hors ordre explicite Fab.
