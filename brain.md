# brain.md — SpaceFortressVs

## État canonique vivant

- Base livrée : **v1.4.0** sur `main` ; aucun merge/release sans validation téléphone de Fab.
- Branche téléphone : `feature/kinetic-dust-impact-v4`.
- SHA gameplay figé : `8f2ee5ed61372f2647ae7284abdf9a0df7d8ebbf` ; workflow Android `37241618498` GREEN.
- `src/main.cpp` reste historique et protégé.
- Détails de cette mission : `docs/archive/2026-10-05-kinetic-dust-impact-v4.md`.

## Canon cinétique / poussières

- Cinétique v3 et surcharge inchangés : maintien 2 s, champ ×2 pendant charge ; armé = champ OFF jusqu’au relâchement ; relâchement armé = purge.
- Destruction par purge 2 s : poussière blanche proportionnelle à la taille/masse, rendement 100 %, vitesse initiale nulle.
- Destruction astéroïde↔astéroïde ou champ cinétique normal : rendement blanc 10 %, proportionnel, projeté selon le vecteur incident avec faible dispersion.
- Une destruction réelle produit une seule émission blanche ; le minage historique reste séparé.
- Poussière blanche : recharge jusqu’à `nrj=0`, puis soigne ; jamais déviée par les vagues cinétiques.
- Poussière rouge : visuelle uniquement ; au champ, micro-flash local jaune→orange→rouge, majorité consumée, petite fraction survivante déviée par incident + réaction du champ.

## Invariants

- Campagne 200, aide/tuto et Danger 9 restent protégés par régressions.
- Danger HOME ne s’applique jamais au cinétique.
- Aucun nouveau soin, recharge, dégât ou multiplicateur via poussière rouge.
- Validation finale = téléphone Fab ; aucun merge/release avant accord explicite.
