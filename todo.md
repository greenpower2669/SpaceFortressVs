# todo.md — SpaceFortressVs

## État après livraison 1.4.0 — 4 octobre 2026

### Clôturé

- [x] Campagne 200 livrée.
- [x] Kinetic balance v3 livré.
- [x] Danger boss HOME par noms livré.
- [x] Poussière rouge visuelle uniquement.
- [x] Poussière blanche : énergie puis soin.
- [x] Vagues centre→extérieur.
- [x] Surcharge deux doigts 2 s + vulnérabilité après armement + purge au relâchement.
- [x] Release publique `v1.4.0` publiée depuis `98da8a01175d5091f487232f65872e92d213b321`.
- [x] Merge réel à deux parents dans `main` : `c50088092744d18ce50f3ef484a2d706aa76a276`.
- [x] `main` vérifié GREEN sur `a30ca6ed4b4679ad9bd69ba8a145f602e9e3cb8b`, workflow `37181486670`.
- [x] APK confirmé OK sur téléphone par Fab.
- [x] Historique détaillé déplacé dans `docs/archive/2026-10-spacefortress-v1.4.0-history.md`.

## Pour la prochaine mission

- [ ] Partir de `main` sauf ordre contraire de Fab.
- [ ] Lire `brain.md` + `brainmap.md` avant toute modification.
- [ ] Lire l'archive uniquement si un ancien bug/décision doit être retrouvé.
- [ ] Créer une branche dédiée avant nouveau code conséquent.
- [ ] Ne pas modifier `src/main.cpp` historique sans preuve qu'aucune autre couche ne peut porter le correctif.

## Invariants

- [ ] Ne pas appliquer le danger boss aux chemins cinétiques.
- [ ] Ne pas redonner de gameplay à la poussière rouge.
- [ ] Conserver `nrj=0` plein / `nrj=50` épuisé.
- [ ] Ne pas conseiller désinstallation/effacement des données pour contourner la signature Android.
- [ ] Toute future release doit publier uniquement les binaires dont le `-build.json` pointe vers son SHA exact.
