# brainmap.md — SpaceFortressVs

## Reprise rapide

- Téléphone : `feature/kinetic-dust-impact-v4`.
- Gameplay figé : `8f2ee5ed61372f2647ae7284abdf9a0df7d8ebbf` ; workflow `37241618498` GREEN.
- Historique détaillé : `docs/archive/2026-10-05-kinetic-dust-impact-v4.md`.

## Fichiers clés

- `src/main.cpp` — historique protégé, lecture seule.
- `src/kinetic_shield.hpp` — modèle cinétique v3 / surcharge.
- `src/legacy_field_runtime.hpp` — champ réel, destruction canonique, rendements blanc, réaction rouge.
- `src/legacy_field_primitives.hpp` — primitives du champ historique.
- `src/t.hpp` — mouvement des particules et fragmentation historique.
- `src/tactical_runtime.hpp` — collecte blanche, campagne/coop/Danger 9.
- `src/remaster_runtime.hpp` — rendu et contrôles.
- `tests/kinetic_dust_regressions.cpp` — purge/collision/champ/rouge.
- `scripts/test-regressions.sh` — suite complète.

## Flux canoniques

- Purge armée 2 s → destruction astéroïde → blanc 100 % proportionnel, quasi statique.
- Collision astéroïde ou champ normal → destruction astéroïde → blanc 10 % proportionnel, vecteur incident.
- Toute destruction blanche → helper canonique + garde `pv>0` → une émission maximum.
- Minage projectile → chemin historique séparé.
- Rouge + vague cinétique → flash local chaud → majorité consumée / petite fraction déviée ; aucun effet gameplay.
- Blanche → collecte énergie puis soin ; aucune déviation cinétique.

Commencer par `brain.md` + ce fichier. Ouvrir `debughistorical.md` seulement pour un piège connu et l’archive pour les SHA/logs détaillés.
