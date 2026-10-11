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
