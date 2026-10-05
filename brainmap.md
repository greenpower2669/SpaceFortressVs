# brainmap.md — SpaceFortressVs

## Reprise rapide
- Branche : `feature/hall-of-fame-global-sync-v1`.
- Spec : `docs/superpowers/specs/2026-10-05-hall-of-fame-global-sync-design.md`.
- Plan : `docs/superpowers/plans/2026-10-05-hall-of-fame-global-sync.md`.
- HEAD code validé avant audit livraison : `c48d7b1af04139ef9e6f8bd3c28fbbfff616759b`.

## Flux réseau vivant
- Victoire -> fame locale durable -> reconcile -> UUID stable -> pending -> upload auto si transport disponible.
- Upload 201/200 -> ack + serverId durable; erreurs -> pending conservé.
- Ouverture Hall -> retry pending + un seul cycle `/sync` depuis le cursor durable, pages de 100.
- `/sync` -> Java HTTPS -> JNI callback typé -> stage/validation -> merge durable -> cursor -> page suivante si `hasMore`, y compris si `entries=[]`.
- Hall -> snapshot en lecture seule : global cache + locaux, dédup serverId/submissionId, pending local conservé hors ligne.

## Validation finale
- CI GREEN : workflow `37372279557`, tentative 2.
- Build sans clé obligatoire pour prouver le mode local/cache-only.
- Scan `scripts/check-hall-secret.py` : fichier privé interdit dans Git et vraie clé recherchée sans jamais être imprimée.
- Build téléphone configuré : artifact `11374640381`, APK SHA-256 `0ca76a074b8148578b95a5b10a1fd32fad41d1c27cb46454ff2cd15d2133ab87`.
- Ancien artifact sans clé à ne plus livrer : `11369888063`, APK SHA-256 `d7a24c1f1f50b569f45bb79cf2ad75ef2be834cd9d8ed5a95b38fe6edb0c72fe`.

## Audit téléphone 2026-10-06
- Symptôme observé sur APK livré : `SYNC NON CONFIGUREE`, local/pending OK.
- Cause : mauvais artifact remis au test téléphone, pas perte de configuration dans le code.
- Prochaine action : installer l’APK exact de l’artifact `11374640381`, faire une vraie victoire puis vérifier POST, GET `/sync`, ack du pending et absence de doublon.
- Pas de rebuild ni patch réseau avant ce re-test.

## Protections
- `src/main.cpp` lecture seule; save campagne séparée du réseau.
- `danger=0` non envoyé; global ne modifie jamais progression.
- Vraie clé absente de Git/tests/logs; aucune clé admin dans l’APK.
