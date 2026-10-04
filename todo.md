# todo.md — SpaceFortressVs

## Lot HELP / TUTORIEL / DANGER 9

- [x] Code et régressions terminés sur `feature/help-tutorial-danger-9-canon`.
- [x] APK + AAB vérifiés au SHA `d9520a0b674d7f21df37f982a444d625b523f8d9`, workflow `37236262962` GREEN.
- [x] `src/main.cpp` absent du diff.
- [ ] Fab : test téléphone du `?`, des 3 formats, du tuto, de la reprise d’une partie et des 9 dangers.
- [ ] Vérifier sur téléphone que le danger classique modifie les tirs IA hostiles mais jamais le cinétique.
- [ ] Après validation explicite de Fab seulement : décider merge `main` puis éventuelle release.

## Invariants

- [ ] `ROCK N ROLL` reste le défaut ×10 interne ; coefficients cachés aux joueurs.
- [ ] Danger = dégâts non cinétiques uniquement ; aucune modification cadence/vitesse/visée.
- [ ] `nrj=0` plein / `nrj=50` épuisé ; rouge sans gameplay.
- [ ] `src/main.cpp` historique protégé.
