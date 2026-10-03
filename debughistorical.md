# debughistorical.md — SpaceFortressVs

## Règle
Ce fichier sépare les bugs/régressions, leurs causes ou hypothèses, les corrections présentes, et les preuves réellement exécutées. Les résultats rapportés par une autre session ne deviennent jamais des preuves Sol sans relance.

## Historique v1.3.1
Base de reprise : `dcf80afb6eb117d405f46f03d8567d31a76b4b12`.

Correctifs déjà historiques : respiration bornée, restauration coop→duel, tirs humains rectilignes, sauvegardes protégées, visibilité/minage du champ, accueil animé.

Limite connue de signature :
- v1.3.0 publiée : certificat `822915992d833e32f555fd4332ff56bba0c5319bc0a359e1925811fe12a86c2b`.
- v1.3.1 publiée : certificat `8abfc11c8bc4f9ac065eb5c086ad4e457290bcbbc1105017865368de7e565868`.
Ces certificats diffèrent ; une installation existante ne doit jamais être supprimée pour contourner ce problème.

## Régressions restaurées pour 1.4.0
Les documents et tests récupérés identifient les régressions suivantes dans l'état antérieur :
- coop : champ simplifié au lieu du champ historique complet ;
- toucher : tir humain lié au cooldown au lieu d'un tap distinct ;
- missile coop absent ;
- bouclier découplé de la réserve historique ;
- tourelles coop déployées sans bonus ;
- campagne limitée à 50 ;
- absence de signature visuelle progressive des quatre difficultés.

Le code de la sauvegarde `9b8cae378f66d461dd8a29f9504972f648b2c40c` contient les restaurations correspondantes :
- champ partagé duel/coop ;
- quatre doigts et tir au tap ;
- missile à pleine réserve ;
- bouclier dépendant de `nrj` ;
- bonus temporaire de tourelles avec réserve propre ;
- 200 affrontements ;
- sauvegarde v2 et migration v1 ;
- rendu partagé difficulté 0/4/8/20.

## Récupération Astra → Sol — 26 septembre 2026
Astra a retrouvé le HEAD local antérieur `9ac9763032ba296e843a1f626ea318affd73b551`, puis a sauvegardé l'état de travail et transféré un arbre identique vers GitHub.

Référence distante à utiliser :
- commit : `9b8cae378f66d461dd8a29f9504972f648b2c40c`
- arbre : `92cf3b03b2aeaf30da6d57b85f126f89cf37caf4`
- branche : `fix/gameplay-campaign-200`.

Sol a vérifié directement via GitHub que le commit et l'arbre correspondent à ces valeurs.

Aucun merge ni release n'a été fait pendant la récupération.

## Incident mémoire FAB Copilot
Au moment de la sauvegarde Astra :
- `todo.md` existait ;
- `brain.md`, `brainmap.md` et `debughistorical.md` étaient absents.

Cause documentaire : ces trois mémoires n'avaient pas été créées/synchronisées dans le workspace récupéré. Elles sont reconstruites maintenant à partir des preuves Git, du code, du plan, de la release et des tests présents. Ne pas réécrire l'histoire en disant qu'elles existaient auparavant.

## État des preuves avant la relance Sol

### Rapport Astra uniquement
- 32 groupes SDL/UBSan.
- 21 groupes du champ historique.
- migration v1/v2.
- neuf tests Python.
- 200 rendus.
- cinq combats simulés et 200 fins d'affrontement automatisées.

Ces résultats ne sont pas encore comptés comme vérification fraîche Sol.

### Vérifications Sol effectuées
- branche/commit/arbre Git distants confirmés ;
- présence des implémentations principales et des tests confirmée par lecture du HEAD récupéré ;
- version 1.4.0/code 10 confirmée ;
- workflow de test/build et contrôle de signature inspectés.

### Vérification fraîche à venir
Le premier commit FAB Copilot de reprise doit déclencher le workflow Android sur `fix/gameplay-campaign-200`. Le résultat de ce workflow remplacera les affirmations de passation par des preuves fraîches pour les tests, le build, les paquets et la signature.

## Règle de correction
Si un test frais échoue : reproduire, déterminer la cause, corriger uniquement cette cause avec test de régression, puis mettre à jour les quatre mémoires dans le même commit. Aucun ajout de fonctionnalité ni refonte opportuniste.


## Vérification fraîche Sol terminée — 26 septembre 2026
Workflow : https://github.com/greenpower2669/SpaceFortressVs/actions/runs/36246639403
SHA testé : `2ca6aa46e732e9d9e86e9889ad2d215117c1f16e`.

Résultats frais :
- 9 tests Python réussis ;
- 32 groupes natifs SDL/UBSan réussis ;
- 21 groupes du champ historique réussis ;
- sélecteur portrait/paysage et lancement combat 200 réussis ;
- 200 rendus et signature visuelle 0/4/8/20 réussis ;
- 200 ouvertures de combat automatisées réussies ;
- 200 coups finaux / victoires durables / Hall après reload réussis ;
- build APK/AAB réussi ;
- packaging, assets, bibliothèque ARM64 et intégrité ZIP réussis.

Aucun blocage logiciel reproduit pendant cette relance : aucune correction de code n'a donc été ajoutée.

Signature Android réellement observée :
- 1.4.0 : `9817ba2bdf29226c72529ec161f8124f11cbee89b2748ca440b1e38148e01781`;
- 1.3.1 publiée : `8abfc11c8bc4f9ac065eb5c086ad4e457290bcbbc1105017865368de7e565868`.
Cause de l'incompatibilité de mise à jour : certificats différents. Ne pas contourner par désinstallation ni suppression des données.

Artefact vérifié : https://github.com/greenpower2669/SpaceFortressVs/actions/runs/36246639403/artifacts/10907503442
APK : `2045bc64083b624e14ecec034df350b03eaa0eeac269d145d171506d5b4f3b81`, 87372144 octets.
AAB : `a30870bece5759ddd4feb2ee4d1cf84a0e06822798afbea1e7624438d727939c`, 84880594 octets.

Les validations physiques sur téléphone restent hors de portée des tests CI et doivent être faites par Fab.


## Retour physique APK 1.4.0 — anomalies observées par Fab — 26 septembre 2026

Le workflow 36246639403 était vert, mais l'essai téléphone révèle des défauts que les tests automatisés n'attrapaient pas. Cela devient un cas de référence : **présence logique/simulation ≠ rendu ou sensation correcte sur appareil**.

### D-140-01 — classique — astéroïdes non rendus
Observation : poussières/effets visibles, sprites/rectangles des astéroïdes absents.
Statut : reproduit visuellement par Fab, cause racine non auditée.
Risque : une suite qui vérifie seulement la population/physique du champ peut passer tout en laissant le champ invisible.

### D-140-02 — coop — missile avec apparence plasma
Observation : logique de lancement missile présente, représentation visuelle non distincte.
Statut : reproduit sur téléphone, cause non auditée.

### D-140-03 — coop — dégâts faibles / récupération dominante
Observation : les dégâts reçus semblent faibles face au bénéfice des poussières d'astéroïdes.
Statut : à mesurer ; déterminer si l'effet passe par PV, énergie ou bouclier avant correction.

### D-140-04 — coop — HUD PV absent
Observation : aucune barre de vie visible pour les pilotes.
Statut : reproduit sur téléphone.

### D-140-05 — coop — boss trop fragiles sous tourelles
Observation : activation des tourelles peut quasi one-shot le boss.
Statut : reproduit sur téléphone ; mesurer DPS tourelles et courbe PV/résistance par difficulté.

### D-140-06 — coop — contact boss insuffisamment destructeur
Attendu : contact direct continu avec le boss fait chuter très vite l'énergie puis les PV.
Statut : comportement attendu non atteint selon le test téléphone.

### D-140-07 — coop — tirs IA alliée tous chasseurs
Observation : les tirs de l'allié IA corrigent leur trajectoire comme des projectiles guidés.
Attendu : prédiction au moment du tir possible, mais projectile ordinaire ensuite rectiligne ; guidage réservé aux types prévus.

### D-140-08 — coop — énergie sans pénalité cadence/précision suffisante
Attendu : moins d'énergie → cadence plus basse + dispersion/précision moins bonne ; pleine énergie → performance maximale.
Statut : exigence gameplay à restaurer/auditer contre moteur historique.

### D-140-09 — coop — collisions astéroïdes trop peu dommageables
Attendu : impact astéroïde plus punitif sur les PV, idéalement selon grandeurs historiques disponibles plutôt qu'une constante arbitraire.
Statut : anomalie de sensation confirmée sur téléphone, quantification à faire.

### Points positifs du test
- combat boss 1 fonctionnel ;
- tir au tap du pilote bas fonctionnel ;
- pilote haut et quatre doigts réels encore à confirmer avec un second joueur.

Aucune correction de code n'a été faite dans cette intervention documentaire.


## Complément test physique — HUD/orientation
- Joueur du haut testé en coop : aucun défaut fonctionnel relevé sur son contrôle/tir.
- Nouveau défaut d'ergonomie : les informations du joueur du haut doivent être tournées à 180° pour être lisibles par le joueur placé de l'autre côté de l'écran.
- Exigence HUD boss ajoutée : barre de vie sans valeur numérique obligatoire, disposition inversée côté joueur rouge/haut, code couleur vert pleine vie → rouge vie vide.
- Statut : exigences de rendu à auditer ; aucune correction de code effectuée.


## Corrections engagées après validation Fab — 26 septembre 2026
- D-140-01 : cause statique acceptée ; correctif généré `::setw(static_cast<float>(DM.w))`, main.cpp non modifié.
- D-140-02 : défaut de rendu corrigé par texture missile historique dans le renderer coop.
- D-140-03 : le soin PV direct reste faible ; la recharge de bouclier via minerai est atténuée afin qu’un nuage ne réinitialise plus la réserve.
- D-140-06 : le contact boss n’utilise plus l’i-frame projectile et devient une exposition continue dépendante du temps.
- D-140-08 : cooldown commun au point de tir + petite dispersion initiale progressive ; trajectoire ordinaire ensuite rectiligne ; refus de tir sans coût.
- D-140-09 : impact astéroïde coop distinct de `sfCoopHurt`, sans disparition silencieuse sous i-frame, chaleur proportionnelle à la surface restaurée.
- D-140-04 : HUD miroir/180° et vie boss dédoublée vert→rouge.
- D-140-05 : tirs tourelles distingués pour diagnostic ; aucune hausse arbitraire des PV boss. « Quasi one-shot » est compris comme destruction beaucoup trop rapide.
- D-140-07 : aucun guidage général ajouté/retiré ; test renforcé sur trajectoire ordinaire.
Statut de preuve : code et tests préparés ; CI fraîche requise sur le nouveau SHA avant de déclarer le lot vérifié.


## Preuve après correction — workflow 36266178070
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


## D-140-10 — rééquilibrage dégâts entrants coop ×5 — 2026-10-02
Décision Fab : multiplier exactement par 5 les pertes de PV coop issues des attaques boss, du contact boss et des astéroïdes, après calcul du bouclier. La consommation/récupération d'énergie n'est pas multipliée. Le correctif est localisé dans `src/campaign_runtime.hpp` via `SF_COOP_INCOMING_DAMAGE_MULTIPLIER=5.0f`.
Couverture ajoutée : deux pilotes, coop locale/IA, nrj 0/25/50, projectile boss réel, beam/wave, contact non létal à 30/60/120 Hz avec comparaison exacte ×5, mort plus précoce sans exigence d'énergie post-mortem, callback réel du champ d'astéroïdes et plafonnement PV à zéro. Le mode classique et les dégâts sortants restent couverts par les suites existantes.
Statut au commit : code et tests préparés ; CI fraîche et essai téléphone Fab requis avant validation du ressenti.


### D-140-10 — première CI et correction de test
Workflow 36939207044, SHA `358c25a0f39598dacf60635d00cfe5d8b983bf88` : setup/SDL réussis ; échec de la suite sur le nouveau test astéroïde, avant build APK/AAB. L'assertion mesurait le résultat après `sfCoopResources()`, qui enchaîne collision/callback puis mise à jour des particules et `sfCollectDust()`. La poussière d'impact peut rendre jusqu'à un peu de PV et réduire la chaleur, donc la valeur n'est plus le delta brut du callback.
Correction : conserver le même scénario de collision du champ mais appeler `sfLegacyFieldStep(sfCoopAsteroidHurt)` pour vérifier exactement le coefficient ×5 avant la passe de récupération. Aucun changement au multiplicateur ni au gameplay.


### D-140-10 — seconde CI
Workflow 36939614317, SHA `1fcab304b34552b7347723ee5f4cbe69b4cf3f3d` : le même test callback échoue encore après retrait de la récupération poussière. Cause exacte supplémentaire : `sprite::setv` fixe la largeur demandée mais initialise historiquement la hauteur avec une variation aléatoire ; la surface réelle n'est donc pas 20×20. Correction du test : calculer `legacyDamage` depuis `impact->w*impact->h*.05f`. Le code de gameplay ×5 reste inchangé.


### D-140-10 — validation CI fraîche
Workflow 36940059444 sur `7e624256d8f50d07a63e92c9c3851ff2b6048ace` : succès complet.
- 58 PASS, dont le test dédié ×5 après bouclier sur deux pilotes, coop locale/IA, nrj 0/25/50, attaques boss normales/spéciales, contact et callback astéroïdes.
- Suites historiques duel/coop et champ réel 30/60/120 Hz : PASS.
- Build Android : `BUILD SUCCESSFUL in 2m 45s`.
- Vérification 1.4.0/versionCode 10, signature APK, intégrité ZIP, bibliothèque et assets : PASS.
- Artefact : https://github.com/greenpower2669/SpaceFortressVs/actions/runs/36940059444/artifacts/11199572849
- Digest : `sha256:181030495d4ba54a875734c762a22f6ff80eb22d44551fe2e20c53b8b0265964`.
- Certificat debug : `a76bcc6b3183e3dbfd3f1ee3a296591dff990ba350e3eb990e44cc7e2b686b38`, différent de la v1.3.1 publiée. APK de test uniquement ; ne pas désinstaller ni effacer les données de la version existante.
Le ressenti réel du nouvel équilibrage reste à valider sur téléphone par Fab.


## D-140-11 — dégâts ×15 et économie de poussières
Fab demande ×3 supplémentaire sur le ×5 validé : multiplicateur coop total ×15 après protection du bouclier. Les poussières blanches historiques peuvent soigner un boss blessé lorsqu'il les touche ; les munitions boss spéciales kind 1/2/3 peuvent aussi les ramasser et transmettre le soin. Le collecteur éligible le plus proche gagne et un boss à pleine vie ne consomme pas la poussière. Les impacts boss sur un vaisseau génèrent des poussières rouges ; elles ne rendent ni PV ni énergie et, après armement, ajoutent seulement un peu de chaleur `nrj`, donc usent le bouclier. Les collisions d'astéroïdes conservent leur émission rouge historique. `src/main.cpp` reste intact.


### Preuve fraîche D-140-11 APK
Gameplay testé : `23d0e3e1354dec2cadf7b40c7a4f10a8b751a585`. Workflow `37095128530` : régressions complètes GREEN puis build Android APK+AAB réussi. Artefact : `SpaceFortressVs-x15-dust-APK-AAB-37095128530`. Le premier essai avait échoué uniquement sur la validation d'un wrapper Gradle tiers SDL téléchargé trop tôt ; l'ordre CI a été corrigé sans changement gameplay. Aucun merge `main`, aucune release ; validation téléphone reste à Fab.


## D-140-12 — ancien carré remplacé
Le ressenti incohérent venait de spent² en coop, d’une usure fixe +2, et des formules nrj² du duel historique. Le cœur est maintenant linéaire dans ship_energy.hpp. La copie Android classique transforme ses impacts tir/missile/astéroïde vers sfApplyShieldImpact sans modifier src/main.cpp. Les poussières blanches récupèrent 0.25 nrj chacune. RED confirmé avant implémentation ; GREEN/CI Android à vérifier.


## D-140-13 — Repetition visuelle campagne/classique
Cause: campaign scenery was indexed with boss 0..49, so difficulty blocks 51..200 repeated visual families; classic had no campaign-atlas scene rotation. Fix: full encounter scenic model plus 100-candidate no-replacement classic bag. Historical `src/main.cpp` is not edited.

## D-140-14
Audit proved asteroid vx/vy affected movement but not historical damage (area-only). Repair routes relative velocity through a shared kinetic resolver while preserving src/main.cpp and the ordinary boss-contact path outside explicit charges.
