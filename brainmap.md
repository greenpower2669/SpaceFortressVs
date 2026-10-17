# brainmap.md — SpaceFortressVs

## Reprise rapide
- Canon public : `v1.4.2`.
- `main` : `414b23cd2e787325b723fe7b0b6bd84fac02494b`.
- Release/tag : `5c3bedbd592f7bf4c50b830d26d6a0da49048813`.
- `src/main.cpp` lecture seule, blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.

## Hall global
- Validé téléphone : `SYNC OK`, `GLOBAL 1 + LOCAL 0`.
- Secret build seulement via `SPACEFORTRESS_HOF_API_KEY`.
- Aucun changement Hall dans v1.4.2.

## Cinétique v1.4.2 — flux livré
Énergie :
- `energyFraction = 1 - nrj/50` ;
- `effectiveEnergy = pow(energyFraction,1.20)` ;
- dissipation = maximum × effectiveEnergy × surgePower ;
- à 10 % d’énergie ≈6,3 % d’efficacité nominale ; surcharge ×2 reste faible ; charged 2 s => puissance 0.

Vague :
- pleine réserve : ~0,27 s ;
- faible réserve : jusqu’à ~0,50 s visuellement ;
- couleur équipe→orange→rouge, alpha croissant ;
- <10 % : rouge + clignotement ~4,5 Hz ;
- physique d’impact non ralentie ;
- énergie vraie capturée au déclenchement de la vague.

CLASSIQUE DUEL local :
- premier doigt = mouvement historique ;
- second doigt même moitié = moteur partagé `sfKineticSurgePress/Release(owner)` ;
- <0,30 s = tir normal ; 0,30–2 s = irisé ×2 ; >=2 s = ready/vulnérable ; release = EMP + purge 3× ;
- owners 0/1 indépendants ;
- sortie du mode = annulation ciblée des charges duel ;
- `src/main.cpp` inchangé.

## Preuves v1.4.2
- RED : `37517885303`.
- GREEN PR final : `37528710590`.
- GREEN main : `37529550402`.
- GREEN publication : `37530458354`, `publish-release` SUCCESS.
- APK SHA-256 : `b5604ba8f103a351e62beb0751893d6ffdcc0d9549d6c75cb2545ea061ea8282`.
- AAB SHA-256 : `4a996b60cc5bca9d874a7d2897c3ec83d095e3f5d9a989a192df3979647d74ea`.

## Protections
- Champ normal max 2,0 diamètres ; blast armé 3,0.
- Hall, Danger 9, campagne 200, poussières et progression hors périmètre.
- Ne jamais modifier `src/main.cpp`.

## Travaux isolés après v1.4.2 — 09/10/2026
- Branche `feature/help-refresh-tutorial-minigames-20261009` depuis `main` `56b86302`.
- Aide intégrée corrigée dans `src/help_runtime.hpp`, tests `tests/help_runtime_regressions.cpp` ; consulter `todo.md` pour validation.
- Séquences de mini-jeux du tutoriel = **proposition** non implémentée : `docs/proposals/2026-10-09-tutorial-mini-jeux.md`.
- Aucune modification de gameplay, signature Android, `main`, Release, ni `src/main.cpp`.
