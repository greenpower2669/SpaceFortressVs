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
- [ ] Task 8 : attendre CI complète, corriger uniquement les échecs prouvés, vérifier `main.cpp`/secret, récupérer APK/AAB et finaliser docs.

## Validation téléphone encore ouverte
- [ ] Hall Danger 1★/9★ sur nouvelles victoires.
- [ ] Sync réelle serveur avec APK configuré.
- [ ] Effets poussières cinétiques.

## Invariants
- [x] `src/main.cpp` protégé.
- [x] Aucun merge main / aucune release.
