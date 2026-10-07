# brain.md — SpaceFortressVs

## État canonique vivant
- Release publique actuelle : `v1.4.2`.
- `main` : merge validé `414b23cd2e787325b723fe7b0b6bd84fac02494b`.
- Commit/tag de publication : `5c3bedbd592f7bf4c50b830d26d6a0da49048813`.
- Workflow publication : `37530458354` / run 339, build + `publish-release` GREEN.
- `src/main.cpp` historique reste strictement protégé, blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- APK v1.4.2 SHA-256 : `b5604ba8f103a351e62beb0751893d6ffdcc0d9549d6c75cb2545ea061ea8282`.
- AAB v1.4.2 SHA-256 : `4a996b60cc5bca9d874a7d2897c3ec83d095e3f5d9a989a192df3979647d74ea`.

## Hall global — livré / hors périmètre
- Hall global v1 fonctionne et a été validé téléphone : `SYNC OK`, `GLOBAL 1 + LOCAL 0`.
- Secret jeu injecté au build via `SPACEFORTRESS_HOF_API_KEY`; jamais dans Git/logs/mémoires.
- Aucun changement Hall dans v1.4.2.

## Cinétique v1.4.2 — livré
- Fatigue énergétique : `effectiveEnergy = pow(energyFraction, 1.20)`.
- `nrj=0` = plein ; `nrj=50` = vide. Repères : 50 % ≈43,5 %, 25 % ≈18,9 %, 10 % ≈6,3 %.
- Surcharge ×2 appliquée après la fatigue ; à 2 s, état armé avec champ OFF jusqu’au relâchement.
- Vague visuelle : ~0,27 s pleine énergie vers ~0,50 s réserve vide ; collision/impact physique inchangé.
- Couleur équipe → orange → rouge ; sous 10 % : rouge lumineux clignotant ~4,5 Hz, sans flash HUD/écran.
- CLASSIQUE DUEL local : second doigt par moitié utilise le même moteur 0,30 s / 2 s / blast 3× pour les deux joueurs, indépendamment.
- Quitter le duel annule proprement uniquement les charges détenues par le bridge duel.
- Chaque vague capture explicitement la vraie réserve via `sfKineticEnergyFraction(ship->nrj)`.
- DUEL IA et COOP conservent leurs chemins existants.

## TDD / preuves
- RED initial : workflow `37517885303`.
- RED de fermeture : CI 335 a détecté le cleanup duel et la capture vraie énergie manquants.
- PR finale : CI `37528710590` GREEN.
- `main` après merge : CI `37529550402` GREEN.
- Publication : workflow `37530458354` GREEN, `publish-release` SUCCESS.

## Invariants permanents
- `src/main.cpp` ne doit jamais être modifié.
- Hall, Danger 9, campagne 200, progression, sauvegardes et poussières hors ordre restent protégés.
- Ne jamais écraser les assets d’une Release existante sous le même tag.


## Mission active après v1.4.2 — dispersion / recharge / champ boss
- Fab a corrigé l'audit : la recharge passive CLASSIQUE existe bien historiquement dans `src/mainv1.hpp::sprite::update()` avec `nrj*=0.997`; elle ne doit pas être supprimée.
- Défaut COOP : `campaign_runtime.hpp` applique actuellement `.997^(60*dt)`, beaucoup plus rapide que le rythme observé du thread historique classique.
- Objectif : recharge passive COOP lente (~ordre de grandeur classique), poussières blanches toujours récupération active.
- Restaurer un spread initial aléatoire gauche/droite partagé : faible à pleine énergie, nettement plus large à faible énergie ; vol ensuite rectiligne.
- Boss COOP : champ cinétique fixe 55 %, inférieur aux joueurs, dégâts astéroïdes résiduels réels sur PV boss, sans Danger.
- Branche : `fix/shot-dispersion-energy-boss-field-v143`.
- TDD RED en préparation ; aucun merge/release sans nouvelle validation Fab.


## Implémentation lot dispersion/recharge/champ boss
- TDD RED prouvé : workflow `37539108268` / run 340 échoue exactement sur les nouveaux helpers/état absents.
- Spread commun : enveloppe ~0,018 rad à pleine énergie jusqu'à ~0,180 rad à réserve vide, échantillon aléatoire symétrique gauche/droite.
- CLASSIQUE humain : `sfFireMain(..., nullptr)` applique ce spread avant le départ ; vol reste rectiligne. L'IA classique ciblée conserve son interception prédictive.
- COOP : remplace le sinus déterministe par le même spread aléatoire ; vitesse/cadence continuent de dépendre de `nrj`.
- Recharge passive COOP : demi-vie de chaleur 21 s, frame-independent, pour retrouver le rythme lent observé du classique historique au lieu du faux 60 Hz.
- Boss COOP : champ fixe 55 %, rayon 1,08× boss, base cinétique 80 ; résiduel retire des PV au boss, sans Danger ; astéroïde détruit via la filière cinétique existante et flash de champ dédié.
- Aucun merge/release avant validation téléphone Fab.


## Suivi CI 341
- L'implémentation compile et les nouveaux chemins boss tournent ; l'unique arrêt est l'ancien seuil COOP `abs(vx)<30`, incompatible avec la dispersion élargie voulue.
- Correctif : adapter uniquement ce bornage de régression à `<120`, sans modifier le gameplay.


## CI 342 GREEN — candidat téléphone
- HEAD code/test : `5c66e892374a0a3596e76e1f3eb4ef0b0d26028d`.
- Workflow `37540141916` / run 342 : GREEN complet (régressions, Hall protocole, secret hygiene, APK, AAB, packaging).
- Artifact : `11447494930`, digest ZIP `sha256:5167034c9d6428d8f92e43c141a6e1aab63369c0f7ac53f5fdb7c4cf76d54bec`.
- APK test SHA-256 : `0eaf0b298858b4f934264daad1ae7dcd2d8b209b7a75b9a76c5f59656284eee2`.
- AAB test SHA-256 : `f2075f73fe959208573566aed9caaa9fa4ca20435de40d322a0b5b428819b67b`.
- `publish-release` SKIPPED. PR #7 reste draft. Aucun merge `main`, aucune Release.
- `src/main.cpp` revérifié au blob canonique `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- Étape restante : validation téléphone Fab du ressenti dispersion/recharge et des collisions astéroïde→champ boss.


## Avenant Fab 2026-10-07 — remplace le candidat GREEN 342 sur trois points
- Champ boss 55 % : NON destructif. Le caillou survit, son impact est amorti/dévié ; seul le résiduel enlève des PV au boss.
- Visuel boss : anneau beaucoup plus transparent, expansion centre -> rayon du champ.
- Charge joueur 0,30–2,00 s : minage continu d'un astéroïde proche, plus faible que des tirs répétés, avec poussière blanche aspirée efficacement ; arrêt du minage à READY 2 s.
- COOP : astéroïdes générés continuellement dans le temps.
- Recharge passive : dernier canon Fab = cadence v1.4.2 ×4 en MOU DU GENOU (.997^(240*dt)) vers ×2 en APOCALYPSE (.997^(120*dt)), interpolation monotone.
- Le run 342 n'est donc plus candidat téléphone ; nouvelle preuve RED/GREEN requise.


## RED avenant run 343 + implémentation
- RED avenant prouvé : workflow `37559681669` / run 343 échoue exactement sur les signatures/états demandés : recharge par Danger, alpha/rayon anneau boss, spawn continu et minage de surcharge.
- Implémentation en cours : recharge COOP 4,0 s (MOU DU GENOU) -> 10,5 s (APOCALYPSE), boss 55 % non destructif avec rebond amorti/cooldown, anneau centre-out alpha <=72, spawn continu 3,5 s plafonné à 24 astéroïdes.
- Charge 0,30–2,00 s : extraction continue 1,25 équivalent-tir/s sur l'astéroïde proche + 7 poussières blanches/s aspirées vers le vaisseau ; arrêt strict à READY 2 s.
- Les tirs ordinaires ciblés IA reçoivent aussi la dispersion initiale aléatoire ; aucun guidage en vol ajouté.
- Nouvelle CI GREEN complète requise avant APK téléphone.


## CI 345 — couture de noms, gameplay non invalidé
- Workflow 37560224642 a échoué à la compilation avant les régressions : les tests RED utilisaient les noms canoniques RING_DURATION, RingRadius et SurgeMineAsteroids, tandis que l'implémentation avait gardé des noms provisoires.
- Correction bornée : aligner ces noms, supprimer l'appel runner obsolète et appliquer le dernier canon recharge ×4→×2.
- Aspiration blanche conserve maintenant une vraie vitesse orientée vers le vaisseau en plus du rapprochement direct.
