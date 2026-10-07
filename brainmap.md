# brainmap.md — SpaceFortressVs

## Reprise rapide
- Canon public : `v1.4.2`.
- `main` : `414b23cd2e787325b723fe7b0b6bd84fac02494b`.
- Release/tag : `5c3bedbd592f7bf4c50b830d26d6a0da49048813`.
- `src/main.cpp` lecture seule, blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.

## Hall global
- Validé téléphone : `SYNC OK`, `GLOBAL 1 + LOCAL 0`.
- Secret build seulement via `SPACEFORTRESS_HOF_API_KEY`.
- Aucun changement Hall dans v1.4.2.

## Cinétique v1.4.2 — flux livré
Énergie :
- `energyFraction = 1 - nrj/50` ;
- `effectiveEnergy = pow(energyFraction,1.20)` ;
- dissipation = maximum × effectiveEnergy × surgePower ;
- à 10 % d’énergie ≈6,3 % d’efficacité nominale ; surcharge ×2 reste faible ; charged 2 s => puissance 0.

Vague :
- pleine réserve : ~0,27 s ;
- faible réserve : jusqu’à ~0,50 s visuellement ;
- couleur équipe→orange→rouge, alpha croissant ;
- <10 % : rouge + clignotement ~4,5 Hz ;
- physique d’impact non ralentie ;
- énergie vraie capturée au déclenchement de la vague.

CLASSIQUE DUEL local :
- premier doigt = mouvement historique ;
- second doigt même moitié = moteur partagé `sfKineticSurgePress/Release(owner)` ;
- <0,30 s = tir normal ; 0,30–2 s = irisé ×2 ; >=2 s = ready/vulnérable ; release = EMP + purge 3× ;
- owners 0/1 indépendants ;
- sortie du mode = annulation ciblée des charges duel ;
- `src/main.cpp` inchangé.

## Preuves v1.4.2
- RED : `37517885303`.
- GREEN PR final : `37528710590`.
- GREEN main : `37529550402`.
- GREEN publication : `37530458354`, `publish-release` SUCCESS.
- APK SHA-256 : `b5604ba8f103a351e62beb0751893d6ffdcc0d9549d6c75cb2545ea061ea8282`.
- AAB SHA-256 : `4a996b60cc5bca9d874a7d2897c3ec83d095e3f5d9a989a192df3979647d74ea`.

## Protections
- Champ normal max 2,0 diamètres ; blast armé 3,0.
- Hall, Danger 9, campagne 200, poussières et progression hors périmètre.
- Ne jamais modifier `src/main.cpp`.


## Lot v1.4.3 candidat — en travail
- CLASSIQUE historique : passive recharge confirmée dans `sprite::update(): nrj*=0.997`.
- COOP : remplacer le faux équivalent 60 Hz trop rapide par une recharge lente frame-independent.
- Tirs : helper commun d'enveloppe de dispersion selon `nrj` + échantillon aléatoire symétrique ; CLASSIQUE `sfFireMain` et COOP `sfCoopFire`.
- Boss : champ fixe 55 % autour du boss ; astéroïde entrant -> calcul masse/vitesse relative -> 55 % dissipé, résiduel sur santé boss -> destruction cinétique/poussière existante.
- Pas de Danger sur le cinétique boss.


## Implémentation active
- `tactical_runtime.hpp` : `sfMainShotSpreadEnvelope/Radians/RandomUnit` + spread humain classique.
- `campaign_runtime.hpp` : spread COOP aléatoire partagé, recharge passive demi-vie 21 s, champ boss fixe 55 %, collision astéroïde→boss et anneau visuel.
- TDD RED run 340 (`37539108268`) confirmé avant code.


## CI 342 GREEN
- Code candidat : `5c66e892...`.
- Run `37540141916` entièrement GREEN ; artifact `11447494930`.
- Phone à vérifier : spread gauche/droite à faible énergie en CLASSIQUE + COOP, recharge COOP lente, anneau boss + dégâts résiduels d'astéroïdes.
- PR #7 reste draft ; pas de merge/release.


## Avenant 07/10 — architecture cible
- Boss field : 55 %, centre-out transparent, astéroïde survivant amorti + cooldown anti-multi-hit.
- Recharge COOP : helper Danger 0..8 ; multiplicateur cadence v1.4.2 ×4 -> ×2, soit exposants 240 -> 120 par seconde.
- Astéroïdes COOP : timer de spawn continu avec plafond de population.
- Surge mining partagé CLASSIQUE/COOP : actif seulement 0,30 <= hold < 2,00 s ; cible proche unique ; shrink continu < cadence tirs ; poussière blanche attirée.
- Run 342 obsolète pour validation téléphone.


## Run 343 RED -> code avenant
- Run `37559681669` RED attendu sur les nouveaux contrats.
- Recharge: half-life Danger 0..8 = 4,0 -> 10,5 s.
- Boss field: 55 %, ne détruit pas ; residual HP + vitesse relative amortie/rebondie ; cooldown .42 s.
- Boss wave: centre -> rayon 1,08× boss, durée .46 s, alpha max 72 + limite permanente alpha 28.
- Spawn COOP: batch historique `setasts(1)` toutes les 3,5 s, plafond 24.
- Surge mining partagé: 1,25 shot-eq/s + aspiration blanche 7/s, seulement 0,30 <= hold < 2,00.

- CI 345 : échec de compilation uniquement sur noms provisoires/runner ; alignement des noms canoniques avant nouvelle CI.

- CI 346 : le nouveau gameplay passe jusqu'au test historique de précision IA ; seul nearest<16 est obsolète avec le spread demandé. Centre prédictif conservé, tolérance bornée <90.


## CI 347 GREEN — candidat téléphone actualisé
- HEAD `602a725eb785ffdac393ebe1e55af3d2cf50376c` ; run `37561150366` SUCCESS.
- Boss : champ FIXE 55 % < joueurs, non destructif, amortissement/déviation + dégâts résiduels boss.
- Recharge : passive historique CLASSIQUE confirmée par Fab et visible en jeu ; COOP = accélération Danger ×4 -> ×2 vs v1.4.2, jamais suppression de la recharge passive.
- Artifact release-files id `11456527917`; téléphone à valider avant merge/release.


## Lot visual v1.4.3 — branche dédiée
- Classic assets → COOP : missile + FX, orbes redimensionnées, explosion nette si bouclier faible.
- HUD : ENERGIE violet clair au-dessus, PV au centre, CINETIQUE jaune/orange au-dessous ; miroir joueur haut.
- Boss : 55 % inchangé ; réserve visuelle 0..1, regen difficulté, anneaux centre→extérieur, alpha ↑ quand réserve ↓.
- Atlas : demi-texel anti-bleeding, pas de crop pourcentage.

- Implémenté : classic missile/orbs/explosion en COOP ; FX missile sans poussière gameplay ; HUD violet/PV/orange ; boss reserve visuelle + regen difficulté ; anneaux centre→extérieur ; UV demi-texel.


## CI 350 GREEN
- Code `1aeb6758...`, run `37608419345` SUCCESS complet.
- APK `d136fcf0...`, AAB `f84b8d5d...`; téléphone à valider.
- Broutage : demi-texel est désormais candidat test ; ne pas aller plus loin sans preuve visuelle Fab.


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


## CI355 candidat téléphone
- Code `6826463d...` GREEN complet.
- Boss HUD compact droit semi-transparent ; regen Boss Danger MOU 0.02/s -> APOCALYPSE 0.18/s (×9).
- Artifact `11489666990`; téléphone à valider avant tout merge/release.


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


## CI358 candidat téléphone
- Code `65dbfa5d...` GREEN complet : BOSS + reflet interne sur les 3 barres compactes.
- APK `239c14f1...`, AAB `44f4cb83...`; gameplay inchangé.


## BOSS-HUD-DUAL-ROTATED — 2026-10-08
- Retour téléphone Fab sur CI358 : le bloc boss top-right est lisible à l'endroit, donc faux pour le joueur du haut.
- Canon demandé : définir le bloc normal en BAS-GAUCHE pour le joueur du bas ; créer le même objet en HAUT-DROITE par transformation 180° complète.
- La transformation 180° comprend : rectangles des 3 barres, sens de remplissage, reflet/gloss, position du label et glyphes `BOSS`.
- Le bloc haut n'est pas une variante bricolée : il doit être le miroir géométrique exact du bloc bas via `sfMirrorRect180`, comme le HUD pilote haut.
- Le HUD reste compact et semi-transparent. Aucune modification de regen Boss Danger, champ 55 %, gameplay, pilotes, FX ou atlas.
- Nouvelle branche `fix/boss-hud-dual-rotated-v143` depuis `4f5c05bec735b9ab2be34e0f1a2ec1da896f0436`. `src/main.cpp` protégé ; aucun merge/release sans Fab.
