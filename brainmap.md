# brainmap.md — SpaceFortressVs

## Reprise rapide

- Branche : `main`.
- Release téléphone validée : **v1.4.0**.
- Historique complet : `docs/archive/2026-10-spacefortress-v1.4.0-history.md`.

## Fichiers utiles

- `src/main.cpp` — historique protégé.
- `src/campaign_runtime.hpp` — campagne/coop, boss, HUD, dégâts.
- `src/campaign_save.hpp` — sauvegardes v1/v2 et Hall of Fame.
- `src/boss_catalog.hpp` / `src/boss_difficulty_visuals.hpp` — 50 boss et difficultés.
- `src/ship_energy.hpp` — énergie, cadence, précision, bouclier historique.
- `src/boss_danger.hpp` / `src/start_ui.hpp` — danger HOME.
- `src/kinetic_shield.hpp` — physique cinétique + surcharge 2 s.
- `src/legacy_field_runtime.hpp` — astéroïdes, collisions, fragmentation, poussières.
- `src/tactical_runtime.hpp` / `src/remaster_runtime.hpp` — collecte, effets, rendu/audio.

## Flux canoniques

- Astéroïde → masse × vitesse relative × fermeture → couches cinétiques → bouclier historique → coque. **Jamais de multiplicateur danger boss**.
- Boss non cinétique → danger HOME → bouclier historique → PV.
- Blanche → énergie jusqu'à `nrj=0`, puis soin.
- Rouge → visuel seulement.
- Surcharge → maintien 2e doigt 2 s → champ ×2 irisé → armé = champ OFF → relâchement = purge.

## Règle de contexte

Commencer par `brain.md` + ce fichier. Lire `debughistorical.md` seulement en cas de régression connue ; lire l'archive uniquement pour retrouver un ancien détail. Ne pas charger tout l'historique par défaut.
