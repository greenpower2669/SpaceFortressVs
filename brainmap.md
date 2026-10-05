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

## Validation finale
- CI : regressions C++ + tests Java + compile Java.
- Build sans clé obligatoire pour prouver le mode local/cache-only.
- Scan `scripts/check-hall-secret.py` : fichier privé interdit dans Git et vraie clé recherchée sans jamais être imprimée.
- Build téléphone : secret Actions `SPACEFORTRESS_HOF_API_KEY` injecté seulement au Gradle final s’il existe.

## Protections
- `src/main.cpp` lecture seule; save campagne séparée du réseau.
- `danger=0` non envoyé; global ne modifie jamais progression.
- Vraie clé absente de Git/tests/logs; aucune clé admin dans l’APK.
