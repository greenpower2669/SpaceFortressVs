# brainmap.md — SpaceFortressVs

## Carte rapide de reprise — état livré 1.4.0

### Où reprendre

- Dépôt : `greenpower2669/SpaceFortressVs`
- Branche active après livraison : `main`
- Release : `v1.4.0`
- SHA publié : `98da8a01175d5091f487232f65872e92d213b321`
- Merge : `c50088092744d18ce50f3ef484a2d706aa76a276`
- Vérification fraîche `main` : `a30ca6ed4b4679ad9bd69ba8a145f602e9e3cb8b`, workflow `37181486670` GREEN
- Validation téléphone Fab : APK OK le 4/10/2026
- Archive détaillée : `docs/archive/2026-10-spacefortress-v1.4.0-history.md`

## Architecture utile

### Historique protégé
- `src/main.cpp` — source historique ; blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.

### HOME / modes
- `src/game_mode.hpp` — duel / coop.
- `src/boss_danger.hpp` — niveaux HOME et multiplicateurs internes.
- `src/start_ui.hpp` — affichage HOME.
- `src/remaster_ai_fix.hpp` — interception tactile HOME / lancement.

### Campagne 200
- `src/campaign_runtime.hpp` — combat coop campagne, boss, HUD, dégâts.
- `src/boss_catalog.hpp` — 50 boss.
- `src/boss_difficulty_visuals.hpp` — auras / 0-4-8-20 tentacules.
- `src/campaign_save.hpp` — sauvegardes v1/v2, migration, Hall of Fame.

### Énergie
- `src/ship_energy.hpp` — `nrj=0` plein, `nrj=50` épuisé, cadence/précision/bouclier.
- `src/tactical_runtime.hpp` — poussière blanche, HUD, rendu tactique.

### Cinétique
- `src/kinetic_shield.hpp` — modèle partagé, fermeture, couches, surcharge 2 s.
- `src/legacy_field_runtime.hpp` — astéroïdes historiques, collisions, fragmentation, poussières.
- `src/remaster_runtime.hpp` — intégration classique/remaster et audio cinétique.

## Flux canoniques

### Astéroïde
`masse relative` → `vitesse relative` → `fermeture` → max 250 PV bruts → couches cinétiques → bouclier historique → coque.

Jamais de multiplicateur danger boss sur ce flux.

### Boss non cinétique
projectile / rayon / vague / contact ordinaire → `sfBossDangerMultiplier()` → bouclier historique → PV.

### Poussières
- blanche : énergie jusqu'à `nrj=0`, puis soin ; non déviée ;
- rouge : visuelle seulement, aucune mutation gameplay.

### Surcharge
2e doigt maintenu → 0..2 s champ ×2 + arc-en-ciel + son → à 2 s état prêt + champ cinétique OFF → relâchement = purge + son décharge.

## Danger HOME

1. `MOU DU GENOU` ×1
2. `CHILL` ×5
3. `ROCK N ROLL` ×10 — défaut
4. `DUR A CUIRE` ×15
5. `MACHINE DE GUERRE` ×20

Les coefficients ne sont pas affichés. Le tap du sélecteur ne lance jamais le combat.

## Tests / livraison

- `scripts/test-regressions.sh`
- `tests/kinetic_regressions.cpp`
- `tests/kinetic_surge_regressions.cpp`
- `tests/kinetic_balance_v3_regressions.cpp`
- `tests/legacy_field_regressions.cpp`
- `tests/regressions.cpp`
- `scripts/package-release.py`
- `scripts/publish-release.py`

Dernière preuve de `main` : workflow `37181486670`, tout GREEN jusqu'au packaging.

Pour l'historique des bugs, anciennes valeurs, essais et séquences de diagnostic, lire l'archive dédiée plutôt que d'élargir cette carte.
