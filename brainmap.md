# brainmap.md — SpaceFortressVs

## Reprise rapide

- Téléphone : `feature/kinetic-dust-impact-v4`.
- Gameplay poussières figé : `8f2ee5ed61372f2647ae7284abdf9a0df7d8ebbf` ; workflow `37241618498` GREEN.
- Hall Danger Boss réel : `47ada1e859e38ec5f09ae5104a0575eb25f08544` ; workflow `37332234356` GREEN.
- Archives : `docs/archive/2026-10-05-kinetic-dust-impact-v4.md` et `docs/archive/2026-10-05-hall-of-fame-danger9.md`.

## Fichiers clés

- `src/main.cpp` — historique protégé, lecture seule.
- `src/boss_danger.hpp` — Danger Boss HOME 1..9 et noms canoniques.
- `src/campaign_save.hpp` — `SfFameEntry.danger`, lecture v1/v2, écriture v3.
- `src/hall_of_fame.hpp` — formule Fab, étoiles 1..9, classement et temps.
- `src/hall_of_fame_runtime.hpp` — Hall visible, noms Danger Boss, étoiles et points.
- `tests/campaign_format_regressions.cpp` — compatibilité v1/v2 + persistance danger + score/ranking.
- `src/kinetic_shield.hpp` — modèle cinétique v3 / surcharge.
- `src/legacy_field_runtime.hpp` — champ réel, destruction canonique, rendements blanc, réaction rouge.
- `src/legacy_field_primitives.hpp` — primitives du champ historique.
- `src/t.hpp` — mouvement des particules et fragmentation historique.
- `src/tactical_runtime.hpp` — collecte blanche, campagne/coop/Danger 9.
- `src/remaster_runtime.hpp` — rendu et contrôles.
- `tests/kinetic_dust_regressions.cpp` — purge/collision/champ/rouge.
- `scripts/test-regressions.sh` — suite complète.

## Flux Hall of Fame

- HOME choisit `sfBossDangerIndex` 0..8 → victoire fraîche `SfFameEntry` capture `danger=1..9`.
- Sauvegarde v3 écrit le danger avec la victoire.
- Lecture v1/v2 → `danger=0` explicitement, donc historique `DANGER INCONNU` sans invention.
- Hall → points depuis boss + danger réel + secondes → tri points décroissant → temps croissant.
- Rendu → boss 1..200 + nom Danger Boss + 1..9 étoiles + temps + points.

## Flux cinétiques canoniques

- Purge armée 2 s → destruction astéroïde → blanc 100 % proportionnel, quasi statique.
- Collision astéroïde ou champ normal → destruction astéroïde → blanc 10 % proportionnel, vecteur incident.
- Toute destruction blanche → helper canonique + garde `pv>0` → une émission maximum.
- Minage projectile → chemin historique séparé.
- Rouge + vague cinétique → flash local chaud → majorité consumée / petite fraction déviée ; aucun effet gameplay.
- Blanche → collecte énergie puis soin ; aucune déviation cinétique.

Commencer par `brain.md` + ce fichier. Ouvrir `debughistorical.md` seulement pour un piège connu et les archives pour les SHA/logs détaillés.
