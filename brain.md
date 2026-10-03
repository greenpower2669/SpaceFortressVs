# brain.md — SpaceFortressVs

## Référence de reprise
- Dépôt : `greenpower2669/SpaceFortressVs`.
- Branche de travail : `fix/gameplay-campaign-200`.
- Sauvegarde Astra : commit `9b8cae378f66d461dd8a29f9504972f648b2c40c`.
- Arbre Git vérifié par Sol : `92cf3b03b2aeaf30da6d57b85f126f89cf37caf4`.
- Base fonctionnelle antérieure : v1.3.1, commit `dcf80afb6eb117d405f46f03d8567d31a76b4b12`.
- Version préparée : 1.4.0, versionCode 10.

Cette mémoire a été reconstruite par Sol le 26 septembre 2026 à partir du code récupéré, de `todo.md`, de `docs/superpowers/plans/2026-09-20-gameplay-200.md`, de `docs/releases/1.4.0.md` et des tests présents. Les trois mémoires `brain.md`, `brainmap.md` et `debughistorical.md` étaient absentes de la sauvegarde Astra ; ne jamais prétendre qu'elles étaient synchronisées auparavant.

## Contrat fonctionnel à préserver
- Préserver le moteur historique, `src/main.cpp`, les sauvegardes, les 50 portraits de boss et l'écran de fin stable.
- Aucun transfert massif Base64 et aucun remplacement gratuit des assets historiques.
- Aucun ajout fonctionnel ni refonte dans cette reprise : corriger uniquement un blocage reproduit par des vérifications fraîches.
- Aucun merge dans `main` et aucune publication de release sans nouvel accord explicite de Fab.
- Livrables Android attendus après vérification : `SpaceFortressVs-1.4.0.apk` et `SpaceFortressVs-1.4.0-unsigned.aab`.
- Ne jamais demander de désinstaller l'application ni d'effacer ses données pour contourner une incompatibilité de signature.

## Comportement présent dans le code récupéré

### Campagne 200
- 200 affrontements = 50 identités × 4 difficultés.
- `sfBossIndex(encounter) = encounter % 50`.
- `sfDifficultyIndex(encounter) = encounter / 50`.
- Difficultés : VIF, ENDURANT, VICIEUX, ULTIME.
- La progression, la sélection, les victoires en attente et le Hall of Fame couvrent jusqu'à 200 rencontres.

### Signature visuelle des difficultés
- Même fonction de rendu en sélection et en combat : `sfDrawEncounterBoss`.
- Niveau 1 : aucune aura, 0 tentacule.
- Niveau 2 : aura verte, 4 tentacules.
- Niveau 3 : aura jaune, 8 tentacules.
- Niveau 4 : aura rouge, 20 tentacules.
- Les reflets restent localisés afin de conserver le portrait original au centre.
- Le sélecteur affiche aussi le niveau et les informations de boss sans générer 200 portraits distincts.

### Coop, tirs, énergie et tourelles
- Premier doigt de chaque pilote : déplacement.
- Doigts supplémentaires : tir au tap ; maintien/déplacement du doigt de tir ne doit pas produire d'auto-tir.
- Tirs humains ordinaires : axe du camp ; pas de guidage en vol.
- Missile : tir spécial à réserve pleine selon la sémantique historique `nrj`.
- `nrj=0` signifie réserve pleine ; `nrj=50` réserve épuisée.
- Le bouclier dépend de la réserve historique.
- Les tourelles coop restent inactives sans bonus.
- Le bonus flottant les active temporairement ; leur réserve d'énergie est distincte de celle des pilotes.

### Champ historique partagé
- Duel et coop utilisent le même propriétaire de champ historique.
- Astéroïdes mobiles, collisions, fragmentation, minage, minerai, impacts et collecte restent basés sur les primitives historiques.
- Les limites de population sont bornées par le runtime.

### Sauvegardes
- Lecture des formats v1 et v2.
- v1 reste limitée à 50 rencontres ; v2 couvre 200.
- Migration v1→v2 sans perte des noms UTF-8, scores, Hall of Fame ni victoire en attente.
- Conservation durable de l'original avant réparation/migration ; les formats inconnus restent protégés.

## Preuves et niveaux de confiance

### Rapporté par Astra — passation, pas preuve fraîche Sol
Astra rapporte avant sauvegarde : 32 groupes SDL/UBSan réussis, 21 groupes du champ historique, migration v1/v2, neuf tests Python, 200 rendus du sélecteur et simulations de combats. Ces résultats servent de contexte mais ne doivent jamais être présentés comme exécutés par Sol.

### Vérifié directement par Sol avant CI
- La branche distante existe et pointe sur `9b8cae378f66d461dd8a29f9504972f648b2c40c`.
- Le commit possède exactement l'arbre `92cf3b03b2aeaf30da6d57b85f126f89cf37caf4`.
- Le source récupéré contient la campagne 200, le rendu 0/4/8/20 partagé sélection/combat, les commandes tactiles, l'énergie historique, le bonus tourelles, le champ partagé et la migration v1/v2.
- `android/version.properties` contient 1.4.0 / code 10.
- `android-build.yml` exécute `scripts/test-regressions.sh`, construit APK/AAB, puis vérifie/versionne les paquets.
- `package-release.py` compare le certificat du nouvel APK au certificat publié de la v1.3.1.

### Vérifications fraîches Sol encore requises
- Exécution CI de `scripts/test-regressions.sh` sur le nouveau HEAD contenant ces mémoires.
- Vérification du dernier ajustement des textes du sélecteur, y compris portrait/paysage.
- Compilation APK/AAB.
- Intégrité des PNG, bibliothèques et archives.
- Comparaison réelle du certificat 1.4.0 à celui de la v1.3.1.
- Essais physiques sur téléphone : quatre doigts, reprise après veille, champ vivant, énergie, bonus/tourelles, son, performances et équilibrage des quatre difficultés.


## Vérification fraîche Sol — workflow 36246639403
Exécutée sur le commit testé `2ca6aa46e732e9d9e86e9889ad2d215117c1f16e`.

- Workflow : https://github.com/greenpower2669/SpaceFortressVs/actions/runs/36246639403
- `scripts/test-regressions.sh` : succès.
- Python : 3 tests + 6 tests = 9 tests réussis.
- Suite native SDL/UBSan : 32 groupes PASS.
- Champ historique généré Android : 21 groupes PASS.
- Fixture campagne : v1 réelle + round-trip v2 jusqu'à l'affrontement 200.
- Sélecteur : PASS portrait/paysage, onglets de difficulté et lancement de l'affrontement 200 avec le portrait du boss 50.
- Rendu : PASS sur 200 rendus, niveau 1 historique inchangé, 0/4/8/20 tentacules, aura visible et reflets localisés.
- Combat : PASS sur 200 ouvertures avec boss mobiles, attaques hostiles, champ vivant et absence d'auto-tir humain.
- Progression : PASS sur 200 coups finaux, 200 victoires nommées durables, frontière finale et Hall complet après rechargement.
- Build Gradle : succès.
- Packaging : versionCode 10, signature APK, intégrité ZIP, bibliothèque ARM64 et assets vérifiés.
- Artefact release : https://github.com/greenpower2669/SpaceFortressVs/actions/runs/36246639403/artifacts/10907503442

Fichiers produits :
- `SpaceFortressVs-1.4.0.apk` — 87372144 octets — SHA-256 `2045bc64083b624e14ecec034df350b03eaa0eeac269d145d171506d5b4f3b81`.
- `SpaceFortressVs-1.4.0-unsigned.aab` — 84880594 octets — SHA-256 `a30870bece5759ddd4feb2ee4d1cf84a0e06822798afbea1e7624438d727939c`.

Signature :
- certificat 1.4.0 : `9817ba2bdf29226c72529ec161f8124f11cbee89b2748ca440b1e38148e01781`;
- certificat publié 1.3.1 : `8abfc11c8bc4f9ac065eb5c086ad4e457290bcbbc1105017865368de7e565868`;
- résultat : différents. L'APK 1.4.0 est un APK de test et n'est pas une mise à jour directe compatible avec l'installation 1.3.1.

Essais téléphone encore requis : quatre doigts réels, veille/reprise, son, performances, champ vivant, énergie/bonus/tourelles et équilibre des quatre difficultés.


## Retour téléphone Fab — 26 septembre 2026 — 1 régression classique + 8 anomalies coop

Ce retour provient d'un essai physique de l'APK 1.4.0. Il prime sur les simulations automatisées pour les défauts visuels et de sensations de jeu. Ne pas transformer ces observations en causes techniques avant audit.

### Mode classique — régression 1
1. **Astéroïdes invisibles à l'écran alors que le champ existe.** Les poussières/effets issus des astéroïdes sont visibles, ce qui indique que la simulation ou au moins une partie des interactions continue. Le sprite/rectangle de rendu des astéroïdes n'apparaît pas. Première consigne d'audit : isoler rendu vs simulation et ne pas toucher au moteur physique tant que la cause visuelle n'est pas prouvée.

### Coop — anomalies 2 à 9
2. **Missile visuellement faux.** Le missile se déclenche bien mais son apparence est celle d'un plasma/tir ordinaire. Restaurer une apparence missile distincte sans changer sa logique si celle-ci est correcte.
3. **Rapport dégâts/récupération trop favorable.** Les dégâts reçus paraissent faibles alors que les poussières d'astéroïdes rendent énormément de survie. Auditer précisément ce qui est restauré en pratique (PV, énergie ou effet indirect de bouclier) avant équilibrage.
4. **Barres de vie pilotes absentes.** Les PV des deux pilotes doivent être lisibles en jeu, distincts de l'énergie.
5. **Boss trop fragiles face aux tourelles.** Dès que les tourelles sont actives, un boss peut être détruit presque instantanément. Auditer PV/résistance des boss et DPS réel des tourelles, sans casser les quatre niveaux de difficulté.
6. **Contact direct avec le boss trop peu punitif.** Un vaisseau qui reste au contact du boss doit voir fondre très rapidement son énergie puis ses PV tant que le contact persiste.
7. **Tirs de l'IA alliée tous chasseurs.** Les projectiles ordinaires de l'allié IA ne doivent pas tous corriger leur trajectoire en vol. La visée peut être prédictive au départ, mais seuls les projectiles explicitement guidés doivent poursuivre une cible.
8. **Énergie insuffisamment couplée au tir.** Quand la réserve baisse, la cadence de tir et la précision doivent se dégrader progressivement ; pleine énergie = cadence/précision maximales.
9. **Impacts d'astéroïdes trop peu dangereux.** Une collision réelle avec un astéroïde doit retirer davantage de PV ; si le moteur historique prévoit taille/vitesse/énergie d'impact, réutiliser ces grandeurs plutôt qu'une constante arbitraire.

### Observations positives du même essai
- Le premier boss a pu être lancé et joué.
- Le tir au tap du joueur du bas fonctionne sur téléphone.
- Le joueur du haut et les scénarios réellement simultanés restent à vérifier avec un deuxième joueur.

### Discipline d'audit
Les anomalies 3, 6, 8 et 9 sont liées par la chaîne survie/énergie/dégâts. Les auditer ensemble avant de choisir des constantes : **impact → énergie/bouclier → PV → récupération → cadence/précision**. Le bug 1 doit au contraire être isolé comme régression de rendu classique jusqu'à preuve du contraire.


## Complément retour téléphone — joueur haut et HUD
- Le joueur du haut a maintenant été testé physiquement en coop : **RAS** sur son contrôle/tir.
- Comme ce joueur joue depuis l'autre côté de l'écran, ses informations HUD doivent être **affichées à 180°** afin d'être lisibles depuis son orientation réelle.
- Ajouter une barre de vie du boss sans valeur numérique imposée.
- Cette barre doit être pensée pour la lecture des deux côtés de l'écran : présentation inversée côté joueur rouge / haut.
- Code couleur demandé pour la vie : **vert à pleine vie → rouge à vie faible/vide**.
- Ces points sont des exigences d'affichage issues du test téléphone ; aucune implémentation n'a encore été faite.


## Lot correctif autorisé par Fab — 26 septembre 2026
Fab valide l’audit Astra et autorise le code.
- D-140-01 : qualification de la fonction historique `::setw` uniquement dans la copie Android générée ; `src/main.cpp` reste inchangé.
- D-140-02 : texture historique missile pour `kind=4`.
- D-140-04 : HUD dédoublé pour les deux orientations : vie boss miroir vert→rouge, PV + énergie des pilotes, joueur haut à 180°.
- D-140-03/08/09 : récupération énergétique du minerai fortement atténuée, cadence et dispersion initiale liées à `nrj`, impacts astéroïdes physiques séparés des i-frames projectile avec chaleur de surface historique.
- D-140-06 : contact boss continu épuise rapidement l’énergie puis les PV.
- D-140-05 : tirs tourelles distingués des tirs pilotes pour attribution ; pas de buff arbitraire de PV boss.
- D-140-07 : tirs ordinaires restent rectilignes ; prédiction initiale conservée ; guidage réservé au missile/types explicitement guidés.
Le joueur haut est confirmé RAS au téléphone. Fab juge aussi le jeu trop facile au niveau facile : la difficulté est donc relevée via les mécanismes identifiés, pas par une hausse aveugle des PV boss.


## Vérification fraîche Sol — workflow 36266178070
SHA de code testé : `fbb1ff92907821217ff94d847d9f2bbf4683636d`.
- Workflow : https://github.com/greenpower2669/SpaceFortressVs/actions/runs/36266178070
- 57 lignes PASS dans les suites de régression.
- Nouveau rendu classique : W/H valides, astéroïde admis par `inxy()` et pixels réels de texture rendus : PASS.
- Cadence/précision selon énergie, contact boss continu, impacts astéroïdes hors i-frame projectile et minerai ne réinitialisant plus le bouclier : PASS.
- HUD haut miroir/180°, vie boss dupliquée avec gradient et texture missile historique + recréation renderer : PASS.
- Tir ordinaire : dispersion initiale bornée, puis aucune correction de trajectoire en vol : PASS.
- Suites historiques, 200 ouvertures, 200 rendus, migration/sauvegardes, champ duel/coop 30/60/120 Hz : PASS.
- Build Android : `BUILD SUCCESSFUL in 2m 28s`.
- Packaging 1.4.0/versionCode 10, bibliothèque et assets : PASS.
- Artefact release : https://github.com/greenpower2669/SpaceFortressVs/actions/runs/36266178070/artifacts/10914467015
- Digest ZIP artefact : `sha256:6cdc725a398e47cd3854ab3f2b4bb4bf6751206ebec4dbb5660fbad5f8f49aec`.
- Certificat APK debug de ce build : `19e25032f58c41ea692554dd7b2849dad4b9589e78fac6a1f1988a8e384295f5`.
Ce certificat diffère de la v1.3.1 publiée (`8abfc11c8bc4f9ac065eb5c086ad4e457290bcbbc1105017865368de7e565868`) : APK de test, pas une mise à jour directe compatible. Ne pas désinstaller ni effacer les données.


## Décision Fab — dégâts entrants coop ×5 — 2026-10-02
Fab fixe le coefficient à ×5 pour les pertes de PV des deux pilotes coop causées par les attaques du boss, le contact boss et les astéroïdes. Le multiplicateur s'applique après la protection actuelle du bouclier : un impact totalement absorbé reste absorbé. Énergie, délais d'invulnérabilité, dégâts sortants, PV boss et `src/main.cpp` restent inchangés. Tests ajoutés pour les deux pilotes, coop locale/IA, nrj 0/25/50, projectiles/spéciaux boss, contact 30/60/120 Hz et callback astéroïdes. Validation CI fraîche et ressenti téléphone Fab requis.


### CI du réglage ×5 — diagnostic test
Le workflow 36939207044 a atteint les régressions puis a échoué uniquement sur l'assertion du nouveau test astéroïde. Cause : le test utilisait `sfCoopResources()`, qui exécute correctement le callback d'impact puis `sfCollectDust()`; la poussière créée à l'impact peut immédiatement rendre un peu de PV et d'énergie, contaminant une mesure censée isoler le seul dégât. Le test est corrigé pour exercer `sfLegacyFieldStep(sfCoopAsteroidHurt)` directement au point réel du callback. Aucun changement supplémentaire du code de gameplay.


### CI D-140-10 — précision historique `setv()`
La seconde CI 36939614317 a confirmé que l'isolement du callback était nécessaire mais a encore échoué car le test supposait une hauteur d'astéroïde fixe à 20. Le `setv()` historique conserve `w=20` mais randomise `h` à la création. Le test calcule désormais le dommage historique depuis `impact->w*impact->h` réellement créé. Aucun changement de gameplay.


## Preuve fraîche D-140-10 — workflow 36940059444
SHA de code testé : `7e624256d8f50d07a63e92c9c3851ff2b6048ace`.
- 58 lignes PASS ; le test dédié confirme « coop incoming damage is exactly x5 after shield for both pilots/modes, boss specials, contact and asteroid callback ».
- Régressions historiques duel/coop, champ réel, 30/60/120 Hz : PASS.
- Build Android : `BUILD SUCCESSFUL in 2m 45s`.
- Packaging 1.4.0/versionCode 10, APK signature, intégrité ZIP, bibliothèque et assets : vérifiés.
- Artefact release-files : https://github.com/greenpower2669/SpaceFortressVs/actions/runs/36940059444/artifacts/11199572849
- Digest artefact : `sha256:181030495d4ba54a875734c762a22f6ff80eb22d44551fe2e20c53b8b0265964`.
- Certificat APK debug : `a76bcc6b3183e3dbfd3f1ee3a296591dff990ba350e3eb990e44cc7e2b686b38`, différent de la v1.3.1 publiée : APK de test uniquement.
Validation du ressenti téléphone reste à Fab. Aucun merge main ni release.


## D-140-11 — dégâts ×15 et économie de poussières
Fab demande ×3 supplémentaire sur le ×5 validé : multiplicateur coop total ×15 après protection du bouclier. Les poussières blanches historiques peuvent soigner un boss blessé lorsqu'il les touche ; les munitions boss spéciales kind 1/2/3 peuvent aussi les ramasser et transmettre le soin. Le collecteur éligible le plus proche gagne et un boss à pleine vie ne consomme pas la poussière. Les impacts boss sur un vaisseau génèrent des poussières rouges ; elles ne rendent ni PV ni énergie et, après armement, ajoutent seulement un peu de chaleur `nrj`, donc usent le bouclier. Les collisions d'astéroïdes conservent leur émission rouge historique. `src/main.cpp` reste intact.


### Preuve fraîche D-140-11 APK
Gameplay testé : `23d0e3e1354dec2cadf7b40c7a4f10a8b751a585`. Workflow `37095128530` : régressions complètes GREEN puis build Android APK+AAB réussi. Artefact : `SpaceFortressVs-x15-dust-APK-AAB-37095128530`. Le premier essai avait échoué uniquement sur la validation d'un wrapper Gradle tiers SDL téléchargé trop tôt ; l'ordre CI a été corrigé sans changement gameplay. Aucun merge `main`, aucune release ; validation téléphone reste à Fab.


## D-140-12 — bouclier linéaire partagé duel + coop — 2026-10-03
Un seul cœur de bouclier sert désormais les deux modes : nrj=0 = 100% protection, nrj=50 = 0%. Dégâts PV linéaires selon spent=nrj/50 ; usure = puissance brute × clamp(0.1+spent,0.1,1.0), état pré-impact. Coop conserve ×15 exactement une fois. Contact boss intégré à 240 Hz interne. Poussière blanche pleine : -0.25 nrj, soit 0.5% de réserve. Le duel Android passe par la copie générée ; src/main.cpp reste byte-identique.


## D-140-13 — Scenic mix / progression 200 (2026-10-03)
- 20 base scenic identities generate 100 mixed candidates (nebula + planet + filter).
- Classic selects from a shuffled 100-candidate bag without replacement; refill restores 100% eligibility and prevents an immediate boundary duplicate.
- Campaign scenery now consumes encounter 0..199, with continuous blue→green→red RGB progression.
- Boss body receives only a subtle RGB correction; historical 0/4/8/20 tentacle counts remain.
- Boss travel expands continuously from centre-biased to near safe arena edges by encounter 200.
- src/main.cpp remains historical and untouched; classic hook is generated through prepare-legacy-source.cmake.

## 2026-10-03 — D-140-14 kinetic shield
Shared two-layer kinetic physics now uses relative vector speed, linear mass and speed squared before the existing energy shield. The kinetic field spends the common nrj reserve efficiently; hull regeneration rewards near-full reserve. Boss kinetics are charge-only.

## 2026-10-03 — D-140-15 kinetic rebalance
Fab supersedes D-140-14 kinetic tuning in BOTH classic and coop/campaign: kinetic damage has no coop x15, slow impacts are genuinely weak, interception distance grows continuously with relative speed, waves expand from the ship then disappear, energy cost is previous shared cost ×0.00001, red dust is perturbed but white resource dust is never physically deflected. Non-kinetic coop attacks keep their validated ×15.

## Kinetic surge / two-finger field — 2026-10-03
- Canon shared by classic + coop: maximum kinetic field diameter = 2.0 ship diameters.
- Short second-finger tap keeps firing; long hold (0.35 s) enters a transparent visible surge.
- Surge doubles kinetic dissipation for at most 2.0 s. Release after activation purges every asteroid inside the max kinetic zone into historical white resource dust.
- White dust is never physically deflected by kinetic waves; red dust remains reactive.
- Normal absorption waves are more transparent; surge aura shows inner + outer circles.
- Classic hull dimensions remain historical; only the kinetic field diameter expands.
- No main merge/release before Fab phone validation.

- 2026-10-03 KINETIC AUDIO/DANGER: 2e doigt charge exactement 2 s avec champ cinétique x2 irisé; à 2.00 s signal CHARGE et protection cinétique coupée jusqu’au relâchement; relâchement chargé = déflagration + purge. Danger boss non cinétique réglable accueil x1/x5/x10/x15/x20, défaut x10.
