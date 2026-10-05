# todo.md — SpaceFortressVs

## Hall of Fame — Danger Boss réel

- [x] Audit téléphone : confusion prouvée entre difficulté campagne 4 niveaux et Danger Boss HOME 9 niveaux.
- [x] TDD RED : `18dc399588dbf6e5aec9b5e7c44ae301dcd78f0f` / workflow `37330863345`.
- [x] Persistance du Danger Boss réel 1..9 dans les nouvelles victoires ; format sauvegarde v3.
- [x] Compatibilité v1/v2 : ancien danger conservé comme inconnu (`0`), jamais inventé.
- [x] Hall : nom réel du Danger Boss + 1..9 étoiles visibles + boss 1..200 + temps + points.
- [x] Formule Fab : `|boss*(danger-minutes)| + boss*(danger-minutes)` et tri points décroissant / temps croissant.
- [x] GREEN : code/test `47ada1e859e38ec5f09ae5104a0575eb25f08544`, workflow `37332234356`, régressions + APK + AAB + packaging SUCCESS.
- [ ] Fab : vérifier sur téléphone qu’une nouvelle victoire `MOU DU GENOU` affiche 1★ et qu’une nouvelle victoire `APOCALYPSE` affiche 9★ avec des points différents.
- [ ] Si Fab veut réparer les deux anciennes entrées, identifier explicitement laquelle (0:41 ou 0:48) était Apocalypse avant toute migration ciblée.

## Mission poussières cinétiques — validation téléphone

- [x] Branche dédiée `feature/kinetic-dust-impact-v4` créée depuis `76bab7bac571df6f42f4ae76011fc61ab2b0c8ee`.
- [x] TDD RED puis GREEN ; gameplay figé `8f2ee5ed61372f2647ae7284abdf9a0df7d8ebbf`.
- [x] `scripts/test-regressions.sh` + APK + AAB + packaging GREEN sur workflow `37241618498`.
- [ ] Fab : vérifier purge armée 2 s → beaucoup de blanc proportionnel, quasi statique.
- [ ] Fab : vérifier collision astéroïde↔astéroïde → nettement moins de blanc (10 %), projeté dans la course de l’objet détruit.
- [ ] Fab : vérifier destruction par champ normal → même ordre 10 %, sans double émission.
- [ ] Fab : vérifier rouge au champ → impacts locaux jaune/orange/rouge, majorité consumée, quelques survivants repoussés de façon cohérente.
- [ ] Fab : vérifier que le blanc se collecte toujours énergie puis soin et n’est pas repoussé par les vagues.
- [ ] Échantillon téléphone aide/tuto/Danger 9/campagne 200 si souhaité.

## Invariants avant intégration

- [x] Aucun équilibrage des dégâts modifié dans cette mission.
- [x] Rouge sans soin/recharge/dégât/danger.
- [x] `src/main.cpp` inchangé.
- [x] Aucun merge `main`, aucune release.
- [ ] Après validation explicite de Fab seulement : décider merge puis éventuelle release.
