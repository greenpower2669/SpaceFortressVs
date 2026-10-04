# todo.md — SpaceFortressVs

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
- [x] Aucun merge `main`, aucune release.
- [ ] Après validation explicite de Fab seulement : décider merge puis éventuelle release.
