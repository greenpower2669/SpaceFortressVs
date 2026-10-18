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


## SOLO — accueil commun et sélecteur lisible (retour téléphone 10/10/2026)
- [x] Retour utilisateur : sélecteur SOLO uniquement constitué de quatre barres sans texte ; confusion avec tous les autres modes du jeu.
- [x] Constater que les modes historiques subsistent via `SpaceFortressActivity`; le SOLO était une activité Android de test distincte.
- [x] Ajouter `SpaceFortressHubActivity` **uniquement dans les sources debug** : accueil unique avec deux choix clairs « Jeu original » (accès au menu historique avec CLASSIQUE/DUEL/VS IA/COOP/CAMPAGNE) et « Mode SOLO » (prototype).
- [x] Manifest debug : une seule entrée LAUNCHER via l'accueil, activité classique intacte dans le code et activité SOLO sans icône supplémentaire ; manifeste release original non modifié.
- [x] Rendu de l'écran SOLO : lettres bitmap intégrées, titres centrés, flèches, valeurs MONDE/NIVEAU/DANGER et JOUER/RETOUR ; grands boutons adaptés à la lisibilité.
- [x] Protection Android 16 : marges de sécurité système pour l'accueil Java.
- [x] Contrôles automatiques : script `scripts/check-solo-launcher.py`, SDL2 selector, et validation `aapt dump xmltree` du manifest final de l'APK debug.
- [ ] Attendre CI complète du commit `c5ac97f8` avant de livrer une nouvelle APK dézippée directement.
- [ ] Validation téléphone : accueil commun, accès historique, SOLO, sélection, bouton retour et orientation, sans régression.
- [ ] SOLO n'est encore qu'un prototype : images, assets, mécaniques COOP, diversité des niveaux/boss et UX finale restent à développer.
- [ ] Aucun merge `main` ou Release sans autorisation explicite.


## SOLO — réactivité x4 et malus glace — 10 octobre 2026
- [x] Retour téléphone Fab : pilotage SOLO et défilement perçus trop lents ; demande d'une réactivité **4 fois supérieure**.
- [x] `bc428459` : SOLO seul, thrust 5→20, vitesse max 7→28, sensibilité du doigt ×4, freinage sans direction ×4 ; caméra suit immédiatement la position (pas de téléportation), anticipation plafonnée.
- [x] Tests vitesse/accélération ×4 et défilement lié au déplacement dans `solo_campaign_model_regressions.cpp` et `solo_viewport_regressions.cpp`.
- [x] Idée malus de glace : astéroïdes blancs distincts des ordinaires, ralentissement temporaire 2,5 s, mobilité 25 %, signal visuel « GLACE ». Paramètres initiaux à faire valider par Fab sur téléphone.
- [x] `561ed8e8` : `Tile::IceAsteroid` (RGB EAF9FF), répartition rare + apprentissage au niveau 1, cartes générées version V2 ; intégration collisions/tirs/minage/HUD.
- [x] Sécurité haute vitesse : balayage du trajet de vaisseau par pas de 0,20 case, afin de ne pas ignorer un mur/astéroïde entre deux images.
- [x] Tests automatisés `solo_ice_regressions.cpp` : détection rapide, ralentissement ×0,25, expiration, mur anti-traversée.
- [ ] Confirmer CI Android complète pour `561ed8e8` et produire l'APK debug ; ne pas confondre réussite compilation et validation du comportement téléphone.
- [ ] Test téléphone : sensations ×4, pilotage vertical et horizontal, cadrage caméra, collision, astéroïdes glacés, durée et lisibilité du malus.
- [ ] Conservé : modes historiques CLASSIQUE, DUEL, VS IA, COOP, CAMPAGNE ; aucune fusion main et aucune Release sans validation.


## 2026-10-10 — Plantages Android et journal exportable
- [x] Retour test téléphone (APK x4 + glace, commit 561ed8e8) : le mode CLASSIQUE semble démarrer, mais les autres modes / SOLO plantent. **Cause encore inconnue**. GitHub Actions vert ne prouve pas que les modes Android marchent.
- [x] Audit : pas de journal de plantage exportable dans l'ancienne APK. Un Android Bug Report système reste disponible via Options de développement (attention aux informations privées).
- [x] Activité SOLO Android isolée dans le processus `:solo` (debug seulement) pour éviter une collision possible des états statiques SDL ; **mesure préventive à confirmer sur téléphone**.
- [x] Application DEBUG enregistre dans des journaux persistants événements de lancement/retour, exceptions Java, phases SDL et heartbeat SOLO, avec rotation de 256 Ko ; journaux JAVA par processus et natifs par mode.
- [x] Accueil de l'APK debug : bouton accessible **EXPORTER JOURNAL DEBUG**, ACTION_CREATE_DOCUMENT, destination choisie sur Samsung, produit `SpaceFortress-debug.txt` ; pas de compte, de connexion réseau ni de permission de stockage.
- [x] Collecter `ApplicationExitInfo` (Android 11+) : crash Java, crash natif, ANR, mémoire, heure et processus, sans export du logcat global.
- [x] CI statique `scripts/check-solo-launcher.py` vérifie manifeste release intact, processus :solo et nouveau bouton ; compilation Java/NDK debug contrôlée par workflow Android.
- [x] CI verte pour le diagnostic initial SHA `956449d2` ; cependant l'APK de test a un **certificat debug différent** de l'APK précédente, donc Android ne peut pas la mettre à jour directement. Ne jamais demander de désinstaller le SpaceFortress existant (sauvegardes).
- [x] APK debug **installable à côté** avec `applicationIdSuffix '.diagnostic'` (package `com.greenpower2669.spacefortressvs.diagnostic`) et icône identifiée « SpaceFortress DIAG ». Le package Google Play release demeure strictement inchangé.
- [ ] Attendre CI du commit `020d07d7`, vérification `aapt` du package séparé ; distribuer l'APK diagnostic directe **sans ZIP**. Reproduire la panne sur l'APK diagnostic, exporter TXT depuis l'accueil.
- [ ] Attention : le package DIAG a un stockage isolé du SpaceFortress historique, donc une panne liée uniquement aux anciennes sauvegardes peut ne pas se reproduire.
- [ ] Test Samsung : lancer le jeu ORIGINAL et les modes problématiques, puis SOLO ; après plantage rouvrir accueil → EXPORTER JOURNAL DEBUG → enregistrer TXT, et envoyer le fichier. Ne pas effacer les données de l'app entre-temps.
- [ ] Analyser le vrai rapport utilisateur avant d'affirmer avoir corrigé le plantage ; en cas de SIGSEGV, utiliser la raison de sortie Android et éventuellement un rapport système opt-in.
- [ ] Jamais merge `main` ni Release sans validation explicite.


## 2026-10-10 — SOLO : vrai jeu d'exploration et visée libre
- [x] Labyrinthe dense en travers/haut/bas, graphes connectés, raccourcis/impasses et objectifs trois relais obligeant à explorer/revenir sur ses pas (`Map` version 3).
- [x] Mini-carte d'exploration progressive, libre retour au début, sortie conditionnée boss + relais.
- [x] Texture du vaisseau original bleu et adversaires orange, roches variées, fonds spatiaux, tirs, poussière ; variations visuelles lave/glace, arrière-plan lié caméra sans défilement imposé.
- [x] Mathématique de déplacement partagée avec mode CLASSIQUE (cible tactile, vitesse finie), vaisseau non ralenti arbitrairement hors dégâts temporaires.
- [x] Tapotement 360° = tir, appui de 2s = cône historique dirigé vers le doigt à 360°, possibilité deuxième doigt pendant pilotage.
- [x] Astéroïdes glacés fracturés par tir ou cône -> poussière blanche mobile -> effet de glace temporaire ; lave ralentit aussi sur collision.
- [x] Aide sélection lisible, redémarrage sur écran tactile après destruction, logs de relais en mode DIAG.
- [x] Tests graphes, relais, 4 directions et diagonale, poussière, double-tactile, caméra, ancienne logique conservés.
- [ ] Attendre **GitHub Android APK + AAB** vert sur le dernier commit applicatif ; corriger échecs éventuels.
- [ ] Tester sur Samsung : chargement réel de tous les assets originaux, maniabilité canonique, visée arrière, charge 2 s, collecte des relais, boss, navigation retour, glace/lave et sortie.
- [ ] Améliorer encore les décors monde par monde et les FX audio sans toucher au canon ; raccorder davantage le gameplay historique COOP (bouclier/énergie/tirs) après audit.
- [ ] Aucun merge main, Release ou signature sans aval Fab.


## 2026-10-11 — Mission zoom ×3, caméra des tiers et stabilisation
- [x] Demande Fab : zoom décor ×3, **vaisseau inchangé**, couloirs visuellement 3 fois plus larges ; carte réelle et dimensions de collision conservées.
- [x] Caméra à seuils central 1/3—2/3 en horizontal et vertical ; à l'intérieur aucun pan ; franchissement → décor suit ; retour en arrière → seuil opposé, monde affiché sans saut.
- [x] Unifier projection visuelle/coordonnées doigt/tir ; désolidariser commande du doigt de la translation de caméra, ancrer au geste initial pour éviter les oscillations.
- [x] Réactivité vaisseau en pixels/s préservée en fonction de colonnes visibles (ne pas accélérer ×3 le pilote avec la carte zoomée), taille graphique du vaisseau inchangée.
- [x] Tests de caméra 4 axes, start/finish, changement orientation, zoom x3, taille vaisseau, déplacement tactile indépendant du pan et visée à deux doigts.
- [ ] CI GitHub Android debug verte du dernier commit puis APK DIAG **directe sans ZIP**.
- [ ] Test Samsung : doigt stable, caméra tiers des bords, chemins larges, visée 360, pas de collisions incohérentes, mode historique préservé.
- [ ] Ne pas fusionner `main`, ne pas publier Release sans validation explicite.


## 2026-10-11 — SOLO caméra avec DEUX zones progressives (livré sur branche)
- [x] Fab valide zones **rectangulaires** d'abord, les zones circulaires restent une idée future.
- [x] Centre 1/3–2/3 : aucun défilement. Zone 1 : 0→55 % de la vitesse de mouvement réel, progression interne douce. Zone 2 : 55→100 %, progression interne douce. Vitesse max atteinte à 6 % des bords pour préserver la visibilité du vaisseau.
- [x] Direction X/Y indépendantes (diagonales), accélération et freinage temporels via `viewVelocityX/Y`, défilement continu quand le pilote maintient sa direction, aucun effet direct sur le pilotage ni la visée.
- [x] Retour vers le centre = décélération caméra ; arrêt du vaisseau = convergence vers zéro, pas de défilement infini. Pas de redéfinition de la carte, zoom décor ×3/vaisseau fixe conservés.
- [x] Tests renforcés : continuité de vitesse aux deux frontières, croissance au sein de chaque zone, accélération dans 4 sens, arrêt, diagonales, resize, limite du monde, stabilisation tactile.
- [x] CI GitHub Actions #38121160188 verte, SHA `cace6129` ; APK DIAG directe extraite et contrôlée SHA-256.
- [ ] Fab : installer sur Samsung et valider sensations, rectifications éventuelles des plages 0–55–100 % ou des seuils.
- [ ] **Chantier 1 toujours à faire** : réintégration exhaustive des assets/FX historiques du jeu (cône plasma, projectiles, impacts, explosions, poussières, animations, boucliers). Ne pas prétendre qu'ils sont déjà tous raccordés au SOLO.
- [ ] Jamais de merge `main` ni publication Release sans autorisation explicite.


## 2026-10-11 — CAMERA-SHIP-ZONE-001 — corrigé sur branche SOLO
- [x] Utiliser zone écran du VAISSEAU (pas du doigt) comme source unique des deux accélérations rectangulaires de caméra.
- [x] Centre 1/3..2/3 = pan immédiatement nul ; retour au centre = zéro strict, même si doigt levé et vitesse/croisière persistantes.
- [x] Finger-up conserve `cruiseX/Y` et `cruiseThrottle` de `TouchPilot`; croisière continue en zone extérieure indépendamment du doigt.
- [x] Axe X/Y indépendant, direction inward désactive le pan concerné ; zone 1 et zone 2 avec continuité `smoothstep`.
- [x] Collision SOLO : conserver progression sur axe libre le long d'un rocher ; stopper axe impacté jusqu'à la fin du frame pour éviter traversée par substeps successifs.
- [x] Vérifications automatisées core + UI SDL2 après correction régressions.
- [ ] Vérifier CI Android complète du dernier commit + extraire APK DIAG directe hors ZIP.
- [ ] Essai Samsung : centre fixe, zone 1/2 croissante, finger-up croisière, retour au centre, virages dans vrai labyrinthe, FX/SFX.
- [ ] Aucun merge main ni Release sans autorisation Fab.

## SOLO-FX-MINING-001 — 11 octobre 2026
- [x] Débris blancs d'astéroïdes ordinaires et glacés après cône 2 s, limite 128.
- [x] Traînée par vitesse du pilote et halo d'impact avec asset bouclier historique, sans mécanique de bouclier fictive.
- [x] Tests automatiques d'émission, glace, plafond, collecte et alpha.
- [ ] CI Android et APK/AAB vérifiées, récupérer APK DIAG directe (pas ZIP), SHA-256.
- [ ] Tester sur Samsung poussières, flash, traînée et non-régressions caméra/croisière.
- [ ] Raccorder véritablement énergie, bouclier, minage continu, projectiles/missiles et FX du COOP canonique.
- [ ] Pas de merge main ni Release avant accord Fab.

## SOLO-CONTINUOUS-CONE-002 — 11/10/2026
- [x] Fonte simultanée multi-astéroïdes dès 0,20 s, prolongée après 2 s READY, distance et Danger via modèle cinétique historique.
- [x] Intégrité persistante par case, diminution progressive du sprite, poussières à fonte finale (glace distincte), pas d'effacement instantané au relâchement.
- [x] Tests seuil, maintien, annulation, 4 orientations, multi-cibles, difficulté 9 et débris glacés.
- [ ] CI Android APK/AAB verte du dernier commit, APK DIAG directe et intégrité SHA-256 ; test Samsung calibrage masse .08.
- [ ] Raccordement réel armes, missiles, énergie, bouclier, poussière et boss du COOP, sans états globaux détournés.
- [ ] Aucun merge main ni Release sans accord Fab.

## APK DIAG directe dans temp/ — 11 octobre 2026
- [x] Dernière CI #38136673247 réussie pour `993bbdd3` : APK diagnostic 87 871 264 octets, SHA-256 `4c438f2642da1ae34c05a07cd08d90b6483b65e8df49b1e69883e4cad5a50cfe`.
- [x] Extraction et contrôle depuis le ZIP Actions pour téléchargement APK directe.
- [x] Workflow ponctuel `.github/workflows/solo-apk-to-temp-once.yml` créé sur branche SOLO seulement pour placer l'APK dans `temp/` GitHub, sous réserve de réussite de son exécution.
- [x] Workflow #38140932745 SUCCESS ; APK vérifiée dans `temp/SpaceFortress-SOLO-DIAG-993bbdd3.apk` (87 871 264 octets), SHA-256 `4c438f2642da1ae34c05a07cd08d90b6483b65e8df49b1e69883e4cad5a50cfe` ; commit GitHub du dépôt temp `71b5bab7` ; aucun merge ni Release.
- [ ] Essai Samsung, aucune fusion main / Release sans accord de Fab.

## SOLO-FINGER-ZONES-003 — 11/10/2026
- [x] Vaisseau suit la position écran du doigt, plus de croisière au vecteur dans la boucle réelle.
- [x] Caméra autonome deux zones progressives, continu quand le doigt reste fixe ou levé, arrêt au centre ; axes indépendants.
- [x] Collisions empêchent pan fantôme à travers mur, zoom ×3, visée 360° et moteur historique préservés.
- [x] Régressions doigt fixe, reprise, mur, vertical, central et branche CI.
- [x] GitHub Actions #38142855123 : SOLO core, SDL2 UI, Android APK/AAB tous SUCCESS sur `47e92eac` ; APK DIAG ARM64 extraite en fichier direct, 87 871 261 octets, SHA-256 `d03ec935ecb28a2035e14c3ec460b8c612e8a47bebb82b2e4067ddbd34104274` ; test Samsung restant.
- [ ] Aucun merge main ni Release sans accord Fab.

## SOLO-CONCENTRIC-ATLAS-004 — 11 octobre 2026
- [x] Cercles concentriques vitesse 0 / 0.1→2 / 2.1→6 / 6 au-delà ; stabilisation extérieure / poursuite doigt.
- [x] Assets originaux depuis main par SHA Git ; atlas rendu par thèmes et transparence de présentation, sans modifier main.
- [x] Tests géométrie cercle, diagonal, plateau, rectangles sources atlas, CI et validation PNG.
- [ ] CI core SDL2 Android ; essai Samsung à effectuer avec nouvelle APK DIAG.
- [ ] Intégration **exacte** canon COOP : orbes, bouclier cinétique, coût énergétique, jauge, missile, dégâts, méga décharge. Moteur séparé actuellement : NE PAS déclarer parité achevée.
- [ ] Ne pas fusionner main, aucune Release sans approbation Fab.

## SOLO-CANONICAL-VESSEL-005 — raccordement historique (11/10/2026)
- [x] Factoriser les vraies équations historiques d'énergie, recharge, tir, réserve, usure/bouclier, utiliser en SOLO et modes historiques.
- [x] Sprites canon orbe/missile/bouclier et jauge énergie, 2 s charge et purge RADIALE canon au relâchement.
- [x] Tests de cadence, énergie, missiles, effet bouclier, purge à l'arrière, onde partagée, fichier de test ciblé.
- [x] CI #38149636477 COMPLÈTEMENT VERTE sur `fd7bdc8a` (SOLO logic / SDL2 UI / APK+AAB Android). APK diagnostic ARM64 `SpaceFortress-SOLO-CERCLES-ATLAS-DIAG-fd7bdc8a.apk`, 94 103 112 octets, SHA-256 `714aea507c3494820d88015d5ba2c11c88518db99c9b67fc189481b97caec7cf`, atlas parois/environnements inclus dans l'APK. Tests Samsung encore nécessaires.
- [ ] Intégrer les composants historiques restants (orbes FX exhaustifs, boss dynamique, guidance missile sur cible mobile, champs cinétiques/collisions avec masse réelle).
- [ ] Inspecter et éventuellement découper au pixel les planches pour éliminer les éventuelles bordures de la maquette graphique.
- [ ] AUCUN merge main ni Release avant ordre explicite.


## 11/10/2026 — SOLO-SCENERY-AUTOTILE-006
- [x] Audit photo Samsung : répétition horizontale et carrés dans les décors.
- [x] Autotiling 16 masques, orientations contrôlées par quatre voisins, vrais modèles verticaux et coins; orientation «┐» du PNG source corrigée.
- [x] Découpage en sprites transparents à chargement unique depuis les 2 atlas d'origine ; pas de labels, panneaux ni rectangles plaqués.
- [x] Variantes de décor stables par coordonnées MONDE ; fumée/énergie/lave avec pulsations douces, aucun frame RNG à l'affichage ; collisions inchangées.
- [x] Tests 16 masques/rotations/bornes/IDs stables compilés et passés sur GitHub SOLO/SDL2.
- [x] Android : workflow `38154338078` sur `7012a3b5` ENTIÈREMENT VERT (core, SDL2, Android), SHA256 APK `a3c20d17706082c78c983549fe290d85364cf86ace77b6aaa2dc749c82c709db`. APK DIAG 94 103 112 octets dans `temp/SpaceFortress-SOLO-PAROIS-AUTOTILES-7012a3b5-DIAG.apk` sur la branche via export vert `38154942723`. Test Samsung sur couloirs H/V/L/T/croix reste à faire.
- [ ] Vérifier qualité des dégradés et jonctions sur téléphone; retoucher si restes de carrés ou découpes aberrantes.
- [ ] Ne pas merger `main` ni créer Release sans validation Fab.


## 2026-10-11 — ASTRA-COOP-SOLO-ARCH-001 — audit et migration proposée

- [x] Lire les cinq mémoires et inspecter la branche de référence au SHA `1d8a48e2`, le graphe Android, les composants COOP/SOLO, FabLib, tests et PNG.
- [x] Consulter la CI existante `38154338078` sur `7012a3b5` : core/SDL2/Android réussis. Aucun nouveau run lancé ; aucune validation Samsung déduite de ce résultat.
- [x] Conserver [l'ordre intégral](docs/audits/2026-10-11-mission-astra-coop-solo.md), produire [l'audit A–G et ses preuves](docs/audits/2026-10-11-coop-solo-architecture.md), mettre à jour les cinq mémoires sans effacer l'historique.
- [ ] **Fab : valider la fondation proposée** — extraction progressive du COOP, code commun, contextes isolés, parité en arène avant labyrinthe. Pas encore d'autorisation d'implémenter.
- [ ] Après validation : détailler puis exécuter le premier lot ; témoin COOP reproductible (inputs, hasard, temps, traces et captures), extraction par sous-système avec façade historique.
- [ ] Contextualiser états/joueurs/champ/boss/sons/charge ; masque des pilotes actifs ; aucune écriture de sauvegarde COOP depuis SOLO ; ressources liées au renderer et cycle Android explicite.
- [ ] Premier SOLO canonique en arène : mêmes sprites/proportions, nrj, armes, guidage, cinétique/minage/poussières/sons/FX ; un pilote ; possibilité de simuler sans boss actif.
- [ ] Monde et caméra : métrique de combat stable, projection seule, collisions de coque et tirs, cercle 0..6. Valider avec Fab l'unité des vitesses (4,32 actuel vs 3,10 COOP), sans ajustement arbitraire caché.
- [ ] Rebrancher carte/générateur/relais/minimap/progression ; préserver SFSOLO1. Une reprise complète de partie est une éventuelle fonctionnalité distincte.
- [ ] Régions de rencontres, profils historiques/miniboss et boss final éloigné ; Danger 1..9 ; mapping de profils explicite ; règles d'occlusion des ondes à arrêter avant ce lot.
- [ ] Préparer atlas/manifeste et continuité remplissage/contours ; vérifier les assemblages des vraies images, puis sur Samsung.
- [ ] Après autorisation : tests de parité, non-régression CLASSIQUE/COOP, cycle Android, CI du commit exact, APK diagnostic ARM64 correctement nommée ; essai Samsung avant retrait du vieux combat SOLO.
- [ ] Aucun merge main, aucune modification de src/main.cpp ni Release sans autorisation correspondante. La liste ci-dessus est un plan proposé, pas un ensemble de fonctionnalités livrées.


## 11/10/2026 — SOLO-COOP-SOCLE-001 (premier jalon COOP commun)
- [x] Audit Astra et fondation approuvée Fab ; branche d'implémentation indépendante `feature/solo-coop-core-v1`.
- [x] Extraction vraie interception + cinématique missile d'origine, branchée aux anciens appels COOP; test 1200 cas contre anciennes équations.
- [x] Masque participants explicite, deux par défaut : contrôles COOP, cible/impacts, secours, collecte poussière et collisions astéroïdes, rendu HUD; pas de pilote absent compté comme mort.
- [x] Vrai COOP `sfCoopTick` fonctionne sans boss, conserve sa physique, ses orbes, poussières et sprites d'origine. Boss réactivable, victoire diagnostic ne sauvegarde PAS la campagne.
- [x] Ajout arène opt-in Android DBG à pilote unique utilisant `sfCoopTick` et `sfCoopDrawArena`, même TU FabLib, 3ème entrée du hub, caméra identité (prototype SOLO conservé).
- [x] Cône historique 360 : même géométrie/puissance/vulnérabilité/charge, direction paramétrée pour minage, attraction blanche et représentation ; wrappers COOP historiques inchangés en valeur par défaut.
- [x] Tests logiques, historiques avec vrai moteur et smoke SDL de l'arène inclus dans CI ; workflows autorisent nouvelle branche.
- [ ] CI complète dernier code `172df0c0` / run `38163698538` : **en cours**, vérifier puis corriger si besoin.
- [ ] Récupérer APK debug Android ARM64, valider signature SHA et fournir test Samsung. Ne PAS déclarer tests manuels faits.
- [ ] Validation Samsung vrai vaisseau/FX/cône/dépenses/boss sur nouveau hub, corriger défauts éventuels.
- [ ] Contexte de session explicite (sans globaux), ensuite conversion coordonnées monde et caméra circulaire ; ensuite labyrinthe/parois, progression/SFSOLO1. NE PAS FAIRE avant validation du jalon.
- [ ] Aucun merge main, aucune Release, aucune modification src/main.cpp.


## SOLO-COOP-PRESENT-002 — retour Samsung (11/10)
- [x] Diagnostic de l'écran : audio fonctionnel et sélection CLASSIQUE cache l'arène ; native exit 0, pas de crash. Macro `SDL_RenderPresent` historique capture la présentation diagnostic et réaffiche HOME.
- [x] Fix ciblé commit `b9072d24` : préserver l'ancienne macro, la désactiver seulement lors de l'include diagnostic, puis la restaurer ; protections compile.
- [ ] CI verte du correctif `38167735625` et récupération artifact APK de ce commit, ne pas réutiliser l'APK `ca0322e` en temp.
- [ ] Remplacer `temp/SpaceFortressVs-1.4.2.apk` (et AAB, manifest, SHA256SUMS) par les nouveaux binaires uniquement après CI verte, sans main/Release.
- [ ] Samsung : vérifier que l'arène COOP vraie apparaît, le pilote est visible, le bouton BOSS TEST marche, sons et toucher persistent.


## SOLO-COOP-PRESENT-002 — livraison contrôlée après CI (11/10/2026)
- Correction code `b9072d24fd0b7452a88f8ec4cccb4b70e9cf51ae` : vrai appel `SDL_RenderPresent` exclusif à l'arène, routeur CLASSIQUE restauré pour modes historiques.
- Workflow Android `38167735625` entièrement VERT (SOLO, SDL2, regressions C++ historiques, compilation APK/AAB), aucune Release.
- Workflow extraction `38168138218` VERT : nouveaux binaires du commit corrigé dans `temp/` sur `feature/solo-coop-core-v1`, remplacement confirmé par manifeste.
- `temp/SpaceFortressVs-1.4.2.apk` : 94 119 741 octets, SHA256 `806da4277b599c7d1b19fb3c8a4ff45531bffe503475c8bb8d3b99f08d99546a`, package séparé `com.greenpower2669.spacefortressvs.diagnostic`.
- `temp/SpaceFortressVs-1.4.2-unsigned.aab` : 91 464 418 octets, SHA256 `d52c91ffb664165f0d9fd8f8d3aba1f3797a06384a6fd87835d5095c9840d10b`.
- **Attendre validation sur Samsung** : vraie arène COOP visible sans menu de sélection CLASSIQUE, avec sons, vaisseau, tir/cône et bouton boss. Pas de merge main ni publication.


## 11/10/2026 — Son cinétique + phase 2 progressive
- [x] Seuil confirmé dans moteur : début vrai cône 0.20s, prêt 2.00s, fond discret au maintien.
- [x] Supprimer lecture charge dès second FINGERDOWN ; déclencher vrai `sfKineticAudioStartCharge` sur transition à 0.20s ; READY/hold par moteur commun; release/cancel + pause.
- [x] Tests unitaires audio `coop_solo_charge_audio_regressions.cpp`.
- [x] Première préparation phase2 : map/générateur historiques + projection caméra monde/écran et collision swept par coque avec glissement, pan borné par mouvement physique, tests dédiés.
- [ ] CI du commit `9927f014` et tests Android complets puis nouvelle APK diagnostic dans `temp/` avec manifeste SHA. Ne pas livrer l'ancienne `b9072d24` sans son corrigé.
- [ ] Essai Samsung auditif : appui <.20s = tir sans son charge ; maintien .20–2.00s = charge ; 2.00s = READY et fond discret ; relâchement chargé = purge et son ; pause/reset annule.
- [ ] **Phase 2 non achevée :** raccordement du bridge à un vrai pilote COOP en coordonnées monde et rendu caméra. Ne pas translater fictivement les acteurs pour simuler pan ; valider collision et choix unité de vitesse en préservant les fonctions classiques.
- [ ] Contrôler que CLASSIQUE/COOP conservent son original et leurs états ; aucun merge main/Release.


## 11/10/2026 — Validation CI et remise APK SON-003 / WORLD-004
- Build/test du code exact `9927f0148486eefb5f49a201b9fd0602151c1905` : GitHub Actions `38169721589` VERT, jobs `solo-core-regressions`, `solo-sdl2-ui`, `build-android` tous success ; `publish-release` skipped. 
- Décompression validée `38170223921` VERTE : `temp/SpaceFortressVs-1.4.2.apk` dans `feature/solo-coop-core-v1`, 94 119 734 octets, SHA256 `47c575b49877dc1af8bceaa966b8201978612edd794a9b7e17fe987726bfd33c`, package Android diagnostic séparé. AAB non signé SHA256 `635fbc4dfac186a27f51155e34123e90218491af80efb0cf8ae02bdf74279f92`.
- Le son de charge 0,20s / READY 2,0s est implémenté mais non encore écouté sur Samsung par Fab. Phase2 pont monde/collision/caméra testé isolément, PAS encore disponible dans l'arène interactive. Pas de merge `main`, aucun changement `src/main.cpp`, aucune Release.


## 11/10/26 — SOLOCOP-WORLD-TEST-005
- [x] Raccordement EXPLICITE du vrai `sfCoopMovePlayers` à collision de coque monde (`soloWorldResolve` opt-in) ; pas de second pilote ni modification historique DUEL/COOP.
- [x] Rendu réel `sfCoopDrawArena` projeté par caméra viewport SDL; fond rock/massifs/repères monde généré et dessiné en coordonnées écran, HUD fixe.
- [x] Premier doigt dirige le pilote COOP vers cible monde avec pan seulement si mouvement réel; deuxième doigt conserve 360°, tirs/minage/son canonique.
- [x] Extension optionnelle monde aux bornes des projectiles et du champ physique historique (astéroïdes spawn de proximité).
- [x] Boutons opt-in MONDE TEST/ARENE, arène canonique BOSS TEST inchangée; double smoke native et test 80 pas vrai pilote contre murs.
- [ ] CI du commit `d86c1bfe`, run `38171610964` ; inspecter erreurs puis seulement exporter APK verte vers `temp/`.
- [ ] Samsung contrôle visuel du viewport négatif/grande carte, pilotage doigt 1, caméra seuils concentriques, parois massives sans traversée, sons 0,20/2s, fond astral.
- [ ] Intégrer vrai atlas/scenery sans cases carrées, occlusion collision tirs/missiles/astéroïdes avec murs, objectif/relais, régions miniboss/boss final, sauvegarde de progression `SFSOLO1`, sessions contextuelles multi-tenant. Ne pas réclamer que ces systèmes sont déjà livrés.
- [ ] Pas de merge main, `src/main.cpp` protégé, pas de Release.


## 2026-10-11 — WORLD TEST livraison Android vérifiée
- Lancement `feature/solo-coop-core-v1` : monde opt-in « MONDE TEST » derrière entrée diagnostique « SOLO : VRAI MOTEUR COOP » ; rendu/collisions/inertie pilotés par le VRAI moteur COOP; arène BOSS TEST conservée. Correctifs `bee803ee`, `3b52fca7`, `d86c1bfe`, `235661b3` (SDL_unsetenv absent dans SDL2, correction test uniquement) et `82825ed5` (levée doigt = vitesse SOLO remise à zéro, caméra figée).
- GitHub CI `38171993509` : TOUT VERT, tests isolated SOLO, SDL2, tests historiques de vrai COOP/collision paroi et smoke native 2 modes, compilation Android ; publish-release SKIPPED. Run précédent `38171610964` rouge UNIQUEMENT à cause du test `SDL_unsetenv`, corrigé. Ne pas effacer ses logs sans accord Fab.
- Artefact définitif extrait dans `temp/` par workflow VERT `38172484944`. APK diagnostic `temp/SpaceFortressVs-1.4.2.apk` commit source `82825ed5e2889ac630fe2379183bc068f0105d01` SHA256 `635c936e0b4fc1335077c2da9d2f236b92e69d8446761703012aa4b65754d5d2` (94 136 117 octets), identité Android séparée ; AAB NON signé SHA256 `14c0df866eba3f704912b1241a2bbbb066311691e8a1d6ad13dca0b110b06c9c`.
- Limites non résolues : absence de collisions projectiles/parois, boss déclenché par progression, relais et sauvegardes ; habillage parois diagnostique seulement. **Validation Samsung de la caméra, déplacements et sons est encore à effectuer**, CI verte ne la remplace pas.
- Aucun merge `main`, Release ni modification `src/main.cpp`.


## 11/10 — CORRECTION VISUELLE ET STABILITÉ TACTILE MONDE
- [x] Retour Fab `23110.jpg` : caméra/décor oui, anneau vert mais vaisseau bleu pas identifiable, décroche du doigt.
- [x] Supprimer viewport SDL de taille égale à toute la carte, rendu GPU écran aux coordonnées projetées `coop_solo_world_draw.hpp` avec vraies textures COOP et fallback de coque bleu.
- [x] Corriger pan caméra qui suivait tous les pas du vaisseau même lorsqu'il se trouvait loin derrière le doigt ; attente rattrapage réel + clamp surplus après doigt, collisions/vel COOP non modifiées.
- [x] Ajouter tests caméra « finger lock » et test réel par `SDL_RenderReadPixels` (présence sprite et viewport écran), log écart doigt/navire.
- [ ] Confirmer CI `38174175100` 100 % VERTE ; corriger échecs potentiels du nouveau rendu, puis exporter APK/AAB/manifest/SHA actualisés dans `temp/`.
- [ ] Tester sur Samsung : vaisseau bleu clairement visible DANS le cercle vert, vaisseau sous doigt après le temps physique de rattrapage, plus aucun décollement dû à la caméra, mouvement bord de mur bloqué physiquement, vue grand parcours.
- [ ] Ensuite seulement améliorer artworks, occlusion collisions projectiles/murs, objectifs et boss, sauvegardes.
- [ ] Aucun merge main/Release ni changement src/main.cpp.


## 2026-10-11 — Livraison vérifiée ANNEAU/VISIBILITÉ PILOTE ET CAMÉRA DOIGT
- Correctifs de code `09f8214c`, `999aedb8`, `63911245`, `e4be8b58`, `ad24535f`, `1088c265` (vaisseau bleu historique rendu dans viewport écran et projeté depuis monde, cône/ring/dust/orbes/missiles réels réutilisés, fallback lumineux si PNG absent, caméra attend le rattrapage du doigt avant de faire défiler le monde, pas de pan sur mur, vraie physique COOP préservée).
- CI exacte `38174175100` **TOUS LES JOBS VERTS**, inclut `SDL_RenderReadPixels` sur centre du vaisseau en mode monde, rendu viewport écran, test de rattrapage du doigt et smoke vrai moteur, compilation APK/AAB. Run `publish-release` SKIPPED.
- Export GitHub `38174728231` **VERT**, fichiers DÉCOMPRESSÉS dans `temp/` sur `feature/solo-coop-core-v1`. Manifeste testé commit `1088c2657642b603fb85b5cba5e7e839152f9890`; APK `temp/SpaceFortressVs-1.4.2.apk` SHA256 `a23c9d0cc5e1222b703504ea35bbd5e824b6119dcfddc30eb6a799fe1f85d76d`, taille 94 136 121 octets, package `com.greenpower2669.spacefortressvs.diagnostic`. AAB non signé SHA256 `55e40cf1000a67975a6afb0433f5747d0e0210e4aedc936204d9800944f8c2fe`.
- **À faire sur Samsung** : installer le DIAG, MONDE TEST, identifier le vrai vaisseau BLEU sous l'anneau vert de supercharge, vérifier l'adhérence au doigt immobile pendant défilement, test rapide doigt → rattrapage physique, blocage mur; signaler toute dérive sur le screenshot. Vert CI ≠ validation ergonomique téléphone.
- Jamais de merge `main`, jamais de Release, `src/main.cpp` inchangé.


## SOLO COOP CAMERA-HOLD-007 — nouveau feedback Samsung 11/10 23h48
- [x] Fab confirme : vrai vaisseau bleu visible avec l'APK `1088c265`, MAIS CAMERA maintenant immobile au doigt maintenu.
- [x] Analyse et correctif minimal : target MONDE de propulsion radiale devant doigt fixe uniquement hors cercle central ; longueur `panSpeed/30` alignée à la vraie loi vitesse COOP et au profil de caméra; caméra à nouveau bornée par déplacement effectif.
- [x] Tests unitaires boucle caméra+finger, milieu et extérieur ; test intégration réel COOP tenu 2 sec sur couloir, puis butée mur = pas de pan.
- [ ] Dernière CI `38178296951` : voir résultat, corriger si rouge, construire APK et exporter temp uniquement VERT.
- [ ] Contrôle Samsung : maintenir doigt hors HUD dans bande extérieure (sans le bouger) => décor déroule et vaisseau demeure sous le doigt ; doigts centre => STOP ; roche => vaisseau et caméra STOP ; lever => STOP, cône sonore 0.2→2s.
- [ ] Plus tard : visuels définitifs des parois, objectifs et collisions missiles/parois.
- [ ] AUCUN merge main, Release ou changement src/main.cpp.


## CAMERA-HOLD-007 — livraison contrôlée Android
- [x] Corriger propulsion continue avec doigt immobile en zones concentriques + limite max du vrai COOP, sans mouvement factice de la caméra.
- [x] Test C++ véritable 2s + butée paroi, tests vitesses toutes zones, sprite bleu déjà vérifié par SDL pixel.
- [x] Réparer condition initiale hors-bornes du test sans relâcher ses assertions.
- [x] GitHub CI `38178801383` verte et APK/AAB décompressés dans `temp/` par `38179331501` verte (source 908749e, APK SHA 372b1376...).
- [ ] Essai réel Samsung : tenir un doigt fixe dans zone extérieure hors menu, suivre décor + vaisseau sous doigt, blocage murs, STOP levé ou centre ; deuxième doigt toujours tir/minage/cône.
- [ ] Si validé par Fab : poursuivre vrai jeu SOLO (arts, collisions tirs/rochers/parois, relais, rencontres et persistance). Pas de main ni Release.


## SOLO WORLD CLEANUP-008 — 12/10/2026
- [x] Auditer les boucles `sfCoopTick`, `sfCoopDefences`, `sfCoopBonus`, le rendu et la bascule MONDE TEST.
- [x] Exclure en MONDE TEST l'IA tourelles, leurs tirs/projections et tout bonus même invisible; nettoyage de tout état résiduel à l'entrée; préserver la boucle de minage/caméra du véritable COOP.
- [x] Ajouter une régression de niveau moteur: WORLD avec boss fictif, projectile tourelle préexistant, absence d'apparition; vérifier retour COOP historique toujours actif.
- [ ] Vérifier la CI SDL2, tests historiques et Android du NOUVEAU commit; ne déclarer aucune nouvelle APK prête tant qu'elle n'est pas verte.
- [ ] Après CI verte, décompresser et vérifier la nouvelle APK, AAB et empreintes sous `temp/`, sans remplacer l'APK antérieure avant validation Samsung de Fab.
- [ ] Phase 2 ultérieure: atlas de roches définitif, collisions tirs/astéroïdes/parois, chemins/relai, ennemis/boss, sauvegardes.
- [ ] Aucun merge main ni Release publique; `src/main.cpp` intact.


### SOLO CLEANUP-008 — suite après CI verte
- [x] CI complète #38182884348 verte et publication publique SKIPPED.
- [x] Export sans ZIP #38182981381 dans `temp/solo-world-cleanup-008/` avec manifeste, checksums, APK, AAB non signé, notes ; contrôle SHA256 OK ; préservation ancienne APK.
- [ ] Installer et tester le nouvel APK sur Samsung ; confirmer disparition totale tourelles/bonus, mouvements caméra contre roche, charge 0,20s / READY 2s, tirs et poussière.
- [ ] Poursuivre progressivement parois texturées, collisions projectiles et astéroïdes sur murs, relais, boss, reprises/sauvegardes. Aucune annonce de réalisation avant code/test.


## SOLO WORLD METEOR-009 — 12/10/2026
- [x] Appliquer nouvelle correction Fab : enlever bonus collectible + tourelles dans MONDE TEST, et transformer cadence bonus en vague de vrais météores minables, recharge grâce à la poussière historique, boss affaibli par impact cinétique réel.
- [x] Tester génération de météores, intervalle sans inondation, boss impact PV, conservation bonus/tourelles hors MONDE TEST ; C++ et SDL2 verts.
- [x] Corriger Android `sa1` std::list, sans supprimer tests ; CI #38190640721 entièrement verte, publication publique sautée.
- [x] Exporter APK + AAB NON signé + SHA + manifest sous `temp/solo-world-meteor-009/`, CI #38190657369 verte ; conserver `temp/solo-world-cleanup-008/` et APK antérieures.
- [ ] Sur Samsung, entrer MONDE TEST, vérifier météorites à exploiter et poussière/recharge, puis BOSS TEST haut droit : vagues infligent dégâts au boss. Vérifier caméra, cône à 0,20 s et READY 2 s, et aucun bonus/tourelles dans MONDE TEST. Ne pas confondre avec mode ARENE ou ancienne APK.
- [ ] Suite SOLO après retour réel : équiper boss monde de vraie IA/progression/sauvegarde, gérer astéroïdes face aux parois, et enrichir décors. Ne pas merger main ni publier Release sans accord Fab.


## 12/10/2026 — SOLO-STABILISATION-010 / retour Samsung prioritaire
- [x] Recevoir et documenter le constat réel : vagues de météorites du SOLO visibles coincées dans les murs ; statut METEOR-009 = NON VALIDÉ sur téléphone malgré CI antérieure verte.
- [x] Suspendre par défaut toute nouvelle vague `sfCoopSoloMeteorites` (test explicite avec timer expiré et boss présent), sans interrompre les astéroïdes naturels/minage/recharge ni altérer modes historiques.
- [x] Auditer séparément naissance (unique appel `canOccupy` en METEOR-009, spawner de fond différent) et trajectoire (pas de collision ROCK dans `rock->ast()` / `sfLegacyFieldStep`) ; ajouter témoin géométrique « deux points libres séparés par ROCK », sans affirmer le bug définitivement diagnostiqué.
- [ ] Vérifier CI C++/SDL2/Android du commit de suspension et livrer APK diagnostique **uniquement** si toute la CI verte et vérification des fichiers/empreintes ; garder anciennes APK.
- [ ] Relever si les astéroïdes historiques restants se coincent également ; collecter capture/log horodaté ; discriminer apparition, déplacement réel, projection/rendu, contact parois. Exiger tests trajectoires réels avant toute réactivation.
- [ ] Boss du MONDE = mannequin fixe et commandes de test, pas boss de rencontre historique. Ne pas ajouter gameplay pendant stabilisation.
- [ ] Aucun merge main/release publique, aucune modification `src/main.cpp`, préserver vaisseau, caméra, maniabilité, ressources, tirs et CLASSIQUE/COOP.


### SOLO-STABILISATION-010 — livraison vérifiée
- [x] Code `6540adece` testé CI #38193630900 : C++/SDL2/Android tous SUCCESS; export #38193705276 SUCCESS avec SHA vérifié dans `temp/solo-world-stabilization-010/`. APK SHA b0cc1fad1543e4a66729a014bd6cb30f2222b9de7a6aa43c98c02dc9f4e36e64, AAB non signé. Anciens diagnostics conservés.
- [ ] Test réel Samsung APK stabilisation : aucune nouvelle vague METEOR-009, astéroïdes fond encore présents/minables, identifier s'ils se coincent dans ROCK. La sortie correcte de la vague N'IMPLIQUE PAS correction du trajet des rochers historiques.
- [ ] Examiner séparément le résolveur `moveWithoutTunnelling` au vu d'un échec initial synthétique (cas deux points libres séparés par mur), **sans changer le pilotage existant dans cette mission**. Ne pas annoncer confirmé sur appareil.


## SOLO TURRETS OFF-011 — correction ciblée (12/10/26)
- [x] Inspecter le manifeste GitHub de l'APK stabilisation-010 : commit `6540adece493acfe25dbf066310c1fd2480a7487`; **installation Samsung exacte inconnue**. Mettre repère visible `SOLO-011` + log pour comparaison.
- [x] Démontrer par source le chemin ARENE (non MONDE) `soloDiagnostic` sans garde `soloWorldMode` : bonus → temps des tourelles → déploiement/tirs kind5 → dessin tactique. Identifier un éventuel visuel WORLD distinct avec Fab, sans inventer de causalité.
- [x] Garde global diagnostic SOLO, état/purge projectiles au démarrage et transitions boss, bonus/IA stoppés, rendu ARENE non-tourelles; préserver toutes ressources/armes/caméra/sons + CLASSIQUE/COOP.
- [x] Tests moteur nouveau `testSoloDiagnosticTurretsRemainAbsent` avec deux lancements, boss off/on, non régression historique, vague météores ajoutée toujours suspendue.
- [x] Historique SOLO-011 : CI C++/SDL2/Android 38209069899 et export 38209723734 déjà validés ; ancienne case ouverte corrigée le 13/10.
- [ ] Vérification physique Samsung dans MONDE et ARENE, boss OFF/ON après relancement, HUD `SOLO-011`, vaisseau/mine/recharge/météorites historiques, aucun effet/tir/collision/dessin de tourelle. Identifier photo de tout objet douteux sans conclure trop vite.
- [ ] Aucune modification main/src/main.cpp, aucun merge ni release, ne pas activer les vagues METEOR-009.


### SOLO-011 — artefacts définitifs et tests restant sur téléphone
- [x] CI C++/SDL2/Android `38209069899` **SUCCESS** source `ad5cb08e`, `publish-release` ignoré.
- [x] Export GitHub `38209723734` **SUCCESS** après réconciliation de l'avancement concurrent des mémoires ; APK `temp/solo-turrets-off-011/SpaceFortressVs-1.4.2.apk` SHA256 `513693f6b2b7fd07b810562fe8fc0c73e336eb3c2ebab02db980fc20106f0e21` (94 136 124 octets), AAB non signé présent, anciens fichiers conservés.
- [ ] Essai physique Samsung sur **édition HUD `SOLO-011`** : ARENE boss OFF/ON, MONDE boss OFF/ON, relance, aucune IA/tir/contact/affichage tourelle ; confirmer et fournir description/capture des éventuels objets ambigus. Ne pas prétendre boss historique achevé ni collision météores réparée.


## SOLO BONUS CORE-012 — 12/10/2026
- [x] Référence distante1c6c0b5, ordres/mémoires/sources et planches examinés ; travail isolé, modification PNG antérieure laissée intacte.
- [x] Cœur huit pouvoirs, collecte, cumul borné et tests purs.
- [x] Tir humain réel/salves, targeting furtif, statuts/dégâts boss, drops accessibles.
- [x] Rendu sprites/FX/HUD commun arène et labyrinthe, reset/relaunch.
- [x] Régressions réelles + SDL2/Android CI ; APK/AAB dans temp/solo-bonus-core-012/, SHA vérifié.
- [ ] Retour Samsung : ergonomie/FPS, statuts boss monde ne prouvent pas mobilité.

### CORE-012 — intégration et revue, 12/10/2026
- Brique C++ pure + adaptateur du vrai SfCoopState + rendu commun arène/labyrinthe implémentés. Salves humaines x3, guidage historique, coût canonique une fois ; furtivité semi-transparente conserve collisions ; missiles3s sans coût ; statuts Prison2s/Glace4s/Lave4s et Météore ciblé. Ancien bonus/tourelles et vagues METEOR-009 toujours exclus du SOLO.
- Revue indépendante : seuils enregistrés au moment du dégât avant soin poussière ; boss SOLO mort non ressuscité par poussières ; taille de coque identique au pilote monde ; BFS accessible calculé une fois par paquet, emplacements distincts. Coûts bornés et logs Android `SOLO_BONUS_012` moyenne/max temps simulation+rendu pour validation Samsung.
- Précontrôles : core pur GREEN, source SDL compilée ; régressions complètes/visuel/Android encore en cours à cet instant. Tests ajoutés par Sol puis poursuivis par Astra après interruption de son exécution. Benchmark hôte seulement : 12 récompenses/carte54×160, 12 lots, moyenne2,270ms maximum6,612ms ; aucune promesse de FPS Samsung.
- `src/main.cpp` inchangé. Pas de merge main/Release. Identifiant HUD SOLO-012 et carte accueil SOLO BONUS012. APK/AAB à livrer uniquement depuis CI verte exacte dans temp/solo-bonus-core-012/.

### CORE-012 — contrôle local terminé, candidat CI
`scripts/test-regressions.sh` termine code0 : historique C++/SDL2, assets, moteurs, 200 ouvertures/rendus COOP, champs et poussières cinétiques, source Android générée et smoke natif arène/monde. Nouveaux tests purecore, tirsréels/statuts/collisions/loot/isolation passent. Dernier ajustement HUD SOLO (plus de double HUD COOP, textes agrandis) recompilé et testé avec lecture pixels SDL ; capture inspectée. Contrôle pont monde réexécuté après ajout des24placementsdistincts ; protection launcher diagnostic OK. Le boss monde reste FIXE de test, Prison/Glace y montrent des statuts, mouvement seulement démontré sur boss mobile de l’arène. CI Android et export exact restent à constater ; APK non annoncée avant cela.


## CORE-012 — livraison vérifiée 13/10/2026
- [x] CORE-012 code, huit pouvoirs, tests, CI exacte et export APK/AAB avec empreintes.
- [ ] Samsung : boss OFF/ON, arène/labyrinthe, relance, chaque pouvoir, contrôles et performances.
- [ ] Futures missions : IA mobile du boss monde, raccord progression/sauvegardes, astéroïdes historiques face aux parois.

Source `95743f2386b220bb692112197c4ddee02d27ae2b` ; [CI Android/C++/SDL2 verte](https://github.com/greenpower2669/SpaceFortressVs/actions/runs/38299079636), publication Release SKIPPED.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-012-95743f2.apk` : 94168883 octets, SHA256 `c61d7e39ecb075d69db863e7cc964021bb9c3ed31ccf0c0597b01b1ffaa09248`.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-012-95743f2-unsigned.aab` : 91525121 octets, SHA256 `a4433fc2f09cae480ab407f1997139821602caa71c0c0f8a61941baceeed6289`.

## SOLO BONUS-013 — 13/10/2026
- [x] Faire naître visuellement les pickups depuis le boss au lieu de les afficher directement à la destination ; bloquer collecte/rendu final pendant le vol.
- [x] WORLD : destination dans la composante accessible du pilote et segment d’éjection boss→destination sans ROCK ; ARENE : dispersion visuelle légère autorisée.
- [x] Ajouter `TEST BONUS OFF/ON`, 8 boutons individuels + `TOUS X8`; fermeture du panneau sans pause/reset ; boss requis.
- [x] Relever uniquement le HUD joueur SOLO au-dessus de la navigation Android ; HUD boss inchangé ; fallback Samsung si usable bounds absent.
- [x] Isoler les variations d’explosion d’astéroïde des autres FX : deux prises historiques + faible variation de volume, canal dédié.
- [x] Tests C++/SDL2 BONUS-013 ajoutés et verts sur les commits intermédiaires.
- [x] Confirmer le workflow Android du HEAD final BONUS-013 et relever run/artifact/SHA avant APK téléphone.
- [ ] Validation Samsung : TEST BONUS fermé/ouvert, huit boutons + TOUS X8, éjection visible depuis boss, collecte après atterrissage, barres joueur au-dessus des boutons système, variations audio discrètes.
- [ ] Après validation seulement : décider d’une livraison APK 🟢 / AAB 📦 dédiée. Aucun merge main ni Release.

## BONUS-013 — livraison vérifiée 13/10/2026
- [x] CI finale BONUS-013 exacte, APK/AAB et SHA256 vérifiés/exportés.
- [ ] Samsung: confirmer panneau OFF/ON, huit bonus + TOUS X8, éjection depuis boss, HUD au-dessus navigation et variété FX.

Source `488de2195e2aa2a1e40f2eec96abaeba19863fad`; CI Android/C++/SDL2 `38361581767` SUCCESS; publish-release SKIPPED.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-013-488de21.apk`: 94185274 octets, SHA256 `4a3511c380d04b21b46593806e01d991e9320c44691c529c6022e60aa4d1a18b`.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-013-488de21-unsigned.aab`: 91532662 octets, SHA256 `3fb742b7b3964620b5f0b8eb76dae0d0fc2b18f2c2e245d87809b08468a68d3a`.

## BONUS-014 — livraison vérifiée 13/10/2026
- [x] BONUS-014 : atlas animé production 4x8 intégré et validé.
- [x] BONUS-014 : Prison/Glace/Lave/Météore animés en vol, impact et statut boss.
- [x] BONUS-014 : HUD SOLO Samsung utilise le safe-bottom inset, HUD boss inchangé.
- [x] BONUS-014 : pitch+tempo astéroïdes réellement bornés à ±3 %, volume légèrement varié.
- [x] BONUS-014 : TEST BONUS + TOUS X8 atomique et fermeture sans interruption.
- [x] BONUS-014 : CI Android/C++/SDL2 exacte verte et APK/AAB exportés.
- [ ] Validation physique Samsung : jauges, animations quatre pouvoirs, TOUS X8, naturel des FX audio.
- [ ] Aucun merge main / Release avant validation Fab.

Source `f9323f3cd239e9027d964e38db369a9217ef02cb`; CI Android/C++/SDL2 `38420099395` SUCCESS; publish-release SKIPPED.
- Atlas animé: `assets/pict/solo/SoloBonusAnimations.png`, 1536x768, 4x8 frames de 192 px, SHA256 `1a337adb15a6a3901b76da566dcd26a1d2d76c26893864a37a85e97b63950001`.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-014-f9323f3.apk`: 94989574 octets, SHA256 `0dfe83dbb1f2716e33442a2d2933356b1a6582180a85a9c2fed5a77b1ebe59ff`.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-014-f9323f3-unsigned.aab`: 92342299 octets, SHA256 `564b0c9427bd776ed93bcb32d3012d86d232f3354ffded5d85b020cfd7424965`.


## SOLO GAME-015 — plan actif, 14/10/2026
- [x] Audit validé, autorisation de coder et précision « plusieurs miniboss » consignées ; base 0c3f4a1.
- [x] Extraire l’hôte SOLO partagé, isoler le diagnostic, ajouter le vrai sélecteur et l’activité interne Android.
- [x] Plusieurs miniboss accessibles et espacés, IA simple, dégâts réels, projectiles historiques et zéro bonus.
- [x] Antre physique, transition zoom/ondulation, arène finale commune à BOSS TEST, conservation PV/énergie/pouvoirs.
- [x] Fin de combat permettant la collecte, progression/sauvegarde SOLO sans ancien moteur Session.
- [ ] Tests ciblés, régressions complètes, revue indépendante, CI Android et APK dans temp/solo-game-015/.
- [ ] Samsung : entrée/sortie/reprise, caméra, rencontres, lisibilité transition, bonus et FPS. Non testé sur appareil.
Plan détaillé : `docs/superpowers/plans/2026-10-14-solo-game-015.md`.

GAME-015 / étape 1 : sélecteur SOLO, activité interne `:sologame`, hôte commun et droits diagnostic séparés implémentés localement. Test ciblé RED (injection hors diagnostic acceptée auparavant) puis GREEN ; launcher statique GREEN. Suite globale arrivée aux tests cinétiques finaux sans erreur visible, statut de processus perdu lors de la reprise de session : une exécution finale avec code retour enregistré reste requise. Android et Samsung non encore validés.

GAME-015 : hôte/selector/activité, quatre miniboss, antre/arène, transition et résultat durable codés. Tests ciblés gameplay GREEN ; parcours vérifiés sur 18 cartes représentatives (6 mondes × stages 1/10/20, difficultés 1/5/9), transition/reset/fallback et sauvegarde/rollback GREEN. Restent suite globale avec code retour enregistré, revue indépendante, Android CI puis export APK/Samsung.

GAME-015 / contrôles complémentaires : snapshot haute densité et état SDL vérifiés ; temps résiduel de transition corrigé ; furtivité conserve le contact miniboss, test RED puis GREEN ; anciens FX libérés au changement de scène. En cours : revue finale, suite globale fraîche et CI Android.

GAME-015 / validation locale finale 15/10/2026 : `bash scripts/test-regressions.sh` terminé avec code 0 ; tests ciblés de furtivité/contact et changement de scène verts. Revue indépendante finale sans défaut critique/important restant. `src/main.cpp` conserve le blob 835059a0ecfe0f74708068b3259cad5db1cdb579. CI Android et export restent à confirmer au SHA du commit envoyé. La modification locale ultérieure de ParoisModeSolo.png est séparée et exclue du commit GAME-015.

## GAME-015 — livraison vérifiée 15/10/2026
- [x] GAME-015 : vrai sélecteur, hôte partagé et processus Android isolé.
- [x] GAME-015 : quatre miniboss, aucun bonus, antre/transition/arène commune.
- [x] GAME-015 : progression compatible, reprise au premier niveau non terminé.
- [x] GAME-015 : revue, régressions C++/SDL2 et CI Android exacte verte.
- [x] GAME-015 : APK/AAB exportés et SHA256 vérifiés.
- [ ] Samsung : fluidité, équilibre, cycle Android, transition, collecte/victoire et relance.
- [ ] Anciennes collisions astéroïdes/parois : hors de cette mission ; METEOR-009 reste OFF.

Source `f5b4ce4f82e99057d041767d982914e903afbf7f`; CI Android/C++/SDL2 `38724287864` SUCCESS; publish-release SKIPPED.
- Atlas animé: `assets/pict/solo/SoloBonusAnimations.png`, 1536x768, 4x8 frames de 192 px, SHA256 `1a337adb15a6a3901b76da566dcd26a1d2d76c26893864a37a85e97b63950001`.
- `SpaceFortressVs-1.4.2-SOLO-GAME-015-f5b4ce4.apk`: 95023204 octets, SHA256 `f5138def32cdb8553e86006f6e079ce6a61a979b764c6c62f1757aca61ca8ec7`.
- `SpaceFortressVs-1.4.2-SOLO-GAME-015-f5b4ce4-unsigned.aab`: 92378949 octets, SHA256 `1b4d9a9bffb7ade770ec033bd25162c20c440c9a01db71ad8613c2156aee0e7c`.

## 16/10/2026 — SOLO PARALLAX-016
- [x] Nouvelle planche alpha et raccord des ressources existantes.
- [x] Décor réversible et carte physique immuable.
- [x] Revue : correction des jointures à échelle fractionnaire ; test pixel vert.
- [ ] Suite complète et CI Android exacte.
- [ ] APK/AAB vérifiés dans temp/solo-parallax-016.
- [ ] Samsung : aspect des parois/répétitions, lisibilité, fluidité et pause/reprise/antre.
- [ ] Hors périmètre : astéroïdes stationnaires, nouvelle couronne caméra, poursuites miniboss.

PARALLAX-016 — export préparé : source `2a933a60639d890fd6168befb8ce1784ba9a18c8`, CI `38837760766`. Le workflow attend obligatoirement SUCCESS et vérifie le SHA exact avant publication dans `temp/solo-parallax-016/`. CI/export pas encore déclarés terminés.

## PARALLAX-016 — livraison vérifiée 16/10/2026
- [x] PARALLAX-016 : nouveau fond, ressources existantes et rendu du vrai SOLO.
- [x] Revue, correction des jointures et tests SDL à plusieurs échelles.
- [x] CI C++/SDL2/Android du commit exact et export APK/AAB avec SHA256.
- [ ] Samsung : aspect, lisibilité, répétitions, fluidité, pause/reprise et antre.
- [ ] Hors périmètre : astéroïdes stationnaires, nouvelle couronne lente et nouvelles poursuites miniboss.

Source `2a933a60639d890fd6168befb8ce1784ba9a18c8`; CI Android/C++/SDL2 `38837760766` SUCCESS; publish-release SKIPPED.
- Atlas animé: `assets/pict/solo/SoloBonusAnimations.png`, 1536x768, 4x8 frames de 192 px, SHA256 `1a337adb15a6a3901b76da566dcd26a1d2d76c26893864a37a85e97b63950001`.
- Fond ajouré : `assets/pict/solo/FondsParallaxeModeSolo.png`, RGBA1254x1254, SHA256 `4becaec70bf7f23d823c3eccbd902b86aa4dbb6942115a1389409bc6a7f7df44`.
- `SpaceFortressVs-1.4.2-SOLO-PARALLAX-016-2a933a6.apk`: 98400063 octets, SHA256 `62a029ca828a85783f324301bcb5d1411cfa8f343875f7e5ebf4108b5ce8515f`.
- `SpaceFortressVs-1.4.2-SOLO-PARALLAX-016-2a933a6-unsigned.aab`: 95691410 octets, SHA256 `1c18b7d99a45816a2332cd77415927cbf26cbb93cc1346897160c18eafc76703`.


## 16/10/2026 — SOLO PHYSICS-017
- [x] Base PARALLAX-016/branche et cinq mémoires inspectées.
- [x] Contrat + plan PHYSICS-017 enregistrés.
- [ ] Inertie/caméra/zoom et tests.
- [ ] Roches stables/trajectoires/audio et tests.
- [ ] 10–12 rencontres/chasse/meutes et tests.
- [ ] Régressions, revue, CI Android, export APK/AAB exact.
- [ ] Samsung : essais nécessaires avant toute intégration définitive.
État : en cours, aucun code PHYSICS-017 livré.

PHYSICS-017 — point de reprise 16/10 après interruption : implémentation locale des briques (inertie/caméra/zoom, repos/collision roches, visibilité audio, 10–12 rencontres/chasse) présente, NON LIVRÉE. Tests purs inertie Danger9/FPS, contact/repos roches, zoom exclusif, navigation réelle/occlusion et placement des rencontres passés. Suite complète a révélé une erreur de portée de variable dans le son du minage, corrigée ; compilation globale/tests runtime à reprendre. Environnement /tmp réinitialisé entre les tours, dépendances SDL en reconstitution dans scratch persistant de la session. Aucun APK-017 annoncé, aucune validation Samsung. Prochaine étape : tests réels moteur, revue, CI Android, export exact.

## PHYSICS-017 — implémentation et revue, 16/10/2026
- [x] PHYSICS-017 : inertie/caméra/zoom, roches/audio, rencontres et compteur dynamique implémentés.
- [x] Revue indépendante et corrections ; tests ciblés réels incluant labyrinthes/Danger9, fragments résiduels et angles de murs.
- [ ] Suite exacte C++/SDL2/UBSan et Android sur CI du commit source.
- [ ] APK/AAB vérifiés et décompressés dans temp/solo-physics-017/.
- [ ] Essai Samsung et accord Fab avant toute intégration définitive.

## PHYSICS-017 — livraison vérifiée 16/10/2026
- [x] PHYSICS-017 : implémentation du vrai SOLO, revue et correctifs.
- [x] Tests C++/SDL2/UBSan/Android et non-régressions du commit exact.
- [x] APK diagnostic + AAB non signé dans temp/solo-physics-017, manifeste/SHA256 vérifiés.
- [ ] Samsung : inertie Danger1..9, murs, dérive/caméra, gisements, meutes, sons, zoom et transition/final.
- [ ] Accord explicite Fab avant toute intégration définitive/main/Release.
- [ ] Nouveaux bruitages : attente des fichiers de Fab, intégration entièrement reportée.

Source `3ef362cd1375034519c458a0effb2d7abef03490`; CI Android/C++/SDL2 `38956636032` SUCCESS; publish-release SKIPPED.
- Atlas animé: `assets/pict/solo/SoloBonusAnimations.png`, 1536x768, 4x8 frames de 192 px, SHA256 `1a337adb15a6a3901b76da566dcd26a1d2d76c26893864a37a85e97b63950001`.
- Fond ajouré : `assets/pict/solo/FondsParallaxeModeSolo.png`, RGBA1254x1254, SHA256 `4becaec70bf7f23d823c3eccbd902b86aa4dbb6942115a1389409bc6a7f7df44`.
- `SpaceFortressVs-1.4.2-SOLO-PHYSICS-017-3ef362c.apk`: 98432836 octets, SHA256 `8387c28fb63ae6d2fe4017af5ac3b29b49c906682e5124c8273031d6cebb2ac9`.
- `SpaceFortressVs-1.4.2-SOLO-PHYSICS-017-3ef362c-unsigned.aab`: 95725985 octets, SHA256 `3af4730f3104606e4e6f3af694baa3165a9cc3447174705c4553b72d6736b95f`.


## SOLO-018 — rebond immédiat et geste tactile direct, 16/10/2026
- [x] Lire PHYSICS-017 et reproduire causalement l'arrêt `velocity=0`, le blocage doigt-mur et la règle de premier doigt.
- [x] Intégrer sur la branche SOLO le solveur de rebond, la déviation, la gestion du reste du frame et le décollage face au doigt maintenu ; préserver l'inertie libre.
- [x] Réserver le pilotage au DOWN sur coque ; réserver deux DOWN hors coque à un pincement ×1..×2, sans tir/charge au moment de reconnaissance.
- [x] Ajuster tests moteur/bridge/zoom pour remplacer le contrat historique d'arrêt.
- [x] CI complète Android/C++/SDL2/UBSan SUCCESS sur code a85ca0d (run 38963255438), après correction du test de stress multichocs initial.
- [x] APK diagnostic et AAB **non signé** directement dans temp/solo-bounce-pinch-018/ : fichiers complets et SHA256SUMS vérifiés, workflow d'export 38964102751 SUCCESS.
- [ ] Essai Fab Samsung : murs face/angle, dégagement sans latence, maintien du doigt, pinch sans tir ni changement de vitesse et 30/60/120 FPS ; valider le ressenti.
- [ ] Aucune fusion main/Release sans validation explicite Fab. src/main.cpp, assets, audio et modes historiques inchangés.

SOLO-018 / CI reprise : test de stress multichocs à 100000 px/s corrigé par `a85ca0d` après l'échec de `38962472820`. Nouvelle CI `38963255438` à examiner avant livrable Android. Aucun nouveau feu vert Samsung demandé pour coder ; accord maintenu pour le correctif SOLO, non pour main/Release.

## SOLO-018 — export vérifié
SOLO-018 : CI exacte 38963255438 SUCCESS, code a85ca0dbd8e94e463d11e42b5b2cefb32ab31116. Export vérifié dans temp/solo-bounce-pinch-018. APK diagnostic + AAB non signé. Rebond et touch/zoom restent à valider sur le Samsung de Fab ; pas de main/Release.


## SOLO-019 — portions mini-boss/hordes, joystick de tir
- [x] Comprendre la correction de Fab : **aucun verrou de l'antre final** ; uniquement les portions de rencontres mini-boss ou hordes.
- [x] Verrouillage local pendant la rencontre, groupe de horde ouvert au dernier KO ; rendu énergétique et résistance physique du pilote.
- [x] Joystick tir semi-transparent gauche, tir au relâchement selon vecteur, tap reprend dernier vecteur ; huit boutons aux angles fixes 45°.
- [x] OPTION SOLO pour miroir gauche/droite et stockage local, sans modifier options historiques.
- [x] Tests de groupe de hordes et de joystick 8 angles ajoutés au script de régressions.
- [x] Collision de macro `pi` historique résolue par `padPi` sans modifier FabLib.
- [ ] CI Android/C++/SDL2 exacte sur SHA `568d689`, run `38969053715`.
- [ ] Export direct APK diagnostic / AAB non signé dans `temp/solo-encounter-fire-019/`, vérification du manifest et SHA256.
- [ ] Tests Samsung du contact, ouvertures aux derniers KO, tir 8 directions, joystick et options miroir ; validation Fab.
- [ ] Aucun merge main ni Release sans validation. `src/main.cpp` protégé.

## SOLO-020 — demande Fab zone plus grande et scroll proche centre
- [x] Barrières de mini-boss isolés et hordes, rayon x1,5, pas de changement d'antre.
- [x] Ajouter une pré-bande de défilement autour du centre dès ~deux diamètres de vaisseau avec smoothstep et progression continue vers la vitesse existante.
- [x] Partager la nouvelle loi entre cible doigt-maintenu et caméra sans altérer ShipMotion ou les modes historiques.
- [x] Renforcer tests 8 directions/seuils de vitesse et rayons mini-boss/hordes.
- [x] CI Android/C++/SDL2 SHA dd6a4c7, run 38973185112 SUCCESS.
- [x] APK diagnostic + AAB non signé en fichiers directs temp/solo-cruise-020/, SHA256 vérifiés (export 38973239202 SUCCESS).
- [ ] Essai Samsung sensation défilement proche centre, collisions, barrières agrandies et joystick.
- [ ] Main et Release interdits sans validation explicite Fab.

## SOLO-020 — livraison CI + dépôt direct vérifiés
- Code source `dd6a4c75066fd51a359c00bbd952a32674eaaa25` ; Android/C++/SDL2 CI `38973185112` **SUCCESS**.
- Export GitHub `38973239202` **SUCCESS**, fichiers réels dans `temp/solo-cruise-020/` : APK diagnostic 98 449 216 octets, SHA-256 `ad683345d31f6857cc90589557905931a4076634ea69781bc636e8887ddfffb0` ; AAB non signé 95 742 279 octets, SHA-256 `16dfdbc3948a8d2689aa43d0320ee1a1f1f09b82e3e5b388fa59749f4bfccbfa`. Sommes vérifiées par workflow. Pas de main ni de Release.
- **Reste :** validation tactile réelle Samsung (défilement proche centre, x1.5 zones, dégâts/barrières et joystick).

## SOLO-021 — parois du premier plan, correction réversible
- [x] Vérifier renderer de la vraie campagne SOLO et comparer les bandes de paroi 200×58 au fond/parallaxe indépendant.
- [x] Tester répétition de matière proche de la densité native et filtrage nearest sur les six textures opaques uniquement.
- [x] Réduire biseaux progressifs qui atténuaient les contours ; ne toucher à aucun PNG, à la carte ou à la physique.
- [x] Enregistrer dans `ordres-de-mission.md` la base `cf6cc6e`, le commit `f896211` et la commande de revert isolée.
- [x] Ajouter test texture source, sampler des six murs, sampler linéaire des deux plans de fond aux régressions existantes.
- [x] CI Android/C++/SDL2 (premier run 39018857560 a détecté un nom de variable test ; correction 04da08d ; run 39019113903 SUCCESS).
- [x] Export APK + AAB diagnostic directs temp/solo-wall-crisp-021/, manifest et SHA256 vérifiés ; run 39019145459 SUCCESS.
- [ ] Tester la netteté comparée et les raccords sur Samsung (zoom x1/x2, déplacement, falaises, parois masquant les acteurs) ; revenir au commit original si motifs trop répétitifs ou pixels trop durs.
- [ ] Aucune fusion dans `main`, aucune Release sans validation explicite Fab.


### SOLO-021 — résultat définitif (17/10/2026)
- **Source exécutée** : `04da08db80f543a081a7a3614f85c1b2fd28e7c1`, code de rendu `f896211164481cede61d0073729f6eb3362be177`, état de référence précédant l'essai `cf6cc6e479ce854c3e64ddb00f6b9566d9cf52a9`.
- **CI verte** : GitHub Actions `39019113903` (logiciel SOLO, SDL2, Android), toutes les tâches requises SUCCESS, publication Release SKIPPED.
- **Export GitHub direct vert** : run `39019145459`, sous `temp/solo-wall-crisp-021/`.
- **APK de diagnostic** : `SpaceFortressVs-1.4.2-SOLO-WALL-CRISP-021-04da08d.apk`, 98 449 218 octets, SHA-256 `5a58885b9a76e52e4c88ceeffd044c5b0862ef421b0a4b4a8848b0dbf38cea43`.
- **AAB non signé** : `SpaceFortressVs-1.4.2-SOLO-WALL-CRISP-021-04da08d-unsigned.aab`, 95 742 237 octets, SHA-256 `3ae61f0554464c29cec1a0640ad7cc922ba7c047a18e6fae3a04412b4085d4fc`.
- **Comparaison à faire sur Samsung** : SOLO-020 avant ↔ SOLO-021 après, à zoom 1 et 2 et en mouvement ; noter netteté, effet pixel, raccord et répétition du rocher. Aucun résultat de qualité visuelle vécu ne peut être attribué au téléphone sans test réel.
- **Retour arrière sans perte de travail** : `git revert 04da08db80f543a081a7a3614f85c1b2fd28e7c1` puis `git revert f896211164481cede61d0073729f6eb3362be177` sur feature/solo-coop-core-v1, suivi de tests. Garder cette trace dans ordres-de-mission. Pas de `main` ni Release.

## SOLO-022 — croisière ultra-lente 1 → 2 diamètres
- [x] Historique confirmé : début SOLO-020 à deux diamètres ; point de retour `9e064b552a6a7bead00817fb4bed04f4dd0d5288`.
- [x] Nouvelle couronne de défilement douce d'un diamètre à deux diamètres, vitesse 0→0,025 puis 0,025→0,1 sans arrêt au raccord.
- [x] Anciennes vitesses extérieures et physique conservées ; loi commune doigt/caméra.
- [x] Tests seuils, continuité, monotonie, huit directions, mouvement réel de la caméra ajoutés.
- [x] CI C++/SDL2/Android `39040122861` SUCCESS sur `6c79f88`.
- [x] Export direct GitHub `39040168281` SUCCESS : APK et AAB dans `temp/solo-inner-cruise-022/`, SHA256 vérifiés.
- [ ] Tester le confort de la nouvelle couronne sur Samsung.
- [ ] Conserver main / Release intacts.

- [x] SOLO-022 code source et tests regroupés `6c79f88addd673c4be8a8efc853c091ee94ae675`.
- [x] Chemin de retour arrière isolé `git revert 6c79f88addd673c4be8a8efc853c091ee94ae675` documenté dans ordres-de-mission.md.
- [x] Workflow de livraison conditionnelle `solo-inner-cruise-022-export.yml` créé et épinglé au bon SHA.
- [x] CI complète source `39040122861` et export direct `39040168281` SUCCESS.
- [ ] Tester sur Samsung la croisière 1D→2D à vitesses très faibles, 8 directions, sans arrêt sur la frontière 2D et sans changement du pilotage ou rebond.

### SOLO-022 — validation finale et téléchargement direct — 17/10/2026
- Commit source exact `6c79f88addd673c4be8a8efc853c091ee94ae675`; CI `39040122861` **SUCCESS** pour C++/SDL2/Android, Release **SKIPPED**.
- Export GitHub `39040168281` **SUCCESS**. Fichiers réellement présents dans `temp/solo-inner-cruise-022/`, sommes SHA256 contrôlées par workflow et manifest.
- APK de diagnostic installable : `SpaceFortressVs-1.4.2-SOLO-INNER-CRUISE-022-6c79f88.apk`, 98 449 221 octets, SHA256 `540fdae8cc278550622bcfa4ba11d9ae7eccb774111a529bbc4365118b84cab8`.
- AAB non signé, pas destiné à Google Play : `SpaceFortressVs-1.4.2-SOLO-INNER-CRUISE-022-6c79f88-unsigned.aab`, 95 742 235 octets, SHA256 `130ab9ad27041aeb7711f86518a3b368063e1aab9c07902a220430c70b93b4ae`.
- La fonction de défilement démarre désormais à un diamètre, atteint 0,025 à deux diamètres, reste monotone jusqu'aux zones précédentes. Toujours aucune modification physique, des textures ou des modes historiques.
- **À valider par Fab sur Samsung** : sensation de lenteur à la nouvelle bande, absence d'arrêt à deux diamètres, cohabitation avec touches de tir et pincement. Retour arrière isolé : `git revert 6c79f88addd673c4be8a8efc853c091ee94ae675` sur la branche SOLO en conservant cet ordre de mission. Ni main, ni Release.


### SOLO-022 — résultat final vérifié, 17/10/2026
CI source **39040122861 SUCCESS** (Android + C++ + SDL2), export direct **39040168281 SUCCESS**. APK 98 449 221 octets sous `temp/solo-inner-cruise-022/SpaceFortressVs-1.4.2-SOLO-INNER-CRUISE-022-6c79f88.apk`, SHA-256 `540fdae8cc278550622bcfa4ba11d9ae7eccb774111a529bbc4365118b84cab8`. AAB non signé disponible au même endroit, 95 742 235 octets, SHA-256 `130ab9ad27041aeb7711f86518a3b368063e1aab9c07902a220430c70b93b4ae`. Tests Samsung de comportement réel non encore faits. Main et Release intacts. Revert isolé `git revert 6c79f88addd673c4be8a8efc853c091ee94ae675` sur la branche SOLO, sans reset ni force-push.
## SOLO-023 — habillage des rebords rectilignes
- [x] Créer le générateur reproductible de la planche des 48 pièces et son workflow de dépôt d'un vrai PNG direct GitHub.
- [x] Générer sur la branche l'atlas 1536×512 (16 voisinages N/E/S/W × 3 variantes).
- [x] Brancher les sprites aux six thèmes des parois Rock, sans changer la collision ou les textures existantes.
- [x] Garder l'ancien renderer de secours, les ressources SDL libérables/rechargeables, et le premier/second passage identiques.
- [x] Ajouter régressions silhouette/alpha, six thèmes et raccords/zoom.
- [x] CI complète du code SOLO-023, correctifs tests inclus : 39058401438 SUCCESS.
- [x] Export APK diagnostic/AAB direct temp/solo-rock-rims-023/ avec SHA256, sans ZIP.
- [ ] Vérifier sur Samsung : rebords organiques (fin des carrés), lisibilité du passage, aucune collision perçue anormale, différence de rendu à zoom haut.
- [ ] Main / Release interdits sans validation.


- [x] SOLO-023 : source `39d0a7c2b6969f546a7dc7537b3bcf993b097dcb`, point de comparaison `13e212927c3ef63e3b7f431e71cbee81b0b2a2f8`.
- [x] Planche physique PNG 1536×512 et 48 silhouettes créée par `39057218668` et vérifiée dans dépôt.
- [x] Rollback autonome code seul `git revert 39d0a7c2b6969f546a7dc7537b3bcf993b097dcb` écrit et vérifiable.
- [ ] Terminer CI `39057515808` et export direct temp/solo-rock-rims-023/ ; ne pas annoncer la réussite avant les résultats.
- [ ] Essayer sur Samsung, puis itérer artistiquement seulement si rendu trop répétitif ou masque visuel gênant.


- [x] SOLO-023 correction du nom du test du premier CI rouge : commit `51752da4`, nouveau CI `39057829358`.
- [x] Workflow d'export repointé vers CI/SHA test corrigés, sans reprendre le premier build rouge.
- [x] CI Android/C++/SDL2 run 39058401438 SUCCESS et export direct 39058427941 SUCCESS ; APK+AAB et SHA256 contrôlés.
- [ ] Tester les silhouettes de roches sur téléphone, surtout raccords, coins, déplacement et zoom.


- [x] SOLO-023 : second correctif de régression `57f74d8c` : opacité corps de roche / alpha découpé coin, CI `39058137853`.
- [x] Documenter rollback 3 commits, sans perdre l'ancienne planche ni supprimer l'historique.
- [x] CI finale `39058401438` verte et export `39058427941` vert, APK/AAB directs SHA256 vérifiés.


### SOLO-023 — livraison finale vérifiée, 17 octobre 2026
- Code du nouvel habillage `39d0a7c2b6969f546a7dc7537b3bcf993b097dcb`, correction premier test `51752da4dd74fffc9b8c0a1448b2d6eb27fd3fcc`, deuxième `57f74d8c4c7437d01cf84d7edea200a57bf4e672`, troisième **`48b30c4a6d7c200fc2979bb411a3a048b4a3348f`** (supprime une ancienne assertion RGB littérale devenue incompatible avec la roche recomposée ; l'opacité et les coins transparents restent testés).
- **CI source entière verte** run `39058401438` : suite SOLO, SDL2 et Android ; Release publique SKIPPED.
- **Export direct vert** run `39058427941`, `temp/solo-rock-rims-023/` ; les fichiers réels ont été vérifiés par métadonnées Git et leurs SHA256.
- APK diagnostic : `SpaceFortressVs-1.4.2-SOLO-ROCK-RIMS-023-48b30c4.apk`, **98 710 354 octets**, SHA256 `8ef3769b08a200f382465907486f28c36d54b300294fa17fdafcbb8c0cdb0e71`.
- AAB non signé : `SpaceFortressVs-1.4.2-SOLO-ROCK-RIMS-023-48b30c4-unsigned.aab`, **95 998 932 octets**, SHA256 `df8a5097fe619a2316bb51cc8705bdbded916d3f92225af5dc2a1aacbdfe065b`.
- **Rollback complet code/test, après validation utilisateur** : sur feature/solo-coop-core-v1, faire DANS CET ORDRE
  1. `git revert 48b30c4a6d7c200fc2979bb411a3a048b4a3348f`
  2. `git revert 57f74d8c4c7437d01cf84d7edea200a57bf4e672`
  3. `git revert 51752da4dd74fffc9b8c0a1448b2d6eb27fd3fcc`
  4. `git revert 39d0a7c2b6969f546a7dc7537b3bcf993b097dcb`
  Résoudre si nécessaire conflits de mémoire et **conserver la présente note d'historique**, puis CI. Atlas et générateur restent conservés mais inutilisés, retrait séparé seulement si explicitement demandé.
- Point à vérifier par Fab sur Samsung : la fin des carrés, la répétition éventuelle du matériau rocheux, la sensation de collision près des rebords, le zoom/pan et la fluidité. Validation perceptive non revendiquée avant essai réel.
- Aucun merge main, aucun tag/Release.


### SOLO-023 — Livraison verte finale, 17/10/2026
- Code testé exact `48b30c4a6d7c200fc2979bb411a3a048b4a3348f`. Dernier ajustement de l'assertion sur une ancienne couleur murale sentinelle, devenu inadapté à la matière recomposée ; commit `48b30c4`. Le code graphique et l'atlas ne sont pas retouchés.
- GitHub Actions **39058401438 SUCCESS** : C++ SOLO, SDL2, Android. Publication de Release SKIPPED. Workflow direct **39058427941 SUCCESS**.
- APK directement installable `temp/solo-rock-rims-023/SpaceFortressVs-1.4.2-SOLO-ROCK-RIMS-023-48b30c4.apk`, **98 710 354 octets**, SHA256 `8ef3769b08a200f382465907486f28c36d54b300294fa17fdafcbb8c0cdb0e71`.
- AAB **non signé**, `temp/solo-rock-rims-023/SpaceFortressVs-1.4.2-SOLO-ROCK-RIMS-023-48b30c4-unsigned.aab`, **95 998 932 octets**, SHA256 `df8a5097fe619a2316bb51cc8705bdbded916d3f92225af5dc2a1aacbdfe065b`.
- **Retour arrière logiciel complet, après accord Fab, branche SOLO uniquement** : `git revert 48b30c4a6d7c200fc2979bb411a3a048b4a3348f` puis `git revert 57f74d8c4c7437d01cf84d7edea200a57bf4e672` puis `git revert 51752da4dd74fffc9b8c0a1448b2d6eb27fd3fcc` puis `git revert 39d0a7c2b6969f546a7dc7537b3bcf993b097dcb`. Vérifier les diffs, résoudre tout conflit documentaire et relancer CI. Ne pas supprimer les ressources historisées, ni les mémoires. Ni reset, ni force-push, ni merge main ni Release.
- Reste : **validation visuelle réelle Samsung** du contact parois/collisions, coins, zoom et répétition des motifs.


- [x] SOLO-023 : correction du test sentinel RGB périmé dans `48b30c4` ; CI complète `39058401438` SUCCESS, export direct `39058427941` SUCCESS.
- [x] APK direct 98 710 354 octets sous `temp/solo-rock-rims-023/SpaceFortressVs-1.4.2-SOLO-ROCK-RIMS-023-48b30c4.apk`, SHA256 vérifié ; AAB non signé présent.
- [x] Retour arrière logiciel complet par les quatre commits de code/test documenté dans ordres-de-mission.md, sans enlever l'atlas.
- [ ] Validation esthétique et comportementale sur Samsung : rebords rocheux non carrés, reconnaissance des passages, variations, zoom et collision réelle.
## SOLO-024 — contours de longue portée et fond seul près du vaisseau
- [x] Audit renderer : les 48 masques de SOLO-023 sont toujours des carrés isolés.
- [x] Code source de contour organique continu par morceaux avec cache limité, original PNG intact.
- [x] Intégration du rendu de roche et conservation fallback SOLO-023.
- [x] Ajouter la dérive des fonds seulement en anneau 0,5→1 diamètre puis raccord vers scrolling normal 1→2 diamètres.
- [x] Ajouter des tests indépendants de dérive, silhouette, cache, continuité.
- [x] CI complète `39067948776` SUCCESS après correction alpha.
- [x] APK diagnostic direct et AAB non signé exportés via `39068019221`, vérifiés.
- [ ] Tester sur Samsung : arrière-plan qui bouge sans caméra près de la coque, rivages naturels et collision, zoom et pas de répétitions.
- [ ] Jamais de merge main ou Release sans validation.


## SOLO-024 — état de la réalisation
- [x] Couper le lien artificiel entre texture des bords et cellule individuelle : continuité de tout contour en coordonnées monde, cache borné.
- [x] Garder les textures existantes et l'ancien habillage SOLO-023 en secours.
- [x] Ajouter parallaxe seule à proximité 0,5→1 diamètre du vaisseau et transition lisse à 2 diamètres.
- [x] Respecter collisions et caméra originales, et défilement normal SOLO-022 au-delà.
- [x] Enrichir tests de contour, cache, opacité, dérive/zoom et seuils.
- [x] Corriger risque double alpha de masquage des acteurs : commit `2946e9a`.
- [x] Documenter baseline `d245f0d` et revert exact de 3 commits dans ordres-de-mission.
- [x] CI C++/SDL2/Android `39067948776` entièrement SUCCESS.
- [x] Export direct `temp/solo-organic-024/` via run `39068019221` SUCCESS, SHA256 vérifiés.
- [ ] Tester réellement sur Samsung l'organique du terrain et le fond seul en zone proche.
- [ ] Ni main ni Release sans validation explicite.


### SOLO-024 — livraison vérifiée 17/10/2026
- Source définitive `2946e9a30cfc9923ccd6371c16a2121c66abc1a3`, CI complète Android/C++/SDL2 **39067948776 SUCCESS**, aucune Release (SKIPPED).
- Export direct **39068019221 SUCCESS**. Vrais binaires présents dans `temp/solo-organic-024/`.
- APK debug installable `SpaceFortressVs-1.4.2-SOLO-ORGANIC-024-2946e9a.apk`, **98 710 352 octets**, SHA-256 `25cff97cf6439b52d38d5de092d452f68cdd34c071308decde29d8e74aad930b`.
- AAB non signé `SpaceFortressVs-1.4.2-SOLO-ORGANIC-024-2946e9a-unsigned.aab`, **96 006 869 octets**, SHA-256 `4aec7e7ea8028551c2c1cf2362301c785c12996aac3496f45efe81927f315454`. Hash et ZIP/artefact source vérifiés dans workflow.
- À vérifier perceptivement sur Samsung : contours longs à l'intérieur/extérieur des roches, répétitions visuelles, absence d'artéfact d'occlusion, effets de défilement du fond dans zone de rayon 0,5→1 diamètre, raccord à 1→2 diamètres, collision logique inchangée. CI n'est pas preuve de confort visuel.
- Revert source si demandé par Fab : `git revert 2946e9a30cfc9923ccd6371c16a2121c66abc1a3`, puis `git revert 599f6a82347f10f25ed9354b35c6f8d895f72ea4`, puis `git revert 210afab295dd2277986fe44b7377a4e9c2bfd5dd`, branche `feature/solo-coop-core-v1` uniquement, résoudre les conflits de documentation sans supprimer cette trace, relancer CI. Ne pas réinitialiser, force-push, fusionner main ou faire Release.

## SOLO-025 — anti-répétition et FX météores Djipie
- [x] Découverte vérifiée des 4 vrais assets nommés `Meteor_explosion_rock_contre_rock1..4` dans Release privée Djipie, tailles et SHA-256 exacts.
- [x] Texture roche : sampling déformé/nuancé en coordonnées monde, lueurs bleues moins répétitives (`91e024457...`).
- [x] Son : shuffle-bag de quatre prises, protection anti-repeat, séparé pour roche-roche/destruction ; sons murs/champs préservés (`b74b062...`).
- [x] Ajouter contrôle d'assets privés par secret (jamais de WAV public) et inspection SHA-256 de l'APK.
- [ ] Valider CI et tests C++/Android complète.
- [ ] Vérifier si `DJIPIE_READ_TOKEN` existe et a le droit de lire Djipie (sinon configuration explicite requise).
- [ ] Livrer APK diagnostic direct **avec** 4 nouveaux sons uniquement si le test `DJIPIE_METEOR_TAKES=4` a réussi.
- [ ] Tester le ressenti sonore et les roches moins répétitives sur Samsung ; rollback séparé.

- [x] Corriger l'ordre d'inclusion de sfCoop dans la physique des roches (signal autonome `b6e0a38`).
- [x] Conserver les quatre WAV privés hors Git public.
- [x] Créer le workflow privé Djipie `feature/solo-025-private-meteor-apk`, `d57710a`, accès autonome aux quatre sons via GITHUB_TOKEN Djipie, sans merge main ni Release.
- [ ] Source corrigée CI Android/C++/SDL2 `39085021465` SUCCESS.
- [ ] APK public de variété avec état audio explicite via export `solo-variety-025-export.yml`.
- [ ] APK **privé Djipie** 4/4 authentiques : vérifier workflow `39085282683`, taille < GitHub limite et fichier direct.
- [ ] Tester sur Samsung le rythme de 4 prises, collision roche-roche vs mur, absence de répétition, matière anti-papier-peint.



### SOLO-025 — clôture CI, essai terrain restant
- [x] Corriger portée du signal C++ `b6e0a38c`, CI Android/C++/SDL2 `39085021465` SUCCESS.
- [x] Constater l'absence de `DJIPIE_READ_TOKEN` public et ne pas annoncer à tort 4/4 dans l'APK public (0/4).
- [x] Construire dans Djipie privé, workflow `39085282683` SUCCESS, les 4 vrais Meteor WAV incorporés et vérifiés dans l'APK.
- [x] APK privé direct sans ZIP, 99 919 413 octets, SHA256 `f08b1f9c9df78e0562b063ffd20a9fdba672147762e25063ee0b4cf4d0df7c6e`.
- [ ] Fab : tester sur Samsung le relief moins répétitif et les explosions variées (roche contre roche, destruction, autres collisions inchangées).
- [ ] Aucun merge main / Release sans validation explicite.

## SOLO-026 — audit 18/10/2026, aucun code autorisé
- [x] Prendre le retour Fab : bonne base SOLO, mais minage irrégulier, near-scroll incohérent, murs insuffisamment pénalisants, réserve trop généreuse.
- [x] Identifier les points source : minage 2,025 diamètres vs visuel/aspiration 3,30 ; fond seul SOLO-024 ; collisions sans PV/nrj ; recharge passive multiplier 4→2.
- [ ] Après **nouveau GO de Fab**, réconcilier indication visuelle du cône et vraie zone minière (vérifier danger 1→9, orientation, bords, vitesse d'érosion). Ne pas changer DUEL/COOP sans accord.
- [ ] Après GO, remplacer near-band « arrière-plan seul » par scroll **cohérent du labyrinthe**, parallaxe relative de fonds, zone ultra-lente de transition 1D→2D réellement perceptible ; préserver centre précis, vitesse intermédiaire, inertie, rebond, zoom, geste continu.
- [ ] Après GO, pénaliser impacts de paroi SOLO en coque + énergie selon vitesse normale, avec seuil bas et protection anti-multiplication par frame, tout en maintenant le glissement et les rebonds.
- [ ] Après GO, réduire fortement recharge énergétique passive SOLO et sa fin de courbe vers 100 % ; rendre 100 % atteignable soit après vraie patience, soit plus vite par minage/poussières. Ne pas modifier PV par inadvertance, ni règles originales CLASSIQUE/COOP.
- [ ] Créer/étendre tests de géométrie cône rendu/minage, caméra toutes bandes et réversibilité, paroi choc-vitesse-cooldown, courbe récupération passive/minage, danger 1→9, boss/bonus, hors-champ et tests tactiles Samsung.
- [ ] Au stade autorisé seulement : CI C++/Android/SDL2, essai Samsung, APK direct `.apk` sous `temp/` de branche de travail + AAB non signé identifié, SHA256, jamais de merge main/Release sans accord.

### SOLO-026 — code engagé suite au GO Fab (18/10/26)
- [x] Aligner la géométrie de minage SOLO avec la surface du cône visible, tout en conservant le facteur niveau Danger 1→9 ; historiques intacts.
- [x] Faire bouger les couches parallaxes uniquement avec le mouvement de caméra physique, y compris la première petite zone 0,5D→1D ; labyrinthe suit la même caméra.
- [x] Donner au contact de paroi SOLO des pertes coque+énergie par vitesse normale avec seuil et anti-recontact répétitif.
- [x] Remplacer la recharge passive SOLO trop rapide par une récupération lente 100% atteignable, poussières/minage avantageux ; COOP/DUEL non modifiés.
- [x] Désactiver l'accueil hub de diagnostic Android : debug APK ouvre directement SOLO ; l'écran debug se réveille après 20 touches haut alternées G/D, debug uniquement.
- [x] Ajouter tests purs survie, modifier régressions caméra et contrôler manifeste launcher.
- [ ] Vérifier CI exacte Android/C++/SDL2 + tests complets ; corriger toute panne avant livraison.
- [ ] Vérifier l'export automatique direct APK et AAB NON SIGNÉ sous `temp/solo-026/` après CI green ; relever SHA256 sans invention.
- [ ] Vérifier sur Samsung : portées et difficultés, pilotage doigt, camera + fond dès première zone, rebonds/dégâts normal/cooldown, recharge, bonus, prise main en jeu sans accueil, 20 taps, relance depuis hub.
- [ ] Confirmer disponibilité sons Meteor 4/4 : le build public peut être fallback et ne doit jamais être décrit comme bundle Djipie si 0/4.
- [ ] Pas de merge main, pas de Release, pas de signature release sans validation Fab.

### SOLO-026 — livraison vérifiée CI et APK (18/10/2026)
- [x] CI finale C++/SDL2/Android sur `788c03c` : run `39132530943` SUCCESS.
- [x] Export direct exact SHA vert sous `temp/solo-026/` : run `39132531008` SUCCESS.
- [x] APK `SpaceFortressVs-1.4.2-SOLO-026-788c03c.apk` direct, 98 711 306 octets, SHA256 `c6e4412ca0c867f2d37f18060a74cb4f99ec6bda26fe6577affe0c1b28950a96`.
- [x] AAB `SpaceFortressVs-1.4.2-SOLO-026-788c03c-unsigned.aab` 96 011 406 octets, SHA256 `2ebc55ec5edcd07fbf58642fdd5eea5f9dd4c1b4fd70ee7d1cf8b38f836cc9de`.
- [ ] Réintégrer les 4 WAV privés de Djipie dans un nouveau build SONORE distinct uniquement avec validation du processus d'accès privé : **actuel 0/4**, fallback historique.
- [ ] Retour Samsung (ressenti cône Danger1→9, vraie première parallaxe, collisions dangereuses mais glissantes, recharge et 20 taps debug).

### SOLO-027 — double-stick, entrée modes et nouveau panneau OPTIONS
- [x] Remettre en launcher diagnostic le véritable menu SOLO / COOP / DUEL; conserver le HUB caché par geste debug 20 taps alternés.
- [x] Tir turquoise continu orientable au doigt (vecteur mémorisé), rafales 1,25s et repos 1,50s, cadence et chaleur historiques respectées.
- [x] Stick violet déplacement : même ShipMotion/difficulté et inertie ; contrôle par cible virtuelle physique au lieu de repositionnement du vaisseau.
- [x] Option pilotage au doigt sur vaisseau OU mini-stick et inversion G/D des deux commandes simultanément ; choix persistants, rétrocompatibles.
- [x] Deux joysticks néon quasi transparents aux codes couleurs turquoise/violet, sentinelle titres `Tourelle.png` au-dessus du bouton Options technologique.
- [x] Tests commandes SDL et préflight launcher mis à jour.
- [ ] Attendre CI exact SHA `android-build.yml` Android/C++/SDL2 et corriger les échecs, sans faux GREEN.
- [ ] Export exact SHA validé direct vers `temp/solo-027/*.apk` (et `-unsigned.aab`) via `.github/workflows/solo-027-direct-export.yml`, SHA256 et manifeste ; jamais ZIP sur appareil Fab.
- [ ] Faire les essais Samsung de 20 taps/hub, retour menu après jeu, tir continu pause et re-tap, switch synchrone G/D, inertie Danger1→9, modal et sécurité de la navigation Android.
- [ ] Revalider présence des 4 WAV privés (si 0/4, le dire : fallback ancien), ne pas divulger sons Djipie.
- [ ] Validation Fab avant merge main/Release.

### SOLO-027 — livraison directe vérifiée le 18/10/2026
Source exacte `d861a22221abaaf08ec516a2a2b9123b0a537e41` : CI Android+C+++SDL2 `39136940735 SUCCESS` ; export GitHub Actions `39136940825 SUCCESS`, livraison binaire sur la branche de travail `temp/solo-027/`. APK direct installable `SpaceFortressVs-1.4.2-SOLO-027-d861a22.apk`, taille 98 727 555 octets, SHA256 `615143149e07af1481bbb490e5d5749fbda30aa20ba6e0935096435186460366`. AAB non signé `SpaceFortressVs-1.4.2-SOLO-027-d861a22-unsigned.aab` taille 96 014 785 octets, SHA256 `4b9895c0768b1c36e3b97c1b9a9843e8476d7c316b5049842876cde8e3687f5b`. Son Meteor Djipie privé non disponible dans cet APK public : `0/4` WAV, fallback audio classique. Pas de test sur Samsung effectué par l'assistant : valider prise en main réelle du menu/hub 20 taps, stick optionnel, swap synchronisé G/D, tir avec pauses et fluidité. Aucun merge `main`, aucune Release. **Règle immuable : extrait `.apk` directement par GitHub Actions sur serveur et disponible par lien direct sans ZIP utilisateur.**

### SOLO-028 — mini-stick modulé, jeu non assombri, écran Game Over
- [x] Diagnostiquer saturation vitesse cible joystick avant bord ; conserver formule ShipMotion et difficulté.
- [x] Agrandir anneau et surface de pilotage, course centrale lente et progression douce jusqu'au bord ; conversion amplitude cible monde dimensionnée par maxSpeed/12.
- [x] Supprimer fond disque plein turquoise/violet, éclaircir et affiner cercle/curseur ; le décor reste visible.
- [x] Ajouter panneau « GAME OVER » + bouton tactile « MENU PRINCIPAL » dans SOLO production, intercepter l'input pendant défaite et quitter proprement vers menu historique.
- [x] Ajouter tests graduations, course, lent/rapide par difficulté, zones tactiles GameOver et régressions de build.
- [ ] Attendre Android/C++/SDL2 CI **du commit exact** verte et export direct `temp/solo-028/*.apk` via GitHub Actions ; enregistrer SHA256 confirmé, ne pas inventer de fichier.
- [ ] Essais Samsung : 5/20/50/75/100 % stick gauche/droit, sensibilité près du centre, inertie au relâchement, aucun masquage décor, Game Over après destruction, retour au vrai menu, hub debug caché.
- [ ] Vérifier sons privés Meteor inclus 0/4 ou 4/4 sans exposer fichiers Djipie ; AAB non signé.
- [ ] Pas de merge main ni Release sans accord explicite Fab.

### SOLO-028 — preuves de compilation et livraison 18/10/2026
- [x] SOLO-028 CI exacte `39143165760` : moteur C++, UI SDL2, Android et régressions **verts**.
- [x] Extraction sur GitHub `39143165717` : direct APK `temp/solo-028/SpaceFortressVs-1.4.2-SOLO-028-f50dbbe.apk`, 98 727 554 octets, SHA256 `4f9fc3a2ecd4be84e05af57cac1a7e2dbd4ae86f7693b1edc97f4bf9b0114c25`.
- [x] AAB `-unsigned.aab` présent, 96 016 719 octets, SHA256 `fe4513dc64382b1f0a033444eed68ded2a49b96e676868a39e750892a09a31c9`.
- [ ] Essais Samsung : douceur stick 10/25/50/75/100%, premier anneau, décor non assombri, Game Over et vrai menu parent après tap, gestes zoom et swap. 
- [ ] Sons Meteor Djipie privés non disponibles dans cet APK public (0/4) ; ne pas déclarer 4/4.
- [ ] Accord Fab avant fusion main/Release.

### SOLO-029 — caméra anticipation contrôlée
- [x] Remplacer suivi par zones écran de la caméra MONDE SOLO par suivi du vrai vaisseau et anticipation de vitesse 360° plafonnée.
- [x] Vitesse nulle = anticipation revient à zéro et recentrage visuel, sans freiner ou déplacer physiquement le vaisseau.
- [x] Inversion/recul suivent vitesse réelle et amortissement, pas la direction brute du joystick.
- [x] Doigt sur coque = zéro fixé au contact initial, course de même courbe/reach que stick violet et indépendante du cadrage.
- [x] Préserver monde/parois/fond/parallax avec une seule vraie caméra ; zoom conserve ancre, marquages caméra historique intacts pour anciens tests.
- [x] Ajouter tests caméra freinage/inversion/recentrage et test commande doigt vs stick.
- [ ] Valider CI finale Android, SDL2 et C++ sur SHA EXACT et livrer apk `temp/solo-029/` déjà extrait côté serveur + SHA-256, sans ZIP.
- [ ] Essais Samsung : visibilité devant dès centre, plus d'anticipation en accélérant, recadrage à l'arrêt, marche arrière, zoom et stick/doigt, absence de tremblement/accélération parasite.
- [ ] Ne pas déclarer les 4 sons privés Djipie s'ils manquent ; aucun merge main/release sans accord Fab.

### SOLO-029 — livraison source CI verte et APK direct vérifié (18/10/2026)
Source testée `e7bb328a4373beb941b6002e5efcc50c8ab5c965`. CI `Android APK + AAB` run `39153071193` **SUCCESS**, trois suites core C++/SDL2/Android vertes, `publish-release` SKIPPED. Export GitHub `SOLO-029 CI-green APK direct` run `39153071248` **SUCCESS**. Fichier INSTALLABLE déjà extrait sur GitHub branche `feature/solo-coop-core-v1` : `temp/solo-029/SpaceFortressVs-1.4.2-SOLO-029-e7bb328.apk`, taille **98 727 558 octets**, SHA256 `f2e93f6fcda37091dd721287a82d7f4825007d3d532af1e18250d7b90a78a95d`. AAB **non signé** : `temp/solo-029/SpaceFortressVs-1.4.2-SOLO-029-e7bb328-unsigned.aab`, taille **96 015 350 octets**, SHA256 `1cd371a10415cee437602d7edfb44fe4b8af9e953dc46e0a135b85bae382f97a`. Meta dans `temp/solo-029/*-build.json` ; Meteor privés `0/4` dans cet APK public, sons historiques de secours, ne pas revendiquer 4/4. Aucun ZIP utilisateur, aucun merge main, aucune release publique. Essai réel de caméra tactile Samsung encore à réaliser avec Fab.

### SOLO-030 — course douce et vecteur cumulatif
- [x] Allonger la course joystick de déplacement et déplacer son centre pour que le débattement reste dans l'écran.
- [x] Rampe cubique plus douce au centre, vitesse maximale conservée au bord.
- [x] Bonus de commande progressif par tenue de vecteur stable près du bord, limité à +.30 de throttle / vitesse max historique.
- [x] Réinitialiser bonus sur changement de direction significatif, relâchement, retour centre et pause / cancel.
- [x] Appliquer la même logique au pilotage doigt sur coque, sans interaction avec caméra ni tir.
- [x] Étendre tests C++ de courbes, maintien, inversion, stabilité infinie et difficulté.
- [ ] Attendre CI Android+C+++SDL2 verte exacte et livrer APK direct SOLO-030 `temp/solo-030/*.apk` ; SHA256 et manifest vérifiés.
- [ ] Valider sur Samsung la douceur des petits déplacements et le temps d'élan dans la couronne ; pas de tremblement en inversion.
- [ ] Tenir les sons privés Meteor (0/4 ou 4/4) distincts ; pas de merge main/Release.

### SOLO-030 — livraison CI verte et APK direct (18/10/2026)
Source exacte `626a24cb021bba6b665f6015567555a8cd9f345f`. Run CI `39156942027` **SUCCESS**, trois suites core C++/SDL2/Android et régressions vertes ; `publish-release` SKIPPED. Export officiel GitHub Actions de branche `39156941944` **SUCCESS**, extrait côté serveur dans `temp/solo-030/` sans ZIP utilisateur. APK de diagnostic installable `temp/solo-030/SpaceFortressVs-1.4.2-SOLO-030-626a24c.apk` **98 727 544 octets**, SHA256 `6e3ad65efe01c759988dd8076bf0c667805a4c0b3d6decb7457d269d10e9a3ec`. AAB non signé `temp/solo-030/SpaceFortressVs-1.4.2-SOLO-030-626a24c-unsigned.aab` **96 017 166 octets**, SHA256 `274b7269b13727a457e22bb59c60bed77ac3f8bde9590f2486bf05aab147ed0c`. `*-build.json` et `SHA256SUMS` vérifiés. Meteor Djipie privé absent `0/4` WAV, sons historiques de secours; ne pas annoncer les sons nouveaux. Tests physiques sur Samsung toujours à réaliser par Fab : réglage de douceur et bonus de maintien, marche arrière et inertie, affichage, main G/D. Aucune fusion main ni Release.
