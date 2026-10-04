# brainmap.md — SpaceFortressVs

## Reprise rapide

- Travail téléphone : `feature/help-tutorial-danger-9-canon`.
- Code vérifié : `d9520a0b674d7f21df37f982a444d625b523f8d9` / workflow `37236262962` GREEN.
- Historique : `docs/archive/2026-10-spacefortress-help-tutorial-danger9-history.md`.

## Fichiers clés

- `src/main.cpp` — historique protégé.
- `src/boss_danger.hpp` — 9 dangers + `sfApplyHostileDanger`.
- `src/classic_danger_runtime.hpp` — qualification des tirs IA hostiles classiques.
- `scripts/patch-classic-danger.cmake` — patch de la copie Android générée.
- `src/campaign_runtime.hpp` — campagne/coop ; danger non cinétique, cinétique exclu.
- `src/help_runtime.hpp` — contenu/rendu/navigation aide.
- `src/help_format_bridge.hpp` — animation seulement en `ANIME`.
- `src/tutorial_runtime.hpp` — tuto guidé sandbox.
- `src/help_live_bridge.hpp` — `?` en jeu, consommation tactile, pause/reprise, pile aide/tuto.
- `src/kinetic_shield.hpp` / `src/legacy_field_runtime.hpp` — canon cinétique.

## Flux canoniques

- Hostile non cinétique → `sfApplyHostileDanger` → protections → PV.
- Classique IA : seulement projectile IA hostile vers joueur humain → danger.
- Astéroïde/cinétique → physique cinétique → protections → coque, **sans danger**.
- Blanche → énergie puis soin ; rouge → visuel seulement.
- Aide jeu → suspend/consomme doigt → aide/tuto → retour exact à la partie.

## Contexte

Commencer par `brain.md` + ce fichier. Ouvrir `debughistorical.md` seulement pour une régression connue et l’archive seulement pour l’historique détaillé.
