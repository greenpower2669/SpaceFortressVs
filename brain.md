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
