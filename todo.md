# todo.md — SpaceFortressVs

## Hall global sync v1
- [x] Spec architecture validée par Fab.
- [x] Plan TDD validé en exécution Native.
- [x] Branche dédiée `feature/hall-of-fame-global-sync-v1`.
- [x] Task 1 RED→GREEN ciblé : modèle/codec + stockage atomique sync.
- [x] Task 2 RED→GREEN ciblé : UUID stable, réconciliation et payload typé.
- [ ] Task 2/4 : brancher les hooks automatiques post-victoire + ouverture Hall.
- [ ] Task 3 pages/cache/déduplication/snapshot offline.
- [ ] Task 4 orchestration automatique + faux transport.
- [ ] Task 5 client HTTPS Java + clé BuildConfig + tests.
- [ ] Task 6 pont JNI.
- [ ] Task 7 Hall global + local visible.
- [ ] Task 8 CI complète, APK/AAB, scan clé, docs finales.
- [ ] CI PR : valider la suite complète dès qu'un runner prend le workflow.

## Validation téléphone encore ouverte
- [ ] Hall Danger 1★/9★ sur nouvelles victoires.
- [ ] Effets poussières cinétiques.

## Invariants
- [x] `src/main.cpp` protégé.
- [x] Aucun merge main / aucune release.
