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


## Implémentation BOSS-HUD-DUAL-ROTATED
- Le bloc canonique boss est maintenant placé BAS-GAUCHE (x=margin, y≈83,5 %), au-dessus du HUD pilote bleu pour éviter le chevauchement.
- Le bloc HAUT-DROITE est construit exclusivement par `sfMirrorRect180` à partir du même objet.
- Le remplissage des 3 jauges est inversé pour le haut via `sfBossHudValueRect(..., upper=true)`, ce qui rend le remplissage haut exactement miroir du bas.
- `BOSS` du haut est dessiné avec `sfCoopText180` à partir des coordonnées logiques du label bas : glyphes réellement retournés à 180°.
- Le gloss suit lui aussi la rotation : bande brillante haute en bas-gauche, bande brillante basse dans la copie haut-droite.
- Taille, transparence, regen Boss Danger, champ 55 %, joueurs, FX, atlas et gameplay inchangés.


## CI361 candidat téléphone
- Code `cd1b3f77...` GREEN complet : boss HUD bas-gauche normal + haut-droite rotation 180° complète.
- APK `3d35a827...`, AAB `4f4cb8bc...`; gameplay inchangé.


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


## CI365 candidat téléphone — aspiration avant
- Code `40a3e91d...` GREEN complet.
- Cône avant orange semi-transparent, fronts convergents extérieur→intérieur, aspiration/minage maintenus après READY, portées ×1.5.
- APK `71a4fb84...`, AAB `9f4a3357...`; blast final et gameplay hors lot inchangés.


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


## CI369 candidat téléphone
- Canon PDF implémenté et GREEN complet sur `b8d3764d...`.
- 0,2 s ; cercle rouge→vert ; shield suspendu ; minage toutes cibles + distance ; boss ×3→×0,03 ; particules non bloquantes.
- APK `31d17b83...`, AAB `10db6126...`; Vprès/Vloin restent à calibrer sur téléphone.


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


## CI377 — candidat téléphone IA/balance
- GREEN complet sur `13025a34...` ; PR #15 draft.
- Cône ÷1..÷9 sur 4 axes ; IA CLASSIQUE 1→9 ; COOP sauveteur/minage-only ; boss agresseur+esquive ; tactile CLASSIQUE vectoriel haut/bas.
- APK `133e558b...`; AAB `99a3b202...`; `src/main.cpp` intact.


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


## 2026-10-10 — Carte SOLO transformation prototype -> labyrinthe
- **Ordre Fab** : décor/rendu réel, labyrinthes, exploration/backtracking, vaisseau d'origine et commandes d'origine. Seul le **vecteur de tir et de cône à 360°** change.
- Moteur carte : `src/solo_map_generator.hpp`, `src/solo_session.hpp` (relays 3/3, exploration mémorisée, poussière et malus).
- Contrôle partagé avec le jeu original : `src/shared_touch_motion.hpp` appelé par `src/tactical_runtime.hpp`, et par `src/solo_campaign_model.hpp` / `src/solo_touch_controls.hpp`.
- Visée/tirs : `src/solo_vector_aim.hpp`, `src/solo_kinetic_control.hpp`, `src/solo_combat.hpp`, `src/solo_prototype.cpp` (tap vs appui 2s, orientation 360).
- Graphismes historiques : `src/solo_renderer.hpp` via SDL_image (forteresses sb/so, champ spatial fond4hlow, ro1, tirj1, pousb). Rendu neige/glace/lave enrichi, mini-carte brouillard d'exploration.
- Tests `tests/solo_maze_exploration_regressions.cpp` + régressions antérieures, CI `.github/workflows/android-build.yml`. Aucun merge main ni Release, vérifier appareil Samsung.


## SOLO — réglage caméra/zoom 2026-10-11
- `src/solo_viewport.hpp` : zoom décor constant `SOLO_MAP_ZOOM=3`, fenêtre panoramique à seuils des tiers horizontal/vertical, rendu cartographique et coordonnées écran ↔ monde, rayon pixel vaisseau conservé.
- `src/solo_session.hpp` : cache de caméra (`viewLeft/viewTop/viewReady/viewWidth/viewHeight`) par session ; vitesse de cible ajustée en nombre de cases visibles, sans changer `tactical_runtime.hpp`.
- `src/solo_touch_controls.hpp` : pilotage en glisser ancré `shipWorldAtDown + (fingerNow-fingerDown)/cell` indépendant de défilement ; deuxième doigt de visée indépendant.
- `src/solo_prototype.cpp` : mise à jour caméra avant événements et après simulation ; remise à zéro par session ; vector de tir 360 maintenu.
- `tests/solo_viewport_regressions.cpp` / `tests/solo_touch_controls_regressions.cpp` : assertions zoom x3, vaisseau fixe, seuils X/Y, bordure monde, vitesse pixels/s et drag stable pendant pan.


## 2026-10-11 — SOLO caméra rectangles, progression au sein de deux zones
- Carte des périmètres : centre 1/3–2/3 fixe ; rectangle 1 bord 1/3→1/6, gain vitesse 0→0,55; rectangle 2 bord 1/6→6 %, gain 0,55→1; dernière marge 6 % à vitesse pleine pour préserver le sprite fixe. Future alternative circulaire seulement si explicitement validée.
- `src/solo_viewport.hpp`: constantes `SOLO_DEADZONE_THIRD`, `SOLO_RECT_ZONE2_START`, `SOLO_RECT_MAX_SPEED_MARGIN`; calcul `soloRectangularCameraGain`, `soloSmoothstep`, `soloUpdateCamera` avec dt et amortissement.
- `src/solo_session.hpp`: vitesses `viewVelocityX/viewVelocityY` présentation uniquement. `src/solo_prototype.cpp`: caméra initialisée sans dt puis évolue une fois par frame après physique.
- `tests/solo_viewport_regressions.cpp`: continuité de la courbe aux deux frontières, croissance dans les deux zones, x/y indépendants, arrêt, retour, vitesse max sans masquer pilote, taille sprite inchangée, rétrocompatibilité zoom ×3.
- GitHub Actions run #38121160188, commit `cace6129`, 3 jobs verts; APK DIAG uniquement. Pas de changement de vaisseau historique ni merge main.


## 2026-10-11 — CAMERA-SHIP-ZONE-001 (source de vérité)
- **Caméra** `src/solo_viewport.hpp` : `soloRectangularCameraGain` conserve deux bandes de progressivité autour du centre; `soloShipCameraTarget` utilise la POSITION ÉCRAN DU VAISSEAU et SA VITESSE SIGNÉE (sortie du centre). Aucun paramètre fingerZone ne peut influencer le scroll. Zéro instantané dans le rectangle central, y compris après relâchement du doigt.
- **Croisière** `src/solo_touch_controls.hpp` : `cruiseX/Y`, `cruiseThrottle`, `cruiseSet` sont latched après finger-up ; la position du doigt est une donnée de pilotage, jamais un curseur de caméra.
- **Moteur/collision** `src/solo_session.hpp` : sweep collision par axes indépendants, blocage de l'axe impacté jusqu'à fin de frame pour ne pas téléporter à travers roc; glissement possible sur axe libre. `src/solo_prototype.cpp` met à jour caméra après step de pilotage.
- **Régressions** : `tests/solo_viewport_regressions.cpp` (centre stop immédiat vs zone active), `tests/solo_cruise_gesture_regressions.cpp` (toucher-glisser-relâcher, vitesse maintenue) et `tests/solo_ice_regressions.cpp` (rocher haute vélocité et wall sliding).


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


## SOLO — reprise rapide du lot 006, parois réellement orientées
- Code : `src/solo_scenery.hpp` (autotiling 4 voisins, 16 masques, rotations et hash stables) et `src/solo_renderer.hpp` (découpe des vraies planches en textures individuelles transparentes, rendu par masque, animations discrètes sans oscillation de variante).
- Tests : `tests/solo_atlas_regressions.cpp`. Sources originalement issues de `main`, à `assets/pict/solo/*.png` sur branche de travail. Ni physique, ni arme, ni camera circulaire modifiées par cette mission. CI Android run `38154338078` lancé sur `7012a3b5`; retester sur téléphone.


## Reprise — ASTRA-COOP-SOLO-ARCH-001 (11 octobre 2026)

- Lire d'abord [la mission Fab](docs/audits/2026-10-11-mission-astra-coop-solo.md), puis [l'audit et les lots proposés](docs/audits/2026-10-11-coop-solo-architecture.md). Branche documentaire `audit/coop-solo-foundation-20261011` ; référence applicative `1d8a48e2` / code 7012a3b5.
- Entrée réelle : `sdl_entry.cpp` → SDL_main historique OU runSoloPrototype. CMake Android → prepare-legacy-source.cmake + patch-classic-danger.cmake → main_android_compat.cpp + solo_prototype.cpp liés dans libmain.so ; activité SOLO en processus :solo.
- Socle historique : main.cpp → t.hpp/FabLib + th2.h → tactical_runtime / legacy_field_primitives / legacy_field_runtime / campaign_runtime. `sfTacticsBeginFrame` fournit les métriques ; `sfCoopTick` / `sfCoopProjectiles` simulent ; `sfCoopDrawArena` rend.
- Monde existant : solo_map_generator + solo_campaign_model ; navigation : solo_viewport + solo_navigation ; progression : solo_campaign_controller + solo_progression + solo_campaign_persistence.
- Parois : solo_scenery sélectionne/cadre ; solo_renderer extrait/redimensionne/mire ; tests/solo_atlas_regressions contrôle la géométrie attendue, pas les jonctions des PNG réels.
- Cible proposée : un état physique commun, vue séparée, métrique de combat stable, régions de rencontres monde, contrôleurs SOLO/COOP distincts. Aucun de ces nouveaux modules n'est livré par l'audit.
- Prochaine action : validation de la fondation par Fab, puis détail du premier lot de parité. Ne pas reprendre automatiquement les adaptations indépendantes de solo_combat.


## SOLO-COOP-SOCLE-001 — nouvelle carte de reprise (11/10/26)
- Reprise : branche `feature/solo-coop-core-v1`, départ audit documentaire `f4526cfa`. `src/main.cpp` protégé ; legacy FabLib dans `t.hpp` et une seule unité `th2.h`.
- Socle déjà codé : `coop_shared_projectile.hpp` (vraie fonction missile utilisée dans COOP) ; `coop_pilot_roster.hpp` (participation), `SfCoopState::bossPresent` / `soloDiagnostic` ; `coop_solo_arena_runtime.hpp` (test COOP réel pilote unique, boss toggle, son/FX), `SfKineticAimAxes` et cône historique orientable.
- Android DEBUG seul : `SpaceFortressCoopSoloActivity` en processus `:coopsolo`, troisième bouton du hub, argument `--spacefortress-coop-solo-arena`. Original SOLO et CLASSIC inchangés.
- Témoin/source : `docs/audits/2026-10-11-coop-solo-architecture.md`; simulations physiques COOP via `sfCoopTick/sfCoopDrawArena`, collecte + minage `sfLegacyFieldFrame`; tests nouveaux `coop_shared_projectile_regressions.cpp`, `coop_pilot_roster_regressions.cpp`, `coop_solo_cone_axis_regressions.cpp`, `coop_solo_native_smoke.cpp`, `tests/restoration_regressions.hpp`.
- Dernier code `172df0c0`, CI `38163698538` en attente de validation. Pas encore carte/caméra/mise en contexte de chaque entité. Pas de merge/main/release.


## 11/10 — Panne écran CLASSIQUE au-dessus de l'arène SOLO/COOP
- Le jeu tournait réellement avec son : le macro final `SDL_RenderPresent` du routeur d'accueil historique capturait le rendu du lanceur `:coopsolo`. Correction de portée minimale commit `b9072d24` : sauvegarder/définir/restaurer la macro dans `src/th2.h` pour exécuter un vrai `SDL_RenderPresent` exclusivement dans `coop_solo_arena_runtime.hpp`.
- Tests de préprocesseur dans l'en-tête et `tests/coop_solo_native_smoke.cpp` ; CI `38167735625` en attente ; besoin d'une APK **nouvelle**, puis essai visuel Samsung. La version `temp/SpaceFortressVs-1.4.2.apk` est pour l'instant l'ancienne (`ca0322e`) et a le défaut.


## SOLO-COOP-PRESENT-002 — livraison contrôlée après CI (11/10/2026)
- Correction code `b9072d24fd0b7452a88f8ec4cccb4b70e9cf51ae` : vrai appel `SDL_RenderPresent` exclusif à l'arène, routeur CLASSIQUE restauré pour modes historiques.
- Workflow Android `38167735625` entièrement VERT (SOLO, SDL2, regressions C++ historiques, compilation APK/AAB), aucune Release.
- Workflow extraction `38168138218` VERT : nouveaux binaires du commit corrigé dans `temp/` sur `feature/solo-coop-core-v1`, remplacement confirmé par manifeste.
- `temp/SpaceFortressVs-1.4.2.apk` : 94 119 741 octets, SHA256 `806da4277b599c7d1b19fb3c8a4ff45531bffe503475c8bb8d3b99f08d99546a`, package séparé `com.greenpower2669.spacefortressvs.diagnostic`.
- `temp/SpaceFortressVs-1.4.2-unsigned.aab` : 91 464 418 octets, SHA256 `d52c91ffb664165f0d9fd8f8d3aba1f3797a06384a6fd87835d5095c9840d10b`.
- **Attendre validation sur Samsung** : vraie arène COOP visible sans menu de sélection CLASSIQUE, avec sons, vaisseau, tir/cône et bouton boss. Pas de merge main ni publication.


## 11/10/2026 — Prochaine reprise : son cinétique et phase 2
- Branche : `feature/solo-coop-core-v1`. Fix diagnostic sonore `74206425` : déclenchement WAV charge au vrai seuil 0.20s, READY 2s par `sfKineticAudioUpdate`, fond discret post READY, stop/purge au lever et reset Android. Contrôle `coop_solo_charge_audio.hpp` et test correspondant.
- Interface monde `coop_solo_world_bridge.hpp` : depuis vraie carte `solo_map_generator`, projection monde/écran, caméra concentrique réutilisée, contrôle collision/swept glissement coque et pan fondé sur delta autorisé du pilote. Tests `coop_solo_world_bridge_regressions.cpp`. Unités combat indépendantes de l'étendue carte.
- **NE PAS annoncer phase2 jouable** ; bridge reste isolé du rendu/tick COOP de production. Ensuite choix d'intégration de la caméra et du rendu historique avec tests Android, sans deuxième moteur.


## 11/10/2026 — Validation CI et remise APK SON-003 / WORLD-004
- Build/test du code exact `9927f0148486eefb5f49a201b9fd0602151c1905` : GitHub Actions `38169721589` VERT, jobs `solo-core-regressions`, `solo-sdl2-ui`, `build-android` tous success ; `publish-release` skipped. 
- Décompression validée `38170223921` VERTE : `temp/SpaceFortressVs-1.4.2.apk` dans `feature/solo-coop-core-v1`, 94 119 734 octets, SHA256 `47c575b49877dc1af8bceaa966b8201978612edd794a9b7e17fe987726bfd33c`, package Android diagnostic séparé. AAB non signé SHA256 `635fbc4dfac186a27f51155e34123e90218491af80efb0cf8ae02bdf74279f92`.
- Le son de charge 0,20s / READY 2,0s est implémenté mais non encore écouté sur Samsung par Fab. Phase2 pont monde/collision/caméra testé isolément, PAS encore disponible dans l'arène interactive. Pas de merge `main`, aucun changement `src/main.cpp`, aucune Release.


## 11/10/26 — Entrée exécutable MONDE TEST + boucle COOP canonique
- Démarrage via HUB DEBUG → SOLO : VRAI MOTEUR COOP → bouton MONDE TEST ; bouton ARENE revient à l'arène canonique. Aussi touches clavier M (bascule)/B (boss uniquement arène).
- `coop_solo_arena_runtime.hpp`: session map créée à la bascule, pilotage world target par finger immobile mis à jour dans chaque substep 90Hz; appel natif `sfCoopTick`; pan après mouvement réel; fenêtre et HUD conservés.
- `campaign_runtime.hpp`: callback `SfCoopState::soloWorldResolve` uniquement pour COOP diagnostic propriétaire 1 et drapeau `soloWorldMode`. Aucun second intégrateur. Tirs extents monde, spawn astéroïdes historiques centrés autour du joueur, render projection via viewport et old HUD supprimé seulement MONDE.
- `coop_solo_world_bridge.hpp`: `draw` parois contiguës visibles/contours + repères, `moveWithoutTunnelling` coque, `soloCirclePan` caméra, combatWidth fixé à l'arène ancienne, carte `generate` existante.
- `legacy_field_runtime.hpp`: limites optionnelles monde propagées pour expiration/collision des vrais astéroïdes; défaut 0 pour tous anciens modes. Témoin COOP local inchangé.
- Tests : `tests/coop_solo_world_bridge_regressions.cpp`, `tests/restoration_regressions.hpp::testActualCoopMovementInWorld`, `tests/coop_solo_native_smoke.cpp` 2 boot modes; CI `38171610964` à vérifier. Tests Samsung indispensables (artefacts/visuels/caméra/collisions) ; pas encore progression/relays/animations.


## 2026-10-11 — WORLD TEST livraison Android vérifiée
- Lancement `feature/solo-coop-core-v1` : monde opt-in « MONDE TEST » derrière entrée diagnostique « SOLO : VRAI MOTEUR COOP » ; rendu/collisions/inertie pilotés par le VRAI moteur COOP; arène BOSS TEST conservée. Correctifs `bee803ee`, `3b52fca7`, `d86c1bfe`, `235661b3` (SDL_unsetenv absent dans SDL2, correction test uniquement) et `82825ed5` (levée doigt = vitesse SOLO remise à zéro, caméra figée).
- GitHub CI `38171993509` : TOUT VERT, tests isolated SOLO, SDL2, tests historiques de vrai COOP/collision paroi et smoke native 2 modes, compilation Android ; publish-release SKIPPED. Run précédent `38171610964` rouge UNIQUEMENT à cause du test `SDL_unsetenv`, corrigé. Ne pas effacer ses logs sans accord Fab.
- Artefact définitif extrait dans `temp/` par workflow VERT `38172484944`. APK diagnostic `temp/SpaceFortressVs-1.4.2.apk` commit source `82825ed5e2889ac630fe2379183bc068f0105d01` SHA256 `635c936e0b4fc1335077c2da9d2f236b92e69d8446761703012aa4b65754d5d2` (94 136 117 octets), identité Android séparée ; AAB NON signé SHA256 `14c0df866eba3f704912b1241a2bbbb066311691e8a1d6ad13dca0b110b06c9c`.
- Limites non résolues : absence de collisions projectiles/parois, boss déclenché par progression, relais et sauvegardes ; habillage parois diagnostique seulement. **Validation Samsung de la caméra, déplacements et sons est encore à effectuer**, CI verte ne la remplace pas.
- Aucun merge `main`, Release ni modification `src/main.cpp`.


## SOLO COOP — retour Samsung 11/10/2026 : ship missing/touch drift
- PHOTO 23110.jpg : vrai monde labyrinthe/sky visible; anneau vert visible, pilote bleu indistinct/invisible, doigt décroché. Correction écran vs monde : `src/coop_solo_world_draw.hpp` projette vrais sprites de `sfCoop` sans viewport SDL géant; cargo/cône/anneau écran + piloter bleu réellement dessiné. `src/coop_solo_arena_runtime.hpp` appelle adapter plutôt que `sfCoopDrawArena` sous un viewport à coordonnées géantes.
- Correction lock doigt : `src/coop_solo_world_bridge.hpp::panIfShipActuallyMoved(..., optional fingerScreen)` attend que la VRAIE position écran du vaisseau rattrape la position doigt avant de consommer son mouvement par pan. Aucun changement aux vitesses physiques 3.10*W/s ni aux collisions. `coop_solo_arena_runtime.hpp` transmet moveScreenX/Y, log lag.
- Régressions `tests/coop_solo_world_bridge_regressions.cpp`, native smoke pixel réelle `SDL_RenderReadPixels`. CI `38174175100` en attente ; APK du dernier run VERT à exporter ensuite dans temp. Tester vrai vaisseau bleu et suivi du doigt sur Samsung avant d'annoncer validation.


## 2026-10-11 — Livraison vérifiée ANNEAU/VISIBILITÉ PILOTE ET CAMÉRA DOIGT
- Correctifs de code `09f8214c`, `999aedb8`, `63911245`, `e4be8b58`, `ad24535f`, `1088c265` (vaisseau bleu historique rendu dans viewport écran et projeté depuis monde, cône/ring/dust/orbes/missiles réels réutilisés, fallback lumineux si PNG absent, caméra attend le rattrapage du doigt avant de faire défiler le monde, pas de pan sur mur, vraie physique COOP préservée).
- CI exacte `38174175100` **TOUS LES JOBS VERTS**, inclut `SDL_RenderReadPixels` sur centre du vaisseau en mode monde, rendu viewport écran, test de rattrapage du doigt et smoke vrai moteur, compilation APK/AAB. Run `publish-release` SKIPPED.
- Export GitHub `38174728231` **VERT**, fichiers DÉCOMPRESSÉS dans `temp/` sur `feature/solo-coop-core-v1`. Manifeste testé commit `1088c2657642b603fb85b5cba5e7e839152f9890`; APK `temp/SpaceFortressVs-1.4.2.apk` SHA256 `a23c9d0cc5e1222b703504ea35bbd5e824b6119dcfddc30eb6a799fe1f85d76d`, taille 94 136 121 octets, package `com.greenpower2669.spacefortressvs.diagnostic`. AAB non signé SHA256 `55e40cf1000a67975a6afb0433f5747d0e0210e4aedc936204d9800944f8c2fe`.
- **À faire sur Samsung** : installer le DIAG, MONDE TEST, identifier le vrai vaisseau BLEU sous l'anneau vert de supercharge, vérifier l'adhérence au doigt immobile pendant défilement, test rapide doigt → rattrapage physique, blocage mur; signaler toute dérive sur le screenshot. Vert CI ≠ validation ergonomique téléphone.
- Jamais de merge `main`, jamais de Release, `src/main.cpp` inchangé.


## 11/10/2026 — Carte reprise CAMERA-HOLD-007
- Symptôme après correction vaisseau visible : arrêt de défilement du monde avec doigt fixe. L'impasse vient de la cible de déplacement COOP exactement sous le doigt : vitesse nulle après rattrapage et donc caméra inactive. Il faut continuer la propulsion radiale sous le doigt sans contourner le moteur physique.
- `src/coop_solo_world_bridge.hpp::heldFingerPilotTarget` convertit écran→MONDE puis ajoute cible avancée radiale seulement hors cercle central, amplitude `circleCameraSpeedPixelsPerSecond/30` (COOP vrai speed=distance*30, capped3.10*W). `src/coop_solo_arena_runtime.hpp` attribue cette cible avant `sfCoopTick`. Camera continue `panIfShipActuallyMoved` avec excess finger clamp : no fake movement.
- Tests `coop_solo_world_bridge_regressions.cpp` zones caméra et `restoration_regressions.hpp::testCoopHeldFingerContinuousWorldCruise` vrai pilote réel, doigt tenu, Rock stop. CI `38178296951` à vérifier. Conserver vaisseau BLUE rendu par `coop_solo_world_draw.hpp` sans viewport XXL.


## 12/10/2026 — CAMERA-HOLD-007 livrée (attente essai Samsung)
- Code source `908749e7` vert CI `38178801383` ; seul pilote COOP, `heldFingerPilotTarget` alimente vrai contrôle, laisse pan limité au mouvement réel / à l'écart au doigt. Tests cohérents centre + couronnes et maintien vrai COOP 2s/butée ROCK.
- CI rouge précédent était un TEST invalide hors bornes caméra, corrigé et tracé. Nouvelle APK décompressée dans `temp/` par `38179331501` verte ; empreinte 372b13765231cfe4b6fe591ea91e90a5dd55df375236ade05971ffcaff4d5730 ; NE PAS confondre avec précédente `1088c265` sans défilement.
- Reprise : retour Samsung, si OK enchaîner habillage parois/progression ; ne jamais sacrifier vaisseau visible ni contrôle COOP.


## 12/10/2026 — SOLO WORLD CLEANUP-008
- `src/campaign_runtime.hpp`: garde `soloWorldMode` sur routines tourelles/bonus et rendu; purge des états/projectiles hérités à l'entrée depuis `src/coop_solo_arena_runtime.hpp`. `tests/restoration_regressions.hpp::testSoloWorldExcludesTurretsAndBonuses` + appel depuis `tests/regressions.cpp`. Ne jamais toucher à la caméra validée ni aux combats historiques. CI à contrôler, APK du `temp/` antérieure inchangée tant que non verte.


### SOLO CLEANUP-008 — résultat vérifié
- CI complète verte #38182884348, extraction vérifiée #38182981381 ; source code 518f331, chemin nouveau `temp/solo-world-cleanup-008/` ; ancienne APK intacte. Après test Samsung: murs/projectiles, atlas roche, progression, boss, sauvegardes. Aucun merge/release.


## 12/10/2026 — SOLO METEOR-009 validé CI / APK diagnostic
- Seul WORLD `soloWorldMode` : bonus collectible et tourelles toujours interdits ; minuteur bonus => météorites historiques (`sfCoopSoloMeteorites`, 4 tous les 18..32 s après 12 s initiales) accessibles au minage/recharge réelle ; météores dirigés vers boss test lorsque activé, dommages par son champ cinétique canonique.
- Monde : `soloWorldCanOccupy` empêche les placements dans les parois ; bouton haut droit = BOSS TEST/OFF stationnaire (pas encore le boss final), atlas projeté écran. Autres modes CLASSIQUE/DUEL/COOP intacts ; ne pas toucher à la caméra/contrôle ni à main.cpp.
- Code 2650237 (fix Android `sa1` liste), CI verte #38190640721, export verte #38190657369, `temp/solo-world-meteor-009/SpaceFortressVs-1.4.2.apk` SHA 07bebf33..., AAB NON signé ; ancien diagnostic cleanup-008 conservé. Ensuite essai physique Samsung et progression SOLO.


## 12/10/2026 — SOLO-STABILISATION-010 : METEOR-009 SUSPENDU (retour Samsung)
- Symptôme déclaré par Fab : nouvelles météorites du MONDE TEST coincées dans murs ; annule toute mention « placement validé sur téléphone ». Source : spawner expérimental valide SEULEMENT la position `canOccupy`; mouvement réel `rock->ast` n'applique pas collision ROCK, et spawn de fond translaté est non filtré. Aucune preuve encore du mécanisme exact observé.
- Protection : `SF_COOP_SOLO_METEOR_WAVES_ENABLED=false`, fonction sans effet même pour BOSS TEST. Ressources/astéroïdes historiques, pilote, caméra, sons, bouclier, minage/armes et CLASSIQUE/COOP conservés. Test négatif vague + test séparation positions/trajectoire (pas une correction de trajectoire réelle).
- Priorités exclusives : stabiliser, dissocier validité naissance et continuité trajet, repro Samsung et tests stricts avant activation. Boss WORLD fixe reste TEST, jamais « boss historique terminé ». Aucune progression/ennemi/relai supplémentaire sans nouvel ordre. Branche `feature/solo-coop-core-v1`, commit initial d01f4d6 ; CI/Android à vérifier.


### 12/10/2026 — finalisation suspension météores
- Commit de test ciblé `6540adece`, CI #38193630900 verte ; export #38193705276 vert dans `temp/solo-world-stabilization-010/`, APK SHA b0cc1fad1543e4a66729a014bd6cb30f2222b9de7a6aa43c98c02dc9f4e36e64. Anciennes APK intactes ; validation physique Samsung ouverte. Aucun nouveau gameplay, aucune réactivation météores avant analyse naissance ET trajet.


## 12/10/2026 — SOLO-011 / protection tourelles diagnostic
- Retour Samsung : boss + météorites OK selon Fab; boss trop sombre mais reporté; tourelles encore visibles. Version réellement installée non vérifiable à distance : manifeste dernier ZIP/APK STABILISATION-010 = source 6540adece, nouveau HUD `SOLO-011` pour discriminer les prochaines captures.
- Audit causal source : ARENE diagnostic = `soloDiagnostic=true` ET `soloWorldMode=false`; avec boss, boucle COOP historique appelait sfCoopDefences/sfCoopBonus et dessinait sfDrawTacticalEffects. En MONDE, cette voie était déjà bloquée, donc objet vu dans MONDE à confirmer visuellement.
- Correctif branche `feature/solo-coop-core-v1`, commit `ad5cb08e`: interdire bonus/tourelles sur TOUT `soloDiagnostic` ; purge réelle entrée/bascules/boss, projectile kind5 aussi avant collisions, exclusion dessin et HUD. Aucun changement COOP historique ni CLASSIQUE/DUEL. Test deux lancements, boss ON/OFF; ne pas élargir gameplay. Météorites additionnelles toujours suspendues; boss test fixe, non historique. CI 38209069899 et export séparé 011 à vérifier.


### SOLO-011 / références de livraison
- CI `38209069899` verte source `ad5cb08e`; export `38209723734` vert dans `temp/solo-turrets-off-011/` (premier export refusé pour concurrence branche avec mémoires, résolu par rebase sans force-push). APK SHA `513693f6b2b7fd07b810562fe8fc0c73e336eb3c2ebab02db980fc20106f0e21`, 94 136 124 octets ; AAB non signé. Application diagnostic affichera `SOLO-011` (ancien APK installé non vérifiable). Toujours tester réellement boss OFF/ON + relance sur Samsung, aucune annonce d'intégration historique.


## SOLO BONUS CORE-012 — architecture en cours
Plan : docs/superpowers/plans/2026-10-12-solo-bonus-core-012.md. Nouveau solo_bonus_core.hpp (état pur) possédé par SfCoopState ; coop_solo_bonus_runtime.hpp (tir/status/loot réels) et coop_solo_bonus_draw.hpp (textures/HUD communs). WorldBridge fournit segment/placement connecté. Tests purs + tests runtime du vrai moteur par Sol. Les fonctions nouvelles sont réservées au SOLO ; conserver une seule position du pilote et les projectiles historiques.

### CORE-012 — intégration et revue, 12/10/2026
- Brique C++ pure + adaptateur du vrai SfCoopState + rendu commun arène/labyrinthe implémentés. Salves humaines x3, guidage historique, coût canonique une fois ; furtivité semi-transparente conserve collisions ; missiles3s sans coût ; statuts Prison2s/Glace4s/Lave4s et Météore ciblé. Ancien bonus/tourelles et vagues METEOR-009 toujours exclus du SOLO.
- Revue indépendante : seuils enregistrés au moment du dégât avant soin poussière ; boss SOLO mort non ressuscité par poussières ; taille de coque identique au pilote monde ; BFS accessible calculé une fois par paquet, emplacements distincts. Coûts bornés et logs Android `SOLO_BONUS_012` moyenne/max temps simulation+rendu pour validation Samsung.
- Précontrôles : core pur GREEN, source SDL compilée ; régressions complètes/visuel/Android encore en cours à cet instant. Tests ajoutés par Sol puis poursuivis par Astra après interruption de son exécution. Benchmark hôte seulement : 12 récompenses/carte54×160, 12 lots, moyenne2,270ms maximum6,612ms ; aucune promesse de FPS Samsung.
- `src/main.cpp` inchangé. Pas de merge main/Release. Identifiant HUD SOLO-012 et carte accueil SOLO BONUS012. APK/AAB à livrer uniquement depuis CI verte exacte dans temp/solo-bonus-core-012/.

### CORE-012 — contrôle local terminé, candidat CI
`scripts/test-regressions.sh` termine code0 : historique C++/SDL2, assets, moteurs, 200 ouvertures/rendus COOP, champs et poussières cinétiques, source Android générée et smoke natif arène/monde. Nouveaux tests purecore, tirsréels/statuts/collisions/loot/isolation passent. Dernier ajustement HUD SOLO (plus de double HUD COOP, textes agrandis) recompilé et testé avec lecture pixels SDL ; capture inspectée. Contrôle pont monde réexécuté après ajout des24placementsdistincts ; protection launcher diagnostic OK. Le boss monde reste FIXE de test, Prison/Glace y montrent des statuts, mouvement seulement démontré sur boss mobile de l’arène. CI Android et export exact restent à constater ; APK non annoncée avant cela.


## CORE-012 — livraison vérifiée 13/10/2026
src/solo_bonus_core.hpp = état pur ; coop_solo_bonus_runtime.hpp = moteur historique ; coop_solo_bonus_draw.hpp = sprites/FX/HUD commun ; coop_solo_world_bridge.hpp = trajectoires/collecte et placement accessible. Livraison : temp/solo-bonus-core-012/.

Source `95743f2386b220bb692112197c4ddee02d27ae2b` ; [CI Android/C++/SDL2 verte](https://github.com/greenpower2669/SpaceFortressVs/actions/runs/38299079636), publication Release SKIPPED.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-012-95743f2.apk` : 94168883 octets, SHA256 `c61d7e39ecb075d69db863e7cc964021bb9c3ed31ccf0c0597b01b1ffaa09248`.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-012-95743f2-unsigned.aab` : 91525121 octets, SHA256 `a4433fc2f09cae480ab407f1997139821602caa71c0c0f8a61941baceeed6289`.

## 13/10/2026 — SOLO BONUS-013
Correctif de CORE-012 sur `feature/solo-coop-core-v1` : bonus désormais éjectés visuellement depuis le boss avant collecte ; WORLD exige destination pilote accessible + segment boss→pickup sans mur. Toggle `TEST BONUS OFF/ON` avec 8 boutons + `TOUS X8`, fermeture sans pause/reset et boss requis. HUD joueur SOLO relevé au-dessus de la navigation Android via safe-bottom inset, HUD boss inchangé. Explosions d’astéroïdes seulement : alternance légère `explo1/explo2` + faible volume sur canal dédié. Tourelles et METEOR-009 restent OFF, `src/main.cpp` protégé. HEAD code avant mémoire `77a42531`; CI Android finale encore à confirmer.

## BONUS-013 — livraison vérifiée 13/10/2026
Livraison diagnostic: `temp/solo-bonus-013/`. Source et CI exactes ci-dessous; aucun merge main ni Release.

Source `488de2195e2aa2a1e40f2eec96abaeba19863fad`; CI Android/C++/SDL2 `38361581767` SUCCESS; publish-release SKIPPED.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-013-488de21.apk`: 94185274 octets, SHA256 `4a3511c380d04b21b46593806e01d991e9320c44691c529c6022e60aa4d1a18b`.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-013-488de21-unsigned.aab`: 91532662 octets, SHA256 `3fb742b7b3964620b5f0b8eb76dae0d0fc2b18f2c2e245d87809b08468a68d3a`.

## BONUS-014 — livraison vérifiée 13/10/2026
SOLO BONUS-014 : rendu offensif = `assets/pict/solo/SoloBonusAnimations.png` (4 lignes x 8 frames). Audio astéroïdes = banque 4 variantes bornées incluant 0,97x/1,03x. HUD diagnostic = `sfSoloDiagnosticHudY()`. TEST BONUS = 8 boutons + lot atomique TOUS X8. Livraison : `temp/solo-bonus-014/`.

Source `f9323f3cd239e9027d964e38db369a9217ef02cb`; CI Android/C++/SDL2 `38420099395` SUCCESS; publish-release SKIPPED.
- Atlas animé: `assets/pict/solo/SoloBonusAnimations.png`, 1536x768, 4x8 frames de 192 px, SHA256 `1a337adb15a6a3901b76da566dcd26a1d2d76c26893864a37a85e97b63950001`.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-014-f9323f3.apk`: 94989574 octets, SHA256 `0dfe83dbb1f2716e33442a2d2933356b1a6582180a85a9c2fed5a77b1ebe59ff`.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-014-f9323f3-unsigned.aab`: 92342299 octets, SHA256 `564b0c9427bd776ed93bcb32d3012d86d232f3354ffded5d85b020cfd7424965`.


## SOLO GAME-015 — intégration prévue, 14/10/2026
`game_mode.hpp` / `start_ui.hpp` / `remaster_ai_fix.hpp` : véritable choix SOLO et pont Android. `solo_launch_jni.cpp` + activité interne : lancement en processus privé, sans second JNI_OnLoad et sans inclure FabLib. `coop_solo_session_runtime.hpp` : hôte commun SDL/contrôles/audio, scènes et cycle de vie ; `coop_solo_arena_runtime.hpp` : entrée diagnostic conservée ; `coop_solo_diagnostic_ui.hpp` : commandes de test isolées. `campaign_runtime.hpp` et `coop_solo_bonus_runtime.hpp` : politique SOLO indépendante du diagnostic, rôle final/miniboss et événements victoire. `solo_level_flow.hpp`, `solo_miniboss.hpp`, `solo_lair_transition.hpp` : progression des scènes, acteurs courts sans récompenses, transition de rendu. Pont monde conserve la navigation validée et fournit les emplacements accessibles. Adaptateur de résultat SOLO sans ancienne simulation Session. Le moteur boss et les projectiles historiques restent uniques ; `src/main.cpp`, FabLib et ses symboles globaux inchangés.

GAME-015 / étape 1 : sélecteur SOLO, activité interne `:sologame`, hôte commun et droits diagnostic séparés implémentés localement. Test ciblé RED (injection hors diagnostic acceptée auparavant) puis GREEN ; launcher statique GREEN. Suite globale arrivée aux tests cinétiques finaux sans erreur visible, statut de processus perdu lors de la reprise de session : une exécution finale avec code retour enregistré reste requise. Android et Samsung non encore validés.

GAME-015 code : solo_miniboss.hpp possède les états légers FabLib ; coop_solo_miniboss_runtime.hpp réutilise émission/intégration/collision/énergie/FX historiques. solo_level_flow.hpp calcule le chemin avec rayon de coque, emplacements et phases ; coop_solo_encounter.hpp initialise le même boss pour BOSS TEST et fin de niveau. solo_lair_transition.hpp possède une texture bornée à 2048 px et maillage 16x24. solo_run_result.hpp relie résultat au Hall/progression existants sans simuler Session. Activité interne :sologame, JNI minuscule ; cinq modes dans le vrai sélecteur.

GAME-015 : `solo_level_flow.hpp::gameplayTime` partage précisément le temps entre transition et combat. `coop_solo_encounter.hpp` conserve le pilote et nettoie les acteurs/FX de scène avant le boss commun. `help_runtime.hpp` route les pages SOLO uniquement si le vrai sélecteur sélectionne SOLO ; pages historiques conservées.

GAME-015 / validation locale finale 15/10/2026 : `bash scripts/test-regressions.sh` terminé avec code 0 ; tests ciblés de furtivité/contact et changement de scène verts. Revue indépendante finale sans défaut critique/important restant. `src/main.cpp` conserve le blob 835059a0ecfe0f74708068b3259cad5db1cdb579. CI Android et export restent à confirmer au SHA du commit envoyé. La modification locale ultérieure de ParoisModeSolo.png est séparée et exclue du commit GAME-015.

## GAME-015 — livraison vérifiée 15/10/2026
GAME-015 : game_mode/start_ui/remaster_ai_fix → JNI/SpaceFortressSoloGameActivity (:sologame) → coop_solo_session_runtime. Parcours/antre = solo_level_flow ; miniboss = coop_solo_miniboss_runtime ; arène commune = coop_solo_encounter ; effet = solo_lair_transition ; résultat = solo_run_result + solo_campaign_persistence. APK/AAB : temp/solo-game-015/.

Source `f5b4ce4f82e99057d041767d982914e903afbf7f`; CI Android/C++/SDL2 `38724287864` SUCCESS; publish-release SKIPPED.
- Atlas animé: `assets/pict/solo/SoloBonusAnimations.png`, 1536x768, 4x8 frames de 192 px, SHA256 `1a337adb15a6a3901b76da566dcd26a1d2d76c26893864a37a85e97b63950001`.
- `SpaceFortressVs-1.4.2-SOLO-GAME-015-f5b4ce4.apk`: 95023204 octets, SHA256 `f5138def32cdb8553e86006f6e079ce6a61a979b764c6c62f1757aca61ca8ec7`.
- `SpaceFortressVs-1.4.2-SOLO-GAME-015-f5b4ce4-unsigned.aab`: 92378949 octets, SHA256 `1b4d9a9bffb7ade770ec033bd25162c20c440c9a01db71ad8613c2156aee0e7c`.

## 16/10/2026 — SOLO PARALLAX-016
coop_solo_session_runtime → CoopSoloScenery, textures par renderer, load/reset/release. solo_scenery_assets/solo_scenery_atlas mutualisent les ressources avec l'ancien SoloAssets ; pas de Session ni de global supplémentaire. Corps Rock plein, matière fixe dans le monde, bevels internes sur faces exposées. Lave/Beam restent traversables selon le moteur. Arène finale reste sfCoopDrawArena. Détails : docs/solo-parallax-016.md.

PARALLAX-016 — export préparé : source `2a933a60639d890fd6168befb8ce1784ba9a18c8`, CI `38837760766`. Le workflow attend obligatoirement SUCCESS et vérifie le SHA exact avant publication dans `temp/solo-parallax-016/`. CI/export pas encore déclarés terminés.

## PARALLAX-016 — livraison vérifiée 16/10/2026
PARALLAX-016 : coop_solo_session_runtime → CoopSoloScenery ; ressources communes solo_scenery_assets/atlas ; textures par renderer, reset/release explicites. Diagnostic APK/AAB : temp/solo-parallax-016/ ; détails docs/solo-parallax-016.md.

Source `2a933a60639d890fd6168befb8ce1784ba9a18c8`; CI Android/C++/SDL2 `38837760766` SUCCESS; publish-release SKIPPED.
- Atlas animé: `assets/pict/solo/SoloBonusAnimations.png`, 1536x768, 4x8 frames de 192 px, SHA256 `1a337adb15a6a3901b76da566dcd26a1d2d76c26893864a37a85e97b63950001`.
- Fond ajouré : `assets/pict/solo/FondsParallaxeModeSolo.png`, RGBA1254x1254, SHA256 `4becaec70bf7f23d823c3eccbd902b86aa4dbb6942115a1389409bc6a7f7df44`.
- `SpaceFortressVs-1.4.2-SOLO-PARALLAX-016-2a933a6.apk`: 98400063 octets, SHA256 `62a029ca828a85783f324301bcb5d1411cfa8f343875f7e5ebf4108b5ce8515f`.
- `SpaceFortressVs-1.4.2-SOLO-PARALLAX-016-2a933a6-unsigned.aab`: 95691410 octets, SHA256 `1c18b7d99a45816a2332cd77415927cbf26cbb93cc1346897160c18eafc76703`.


## 16/10/2026 — SOLO PHYSICS-017
PHYSICS-017 : points vérifiés = campaign_runtime::sfCoopMovePlayers, coop_solo_session_runtime (release, caméra, transitions), coop_solo_world_bridge (projection/collision), legacy_field_runtime (ast() et paires sans parois), solo_level_flow (quatre ancres), coop_solo_miniboss_runtime (poursuite LOS locale), coop_solo_scenery/world_draw/bonus_draw (projection). Plan : docs/superpowers/plans/2026-10-16-solo-physics-017.md.

PHYSICS-017 — point de reprise 16/10 après interruption : implémentation locale des briques (inertie/caméra/zoom, repos/collision roches, visibilité audio, 10–12 rencontres/chasse) présente, NON LIVRÉE. Tests purs inertie Danger9/FPS, contact/repos roches, zoom exclusif, navigation réelle/occlusion et placement des rencontres passés. Suite complète a révélé une erreur de portée de variable dans le son du minage, corrigée ; compilation globale/tests runtime à reprendre. Environnement /tmp réinitialisé entre les tours, dépendances SDL en reconstitution dans scratch persistant de la session. Aucun APK-017 annoncé, aucune validation Samsung. Prochaine étape : tests réels moteur, revue, CI Android, export exact.

## PHYSICS-017 — implémentation et revue, 16/10/2026
Nouveaux modules : solo_ship_motion, solo_zoom_gesture, solo_rock_physics, solo_field_policy/runtime, solo_maze_path. Intégration opt-in dans campaign_runtime, hôte SOLO, bridge, rendu et miniboss ; historique inchangé hors SOLO. Rapport/protocole : docs/solo-physics-017.md. CI source puis export vérifié temp/solo-physics-017 à suivre.

## PHYSICS-017 — livraison vérifiée 16/10/2026
PHYSICS-017 : ShipMotion/ZoomGesture → hôte du vrai SOLO ; bridge écran/monde et balayages ; SoloFieldPolicy opt-in pour roches et audio ; mazeRoute + miniboss pour la chasse. Rapport et protocole : docs/solo-physics-017.md. APK/AAB directs : temp/solo-physics-017/.

Source `3ef362cd1375034519c458a0effb2d7abef03490`; CI Android/C++/SDL2 `38956636032` SUCCESS; publish-release SKIPPED.
- Atlas animé: `assets/pict/solo/SoloBonusAnimations.png`, 1536x768, 4x8 frames de 192 px, SHA256 `1a337adb15a6a3901b76da566dcd26a1d2d76c26893864a37a85e97b63950001`.
- Fond ajouré : `assets/pict/solo/FondsParallaxeModeSolo.png`, RGBA1254x1254, SHA256 `4becaec70bf7f23d823c3eccbd902b86aa4dbb6942115a1389409bc6a7f7df44`.
- `SpaceFortressVs-1.4.2-SOLO-PHYSICS-017-3ef362c.apk`: 98432836 octets, SHA256 `8387c28fb63ae6d2fe4017af5ac3b29b49c906682e5124c8273031d6cebb2ac9`.
- `SpaceFortressVs-1.4.2-SOLO-PHYSICS-017-3ef362c-unsigned.aab`: 95725985 octets, SHA256 `3af4730f3104606e4e6f3af694baa3165a9cc3447174705c4553b72d6736b95f`.


## SOLO BOUNCE + DIRECT PINCH-018 — 16/10/2026
Retour Fab Samsung : inertie de PHYSICS-017 excellente ; murs collants, rebond tardif, doigt bloqué. Correctif autorisé : `coop_solo_world_bridge.hpp::moveWithBounce` (collision balayée + restitution .46 + tangente .94 + sous-temps restant + récupération légère), `campaign_runtime.hpp::sfCoopMovePlayers` (vitesse réfléchie, 0,20 s suppression de pression vers paroi, sortie immédiate), `coop_solo_session_runtime.hpp` (DOWN mouvement sur coque uniquement, pinch direct avec deux doigts hors coque), `solo_zoom_gesture.hpp` (pinch exclusif sans présélection). Tests mis à jour : moteur, bridge, zoom. Code `8875905`; CI Android `38962472820` en cours lors de ce suivi. Ne pas présenter comme testé Samsung, ni faire merge/main/Release. Guide : `docs/solo-bounce-pinch-018.md`.

SOLO-018 / reprise CI : `8875905` implémente la physique et le zoom ; `a85ca0d` adapte uniquement le test de traversée extrême (100000 px/s n'implique pas une vitesse finale de signe unique après rebonds multiples). CI initiale `38962472820` échec à cette assertion, CI relancée `38963255438` : attendre verdict Android ; aucune APK-018 livrée à ce stade.

## SOLO-018 — export vérifié
SOLO-018 : CI exacte 38963255438 SUCCESS, code a85ca0dbd8e94e463d11e42b5b2cefb32ab31116. Export vérifié dans temp/solo-bounce-pinch-018. APK diagnostic + AAB non signé. Rebond et touch/zoom restent à valider sur le Samsung de Fab ; pas de main/Release.


## SOLO-019 — zones de mini-boss/hordes et pad de tir
`solo_encounter_gates.hpp` regroupe par pack et ouvre après dernier KO ; `coop_solo_world_bridge.hpp` porte la barrière physique locale active ; `coop_solo_session_runtime.hpp` relie déclenchement, rendu de cercle énergétique et OPTIONS. **Aucun verrou de l'antre, aucune altération de `enterLair`**. `solo_fire_pad.hpp` : huit directions 0..315° de 45 en 45, joystick à relâchement avec mémoire du vecteur, menu de permutation gauche/droite sauvegardé. `campaign_runtime.hpp` : précision du pad SOLO seule, tirs historiques autres inchangés. Source `568d689`, CI `38969053715` à vérifier. Rapport : `docs/solo-encounter-fire-019.md`.

## SOLO-020 — rayon et défilement
`solo_encounter_gates.hpp` : radius *=1.5, marge de sécurité cohérente ; `solo_viewport.hpp::soloSoftCircleZoneSpeed` (profil dédié SOLO-COOP) : centre calme sur 2 diamètres, pré-bande douce jusqu'à 65 % rayon cercle extérieur (0→0.1), bande 65→80 % (0.1→2), bande 80→100 % (2→6). `coop_solo_world_bridge.hpp` : heldFingerPilotTarget et panIfShipActuallyMoved utilisent le même profil ; `soloCirclePan` historique inchangé. Tests dans coop_solo_world_bridge_regressions.cpp et solo_encounter_gates_regressions.cpp. Commit `dd6a4c7`; CI 38973185112 ; export conditionnel direct `temp/solo-cruise-020/`.

## SOLO-020 — livraison CI + dépôt direct vérifiés
- Code source `dd6a4c75066fd51a359c00bbd952a32674eaaa25` ; Android/C++/SDL2 CI `38973185112` **SUCCESS**.
- Export GitHub `38973239202` **SUCCESS**, fichiers réels dans `temp/solo-cruise-020/` : APK diagnostic 98 449 216 octets, SHA-256 `ad683345d31f6857cc90589557905931a4076634ea69781bc636e8887ddfffb0` ; AAB non signé 95 742 279 octets, SHA-256 `16dfdbc3948a8d2689aa43d0320ee1a1f1f09b82e3e5b388fa59749f4bfccbfa`. Sommes vérifiées par workflow. Pas de main ni de Release.
- **Reste :** validation tactile réelle Samsung (défilement proche centre, x1.5 zones, dégâts/barrières et joystick).

## SOLO-021 — correction visuelle des murs, branche uniquement
Renderer réel `src/coop_solo_scenery.hpp` (et non le renderer SOLO prototype). Matière murale `material[6]` chargée depuis `assets/pict/solo/ParoisModeSolo.png` : source 200×58, répétition de motif `wallRepeatCellsX=2`, `wallRepeatCellsY=.65` contre 4,5×2 auparavant ; `SDL_ScaleModeNearest` exclusivement sur les parois ; `space` et `mineral` restent `SDL_ScaleModeLinear`. Biseau plus lisible. Régression `tests/coop_solo_scenery_regressions.cpp`. Base `cf6cc6e`, commit `f896211`, réversibilité documentée précisément dans `ordres-de-mission.md`. Ne jamais recoder depuis la mémoire : lire le code et le commit exact.


### SOLO-021 — résultat définitif (17/10/2026)
- **Source exécutée** : `04da08db80f543a081a7a3614f85c1b2fd28e7c1`, code de rendu `f896211164481cede61d0073729f6eb3362be177`, état de référence précédant l'essai `cf6cc6e479ce854c3e64ddb00f6b9566d9cf52a9`.
- **CI verte** : GitHub Actions `39019113903` (logiciel SOLO, SDL2, Android), toutes les tâches requises SUCCESS, publication Release SKIPPED.
- **Export GitHub direct vert** : run `39019145459`, sous `temp/solo-wall-crisp-021/`.
- **APK de diagnostic** : `SpaceFortressVs-1.4.2-SOLO-WALL-CRISP-021-04da08d.apk`, 98 449 218 octets, SHA-256 `5a58885b9a76e52e4c88ceeffd044c5b0862ef421b0a4b4a8848b0dbf38cea43`.
- **AAB non signé** : `SpaceFortressVs-1.4.2-SOLO-WALL-CRISP-021-04da08d-unsigned.aab`, 95 742 237 octets, SHA-256 `3ae61f0554464c29cec1a0640ad7cc922ba7c047a18e6fae3a04412b4085d4fc`.
- **Comparaison à faire sur Samsung** : SOLO-020 avant ↔ SOLO-021 après, à zoom 1 et 2 et en mouvement ; noter netteté, effet pixel, raccord et répétition du rocher. Aucun résultat de qualité visuelle vécu ne peut être attribué au téléphone sans test réel.
- **Retour arrière sans perte de travail** : `git revert 04da08db80f543a081a7a3614f85c1b2fd28e7c1` puis `git revert f896211164481cede61d0073729f6eb3362be177` sur feature/solo-coop-core-v1, suivi de tests. Garder cette trace dans ordres-de-mission. Pas de `main` ni Release.


## SOLO-022 — 1 diamètre de vaisseau, raccord sans arrêt
`src/solo_viewport.hpp` : `soloExtraCruiseStart(screen)` = .5 × `soloSoftCruiseStart(screen)` = 1 diamètre de coque réel en coordonnée écran à zoom 1. `SOLO_EXTRA_CRUISE_HANDOFF=.025`. `soloSoftCircleZoneSpeed` : 0 avant 1D ; smoothstep 0→.025 sur 1D..2D ; smoothstep .025→.1 sur 2D..65 % du rayon externe ; .1→2 sur 65..80 % ; 2→6 jusqu'à bord. `coop_solo_world_bridge.hpp` réutilise cette fonction pour cible du doigt et caméra, identiques. `tests/coop_solo_world_bridge_regressions.cpp` couvre les quatre raccords, huit directions, cible world, caméra conditionnée au mouvement physique. Source `6c79f88`, CI `39040122861` / export `39040168281`. Ancienne `soloCirclePan` du prototype inchangée. Documentation rollback : ordres de mission.

### SOLO-022 — validation finale et téléchargement direct — 17/10/2026
- Commit source exact `6c79f88addd673c4be8a8efc853c091ee94ae675`; CI `39040122861` **SUCCESS** pour C++/SDL2/Android, Release **SKIPPED**.
- Export GitHub `39040168281` **SUCCESS**. Fichiers réellement présents dans `temp/solo-inner-cruise-022/`, sommes SHA256 contrôlées par workflow et manifest.
- APK de diagnostic installable : `SpaceFortressVs-1.4.2-SOLO-INNER-CRUISE-022-6c79f88.apk`, 98 449 221 octets, SHA256 `540fdae8cc278550622bcfa4ba11d9ae7eccb774111a529bbc4365118b84cab8`.
- AAB non signé, pas destiné à Google Play : `SpaceFortressVs-1.4.2-SOLO-INNER-CRUISE-022-6c79f88-unsigned.aab`, 95 742 235 octets, SHA256 `130ab9ad27041aeb7711f86518a3b368063e1aab9c07902a220430c70b93b4ae`.
- La fonction de défilement démarre désormais à un diamètre, atteint 0,025 à deux diamètres, reste monotone jusqu'aux zones précédentes. Toujours aucune modification physique, des textures ou des modes historiques.
- **À valider par Fab sur Samsung** : sensation de lenteur à la nouvelle bande, absence d'arrêt à deux diamètres, cohabitation avec touches de tir et pincement. Retour arrière isolé : `git revert 6c79f88addd673c4be8a8efc853c091ee94ae675` sur la branche SOLO en conservant cet ordre de mission. Ni main, ni Release.


### SOLO-022 — résultat final vérifié, 17/10/2026
CI source **39040122861 SUCCESS** (Android + C++ + SDL2), export direct **39040168281 SUCCESS**. APK 98 449 221 octets sous `temp/solo-inner-cruise-022/SpaceFortressVs-1.4.2-SOLO-INNER-CRUISE-022-6c79f88.apk`, SHA-256 `540fdae8cc278550622bcfa4ba11d9ae7eccb774111a529bbc4365118b84cab8`. AAB non signé disponible au même endroit, 95 742 235 octets, SHA-256 `130ab9ad27041aeb7711f86518a3b368063e1aab9c07902a220430c70b93b4ae`. Tests Samsung de comportement réel non encore faits. Main et Release intacts. Revert isolé `git revert 6c79f88addd673c4be8a8efc853c091ee94ae675` sur la branche SOLO, sans reset ni force-push.


## SOLO-023 — planche de rebords rocheux
`assets/pict/solo/HabillageRebordsModeSolo.png` : 1536×512, 12 colonnes × 4 lignes de sprites 128×128 (3 variantes × 16 connectivités binaires N/E/S/W). `scripts/generate-solo-edge-atlas.py` et action de création d'asset `.github/workflows/solo-023-build-edge-atlas.yml`. `src/solo_edge_sprites.hpp` construit à chargement 6×16×3 textures RGBA en gardant les couleurs des six thèmes `ParoisModeSolo.png`. `src/coop_solo_scenery.hpp` détecte voisins `Tile::Rock`, choisit un variant par hash des coordonnées, et dessine des rebords organiques via SDL_RenderGeometry ; fallback original si le PNG manque. `tests/coop_solo_scenery_regressions.cpp` : présence 288 textures, alpha en coins, occlusion idempotente, rendu fondu identique, collisions inchangées. Pas d'autre mode. Source `39d0a7c2`. Revert ciblé dans ordres de mission.


**SOLO-023 correction de test + livraison** : correctif `51752da4` (noms de variables `redPixels`, `greenPixels`) ; run source `39057829358`. Export exact `.github/workflows/solo-rock-rims-023-export.yml` épinglé à ce run/commit. Réversion : test `51752da4` puis fonctionnalité `39d0a7c2`. Référence source du rendu `src/solo_edge_sprites.hpp`, générateur Python séparé.


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


SOLO-023 livraison contrôlée CI `39058401438` + export `39058427941` SUCCESS, code `48b30c4`, APK `temp/solo-rock-rims-023/SpaceFortressVs-1.4.2-SOLO-ROCK-RIMS-023-48b30c4.apk`, SHA256 `8ef3769b08a200f382465907486f28c36d54b300294fa17fdafcbb8c0cdb0e71`. Revert de la fonctionnalité après correction de 3 tests : `48b30c4` → `57f74d8c` → `51752da4` → `39d0a7c2`, ordres-de-mission.md canonique.


## SOLO-024 — code et frontières
`src/solo_organic_rims.hpp` : `SoloOrganicRims::field(map,gx,gy)` smoothstep bilinéaire des centres Rock ; `contourThreshold(gx,gy)` ondes sinusoïdales en coordonnées **monde** ; `visibleRock`, `signature` et cache LRU jusqu'à 24 textures de 384×384 RGBA (8×8 cellules ×48 px). Change de morceaux si voisins de map changent ; peinture intacte `ParoisModeSolo.png`. `CoopSoloScenery::drawWalls` rend le champ continu avant dangers et garde SOLO-023 comme fallback ; même binaire alpha pour les deux passes (filtre nearest). `src/solo_viewport.hpp::soloBackgroundOnlyGain` = progression 0→1 dans bande centre 0,5D..1D, puis 1→0 dans 1D..2D. `CoopSoloWorldBridge::updateBackgroundOnly` ajoute déplacement réel × gain au `backgroundDrift`, caméra inchangée ; `drawBackground` utilise .20×drift pour espace et .48×drift pour minéral. Rendu indépendant du zoom, contrôle/doigt/collision inchangé. Tests dans `tests/coop_solo_world_bridge_regressions.cpp`, `tests/coop_solo_scenery_regressions.cpp`. SHA `210afab`, `599f6a8`, `2946e9a`; rollback dans ordres de mission.


### SOLO-024 — livraison vérifiée 17/10/2026
- Source définitive `2946e9a30cfc9923ccd6371c16a2121c66abc1a3`, CI complète Android/C++/SDL2 **39067948776 SUCCESS**, aucune Release (SKIPPED).
- Export direct **39068019221 SUCCESS**. Vrais binaires présents dans `temp/solo-organic-024/`.
- APK debug installable `SpaceFortressVs-1.4.2-SOLO-ORGANIC-024-2946e9a.apk`, **98 710 352 octets**, SHA-256 `25cff97cf6439b52d38d5de092d452f68cdd34c071308decde29d8e74aad930b`.
- AAB non signé `SpaceFortressVs-1.4.2-SOLO-ORGANIC-024-2946e9a-unsigned.aab`, **96 006 869 octets**, SHA-256 `4aec7e7ea8028551c2c1cf2362301c785c12996aac3496f45efe81927f315454`. Hash et ZIP/artefact source vérifiés dans workflow.
- À vérifier perceptivement sur Samsung : contours longs à l'intérieur/extérieur des roches, répétitions visuelles, absence d'artéfact d'occlusion, effets de défilement du fond dans zone de rayon 0,5→1 diamètre, raccord à 1→2 diamètres, collision logique inchangée. CI n'est pas preuve de confort visuel.
- Revert source si demandé par Fab : `git revert 2946e9a30cfc9923ccd6371c16a2121c66abc1a3`, puis `git revert 599f6a82347f10f25ed9354b35c6f8d895f72ea4`, puis `git revert 210afab295dd2277986fe44b7377a4e9c2bfd5dd`, branche `feature/solo-coop-core-v1` uniquement, résoudre les conflits de documentation sans supprimer cette trace, relancer CI. Ne pas réinitialiser, force-push, fusionner main ou faire Release.

## SOLO-025 — chemins source sans WAV dans Git
`src/solo_rock_material_variation.hpp::soloRockMaterialAt` et `src/solo_organic_rims.hpp` : warping et modulation continus, aucune couture aux chunks. `src/solo_meteor_take_picker.hpp::MeteorTakePicker` : quatre prises mélangées équitablement, sans répétition immédiate. `src/solo_meteor_sound_state.hpp::meteorRockPairPending` : bool explicitement partagé entre `solo_field_runtime.hpp::sfSoloRockPair` et `campaign_runtime.hpp::sfCoopPlaySounds`, pas de dépendance à sfCoop avant déclaration. `coop_solo_session_runtime.hpp` charge les 4 WAV de Djipie si présents, sinon banque historique. Identités des WAV, tailles et SHA dans `docs/solo-025-material-audio.md`. `scripts/fetch-djipie-meteors.sh` (gh release privée), `scripts/verify-solo-meteors-apk.py` (preuve 0/4 ou 4/4), Android workflow optionnel `DJIPIE_READ_TOKEN`. Branche privée Djipie `feature/solo-025-private-meteor-apk` action `39085282683` pour build **avec 4 WAV** sans secret inter-repos. Runs source public `39085021465`. Retour arrière : ordres-de-mission.



### SOLO-025 — repères vérifiés
Code matière `src/solo_rock_material_variation.hpp` et `src/solo_organic_rims.hpp`; son `src/solo_meteor_take_picker.hpp`, `src/solo_meteor_sound_state.hpp`, `src/campaign_runtime.hpp`, `src/solo_field_runtime.hpp`, `src/coop_solo_session_runtime.hpp`. CI source `39085021465` SUCCESS sur `b6e0a38c`. Export public `temp/solo-variety-025/` = 0 nouveau WAV (fallback). APK quatre WAV authentiques **seulement** dans Djipie privé branche `feature/solo-025-private-meteor-apk`, `temp/SpaceFortressVs-SOLO-025-Djipie/`, run `39085282683` SUCCESS, SHA256 `f08b1f9c...`. Les clients se connectent au dépôt privé pour télécharger. Retour arrière exact dans ordres-de-mission.md.

## SOLO-026 — index de l'audit sans code, 18/10/2026
Retour Fab Samsung : (1) cône visible plus étendu que minage (3,30 D poussière dessinée vs 2,025 D minage, dépend de la difficulté), (2) défilement « fond seul » proche centre de SOLO-024 rompt la cohérence des murs et masque la vitesse intermédiaire, (3) rebond de paroi WORLD sans coût énergie/PV, (4) recharge COOP réutilisée en SOLO trop rapide : viser plein par patience ou minage. Lire les conclusions et le contrat fonctionnel dans `brain.md`, causes et pièges dans `debughistorical.md`, tâches non commencées dans `todo.md`, autorisations dans `ordres-de-mission.md`. **Aucun code modifié ni autorisation de coder donnée.** Ne pas charger des mémoires d'autres dépôts pour ce suivi.

### SOLO-026 — code de gameplay après GO Fab
Point de reprise : `src/legacy_field_runtime.hpp` (parité vraie portée cône visuel et minage, danger préservé), `src/solo_viewport.hpp` + `src/coop_solo_world_bridge.hpp` + `src/coop_solo_scenery.hpp` (première bande caméra et fond uniquement asservi à la caméra), `src/solo_wall_impact.hpp` + `src/campaign_runtime.hpp` (chocs murs coque+énergie/cooldown), `src/ship_energy_rules.hpp` (recharge SOLO ralentie ; COOP/DUEL préservés), `android/app/src/debug/AndroidManifest.xml` + `SpaceFortressSoloGameActivity.java` (entrée directe SOLO et secret 20 taps alternés en haut). Tests et livraison dans `todo.md`. Workflow sans livrer avant GREEN : `.github/workflows/solo-026-direct-export.yml`. Ne pas confondre nouvelle implémentation et audit précédent, ni affirmer que tests / APK / audio privé sont validés avant contrôle.

### SOLO-026 — livraison vérifiée CI et APK (18/10/2026)
SOL0-026 : source `788c03c`, CI `39132530943 SUCCESS`, export `39132531008 SUCCESS`, binaires directs `temp/solo-026/SpaceFortressVs-1.4.2-SOLO-026-788c03c.apk` et `-unsigned.aab`. Metadata et checksums dans `temp/solo-026/`. APK 0/4 sons privés Meteor ; ne pas confondre avec SOLO-025 privé contenant 4/4. Essais Samsung en attente.

### SOLO-027 — cartes de reprise du double stick et du démarrage
- `src/solo_fire_pad.hpp` : `FirePad` tir continu 360° à mémoire de direction / 1,25s ON puis 1,50s OFF, `MovePad` contrôle violet indépendant; rendu SDL pointillés translucides TURQUOISE/VIOLET.
- `src/coop_solo_session_runtime.hpp` : `stickSteering` vs déplacement tactile vaisseau ; `movePad.right=!firePad.right` systématique ; `solo-fire-hand-v1.cfg` rétrocompatible (R/L puis S/F), reconnexion de `ShipMotion` par `sfCoop.controls[1].target` chaque substep, deux lignes Options et petite sentinelle `Tourelle.png`. `fireAlongPad` appelle vraie `sfCoopFire`.
- `android/app/src/debug/AndroidManifest.xml` : launcher historique `SpaceFortressActivity` au lieu de jeu SOLO direct ou HUB; SOLO interne :sologame inchangé. `SpaceFortressActivity.java` : 20 taps G-D tout en haut pour hub en debug seulement.
- Tests `tests/solo_fire_pad_regressions.cpp` et `scripts/check-solo-launcher.py`.
- Export APK vérifié direct `.github/workflows/solo-027-direct-export.yml` vers `temp/solo-027/` après source CI vert exact. Aucun ZIP utilisateur, WAV privés Djipie à distinguer du fallback public.

### SOLO-027 — livraison directe vérifiée le 18/10/2026
Source exacte `d861a22221abaaf08ec516a2a2b9123b0a537e41` : CI Android+C+++SDL2 `39136940735 SUCCESS` ; export GitHub Actions `39136940825 SUCCESS`, livraison binaire sur la branche de travail `temp/solo-027/`. APK direct installable `SpaceFortressVs-1.4.2-SOLO-027-d861a22.apk`, taille 98 727 555 octets, SHA256 `615143149e07af1481bbb490e5d5749fbda30aa20ba6e0935096435186460366`. AAB non signé `SpaceFortressVs-1.4.2-SOLO-027-d861a22-unsigned.aab` taille 96 014 785 octets, SHA256 `4b9895c0768b1c36e3b97c1b9a9843e8476d7c316b5049842876cde8e3687f5b`. Son Meteor Djipie privé non disponible dans cet APK public : `0/4` WAV, fallback audio classique. Pas de test sur Samsung effectué par l'assistant : valider prise en main réelle du menu/hub 20 taps, stick optionnel, swap synchronisé G/D, tir avec pauses et fluidité. Aucun merge `main`, aucune Release. **Règle immuable : extrait `.apk` directement par GitHub Actions sur serveur et disponible par lien direct sans ZIP utilisateur.**

### SOLO-028 — reprise du contrôle progressif / Game Over
`src/solo_fire_pad.hpp::MovePad`: rayon tactile accru, double graduation du doigt (`travel`) vs commande moteur (`magnitude` ease-in), `virtualReach(width,danger)` indexé sur `sfsolo::ShipMotion`; suppression disques pleine taille sous **les deux** joysticks (`FirePad::draw` / `MovePad::draw`). `src/coop_solo_session_runtime.hpp` : `movePad.virtualReach(sfArenaW,sfBossDangerIndex)` remplace cible `*.72*magnitude`, event GameOver attrapé avant sticks/zoom/options, affichage final Game Over et bouton menu via `src/solo_game_over_ui.hpp`. Tests exacts `tests/solo_fire_pad_regressions.cpp`, `tests/solo_game_over_ui_regressions.cpp`, orchestration dans `scripts/test-regressions.sh`. Livraison GitHub CI exact SHA : `.github/workflows/solo-028-direct-export.yml`, `temp/solo-028/*.apk` direct.

### SOLO-028 — preuves de compilation et livraison 18/10/2026
SOLO-028 source `f50dbbe` ; Android/C++/SDL2 CI `39143165760 SUCCESS` ; export décompressé serveur `39143165717 SUCCESS`. Fichier direct `temp/solo-028/SpaceFortressVs-1.4.2-SOLO-028-f50dbbe.apk` SHA256 `4f9fc3a2ecd4be84e05af57cac1a7e2dbd4ae86f7693b1edc97f4bf9b0114c25` ; AAB unsigned dans même temp. 0/4 WAV privés, fallback audio historique. Essai Samsung indispensable.

### SOLO-029 — reprise architecture caméra / pilotage
Production : `src/coop_solo_world_bridge.hpp` contient `cameraDifficulty`, `lookAheadX/Y`, `followPhysicalShip` qui utilise le mouvement PHYSIQUE par substep, pas les positions/paliers écran; `centerOn` annule l'avance, `setZoom` préserve l'ancre. `src/coop_solo_session_runtime.hpp` capte l'intention de doigt lors du DOWN sur la coque via `hullTouch.begin`, la met à jour sur SDL_FINGERMOTION (différence au DOWN, non le cadrage), et transmet `target=shipWorld+direction*reach` au même ShipMotion que `MovePad`. `src/solo_fire_pad.hpp` fournit `MovePad::steeringCurve/reachFromMagnitude` et `HullTouchPilot`. Tests `tests/coop_solo_world_bridge_regressions.cpp`, `tests/solo_fire_pad_regressions.cpp`. Les anciens helpers de zones `solo_viewport.hpp` sont conservés à titre historique pour les autres tests/anciennes voies. Export CI vert `.github/workflows/solo-029-direct-export.yml` vers vrai `temp/solo-029/*.apk`, aucun ZIP. Son Meteor Djipie privé doit être annoncé 0/4 s'il est absent.

### SOLO-029 — livraison source CI verte et APK direct vérifié (18/10/2026)
Source testée `e7bb328a4373beb941b6002e5efcc50c8ab5c965`. CI `Android APK + AAB` run `39153071193` **SUCCESS**, trois suites core C++/SDL2/Android vertes, `publish-release` SKIPPED. Export GitHub `SOLO-029 CI-green APK direct` run `39153071248` **SUCCESS**. Fichier INSTALLABLE déjà extrait sur GitHub branche `feature/solo-coop-core-v1` : `temp/solo-029/SpaceFortressVs-1.4.2-SOLO-029-e7bb328.apk`, taille **98 727 558 octets**, SHA256 `f2e93f6fcda37091dd721287a82d7f4825007d3d532af1e18250d7b90a78a95d`. AAB **non signé** : `temp/solo-029/SpaceFortressVs-1.4.2-SOLO-029-e7bb328-unsigned.aab`, taille **96 015 350 octets**, SHA256 `1cd371a10415cee437602d7edfb44fe4b8af9e953dc46e0a135b85bae382f97a`. Meta dans `temp/solo-029/*-build.json` ; Meteor privés `0/4` dans cet APK public, sons historiques de secours, ne pas revendiquer 4/4. Aucun ZIP utilisateur, aucun merge main, aucune release publique. Essai réel de caméra tactile Samsung encore à réaliser avec Fab.

### SOLO-030 — implémentation et points d'entrée
`src/solo_fire_pad.hpp::SteeringHoldBoost` = cumul borné d'intention et contrôle direction référence figée. `MovePad::steeringCurve` cubique, `MovePad::orbit/cx` rayon +30%/centre intérieur, `MovePad::tick`, `MovePad::effectiveMagnitude` (clamp<=1). `HullTouchPilot::tick/effectiveMagnitude` même algorithme pour pilotage doigt au vaisseau. `src/coop_solo_session_runtime.hpp` : appels tick sur chaque sous-pas physique avant calcul `virtualReach`. `tests/solo_fire_pad_regressions.cpp` : premier cercle doux, cumul ~2,2s bord stable, désactivation sur inversion/centre/release, borne physique Danger1/9. Workflow à installer `.github/workflows/solo-030-direct-export.yml` pour `temp/solo-030/`, vrai APK direct sans ZIP.

### SOLO-030 — livraison CI verte et APK direct (18/10/2026)
Source exacte `626a24cb021bba6b665f6015567555a8cd9f345f`. Run CI `39156942027` **SUCCESS**, trois suites core C++/SDL2/Android et régressions vertes ; `publish-release` SKIPPED. Export officiel GitHub Actions de branche `39156941944` **SUCCESS**, extrait côté serveur dans `temp/solo-030/` sans ZIP utilisateur. APK de diagnostic installable `temp/solo-030/SpaceFortressVs-1.4.2-SOLO-030-626a24c.apk` **98 727 544 octets**, SHA256 `6e3ad65efe01c759988dd8076bf0c667805a4c0b3d6decb7457d269d10e9a3ec`. AAB non signé `temp/solo-030/SpaceFortressVs-1.4.2-SOLO-030-626a24c-unsigned.aab` **96 017 166 octets**, SHA256 `274b7269b13727a457e22bb59c60bed77ac3f8bde9590f2486bf05aab147ed0c`. `*-build.json` et `SHA256SUMS` vérifiés. Meteor Djipie privé absent `0/4` WAV, sons historiques de secours; ne pas annoncer les sons nouveaux. Tests physiques sur Samsung toujours à réaliser par Fab : réglage de douceur et bonus de maintien, marche arrière et inertie, affichage, main G/D. Aucune fusion main ni Release.

### SOLO-031 — points d'entrée du code
1. `src/solo_fire_pad.hpp` : `HullTouchPilot::orbit` 0,125W (52..82px), `SteeringHoldBoost` edgeStart=.68 edgeFull=.94 maxBonus=.36, surcharge `effectiveMagnitude` plafonnée à 1.30, maintien cap fixe + stabilité (.925/.985 dot) ; `MovePad::update` filtre petit jitter angulaire, son rayon large inchangé.
2. `src/solo_ship_motion.hpp` : septième argument facultatif `soloCruiseFactor=1`, max vitesse multiplié par clamp(1,1.3), inertie historique mesurée contre le maximum sans boost, conservation de toutes les autres signatures.
3. `src/campaign_runtime.hpp` : champ `sfCoop.soloWorldCruiseFactor` par défaut 1 ; transmis dans `sfCoopMovePlayers` seulement si `sfCoop.soloGame&&sfCoop.soloWorldMode&&owner==1`.
4. `src/coop_solo_session_runtime.hpp` : chaque substep réinitialise le facteur, lit `hullTouch.speedMultiplier` ou `movePad.speedMultiplier` pendant leur maintien actif ; annule au `cancelControls` et au changement monde ; passe doigt au `followPhysicalShip` uniquement en direct sur la coque.
5. `src/coop_solo_world_bridge.hpp` : caméra full en stick, en direct limite `lookAhead` pour réduire le gap doigt-vaisseau à l'aide d'une contrainte quadratique. Pas de retour de caméra dans la commande.
Tests `tests/solo_fire_pad_regressions.cpp`, `tests/solo_ship_motion_regressions.cpp`, `tests/coop_solo_world_bridge_regressions.cpp`. Workflow `.github/workflows/solo-031-direct-export.yml` copie vrai .apk après CI vert même-SHA vers `temp/solo-031/`.

### SOLO-031 — CI et livraison APK direct vérifiés (18/10/2026)
Source CI exacte : `c6ea6866e03cdcf0dd287efbc63d1aa08af0c3a0`, workflow Android `39164322357` **SUCCESS** (trois jobs : core, SDL2, build Android) ; publication Release SKIPPED, aucun merge main. Export GitHub `39164322107` **SUCCESS**, vrai APK décompressé par GitHub sur branche `feature/solo-coop-core-v1`, fichier `temp/solo-031/SpaceFortressVs-1.4.2-SOLO-031-c6ea686.apk`, taille **98 727 558 octets**, SHA256 `4f34d92467c370f9b03ba28ac8aee938527847bdf84cc8b29989e4004ad5f878`. AAB non signé `temp/solo-031/SpaceFortressVs-1.4.2-SOLO-031-c6ea686-unsigned.aab` **96 018 861 octets**, SHA256 `5077b545a892d21ca87ae736af99cb8144fc280081910683712eb5e6a4d81936`. Manifest et `SHA256SUMS` concordent. Sons Meteor Djipie privés dans cet APK : **0/4**, sons historiques de secours. Fab doit encore vérifier sur Samsung rayon doigt, maintien de cap, survitesse et impression de proximité. Ne jamais réattribuer cette APK à un commit ultérieur : source reste c6ea6866.

### SOLO-032 — localisation dynamique de réserve
`src/solo_fire_pad.hpp::SteeringHoldBoost::tick` : stabilité directionnelle bidimensionnelle (`anchorDx/Y`), `stableSeconds` cap5, `bonus` lissé, `recoverySeconds` bloque recharge transitoire après virage. Rotation angulaire bornée via `atan2` : même 180° exact sans norme nulle ni flip directionnel ; quand dot≥.987 et `recoverySeconds==0` dans couronne extérieure : charge ~1,05 u/s ; sinon décharge ~1,15 à 3,8 u/s; cible bonus `maximumBonus*.smooth(edge)*smooth(charge/5)` ; filtre exponentiel montée1/s descente1,75/s. `MovePad` et `HullTouchPilot` emploient déjà ce composant sans recodage, commande x1,30 max inchangée. Tests `tests/solo_fire_pad_regressions.cpp`. Export Github `.github/workflows/solo-032-direct-export.yml` copié du contrôleur SHA `solo-031`, mission SOLO-032 vers `temp/solo-032`.

### SOLO-032 — preuves CI vertes et livraison directe (18/10/2026)
Source exacte `503fac23d4ba09d70e9567ca3374ab59cd6355c5`. GitHub CI `Android APK + AAB` run `39169561372` **SUCCESS** sur `build-android`, `solo-sdl2-ui`, `solo-core-regressions` ; `publish-release` SKIPPED. Export `SOLO-032 CI-green APK direct — smooth vector reserve` run `39169561443` **SUCCESS**. Fichier vraiment décompressé côté GitHub dans `temp/solo-032/SpaceFortressVs-1.4.2-SOLO-032-503fac2.apk`, **98 727 553 octets**, SHA256 `075f286d058cd9dfefa5b340bfa3bef8ee5d07756551e443462791103dc9b087`. AAB non signé 96 019 380 octets, SHA256 `b4f78014b072f0cd760f64d2ee5c7016cd91daa5dbfaae0f831612658c6b70f4`. Source/SHA dans `temp/solo-032/*-build.json`, `SHA256SUMS` GitHub vérifiés. Sons Meteor privés toujours absents **0/4** — repli sons historiques, ne pas annoncer intégration. Pas de ZIP sur appareil Fab, pas de merge main/release. Tests tactiles réels encore à faire sur Samsung : démarrage +2/+4/+8s, demi-tour et virages, passage faible vitesse, rebonds.

### SOLO-033 — carte code direct+tensor
`src/solo_fire_pad.hpp::SteeringHoldBoost` : edgeStart=.90, edgeFull=.98, maximumBonus=1.00, maximumRequested=1.30, directCruise=.30, chargeSeconds=6 ; charge `dt*1.05*edge` (aucune précharge secrète à 90 %) ; décharge hystérésis/angle déjà présente SOLO-032. `MovePad::effectiveMagnitude()` ET `HullTouchPilot::effectiveMagnitude()` : 30 % * cubic `magnitude` + filtre bonus sous enveloppe lisse de rayon .30.. .90 ; `virtualReach` et `speedMultiplier` inchangés, envoyés en sous-pas à `ShipMotion`. Retour au centre annule demande de grosse vitesse sans supprimer l'inertie. Tests `tests/solo_fire_pad_regressions.cpp` couvrent aussi **ShipMotion réel** sur les neuf dangers. Exiger CI Android / C++ / SDL2 puis workflow `.github/workflows/solo-033-direct-export.yml`, vrai .apk dans `temp/solo-033/`.
