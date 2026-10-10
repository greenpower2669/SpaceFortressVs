# todo.md — SpaceFortressVs

## Livré
- [x] Hall global v1 validé téléphone.
- [x] Surcharge cinétique v2 : 0,30 s / 2,00 s / blast 3,0× / EMP.
- [x] Release publique `v1.4.1`.
- [x] Fatigue cinétique `pow(energyFraction,1.20)`.
- [x] Vagues visuelles 0,27→~0,50 s avec couleur équipe→orange→rouge et flash <10 %.
- [x] Surcharge 2 s partagée pour les deux joueurs en CLASSIQUE DUEL.
- [x] Cleanup ciblé des charges duel à la sortie du mode.
- [x] Capture explicite de la vraie réserve d’énergie dans les vagues.
- [x] PR #6 GREEN puis merge vers `main` au SHA `414b23cd2e787325b723fe7b0b6bd84fac02494b`.
- [x] `src/main.cpp` vérifié au blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- [x] Publication `v1.4.2` GREEN via workflow `37530458354`.
- [x] Release publique vérifiée avec APK/AAB/build.json/SHA256SUMS.

## Release v1.4.2
- [x] Tag/commit : `5c3bedbd592f7bf4c50b830d26d6a0da49048813`.
- [x] APK SHA-256 : `b5604ba8f103a351e62beb0751893d6ffdcc0d9549d6c75cb2545ea061ea8282`.
- [x] AAB SHA-256 : `4a996b60cc5bca9d874a7d2897c3ec83d095e3f5d9a989a192df3979647d74ea`.

## Invariants
- [x] `src/main.cpp` non modifié.
- [x] Hall, Danger 9, campagne 200, progression et poussières hors périmètre.
- [x] Aucune vraie clé Hall ajoutée à Git/logs/mémoires.


## Mission dispersion / recharge / champ boss
- [x] Audit : dispersion CLASSIQUE perdue car la copie Android remplace `tirerj1/2` historique par `sfFireMain` où le tir humain part avec X=0.
- [x] Audit : dispersion COOP existe mais déterministe/faible et est masquée par la recharge passive trop rapide.
- [x] Audit corrigé : recharge passive CLASSIQUE historique confirmée dans `sprite::update()`.
- [x] Fab choisit un champ boss fixe moins puissant que joueurs ; lot fixe 55 %.
- [x] TDD RED pour dispersion, recharge lente COOP et impact astéroïde boss : workflow `37539108268`.
- [x] Implémentation minimale.
- [x] CI complète GREEN + APK test : workflow `37540141916` / run 342, artifact `11447494930`.
- [ ] Validation téléphone Fab : cône niveaux 1/3/9, IA CLASSIQUE, secours COOP, boss, tactile haut/bas.
- [ ] Aucun merge/release avant ordre explicite.


## Avenant Fab post-342
- [x] Ancien candidat 342 déclaré obsolète avant validation téléphone.
- [x] RED : boss field non destructif + anneau centre-out transparent — run 343.
- [x] RED : recharge Danger 4,0 s -> 10,5 s — run 343.
- [x] RED : spawn astéroïdes COOP continu — run 343.
- [x] RED : minage/aspiration blanc pendant charge 0,30–2,00 s — run 343.
- [x] Implémentation minimale codée ; [x] couture CI 345 corrigée ; [x] assertion IA CI 346 adaptée au spread ; [x] CI 347 complète GREEN.
- [x] Nouvel APK téléphone prêt dans artifact `SpaceFortressVs-1.4.2-release-files` id `11456527917` ; [ ] validation physique Fab.
- [ ] Aucun merge/release sans ordre explicite.

- [x] Dernier canon recharge ×4→×2 par rapport à v1.4.2 appliqué.

- [x] Canon reconfirmé : champ boss fixe 55 % < joueurs ; recharge passive historique présente et préservée.


## CLASSIC-FX-KINETIC-HUD-V143
- [x] Branche dédiée créée depuis 898227ec.
- [x] Diagnostic initial broutage : atlas/UV plausible ; demi-texel retenu pour TDD.
- [x] Tests RED écrits : HUD 3 barres, couleurs, réserve boss/regen difficulté, alpha inverse, UV demi-texel, textures classiques COOP.
- [x] RED prouvé : run 349 / `37608129628`.
- [x] Implémenter sans toucher src/main.cpp.
- [x] CI 350 GREEN + APK/AAB sur `1aeb6758dd5ee8bbcf52e00f424361d8262f0c6d`.
- [ ] Validation téléphone Fab ; aucun merge/release avant accord.

- [x] Classic missile/orbs/explosion COOP ; HUD 3 barres ; réserve visuelle boss ; UV demi-texel implémentés.
- [x] RED 349 vérifié ; [x] CI 350 GREEN du commit d'implémentation.


## BOSS-HUD-RESERVES-V143
- [x] Correction Fab capturée : jauges triple réservées au boss ; pilotes = PV + ENERGIE.
- [x] Branche dédiée `fix/boss-hud-reserves-v143` depuis `b588abe195000a6ab00e4bae86b441eb4b6c3455`.
- [x] Tests RED écrits pour géométrie boss ENERGIE/VIE/CINETIQUE + réserve énergétique séparée.
- [x] RED prouvé : run 354 / `37634894304`.
- [x] Implémenter correction minimale sans toucher au reste du lot 350.
- [x] CI355 GREEN APK/AAB ; [ ] validation téléphone Fab.

- [x] Pilotes 2 jauges restaurés ; boss 3 jauges + réserve énergétique séparée implémentés.


## BOSS-HUD-COMPACT-DANGER-REGEN
- [x] CI353 précédente consignée.
- [x] Branche dédiée créée depuis 2e0fb31e.
- [x] Tests RED écrits : bloc boss unique compact semi-transparent + regen Danger 9 avec APOCALYPSE = 9× MOU.
- [x] RED prouvé : run 357 / `37694965125`.
- [x] Géométrie compacte + alpha + regen Boss Danger implémentées.
- [ ] CI GREEN APK/AAB puis validation téléphone Fab.

- [x] Boss MOU=0.02/s ; APOCALYPSE=0.18/s ; interpolation 9 Dangers.

- [x] Artifact CI355 id `11489666990` ; APK `3d843a48...` ; AAB `044f4fae...`.


## BOSS-HUD-MIRROR-LABEL
- [x] Branche dédiée depuis fb8b98d9.
- [x] Tests RED écrits pour label BOSS + reflet interne symétrique.
- [x] RED prouvé : run 360 / `37697366838`.
- [x] Implémenter uniquement le polish HUD boss.
- [x] CI358 GREEN + APK/AAB ; [ ] validation téléphone Fab.

- [x] BOSS au-dessus + reflet/gloss interne sur les 3 barres, aucune emprise HUD supplémentaire.

- [x] Artifact CI358 id `11514673145` ; APK `239c14f1...` ; AAB `44f4cb83...`.


## BOSS-HUD-DUAL-ROTATED
- [x] Branche dédiée créée depuis 4f5c05be.
- [x] Tests RED : bas-gauche normal, haut-droite miroir 180°, remplissage inversé, label miroir.
- [ ] Prouver RED en CI.
- [x] Même objet HUD implémenté bas-gauche + miroir 180° haut-droite.
- [x] CI361 GREEN + APK/AAB ; [ ] validation téléphone Fab.

- [x] Remplissage, label BOSS et gloss tournés avec le bloc haut.

- [x] Artifact CI361 id `11515479617` ; APK `3d35a827...` ; AAB `4f4cb8bc...`.


## SURGE-CONE-MINING
- [x] Branche `fix/surge-cone-mining-v143` depuis d5a897d9.
- [x] Contrat RED : continuation après 2 s, portées ×1.5, cône avant, visuel orange convergent, son discret post-READY.
- [x] CI363/364 ont exposé les deux incompatibilités résiduelles (header autonome + ancien test 360°).
- [x] Gameplay + cône visuel + audio post-READY implémentés sans toucher `src/main.cpp`.
- [x] CI365 GREEN + APK/AAB ; [ ] validation téléphone Fab.

- [x] Restitution poussières réutilise `sfMineAsteroid` / `partsforiw` historique.

- [x] Artifact CI365 id `11531471894` ; APK `71a4fb84...` ; AAB `9f4a3357...`.


## CONE-MINAGE-SUPERCHARGE-SPEC
- [x] Branche `fix/cone-mining-supercharge-spec-v143` depuis CI365 documenté.
- [x] PDF canonique repris depuis main.
- [x] 0,2 s + cercle rouge→vert + bouclier normal suspendu.
- [x] Minage simultané distance-dépendant, temps réel, non bloqué par les particules.
- [x] Boss DPS continu ×3→×0,03 selon point de collision.
- [x] CI369 GREEN + APK/AAB.
- [ ] Validation téléphone Fab des vitesses Vprès/Vloin provisoires.

- [x] Artifact CI369 id `11570413138`; APK `31d17b83...`; AAB `10db6126...`.
- [ ] Test téléphone Fab : calibrer uniquement Vprès/Vloin si nécessaire.


## IA-BALANCE-CLASSIC-VECTOR
- [x] Branche dédiée depuis le candidat CI369.
- [x] Cône difficulté ÷1..÷9 : minage / offense / ouverture / longueur.
- [x] IA adverse CLASSIQUE progressive N1→N9 + récupération <=10 % + supercharge stratégique.
- [x] IA COOP sauveteur + ressources <=10 % + petit astéroïde exploitable + cône minage uniquement.
- [x] Boss : agresseur principal + charge adaptative + esquive missile bornée.
- [x] CLASSIQUE tactile haut/bas : destination vectorielle ; COOP tactile intacte.
- [x] CI377 GREEN + APK/AAB.
- [ ] Validation téléphone Fab.

- [x] Artifact CI377 `11578789486`; APK `133e558b...`; AAB `99a3b202...`.


## CI385 — RETOUR TELEPHONE IA/MINAGE + CONE
- [x] Courbe cône remplacée par 2/(niveau+1) : niveau 1 = 100 %, niveau 9 = 20 %.
- [x] CLASSIQUE VS IA : mode Mine déclenche réellement et maintient le cône ; approche adaptée à la portée réelle.
- [x] COOP IA : minage préventif <=35 %, urgence <=10 %, cible exploitable non fuie, secours prioritaire.
- [x] Synchronisation défensive COOP+IA au démarrage campagne.
- [x] CI385 GREEN + APK/AAB ; src/main.cpp intact.
- [ ] Validation téléphone Fab.

## 2026-10-09 — FAB PHONE FEEDBACK — ON VALIDATION BRANCH ONLY
- [x] Trace Android : generated main via CMake; sfTacticsBeginFrame → sfUpdatePilot → sfAiUpdateConeStrategy ; sfLegacyFieldFrame → sfKineticSurgeMineAsteroids.
- [x] VS IA : real nearby mining opportunities, temporary mining commitment across rethinks, logcat traces and generated-runtime integration test.
- [x] Border refuge from actual radius + relative arena dimension for classic AI, coop AI and boss (charge and dodge kept).
- [x] Vector touch speed up for classic and coop, still physically capped.
- [ ] CI APK/AAB green and main.cpp blob check.
- [ ] Fab phone test: mining VS IA, borders/boss charge, faster vector tracking, difficulty 1 and 9.
- [ ] No main merge, no Release without Fab approval.


## FAB-DUEL-PARITY-SPEED-2X — 09/10/2026
- [x] Audit alternate VS-AI remaster and local-duel second-finger controls.
- [x] Double human vector speed only, no teleports, keep CPU speed.
- [x] Same lower human finger/surge controls in LOCAL and VS AI.
- [x] Add regression coverage for both caps and input route.
- [ ] New Android CI, tests, APK/AAB and historical SHA verification.
- [ ] Fab phone test of controls, collisions, mining and two duel modes.
- [ ] Signature, main merge and Release remain on hold.


## 2026-10-09 — Aides sur CI389
- Base CI389 `5ed37a7647f348767f90d913f31fae76bd3b6089`; branche `fix/help-refresh-on-ci389-20261009`.
- Report limité à `src/help_runtime.hpp`, `tests/help_runtime_regressions.cpp`, `docs/proposals/2026-10-09-tutorial-mini-jeux.md`. Cônes, IA, minage, vitesse et `src/main.cpp` conservés.
- Mini-jeux proposés seulement. Validation CI/APK téléphone requise. Pas de merge main ni Release.


## SOLO Android — 2026-10-10 (branche feature/solo-worlds-tutorials-v1)
- [x] Réparer la duplication et la commande Bash tronquée dans `.github/workflows/android-build.yml` (`509a6310`) : CI à nouveau exécutable.
- [x] Point d'entrée C++ SOLO distinct du `SDL_main` historique, avec test de liaison sans second `main`.
- [x] Activité Android `SpaceFortressSoloActivity` et second lanceur **DEBUG uniquement** ; lancement historique inchangé (`SpaceFortressMain` sans option).
- [x] Lier `src/solo_prototype.cpp` à `libmain.so` ; lancement du prototype via `--spacefortress-solo-prototype`.
- [x] Progression SOLO chargée via `SDL_GetPrefPath`, sauvegarde atomique à la victoire, sans modifier les sauvegardes historiques.
- [x] Empêcher une lecture de session détruite après victoire.
- [ ] Vérifier la compilation Android du commit `a31bd91b` et récupérer l'artefact APK debug ; **aucune validation téléphone à ce stade**.
- [ ] Tester séparément les deux lanceurs sur téléphone : jeu classique, SOLO prototype, événements Android et retour accueil.
- [ ] Raccorder le vrai moteur COOP canonique via `solo_canonical_adapter.hpp` ; tirs, cône, énergie, boucliers, collisions, minage, FX : encore absents du prototype final.
- [ ] Intégrer un accès SOLO final dans l'application unique après validation des mécaniques ; retirer le lanceur test si devenu inutile.
- [ ] Ne pas fusionner `main`, ne pas publier de Release sans validation explicite.


## SOLO — Canonical kinetic steering/charge (2026-10-10)
- [x] `src/solo_kinetic_control.hpp` réutilise les vrais `sfKineticSurgePress`, `sfKineticAdvanceSurges`, `sfKineticSurgeRelease` et la géométrie `sfKineticSurgeConeContains` (vaisseau SOLO inférieur = propriétaire cinétique 1).
- [x] Toucher 1 réservé au pilotage ; toucher 2 ou SPACE : appui court déclenche un tir, appui de 2 secondes déclenche la décharge cinétique à la libération.
- [x] Première interaction SOLO : suppression des astéroïdes dans le cône historique (simulation de récolte provisoire ; poussières, sons et FX réels encore à raccorder).
- [x] Jauge visuelle haute lisibilité de charge 2 s, portée et demi-angle issus des constantes cinétiques partagées.
- [x] Régressions `tests/solo_kinetic_control_regressions.cpp` et `tests/solo_touch_controls_regressions.cpp`, ajoutées à Actions.
- [ ] Vérifier la CI Android complète au SHA `7faf0eee` ; essais écran Samsung obligatoires.
- [ ] Coupler moteur COOP réel : énergie/bouclier, vrai tir/missile, poussière blanche, FX et IA. Le prototype reste une démonstration non finale.
- [ ] Aucun merge vers `main`, ni Release, sans accord explicite.


## SOLO CAMERA — retour téléphone 2026-10-10
- [x] Retour screenshot téléphone : premier essai SOLO rendu comme une mini-carte géante (54 cases sur largeur), vaisseau trop petit et collé en bas dans la barre de navigation Android ; la petite mini-carte est bien le rectangle en haut à droite.
- [x] Corriger cadrage dans `src/solo_viewport.hpp` : ~18 colonnes visibles, suivi du pilote avec marge inférieure, sans toucher à la grille de collisions logique.
- [x] Réutiliser la projection commune pour `solo_renderer.hpp`, `solo_touch_controls.hpp`, tirs et cône cinétique ; agrandir le vaisseau technique et décaler HUD en dehors de la barre d'état.
- [x] Ajouter `tests/solo_viewport_regressions.cpp` au workflow SDL2.
- [ ] Attendre le CI vert du commit `26877a4e`, récupérer nouvel APK ; faire vérifier visuellement sur Samsung que le vaisseau est visible et que pilotage / tirs / cône s'alignent.
- [ ] Rendu définitif NON livré : remplacer les symboles techniques par les graphismes du vaisseau et assets historiques COOP, intégrer collisions, animations et FX sans recoder le canon.
- [ ] Aucun merge main ni Release sans validation explicite.
