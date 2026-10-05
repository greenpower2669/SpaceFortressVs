# brainmap.md — SpaceFortressVs

## Reprise rapide
- Branche : `feature/hall-of-fame-global-sync-v1`.
- Spec : `docs/superpowers/specs/2026-10-05-hall-of-fame-global-sync-design.md`.
- Plan : `docs/superpowers/plans/2026-10-05-hall-of-fame-global-sync.md`.

## Flux réseau vivant
- Victoire -> fame locale durable -> reconcile -> UUID stable -> pending -> upload auto si transport disponible.
- Upload 201/200 -> ack + serverId durable; erreurs -> pending conservé.
- Ouverture Hall -> retry pending + un seul cycle `/sync` depuis le cursor durable, pages de 100.
- `/sync` -> Java HTTPS -> JNI callback typé -> stage/validation -> merge durable -> cursor -> page suivante si `hasMore`.
- Hall -> snapshot en lecture seule : global cache + locaux, dédup serverId/submissionId, pending local conservé hors ligne.

## Fichiers
- `src/hall_sync*.hpp` : modèle, stockage, runtime, transport, hook événements.
- `src/hall_of_fame_runtime.hpp` : rendu fusionné global + téléphone, aucune opération réseau.
- `android/.../HallOfFameSyncProtocol.java` : JSON protocole serveur.
- `android/.../HallOfFameSyncClient.java` : HTTPS async, timeouts, classification erreurs.
- `android/app/src/main/cpp/hall_sync_jni.cpp` : pont natif/Java, aucune logique JSON.
- `tests/hall_sync_regressions.cpp` + test Java : TDD natif/protocole.
- Reste : validation CI/package/docs finales.

## Protections
- `src/main.cpp` lecture seule; save campagne séparée du réseau.
- `danger=0` non envoyé; global ne modifie jamais progression.
- Vraie clé absente de Git/tests/logs; BuildConfig lit uniquement `SPACEFORTRESS_HOF_API_KEY`.
