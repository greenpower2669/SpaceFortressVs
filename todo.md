# todo.md — SpaceFortressVs

## Hall global sync v1
- [x] Spec + plan TDD validés; branche dédiée.
- [x] Task 1 RED→GREEN ciblé : état/codec + stockage atomique.
- [x] Task 2 RED→GREEN ciblé : UUID stable + réconciliation + payload.
- [x] Task 3 RED→GREEN ciblé : ack/errors + pages/cursor + snapshot global/local.
- [x] Task 4 hooks automatiques post-victoire/Hall + faux transport + anti-concurrence.
- [x] Task 5 client HTTPS Java + clé BuildConfig + tests protocole.
- [x] Task 6 pont JNI Android, transport natif ↔ Java.
- [x] Task 7 Hall fusionné global + local, statuts sync, aucun réseau au rendu.
- [x] Task 8 : CI complète GREEN sur workflow `37372279557`, tentative 2; artifact configuré `11374640381`; secret absent de Git; `src/main.cpp` protégé.

## Incident livraison 2026-10-06
- [x] Cause `SYNC NON CONFIGUREE` identifiée : APK sans clé de l’artifact précédent livré par erreur au téléphone.
- [x] Bon APK récupéré depuis artifact `11374640381`, SHA-256 `0ca76a074b8148578b95a5b10a1fd32fad41d1c27cb46454ff2cd15d2133ab87`.
- [ ] Fab installe ce bon APK et refait une vraie victoire.
- [ ] Vérifier score local conservé + POST accepté + pending acquitté.
- [ ] Rouvrir Hall et vérifier GET `/sync`, global > 0 si données serveur compatibles, absence de doublon.
- [ ] Vérifier que la progression personnelle reste inchangée.

## Validation téléphone encore ouverte
- [ ] Hall Danger 1★/9★ sur nouvelles victoires.
- [ ] Sync réelle serveur avec APK configuré.
- [ ] Effets poussières cinétiques.

## Invariants
- [x] `src/main.cpp` protégé.
- [x] Aucun merge main / aucune release.
