# brainmap.md — SpaceFortressVs

## Reprise rapide
- Branche : `feature/hall-of-fame-global-sync-v1`.
- Spec Hall : `docs/superpowers/specs/2026-10-05-hall-of-fame-global-sync-design.md`.
- Plan Hall : `docs/superpowers/plans/2026-10-05-hall-of-fame-global-sync.md`.
- `src/main.cpp` reste lecture seule.
- Fab a validé le lot sur téléphone et a autorisé explicitement le 2026-10-06 : `release et merge main`.
- Préparation Release : `1.4.1` / `versionCode 11`.

## Hall global — validé téléphone
- Victoire -> fame locale durable -> reconcile -> UUID stable -> pending -> upload auto si transport disponible.
- Upload 201/200 -> ack + serverId durable; erreurs -> pending conservé.
- Ouverture Hall -> retry pending + un seul cycle `/sync` depuis le cursor durable, pages de 100.
- `/sync` -> Java HTTPS -> JNI callback typé -> stage/validation -> merge durable -> cursor -> page suivante si `hasMore`, y compris si `entries=[]`.
- Hall -> snapshot en lecture seule : global cache + locaux, dédup serverId/submissionId, pending local conservé hors ligne.
- CI : workflow `37372279557`, tentative 2, artifact configuré `11374640381`.
- Téléphone 2026-10-06 : `SYNC OK`, `GLOBAL 1 + LOCAL 0`; chaîne globale validée.
- Incident de livraison clos : ne jamais reprendre l’ancien artifact sans clé `11369888063`.

## Mini-fix surcharge cinétique v2 — VALIDÉ / AUTORISÉ RELEASE
Flux commun CLASSIQUE + COOP :
- second doigt 0–<0,30 s -> puissance normale ×1, aucun cercle irisé, relâchement court = tir historique ;
- 0,30–<2,00 s -> surcharge x2 et cercle irisé ;
- >=2,00 s -> armé, champ cinétique OFF jusqu’au relâchement ;
- relâchement armé -> purge des astéroïdes et vague visuelle sur 3,0 diamètres de vaisseau ;
- purge conserve 100 % de poussière blanche ; blanc reste physiquement intangible ;
- `kinetic_release_emp.b64` -> `prepare-assets.py` -> `kinetic_release.wav` EMP pour l’APK.
- TDD RED : workflow `37424970512`.
- Code/asset final : `43326ec5ba92d40b2378b0877775bce28d21b1b4`.
- GREEN Android initial : workflow `37427270384` / run 320.
- GREEN frais du HEAD documentaire : workflow `37476092719` / run 321 au SHA `48eeb687d8cb60c91b28e70587b00fae3d8fe1ab`.
- Téléphone : validation Fab acquise ; merge/release explicitement autorisés.
- `src/main.cpp` : blob protégé `835059a0ecfe0f74708068b3259cad5db1cdb579`.

## Protections
- Normal field max reste 2,0 diamètres; seul le blast armé passe à 3,0.
- Campagne, Hall, Danger Boss, poussières hors purge et progression restent inchangés.
- Vraie clé absente de Git/tests/logs; aucune clé admin dans l’APK.
- La Release 1.4.1 ne doit partir qu’après une CI fraîche du commit de préparation version/notes, puis merge vers `main`.
