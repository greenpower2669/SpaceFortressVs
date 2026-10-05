# brain.md — SpaceFortressVs

## État canonique vivant

- Base livrée : **v1.4.0** sur `main` ; aucun merge/release sans validation téléphone de Fab.
- Branche téléphone : `feature/kinetic-dust-impact-v4`.
- Gameplay poussières figé : `8f2ee5ed61372f2647ae7284abdf9a0df7d8ebbf` ; workflow Android `37241618498` GREEN.
- Hall of Fame Danger Boss réel : code/test validé `47ada1e859e38ec5f09ae5104a0575eb25f08544` ; workflow `37332234356` GREEN.
- `src/main.cpp` reste historique et protégé ; blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- Archives : `docs/archive/2026-10-05-kinetic-dust-impact-v4.md` et `docs/archive/2026-10-05-hall-of-fame-danger9.md`.

## Hall of Fame — canon vivant

- Le **Danger Boss HOME** est un système 1..9 distinct de la difficulté de campagne 4 niveaux.
- Une nouvelle victoire stocke le Danger Boss réel choisi, de `MOU DU GENOU` (1★) à `APOCALYPSE` (9★).
- Sauvegarde campagne : format v3 ; v1/v2 restent lisibles. Une ancienne entrée sans danger sauvegardé garde `danger=0` / `DANGER INCONNU` : ne jamais inventer sa difficulté.
- Points : `|boss*(danger-minutes)| + boss*(danger-minutes)`, avec minutes calculées à partir des secondes enregistrées.
- Classement : points décroissants ; à égalité, temps le plus court.
- Affichage : boss réel 1..200, nom du Danger Boss, 1..9 étoiles visibles, temps, points.

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
