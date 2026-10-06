# brainmap.md — SpaceFortressVs

## Reprise rapide
- Dernière Release : `v1.4.1`.
- Base mission : `main` `21b1ff3592ce6f531da58fad6c102049a313325d`.
- Branche : `feature/kinetic-energy-fatigue-duel-v142`.
- PR #6 draft.
- `src/main.cpp` lecture seule, blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.

## Hall global — hors périmètre
- Validé téléphone : `SYNC OK`, `GLOBAL 1 + LOCAL 0`.
- Secret build seulement via `SPACEFORTRESS_HOF_API_KEY`.
- Aucun changement Hall dans v1.4.2.

## Cinétique v1.4.2 — flux cible
Énergie :
- `energyFraction = 1 - nrj/50` ;
- `effectiveEnergy = pow(energyFraction,1.20)` ;
- dissipation = maximum × effectiveEnergy × surgePower ;
- 10 % d’énergie ≈ 6,3 % d’efficacité nominale ; surcharge ×2 reste faible ; charged 2 s => puissance 0.

Vague :
- pleine réserve : durée ~0,27 s, couleur équipe propre ;
- réserve en baisse : durée visuelle progresse vers ~0,50 s, couleur équipe→orange→rouge, alpha augmente ;
- <10 % : rouge + clignotement ~4,5 Hz ;
- impact/collision physique ne ralentit jamais.

CLASSIQUE DUEL local :
- premier doigt de chaque joueur = mouvement historique ;
- second doigt même moitié = `sfKineticSurgePress(owner)` ;
- <0,30 s = tir normal ; 0,30–2 s = irisé ×2 ; >=2 s = ready/vulnérable ; release = EMP + purge 3× ;
- owners 0/1 indépendants ;
- interception Android générée via `sfClassicDuelSurgeHandleEvent`, sans modifier `main.cpp`.

## Fichiers v1.4.2
- `src/kinetic_shield.hpp` : courbe énergie, énergie capturée par vague, durée visuelle.
- `src/kinetic_energy_visuals.hpp` : couleur/alpha/flash et overlay graphique.
- `src/classic_duel_surge.hpp` : second doigt duel local partagé avec le moteur de surcharge.
- `src/th2.h` : branchement des deux headers.
- `scripts/patch-classic-danger.cmake` : routage du filtre d’événement + overlay sur copie Android générée.
- Tests : `tests/kinetic_regressions.cpp`, `tests/test_kinetic_surge_integration.py`.

## TDD
- RED : workflow `37517885303`, attente duel owner 0 absente avant implémentation.
- GREEN complet encore requis avant préparation version 1.4.2, merge et publication.

## Protections
- Champ normal max 2,0 diamètres ; blast armé 3,0.
- Hall, Danger 9, campagne 200, poussières et progression hors périmètre.
- Publication v1.4.2 seulement après CI fraîche GREEN et revalidation du blob historique.


## Fermeture avant v1.4.2 — CI 335
- RED utile au HEAD `091299ef...` : cleanup duel hors mode et source d'énergie vraie des vagues.
- Fix : `sfClassicDuelCancelOwnedSurges()` annule seulement les doigts secondaires possédés par le duel local.
- Fix : `sfKineticTriggerWave(..., sfKineticEnergyFraction(ship->nrj))` sur impact et purge.
- CI fraîche complète obligatoire avant intégration.
