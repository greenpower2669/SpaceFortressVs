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
