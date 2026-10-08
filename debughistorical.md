# debughistorical.md — SpaceFortressVs

## Pièges permanents
- `src/main.cpp` historique ne doit jamais être modifié ; blob canonique `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- Hall : save campagne et état réseau séparés ; `submissionId` immuable ; page vide + `hasMore=true` avance durablement le cursor ; aucune vraie clé dans Git/logs/tests/mémoires.
- Livraison Actions : plusieurs artifacts peuvent porter le même nom ; toujours vérifier tentative/ID/SHA-256.
- Surcharge : champ normal max 2,0 diamètres ; blast armé 3,0 ; <0,30 s aucune irisation/x2 ; à 2 s champ OFF jusqu’au relâchement.
- EMP : asset de déflagration livré depuis v1.4.1 ; ne pas le ré-authored sans nouvel ordre.

## Clôture v1.4.2 — 2026-10-06
- PR #6 mergée dans `main` au commit `414b23cd2e787325b723fe7b0b6bd84fac02494b`.
- Release `v1.4.2` publiée sur `5c3bedbd592f7bf4c50b830d26d6a0da49048813`.
- Workflow publication `37530458354` GREEN ; job `publish-release` SUCCESS.
- APK SHA-256 `b5604ba8f103a351e62beb0751893d6ffdcc0d9549d6c75cb2545ea061ea8282`.
- AAB SHA-256 `4a996b60cc5bca9d874a7d2897c3ec83d095e3f5d9a989a192df3979647d74ea`.
- Release contient APK, AAB non signé, build.json et SHA256SUMS.
- Signature APK différente de v1.3.1 : ne pas désinstaller/effacer les données pour forcer un test de mise à jour.

## Pièges cinétiques v1.4.2
- Courbe canonique : `pow(energyFraction,1.20)`. Ne jamais transformer `nrj` lui-même.
- La surcharge ×2 est appliquée après la fatigue. Ne pas faire `pow(energy×2,1.2)`.
- Le state `charged` à 2 s reste prioritaire : puissance cinétique 0 jusqu’au release.
- Le ralentissement concerne uniquement `SfKineticWave.duration`. Ne jamais retarder collision, fragmentation, dégâts, poussière ou purge.
- Une vague capture son état énergétique à sa création ; elle ne doit pas changer de vitesse si `nrj` change ensuite.
- Ne pas déduire l’énergie visuelle depuis la force de couche/surcharge ; passer explicitement `sfKineticEnergyFraction(ship->nrj)`.
- Warning graphique sous 10 % : rouge clignotant ~4,5 Hz, graphique uniquement.
- CLASSIQUE DUEL local : intercepter uniquement le second doigt dans la copie Android générée ; premier doigt reste au moteur historique.
- En quittant le duel local, annuler uniquement les owners effectivement possédés par le bridge : `sfKineticSurgeCancel(owner)` + `sfKineticAudioCancel(owner)`.
- DUEL IA garde son chemin owner 1 ; ne pas le doubler.


## Audit téléphone post-v1.4.2 — correction de l'observation
- Fab a raison : la recharge passive CLASSIQUE existe dans `src/mainv1.hpp`, méthode `sprite::update()`, ligne historique `nrj*=0.997`.
- Elle s'exécute via `threadaux1` seulement lorsque `tics3` est libéré (cycles 4 et 8), donc sa cadence effective n'est PAS équivalente à 60 Hz.
- La COOP utilise au contraire `nrj*=pow(.997,60*dt)`, ce qui explique la recharge beaucoup plus rapide observée.
- Ne jamais conclure qu'un comportement historique est absent en ne regardant que les runtimes modernes : auditer aussi `mainv1.hpp` / objets historiques.


## TDD RED run 340
- Workflow `37539108268` a échoué à la compilation comme prévu : `sfMainShotSpreadEnvelope`, `sfMainShotSpreadRadians`, `sfCoopPassiveRechargeHeat`, `SF_COOP_BOSS_KINETIC_DISSIPATION` et `bossKineticFlash` absents.
- Cette panne est la preuve RED du lot ; l'implémentation suivante doit uniquement satisfaire ces comportements et préserver les régressions existantes.


## CI 341 — assertion historique devenue trop stricte
- Workflow `37539739951` atteint les régressions gameplay puis échoue uniquement sur `testCoopHumanAim(): abs(vx)<30`.
- Cette limite de 30 contredit le nouveau comportement demandé par Fab : dispersion COOP plus visible et aléatoire.
- Le test est élargi à un bornage de sécurité `abs(vx)<120` tout en exigeant le sens avant correct et l'absence de guidage en vol. Aucun changement gameplay dans ce correctif de test.


## CI 342 GREEN — 2026-10-06
- Après adaptation du seul ancien seuil de test COOP, workflow `37540141916` entièrement GREEN.
- Les simulations campagne montrent des événements `BOSS_KINETIC_FIELD` réels : raw, 55 % dissipé, résiduel retiré des PV, puis poussière blanche via la filière cinétique existante.
- Build final de test : artifact `11447494930`; APK SHA-256 `0eaf0b298858b4f934264daad1ae7dcd2d8b209b7a75b9a76c5f59656284eee2`.
- APK toujours debug-signé et incompatible signature v1.3.1 : ne pas désinstaller ni effacer les données pour forcer l'installation.


## Avenant après CI 342 — ne pas livrer l'ancien comportement
- L'ancien code GREEN 342 détruisait l'astéroïde dans `sfCoopBossKineticAsteroidImpact`; Fab l'interdit maintenant.
- Le cercle boss ancien était opaque et dessiné directement au rayon final ; Fab demande centre -> extérieur et forte transparence.
- La demi-vie 21 s de recharge COOP est remplacée par le dernier canon : cadence v1.4.2 ×4 facile -> ×2 apocalypse (.997^(240*dt) -> .997^(120*dt)).
- La charge 2 s ajoute un minage/aspiration blanc uniquement pendant la phase visible 0,30–2,00 s ; ne pas continuer en état charged/vulnérable.
- Le spawn campagne doit être temporel/continu avec plafond pour éviter une explosion de population.


## RED avenant run 343 — 2026-10-07
- Workflow `37559681669` échoue volontairement avant code : surcharge de `sfCoopPassiveRechargeHeat` à 3 arguments absente, alpha/rayon boss absents, timer spawn absent, `sfKineticSurgeMineStep` absent.
- Preuve RED valide : les erreurs correspondent exclusivement au nouvel avenant Fab.
- Le nouveau champ boss ne doit JAMAIS appeler `sfKineticDestroyAsteroid`; appliquer le résiduel au boss puis réduire/réfléchir la vitesse du rocher survivant avec cooldown anti multi-hit.
- L'anneau boss ne doit plus utiliser `sfUiCircle` opaque pour le pulse : dessin alpha dédié.
- Le minage de charge doit s'arrêter dès `charged=true` pour préserver la vulnérabilité READY.


## CI 345 — échec de compilation de couture
- Run 37560224642 : pas un défaut gameplay. Les tests appelaient les noms canoniques RING_DURATION/RingRadius/SurgeMineAsteroids, l'implémentation utilisait encore WAVE_DURATION/WaveRadius/SurgeMineStep, et le runner conservait un appel à un test séparé supprimé.
- Fix : noms alignés + runner nettoyé. Ne pas relâcher les assertions comportementales.


## CI 346 — assertion Orion devenue contradictoire
- Run 37560831848 : BOSS_KINETIC_FIELD non destructif exécuté avec succès, puis seul échec sur testVelocityGhosts nearest<16.
- Avec le spread initial aléatoire restauré, exiger <16 revient à interdire la dispersion. Le test garde la preuve d'anticipation directionnelle et borne l'écart à <90 ; aucun guidage en vol n'est ajouté.


## CI 347 — GREEN après correction canon Fab
- Workflow `37561150366` / run 347 : SUCCESS complet sur `602a725eb785ffdac393ebe1e55af3d2cf50376c`.
- Ne plus écrire que la recharge passive serait absente : elle existe historiquement via `nrj*=0.997` et Fab l'a reconstatée en jeu.
- Champ boss : conserver la valeur fixe 55 %, explicitement moins puissante que le champ joueur ; ne jamais la scaler au Danger ni la rendre destructrice pour les astéroïdes.
- Artifact téléphone `SpaceFortressVs-1.4.2-release-files` id `11456527917`, digest ZIP `sha256:1e9ef7a5276c74a9db3064a83f00c17c614ff8cddf060245336ab2f3479d9da5`.


## Diagnostic broutage — 2026-10-07
- Les boss campagne passent par un atlas + maillage UV ; plusieurs autres sprites utilisent des textures entières. La piste coordonnées/découpe est donc plausible.
- Le code actuel inset les UV boss de 0,5 % de la cellule (`.005 + u*.99`), ce qui peut réellement rogner les bords selon la taille de cellule.
- Correctif borné à tester : demi-texel réel à l'intérieur de chaque cellule. Ne pas modifier les PNG ni appliquer de crop supplémentaire avant retour téléphone.


## Correctif anti-bleeding borné
- Ancien UV boss : `.005 + u*.99` = retrait proportionnel de 0,5 % par bord.
- Nouveau UV : demi-texel réel à l'intérieur de la cellule, indépendant de la taille du sprite. C'est le seul correctif asset/découpe appliqué avant test téléphone.
- Si Fab voit encore un asset « brouté », ne pas multiplier les clamps/crops : relever quel asset précis et quel mode avant autre modification.
- Le missile COOP n'émet volontairement pas `particulesr` pour son échappement : ces particules ont désormais un sens gameplay de poussière rouge.


## TDD 349 → 350
- RED 349 `37608129628` : compilation arrêtée exactement sur les nouveaux helpers/champs absents, donc contrat correctement testé avant code.
- GREEN 350 `37608419345` : suite runtime entière, Hall, build APK/AAB et packaging SUCCESS sur `1aeb6758dd5ee8bbcf52e00f424361d8262f0c6d`.
- Le diagnostic « UV demi-texel » reste à confirmer sur téléphone : un GREEN logiciel ne prouve pas à lui seul que le rare défaut visuel observé est totalement éliminé.


## ERRATUM HUD BOSS — 2026-10-07
- Retour téléphone Fab sur CI 350 : les trois jauges ENERGIE / PV / CINETIQUE ont été mises par erreur sur les pilotes.
- Correction canonique : les PILOTES reviennent au bloc historique PV + ENERGIE uniquement.
- Les trois jauges appartiennent au BOSS : ENERGIE au-dessus, VIE au centre, CINETIQUE au-dessous, avec la représentation miroir existante pour les deux côtés.
- Ajouter une réserve énergétique boss séparée de la réserve cinétique. Dans ce lot, les deux sont des réserves de stress/affichage uniquement : elles ne modifient ni la santé boss, ni les dégâts reçus, ni la dissipation cinétique fixe 55 %.
- Les deux réserves s'auto-régénèrent plus vite aux difficultés basses et plus lentement aux difficultés hautes.
- Le champ cinétique visuel reste piloté par la réserve cinétique : plus elle baisse, plus le champ est visible.
- Tous les autres points de CI 350 sont conservés : missile/orbes/explosion classiques, anti-bleeding demi-texel, spawn/minage/recharge/Danger.
- Nouvelle branche : `fix/boss-hud-reserves-v143`. Aucun merge main ni Release sans validation Fab.


## Implémentation correction HUD boss
- Pilotes remis exactement au bloc PV + ENERGIE du lot précédent : positions .925/.963, couleur énergie équipe, aucun affichage CINETIQUE pilote.
- Boss : trois barres miroir ENERGIE (.831) / VIE (.858) / CINETIQUE (.885).
- `bossEnergyReserve` séparée : les tirs joueurs/tourelles qui touchent le boss la stressent visuellement ; aucun changement de dégâts/PV.
- `bossKineticReserve` reste stressée uniquement par les impacts cinétiques d'astéroïdes et pilote l'alpha du champ.
- Les deux réserves remontent avec la même courbe difficulté .18/s facile -> .07/s difficile.
- Dissipation boss toujours 55 % fixe ; aucun gameplay du lot CI350 n'est modifié.


## CI 353 GREEN — CORRECTION HUD BOSS
- Code validé : `2e0fb31e662618958ff9caa35b433f2368d61734`.
- TDD RED : run 352 / `37618451133` sur `9a6cf28b...`, échec attendu sur les nouveaux helpers boss.
- GREEN : run 353 / `37618641758`, régressions + Hall + APK/AAB + packaging SUCCESS.
- Artifact : `SpaceFortressVs-1.4.2-release-files` id `11481795087`, digest `sha256:e37204e5bc2bf51b1ef3c785f927d227e8cbcf2fe949c45065269bed08b44d24`.
- Aucun merge main ni Release.


## BOSS-HUD-COMPACT-DANGER-REGEN — 2026-10-07
- Retour Fab : les 3 barres boss sont correctes mais trop grandes, trop espacées et trop opaques.
- Nouveau canon : UN SEUL bloc latéral droit, largeur <=30 % écran, barres fines et serrées, fond + remplissage semi-transparents.
- Régénération boss : ne plus utiliser les 4 difficultés campagne. Utiliser exclusivement les 9 niveaux de `sfBossDangerIndex`.
- `MOU DU GENOU` : boss handicapé, vitesse de régénération = vitesse maximale actuelle / 9.
- Progression strictement croissante sur les 9 Dangers.
- `APOCALYPSE` : boss avantagé, vitesse = vitesse maximale actuelle (0.18 réserve/s).
- Même courbe pour réserve énergie et réserve cinétique boss. Le champ reste 55 % fixe : la réserve ne modifie pas la dissipation gameplay.
- Pilotes inchangés, FX/orbes/missile/anti-bleeding inchangés, `src/main.cpp` protégé.
- Branche `fix/boss-hud-compact-danger-regen-v143`; TDD RED avant code ; aucun merge/release sans validation Fab.


## Implémentation BOSS-HUD-COMPACT-DANGER-REGEN
- Boss HUD : un seul bloc à droite, largeur 28 % écran, y≈13,5 %, trois barres fines (~width/110) séparées d'environ 2 px.
- Fond boss alpha 88/255 ; remplissage alpha 188/255. HUD pilotes inchangé.
- Régénération réserves boss pilotée exclusivement par `sfBossDangerIndex` 0..8.
- Vitesse max conservée à 0.18 réserve/s en APOCALYPSE ; MOU DU GENOU = 0.18/9 = 0.02/s ; interpolation linéaire strictement croissante pour les 7 Dangers intermédiaires.
- Énergie et cinétique boss partagent cette courbe. Santé, dégâts, dissipation cinétique 55 %, champ et autres gameplay restent inchangés.
- RED prouvé run 354 / `37634894304` : échec attendu sur helpers alpha absents avant code.


## CI354 RED -> CI355 GREEN
- RED 354 : échec exactement sur `sfBossHudBackgroundAlpha/sfBossHudFillAlpha` absents, preuve TDD valide.
- GREEN 355 : suite complète SUCCESS sur `6826463de42ca7ad794f5ec775fb46d98009c836`.
- Le réglage regen boss utilise maintenant `sfBossDangerIndex`, pas `sfDifficultyIndex(encounter)`.
- Ne pas confondre cette regen de réserves boss avec la recharge passive des joueurs.


## BOSS-HUD-MIRROR-LABEL — 2026-10-08
- Fab valide le HUD boss compact CI355 et demande un dernier polish purement visuel : ajouter le texte `BOSS` au-dessus et un effet miroir/reflet sur les trois barres.
- Interprétation bornée pour conserver le HUD ramassé : PAS de second bloc dupliqué ; l'effet miroir est un reflet/gloss interne symétrique dans chaque barre, sans augmenter l'emprise du HUD.
- Le bloc reste à droite, compact et semi-transparent. Aucun changement de géométrie globale, gameplay, régénération Danger, champ 55 %, joueurs, FX, atlas ou assets.
- TDD RED : nouveaux helpers `sfBossHudLabelRect`, `sfBossHudMirrorAlpha`, `sfBossHudMirrorBand` exigés avant implémentation.
- Branche `fix/boss-hud-mirror-label-v143` depuis `fb8b98d92c10acfb43ec08d8ca61821b0466fda2`. `src/main.cpp` reste protégé. Aucun merge main ni Release sans validation Fab.


## Implémentation BOSS-HUD-MIRROR-LABEL
- Ajout de `BOSS` directement au-dessus du bloc latéral compact, sans déplacer ni agrandir les trois barres.
- Effet miroir interprété comme reflet/gloss interne : deux bandes symétriques dans la partie remplie de chaque barre (haut alpha 76, bas alpha ~25), sans second bloc ni emprise écran supplémentaire.
- Fond 88/255 et remplissage 188/255 conservés ; géométrie 28 % et regen Danger MOU 0.02/s -> APOCALYPSE 0.18/s inchangées.
- Aucun changement gameplay, FX, atlas, joueurs, champ 55 % ou `src/main.cpp`.


## CI357 RED -> CI358 GREEN
- RED 357 : helpers label/reflet volontairement absents avant code.
- GREEN 358 : suite complète SUCCESS sur `65dbfa5d2676b76ac438df462917c5bd01fa1cca`.
- Effet miroir réalisé comme gloss interne, pas comme duplication du HUD, pour respecter la demande précédente de compacité latérale.


## BOSS-HUD-DUAL-ROTATED — 2026-10-08
- Retour téléphone Fab sur CI358 : le bloc boss top-right est lisible à l'endroit, donc faux pour le joueur du haut.
- Canon demandé : définir le bloc normal en BAS-GAUCHE pour le joueur du bas ; créer le même objet en HAUT-DROITE par transformation 180° complète.
- La transformation 180° comprend : rectangles des 3 barres, sens de remplissage, reflet/gloss, position du label et glyphes `BOSS`.
- Le bloc haut n'est pas une variante bricolée : il doit être le miroir géométrique exact du bloc bas via `sfMirrorRect180`, comme le HUD pilote haut.
- Le HUD reste compact et semi-transparent. Aucune modification de regen Boss Danger, champ 55 %, gameplay, pilotes, FX ou atlas.
- Nouvelle branche `fix/boss-hud-dual-rotated-v143` depuis `4f5c05bec735b9ab2be34e0f1a2ec1da896f0436`. `src/main.cpp` protégé ; aucun merge/release sans Fab.


## Implémentation BOSS-HUD-DUAL-ROTATED
- Le bloc canonique boss est maintenant placé BAS-GAUCHE (x=margin, y≈83,5 %), au-dessus du HUD pilote bleu pour éviter le chevauchement.
- Le bloc HAUT-DROITE est construit exclusivement par `sfMirrorRect180` à partir du même objet.
- Le remplissage des 3 jauges est inversé pour le haut via `sfBossHudValueRect(..., upper=true)`, ce qui rend le remplissage haut exactement miroir du bas.
- `BOSS` du haut est dessiné avec `sfCoopText180` à partir des coordonnées logiques du label bas : glyphes réellement retournés à 180°.
- Le gloss suit lui aussi la rotation : bande brillante haute en bas-gauche, bande brillante basse dans la copie haut-droite.
- Taille, transparence, regen Boss Danger, champ 55 %, joueurs, FX, atlas et gameplay inchangés.


## CI360 RED -> CI361 GREEN
- RED 360 : tests cassent uniquement sur le nouveau contrat d'orientation avant code.
- GREEN 361 : suite complète SUCCESS sur `cd1b3f773b3e08e19fd36c025c274b12e661c844`.
- Le texte BOSS du haut utilise `sfCoopText180` avec les coordonnées logiques du bloc bas ; ce n'est pas un simple texte inversé isolé.


## SURGE-CONE-MINING — 2026-10-08
- Retour Fab : le minage/marée ne doit plus s'arrêter à READY (2 s). Tant que le second doigt reste maintenu, l'aspiration et la transformation d'astéroïde continuent.
- Portée demandée +50 % : marée surface 1.35 -> 2.025 diamètres vaisseau ; poussières 2.2 -> 3.30 diamètres. Le blast/purge final reste à 3 diamètres, inchangé.
- Correction visuelle : PAS de halo 360°. Aspiration uniquement DEVANT le vaisseau dans un cône. Joueur haut regarde vers +Y ; joueur bas vers -Y. Le même cône est donc naturellement retourné.
- Visualisation : halo/funnel orange semi-transparent animé de l'extérieur vers l'intérieur, visible pendant toute l'aspiration (y compris après READY).
- Effet matière : réutiliser la primitive historique `sfMineAsteroid` / `partsforiw` pour retrouver la restitution blanche du mode classique au lieu de l'émission manuelle pauvre.
- Son : à 2 s, conserver READY mais poursuivre un fond de charge nettement plus discret jusqu'au relâchement/cancel.
- `src/main.cpp` reste strictement protégé. Aucun merge main ni Release sans validation téléphone Fab.


## Implémentation SURGE-CONE-MINING
- `sfKineticSuctionVisible` reste vrai après READY tant que le doigt est maintenu ; le boost de champ historique `sfKineticSurgeVisible` reste, lui, borné avant READY.
- Portées gameplay : minage de surface `2.025 × diamètre`, aspiration poussières `3.30 × diamètre`. Le purge/blast final reste `3.0 × diamètre`.
- Sélection strictement dans un cône avant de 70° (35° de demi-angle), orienté +Y pour le pilote haut et -Y pour le pilote bas.
- Transformation astéroïde : suppression de l'émission manuelle 7 poussières/s ; impulsions à cadence équivalente 1.25 tir/s via le vrai `sfMineAsteroid`, donc `partsforiw` + effet `pous` + shrink historiques.
- Visuel : cône orange semi-transparent + 5 fronts convergents extérieur→intérieur, sans halo 360°, actif pendant toute l'aspiration y compris après 2 s.
- Audio : READY reste joué ; ensuite `kinetic_charge.wav` boucle au volume canal 24/128 jusqu'au release/cancel (charge initiale 76/128).
- Aucun changement `src/main.cpp`, boss HUD, Danger, boss kinetic ou blast final.


## CI363/364 -> CI365 GREEN — SURGE-CONE-MINING
- 363 : dépendance `vlong` interdite dans le helper header-only du cône ; remplacée par `sqrt` locale.
- 364 : ancien test supposait minage latéral/360° ; corrigé pour astéroïde frontal et ajouté contrôle de poursuite après READY.
- 365 : chaîne complète GREEN sur `40a3e91d8d4fff7e8ac44f94d2bda6d4010efc50`.


## CONE-MINAGE-SUPERCHARGE-SPEC — 2026-10-08 — AUTORISATION CODE FAB
- Contrat source : `SpaceFortressVs_Specification_Cone_Minage_Supercharge_2026-10-08.pdf`, ajouté par Fab sur `main` et repris inchangé sur cette branche.
- Déclenchement cône + cercle supercharge : 0,20 s depuis le geste à deux doigts.
- Supercharge indépendante : cercle rouge à 0,20 s, progression rouge -> orange -> jaune -> vert, 100 % à 2,00 s total, vert maintenu jusqu'au relâchement.
- Bouclier cinétique normal : suspendu dès 0,20 s pendant toute la supercharge et son maintien.
- Minage : tous les astéroïdes intersectant le cône fondent simultanément, en temps de simulation, sans cible unique, quota ni blocage par le plafond de poussières.
- Loi distance : interpolation linéaire proche -> loin. Le PDF laisse Vprès/Vloin à calibrer ; valeurs d'essai isolées : 0,020 H/s au nez et 0,004 H/s au bout, afin de viser environ 4 s pour un gros astéroïde proche. Ces deux valeurs ne deviennent pas canoniques avant essai Fab.
- Poussières : rendu historique `partsforiw` réutilisé, mais strictement découplé de la fonte physique.
- Boss : dégâts continus en parallèle du minage ; DPS = DPS réel du phaser × [3 - 2,97 × d/L]. Le point d'entrée est recherché sur le périmètre de collision du boss (même rayon que le phaser), pas seulement son centre. Les astéroïdes ne bloquent pas.
- Déflagration pleine charge : mécanisme existant ×3 préservé ; relâchement avant 100 % ne lance pas la purge pleine puissance.
- `src/main.cpp` protégé. Aucun merge `main` ni Release sans validation Fab.


## CI368 -> CI369 GREEN — spec cône/minage/supercharge
- CI368 a révélé uniquement un test d'intégration textuel encore figé sur `0.30f`/ancien feedback ; garde-fou remis au canon PDF.
- CI369 : toute la chaîne Android et régressions GREEN sur `b8d3764d02b59efeb2ef09b20b137da7d5da3b6b`.


## IA-BALANCE-CLASSIC-VECTOR — 2026-10-08 — AUTORISATION CODE FAB
- Fab a explicitement levé la consigne « NE PAS CODER » par « Ok code ça ».
- Cône : diviseur de difficulté exact 1..9 appliqué aux quatre axes demandés, sans réécrire le mécanisme CI369 : vitesse de fonte, DPS offensif autorisé, ouverture, longueur. Niveau 1 = efficacité maximale CI369 ; niveau 9 = /9.
- Supercharge : mécanisme 0,20 s -> 2,00 s inchangé et indépendant ; aucun diviseur appliqué à sa progression.
- IA CLASSIQUE adverse : neuf niveaux de stratégie via le même `sfBossDangerIndex`. N1 vise la position courante, réagit lentement et reste lisible ; N2-N4 activent ressources/tactiques ; N5-N7 raccourcissent réaction, améliorent risque/attaque et supercharge défensive ; N8-N9 ajoutent anticipation basée sur une moyenne de vitesse adverse réellement observée. Aucun accès à une information cachée.
- IA adverse <=10 % réserve : récupération prioritaire, recherche d'astéroïde, positionnement de minage, activation du cône quand la cible entre réellement dans le cône, tirs offensifs suspendus.
- IA COOP : survivant/sauveteur. Joueur à terre = secours prioritaire ; énergie <=10 % = seulement ressource rapide et proche de la route de secours. Les petits astéroïdes lents sont traités comme ressources exploitables et leur risque est réduit dans le choix de trajectoire.
- Règle absolue COOP IA : le cône du coéquipier ne peut jamais infliger de dégâts boss. Il est utilisé pour miner et est annulé lorsque le minage n'est plus pertinent, sans purge offensive volontaire. Tirs boss suspendus pendant secours, réserve faible, PV faibles ou minage.
- Boss : mémoire de menace par propriétaire des dégâts (tirs et cônes autorisés), décroissance temporelle, sélection/réévaluation de l'agresseur principal. Un DPS lourd raccourcit le délai de charge ; pendant la charge, le boss réajuste physiquement son vecteur vers l'agresseur selon le niveau.
- Boss missiles : estimation de trajectoire sur missiles visibles uniquement ; fenêtre d'anticipation, temps de réaction et amplitude d'esquive progressent avec la difficulté. Esquive latérale bornée en accélération/amplitude, donc jamais parfaite ni téléportée.
- CLASSIQUE tactile uniquement : les affectations historiques directes `Spritej1/2->x/y = touch` sont remplacées dans le source Android généré par une destination. `sfClassicTouchVectorUpdate` avance les deux vaisseaux vers cette destination à vitesse bornée selon le modèle COOP. Le code tactile COOP n'est pas modifié.
- `src/main.cpp` reste strictement protégé. Aucun merge main ni Release avant validation téléphone Fab.
