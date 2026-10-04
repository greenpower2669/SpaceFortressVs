# brain.md — SpaceFortressVs

## État canonique vivant

- Branche courante : `main`.
- Version livrée et validée téléphone par Fab : **v1.4.0**.
- Détails de livraison, SHA, CI, anciens bugs et décisions : `docs/archive/2026-10-spacefortress-v1.4.0-history.md`.
- `src/main.cpp` est la référence historique et reste protégé ; lire le code réel avant toute modification.

## Canon gameplay à préserver

- Campagne : **200 affrontements = 50 boss × 4 difficultés**.
- Énergie historique : `nrj=0` = plein ; `nrj=50` = épuisé.
- Poussière blanche : recharge jusqu'à `nrj=0`, puis soigne la coque.
- Poussière rouge : **visuelle uniquement**, aucun effet gameplay.
- Danger HOME : `MOU DU GENOU` ×1, `CHILL` ×5, `ROCK N ROLL` ×10 par défaut, `DUR A CUIRE` ×15, `MACHINE DE GUERRE` ×20. Il ne s'applique qu'aux attaques/contact **non cinétiques** du boss.
- Astéroïdes : dégâts cinétiques linéaires `masse × vitesse relative × fermeture`, référence maximale 250 PV bruts avant protections, aucun dégât inventé à vitesse nulle.
- Les vagues cinétiques partent du centre vers l'extérieur.
- Surcharge 2 doigts : charge 2 s ; champ ×2 + irisation + son pendant la charge ; après armement le bouclier cinétique est OFF jusqu'au relâchement ; relâchement armé = purge.

## Discipline

- Fab décide et valide sur téléphone ; Astra analyse ; Sol code/teste/build.
- `ordres-de-mission.md` est le contrat de mission actif.
- Garder `brain.md`, `brainmap.md`, `debughistorical.md`, `todo.md` courts et synchronisés.
- Ne remettre dans les mémoires vivantes ni anciens logs, ni longues preuves CI, ni historique résolu : utiliser l'archive ou Git.
- Ne jamais conseiller désinstallation/effacement des données pour contourner une signature Android différente.
