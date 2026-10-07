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


## Lot v1.4.3 candidat — en travail
- CLASSIQUE historique : passive recharge confirmée dans `sprite::update(): nrj*=0.997`.
- COOP : remplacer le faux équivalent 60 Hz trop rapide par une recharge lente frame-independent.
- Tirs : helper commun d'enveloppe de dispersion selon `nrj` + échantillon aléatoire symétrique ; CLASSIQUE `sfFireMain` et COOP `sfCoopFire`.
- Boss : champ fixe 55 % autour du boss ; astéroïde entrant -> calcul masse/vitesse relative -> 55 % dissipé, résiduel sur santé boss -> destruction cinétique/poussière existante.
- Pas de Danger sur le cinétique boss.


## Implémentation active
- `tactical_runtime.hpp` : `sfMainShotSpreadEnvelope/Radians/RandomUnit` + spread humain classique.
- `campaign_runtime.hpp` : spread COOP aléatoire partagé, recharge passive demi-vie 21 s, champ boss fixe 55 %, collision astéroïde→boss et anneau visuel.
- TDD RED run 340 (`37539108268`) confirmé avant code.


## CI 342 GREEN
- Code candidat : `5c66e892...`.
- Run `37540141916` entièrement GREEN ; artifact `11447494930`.
- Phone à vérifier : spread gauche/droite à faible énergie en CLASSIQUE + COOP, recharge COOP lente, anneau boss + dégâts résiduels d'astéroïdes.
- PR #7 reste draft ; pas de merge/release.


## Avenant 07/10 — architecture cible
- Boss field : 55 %, centre-out transparent, astéroïde survivant amorti + cooldown anti-multi-hit.
- Recharge COOP : helper Danger 0..8 ; multiplicateur cadence v1.4.2 ×4 -> ×2, soit exposants 240 -> 120 par seconde.
- Astéroïdes COOP : timer de spawn continu avec plafond de population.
- Surge mining partagé CLASSIQUE/COOP : actif seulement 0,30 <= hold < 2,00 s ; cible proche unique ; shrink continu < cadence tirs ; poussière blanche attirée.
- Run 342 obsolète pour validation téléphone.


## Run 343 RED -> code avenant
- Run `37559681669` RED attendu sur les nouveaux contrats.
- Recharge: half-life Danger 0..8 = 4,0 -> 10,5 s.
- Boss field: 55 %, ne détruit pas ; residual HP + vitesse relative amortie/rebondie ; cooldown .42 s.
- Boss wave: centre -> rayon 1,08× boss, durée .46 s, alpha max 72 + limite permanente alpha 28.
- Spawn COOP: batch historique `setasts(1)` toutes les 3,5 s, plafond 24.
- Surge mining partagé: 1,25 shot-eq/s + aspiration blanche 7/s, seulement 0,30 <= hold < 2,00.

- CI 345 : échec de compilation uniquement sur noms provisoires/runner ; alignement des noms canoniques avant nouvelle CI.
