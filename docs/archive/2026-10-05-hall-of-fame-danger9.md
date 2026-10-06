# Hall of Fame — Danger Boss réel 1..9 — 2026-10-05

## Constat téléphone
Le premier Hall de Fame affichait `VIF`, une micro-étoile et `0 PTS` pour deux victoires BOSS 1 pourtant jouées avec deux Danger Boss différents (`MOU DU GENOU` et `APOCALYPSE`).

## Cause racine
Deux systèmes distincts avaient été confondus :
- difficulté de campagne, 4 tranches (`VIF`, `ENDURANT`, `VICIEUX`, `ULTIME`) dérivées de l’affrontement ;
- Danger Boss HOME, 9 niveaux de `MOU DU GENOU` à `APOCALYPSE`.

Le format de sauvegarde v1/v2 ne stockait pas le Danger Boss. Il était donc impossible de reconstruire honnêtement cette donnée pour une ancienne victoire.

## Correction canonique
- `SfFameEntry.danger` stocke le Danger Boss réel 1..9 au moment de la création de la victoire.
- format campagne v3 ; lecture v1/v2 conservée, avec `danger=0` pour l’historique inconnu ; aucune valeur n’est inventée ;
- Hall : nom réel du Danger Boss + 1..9 étoiles visibles ; ancien historique = `DANGER INCONNU` ;
- points : `|boss*(danger-minutes)| + boss*(danger-minutes)`, minutes évaluées à partir des secondes enregistrées ;
- classement : points décroissants, puis temps le plus court ;
- `src/main.cpp` inchangé.

## TDD / preuve
- RED : commit `18dc399588dbf6e5aec9b5e7c44ae301dcd78f0f`, workflow `37330863345`, échec attendu car `danger` et les nouvelles signatures n’existaient pas.
- Le premier GREEN candidat a révélé uniquement un fixture invalide : remise à zéro de `victory` avec ID=0, refusé par le format existant. Le test a été corrigé sans changement de logique.
- GREEN final code/test : `47ada1e859e38ec5f09ae5104a0575eb25f08544`, workflow `37332234356`, régressions + APK + AAB + packaging SUCCESS ; `publish-release` SKIPPED.
- Artefact : `SpaceFortressVs-1.4.0-release-files`, id `11355525398`, digest `sha256:40d1195b7a24b92cb2849eff152c48650b3b933e0a8f57af6338e27f604b1eba`.

Aucun merge `main`, aucune release. Validation téléphone Fab requise.
