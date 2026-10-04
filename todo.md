# todo.md — SpaceFortressVs

## État

- [x] v1.4.0 livrée, fusionnée dans `main`, revalidée et confirmée OK sur téléphone par Fab.
- [x] Historique du lot archivé dans `docs/archive/2026-10-spacefortress-v1.4.0-history.md`.

## Prochaine mission

- [ ] Partir de `main` sauf ordre contraire de Fab.
- [ ] Lire seulement `brain.md` + `brainmap.md` au démarrage ; ouvrir `debughistorical.md` ou l'archive uniquement si nécessaire.
- [ ] Créer une branche dédiée avant nouveau code conséquent.
- [ ] Synchroniser les 4 mémoires vivantes sans y recopier logs, CI ou historique résolu.

## Invariants

- [ ] `src/main.cpp` historique protégé.
- [ ] `nrj=0` plein / `nrj=50` épuisé.
- [ ] Danger boss jamais appliqué au cinétique.
- [ ] Poussière rouge sans gameplay.
- [ ] Ne jamais contourner une signature Android différente par effacement des données.
