# SpaceFortressVs 1.4.x — ordre de mission validé par Fab

## Références
- Branche : `fix/gameplay-campaign-200`.
- APK téléphone de départ : code `2ca6aa46e732e9d9e86e9889ad2d215117c1f16e`.
- Audit Astra : HEAD documentaire `300c3f4e86148d84bb7e1690b4dca79cb90a3dde`.
- `src/main.cpp` reste historique et inchangé.

## Décision Fab
Fab valide l’audit Astra et autorise les corrections. Le jeu est jugé globalement trop facile, y compris au niveau facile, mais l’équilibrage doit corriger les mécanismes démontrés avant tout multiplicateur arbitraire de PV.

## Lot autorisé
1. D-140-01 : corriger uniquement dans la copie Android générée l’ambiguïté `setw(DM.w)` / `std::setw(int)`; ne pas modifier `src/main.cpp`.
2. D-140-02 : rendre `kind=4` avec le missile historique `missilebb.png`, mouvement/collisions inchangés.
3. D-140-04 : dupliquer l’information de vie boss pour les deux côtés, vert plein → rouge vide, sans valeur numérique ; PV + énergie de chaque pilote sur son côté, bloc du joueur haut tourné à 180°.
4. D-140-03/08/09 : empêcher le minerai de presque réinitialiser le bouclier ; cadence et dispersion initiale dépendent de la réserve ; un tir ordinaire reste rectiligne après départ ; les collisions astéroïdes ne sont pas annulées par l’invulnérabilité projectile et retrouvent la chaleur proportionnelle à la surface.
5. D-140-06 : contact boss = exposition continue, épuisement rapide de l’énergie puis des PV, indépendant de l’invulnérabilité des projectiles.
6. D-140-05 : distinguer les tirs tourelles des tirs pilotes pour mesure. Ne pas augmenter arbitrairement les PV boss dans ce lot.
7. D-140-07 : préserver la prédiction initiale d’Orion ; aucun guidage en vol pour les tirs ordinaires. Le missile reste guidé.

## Règles précises
- Un tir refusé par cooldown ne consomme rien, ne tire rien et ne programme rien.
- Le maintien du doigt ne déclenche jamais d’auto-tir.
- La baisse de précision est une petite dispersion INITIALE ; le projectile ordinaire reste ensuite rectiligne.
- Vie boss : deux barres miroir de la même valeur ; vert plein → rouge vide ; pas de valeur numérique obligatoire.
- Joueur haut : bloc PV + énergie réellement retourné à 180°.
- Joueur bas : bloc PV + énergie orientation normale.
- D-140-05 « quasi one-shot » signifie « boss détruit beaucoup trop vite », pas « une seule salve ».

## Tests obligatoires
- Initialisation réelle W/H de la copie Android + rendu réel d’un astéroïde classique.
- Missile visuellement différent du plasma et recréation renderer.
- HUD miroir, code couleur de vie et géométrie cohérente.
- Cadence plus lente et dispersion initiale plus grande à faible réserve ; tir refusé sans coût ni tir différé.
- Projectile ordinaire rectiligne après départ.
- Contact boss stable à 30/60/120 Hz.
- Impacts astéroïdes successifs non effacés par l’i-frame projectile.
- Nuage de minerai ne réinitialisant plus la réserve.
- Suites historiques complètes, build Android et packaging.

## Contraintes permanentes
Synchroniser `brain.md`, `brainmap.md`, `debughistorical.md`, `todo.md` dans tout commit de code. Aucun merge main ni release sans nouvel accord de Fab. Préserver sauvegardes, 50 portraits, quatre difficultés, écran de fin, icône/assets et moteur historique.


## État après exécution
Lot appliqué au SHA `fbb1ff92907821217ff94d847d9f2bbf4683636d` et vérifié par le workflow 36266178070 : 57 PASS, build et packaging réussis. Prochaine étape : essai physique Fab. Aucun merge main ni release.


## Mission active — dégâts entrants coop ×5 — 2026-10-02
Fab autorise le réglage ×5 des pertes de PV des deux pilotes coop provenant des attaques du boss, du contact direct avec le boss et des astéroïdes. Le coefficient est appliqué exactement une fois après la protection du bouclier. Conserver énergie, récupération, i-frames, dégâts sortants, PV boss, mode classique et `src/main.cpp`. Vérifier les deux pilotes, coop locale/IA, nrj 0/25/50, projectiles et spéciaux boss, contact 30/60/120 Hz, callback astéroïdes et clamp zéro. Aucun merge main ni release. Validation téléphone finale : Fab.


### Suivi CI D-140-10
La première CI (36939207044) a échoué dans le nouveau test astéroïde parce que la mesure incluait la récupération de poussière exécutée après le callback. Corriger uniquement le test pour mesurer l'impact brut au niveau de `sfLegacyFieldStep(sfCoopAsteroidHurt)`; ne pas modifier le gameplay ×5. Relancer une CI fraîche.

Deuxième CI 36939614317 : échec de test uniquement, dû à la hauteur randomisée par `setv()`. Utiliser la surface réelle `impact->w*impact->h`; ne pas modifier le gameplay.


### État validé D-140-10
SHA de code `7e624256d8f50d07a63e92c9c3851ff2b6048ace` vérifié par le workflow 36940059444 : 58 PASS, build Android et packaging 1.4.0 réussis. Artefact 11199572849 (sha256:181030495d4ba54a875734c762a22f6ff80eb22d44551fe2e20c53b8b0265964). Aucun merge main ni release. Étape restante : validation du ressenti sur téléphone par Fab.


## D-140-11 — dégâts ×15 et économie de poussières
Fab demande ×3 supplémentaire sur le ×5 validé : multiplicateur coop total ×15 après protection du bouclier. Les poussières blanches historiques peuvent soigner un boss blessé lorsqu'il les touche ; les munitions boss spéciales kind 1/2/3 peuvent aussi les ramasser et transmettre le soin. Le collecteur éligible le plus proche gagne et un boss à pleine vie ne consomme pas la poussière. Les impacts boss sur un vaisseau génèrent des poussières rouges ; elles ne rendent ni PV ni énergie et, après armement, ajoutent seulement un peu de chaleur `nrj`, donc usent le bouclier. Les collisions d'astéroïdes conservent leur émission rouge historique. `src/main.cpp` reste intact.


### Preuve fraîche D-140-11 APK
Gameplay testé : `23d0e3e1354dec2cadf7b40c7a4f10a8b751a585`. Workflow `37095128530` : régressions complètes GREEN puis build Android APK+AAB réussi. Artefact : `SpaceFortressVs-x15-dust-APK-AAB-37095128530`. Le premier essai avait échoué uniquement sur la validation d'un wrapper Gradle tiers SDL téléchargé trop tôt ; l'ordre CI a été corrigé sans changement gameplay. Aucun merge `main`, aucune release ; validation téléphone reste à Fab.


## Mission D-140-12 — bouclier commun duel + coop
Même modèle de bouclier dans les deux modes : protection = réserve restante ; usure = 10% du coup à 100%, 20% à 90%, etc. Coop garde ×15 une seule fois. Poussières blanches ~0.5% de bouclier chacune. Duel Android via copie générée, main.cpp historique intact. Aucun merge main ni release.


## D-140-13 — MAP MIX / PROGRESSION VISUELLE
Contract: reuse campaign scenic atlas in classic; select classic scenes from 100 candidates without replacement before refill; derive candidates by mixing 20 stable base identities; use subtle planet and boss RGB filtering; progress blue→green→red continuously over 200 campaign encounters; progressively widen boss travel from centre toward safe edges. Preserve `src/main.cpp`, no main merge, no release before Fab validation.

## D-140-14 — Bouclier cinétique (03/10/2026)
Contrat: deux modes, vitesse relative vectorielle, masse linéaire, v², deux couches, énergie nrj partagée à faible coût, dégâts résiduels vers le bouclier énergétique, boss cinétique uniquement en charge, PV régénérés selon réserve avec bonus proche de 100 %, aucun merge/release sans Fab.

## D-140-15 — ordre canonique Fab (03/10/2026)
Remplace les réglages cinétiques incompatibles de D-140-14: BOTH modes, aucun ×15 cinétique, dégâts masse×vitesse d'impact² avec faible plancher masse, champs compacts dépendant de la vitesse, vagues transitoires, coût réserve ×0.00001, rouge réactif, blanc physiquement intangible, aucune merge/release avant test téléphone.

## Kinetic surge / two-finger field — 2026-10-03
- Canon shared by classic + coop: maximum kinetic field diameter = 2.0 ship diameters.
- Short second-finger tap keeps firing; long hold (0.35 s) enters a transparent visible surge.
- Surge doubles kinetic dissipation for at most 2.0 s. Release after activation purges every asteroid inside the max kinetic zone into historical white resource dust.
- White dust is never physically deflected by kinetic waves; red dust remains reactive.
- Normal absorption waves are more transparent; surge aura shows inner + outer circles.
- Classic hull dimensions remain historical; only the kinetic field diameter expands.
- No main merge/release before Fab phone validation.

## AVENANT 2026-10-03 — SURCHARGE AUDIO + DANGER BOSS
Canon validé Fab: charge 2e doigt exactement 2 s, champ x2 irisé pendant la charge; à 2 s son prêt et bouclier cinétique OFF jusqu’au relâchement; relâchement chargé = son de déflagration + purge astéroïdes. Trois sons originaux synthétiques intégrés. Dégâts boss non cinétiques: défaut x10, sélection accueil x1/x5/x10/x15/x20. Le cinétique reste hors multiplicateur boss. Classique et coop homogènes. Aucun merge main/release sans validation téléphone.

## HELP-TUTORIAL-DANGER9 — 2026-10-04 — CODE GREEN / TÉLÉPHONE EN ATTENTE

Fab a validé la spec puis le plan TDD du lot `feature/help-tutorial-danger-9-canon`.

Contrat canonique :
- `?` accueil + en jeu ; reprise de la même partie sans reset ;
- aide `RAPIDE` / `DETAILLE` / `ANIME`, avec `ANIME` par défaut et seul format animé ;
- tutoriel séparé, guidé, sandbox sans progression ni sauvegarde ;
- 9 dangers : `MOU DU GENOU` ×1, `CHILL` ×5, `ROCK N ROLL` ×10 défaut, `DUR A CUIRE` ×15, `MACHINE DE GUERRE` ×20, `CA VA PIQUER` ×25, `SANS PITIE` ×30, `ENFER STELLAIRE` ×35, `APOCALYPSE` ×40 ; coefficients non affichés ;
- multiplicateur uniquement sur dégâts hostiles non cinétiques concernés, y compris tirs IA hostiles en classique ; aucune modification cadence/vitesse/visée ; jamais sur astéroïdes/cinétique ;
- `src/main.cpp` historique inchangé.

Preuve code : SHA `d9520a0b674d7f21df37f982a444d625b523f8d9`, workflow `37236262962` (run 268) entièrement GREEN : régressions, APK, AAB, vérification, packaging. Artefact `SpaceFortressVs-1.4.0-release-files` id `11316375150`, digest `sha256:65d8a536339e48f8b5f95207a082215b0e306289171183f999824f807b1a59c5`. `publish-release` SKIPPED.

Étape restante : validation physique sur téléphone par Fab. **Aucun merge `main` ni aucune release avant accord explicite de Fab.**

## KINETIC-DUST-IMPACT-V4 — 2026-10-05 — CODE GREEN / TÉLÉPHONE EN ATTENTE

Branche dédiée `feature/kinetic-dust-impact-v4` depuis `76bab7bac571df6f42f4ae76011fc61ab2b0c8ee`. Gameplay figé au SHA `8f2ee5ed61372f2647ae7284abdf9a0df7d8ebbf`.

Contrat canonique :
- purge armée 2 s : destruction astéroïde = 100 % du rendement blanc proportionnel à la taille/masse, poussière initialement quasi statique ;
- destruction astéroïde↔astéroïde et destruction par champ cinétique normal : rendement blanc 10 %, proportionnel, projection suivant principalement le vecteur incident du détruit ;
- une destruction réelle = une seule émission de poussière blanche ; minage historique séparé ;
- poussière rouge au champ : réaction locale jaune→orange→rouge vif, majorité consumée en micro-flashes, petite fraction survivante déviée par vecteur incident + réaction locale du champ ;
- aucun soin, recharge, dégât, danger ou équilibrage ajouté via le rouge ; cinétique v3, campagne 200, aide/tuto/Danger 9 et `src/main.cpp` restent protégés.

Preuve code : TDD RED `afbd28da13d81409d083ef7cc1f8a93010ceaec0` / workflow `37240892431`, puis GREEN au SHA gameplay `8f2ee5ed61372f2647ae7284abdf9a0df7d8ebbf` / workflow Android `37241618498` : régressions, APK, AAB, vérification et packaging réussis ; `publish-release` SKIPPED. Artefact `SpaceFortressVs-Android-444`, id `6545681985`, digest `sha256:399dcf6808a83fe4071ab91d2b9b9d2465c0a6e2c6f5950824482687a936fd6e`.

Étape restante : validation physique des effets sur téléphone par Fab. **Aucun merge `main` ni aucune release avant accord explicite de Fab.**

## ERRATUM KINETIC-DUST-IMPACT-V4 — 2026-10-05

La ligne d’artefact ci-dessus était une erreur de recopie documentaire et ne modifie ni le code ni la validation GREEN.

Métadonnée CI correcte pour le workflow `37241618498` / SHA gameplay `8f2ee5ed61372f2647ae7284abdf9a0df7d8ebbf` : artefact `SpaceFortressVs-1.4.0-release-files`, id `11317688487`, digest `sha256:3571fc53e4a6cb2ce8c0c5793bcccae219eaa0905cf509efa541a9cd44789cf0`. Le job `build-android` `111551304971` est `SUCCESS` et `publish-release` est `SKIPPED`.

## HALL GLOBAL — VALIDATION TÉLÉPHONE 2026-10-06

Fab a testé l’APK configuré de l’artifact `11374640381`. Preuve téléphone : l’écran Hall affiche `SYNC OK` et `GLOBAL 1 + LOCAL 0`; l’entrée globale est récupérée et affichée. Le chemin runtime configuré est donc validé sur téléphone. Aucun secret n’est recopié dans les mémoires.

## MINI-FIX SURCHARGE CINÉTIQUE V2 — 2026-10-06 — AUTORISÉ PAR FAB

Fab demande un ajustement borné avant merge/release :
- appui second doigt de 0 à moins de 0,30 s : comportement classique, aucun cercle irisé et puissance cinétique normale ×1 ; un relâchement court garde le tir historique ;
- de 0,30 s à moins de 2,00 s : cercle irisé visible et dissipation de surcharge ×2 ;
- à 2,00 s : état armé historique conservé, signal prêt et champ cinétique OFF jusqu’au relâchement ;
- relâchement armé : purge réelle ET vague visuelle portées à 3,0 diamètres de vaisseau, afin d’englober aussi les gros astéroïdes ; le rendement blanc 100 % de la purge et l’intangibilité de la poussière blanche restent inchangés ;
- le son de relâchement est ré-authored comme déflagration électromagnétique originale, empaquetée via un transport base64 validé au build ;
- même logique partagée en CLASSIQUE et COOP/campagne ; aucun changement Hall, Danger, campagne ou `src/main.cpp`.

TDD : RED prouvé par le workflow `37424970512` sur l’absence attendue du nouveau seuil `SF_KINETIC_SURGE_VISIBLE_DELAY_SECONDS`. GREEN complet Android + APK/AAB et validation téléphone restent obligatoires avant merge/release.

## AUTORISATION FINALE FAB — 2026-10-06 — MERGE + RELEASE

Après validation téléphone du mini-fix surcharge cinétique v2, Fab donne explicitement l’ordre : `Super, release et merge main`.

Cette autorisation lève le verrou merge/release pour le lot courant uniquement, sous les conditions suivantes :
- incrémenter la publication vers `1.4.1` / `versionCode 11`, car `v1.4.0` existe déjà et ne doit jamais être écrasée ;
- ne modifier aucun gameplay supplémentaire ;
- conserver `src/main.cpp` byte-for-byte inchangé ;
- exiger une CI fraîche GREEN du commit de préparation 1.4.1 ;
- merger la PR #5 vers `main` uniquement après cette preuve ;
- publier ensuite la Release `v1.4.1` avec uniquement les artefacts vérifiés du commit de publication.


## KINETIC-ENERGY-FATIGUE-DUEL-V142 — 2026-10-06 — ORDRE FAB

Fab demande :
- efficacité d'absorption cinétique décroissante avec l'énergie disponible, plus sévère que linéaire ;
- courbe canonique `pow(energyFraction, 1.20)` ;
- vagues visuellement plus lentes quand l'énergie baisse, sans ralentir la physique ;
- couleur équipe -> orange -> rouge, luminosité croissante ; sous 10 % d'énergie, rouge lumineux clignotant ~4,5 Hz ;
- surcharge 0,30 s / 2,00 s / blast 3× disponible aussi en CLASSIQUE DUEL pour les deux joueurs, via le moteur partagé, sans duplication ;
- sous 10 %, bouclier presque inefficace même en surcharge ;
- cible `v1.4.2` / versionCode 12 ;
- publication directe une fois le lot entièrement GREEN.

Protections :
- `src/main.cpp` byte-for-byte inchangé, blob canonique `835059a0ecfe0f74708068b3259cad5db1cdb579` ;
- Hall global, Danger 9, campagne 200, progression, sauvegardes et poussières hors périmètre ;
- TDD obligatoire, CI complète APK/AAB + secret hygiene avant merge/release ;
- ne jamais écraser v1.4.1.


## SHOT-DISPERSION-ENERGY-BOSS-FIELD-V143 — 2026-10-06 — ORDRE FAB

Constat téléphone confirmé par Fab :
- en CLASSIQUE et COOP, la dispersion des tirs ordinaires est devenue absente ou imperceptible ;
- les tirs doivent perdre en vitesse/efficacité et partir davantage au hasard à gauche/droite quand l'énergie baisse, puis rester rectilignes après le départ ;
- la COOP recharge passivement beaucoup trop vite ;
- Fab confirme que le CLASSIQUE possède historiquement une recharge passive lente dans `sprite::update()` (`nrj*=0.997`) et demande que la COOP retrouve un rythme lent comparable, sans supprimer cette recharge passive ;
- les poussières blanches restent une récupération active distincte ;
- en COOP/campagne, les boss doivent posséder un champ cinétique FIXE, moins puissant que celui des joueurs ;
- un astéroïde entrant dans le champ du boss doit subir le champ et infliger au boss les dégâts cinétiques résiduels ; aucun multiplicateur Danger sur ces dégâts ;
- champ boss proposé/canon du lot : dissipation fixe 55 %, donc inférieure au champ joueur plein (inner 78 %, outer 94 %) ;
- l'impact peut détruire l'astéroïde en poussière blanche selon le rendement cinétique existant, sans modifier les règles de collecte boss déjà livrées.

Protections :
- nouvelle branche `fix/shot-dispersion-energy-boss-field-v143` depuis `main` `7722d806...` ;
- TDD RED avant code ;
- `src/main.cpp` byte-for-byte inchangé, blob `835059a0ecfe0f74708068b3259cad5db1cdb579` ;
- Hall, Danger 9, campagne/progression/sauvegardes, surcharge 0,30/2 s et poussières hors points ci-dessus restent protégés ;
- aucun merge main ni Release de ce nouveau lot sans validation/ordre explicite de Fab.


## AVENANT FAB — CHAMP BOSS NON DESTRUCTIF / ASPIRATION / RECHARGE DANGER — 2026-10-07

Cet avenant REMPLACE les points incompatibles du lot SHOT-DISPERSION-ENERGY-BOSS-FIELD-V143 :
- le champ cinétique du boss reste fixe et moins puissant que celui des joueurs (55 %), mais il NE DETRUIT JAMAIS les astéroïdes ;
- il réduit leur impact, retire au boss uniquement le résiduel cinétique, puis ralentit/dévie l'astéroïde survivant ;
- les anneaux du boss doivent être nettement plus transparents que ceux des joueurs et partir visuellement du CENTRE du boss vers le rayon du champ ;
- pendant la charge joueur entre 0,30 s et 2,00 s, un astéroïde proche peut être miné en CONTINU : extraction moins forte que des tirs répétés, réduction progressive de taille, production de poussière blanche et aspiration efficace de cette poussière vers le vaisseau ;
- à READY 2 s, le champ joueur est OFF comme avant et le minage continu s'arrête ; le blast/purge au relâchement reste inchangé ;
- les astéroïdes de campagne/COOP doivent continuer à apparaître dans le temps comme dans le classique, pas seulement attendre que le stock tombe presque à zéro ;
- la recharge passive devient un élément important de survie et varie avec le Danger Boss : MOU DU GENOU = maximum, APOCALYPSE = minimum ;
- réglage borné retenu pour TDD : demi-vie de chaleur 4,0 s en MOU DU GENOU et 10,5 s en APOCALYPSE, interpolation monotone sur les 9 dangers. Ce réglage remplace la demi-vie provisoire 21 s et garde Apocalypse deux fois plus rapide que cette proposition précédente ;
- les poussières blanches restent une recharge active supplémentaire.
- dispersion initiale aléatoire CLASSIQUE + COOP du lot précédent reste demandée.

Protections inchangées : `src/main.cpp` strictement intact ; aucun merge/release sans validation Fab.


## ERRATUM RECHARGE DANGER — 2026-10-07 — DERNIER CANON FAB

Cet erratum remplace UNIQUEMENT les valeurs de demi-vie 4,0 s / 10,5 s écrites dans l'avenant précédent ; l'historique reste append-only.

Référence : la recharge COOP de v1.4.2 équivalait à nrj *= pow(.997, 60*dt).
Fab demande maintenant une recharge passive IMPORTANTE, inversement proportionnelle au Danger :
- MOU DU GENOU : cadence maximale = 4× la cadence v1.4.2, soit pow(.997, 240*dt) ;
- APOCALYPSE : cadence minimale mais encore 2× la cadence v1.4.2, soit pow(.997, 120*dt) ;
- les 7 niveaux intermédiaires interpolent monotoniquement entre ×4 et ×2.
Le sens de nrj reste historique : 0 = réserve pleine, 50 = épuisée.

Les autres règles du dernier avenant restent inchangées : boss 55 % non destructif, anneau transparent centre→extérieur, astéroïdes COOP continus, minage/aspiration blanche pendant 0,30–2,00 s.


## CLASSIC-FX-KINETIC-HUD-V143 — RED — 2026-10-07
- Nouvelle branche `feature/classic-fx-kinetic-hud-v143` depuis `898227ecd99d1dc1d3e3d287bfcc969dcaec8f90`.
- Fab autorise le codage : projectile guidé COOP aligné sur le missile classique et ses FX ; explosions classiques nettes plutôt que fumée quand le bouclier est faible ; orbes classiques réutilisées et redimensionnées en COOP ; HUD à trois barres ENERGIE / PV / CINETIQUE ; champ boss semi-transparent à anneaux centre→extérieur, plus visible quand sa réserve visuelle baisse, avec régénération plus rapide aux difficultés basses.
- Diagnostic du « broutage » : piste atlas/UV retenue de façon bornée. Test demandé sur une marge demi-texel anti-bleeding ; aucune retouche destructive des assets sans nouvelle preuve téléphone.
- La réserve cinétique boss ajoutée par ce lot est une réserve de stress/visibilité et ne remplace PAS la dissipation gameplay fixe 55 % déjà canonique.
- TDD RED : les tests référencent volontairement `sfPlayerKineticRect`, couleurs dédiées, réserve/régénération boss et `sfAtlasSafeUv` avant implémentation.
- `src/main.cpp` reste strictement intact. Aucun merge main ni Release sans validation Fab.


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
