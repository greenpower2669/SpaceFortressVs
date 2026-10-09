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


## CI 346 — ancienne précision IA incompatible avec le spread restauré
- Workflow 37560831848 atteint les régressions gameplay et valide le nouveau champ boss non destructif.
- Seul arrêt : testVelocityGhosts exigeait encore nearest<16, c'est-à-dire une précision quasi parfaite, alors que Fab demande désormais une dispersion initiale aléatoire aussi sur les tirs ordinaires du classique.
- La prédiction Orion reste le centre de visée et le sens de l'anticipation reste testé ; tolérance de l'impact portée à <90 px pour accepter le spread sans autoriser un tir incohérent.
- Aucun changement gameplay dans ce correctif.


## CI 347 GREEN — correction Fab confirmée
- HEAD gameplay/test `602a725eb785ffdac393ebe1e55af3d2cf50376c` validé par workflow `37561150366` / run 347 : SUCCESS complet.
- Canon confirmé par Fab : le champ cinétique boss est FIXE à 55 %, donc inférieur au champ normal des joueurs (et très inférieur à leur surcharge ×2) ; il amortit/dévie l'astéroïde sans le détruire.
- Correction d'observation : la recharge passive d'énergie existe historiquement déjà en CLASSIQUE (`nrj*=0.997`) et Fab l'observe en jeu ; le lot ne doit jamais la supprimer. En COOP, elle est accélérée selon Danger, de ×4 du rythme v1.4.2 en MOU DU GENOU vers ×2 en APOCALYPSE.
- Artifact téléphone : `SpaceFortressVs-1.4.2-release-files` id `11456527917`, digest ZIP `sha256:1e9ef7a5276c74a9db3064a83f00c17c614ff8cddf060245336ab2f3479d9da5`.
- Aucun merge main ni release ; prochaine étape : validation téléphone Fab.


## Mission active — classic FX / kinetic HUD / anti-bleeding
- Branche `feature/classic-fx-kinetic-hud-v143`.
- RED d'abord : tests du HUD 3 barres, réserve visuelle boss, difficulté de régénération, visibilité inverse de réserve et demi-texel atlas.
- COOP doit réutiliser missile/orbes/explosion classiques sans modifier `src/main.cpp`.
- Boss reste gameplay fixe 55 % ; nouvelle réserve = stress visuel/régénération seulement.
- Broutage : correction bornée UV demi-texel ; si le téléphone montre encore le défaut, stopper avant retouche asset plus large et rediscuter avec Fab.


## Implémentation classic FX / kinetic HUD / anti-bleeding
- COOP charge maintenant les assets historiques `missilebb.png`, `explobb.png`, `orberr.png`, `orbebb.png`.
- Tirs joueurs ordinaires COOP utilisent les orbes classiques redimensionnées ; missile guidé garde `missilebb.png` avec échappement visuel pur (aucune poussière rouge gameplay créée).
- Explosions COOP suivent la distinction classique fumée/explosion ; près d'un pilote sous 35 % de bouclier, l'explosion nette `explobb.png` est privilégiée.
- Boss 55 % gameplay inchangé. Ajout d'une réserve de stress visuelle 0..1 : impacts cinétiques la baissent, auto-régénération campagne .18/s -> .07/s de difficulté 1 à 4 ; alpha du champ augmente quand la réserve baisse.
- Champ boss : limite très discrète + 3 anneaux continus centre→extérieur, toujours semi-transparents.
- HUD COOP : ENERGIE violet clair / PV / CINETIQUE jaune-orange. CINETIQUE affiche l'efficacité réelle `pow(energyFraction,1.20)` ; aucune nouvelle ressource gameplay joueur.
- Broutage boss : inset UV 0,5 % remplacé par demi-texel réel. Aucun PNG retouché.


## CI 350 GREEN — CLASSIC-FX-KINETIC-HUD-V143
- TDD RED : commit `78b0ea35018a095f0ced6228fdf04c50f312caaf`, workflow `37608129628` / run 349, échec attendu dans les régressions sur les nouveaux symboles (HUD cinétique, réserve boss, UV demi-texel, textures classiques).
- Implémentation : `1aeb6758dd5ee8bbcf52e00f424361d8262f0c6d`.
- GREEN : workflow `37608419345` / run 350, régressions + Hall + secret hygiene + APK + AAB + packaging SUCCESS.
- Artifact : `SpaceFortressVs-1.4.2-release-files` id `11476546682`, digest ZIP `sha256:e5b62e26afbe8b29e3710dcabe6adada0c70989616e9a06dab74cef1065cf4f3`.
- APK SHA-256 : `d136fcf0690f2fc13ac1004022162aa1601544ab8474c7e72bb1cc3ec24d07e1`.
- AAB SHA-256 : `f84b8d5d5c964fd8e3f787b565c776b7e18c62ac88d1e238b2c7b01d5ecec4aa`.
- `src/main.cpp` revérifié au blob protégé `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- Aucun merge main, aucune Release. Validation téléphone Fab requise, notamment pour confirmer/disprover le diagnostic du broutage.


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


## CI 355 GREEN — BOSS-HUD-COMPACT-DANGER-REGEN
- Code candidat : `6826463de42ca7ad794f5ec775fb46d98009c836`.
- RED préalable : run 354 / `37634894304` sur `a8859eb2406f467acf9722d315a42804fef21ee9`, échec attendu sur les helpers de transparence HUD absents.
- GREEN : run 355 / `37635571916`, régressions + Hall + secret hygiene + APK + AAB + packaging SUCCESS.
- Artifact : `SpaceFortressVs-1.4.2-release-files` id `11489666990`, digest ZIP `sha256:60b0eb8a6a310896c930b66f60055f257af5355fae759c62bc638ec3fa3a6665`.
- APK SHA-256 : `3d843a48f1236e8fdb9d4e47b2f4f86044102086ae658a5f11a18f8f57c5f470`.
- AAB SHA-256 : `044f4faebbc0bf04d6e8fdc73e2af24a1ac3f7200e15dc17e0aabaf745f53907`.
- HUD boss : bloc unique latéral droit, 28 % largeur, 3 barres fines serrées, fond alpha 88/255, remplissage alpha 188/255.
- Régénération boss : Boss Danger 0..8 uniquement ; MOU DU GENOU = 0.02/s, APOCALYPSE = 0.18/s, interpolation croissante ; rapport exact ×9 entre extrêmes.
- Joueurs, missile/orbes/explosions, anti-bleeding demi-texel, champ boss 55 % et `src/main.cpp` inchangés.
- Aucun merge main ni Release ; prochaine étape : validation téléphone Fab.


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


## CI 358 GREEN — BOSS-HUD-MIRROR-LABEL
- Code candidat : `65dbfa5d2676b76ac438df462917c5bd01fa1cca`.
- TDD RED : run 357 / `37694965125` sur `5d30731a633929d5213a22cb5a316fe6553a0c6b`, échec attendu uniquement sur `sfBossHudLabelRect`, `sfBossHudMirrorAlpha`, `sfBossHudMirrorBand` absents.
- GREEN : run 358 / `37695067844`, régressions + Hall + secret hygiene + APK + AAB + packaging SUCCESS.
- Artifact : `SpaceFortressVs-1.4.2-release-files` id `11514673145`, digest ZIP `sha256:ab17b6946b26ba985bbe763f5194f2771558c7e8fabc9f1f062fc315301c6500`.
- APK SHA-256 : `239c14f17dc446b8c9cf969c5668c185a42253133f44ac60664429477895f406`.
- AAB SHA-256 : `44f4cb83e65b0eb08276eaf0f6327fa4b764fa190f82728bc0dda85437102611`.
- Résultat : label `BOSS` au-dessus du bloc compact + reflet/gloss interne haut/bas sur la partie remplie des 3 barres. Aucune duplication de bloc, aucune emprise écran supplémentaire.
- Regen Boss Danger, champ 55 %, joueurs, missile/orbes/explosions, anti-bleeding et gameplay inchangés.
- `src/main.cpp` revérifié au blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- Aucun merge main ni Release ; validation téléphone Fab requise.


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


## CI 361 GREEN — BOSS-HUD-DUAL-ROTATED
- Code candidat : `cd1b3f773b3e08e19fd36c025c274b12e661c844`.
- TDD RED : run 360 / `37697366838` sur `9d913f1fc770bb91cfe6e9d042861574407ade56`, échec attendu sur `sfBossHudValueRect` absent et la nouvelle signature orientée de `sfBossHudLabelRect`.
- GREEN : run 361 / `37697488143`, régressions + Hall + secret hygiene + APK + AAB + packaging SUCCESS.
- Artifact : `SpaceFortressVs-1.4.2-release-files` id `11515479617`, digest ZIP `sha256:418c63502daa50342de448abc7f442b71aa3d5502e5d9f60fdf47d1446414a27`.
- APK SHA-256 : `3d35a827ae4a176eaeef8dc06824e73927d9dcc779393fb9182975e5b12f097e`.
- AAB SHA-256 : `4f4cb8bc86a363b56394749bd281cb1edd67022ff7ef55974119f02ad04a56fa`.
- HUD boss : bloc canonique normal BAS-GAUCHE + copie exacte HAUT-DROITE par rotation 180° ; rectangles, sens de remplissage, gloss et glyphes BOSS sont tous retournés.
- Position bas choisie au-dessus du HUD pilote bleu pour éviter le chevauchement ; le haut est dérivé uniquement par `sfMirrorRect180`.
- Regen Boss Danger, champ 55 %, joueurs, missile/orbes/explosions, anti-bleeding et gameplay inchangés.
- `src/main.cpp` revérifié au blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- Aucun merge main ni Release ; validation téléphone Fab requise.


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


## CI 365 GREEN — SURGE-CONE-MINING
- Candidat téléphone : `40a3e91d8d4fff7e8ac44f94d2bda6d4010efc50`.
- CI363 / `37702340954` : RED de compilation, helper de cône dépendait de `vlong` dans un header compilé isolément ; corrigé par distance autonome `sqrt(dx*dx+dy*dy)`.
- CI364 / `37702932439` : RED de régression attendue après changement de règle ; ancien test plaçait l'astéroïde latéralement alors que l'aspiration est désormais strictement DEVANT. Test réaligné sur le cône et renforcé pour vérifier la poursuite du minage après READY.
- CI365 / `37734752753` : toute la chaîne SUCCESS (régressions, Hall, secret hygiene, debug APK, APK/AAB installables, packaging).
- Artifact `SpaceFortressVs-1.4.2-release-files` id `11531471894`, digest ZIP `sha256:e292ee7acd9616fa6806adc9f1ff884e18acdcc7de0e705edcde87cc2e21a5d4`.
- APK SHA-256 : `71a4fb848d421176a496078197e654cf7962e1f238e8d33e5e992a2492b3108e`.
- AAB SHA-256 : `9f4a33574989853451a8d2216df937dd0f0e24ad3c5db61663b2e01163c6768d`.
- Fonctionnel : aspiration/marée continue après 2 s tant que le doigt est maintenu ; portée minage 2.025 diamètres ; poussières 3.30 diamètres ; cône avant 70° ; halo/funnel orange semi-transparent avec fronts extérieur→intérieur ; transformation astéroïde via le vrai `sfMineAsteroid` / `partsforiw` classique ; son post-READY discret en boucle.
- Blast/purge final 3 diamètres inchangé ; HUD boss, Danger, boss kinetic et autres gameplay inchangés.
- `src/main.cpp` revérifié au blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- Aucun merge main ni Release ; validation téléphone Fab requise.


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


## CI 369 GREEN — CONE-MINAGE-SUPERCHARGE-SPEC
- Code/test candidat : `b8d3764d02b59efeb2ef09b20b137da7d5da3b6b`.
- CI368 / `37823187540` : seul échec = ancien garde-fou Python exigeant encore littéralement le délai historique `0.30f`; aucun échec C++ fonctionnel observé avant ce stop.
- Garde-fou réaligné sur le PDF : `0.20f`, supercharge rouge→vert, minage simultané et DPS boss.
- CI369 / `37823744853` : chaîne complète SUCCESS (assets/régressions, Hall protocol, debug APK, secret hygiene, APK/AAB installables, packaging).
- Artifact `SpaceFortressVs-1.4.2-release-files` id `11570413138`, digest ZIP `sha256:d429b88f7160a26a0817da19e6f5e07e11bf628944dd56414d8e70b02967b522`.
- APK SHA-256 : `31d17b8317ce831176ff7d81957369dae392239f954fc06fdfcf42f215a785b6`.
- AAB SHA-256 : `10db6126094e654ff346bbe84240b9de61639023645bd9256cce35ddbd0b450d`.
- Fonctionnel couvert : activation 0,2 s ; cercle supercharge rouge→orange→jaune→vert ; 100 % à 2 s et maintien ; bouclier cinétique normal suspendu pendant la supercharge ; minage simultané de toutes les cibles ; vitesse de fonte linéaire selon distance ; fonte indépendante du plafond de poussières ; boss DPS phaser ×3→×0,03 selon point touché de sa zone de collision ; astéroïdes non bloquants.
- Calibration Vprès/Vloin reste volontairement provisoire : `0.020 H/s` / `0.004 H/s`, à ajuster après essai téléphone Fab.
- `src/main.cpp` revérifié au blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- Aucun merge main ni Release ; validation téléphone Fab requise.


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


## CI377 GREEN — IA / EQUILIBRAGE CONE / CLASSIC VECTOR — 2026-10-08
- Branche : `fix/ai-balance-classic-vector-v143`.
- Candidat code+tests : `13025a34f5ad8739388787f00cfae243e1e8eb37`.
- PR : #15 (draft), aucun merge `main`, aucune Release.
- Cône : quatre axes liés au Boss Danger 1..9 par diviseur exact 1..9 : vitesse de minage, DPS offensif autorisé, ouverture, longueur. MOU DU GENOU conserve CI369 ; APOCALYPSE = /9. Supercharge/minage restent indépendants.
- IA CLASSIQUE : progression stratégique niveau 1→9, réaction/anticipation/esquive/risque/agressivité graduelles, anticipation haute difficulté basée uniquement sur vitesse observée, priorité récupération à <=10 % d'énergie, recherche d'astéroïdes et cône de minage, supercharge stratégique sans omniscience.
- IA COOP : survivant/sauveteur ; secours joueur neutralisé prioritaire ; à <=10 % énergie, ressources proches exploitables ; petit astéroïde lent reconnu comme ressource ; pas de fuite automatique. Cône IA COOP strictement minage : dégâts boss explicitement interdits et pas de purge offensive volontaire.
- Boss : attribution des dégâts par propriétaire, menace avec décroissance, agresseur principal réévalué, charge physique adaptative vers lui, esquive missile bornée/imparfaite et dépendante du niveau.
- CLASSIQUE tactile : les deux affectations historiques directes `Spritej1/2 x/y = doigt` sont remplacées dans le patch Android par destinations vectorielles ; vitesse bornée ; haut et bas couverts. Le déplacement tactile COOP n'est pas modifié.
- CI371→376 : rouges utilisés pour corriger gardes/test hérités (patch tactile, dépendance helper, cadence niveau 3, largeur cône désormais danger-scalée, isolation des assertions historiques ROCK N ROLL).
- CI377 / run `37845179868` : chaîne complète SUCCESS : assets/régressions, protocole Hall, debug APK, secret hygiene, APK/AAB installables, packaging.
- Artifact release-files id `11578789486`, digest ZIP `sha256:d5b8b820f5cda438360510858120d2ba598a7362dfc2104fcd5d163a0980e78e`.
- APK SHA-256 : `133e558b2fceb9ad2bfb33f84add6ff2087e91b8f85ddcb81343d7cbc1895e0f`.
- AAB SHA-256 : `99a3b202eca1dc0e139f8e74c8a9b4b1cb580bc1f04d0074be14f31dfa161948`.
- `src/main.cpp` revérifié : blob `835059a0ecfe0f74708068b3259cad5db1cdb579` inchangé.
- Prochaine étape : validation téléphone Fab, en particulier largeur/portée du cône aux niveaux 1/3/9, comportement IA <=10 %, sauvetage COOP, charge/esquive boss et absence totale de téléportation CLASSIQUE.


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
