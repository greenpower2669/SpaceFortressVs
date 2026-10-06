# Archive — SpaceFortressVs HELP / TUTORIEL / DANGER 9 — 2026-10-04

Ce document conserve l’historique technique du lot afin que les mémoires vivantes restent courtes.

## Références

- Branche : `feature/help-tutorial-danger-9-canon`.
- Base : `main` au SHA `703676aaa3ae16a0ef415802dbb9ea24a229e61c`.
- Code téléphone vérifié : `d9520a0b674d7f21df37f982a444d625b523f8d9`.
- Workflow final : `37236262962` (run 268) — régressions, APK, AAB, vérification et packaging GREEN.
- Artefact téléphone : `SpaceFortressVs-1.4.0-release-files`, id `11316375150`, digest `sha256:65d8a536339e48f8b5f95207a082215b0e306289171183f999824f807b1a59c5`.
- Publication de release : volontairement SKIPPED. Aucun merge `main`.
- Comparaison base→code : `src/main.cpp` absent du diff, donc source historique protégée.

## Canon fonctionnel implémenté

### Aide

- Bouton `?` à l’accueil et pendant une partie.
- Trois formats choisis par le joueur : `RAPIDE`, `DETAILLE`, `ANIME`.
- `ANIME` est le défaut et le niveau maximal de détail.
- `RAPIDE` et `DETAILLE` utilisent des schémas fixes ; `ANIME` seul reçoit l’horloge de rendu et anime les schémas.
- Pages : objectif, contrôles, HUD, énergie, PV/protections, poussières, bouclier cinétique, vagues, surcharge 2 doigts, campagne 200, danger.
- Les illustrations réutilisent les textures du jeu lorsqu’elles existent et gardent un fallback procédural.
- Ouvrir l’aide depuis une partie suspend les entrées concernées et consomme le doigt du `?`; fermer reprend la même partie sans reset PV/énergie/position.

### Tutoriel

- Tutoriel séparé du centre d’aide, avec sélecteur de modules ou parcours complet.
- Étapes : déplacement, tir, HUD, énergie, PV/protection, poussières, cinétique, maintien surcharge, état prêt, relâchement, danger.
- Sandbox isolée : aucune modification de progression campagne, sauvegarde, Hall of Fame, listes de projectiles/astéroïdes ou état réel des vaisseaux.
- Retour : TUTORIEL → AIDE → écran d’origine ; si l’aide venait d’une partie, retour à cette même partie.

### Danger 9

Noms visibles ; coefficients internes cachés :

1. `MOU DU GENOU` ×1
2. `CHILL` ×5
3. `ROCK N ROLL` ×10 — défaut
4. `DUR A CUIRE` ×15
5. `MACHINE DE GUERRE` ×20
6. `CA VA PIQUER` ×25
7. `SANS PITIE` ×30
8. `ENFER STELLAIRE` ×35
9. `APOCALYPSE` ×40

Le helper canonique `sfApplyHostileDanger()` s’applique uniquement aux dégâts hostiles non cinétiques concernés. En classique IA, seuls les tirs IA hostiles vers le joueur humain sont multipliés. Le danger ne modifie ni cadence, ni vitesse projectile, ni précision/visée. Astéroïdes, contact/charge cinétique et autres chemins cinétiques restent hors multiplicateur.

## Architecture

- `src/boss_danger.hpp` — table unique des 9 niveaux + helper.
- `src/classic_danger_runtime.hpp` — qualification des tirs IA classiques hostiles.
- `scripts/patch-classic-danger.cmake` — injection uniquement dans la copie Android générée ; `src/main.cpp` intact.
- `src/help_runtime.hpp` — état, contenu, rendu et navigation du centre d’aide.
- `src/help_format_bridge.hpp` — horloge animée uniquement en mode `ANIME`.
- `src/tutorial_runtime.hpp` — sandbox et parcours guidé.
- `src/help_live_bridge.hpp` — interception finale des événements/rendus, suspension/reprise et pile AIDE↔TUTORIEL.
- `src/campaign_runtime.hpp` — attaques boss non cinétiques routées par le helper ; cinétique explicitement exclu.

## TDD — incidents utiles

- Le premier test 9 niveaux a été posé rouge avant la table canonique.
- Le `?` en partie était d’abord ignoré/intercepté par les handlers historiques : ajout d’un bridge final consommant le doigt et préservant l’état vivant.
- Le tutoriel surcharge plafonnait chaque `dt` à 0,25 s : un maintien réel de 1,99 s ne comptait donc que 0,25 s. Correction : compter le temps réel jusqu’au seuil 2 s.
- L’intégration AIDE→TUTORIEL était absente : test rouge de pile HOME/JEU → AIDE → TUTORIEL → AIDE → origine, puis branchement.
- Deux anciens gardes Python recherchaient l’expression historique `sfBossDangerMultiplier()*damage`; ils ont été remplacés par des assertions plus fortes : helper obligatoire sur non-cinétique, interdit sur cinétique.
- Dernier rouge : `RAPIDE`/`DETAILLE` animaient les mêmes schémas que `ANIME`. Test explicite puis `help_format_bridge.hpp` pour figer l’horloge hors `ANIME`.

## Preuve finale

Workflow `37236262962` au code SHA `d9520a0b674d7f21df37f982a444d625b523f8d9` :

- validation assets + régressions : SUCCESS ;
- build Android APK + AAB : SUCCESS ;
- vérification/nommage : SUCCESS ;
- artefacts : SUCCESS ;
- `publish-release` : SKIPPED.

## Validation téléphone restante

Fab doit encore vérifier physiquement :

- `?` HOME et en jeu ;
- RAPIDE et DETAILLE fixes, ANIME réellement animé ;
- parcours tutoriel et seuil surcharge 2 s ;
- reprise exacte d’une partie après aide/tuto ;
- les 9 noms de danger et `ROCK N ROLL` par défaut ;
- dégâts IA classiques faibles/forts selon danger, sans modifier les tirs humains ;
- comportement cinétique identique quel que soit le danger.

Aucun merge `main` ni aucune release avant validation téléphone et accord explicite de Fab.
