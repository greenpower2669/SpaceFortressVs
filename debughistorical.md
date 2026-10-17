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


## CI371→CI377 — convergence mission IA/balance
- 371/372/373/374 : corrections successives des gardes d'intégration et du contrat historique niveau 3.
- 375 : ancien test de largeur supposait un cône non danger-scalé ; test réaligné MOU DU GENOU + preuve /9 APOCALYPSE.
- 376 : assertions historiques bouclier/contact dépendaient implicitement de ROCK N ROLL ; tests rendus autonomes sans changer le gameplay.
- 377 : chaîne complète GREEN sur `13025a34f5ad8739388787f00cfae243e1e8eb37`.


## CI385 — CORRECTION TELEPHONE FAB : MINAGE IA + CONE APOCALYPSE — 2026-10-08
- Retour téléphone Fab : comportement de minage IA non observable/appliqué ; cône APOCALYPSE devenu trop petit avec la réduction /9.
- Cause VS IA confirmée : le pilote pouvait entrer en mode `Mine`, mais `sfAiUpdateConeStrategy` ne déclenchait réellement le cône de minage que dans l'urgence <=10 % énergie. De plus, le point d'approche utilisait une distance fixe qui pouvait se trouver hors de la portée réduite.
- Correction CLASSIQUE VS IA : toute stratégie `Mine`/cible minière suivie peut maintenant posséder et maintenir le cône quand la cible est réellement dedans ; point d'approche calculé depuis la portée réelle du cône ; l'astéroïde cible n'est pas traité comme obstacle à fuir pendant l'approche, tout en gardant un espacement anti-collision.
- Correction COOP IA : récupération anticipée sous 35 % hors secours ; <=10 % reste la règle d'urgence absolue. Pendant un secours, aucun détour minage sauf urgence énergétique et uniquement vers une ressource proche de la route. Les petits astéroïdes lents jusqu'à 1,5 diamètre de vaisseau sont considérés exploitables. Le cône du coéquipier reste strictement MINAGE et ne blesse jamais le boss.
- Synchronisation défensive : `sfCampaignStart` recopie le mode COOP sélectionné dans `sfActiveMode` et synchronise `setia`, pour empêcher un lancement COOP+IA de retomber silencieusement en local.
- Nouvelle courbe cône demandée par Fab : `scale = 2 / (niveau + 1)`, niveau humain 1..9. Donc MOU DU GENOU = 2/2 = 100 %, APOCALYPSE = 2/10 = 20 %. Rapport extrêmes = x5, et non x9. Les quatre axes restent liés : minage, dégâts offensifs autorisés, largeur, longueur. Supercharge inchangée.
- CI379 valide isolément la nouvelle courbe. CI380→384 ont exposé/aligné deux tests historiques (distance fixe de minage et signature `sfCoopRisk`) sans changement de règle supplémentaire.
- CI385 / run `37851018766` : chaîne complète GREEN sur `c9e97c0e88f1d2603561ea9d7964221f43852916`.
- Artifact release-files id `11582216612`, digest ZIP `sha256:5809fc424cc66d9f91fdc0bde665dd391816ec91bb8a362df51125c3c30fe68d`.
- APK SHA-256 : `283263dd8117621a99d33edfe6669d7b1ec82ee4bc790a3280f37f6cec681a52`.
- AAB SHA-256 : `9428be07957364798c8ea86162f9d086e72469fa61c39d63dbf1646ae664cf40`.
- `src/main.cpp` reste strictement intact : blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- Aucun merge main, aucune Release. Prochaine validation : téléphone Fab, surtout minage VS IA et lisibilité/efficacité du cône en APOCALYPSE.

## FAB-DUEL-PARITY-SPEED-2X — 2026-10-09 — test branch only
- Fab suspends Google Play upload-key preparation, main merge and publication. Existing v1.4.2 stays untouched.
- Double human vector motion cap in both classic duels: 0.95 to 1.90 arena widths/s; in cooperative modes: 1.55 to 3.10 arena widths/s, with proportional response doubled too. No teleportation, no AI/boss acceleration.
- Both CLASSIC duel modes now share the same second-finger / kinetic charge / mining / release logic for the blue human; retire the conflicting VS-AI remaster touch shortcut. Orange AI remains CPU-controlled.
- Added native regressions for vector speed bounds, COOP speed and duel touch parity. Phone validation remains mandatory.
- src/main.cpp historical SHA must remain unchanged. No merge, no Release, no signing key generation until new Fab order.


## 2026-10-09 — Aides sur CI389
- Base CI389 `5ed37a7647f348767f90d913f31fae76bd3b6089`; branche `fix/help-refresh-on-ci389-20261009`.
- Report limité à `src/help_runtime.hpp`, `tests/help_runtime_regressions.cpp`, `docs/proposals/2026-10-09-tutorial-mini-jeux.md`. Cônes, IA, minage, vitesse et `src/main.cpp` conservés.
- Mini-jeux proposés seulement. Validation CI/APK téléphone requise. Pas de merge main ni Release.


## 2026-10-10 — SOLO labyrinthe/visée 360, incidents d'intégration
- Capture téléphone initiale : immense carte-matrice de carrés, petit vaisseau, menu SOLO barres muettes et défilement peu réactif. Exigence Fab : vrais décors et gameplay labyrinthique MAINTENANT, conserver maniabilité historique.
- Premier compilateur C++ : `std::vector<std::array<bool,4>> links(size_t(total));` interprété comme déclaration fonction (most vexing parse), réparé via constructeur à deux arguments `7a95a1a5`.
- Deuxième compilateur : noms `targetX/targetY` shadowant paramètres de `Session::step`, corrigés `151bb81a`.
- Nouvelle architecture : `shared_touch_motion.hpp` pure, terrain V3, rendu SDL_image de vrais assets, `SoloAssets` détruit avant SDL renderer, visée directionnelle et cône canonique tourné, appui de 2s, poussière glacée.
- Tests automatisés compilés par core/UI GitHub Actions ; **aucune preuve d'exécution sur Samsung avant essai utilisateur**. Sur appareil, conserver l'APK DIAG isolée et exporter son TXT si plantage. Release/main interdits sans validation Fab.


## 2026-10-11 — Retour Samsung : boyaux étroits et pilotage instable
- Symptôme utilisateur : malgré vrai labyrinthe, couloirs visuellement étroits et manœuvres peu naturelles ; la carte suit trop constamment le vaisseau, alors que le monde devrait rester fixe jusqu'au seuil du tiers d'un bord.
- Correction ciblée : zoom visuel de la carte ×3 ; taille sprite vaisseau identique ; caméra mémorisée dans la session, défilement uniquement après franchissement 1/3 ou 2/3 en X/Y ; reprise dans les quatre sens.
- Diagnostic technique du pilotage : l'ancienne `pilotTarget()` reconvertissait la position du doigt en coordonnées mondiales à chaque image avec une caméra qui suivait le joueur ; boucle de retour susceptible de donner un effet de poursuite/instabilité. Nouveau pilotage ancré au premier contact : le déplacement du doigt donne un déplacement dans le monde indépendant des panoramiques.
- Zoom cartographique et cinématique : le mouvement cible SOLO prend désormais la largeur de la zone de monde visible, gardant approximativement la même vitesse en pixels/s qu'avant zoom, sans accélération excessive. Les collisions restent inchangées en unités carte.
- Validation CI + Android build en attente ; vérifier vraie expérience Samsung et export diagnostic si plantage.


## 2026-10-11 — Caméra SOLO 2 rectangles progressifs, build vert
- Ancienne règle caméra en `solo_viewport.hpp` : au seuil de 1/3, translation instantanée du décor rattachée au vaisseau ; sensation de vitesse insuffisante. Besoin Fab : **vitesse variable continue dans chaque bande**, accélération/décélération sans seuil brusque, croisière maximale près du bord.
- Nouvelle fonction `soloRectangularCameraGain(normalizedPosition)` : deadzone 1/3 centrale ; zone1 1/3→1/6, douceur `smoothstep` 0→55 % ; zone2 1/6→6 %, douceur `smoothstep` 55→100 % ; dernier 6 % à 100 % pour préserver dimensions du sprite et visibilité.
- `soloUpdateCamera` prend désormais `dt` réel depuis la boucle SDL. Vitesses caméra stockées par session et filtrées temporellement indépendamment X/Y; retour vers le centre et arrêt imposent une cible nulle, vitesse lissée jusqu'à zéro. Reset de l'inertie au changement écran/session; contrainte min/max monde, garde anti-clipping aux bords.
- Pilotage en coordonnées mondiales toujours indépendant du défilement; mouvement réel de la caméra **uniquement après** step physique. Tests distincts 2D/diagonale, plateau pleine vitesse, freinage et bordure d'écran.
- Action #38121160188 entièrement verte (core, SDL2, Android) pour SHA `cace6129`. APK directe `SpaceFortress-DIAG-Camera-2-Zones-Progressives.apk` créée localement, SHA256 `a9b040b9566ce17b576c18c28e7bf3f1c46e92d37a5c3987a93be6c267a7daa7` contrôlé. Épreuve réelle Samsung non effectuée par CI; conserver TXT de diagnostic.


## 2026-10-11 — Analyse et correction STOP camera en zone + collision paroi
- Signalement utilisateur Samsung : défilement s'arrête en zone, comportement non fiable. Audit GitHub `b1b1973` : en présence de `cruiseSet`, la caméra recevait `touch.cameraControlX/Y` latched et évaluait `soloRectangularCameraGain` de la position du doigt ; une empreinte à 50 % pouvait donner gain zéro alors que le vaisseau était près du bord. Inversement, finger en marge pouvait forcer scroll alors que le vaisseau était au centre.
- Correction : supprimer dépendance des coordonnées du doigt dans `soloUpdateCamera`, et déterminer le gain X/Y avec vraie position normalisée de la forteresse + vélocité sortante. Seuil centre sans inertie résiduelle ; `fingerUp` laisse `cruiseSet` actif.
- Autre cause : dans `Session::step`, la collision `Tile::Rock` annule les deux composantes de vitesse. Correction sweep indépendant X/Y, blocage d'un axe conservé sur tout le frame (un premier essai laissait traverser le roc après le substep bloqué ; régression ajoutée), dégâts et malus préservés.
- Tests CI intermédiaires rouges identifiés puis corrigés AVANT livraison : anciens arguments fingerZone dans test de croisière + sortie de rock par sweep insuffisamment bloqué. Tests core et SDL2 au commit correctif `cd9a568a` constatés verts ; Android et Samsung à vérifier, pas de présomption de stabilité matérielle.


## 2026-10-11 — SOLO-FX-MINING-001 (autorisation Fab « Ok, code »)
- SOLO seul : après décharge cinétique, les astéroïdes ordinaires comme glacés émettent des poussières visibles via `Session::scatterMining`. Collection de particules plafonnée à 128 ; seuls les fragments glacés infligent GLACE. L'énergie n'est pas inventée.
- FX originaux : `shield.png` pour le FLASH D'IMPACT visuel (pas un bouclier gameplay COOP), `pousc.png` pour la traînée selon vitesse réelle, `pousb.png` pour les débris.
- Préservés : zones de caméra selon position/vitesse du vaisseau, croisière après finger-up, zoom terrain ×3, taille pilote fixe, tir/cone 360°, modes historiques et `src/main.cpp`.
- Régressions poussière/plafond/collecte, release cône, flash ajoutées. CI Android APK/AAB et essai Samsung à confirmer.
- À faire : raccordement réel COOP énergie/bouclier, tirs/missiles et minage continu, FX exhaustifs. Aucune fusion `main` ni Release sans Fab.


## 2026-10-11 — SOLO-CONTINUOUS-CONE-002 (« Super, continue »)
- SOLO : fonte progressive multi-astéroïdes dès le seuil cinétique commun 0,20 s, toujours active après READY à 2 s. Plus de destruction instantanée par simple doigt levé.
- Vitesse distance et difficulté via `sfKineticMiningArenaFractionPerSecond`, cône partagé avec `inAimedCanonicalCone`. Conversion masse de case expérimentale `SOLO_TILE_REFERENCE_ARENA_FRACTION=.08f` isolée du moteur historique ; essais Samsung indispensables.
- Érosion 0..1 persistante dans `Session::asteroidIntegrity` entre les gestes. Sprite d'astéroïde rétrécit en continu, poussière blanche (glacée ou ordinaire) à la destruction complète.
- Régressions : seuil 0,20 s, charge 2 s, maintien prolongé, annulation, différence Danger 1/9, rotation 360°, multi-cible et glace.
- Aucun changement de caméra, croisière, zoom ×3, contrôles historiques, `src/main.cpp`. Vraies armes COOP, énergie, bouclier, boss et FX exhaustifs encore à raccorder. Pas de merge main ni Release.


## 2026-10-11 — SOLO-FINGER-ZONES-003 — correction validée « Ok, corrige »
- Bug : `solo_prototype.cpp` utilisait `stepCruise` (direction/throttle mémorisés), pas la position écran du doigt. Caméra basée sur vitesse ship ; arrêt anormal quand le doigt est fixe.
- `solo_navigation.hpp` : coordonnée absolue du doigt, transformée avec pan futur de caméra, puis mouvement borné via `Session::stepToward` et collision sweep existante. Caméra pan continu selon la zone réelle du vaisseau à l'écran, PAS vitesse du vaisseau : gain 0 au centre, deux bandes 0→55→100 %. Pan par axe plafonné au déplacement réellement obtenu pour bloquer contre murs.
- `TouchPilot::steeringEngaged` reste actif sur finger-up pour mémoriser dernière position écran (pas la direction), permet continuité en zone extérieure. Retour au centre = caméra immédiatement fixe.
- Vitesse finie alignée borne COOP 3,10 largeurs/s ; tir 360°, zoom ×3, taille vaisseau inchangés. Aucun changement au jeu historique ni `src/main.cpp`.
- Tests : geste vers bord, doigt immobile et pan persistant, finger-up, retour centre, mur, défilement vertical et courbes. CI Android et test Samsung requis. Aucun merge main/Release.


## 2026-10-11 — SOLO-CONCENTRIC-ATLAS-004 — première intégration
- Accord de Fab « Ok code » après prompt de passation. Nouvelle navigation **circulaire** : rayon externe 42 % plus petit côté, rayon intérieur moitié et intermédiaire 75 %, caméra arrêt central, couronne 0.1→2, couronne 2.1→6 et extérieur 6 fixe, radial dans toutes directions, doigt fixe/mémorisé, collisions conservées. Vitesses intégrées au pan en unités de colonne de carte sans changer la taille du vaisseau.
- Les deux PNG authentiques postés dans main sont repris par leur SHA Git (jamais merge main) : `assets/pict/solo/ParoisModeSolo.png` et `.../EnvironnementsModeSolo.png`. Rendu SOLO tire directement les plages peintes de ces planches, exclut textes des zones sources, supprime le fond sombre par alpha à chargement unique, thèmes roche/lave/énergie/glace/poussière/fumée et fallback vers ro1.
- Scripts d'assets contrôlent CRC/PNG de ces deux fichiers ; tests cercles indépendants XY, maximum fixe, diagonal, bornes de découpe atlas, et tests navigation existants. Réserver la validation visuelle réelle Samsung : atlas issu d'une planche de présentation, aucun sprite parfait garanti.
- IMPORTANT : ceci n'affirme PAS la parité canonique complète (énergie, missiles, orbes, vrai bouclier, méga décharge). Le moteur SOLO contient toujours `solo_combat.hpp` et `solo_canonical_adapter.hpp` encore non branché. Prochaine mission : audit puis rattachement des fonctions historiques effectivement réutilisables, jamais copies simplifiées. `main` et `src/main.cpp` inchangés ; aucun merge ni Release.


## 2026-10-11 — SOLO-CANONICAL-VESSEL-005 — règle partagée et rendu natif
- Autorisation Fab : demande de parité canonique vaisseau SOLO/COOP. Refactorisation MINIMALE de `ship_energy.hpp` et des trois règles de tir/recharge historiques vers `ship_energy_rules.hpp`, avec mêmes coefficients et noms de fonctions pures, réutilisées en SOLO : heat/nrj 0 plein–50 épuisé, dépense au tir 1 (+10 si missile), missile uniquement si recharge après +1 reste sous 1,5, loi vitesse orb, dispersion, dégâts bouclier / wear, recharge selon Danger, repousse hull et collecte de poussières blanches.
- `CampaignController::launch` active la parité dans les vraies sessions SOLO, mais les tests/unités isolées du prototype conservent leur ancien mode pour ne pas fabriquer de faux tests historiques. `SoloCombat` utilise les coûts/dégâts orbe=12 missile=60 et enveloppe d'accélération missile du canon, avec vecteur libre SOLO 360°, boss local conservé. Note : guidage missile SOLO vers la cible pointée, pas encore la prédiction d'une cible mobile COOP.
- Sprites de tir natifs `orbeb.png` et `missilebb.png`, vaisseau historique `sb.png`, bouclier `shield.png`, vraie jauge énergie visible sous HP, onde cinétique via durée/rayon `kinetic_shield.hpp`.
- Après 2 s de charge, au relâchement, purge RADIALE selon la règle exacte `sfKineticPurgeAsteroids` (rayon = diamètre vaisseau × 3 × .5, sans Danger), conversion astéroïdes/grille SOLO et onde partagée si une roche détruite. Ne touche pas aux murs ; la fonte progressive du cône avant relâchement persiste.
- Limites honnêtes : nous n'avons pas encore branché le moteur de combat global complet COOP dans le labyrinthe : boss/IA/effets multicouches, collisions astéroïdes libres, simulation guidage dynamique et états historiques complets requièrent intégration/adaptation supplémentaire. Graphismes atlas sont extraits dynamiquement et doivent être inspectés sur Samsung ; pas de prétendue parité complète validée.
- Tests CI core/SDL2/Android requis, aucune fusion main/Release, `src/main.cpp` inchangé.


## 2026-10-11 — SOLO mauvais rendu suite à atlas de présentation
- Symptômes photo Fab : parois répétitives toujours horizontales, coins tournés à l'envers, collage de vignettes brunes et bleu-violet carrées et animation peu naturelle.
- Cause prouvée : `soloWallAtlasSample(theme,x,y)` renvoyait chaque fois une bande horizontale de l'illustration, étirée au carré pour tous les murs, sans tenir compte des voisins; `soloEnvironmentAtlasSample` copiait des zones de présentation trop larges avec fond parasite. Le découpage alpha par seuil RGB supprimait même des zones sombres du rocher.
- Remède : masque topology N/E/S/W dans `solo_scenery.hpp`; vraie image coin source est une forme «┐» (liaisons Ouest+Sud=masque12, angle0), rotations corrigées pour tous les coins; extrémités horizontales/verticales orientées séparément. Images découpées au chargement, seuil alpha et adoucissement des bords, décorations peu nombreuses et phase calculée depuis coordonnées fixes. Tests systématiques de tous les masques, de leurs rotations, des bornes/catégories du sprite.


## 2026-10-11 — ASTRA-COOP-SOLO-ARCH-001 — constats d'audit, non corrigés

Référence `1d8a48e2`. [Preuves, chemins et propositions](docs/audits/2026-10-11-coop-solo-architecture.md). Aucune nouvelle exécution Android ou reproduction Samsung.

- **Parité incomplète :** solo_canonical_adapter est testé avec FakeShip mais non raccordé à la boucle COOP. Session/Combat/Renderer réimplémentent une partie du comportement. Les tests locaux de formules ne constituent pas une comparaison de simulations.
- **Unités :** les tirs SOLO emploient map.width=54 comme référence, alors que la vue expose 6 colonnes. La largeur du monde influence alors le comportement apparent. Séparer référence de combat / monde / région de rencontre / viewport avant toute migration.
- **Caméra :** le pan maximal 6×0,72 = 4,32 largeurs/s dépasse le plafond COOP 3,10 ; la navigation actuelle augmente son plafond avec le pan. La mémoire antérieure disant simplement « aligné 3,10 » ne décrit pas cette combinaison. Le fallback clavier utilise encore les rectangles.
- **Contacts :** le sweep de Session vérifie le centre contre les cases rocheuses, pas le volume complet de la coque. Le repoussement historique par boss nécessite aussi une résolution contre parois ; le seul déplacement tactile ne suffit pas.
- **Rendu :** textures et proportions du SOLO ne sont pas celles de sfCoopDrawArena. Boss en case et missiles visant un point fixe ne sont pas les systèmes historiques.
- **Parois :** planche à colonnes inégales mais découpe theme×256 ; redimensionnement carré de formes allongées ; miroir après rotation des coins pouvant invalider leur masque ; alpha et chevauchement ne garantissent pas une continuité. Les correctifs précédents restent présents, mais leur efficacité visuelle complète n'est pas prouvée.
- **Compilation/états :** FabLib définit variables et fonctions non inline dans un header non gardé. Ajouter son inclusion à plusieurs unités peut introduire des définitions multiples. Les globals inline de cinétique restent partagés dans un processus. Aucun nouvel échec de compilation n'est allégué : cette branche n'a pas été recompilée pour l'audit.
- **Règles actuelles à préserver :** activation/vulnérabilité cinétique à 0,20 s, prêt à 2 s ; sfKineticSurgePower renvoie zéro pendant l'activité. Ne pas réintroduire l'ancien délai 0,30 s ni l'ancienne amplification de protection ×2 pendant la charge issus de mémoires plus anciennes.
- Décision corrective proposée : extraction graduelle avec comparaison réelle au témoin COOP. **Attendre validation Fab avant code.**


## 11/10/2026 — SOLO-COOP-SOCLE-001 — problèmes techniques identifiés/résolus
- Avant : SOLO imitait `sfCoopFire` et missile vers un point mémorisé, pas la simulation `sfCoopProjectiles`, Boss/astéroïdes sous forme de tiles. Ne PAS entretenir ce deuxième moteur ; prototype gardé pour comparaison.
- Real extraction `sfCoopPredictInterceptCore`/`sfCoopGuidedMissileCore` depuis fonctions anciennes puis wrapper historique réutilisant core ; test 1200 trajectoires à 30/60/120Hz. Ne pas confondre équations partagées et contexte de simulation réellement isolé.
- Danger pilote absent : `Spritej1->pv=0` déclenchait secours/fin de combat, ciblage et minerais fantômes. Nouveau masque `SfCoopPilotRoster` avec défaut 0x03, branché sur recherche de cible, rendu/physique, poussières, sauvegarde/défaite. En SOLO 0x02. Attention autres fonctions historiques non encore contextualisées.
- Danger boss absent : health=0 provoquait `sfCoopWin` et écriture de campagne. Flag `bossPresent=false` désactive boss avant rencontre ; `soloDiagnostic` capture victoire boss sans sauvegarde COOP. La procédure de test a `boss=0` en phase de vol, réactivation dans une salle arène.
- Erreur 360° : changer uniquement l'angle du tir laissait minage/aspiration et image du cône orientés verticalement. `SfKineticAimAxes` via vecteurs normalisés + dernier cap si cible confondue avec vaisseau ; la physique, l'aspiration et le rendu utilisent le même axe, en gardant coordonnées FabLib dans l'unité legacy et les coefficients COOP.
- Attention technique : `sfCoopDrawArena` originel dessine directement en coords arène, donc caméra reste IDENTITÉ à ce jalon. Déplacement COOP demeure borné arène, pas encore collisions parois. Les globals sont isolés par Android `:coopsolo` temporairement ; pas encore vrais contextes session multi-process locaux. Ne pas annoncer l'étape labyrinthe ni parité téléphone.


## 2026-10-11 — BUG: SOLO COOP audio oui / menu sélection CLASSIQUE superposé
- Preuve Samsung: `SpaceFortressCoopSoloActivity` lancée, aucune exception, sortie 0 après ~27 s ; son du jeu entendu, UI mode CLASSIQUE au premier plan. Ce n'était PAS une activité Android superposée ; le menu était redessiné par le wrapper SDL HISTORIQUE.
- Dépendance : `th2.h` inclut `start_ui.hpp`, `remaster_runtime.hpp`, `remaster_visual_restore.hpp`, `help_live_bridge.hpp`. Ce dernier définit `#define SDL_RenderPresent SpaceFortressHelpLive_RenderPresent`. Le wrapper appelle `sfFixApplyUiRequests` qui remet l'écran HOME si `sfFixRequestedScreen` (défaut HOME) n'a jamais été mis à GAME. Puis `SpaceFortressUi_RenderPresent` peint `sfUiDrawHome` SUR le rendu COOP, ce que la CI de 7 images headless ne détectait pas.
- Correction `b9072d24` : restaurer temporairement le vrai symbole SDL_RenderPresent uniquement dans l'inclusion de l'arène diagnostic, puis remettre l'interception CLASSIQUE/COOP intacte. Build-time asserts préprocesseur dans l'arène et dans la smoke. Ne pas 'réparer' ceci en ajoutant un menu officiel ou en modifiant `src/main.cpp`.


## SOLO-COOP-PRESENT-002 — livraison contrôlée après CI (11/10/2026)
- Correction code `b9072d24fd0b7452a88f8ec4cccb4b70e9cf51ae` : vrai appel `SDL_RenderPresent` exclusif à l'arène, routeur CLASSIQUE restauré pour modes historiques.
- Workflow Android `38167735625` entièrement VERT (SOLO, SDL2, regressions C++ historiques, compilation APK/AAB), aucune Release.
- Workflow extraction `38168138218` VERT : nouveaux binaires du commit corrigé dans `temp/` sur `feature/solo-coop-core-v1`, remplacement confirmé par manifeste.
- `temp/SpaceFortressVs-1.4.2.apk` : 94 119 741 octets, SHA256 `806da4277b599c7d1b19fb3c8a4ff45531bffe503475c8bb8d3b99f08d99546a`, package séparé `com.greenpower2669.spacefortressvs.diagnostic`.
- `temp/SpaceFortressVs-1.4.2-unsigned.aab` : 91 464 418 octets, SHA256 `d52c91ffb664165f0d9fd8f8d3aba1f3797a06384a6fd87835d5095c9840d10b`.
- **Attendre validation sur Samsung** : vraie arène COOP visible sans menu de sélection CLASSIQUE, avec sons, vaisseau, tir/cône et bouton boss. Pas de merge main ni publication.


## 2026-10-11 — Son de charge cinétique décalé dans l'arène COOP/SOLO
- Reproduit par lecture code : l'arène expérimentale `coop_solo_arena_runtime.hpp` jouait `kinetic_charge.wav` dès le FINGERDOWN du deuxième doigt / espace. Le canon COOP verrouille au contraire l'activation physique du cône à 0.20s depuis la pression, et le signal READY à 2.00s. Deux pipelines audio (manual + historique) provoquaient des séquences confuses.
- Fix `74206425` : enlever le son immédiat et les duplications de Mix_Chunk ; surveiller `sfKineticSurges[1].heldSeconds` après vraie simulation `sfCoopTick`, déclencher une seule fois `sfKineticAudioStartCharge` à 0.20s ; `sfKineticAudioUpdate` (appelé dans la boucle historique) gère READY/quiet hold à 2.00s ; relâchement total joue release + purge, annulation ferme canal. Pause efface état de son.
- Test déterministe cas 0/.1/.199/.2/2s, double appui, annulation, relâchement. Restent : test audible Samsung, asset charge/ready/release présents dans paquet, aucune garantie sur perception sonore avant essai.
- L'adaptateur technique phase 2 monde/camera/collision est seulement préparé, non branché : `coop_solo_world_bridge.hpp`. Pas d'écart caché de vitesse (4.32 versus 3.10 à arbitrer).


## 11/10/2026 — Validation CI et remise APK SON-003 / WORLD-004
- Build/test du code exact `9927f0148486eefb5f49a201b9fd0602151c1905` : GitHub Actions `38169721589` VERT, jobs `solo-core-regressions`, `solo-sdl2-ui`, `build-android` tous success ; `publish-release` skipped. 
- Décompression validée `38170223921` VERTE : `temp/SpaceFortressVs-1.4.2.apk` dans `feature/solo-coop-core-v1`, 94 119 734 octets, SHA256 `47c575b49877dc1af8bceaa966b8201978612edd794a9b7e17fe987726bfd33c`, package Android diagnostic séparé. AAB non signé SHA256 `635fbc4dfac186a27f51155e34123e90218491af80efb0cf8ae02bdf74279f92`.
- Le son de charge 0,20s / READY 2,0s est implémenté mais non encore écouté sur Samsung par Fab. Phase2 pont monde/collision/caméra testé isolément, PAS encore disponible dans l'arène interactive. Pas de merge `main`, aucun changement `src/main.cpp`, aucune Release.


## 2026-10-11 — Risques surveillés pour le monde interactif du moteur COOP
- Immédiatement visible : l'ancien moteur contraignait pilote et tirs aux tailles écran `sfArenaW/H` ; impossible de traverser une carte 54×160 sans opt-in world coordinates. Nouveau `soloWorldResolve` remplace seulement le clamp, et `soloWorldWidth/Height` prolongent les projectiles.
- La projection viewport SDL transporte les VRAIES entités anciennes sans leur toucher dans la simulation; `sfCoopDrawArena` ne doit redessiner ni le fond/ni le HUD historique derrière le menu en WORLD. `src/main.cpp` protégé.
- Les astéroïdes créés par `setasts` historique naissent aux 4 coins de l'arène de référence; en mode monde déplacer au SPAWN uniquement vers la région de l'unique pilote. Limites `inxy` de campagne : fournir extents monde en paramètres facultatifs `sfLegacyFieldFrame`, sans forcer W/H mondiaux (ce qui changerait masses et vitesses). Auditer encore les autres helpers de limite par écran durant test Samsung.
- Canons : carte env 54×160 ; largeur de combat reste 540 px; pan circulaire borné par delta collision, pas de mouvement fictif. Premier doigt suit cible monde recalculée; second doigt reste propriétaire de la charge audio 0.20→2.00 s.
- À tester en priorité : SDL viewport négatif de grande hauteur sur GPU Android ; conservation rendu arrière-plan et position du vaisseau quand pan; collision coque/glissement dans couloir ; objets physiques dynamiques visibles et bornes champ. Cette première version n'implémente pas les collisions de tirs ni boss avec parois, et n'est PAS une exploration complète avec relais/sauvegarde.


## 2026-10-11 — WORLD TEST livraison Android vérifiée
- Lancement `feature/solo-coop-core-v1` : monde opt-in « MONDE TEST » derrière entrée diagnostique « SOLO : VRAI MOTEUR COOP » ; rendu/collisions/inertie pilotés par le VRAI moteur COOP; arène BOSS TEST conservée. Correctifs `bee803ee`, `3b52fca7`, `d86c1bfe`, `235661b3` (SDL_unsetenv absent dans SDL2, correction test uniquement) et `82825ed5` (levée doigt = vitesse SOLO remise à zéro, caméra figée).
- GitHub CI `38171993509` : TOUT VERT, tests isolated SOLO, SDL2, tests historiques de vrai COOP/collision paroi et smoke native 2 modes, compilation Android ; publish-release SKIPPED. Run précédent `38171610964` rouge UNIQUEMENT à cause du test `SDL_unsetenv`, corrigé. Ne pas effacer ses logs sans accord Fab.
- Artefact définitif extrait dans `temp/` par workflow VERT `38172484944`. APK diagnostic `temp/SpaceFortressVs-1.4.2.apk` commit source `82825ed5e2889ac630fe2379183bc068f0105d01` SHA256 `635c936e0b4fc1335077c2da9d2f236b92e69d8446761703012aa4b65754d5d2` (94 136 117 octets), identité Android séparée ; AAB NON signé SHA256 `14c0df866eba3f704912b1241a2bbbb066311691e8a1d6ad13dca0b110b06c9c`.
- Limites non résolues : absence de collisions projectiles/parois, boss déclenché par progression, relais et sauvegardes ; habillage parois diagnostique seulement. **Validation Samsung de la caméra, déplacements et sons est encore à effectuer**, CI verte ne la remplace pas.
- Aucun merge `main`, Release ni modification `src/main.cpp`.


## 11/10/2026 — BUG retour téléphone SOLO MONDE : grand viewport Android, anneau vert sans sprite, caméra décolle du doigt
- Repro utilisateur par capture : fond nébuleuse et gros blocs roche affichés, anneau vert presque seul ; absence d'avatar nettement distinct et position qui décroche du doigt. Le cercle vert correspond vraisemblablement à l'effet de supercharge du moteur, **ce n'est pas une preuve qu'il représente le sprite**.
- Piste GPU fondée sur code: `SDL_RenderSetViewport` dimensionné à `worldHeight=160*sfArenaW/6` peut atteindre ~28k px sur Samsung, hors bornes GL courantes. Aucun code erreur SDL tracé avant le fix. Solution sans faux moteur : seul le rendu applique `world.toScreen` sur les vrais `SfCoopShot`, sprite historique bleu, champs, particules, astéroïdes, la simulation demeure monde.
- Piste tactile démontrée géométriquement : pan désiré max 4.32 largeurs d'écran/s contre déplacement COOP max 3.10 ; clamp pan au déplacement total empêchait l'avatar d'atteindre le doigt s'il avait du retard. Maintenant clamp au surplus après position doigt; vitesse/physique conservées. Test déterministe doigt stationnaire, blocage mur, projection au bout map.
- Test pixel dans la CI native smoke en mode monde vérifie viewport écran et présence de couleur bleu-clair au centre de l'avatar. CI `38174175100` en cours. Ne pas inventer d'APK ni conclure sur rendu téléphone avant nouvelle preuve.


## 2026-10-11 — Livraison vérifiée ANNEAU/VISIBILITÉ PILOTE ET CAMÉRA DOIGT
- Correctifs de code `09f8214c`, `999aedb8`, `63911245`, `e4be8b58`, `ad24535f`, `1088c265` (vaisseau bleu historique rendu dans viewport écran et projeté depuis monde, cône/ring/dust/orbes/missiles réels réutilisés, fallback lumineux si PNG absent, caméra attend le rattrapage du doigt avant de faire défiler le monde, pas de pan sur mur, vraie physique COOP préservée).
- CI exacte `38174175100` **TOUS LES JOBS VERTS**, inclut `SDL_RenderReadPixels` sur centre du vaisseau en mode monde, rendu viewport écran, test de rattrapage du doigt et smoke vrai moteur, compilation APK/AAB. Run `publish-release` SKIPPED.
- Export GitHub `38174728231` **VERT**, fichiers DÉCOMPRESSÉS dans `temp/` sur `feature/solo-coop-core-v1`. Manifeste testé commit `1088c2657642b603fb85b5cba5e7e839152f9890`; APK `temp/SpaceFortressVs-1.4.2.apk` SHA256 `a23c9d0cc5e1222b703504ea35bbd5e824b6119dcfddc30eb6a799fe1f85d76d`, taille 94 136 121 octets, package `com.greenpower2669.spacefortressvs.diagnostic`. AAB non signé SHA256 `55e40cf1000a67975a6afb0433f5747d0e0210e4aedc936204d9800944f8c2fe`.
- **À faire sur Samsung** : installer le DIAG, MONDE TEST, identifier le vrai vaisseau BLEU sous l'anneau vert de supercharge, vérifier l'adhérence au doigt immobile pendant défilement, test rapide doigt → rattrapage physique, blocage mur; signaler toute dérive sur le screenshot. Vert CI ≠ validation ergonomique téléphone.
- Jamais de merge `main`, jamais de Release, `src/main.cpp` inchangé.


## 11/10/2026 — RÉGRESSION : Vaisseau corrigé mais caméra plus de défilement
- Le code de la version `1088c265` avait un pan qui refusait de consommer le déplacement avant que le vaisseau atteigne le doigt (`excess` correct), mais la cible monde, elle, était toujours `toWorld(finger)`. À l'alignement, vitesse COOP zéro → camera delta zéro. Pas un problème GPU ! L'ancien GPU gros viewport a été corrigé et ne doit pas revenir.
- Fix : générer une cible monde « légèrement au-delà du doigt » uniquement dans l'anneau extérieur, direction de `soloCirclePan`, lookahead calibré `panSpeedPixels/30` au véritable contrôle 3.10W max. Ainsi doigt immobile commande encore un mouvement physique : pan se cale sur CE déplacement, coque + obstacles prioritaires.
- Ne JAMAIS traduire artificiellement le monde ou la caméra sans mouvement accepté ; ne JAMAIS toucher aux vitesses CLASSIQUE/COOP. Tests véritable pilote au mur et 4 cercles. Vérification Samsung nécessaire après CI `38178296951`.


## CAMERA-HOLD-007 — diagnostic CI complet et correctif validé 12/10/2026
- Le gel écran provenait de la cible physique `toWorld(doigt)` : à l'arrivée, vitesse `distance*30`=0 donc caméra0. Non du rendu! Ajouter avance radiale proportionnelle à la vitesse de zone/30, recalcul à chaque sous-pas; vraie collision et vitesse historique restent souveraines.
- Les premières CI rouges étaient un faux échec du **nouveau test** : la carte générée place START presque à la limite sud, et un simulateur de doigt à y160 plaçait artificiellement la caméra au-delà de max `worldHeight-visibleHeight*.5`, provoquant dès frame0 un re-clamp de ~95px interprété comme drift. Test corrigé : position initiale du vrai pilote 250px plus au nord dans le couloir ouvert et assertion préalable de validité de la caméra ; validations strictes poursuivies.
- CI 38178801383 ENTIÈREMENT VERTE, export 38179331501 VERT vers temp. Voir manifest 908749e et SHA APK 372b1376... Vrai téléphone à re-tester.


## 12/10/2026 — SOLO WORLD CLEANUP-008 / prévention régression future
- Audit: `sfCoopTick` sans boss ne lançait déjà pas les routines bonus/tourelles, mais un boss futur les aurait réactivées. Correction explicite sur le drapeau `soloWorldMode`: aucune mise à jour de l'IA tourelles ni minuteur/apparition/collision de bonus; purge d'entrée des projectiles kind=5 et de l'état tourelles/bonus. Rendues invisibles ET physiquement absentes. Le reste (astéroïdes, poussière, tirs, énergie, caméra) est conservé.
- Vérification C++ ajoutée avec un boss fictivement présent et un retour à l'arène historique; CI nouvelle à vérifier. Test caméra hors borne corrigé précédemment et non neutralisé.


### SOLO CLEANUP-008 — preuve d'intégration
- CI #38182884348 SUCCESS (solo-core, SDL2, regressions historiques/Android, AAB); nouvelle régression de suppression tourelles/bonus incluse et verte. Export GitHub #38182981381 SUCCESS avec manifest source 518f331, APK SHA256 e6ab53161be15f10c8675098d4b7f325b6c1b060edc1ecede7f27ee92ae6820a. Le précédent test caméra hors bornes demeure strict et vert. Reste confirmation ergonomique Samsung.


## 12/10/2026 — METEOR-009 : premier Android rouge résolu, tests intacts
- Android run 38190409848 échouait uniquement sur `sa1.begin()+oldCount` dans `sfCoopSoloMeteorites` : `sa1` est `std::list<sprite*>` dans moteur natif et son itérateur n'accepte pas l'addition. Fix `auto it=sa1.begin();std::advance(it,oldCount)` commit 2650237, aucune assertion retirée.
- Run final #38190640721 : **success** pour tests SOLOC++, SDL2 tactile, Android native/Java/Gradle/APK/AAB ; `publish-release` skipped. Extraction #38190657369 **success** avec vérification SHA ; APK 07bebf33edd1f366cd34fc965c7c5b2b75a6e9130cbfb0f33ef41c7f55dd7d6d. Pas de validation visuelle Samsung encore.
- Vague météore basée sur `setasts(1)` exclusivement monde : bonus/tourelles toujours absents, le champ réel boss absorbe 55 % et réduit PV; pas de duplication recharge, vraie poussière/minage. Boss test du monde stationnaire seulement : AI, progression et murs/projectiles restent phase suivante. Classements et classiques préservés.


## 12/10/2026 — Incident Samsung : météorites SOLO coincées (non résolu physiquement)
- **Fait utilisateur / appareil** : Fab constate des météorites plantées dans les parois du labyrinthe après APK METEOR-009. Pas de logs trajectoire/état fournis : impossible de savoir pour chacune si elle naît dans ROCK, entre ensuite dans ROCK, ou subit un défaut de rendu/coordonnées. **Ne pas écrire « corrigé sur téléphone ».**
- **Analyse statique séparée** : `sfCoopSoloMeteorites` posait un cercle hors `Tile::Rock` avec `soloWorldCanOccupy` à t0 mais dirigeait `vx/vy` vers le boss, sans vérifier les segments suivants. `sfLegacyFieldStep` exécute `rock->ast()`, traite champ/limites monde, collisions vaisseau et astéroïdes, mais AUCUN obstacle de tuile ROCK. Le spawner de fond `sfCoopAdvanceAsteroidSpawner` utilise aussi `setasts` puis translation autour du pilote sans test de carte. Donc cohérence naissance/chemin non démontrée ; pistes à tester isolément.
- **Correction réversible sans nouveau gameplay** : drapeau compile-time désactivé `SF_COOP_SOLO_METEOR_WAVES_ENABLED=false` pour les vagues bonus METEOR-009 uniquement ; astéroïdes historiques toujours présents pour les ressources. Test de non-génération avec/sans boss, plus test de trajectoire bloquée par un ROCK dans le pont de monde (NE teste PAS le runtime astéroïdes). Test matériel à refaire sur la prochaine APK après CI ; aucune annonce de résolution des déplacements.
- Boss stationnaire et boutons = banc d'essai, non réalisation de la rencontre historique. CLASSIQUE et COOP inchangés ; ne toucher ni caméra/pilotage ni `src/main.cpp`.


### SOLO-STABILISATION-010 — correction de test / preuve finale
- Première CI de suspension #38193469094 : échec **nouveau test SDL2 synthétique** lorsque `moveWithoutTunnelling(before,after,12)` a dépassé l'arrêt initialement attendu malgré ROCK intercalaire. Cela révèle une vulnérabilité potentielle du résolveur sur ce cas artificiel, **pas un fait observé sur le pilote Samsung**. Contrôle du vaisseau gelé volontairement, ne pas le modifier sans audit et ordre explicite. Correction du PÉRIMÈTRE du test dans `6540adece` : échantillonne géométriquement le segment entre deux centres libres et démontre présence de positions ROCK bloquantes, sans prétendre valider l'intégrateur historique des astéroïdes ni le déplacement du vaisseau. Ne pas omettre cet ancien échec au prochain audit.
- CI finale #38193630900 SUCCESS (SDL2, C++, Android), export #38193705276 SUCCESS; APK SHA b0cc1fad1543e4a66729a014bd6cb30f2222b9de7a6aa43c98c02dc9f4e36e64 dans `temp/solo-world-stabilization-010/`; tests téléphone sur le point précis du bug à refaire. Cette livraison SUSPEND les vagues METEOR-009, ne répare pas physiquement les astéroïdes historiques dans les parois.


## 12/10/2026 — TOURELLES SOLO : diagnostic différentiel/ preuve
- Témoin utilisateur Samsung : tourelles visibles lorsqu'un boss de test apparaît, boss un peu sombre, météorites autour OK. **Pas de preuve que le téléphone utilisait APK `6540adece`**; la seule preuve GitHub est le manifeste du diagnostic 010. Version 011 affichera `SOLO-011` dans les deux écrans.
- Source : l'arène un-pilote `sfCoop.soloDiagnostic=true` avait `soloWorldMode=false`, donc avant fix `sfCoopDefences` animait et tirait quand boss présent, `sfCoopBonus` activait `turretTime`, `sfCoopDrawArena` appelait `sfDrawTacticalEffects` qui dessine 12 canons, et bonus. En revanche `sfCoopSoloWorldActors` ne rend ni canons ni bonus; les fonctions bloquées sous `soloWorldMode` ne réintroduisaient rien dans la voie WORLD inspectée. **Conclusion : défaut démontré en ARENE du diagnostic; rattachement exact au visuel du téléphone à vérifier**.
- Remède source `ad5cb08e` : garde supplémentaire `soloDiagnostic` à création/physique/bonus/rendu, purge états et tirs kind5, test relancement + boss basculé. Tests classiques ne doivent rien perdre. Tout combat réel boss historique reste hors périmètre. `SF_COOP_SOLO_METEOR_WAVES_ENABLED=false` inchangé ; mines/poussière astéroïdes d'origine conservés.
- Run CI 38209069899 démarré, pas déclarer APK prête avant SUCCESS et export vérifié.


### SOLO-011 — CI/export sans régression du moteur
- `38209069899` intégralement **SUCCESS** (compilations, tests régressions, Android), publication sautée. Export `38209121497` rejeté uniquement au `git push` car documents mémoire avaient avancé la branche entre checkout et commit ; les fichiers APK/AAB et SHA y étaient déjà vérifiés. Workflow amélioré : `git fetch` / `git rebase origin/feature/solo-coop-core-v1`, jamais `force push`. Export de reprise `38209723734` **SUCCESS**, manifeste source `ad5cb08e`, APK SHA `513693f6b2b7fd07b810562fe8fc0c73e336eb3c2ebab02db980fc20106f0e21` dans `temp/solo-turrets-off-011/`. La CI valide les chemins logiciels, pas l'identité exacte des tourelles vues dans MONDE sur Samsung, toujours à confirmer.


## SOLO BONUS CORE-012 — risques identifiés avant code
Base1c6c0b5 : deux parcours SOLO existent (arène/world) mais bonus à tourelles correctement interdit par soloDiagnostic||soloWorldMode. Nouveau module doit couvrir les deux. Le monde ne bloque pas encore les trajectoires des rochers historiques ; ne pas prétendre corriger cela avec le Météore ciblé. Les nouveaux tirs doivent vérifier le segment contre les parois. Un simple canOccupy au point de loot n’établit pas son accessibilité depuis le pilote. Priorité aux tests boss toggle, gros impact traversant plusieurs seuils, arrêt de statut et coût d’une salve.

### CORE-012 — intégration et revue, 12/10/2026
- Brique C++ pure + adaptateur du vrai SfCoopState + rendu commun arène/labyrinthe implémentés. Salves humaines x3, guidage historique, coût canonique une fois ; furtivité semi-transparente conserve collisions ; missiles3s sans coût ; statuts Prison2s/Glace4s/Lave4s et Météore ciblé. Ancien bonus/tourelles et vagues METEOR-009 toujours exclus du SOLO.
- Revue indépendante : seuils enregistrés au moment du dégât avant soin poussière ; boss SOLO mort non ressuscité par poussières ; taille de coque identique au pilote monde ; BFS accessible calculé une fois par paquet, emplacements distincts. Coûts bornés et logs Android `SOLO_BONUS_012` moyenne/max temps simulation+rendu pour validation Samsung.
- Précontrôles : core pur GREEN, source SDL compilée ; régressions complètes/visuel/Android encore en cours à cet instant. Tests ajoutés par Sol puis poursuivis par Astra après interruption de son exécution. Benchmark hôte seulement : 12 récompenses/carte54×160, 12 lots, moyenne2,270ms maximum6,612ms ; aucune promesse de FPS Samsung.
- `src/main.cpp` inchangé. Pas de merge main/Release. Identifiant HUD SOLO-012 et carte accueil SOLO BONUS012. APK/AAB à livrer uniquement depuis CI verte exacte dans temp/solo-bonus-core-012/.

### CORE-012 — contrôle local terminé, candidat CI
`scripts/test-regressions.sh` termine code0 : historique C++/SDL2, assets, moteurs, 200 ouvertures/rendus COOP, champs et poussières cinétiques, source Android générée et smoke natif arène/monde. Nouveaux tests purecore, tirsréels/statuts/collisions/loot/isolation passent. Dernier ajustement HUD SOLO (plus de double HUD COOP, textes agrandis) recompilé et testé avec lecture pixels SDL ; capture inspectée. Contrôle pont monde réexécuté après ajout des24placementsdistincts ; protection launcher diagnostic OK. Le boss monde reste FIXE de test, Prison/Glace y montrent des statuts, mouvement seulement démontré sur boss mobile de l’arène. CI Android et export exact restent à constater ; APK non annoncée avant cela.


## CORE-012 — livraison vérifiée 13/10/2026
Régressions C++/SDL2/Android et huit pouvoirs vertes au SHA compilé. Seuils de drops capturés avant soins, aucune résurrection boss mort, rayon coque réel et BFS par paquet. Perf hôte seule mesurée ; FPS Samsung inconnu. Défaut des astéroïdes historiques contre murs NON corrigé, METEOR-009 toujours suspendu.

Source `95743f2386b220bb692112197c4ddee02d27ae2b` ; [CI Android/C++/SDL2 verte](https://github.com/greenpower2669/SpaceFortressVs/actions/runs/38299079636), publication Release SKIPPED.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-012-95743f2.apk` : 94168883 octets, SHA256 `c61d7e39ecb075d69db863e7cc964021bb9c3ed31ccf0c0597b01b1ffaa09248`.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-012-95743f2-unsigned.aab` : 91525121 octets, SHA256 `a4433fc2f09cae480ab407f1997139821602caa71c0c0f8a61941baceeed6289`.

## 13/10/2026 — SOLO BONUS-013 — diagnostic et correctifs
Retour Fab : CORE-012 ne donnait pas l’impression que les récompenses sortaient du boss ; jauges joueur occultées par la navigation Samsung ; explosions d’astéroïdes répétitives ; besoin d’un panneau manuel TEST BONUS.

**Cause bonus prouvée en source :** `sfCoopSoloBonusCollectAndDrop()` calculait une destination accessible puis appelait directement `addPickup(..., at.x, at.y)`. Les seuils 75/50/25/0 provenaient bien du boss, mais le pickup naissait visuellement à sa destination. BONUS-013 conserve la destination gameplay mais ajoute `SfSoloBonusEjection` : origine figée = position réelle boss, cible = pickup validé, pickup caché/incollectable jusqu’à la fin de l’éjection (~0,42–0,53 s). En WORLD, la sélection filtre aussi `firstWallImpact(boss,target,clearance)>=.999`; le rendu WORLD suit ce segment sans arc afin de ne pas traverser ROCK.

**TEST BONUS :** le HEAD GitHub CORE-012 n’avait pas encore ce panneau. Ajout d’un toggle `TEST BONUS OFF/ON`; ON affiche huit types + `TOUS X8`. Chaque commande alimente le même pipeline de placement/éjection que les drops boss. Boss absent/mort => commande ignorée. Fermer le panneau change uniquement le booléen d’affichage : aucune pause, reset, suppression de pickup ou bascule BOSS.

**Samsung :** cause visible en source : barres joueur autour de `.925H` et `.963H`, l’énergie pouvant être sous la barre système. `sfPlayerSafeBottomInset` relève uniquement le HUD inférieur du SOLO diagnostic ; tentative `SDL_GetDisplayUsableBounds`, puis fallback Android `height/18` si le fullscreen rapporte tout le framebuffer. Les fonctions HUD boss ne dépendent pas de cet inset.

**Audio :** le chemin `sfFieldMiningSound` partageait le chunk d’impact avec d’autres événements. Il est maintenant séparé : variante pseudo-aléatoire entre les deux prises historiques d’explosion quand disponibles, volume borné de MIX_MAX_VOLUME à MIX_MAX_VOLUME-10, canal 8 réservé. `sfCoop.soundHit`, boss, tirs et collisions gardent leurs chemins. Dans le diagnostic, la deuxième prise est passée par un argument `asteroidAlt` distinct afin de ne pas rendre audible/modifier le son boss.

Tests ajoutés/renforcés : origine boss, état in-flight non collectable, 8 tests simultanés, mur WORLD, bornes FX, safe-bottom Android native smoke. C++/SDL2 verts sur commits intermédiaires ; Android final du HEAD `77a42531` reste à confirmer. `src/main.cpp` vérifié blob `835059a0...`. METEOR-009 toujours suspendu.

## BONUS-013 — livraison vérifiée 13/10/2026
Les deux RED intermédiaires provenaient de tests trop stricts (premier pickup supposé PV; hash audio ne couvrant pas les deux variantes sur 32 séquences), corrigés sans modifier le contrat gameplay. La CI finale exacte est verte.

Source `488de2195e2aa2a1e40f2eec96abaeba19863fad`; CI Android/C++/SDL2 `38361581767` SUCCESS; publish-release SKIPPED.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-013-488de21.apk`: 94185274 octets, SHA256 `4a3511c380d04b21b46593806e01d991e9320c44691c529c6022e60aa4d1a18b`.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-013-488de21-unsigned.aab`: 91532662 octets, SHA256 `3fb742b7b3964620b5f0b8eb76dae0d0fc2b18f2c2e245d87809b08468a68d3a`.

## BONUS-014 — livraison vérifiée 13/10/2026
Audit BONUS-013 avait montré quatre écarts réels : ancienne planche statique/crops, HUD SOLO redessiné à height-58/height-40 après le HUD commun, audio sans vrai changement de vitesse, et TOUS X8 non garanti atomique. BONUS-014 les corrige. L'atlas de production est généré de façon déterministe depuis les motifs SOLO, validé 1536x768 et utilisé par le renderer; le projectile ne joue que les frames 0..3, l'impact et le statut boss les frames de fermeture 4..7/5..7. Audio: resampling linéaire 0,97x/1,03x en AUDIO_S16SYS, canal 8 isolé. CI exacte finale verte.

Source `f9323f3cd239e9027d964e38db369a9217ef02cb`; CI Android/C++/SDL2 `38420099395` SUCCESS; publish-release SKIPPED.
- Atlas animé: `assets/pict/solo/SoloBonusAnimations.png`, 1536x768, 4x8 frames de 192 px, SHA256 `1a337adb15a6a3901b76da566dcd26a1d2d76c26893864a37a85e97b63950001`.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-014-f9323f3.apk`: 94989574 octets, SHA256 `0dfe83dbb1f2716e33442a2d2933356b1a6582180a85a9c2fed5a77b1ebe59ff`.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-014-f9323f3-unsigned.aab`: 92342299 octets, SHA256 `564b0c9427bd776ed93bcb32d3012d86d232f3354ffded5d85b020cfd7424965`.


## SOLO GAME-015 — risques confirmés par audit, 14/10/2026
`soloDiagnostic` pilote actuellement aussi l’exclusion tourelles/bonus hérités, le HUD Samsung, les bonus SOLO et l’absence de sauvegarde COOP : le mettre simplement à false réintroduirait ces défauts. `setWorldMode(false)` efface les pouvoirs et ne convient pas à la transition finale. Le boss WORLD est stationnaire ; seule la branche arène de sfCoopTick contient le combat historique complet. `runCoopSoloArena` possède SDL/window/mixer et SDL_Quit : ne pas l’appeler dans la boucle historique du menu. Les helpers d’en-tête restent dans l’unique TU historique pour éviter la duplication FabLib ; JNI séparé sans FabLib.
Le placement libre d’un astéroïde ne prouve toujours pas sa collision correcte avec les parois. METEOR-009 reste suspendu. Les miniboss devront tester les segments contre les murs, ne jamais alimenter le pipeline de récompenses, et ne jamais écraser l’état du boss final. Android matériel/performance : à valider après CI, pas réputé acquis.

GAME-015 / étape 1 : sélecteur SOLO, activité interne `:sologame`, hôte commun et droits diagnostic séparés implémentés localement. Test ciblé RED (injection hors diagnostic acceptée auparavant) puis GREEN ; launcher statique GREEN. Suite globale arrivée aux tests cinétiques finaux sans erreur visible, statut de processus perdu lors de la reprise de session : une exécution finale avec code retour enregistré reste requise. Android et Samsung non encore validés.

GAME-015 tests : la première assertion de clip SDL comparait les coordonnées avant changement d’échelle à celles après celui-ci (3,4 vs 4,4). Mesure avant/après capture confirme restauration exacte du rectangle réellement actif ; test corrigé pour comparer cet état observable. Aucun correctif renderer requis pour ce cas. Sauvegarde illisible détectée sans écrasement ; collision astéroïdes historiques/parois toujours incident ouvert, METEOR-009 OFF.

GAME-015 / contrôle de revue : test SDL réel avec logique 3200x4800 et snapshot borné à 2048 px confirme le rendu jusqu’au coin inférieur droit. Test furtivité/contact miniboss reproduit un défaut réel (assertion PV inchangés) puis passe après séparation ciblage/contact/cône. La première suite complète était verte ; la suivante a reproduit ce test RED pendant correction. Nouvelle suite finale lancée sur les correctifs. Nettoyage explicite des anciennes poussières/explosions/ondes lors du changement de scène, sans remise à zéro des pouvoirs du pilote.

GAME-015 / validation locale finale 15/10/2026 : `bash scripts/test-regressions.sh` terminé avec code 0 ; tests ciblés de furtivité/contact et changement de scène verts. Revue indépendante finale sans défaut critique/important restant. `src/main.cpp` conserve le blob 835059a0ecfe0f74708068b3259cad5db1cdb579. CI Android et export restent à confirmer au SHA du commit envoyé. La modification locale ultérieure de ParoisModeSolo.png est séparée et exclue du commit GAME-015.

## GAME-015 — livraison vérifiée 15/10/2026
GAME-015 : revue indépendante sans défaut majeur restant, 1080 combinaisons monde/niveau/danger traversables avec quatre miniboss distincts. Corrections de revue : temps résiduel après transition, collisions furtives et nettoyage des FX de scène. Tests SDL haute densité, régressions historiques, tests huit pouvoirs BONUS et CI Android exacts verts. src/main.cpp inchangé ; tourelles SOLO et METEOR-009 désactivés. Performances et cycle de vie Samsung restent à tester.

Source `f5b4ce4f82e99057d041767d982914e903afbf7f`; CI Android/C++/SDL2 `38724287864` SUCCESS; publish-release SKIPPED.
- Atlas animé: `assets/pict/solo/SoloBonusAnimations.png`, 1536x768, 4x8 frames de 192 px, SHA256 `1a337adb15a6a3901b76da566dcd26a1d2d76c26893864a37a85e97b63950001`.
- `SpaceFortressVs-1.4.2-SOLO-GAME-015-f5b4ce4.apk`: 95023204 octets, SHA256 `f5138def32cdb8553e86006f6e079ce6a61a979b764c6c62f1757aca61ca8ec7`.
- `SpaceFortressVs-1.4.2-SOLO-GAME-015-f5b4ce4-unsigned.aab`: 92378949 octets, SHA256 `1b4d9a9bffb7ade770ec033bd25162c20c440c9a01db71ad8613c2156aee0e7c`.

## 16/10/2026 — SOLO PARALLAX-016
GAME015 dessinait les parois en aplats : atlas jamais raccordés. Correction par renderer dédié. Revue indépendante : scissor entier par cellule causait de vrais trous sous scale .75/.4266667/1.25. Correctif final : SDL_RenderGeometry à cinq sommets et quatre triangles, UV bornés, évite le raccourci SDL logiciel rectangle/copie sans arrondi de scissor. Test compare rectangle unique et32cellules, puis murs sur fonds rose/vert à4échelles et caméras fractionnaires : vert. Extraction des ressources, ownership/reset et isolation des autres modes vérifiés par revue ; pas d'autre défaut bloquant rapporté. Après interruption de session, reconstruction depuis le patch puis tests relancés ; suite globale/CI restent à confirmer.

PARALLAX-016 — export préparé : source `2a933a60639d890fd6168befb8ce1784ba9a18c8`, CI `38837760766`. Le workflow attend obligatoirement SUCCESS et vérifie le SHA exact avant publication dans `temp/solo-parallax-016/`. CI/export pas encore déclarés terminés.

## PARALLAX-016 — livraison vérifiée 16/10/2026
PARALLAX-016 : défaut de raccord à échelle fractionnaire trouvé par revue puis corrigé par éventail de triangles sans scissor par cellule. Tests pixel rectangle/32cellules et murs sur fonds opposés verts à quatre échelles. CI complète C++/SDL2/Android du commit exact verte. Validation visuelle et performances Samsung encore à faire.

Source `2a933a60639d890fd6168befb8ce1784ba9a18c8`; CI Android/C++/SDL2 `38837760766` SUCCESS; publish-release SKIPPED.
- Atlas animé: `assets/pict/solo/SoloBonusAnimations.png`, 1536x768, 4x8 frames de 192 px, SHA256 `1a337adb15a6a3901b76da566dcd26a1d2d76c26893864a37a85e97b63950001`.
- Fond ajouré : `assets/pict/solo/FondsParallaxeModeSolo.png`, RGBA1254x1254, SHA256 `4becaec70bf7f23d823c3eccbd902b86aa4dbb6942115a1389409bc6a7f7df44`.
- `SpaceFortressVs-1.4.2-SOLO-PARALLAX-016-2a933a6.apk`: 98400063 octets, SHA256 `62a029ca828a85783f324301bcb5d1411cfa8f343875f7e5ebf4108b5ce8515f`.
- `SpaceFortressVs-1.4.2-SOLO-PARALLAX-016-2a933a6-unsigned.aab`: 95691410 octets, SHA256 `1c18b7d99a45816a2332cd77415927cbf26cbb93cc1346897160c18eafc76703`.


## 16/10/2026 — SOLO PHYSICS-017
Audit PHYSICS-017 : vitesse remplacée vers la cible chaque tick ; FINGERUP annule la vitesse WORLD ; caméra uniquement si doigt tenu et limite son déplacement au dépassement du doigt. Roches : rock->ast() borné arène historique, aucune tuile ; paires statiques peuvent se détruire. Ces causes sont lues dans le code, pas un nouvel essai Samsung. Baseline locale stoppée après tests purs verts car pkg-config/SDL2 absents, installation en cours.

PHYSICS-017 — point de reprise 16/10 après interruption : implémentation locale des briques (inertie/caméra/zoom, repos/collision roches, visibilité audio, 10–12 rencontres/chasse) présente, NON LIVRÉE. Tests purs inertie Danger9/FPS, contact/repos roches, zoom exclusif, navigation réelle/occlusion et placement des rencontres passés. Suite complète a révélé une erreur de portée de variable dans le son du minage, corrigée ; compilation globale/tests runtime à reprendre. Environnement /tmp réinitialisé entre les tours, dépendances SDL en reconstitution dans scratch persistant de la session. Aucun APK-017 annoncé, aucune validation Samsung. Prochaine étape : tests réels moteur, revue, CI Android, export exact.

## PHYSICS-017 — implémentation et revue, 16/10/2026
Revue PHYSICS-017 : corrigés l’inertie perdue dans l’arène, les UP de sélection ignorés avec un autre doigt actif, le masquage des bonus et la double composition translucide, le couplage zoom/vitesse, les contacts de roches interpolés sur un chemin de glissement non rectiligne, et les fragments cinétiques simplifiés à tort. Calcul historique résiduel/6 ou 4 fragments conservé, placement seul sécurisé. Test moteur ciblé --solo-physics réussi, y compris parcours généré aux neuf dangers. Le test de parcours devait sélectionner une ancre nécessitant un détour (la première pouvait être en ligne directe) ; assertion rectifiée sans masquer de défaut moteur. Suite complète exacte et Android : CI à lancer.

## PHYSICS-017 — livraison vérifiée 16/10/2026
PHYSICS-017 : revue a corrigé inertie désactivée dans l'arène, UP tactile perdu avec deuxième doigt, double couche translucide/FX au-dessus des murs, couplage zoom/croisière, segment de glissement impropre aux contacts de météores et perte des trajectoires cinétiques historiques. Tests purs et moteur, SDL natif/UBSan et CI exacte verts. Samsung : ressenti, fluidité et visibilité restent à valider.

Source `3ef362cd1375034519c458a0effb2d7abef03490`; CI Android/C++/SDL2 `38956636032` SUCCESS; publish-release SKIPPED.
- Atlas animé: `assets/pict/solo/SoloBonusAnimations.png`, 1536x768, 4x8 frames de 192 px, SHA256 `1a337adb15a6a3901b76da566dcd26a1d2d76c26893864a37a85e97b63950001`.
- Fond ajouré : `assets/pict/solo/FondsParallaxeModeSolo.png`, RGBA1254x1254, SHA256 `4becaec70bf7f23d823c3eccbd902b86aa4dbb6942115a1389409bc6a7f7df44`.
- `SpaceFortressVs-1.4.2-SOLO-PHYSICS-017-3ef362c.apk`: 98432836 octets, SHA256 `8387c28fb63ae6d2fe4017af5ac3b29b49c906682e5124c8273031d6cebb2ac9`.
- `SpaceFortressVs-1.4.2-SOLO-PHYSICS-017-3ef362c-unsigned.aab`: 95725985 octets, SHA256 `3af4730f3104606e4e6f3af694baa3165a9cc3447174705c4553b72d6736b95f`.


## 16/10/2026 — défaut réel PHYSICS-017 : collage mur + délai apparent du rebond
Retour Samsung Fab : le vaisseau perd le contrôle un moment au contact d'une paroi. Cause démontrée : `campaign_runtime.hpp` met `control.velocity.vx/vy` à zéro si la position désirée est rejetée ; `moveWithoutTunnelling` renvoie une position sans normale/vitesse et le test `testSoloPhysicalShipAndCamera` imposait `vx==0`. Le doigt tenu en direction du mur réinjecte ensuite une accélération sans sortie de contact ; possible coinçage numérique si `canOccupy(from,radius)==false`. Également le premier doigt disponible déplaçait le vaisseau hors coque, en conflit avec le zoom direct. Correctif candidat `8875905` : solveur `moveWithBounce`, callback SOLO dédié, retour collision/rebond et fenêtre de neutralisation de la seule pression normale ; contrôle tactile corrigé et pincement direct. CI Android run `38962472820` et essai téléphone à confirmer, pas de régression déclarée corrigée sans preuve.

SOLO-018 / CI 1 `38962472820` : jobs solo-core et SDL2 SUCCESS ; job Android échoue à `tests/solo_physics_runtime_regressions.hpp:35` sur `c.velocity.vx<0` après injection irréaliste `100000 px/s`, et non sur une erreur de compilation du solveur. Test corrigé `a85ca0d` : force l'anti-traversée et la vitesse finie dans le stress multichocs ; la direction du rebond mono-choc est testée dans `coop_solo_world_bridge_regressions.cpp`. Relance `38963255438` en cours.

## SOLO-018 — export vérifié
SOLO-018 : CI exacte 38963255438 SUCCESS, code a85ca0dbd8e94e463d11e42b5b2cefb32ab31116. Export vérifié dans temp/solo-bounce-pinch-018. APK diagnostic + AAB non signé. Rebond et touch/zoom restent à valider sur le Samsung de Fab ; pas de main/Release.


## SOLO-019 — correction de la cible et de compilation
Une première lecture trop large voulait verrouiller l'antre. Fab corrige explicitement : ce sont les portions de chaque mini-boss / horde. Implémentation isolée par rencontre sans toucher à l'antre. Première CI de `16ce8df` : logique et SDL2 verts, mais test natif FabLib bloque sur `#define pi` opposé à `FirePad::pi` (`fablib.hpp:8`), pas un souci physique. Renommage en `padPi` au commit `568d689` ; CI exacte `38969053715` en attente. Compteur mini-boss et horde non modifiés, pas de faux résultat de test Android déclaré.

## SOLO-020 — détail physique et défaut de conception évité
Ancien inner radial ~0.21 fois largeur d'écran, soit ~113 px pour 540 px, quasiment identique aux deux diamètres du vaisseau COOP (~114 px). Impossible d'insérer une pré-bande mesurable sans décaler le début historique : SOLO-020 réserve ~32 px à une interpolation smoothstep 0→0.1 puis poursuit sans saut de vitesse. Pas de nouveau pilotage ni intégrateur, conservation des limites d'accélération et protections mur. Rayons horde et solo x1.5 une seule fois ; pas x1.5 par frame. CI/essai Samsung à confirmer.

## SOLO-020 — livraison CI + dépôt direct vérifiés
- Code source `dd6a4c75066fd51a359c00bbd952a32674eaaa25` ; Android/C++/SDL2 CI `38973185112` **SUCCESS**.
- Export GitHub `38973239202` **SUCCESS**, fichiers réels dans `temp/solo-cruise-020/` : APK diagnostic 98 449 216 octets, SHA-256 `ad683345d31f6857cc90589557905931a4076634ea69781bc636e8887ddfffb0` ; AAB non signé 95 742 279 octets, SHA-256 `16dfdbc3948a8d2689aa43d0320ee1a1f1f09b82e3e5b388fa59749f4bfccbfa`. Sommes vérifiées par workflow. Pas de main ni de Release.
- **Reste :** validation tactile réelle Samsung (défilement proche centre, x1.5 zones, dégâts/barrières et joystick).

## SOLO-021 — diagnostic vérifiable netteté des couloirs
La texture minérale du 1er plan est une découpe 200×58 px, étalée sur 405×180 px sur écran de référence 540 (cellule=90), avec interpolation linéaire, et biseau progressif en 8 couches : forte perte de définition contrairement au fond. Hypothèse testée : texel density quasi native ~180×58,5 px au zoom 1, sampler nearest sur parois uniquement ; réduction du biseau. La texture PNG originale est inchangée. Risques visuels : répétition plus rapide de motifs, aliasing/pixelisation au zoom maximum ; ne pas prétendre une amélioration subjective sans tester le Samsung. Avant correction `cf6cc6e`, test isolé `f896211`; revenir par `git revert f896211164481cede61d0073729f6eb3362be177` sur la seule branche feature sans réécrire les historiques.


### SOLO-021 — résultat définitif (17/10/2026)
- **Source exécutée** : `04da08db80f543a081a7a3614f85c1b2fd28e7c1`, code de rendu `f896211164481cede61d0073729f6eb3362be177`, état de référence précédant l'essai `cf6cc6e479ce854c3e64ddb00f6b9566d9cf52a9`.
- **CI verte** : GitHub Actions `39019113903` (logiciel SOLO, SDL2, Android), toutes les tâches requises SUCCESS, publication Release SKIPPED.
- **Export GitHub direct vert** : run `39019145459`, sous `temp/solo-wall-crisp-021/`.
- **APK de diagnostic** : `SpaceFortressVs-1.4.2-SOLO-WALL-CRISP-021-04da08d.apk`, 98 449 218 octets, SHA-256 `5a58885b9a76e52e4c88ceeffd044c5b0862ef421b0a4b4a8848b0dbf38cea43`.
- **AAB non signé** : `SpaceFortressVs-1.4.2-SOLO-WALL-CRISP-021-04da08d-unsigned.aab`, 95 742 237 octets, SHA-256 `3ae61f0554464c29cec1a0640ad7cc922ba7c047a18e6fae3a04412b4085d4fc`.
- **Comparaison à faire sur Samsung** : SOLO-020 avant ↔ SOLO-021 après, à zoom 1 et 2 et en mouvement ; noter netteté, effet pixel, raccord et répétition du rocher. Aucun résultat de qualité visuelle vécu ne peut être attribué au téléphone sans test réel.
- **Retour arrière sans perte de travail** : `git revert 04da08db80f543a081a7a3614f85c1b2fd28e7c1` puis `git revert f896211164481cede61d0073729f6eb3362be177` sur feature/solo-coop-core-v1, suivi de tests. Garder cette trace dans ordres-de-mission. Pas de `main` ni Release.


## SOLO-022 — diagnostic de raccord 1D→2D
SOLO-020 avait un seuil de défilement 0 exactement à 2 diamètres du vaisseau, donc déplacer simplement ce seuil à 1 diamètre aurait modifié brutalement la courbe sans créer la zone supplémentaire demandée. SOLO-022 conserve le repère 2D mais lui donne **une vitesse positive 0,025**, insère entre 1D et 2D une progression monotone smoothstep 0→0,025, puis redessine 2D→65 % depuis 0,025 vers 0,1. Les maxima externes 2 et 6 restent. Pas de compensation de téléportation ni changement d'inertie. Tests sur 540×960 et les huit angles et limite par la translation réelle. Ancien état `9e064b5`, patch `6c79f88`, validation en cours `39040122861`.
