# brainmap.md — SpaceFortressVs

## Carte rapide de reprise

### Base livrée 1.4.0

- Dépôt : `greenpower2669/SpaceFortressVs`
- Release : `v1.4.0`
- SHA publié : `98da8a01175d5091f487232f65872e92d213b321`
- Merge : `c50088092744d18ce50f3ef484a2d706aa76a276`
- Vérification fraîche `main` : `a30ca6ed4b4679ad9bd69ba8a145f602e9e3cb8b`, workflow `37181486670` GREEN
- Validation téléphone Fab : APK OK le 4/10/2026
- Archive détaillée : `docs/archive/2026-10-spacefortress-v1.4.0-history.md`

### Mission active

- Branche : `feature/in-game-help-tutorial-danger-9`
- Spec : `docs/superpowers/specs/2026-10-04-in-game-help-tutorial-danger-9-design.md`
- État : design écrit, avant plan d'implémentation/code.

## Architecture utile existante

### Historique protégé
- `src/main.cpp` — source historique ; blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.

### HOME / modes / aide
- `src/game_mode.hpp` — duel / coop.
- `src/boss_danger.hpp` — actuellement 5 niveaux ; doit devenir source unique du profil 9 niveaux.
- `src/start_ui.hpp` — HOME + aide statique actuelle ; futur point d'entrée du centre `?`.
- `src/remaster_ai_fix.hpp` — interception tactile HOME/jeu ; futur routage pause/aide.

### Mission prévue
- nouveau runtime aide : RAPIDE / DÉTAILLÉ / ANIMÉ ;
- nouveau runtime tutoriel sandbox ;
- profil danger 9 niveaux partagé ;
- adaptation IA classique : anticipation au départ seulement, projectile droit ensuite ;
- adaptation coop/campagne pour cadence/vitesse/anticipation non cinétiques.

### Campagne 200
- `src/campaign_runtime.hpp` — combat coop campagne, boss, HUD, dégâts.
- `src/boss_catalog.hpp` — 50 boss.
- `src/boss_difficulty_visuals.hpp` — auras / 0-4-8-20 tentacules.
- `src/campaign_save.hpp` — sauvegardes v1/v2, migration, Hall of Fame.

### Énergie
- `src/ship_energy.hpp` — `nrj=0` plein, `nrj=50` épuisé.
- `src/tactical_runtime.hpp` — poussière blanche, HUD, rendu tactique.

### Cinétique
- `src/kinetic_shield.hpp` — modèle partagé, fermeture, couches, surcharge 2 s.
- `src/legacy_field_runtime.hpp` — astéroïdes historiques, collisions, fragmentation, poussières.
- `src/remaster_runtime.hpp` — intégration classique/remaster et audio cinétique.

## Danger canon pour la mission

1. `MOU DU GENOU` ×1
2. `CHILL` ×5
3. `ROCK N ROLL` ×10
4. `DUR A CUIRE` ×15
5. `MACHINE DE GUERRE` ×20
6. `SANS PITIE` ×25
7. `CAUCHEMAR` ×30
8. `APOCALYPSE` ×35
9. `FIN DU MONDE` ×40 — défaut

HOME affiche les noms, pas les coefficients. Cadence max prévue ×2.50, vitesse projectile max ×1.80. Les tirs classiques ordinaires anticipent avant création puis conservent un vecteur constant.

## Flux canoniques à préserver

### Astéroïde
`masse relative` → `vitesse relative` → `fermeture` → max 250 PV bruts → couches cinétiques → bouclier historique → coque.

Jamais de multiplicateur danger sur ce flux.

### Poussières
- blanche : énergie jusqu'à `nrj=0`, puis soin ;
- rouge : visuelle seulement.

### Surcharge
2e doigt maintenu → 0..2 s champ ×2 + arc-en-ciel + son → à 2 s état prêt + champ cinétique OFF → relâchement = purge + son décharge.

## Tests / livraison

- `scripts/test-regressions.sh`
- régressions cinétiques + legacy field + campagne
- futurs tests dédiés aide / tutoriel / danger / interception IA

Aucun merge/release avant validation complète de Fab.
