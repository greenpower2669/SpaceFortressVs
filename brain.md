# brain.md — SpaceFortressVs

## État canonique vivant

- Base livrée : **v1.4.0** sur `main`.
- Lot en validation téléphone : `feature/help-tutorial-danger-9-canon`.
- Code APK/AAB vérifié : `d9520a0b674d7f21df37f982a444d625b523f8d9`, workflow `37236262962` GREEN.
- Aucun merge `main` ni release de ce lot avant validation de Fab.
- Historique du lot : `docs/archive/2026-10-spacefortress-help-tutorial-danger9-history.md`.
- `src/main.cpp` reste historique et protégé.

## Canon gameplay

- Campagne : **200 affrontements = 50 boss × 4 difficultés**.
- Énergie : `nrj=0` plein ; `nrj=50` épuisé.
- Blanche : recharge jusqu’à `nrj=0`, puis soigne. Rouge : visuelle uniquement.
- Danger HOME : `MOU DU GENOU` ×1, `CHILL` ×5, `ROCK N ROLL` ×10 défaut, `DUR A CUIRE` ×15, `MACHINE DE GUERRE` ×20, `CA VA PIQUER` ×25, `SANS PITIE` ×30, `ENFER STELLAIRE` ×35, `APOCALYPSE` ×40. Le joueur voit les noms, pas les coefficients.
- Danger = dégâts hostiles **non cinétiques uniquement**, y compris tirs IA hostiles en classique. Jamais cadence/vitesse/précision, jamais astéroïdes/cinétique.
- Cinétique v3 et surcharge : maintien 2 s, champ ×2 irisé pendant charge ; armé = champ OFF jusqu’au relâchement ; relâchement armé = purge.

## Aide / tuto

- `?` à l’accueil et en jeu ; retour à la même partie sans reset.
- `RAPIDE`, `DETAILLE`, `ANIME` ; `ANIME` par défaut et seul format animé.
- Tutoriel séparé, modules ou parcours complet, sandbox sans progression/sauvegarde.

## Discipline

- Fab valide sur téléphone ; aucun merge/release avant son accord explicite.
- Charger d’abord seulement `brain.md` + `brainmap.md` ; historique détaillé dans l’archive.
- Garder les 4 mémoires vivantes courtes et synchronisées.
- Ne jamais contourner une signature Android différente par désinstallation/effacement des données.
