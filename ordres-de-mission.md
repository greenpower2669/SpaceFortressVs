# SpaceFortressVs 1.4.x — ordre de mission validé par Fab

## IMMUTABLE / À PRÉSERVER

- Identité release/store historique : `com.greenpower2669.spacefortressvs` ; ne pas renommer pour une mise à jour.
- Diagnostic existant : suffixe `.diagnostic`, soit `com.greenpower2669.spacefortressvs.diagnostic`.
- APK diagnostic debug ; AAB release livré NON SIGNÉ. Configuration de signature inchangée ; aucune clé d’upload ajoutée au dépôt. Ne jamais copier de clé privée dans le dépôt ou les logs.
- `src/main.cpp` protégé. Aucun merge main ni Release sans validation explicite Fab.
- Livrer APK identifiable sous temp avec source CI verte exacte et SHA256 ; préserver les diagnostics précédents.


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


## ORDRE FAB — SOLO LABYRINTHE VISÉE VECTORIELLE 360° (2026-10-10)
Développer sur `feature/solo-worlds-tutorials-v1` une aventure immédiatement jouable et belle, non une mini-carte technique. La grande carte devient un vrai labyrinthe roche/lave/glace avec bifurcations et zones imposant l'exploration, parfois retour jusqu'au départ. Employez vaisseau et cinématique canonique des autres modes : réactivité intrinsèque identique. Le seul geste différent est la visée 360° : **vecteur position vaisseau → point tapoté ou maintenu**. Tap => tir, appui 2 s => cône cinétique tourné selon ce même vecteur. Défilement uniquement lié au mouvement/événements et non imposé. Lave, glace et poussière glacée : malus temporaires. Préserver CLASSIQUE/DUEL/COOP/VS IA/CAMPAGNE, scores/sauvegardes, debug exportable, et respecter le protocole FabCopilot : jamais de merge main ou Release sans validation explicite. Tests CI + APK Samsung obligatoire.


### Ordre validé Fab — SOLO zoom ×3 sans grossir le vaisseau (2026-10-11)
Sur la branche `feature/solo-worlds-tutorials-v1` : zoom des **décors/couloirs** ×3, pas des dimensions apparentes du vaisseau. Ne pas modifier les collisions en espace logique. Mise en place d'une caméra à zone morte : vaisseau se déplace librement dans le tiers central de chaque axe, la carte ne défile qu'une fois un des seuils du tiers dépassé, sans inertie ni saut, y compris en retour arrière. Stabiliser le pilotage tactile (ne pas faire rétroagir la caméra sur la cible de mouvement) et maintenir la sensibilité/vitesse apparente du vaisseau sans triplement dû au zoom. Préserver la visée/tirs/cône cinétique 360° et tous modes historiques. Tests automatiques puis APK DIAG directe, validation Samsung obligatoire, jamais main/Release sans autorisation.


### ORDRE VALIDÉ FAB — SOLO 2 rectangles de vitesse variable, au sein de chaque zone (2026-10-11)
Valider d'abord des périmètres **rectangulaires**, éventuelle transformation en zones circulaires ultérieure. Conserver la zone centrale fixe (entre 1/3 et 2/3 écran), puis **deux bandes de défilement chacune avec accélération graduelle en son sein**, sans paliers ni changements brusques : 0→55 % dans la bande 1, 55→100 % dans la bande 2, dernière marge 6 % de l'écran à pleine vitesse. Défilement du décor continuel lorsque le vaisseau maintient sa direction et navigue dans la bande extérieure ; arrêt et retour vers le centre = décélération fluide, jamais de modification du contrôle du vaisseau. Axes indépendants X/Y, diagonales, pas de caméra qui altère le doigt. Zoom décor ×3 et taille vaisseau inchangés ; préserver la visée/cône à 360°.
Exécution SOLO strictement sur `feature/solo-worlds-tutorials-v1`; CI #38121160188 verte au commit `cace6129`, APK DIAG directe pour test Samsung, pas de merge main, ni Release. Conserver séparément le **chantier des assets et FX historiques complets**, toujours à auditer et intégrer, notamment cône plasma.


### Exécution sur autorisation Fab — CAMERA-SHIP-ZONE-001 (2026-10-11)
L'ordre antérieur « deux rectangles progressifs » est précisé sans modification des seuils : ce sont les **coordonnées et la vélocité du vaisseau** qui déclenchent les gains de défilement, jamais la position du doigt. Lever le doigt maintient la propulsion/croisière ; si le vaisseau reste dans une bande extérieure le décor continue de défiler. Le retour en zone centrale (entre 1/3 et 2/3 de chaque axe) impose le zéro de défilement sans résidu même si croisière active. Demi-tour vers l'intérieur : ne pas entraîner la caméra dans le mauvais sens. Collision latérale en SOLO : conserver glissement sur axe libre sans traversée de rocher à haute vitesse. Préserver zoom carte ×3, taille sprite inchangée, tir/plasma/FX originaux, autres modes historiques, sauvegardes ; build APK DIAG puis test réel Samsung. Strictement `feature/solo-worlds-tutorials-v1`, pas main, ni Release, ni signature publique avant autorisation.


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


## 11 octobre 2026 — Ordre de mission SOLO-SCENERY-AUTOTILE-006
Demande explicite « Ok code ça » de Fab après sa capture montrant mauvais sens des parois et sprites collés. Corriger uniquement le rendu du mode SOLO : voisinage 4 directions, orientation, vrais sprites isolés/alpha propre, décoration classée et animation stable. Moteur dans `src/solo_scenery.hpp`, rendu SDL2 dans `src/solo_renderer.hpp`, tests `tests/solo_atlas_regressions.cpp`. Intégration sur `feature/solo-worlds-tutorials-v1`; protections : garder la géométrie circulaire de caméra, les valeurs énergie, tirs, cône, bouclier, physique et les autres modes. Pas de merge/main ni Release. CI de vérification `38154338078` source `7012a3b5`, test réel Samsung encore à réaliser.


## 2026-10-11 — ASTRA-COOP-SOLO-ARCH-001 — ordre Fab : audit avant implémentation

- **Demandeur : Fab.** Branche de référence `feature/solo-worlds-tutorials-v1`, snapshot audité `1d8a48e245a8a019759c1a109a6e21d15e11b770`.
- **Ordre intégral conservé séparément :** [mission reçue](docs/audits/2026-10-11-mission-astra-coop-solo.md). Ce texte fait référence pour les exigences ; le plan technique ne le remplace pas.
- Renverser la fondation : partir du véritable COOP, avec ses vaisseaux, armes, nrj, bouclier, cône, minage, poussières, sons, FX et IA historiques. Évaluer copie isolée / extraction / autre architecture, sans entretenir deux moteurs indépendants.
- Ajouter monde explorable, parois, rencontres progressives, miniboss et boss final éloigné ; conserver carte, relais, progression et sauvegardes SOLO. Conserver FabLib lorsque pertinente. Préserver les neuf difficultés historiques.
- Caméra confirmée : cercles concentriques, vitesses 0 / 0,1→2 / 2,1→6 / 6 ; vaisseau sous le doigt, pan continu doigt immobile, collisions prioritaires. Visée du cône à 360° par second doigt. Simulation en coordonnées monde ; caméra = projection.
- Contraintes : comportement public CLASSIQUE/COOP préservé, ni modification de main ni Release, compilation C++/Android et isolation des états ; aucune approximation des fonctions historiques. Tests/CI/APK diagnostic prévus **après autorisation**.
- **Périmètre autorisé maintenant : audit A–G et maintien des cinq mémoires. Implémentation non autorisée à ce stade.** La demande actuelle remplace la poursuite automatique des greffes SOLO antérieures ; leur code demeure disponible.
- [Audit et proposition](docs/audits/2026-10-11-coop-solo-architecture.md) : extraction progressive, façade historique et contexte par session ; premier jalon de parité en arène. **Proposition de l'agent, pas décision validée par Fab.**
- Travail consigné sur `audit/coop-solo-foundation-20261011`, uniquement documentation ; aucun changement de source, asset, workflow ou build. Les défauts observés dans le code ne sont pas déclarés corrigés.


## Mission approuvée SOLO-COOP-SOCLE-001 — 11 octobre 2026
Demandeur : Fab, ordre explicite « Ok code » après audit Astra. Fondation validée : extraction progressive du VRAI moteur COOP vers fonctions communes, façade legacy compatible et arène SOLO avec un pilote avant monde/caméra. Base documentaire `f4526cfa` ; branche `feature/solo-coop-core-v1`. Code progressif : `1b88bdef` (prédiction missile véritable), `c5dbf5c2` (pilotes actifs), `f48c697d` (boss absent), `d98bffa7` (test vrai COOP), `a413da20` (champ et minerais), `7b9dd1df` (troisième lanceur et arène native), `f1971dfb` (headless smoke), `172df0c0` (cône historique 360 directionnel). Livrable : première arène diagnostic COOP, caméra identité et boss déclenchable, aucune prétention labyrinthe intégré/contexts totalement extraits. CI Android en cours, test téléphone obligatoire. Garder `main`, `src/main.cpp`, Release et modes historiques inchangés. Docs ASTRA sous `docs/audits/` = preuve et contexte du jalon. Les demandes futures d'un monde/caméra ne changent pas la portée de la validation de ce premier jalon.


## 11 octobre 2026 — Correction écran diagnostic SOLO-COOP-PRESENT-002
Fab confirme l'écran est le menu de sélection CLASSIQUE, tandis que les sons de l'arène COOP jouent. Diagnostic confirmé par le journal `SpaceFortress-debug.txt`, sans plantage Android. Mission : corriger la véritable interception `SDL_RenderPresent` du routeur HOME pour la SEULE arène : `src/th2.h` sauvegarde/undef/restaure la macro autour de `coop_solo_arena_runtime.hpp`; `src/main.cpp` intact ; contrôle de compilation dans diagnostic/smoke. Code commit `b9072d24`, branche `feature/solo-coop-core-v1`, nouveau build `38167735625` à valider avant APK Samsung. Aucune publication ni merge main.


## SOLO-COOP-PRESENT-002 — livraison contrôlée après CI (11/10/2026)
- Correction code `b9072d24fd0b7452a88f8ec4cccb4b70e9cf51ae` : vrai appel `SDL_RenderPresent` exclusif à l'arène, routeur CLASSIQUE restauré pour modes historiques.
- Workflow Android `38167735625` entièrement VERT (SOLO, SDL2, regressions C++ historiques, compilation APK/AAB), aucune Release.
- Workflow extraction `38168138218` VERT : nouveaux binaires du commit corrigé dans `temp/` sur `feature/solo-coop-core-v1`, remplacement confirmé par manifeste.
- `temp/SpaceFortressVs-1.4.2.apk` : 94 119 741 octets, SHA256 `806da4277b599c7d1b19fb3c8a4ff45531bffe503475c8bb8d3b99f08d99546a`, package séparé `com.greenpower2669.spacefortressvs.diagnostic`.
- `temp/SpaceFortressVs-1.4.2-unsigned.aab` : 91 464 418 octets, SHA256 `d52c91ffb664165f0d9fd8f8d3aba1f3797a06384a6fd87835d5095c9840d10b`.
- **Attendre validation sur Samsung** : vraie arène COOP visible sans menu de sélection CLASSIQUE, avec sons, vaisseau, tir/cône et bouton boss. Pas de merge main ni publication.


## 11/10/2026 — Ordre Fab : son de charge + lancement deuxième phase d'Astra
Fab valide visuellement le nouveau SOLO/COOP après correction du menu ; signale que le son de charge du cône à 2 doigts ne démarre plus au bon seuil, le reste fonctionne. Demande de le corriger **avec** la deuxième phase architecturale. Seuils canon confirmés en code : activation 0.20s, READY 2.00s, fond discret puis déflagration sur purge. Code `74206425` isole le son local et réutilise vrai moteur sonore COOP ; tests audio ajoutés. Démarrage technique phase2 en `f5197d2b` puis `9927f014` : `coop_solo_world_bridge.hpp` reliant carte existante, projections, caméra et collisions swept, indépendamment de la largeur de combat. **Ne pas confondre tests bridge avec intégration monde visible/active**. Vérifier CI et APK Samsung, puis brancher au vrai COOP par lots sans duplication ni modification de `src/main.cpp`, merge main ou Release.


## 11/10/2026 — Validation CI et remise APK SON-003 / WORLD-004
- Build/test du code exact `9927f0148486eefb5f49a201b9fd0602151c1905` : GitHub Actions `38169721589` VERT, jobs `solo-core-regressions`, `solo-sdl2-ui`, `build-android` tous success ; `publish-release` skipped. 
- Décompression validée `38170223921` VERTE : `temp/SpaceFortressVs-1.4.2.apk` dans `feature/solo-coop-core-v1`, 94 119 734 octets, SHA256 `47c575b49877dc1af8bceaa966b8201978612edd794a9b7e17fe987726bfd33c`, package Android diagnostic séparé. AAB non signé SHA256 `635fbc4dfac186a27f51155e34123e90218491af80efb0cf8ae02bdf74279f92`.
- Le son de charge 0,20s / READY 2,0s est implémenté mais non encore écouté sur Samsung par Fab. Phase2 pont monde/collision/caméra testé isolément, PAS encore disponible dans l'arène interactive. Pas de merge `main`, aucun changement `src/main.cpp`, aucune Release.


## 11/10/2026 — Suite autorisée Fab : livrer MONDE TEST par moteur COOP existant
Instruction utilisateur « Parfait, continue maintenant », phase 2 préalablement auditée Astra. Implémentation incrémentale sur `feature/solo-coop-core-v1`: callback de collision du vrai COOP vers `CoopSoloWorldBridge` (aucun swap d'états globaux, C++ FabLib 1 TU), même `sfCoopTick/sfCoopDrawArena` avec translation caméra SDL uniquement sur rendu; arène historique témoignée inchangée ; MONDE TEST opt-in. Carte générateur existant, collision des parois pilote, astéroïdes historiques et tirs gardent leurs règles, boss désactivé jusqu'à rencontre future. Tests historiques + smoke arène et monde ; compilation Android CI `38171610964` à valider, APK puis téléphone. Limites ouvertes : arts vraies parois, obstruction shots/rocks par walls, mini/boss, progression/persistence, éventuelle unité vitesse caméra. Aucun merge main/Release ni modification de `src/main.cpp`.


## 2026-10-11 — WORLD TEST livraison Android vérifiée
- Lancement `feature/solo-coop-core-v1` : monde opt-in « MONDE TEST » derrière entrée diagnostique « SOLO : VRAI MOTEUR COOP » ; rendu/collisions/inertie pilotés par le VRAI moteur COOP; arène BOSS TEST conservée. Correctifs `bee803ee`, `3b52fca7`, `d86c1bfe`, `235661b3` (SDL_unsetenv absent dans SDL2, correction test uniquement) et `82825ed5` (levée doigt = vitesse SOLO remise à zéro, caméra figée).
- GitHub CI `38171993509` : TOUT VERT, tests isolated SOLO, SDL2, tests historiques de vrai COOP/collision paroi et smoke native 2 modes, compilation Android ; publish-release SKIPPED. Run précédent `38171610964` rouge UNIQUEMENT à cause du test `SDL_unsetenv`, corrigé. Ne pas effacer ses logs sans accord Fab.
- Artefact définitif extrait dans `temp/` par workflow VERT `38172484944`. APK diagnostic `temp/SpaceFortressVs-1.4.2.apk` commit source `82825ed5e2889ac630fe2379183bc068f0105d01` SHA256 `635c936e0b4fc1335077c2da9d2f236b92e69d8446761703012aa4b65754d5d2` (94 136 117 octets), identité Android séparée ; AAB NON signé SHA256 `14c0df866eba3f704912b1241a2bbbb066311691e8a1d6ad13dca0b110b06c9c`.
- Limites non résolues : absence de collisions projectiles/parois, boss déclenché par progression, relais et sauvegardes ; habillage parois diagnostique seulement. **Validation Samsung de la caméra, déplacements et sons est encore à effectuer**, CI verte ne la remplace pas.
- Aucun merge `main`, Release ni modification `src/main.cpp`.


## 11/10 — Retour visuel Fab à traiter : véritable vaisseau + lock tactile
Fab fournit photo du mode MONDE TEST. Il confirme défilement caméra ; observe « machin vert » susceptible d'être le vaisseau et le vaisseau qui se décroche du doigt. Priorité : faire apparaitre vrai vaisseau bleu canonique et empêcher le défilement caméra de prélever la totalité de la translation COOP avant d'avoir rejoint le premier doigt. Garder la mécanique 360, charge 0.2s/READY 2s, physique canonique, collision coque, boss seulement en ARENE, autre modes inchangés. Fix écran + projection `coop_solo_world_draw.hpp`, pan finger lock `coop_solo_world_bridge.hpp`, tests pixel `SDL_RenderReadPixels`, scénario rattrapage/maintien. Commits `09f8214c..1088c265`, CI en cours `38174175100`, livrer sous `temp/` uniquement après vert. Interdit merge main/Release ou src/main.cpp.


## 2026-10-11 — Livraison vérifiée ANNEAU/VISIBILITÉ PILOTE ET CAMÉRA DOIGT
- Correctifs de code `09f8214c`, `999aedb8`, `63911245`, `e4be8b58`, `ad24535f`, `1088c265` (vaisseau bleu historique rendu dans viewport écran et projeté depuis monde, cône/ring/dust/orbes/missiles réels réutilisés, fallback lumineux si PNG absent, caméra attend le rattrapage du doigt avant de faire défiler le monde, pas de pan sur mur, vraie physique COOP préservée).
- CI exacte `38174175100` **TOUS LES JOBS VERTS**, inclut `SDL_RenderReadPixels` sur centre du vaisseau en mode monde, rendu viewport écran, test de rattrapage du doigt et smoke vrai moteur, compilation APK/AAB. Run `publish-release` SKIPPED.
- Export GitHub `38174728231` **VERT**, fichiers DÉCOMPRESSÉS dans `temp/` sur `feature/solo-coop-core-v1`. Manifeste testé commit `1088c2657642b603fb85b5cba5e7e839152f9890`; APK `temp/SpaceFortressVs-1.4.2.apk` SHA256 `a23c9d0cc5e1222b703504ea35bbd5e824b6119dcfddc30eb6a799fe1f85d76d`, taille 94 136 121 octets, package `com.greenpower2669.spacefortressvs.diagnostic`. AAB non signé SHA256 `55e40cf1000a67975a6afb0433f5747d0e0210e4aedc936204d9800944f8c2fe`.
- **À faire sur Samsung** : installer le DIAG, MONDE TEST, identifier le vrai vaisseau BLEU sous l'anneau vert de supercharge, vérifier l'adhérence au doigt immobile pendant défilement, test rapide doigt → rattrapage physique, blocage mur; signaler toute dérive sur le screenshot. Vert CI ≠ validation ergonomique téléphone.
- Jamais de merge `main`, jamais de Release, `src/main.cpp` inchangé.


## 11/10/2026 — Fab signale « Oui le vaisseau est là mais le déplacement caméra a disparu cette fois »
Mission prioritaire : supprimer régression du gel caméra SANS réintroduire avatar absent ni modifier modes historiques. Audit montre cible COOP exactement sous doigt → vitesse 0 à alignement → pan0. Solution le vrai pilote avance grâce à une cible MONDE légèrement au-delà du doigt dans le sens de la zone concentrique (0.1/2/2.1/6), calibrée à vitesse voulue /30 de l'historique COOP. Caméra actualisée seulement par déplacement réel et excédent au-delà du doigt, mur bloque, doigt levé stop. Fichiers `coop_solo_world_bridge.hpp`, `coop_solo_arena_runtime.hpp`, vrais tests `restoration_regressions.hpp` et `tests/regressions.cpp`. Commits jusqu'à `68e39669`, run `38178296951` à valider avant APK. INTERDIT main merge/release et src/main.cpp. Vaisseau bleu/FX corrigés précédemment à conserver.


## Exécution terminée CAMERA-HOLD-007 (12/10/26)
Instruction Fab « Oui le vaisseau est là mais le déplacement camera à disparu cette fois ». Correction code `908749e` : en zone hors dead-zone, un doigt immobile devient un objectif physique COOP avancé dans la direction concentrique, calibré à la vitesse réelle `distance*30`; pan ne consomme que déplacement réel pour garder le pilote sous doigt. Tous autres canons préservés (pilot unique, vrai sprite, charge 0.20/2sec, murs, boss démo séparé). CI verte `38178801383`, APK diagnostique décompressée et vérifiée dans `temp/` par `38179331501`, SHA 372b1376... ; Samsung reste à confirmer. Interdictions main, release, `src/main.cpp` respectées.


## Ordre Fab — 12/10/2026 : SOLO WORLD CLEANUP-008
- Sur branche `feature/solo-coop-core-v1`, exclure totalement tourelles défensives et bonus hérités du COOP dans le SOLO exploratoire; les conserver intégralement dans CLASSIQUE/DUEL/COOP. Ne pas simplement les masquer: stopper IA, apparitions, collisions, effets et purger les états avant toute simulation monde.
- Ne pas modifier `src/main.cpp`, ni la caméra/maniabilité et charge 0.20s/READY 2s déjà validées. Corriger uniquement les tests véritablement erronés; précédent défaut de borne caméra résolu par scénario valide, assertions intactes.
- Tests historiques + SDL2 + Android obligatoires avant APK; extraction nouvelle APK/AAB vérifiée dans `temp/` après CI verte et conservation de l'ancienne APK. Aucun merge, aucune Release sans validation de Fab. État immédiat: code/test en vérification CI.


### Livraison sous contrôle Fab — SOLO CLEANUP-008
- Source 518f331, CI historique + SDL2 + Android verte #38182884348, aucun merge/main ni Release; export #38182981381 vert dans `temp/solo-world-cleanup-008/` sans remplacer l'ancien APK. AAB reste non signé. Validation réelle Samsung toujours requise avant toute évolution de gameplay.


## Exécution Fab — SOLO METEOR-009 / 12-10-2026
Instruction : « Ne sort pas le bonus ni les tourelle. Le bonus envoi des météorite ... sur le boss affaible et possibilité de recharger », précisée « Le bonus mais envoi meteor à la place » puis GO : interprétation explicitée/validée dans la discussion = ni bonus collectible ni tourelles en MONDE TEST, météorites réelles **à la place** avec minage/recharge et dégâts cinétiques du boss ; conserver en CLASSIQUE/DUEL/COOP. Dans WORLD, boss uniquement diagnostic stationnaire avec atlas actuel et bouton BOSS TEST/OFF : ne pas annoncer IA ni rencontres finalisées.
Code moteur `sfCoopSoloMeteorites`, mapping traversabilité `soloWorldCanOccupy`, boss test SDL2, test historique renforcé ; Android `sa1` liste (fix 2650237). CI complète 38190640721 verte et export exact 38190657369 verte dans `temp/solo-world-meteor-009/` ; ancienne APK conservée ; test Samsung restant. Aucun changement de main/src/main.cpp, merge ni release publique ; AAB non signé.


## Directive Fab — 12/10/2026 / PRIORITÉ STABILISATION DU SOLO
**Ordre explicite :** « Ne plus ajouter de nouveaux éléments de gameplay. Suspendre les nouvelles vagues de météorites dans le SOLO : Fab constate qu'elles restent plantées dans les murs. Ne pas présenter le contrôle de placement actuel comme validé sur téléphone. Examiner séparément leur apparition et leur déplacement contre les parois : apparaître dans un emplacement libre ne garantit pas une trajectoire correcte. Préserver le vaisseau, la caméra, le pilotage, les armes et les ressources déjà fonctionnels. Conserver intégralement les mécanismes des modes CLASSIQUE et COOP. Le boss stationnaire et ses commandes de test ne constituent pas la rencontre historique demandée. Ne pas annoncer cette intégration comme terminée. Consigner le retour Samsung dans les mémoires, en distinguant les symptômes observés des causes encore à vérifier. »
**Décision technique réversible** : inactivation stricte des vagues supplémentaires METEOR-009 par `SF_COOP_SOLO_METEOR_WAVES_ENABLED=false`. Conserver le vrai moteur COOP et ses astéroïdes de fond/dust/minage, ni retouche caméra/armes. Audit : point d'origine `soloWorldCanOccupy` ≠ garantie de trajet; `rock->ast()` ne gère pas la carte ROCK; spawner de fond également sans filtre de tuile. Pistes source seulement, non diagnostic confirmé sur Samsung. Tests négatifs activation/boss et témoin géométrique distinct, test Android/CI à valider avant APK.
**Restrictions** : pas d'ajout de gameplay, pas de merge main/release, pas d'édition `src/main.cpp`. Boss WORLD fixe = banc diagnostic, ne pas annoncer combat historique.


### Clôture technique de la suspension 010 (pas clôture de l'incident Android)
- Implémentation `d01f4d6` + test géométrique resserré `6540adece`, CI 38193630900 full green et export 38193705276 green. Diagnostic APK dans `temp/solo-world-stabilization-010/`, SHA b0cc1fad1543e4a66729a014bd6cb30f2222b9de7a6aa43c98c02dc9f4e36e64 ; APK METEOR-009 conservée. **Ne pas confondre suspension du nouveau générateur avec correction de la collision des rochers de base.** Premier scénario SDL2 de traversée fictive a montré un comportement à réexaminer du résolveur vaisseau, sans modifier la caméra/maniabilité. Retour réel Samsung à recontrôler.


## 12/10/2026 — Directive Fab SOLO TOURELLES OFF-011
Instruction : corriger tourelles défensives encore vues Samsung avec boss-test, sans nouveaux éléments de gameplay. Boss/météorites testés OK; boss visuel sombre reporté. Commencer par commit de l'APK, identifier objet et vrai chemin spawn/rendu; jamais seulement masquer la tourelle qui agit. Tester boss absent/présent/relaunch, historique CLASSIQUE/COOP, livrer APK identifiée après CI, mémoires des 5 fichiers ; jamais main, src/main.cpp, merge ni Release.
Audit : manifeste dernière APK GitHub STABILISATION-010 = source 6540adece, mais impossible de confirmer la version réellement installée sur téléphone. Source ARENE SOLO `soloDiagnostic=true` et `soloWorldMode=false` : gardes WORLD insuffisantes, `sfCoopDefences`, `sfCoopBonus` et `sfDrawTacticalEffects` historiques pouvaient produire/render des tourelles quand boss actif. Source MONDE déjà protégée, identification de l'objet réellement aperçu à confirmer. Fix `ad5cb08e` ajoute protection physique/rendu à `soloDiagnostic`, purge kind5, nettoyage relance/boss, repère HUD `SOLO-011`. Tests automatiques, CI 38209069899 à valider ; export nouveau temp séparé avec SHA stricte après vert. Météorites bonus nouvelles toujours suspendues; astéroïdes historiques minables/recharge OK; boss fixe outil diagnostic non rencontre finale. Ne pas extrapoler les essais Samsung à toutes trajectoires.


### SOLO-011 — validation technique, réserve matériel
- Code source `ad5cb08e` testé CI `38209069899` verte ; export isolé `38209723734` vert dans `temp/solo-turrets-off-011/`, APK SHA `513693f6b2b7fd07b810562fe8fc0c73e336eb3c2ebab02db980fc20106f0e21`, AAB non signé. Workflow d'export corrigé pour rebase après changement des cinq mémoires, sans force-push. Aucun merge, aucune release, `src/main.cpp` inchangé. Confirmation visuelle/matérielle téléphone encore nécessaire ; la cause prouvée en ARENE ne prouve pas quel objet exact était vu dans MONDE.


## 12/10/2026 — SOLO BONUS CORE-012 — autorisation Fab, livré à valider
Fab demande explicitement « coder ça en plus et joindre les deux mondes », huit pouvoirs SOLO : PV, énergie, furtivité 5 s avec coque semi-transparente/collisions conservées, missiles gratuits 3 s au tir humain, orbes Prison/Glace/Lave/Météore en salves guidées multiples. Drops différents aux paliers PV et défaite des boss, collecte contact accessible, HUD/FX distincts, règles de cumul bornées. Boss monde demeure stationnaire de test ; ne pas annoncer une démonstration mobile. Ressources/sprites historiques à réutiliser ; pas de tourelles/ancien bonus ni vagues METEOR-009. Vaisseau/caméra/armes/cône/sons et CLASSIQUE/DUEL/COOP protégés ; src/main.cpp interdit, ni merge main ni Release. Tests huit effets, combinaison, boss OFF/ON, labyrinthe/relaunch, CI Android et APK identifiable dans nouveau temp. Astra intègre ; Sol vérifie. [Contrat technique et paramètres agent](docs/superpowers/specs/2026-10-12-solo-bonus-core-012.md), [plan](docs/superpowers/plans/2026-10-12-solo-bonus-core-012.md). La nouvelle autorisation lève le gel de fonctionnalités pour cette brique seulement ; protection météores et tourelles maintenue. Statut : livré à valider sur Samsung ; preuves de livraison CORE-012 ci-dessous.

### CORE-012 — intégration et revue, 12/10/2026
- Brique C++ pure + adaptateur du vrai SfCoopState + rendu commun arène/labyrinthe implémentés. Salves humaines x3, guidage historique, coût canonique une fois ; furtivité semi-transparente conserve collisions ; missiles3s sans coût ; statuts Prison2s/Glace4s/Lave4s et Météore ciblé. Ancien bonus/tourelles et vagues METEOR-009 toujours exclus du SOLO.
- Revue indépendante : seuils enregistrés au moment du dégât avant soin poussière ; boss SOLO mort non ressuscité par poussières ; taille de coque identique au pilote monde ; BFS accessible calculé une fois par paquet, emplacements distincts. Coûts bornés et logs Android `SOLO_BONUS_012` moyenne/max temps simulation+rendu pour validation Samsung.
- Précontrôles : core pur GREEN, source SDL compilée ; régressions complètes/visuel/Android encore en cours à cet instant. Tests ajoutés par Sol puis poursuivis par Astra après interruption de son exécution. Benchmark hôte seulement : 12 récompenses/carte54×160, 12 lots, moyenne2,270ms maximum6,612ms ; aucune promesse de FPS Samsung.
- `src/main.cpp` inchangé. Pas de merge main/Release. Identifiant HUD SOLO-012 et carte accueil SOLO BONUS012. APK/AAB à livrer uniquement depuis CI verte exacte dans temp/solo-bonus-core-012/.

### CORE-012 — contrôle local terminé, candidat CI
`scripts/test-regressions.sh` termine code0 : historique C++/SDL2, assets, moteurs, 200 ouvertures/rendus COOP, champs et poussières cinétiques, source Android générée et smoke natif arène/monde. Nouveaux tests purecore, tirsréels/statuts/collisions/loot/isolation passent. Dernier ajustement HUD SOLO (plus de double HUD COOP, textes agrandis) recompilé et testé avec lecture pixels SDL ; capture inspectée. Contrôle pont monde réexécuté après ajout des24placementsdistincts ; protection launcher diagnostic OK. Le boss monde reste FIXE de test, Prison/Glace y montrent des statuts, mouvement seulement démontré sur boss mobile de l’arène. CI Android et export exact restent à constater ; APK non annoncée avant cela.


## CORE-012 — livraison vérifiée 13/10/2026
Statut CORE-012 : LIVRÉ, À VALIDER SUR SAMSUNG. Autorisation Fab exécutée pour cette brique, sans prétendre finir toute la migration SOLO. Boss monde reste de test ; pas de main/src/main.cpp, pas de merge main ni Release. Anciennes APK conservées.

Source `95743f2386b220bb692112197c4ddee02d27ae2b` ; [CI Android/C++/SDL2 verte](https://github.com/greenpower2669/SpaceFortressVs/actions/runs/38299079636), publication Release SKIPPED.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-012-95743f2.apk` : 94168883 octets, SHA256 `c61d7e39ecb075d69db863e7cc964021bb9c3ed31ccf0c0597b01b1ffaa09248`.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-012-95743f2-unsigned.aab` : 91525121 octets, SHA256 `a4433fc2f09cae480ab407f1997139821602caa71c0c0f8a61941baceeed6289`.

## 13/10/2026 — SOLO BONUS-013 — ordre Fab exécuté, validation Android ouverte
Autorisation : corriger CORE-012 sans `src/main.cpp`. Exigences : (1) variations légères uniquement pour explosions d’astéroïdes ; (2) relever PV/énergie joueur masqués par navigation Samsung, sans déplacer HUD boss ; (3) toggle TEST BONUS masquant/affichant 8 commandes + TOUS X8, chaque commande éjectant réellement depuis BOSS TEST, fermeture sans interrompre la partie.

Implémentation branche `feature/solo-coop-core-v1`, HEAD code avant mémoire `77a425310ad0ec9b17cffc4271fd6b6005086d83` : ejections boss temporisées, pickup non collectable en vol, destination WORLD accessible et segment sans ROCK, panneau test sans reset, safe-bottom HUD joueur SOLO, explosion astéroïde isolée sur canal mixer dédié avec `explo1/explo2` et faible variation de volume. Tests dédiés ajoutés. Tourelles SOLO et vagues METEOR-009 restent désactivées ; caméra/pilotage/minage/recharge/CORE-012 préservés ; CLASSIQUE/DUEL/COOP hors modification fonctionnelle. `src/main.cpp` blob canonique inchangé. C++/SDL2 déjà verts ; Android final reste à confirmer avant remise téléphone. Aucun merge main, aucune Release.

## BONUS-013 — livraison vérifiée 13/10/2026
Statut technique BONUS-013: livré en diagnostic à tester sur Samsung. `src/main.cpp` protégé; METEOR-009/tourelles SOLO restent OFF; aucun merge main ni Release.

Source `488de2195e2aa2a1e40f2eec96abaeba19863fad`; CI Android/C++/SDL2 `38361581767` SUCCESS; publish-release SKIPPED.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-013-488de21.apk`: 94185274 octets, SHA256 `4a3511c380d04b21b46593806e01d991e9320c44691c529c6022e60aa4d1a18b`.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-013-488de21-unsigned.aab`: 91532662 octets, SHA256 `3fb742b7b3964620b5f0b8eb76dae0d0fc2b18f2c2e245d87809b08468a68d3a`.

## BONUS-014 — livraison vérifiée 13/10/2026
Ordre Fab exécuté pour SOLO BONUS-014 : corriger l'ensemble des écarts audités sans modifier `src/main.cpp`, sans merge main ni Release. Atlas animé dérivé des sprites validés, effets offensifs réellement animés jusqu'au boss, HUD Samsung corrigé dans le rendu SOLO lui-même, explosions astéroïdes avec vraie variation de vitesse/hauteur, TEST BONUS complet et TOUS X8 atomique. Statut : livré techniquement, à valider sur Samsung.

Source `f9323f3cd239e9027d964e38db369a9217ef02cb`; CI Android/C++/SDL2 `38420099395` SUCCESS; publish-release SKIPPED.
- Atlas animé: `assets/pict/solo/SoloBonusAnimations.png`, 1536x768, 4x8 frames de 192 px, SHA256 `1a337adb15a6a3901b76da566dcd26a1d2d76c26893864a37a85e97b63950001`.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-014-f9323f3.apk`: 94989574 octets, SHA256 `0dfe83dbb1f2716e33442a2d2933356b1a6582180a85a9c2fed5a77b1ebe59ff`.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-014-f9323f3-unsigned.aab`: 92342299 octets, SHA256 `564b0c9427bd776ed93bcb32d3012d86d232f3354ffded5d85b020cfd7424965`.


## 14/10/2026 — SOLO GAME-015 — autorisation Fab, en cours
Fab valide l’audit de la branche `feature/solo-coop-core-v1` puis demande : « Ok, met l ordre de mission net les mémoires à jour et tu te sent de coder ça? ». Précision immédiate : plusieurs **miniboss sur les chemins du labyrinthe**, à implémenter dans cette mission, pas seulement à préparer pour plus tard.

Contrat demandé : une entrée SOLO dans le véritable sélecteur existant ; lancement direct du labyrinthe ; pilotage/caméra/astéroïdes/minage/énergie/cinétique/sons validés préservés. Plusieurs miniboss peu résistants, dangereux, à IA simple et lisible ; **aucun bonus ni seuil 75/50/25/mort pour les miniboss**. Antre identifiable, arrêt propre du pilotage, transition courte zoom/distorsion, puis arène dédiée utilisant le même cœur que BOSS TEST. Aucun contrôle de diagnostic dans le vrai jeu. Le boss final seul éjecte les récompenses BONUS-014 aux seuils 75/50/25 et à sa mort, avec collecte accessible et animations conservées.

Protections : `src/main.cpp` intact, CLASSIQUE/DUEL/COOP inchangés, tourelles SOLO et METEOR-009 toujours OFF. Aucune fusion main ni Release. Tests C++/SDL2, CI Android et nouvelle APK diagnostic identifiable dans `temp/solo-game-015/`. Le gel « audit seulement » est levé par cette autorisation ; les anciens incidents de parois restent documentés. Statut : implémentation autorisée, non livrée. Référence audit : `0c3f4a1118c7306025bcd8c8bf50495b0d82eba9`.

GAME-015 / étape 1 : sélecteur SOLO, activité interne `:sologame`, hôte commun et droits diagnostic séparés implémentés localement. Test ciblé RED (injection hors diagnostic acceptée auparavant) puis GREEN ; launcher statique GREEN. Suite globale arrivée aux tests cinétiques finaux sans erreur visible, statut de processus perdu lors de la reprise de session : une exécution finale avec code retour enregistré reste requise. Android et Samsung non encore validés.

GAME-015 : quatre miniboss effectivement intégrés sur le chemin accessible, sans récompense ; antre et arène commune codés. Transition et progression en vérification locale, pas encore livrés.

GAME-015 / suivi 15/10/2026 : plusieurs miniboss effectivement implémentés (quatre par parcours), zéro bonus de leur part. Entrée SOLO par le vrai sélecteur, diagnostic séparé, moteur final commun, transition et sauvegarde raccordés. Fab doit encore valider l’équilibrage et le rendu sur Samsung ; aucune validation physique ni nouvelle livraison Android annoncée avant preuve CI.

GAME-015 / validation locale finale 15/10/2026 : `bash scripts/test-regressions.sh` terminé avec code 0 ; tests ciblés de furtivité/contact et changement de scène verts. Revue indépendante finale sans défaut critique/important restant. `src/main.cpp` conserve le blob 835059a0ecfe0f74708068b3259cad5db1cdb579. CI Android et export restent à confirmer au SHA du commit envoyé. La modification locale ultérieure de ParoisModeSolo.png est séparée et exclue du commit GAME-015.

## GAME-015 — livraison vérifiée 15/10/2026
Ordre Fab GAME-015 exécuté sur feature/solo-coop-core-v1 : plusieurs miniboss maintenant inclus, aucune récompense de miniboss, vrai SOLO relié au sélecteur et au moteur final commun. Pas de merge main, pas de Release, pas de modification src/main.cpp. Livraison APK diagnostic identifiée et AAB non signé ; validation Samsung réservée à Fab.

Source `f5b4ce4f82e99057d041767d982914e903afbf7f`; CI Android/C++/SDL2 `38724287864` SUCCESS; publish-release SKIPPED.
- Atlas animé: `assets/pict/solo/SoloBonusAnimations.png`, 1536x768, 4x8 frames de 192 px, SHA256 `1a337adb15a6a3901b76da566dcd26a1d2d76c26893864a37a85e97b63950001`.
- `SpaceFortressVs-1.4.2-SOLO-GAME-015-f5b4ce4.apk`: 95023204 octets, SHA256 `f5138def32cdb8553e86006f6e079ce6a61a979b764c6c62f1757aca61ca8ec7`.
- `SpaceFortressVs-1.4.2-SOLO-GAME-015-f5b4ce4-unsigned.aab`: 92378949 octets, SHA256 `1b4d9a9bffb7ade770ec033bd25162c20c440c9a01db71ad8613c2156aee0e7c`.

## 16/10/2026 — SOLO PARALLAX-016
Fab autorise PARALLAX-016 : réutiliser ParoisModeSolo.png, EnvironnementsModeSolo.png et fond4hlow.png ; créer une seule planche minérale ajourée FondsParallaxeModeSolo.png ; raccorder le vrai SOLO GAME015 sans changer géométrie/collisions. Aucune modification src/main.cpp, aucun merge main ni Release ; tourelles SOLO et METEOR-009 OFF. Portée graphique uniquement : demandes d'astéroïdes immobiles, zone caméra lente et nouveaux comportements miniboss du précédent audit restent distinctes.

PARALLAX-016 — export préparé : source `2a933a60639d890fd6168befb8ce1784ba9a18c8`, CI `38837760766`. Le workflow attend obligatoirement SUCCESS et vérifie le SHA exact avant publication dans `temp/solo-parallax-016/`. CI/export pas encore déclarés terminés.

## PARALLAX-016 — livraison vérifiée 16/10/2026
Ordre graphique Fab PARALLAX-016 exécuté. Atlas existants et géométrie conservés ; vraie transparence et parallaxe indépendante. src/main.cpp inchangé, aucun merge main ni Release, tourelles SOLO et METEOR-009 OFF. APK diagnostic identifiée et AAB non signé exportés après CI verte.

Source `2a933a60639d890fd6168befb8ce1784ba9a18c8`; CI Android/C++/SDL2 `38837760766` SUCCESS; publish-release SKIPPED.
- Atlas animé: `assets/pict/solo/SoloBonusAnimations.png`, 1536x768, 4x8 frames de 192 px, SHA256 `1a337adb15a6a3901b76da566dcd26a1d2d76c26893864a37a85e97b63950001`.
- Fond ajouré : `assets/pict/solo/FondsParallaxeModeSolo.png`, RGBA1254x1254, SHA256 `4becaec70bf7f23d823c3eccbd902b86aa4dbb6942115a1389409bc6a7f7df44`.
- `SpaceFortressVs-1.4.2-SOLO-PARALLAX-016-2a933a6.apk`: 98400063 octets, SHA256 `62a029ca828a85783f324301bcb5d1411cfa8f343875f7e5ebf4108b5ce8515f`.
- `SpaceFortressVs-1.4.2-SOLO-PARALLAX-016-2a933a6-unsigned.aab`: 95691410 octets, SHA256 `1c18b7d99a45816a2332cd77415927cbf26cbb93cc1346897160c18eafc76703`.


## 16/10/2026 — SOLO PHYSICS-017
Ordre Fab autorise PHYSICS-017 complet : inertie réelle prioritaire + Danger9, caméra, gisements stables/météores antre, 10–12 miniboss et meutes, sons visibles uniquement, zoom volontaire 1..2 et maintien PARALLAX-016/gameplay. Contrat détaillé : docs/superpowers/specs/2026-10-16-solo-physics-017.md. Nouveaux sons reportés. src/main.cpp/CLASSIQUE/DUEL/COOP protégés, aucun merge/main/Release. APK diagnostic + AAB non signé temp/solo-physics-017 après CI verte ; Samsung requis. État : EN COURS, aucune livraison nouvelle.

PHYSICS-017 — point de reprise 16/10 après interruption : implémentation locale des briques (inertie/caméra/zoom, repos/collision roches, visibilité audio, 10–12 rencontres/chasse) présente, NON LIVRÉE. Tests purs inertie Danger9/FPS, contact/repos roches, zoom exclusif, navigation réelle/occlusion et placement des rencontres passés. Suite complète a révélé une erreur de portée de variable dans le son du minage, corrigée ; compilation globale/tests runtime à reprendre. Environnement /tmp réinitialisé entre les tours, dépendances SDL en reconstitution dans scratch persistant de la session. Aucun APK-017 annoncé, aucune validation Samsung. Prochaine étape : tests réels moteur, revue, CI Android, export exact.

## PHYSICS-017 — implémentation et revue, 16/10/2026
PHYSICS-017 en phase de validation : toutes les fonctions demandées codées dans le vrai SOLO, sans modifier src/main.cpp, les assets ou les fichiers audio. Nouvelles familles sonores toujours reportées. Validation technique finale puis APK diagnostic + AAB non signé via export GitHub autorisé ; Samsung et accord Fab restent nécessaires avant main/Release.

## PHYSICS-017 — livraison vérifiée 16/10/2026
Ordre complet Fab PHYSICS-017 exécuté et diagnostic exporté après validation technique exacte. src/main.cpp, assets et audio conservés ; modes historiques protégés ; METEOR-009 reste OFF. Aucun merge main ni Release. Intégration définitive seulement après essai Samsung et accord explicite de Fab. Nouveaux bruitages reportés.

Source `3ef362cd1375034519c458a0effb2d7abef03490`; CI Android/C++/SDL2 `38956636032` SUCCESS; publish-release SKIPPED.
- Atlas animé: `assets/pict/solo/SoloBonusAnimations.png`, 1536x768, 4x8 frames de 192 px, SHA256 `1a337adb15a6a3901b76da566dcd26a1d2d76c26893864a37a85e97b63950001`.
- Fond ajouré : `assets/pict/solo/FondsParallaxeModeSolo.png`, RGBA1254x1254, SHA256 `4becaec70bf7f23d823c3eccbd902b86aa4dbb6942115a1389409bc6a7f7df44`.
- `SpaceFortressVs-1.4.2-SOLO-PHYSICS-017-3ef362c.apk`: 98432836 octets, SHA256 `8387c28fb63ae6d2fe4017af5ac3b29b49c906682e5124c8273031d6cebb2ac9`.
- `SpaceFortressVs-1.4.2-SOLO-PHYSICS-017-3ef362c-unsigned.aab`: 95725985 octets, SHA256 `3af4730f3104606e4e6f3af694baa3165a9cc3447174705c4553b72d6736b95f`.


## 16/10/2026 — SOLO BOUNCE-PINCH-018 — feu vert explicite Fab
Après audit du ressenti PHYSICS-017 sur Samsung, Fab dit « Ok corrige ». Portée exacte : conserver inertie excellente, empêcher le vaisseau de coller longtemps aux parois et rendre le rebond immédiat, conserver pilotabilité vers l'extérieur, régler la première pression tactile : uniquement sur la coque pour initier mouvement ; zoom pinch-in/out direct à deux doigts hors coque, sans vols de doigt/tir intempestif. Préserver mode SOLO et anciens modes, caméra, parallax, armes, assets/sounds et src/main.cpp ; pas de merge main ni de Release. Livraison distincte attendue : APK Android diagnostic et AAB non signé après CI verte, validation Samsung ensuite. Code initial `8875905`, CI `38962472820` lancée ; aucune validation inventée. Doc `docs/solo-bounce-pinch-018.md`.

SOLO-018 / suivi CI : le test antérieur d'arrêt `vx==0` avait été remplacé par une attente de recul de signe négatif même avec vitesse injectée `100000 px/s`. Ce nouveau test était trop strict pour des contacts multiples et a fait échouer `38962472820` ; seule son assertion a été rectifiée au commit `a85ca0d`, avec vérification mono-choc déjà dans bridge. CI 2 `38963255438` attendue, aucune livraison déclarée verte à ce stade.

## SOLO-018 — export vérifié
SOLO-018 : CI exacte 38963255438 SUCCESS, code a85ca0dbd8e94e463d11e42b5b2cefb32ab31116. Export vérifié dans temp/solo-bounce-pinch-018. APK diagnostic + AAB non signé. Rebond et touch/zoom restent à valider sur le Samsung de Fab ; pas de main/Release.


## 16/10/2026 — SOLO-019 — instruction corrigée par Fab
Fab demande des barrières si mini-boss vivant et des commandes de tir joystick + huit boutons directionnels (tous les 45°) et inversion pour droitier via options SOLO. **Rectification explicite immédiate : « Pas l'antre, les portions de chaque mini boss ou ordes... ».** C'est l'autorité sur l'ordre de mission : verrouiller les portions de combat activées de mini-boss isolés ou hordes entières jusqu'à leur défaite, aucune condition sur l'antre. Joystick bas gauche semi-transparent, geste vectorisé puis relâché déclenchant le tir ; tap seul réutilise dernier vecteur ; huit petits boutons directs, permutation bas droite par menu et préférence locale. Code candidat SOLO `568d689`; CI `38969053715` à vérifier ; build APK et AAB seulement sur résultat vert ; `src/main.cpp`, CLASSIQUE, COOP, DUEL, main, Release intouchés.

## SOLO-020 — ordre de Fab, 16 octobre 2026
« Parfait une zone moitié plus grande sera mieux, je veux une zone supplémentaire de déplacement par defilement aussi entre le presque centre 2 véseau de diamètre et le début de la zone actuel et en progressif aussi ». Appliquer x1,5 au rayon des seules zones de mini-boss/hordes, pas à l'antre final. Créer une bande de croisière progressive du centre (~deux diamètres du vaisseau) vers le début des zones externes, en gardant l'inertie/rebond, vitesse extérieure, pilotage doigt-sur-coque, pince et tir à huit directions. Sur branche feature/solo-coop-core-v1 uniquement, code dd6a4c7, tests CI en cours ; aucune fusion main ni Release. Export diagnostic direct après CI verte.

## SOLO-020 — livraison CI + dépôt direct vérifiés
- Code source `dd6a4c75066fd51a359c00bbd952a32674eaaa25` ; Android/C++/SDL2 CI `38973185112` **SUCCESS**.
- Export GitHub `38973239202` **SUCCESS**, fichiers réels dans `temp/solo-cruise-020/` : APK diagnostic 98 449 216 octets, SHA-256 `ad683345d31f6857cc90589557905931a4076634ea69781bc636e8887ddfffb0` ; AAB non signé 95 742 279 octets, SHA-256 `16dfdbc3948a8d2689aa43d0320ee1a1f1f09b82e3e5b388fa59749f4bfccbfa`. Sommes vérifiées par workflow. Pas de main ni de Release.
- **Reste :** validation tactile réelle Samsung (défilement proche centre, x1.5 zones, dégâts/barrières et joystick).

## SOLO-021 — mission de netteté des couloirs — 17 octobre 2026

**Ordre de Fab :** « Tu sauras améliorer les textures des couloirs l'arrière plan est net mais pas l'avant plan ? » puis « Tente toi même en laissant la trace de ta modif dans ordre de mission comme ça on pourra revenir en arrière facilement ». Autorisation d'essayer une correction visuelle **réversible sur la seule branche** `feature/solo-coop-core-v1`, sans fusion `main` ni Release.

**Constat dans le code :** `src/coop_solo_scenery.hpp` charge la bande opaque de `ParoisModeSolo.png` coupée à 200×58 px. Le motif était projeté sur 4,5 cellules × 2 cellules (pour une cellule de 90 px à l'écran de référence, 405×180 px), puis échantillonné en `SDL_ScaleModeLinear`. Le relief était encore atténué par huit marches de biseau sur 15 % de la cellule. Le fond `fond4hlow.png` et `FondsParallaxeModeSolo.png` utilise d'autres textures / plans, conservés.

**Correction expérimentale appliquée, commit unique exact `f896211164481cede61d0073729f6eb3362be177` :**
- `src/coop_solo_scenery.hpp` : répétition du matériau mural passée de `4.5 × 2.0` cellules à `2.0 × 0.65` cellule ; résolution affichée du motif ~180×58,5 px au zoom 1, donc proche des 200×58 px source. Ancrage au monde, symétrie répétitive et collision identiques.
- Pour **ces six textures opaques de murs seulement** : `SDL_ScaleModeLinear` -> `SDL_ScaleModeNearest` ; les plans de parallaxe et les décors gardent leur filtrage actuel. Biseau de `8 marches/15 %` à `4 marches/10 %` de cellule pour des contours plus lisibles.
- `tests/coop_solo_scenery_regressions.cpp` : contrôle des six échantillonnages de murs, des deux filtres linéaires de fond, de la taille de source et de la densité du motif. Tous les autres tests du fichier (carte intacte, continuité du motif, caméra, compositing, transitions) sont conservés.
- Aucun fichier PNG modifié, aucun changement de moteur physique / inertie / parois de collision / mini-boss / tir / zoom / CLASSIQUE / COOP / DUEL ou `src/main.cpp`.

**Point de retour incontestable :** `cf6cc6e479ce854c3e64ddb00f6b9566d9cf52a9`, tête du dépôt SOLO **juste avant** cette correction.

**Procédure de retour arrière sûre :** si rendu trop pixellisé ou motif trop répétitif sur le Samsung, demander validation de retour arrière, puis **sur la branche SOLO uniquement** exécuter `git revert f896211164481cede61d0073729f6eb3362be177` (restaure uniquement le renderer et son test), relancer CI. **Ne pas** faire de `git reset --hard` ni force-push : cela supprimerait d'autres évolutions. La trace dans le présent ordre de mission doit rester même après revert. Si des corrections ultérieures ont modifié ces deux fichiers, vérifier le diff et résoudre localement avant d'appliquer le revert.

**État :** candidat à la validation CI et Samsung ; qualité visuelle améliorée attendue, mais aucun résultat perceptif affirmé avant le test sur téléphone.

### SOLO-021 — complément de traçabilité, suite de la CI
La première compilation complète `39018857560` a découvert une **erreur de nom dans le test ajouté** : `SDL_ScaleMode mode` entrait en collision avec un `SDL_BlendMode mode` existant. Ce n'était **pas** un défaut du rendu. Petit correctif exclusivement dans le test : commit `04da08db80f543a081a7a3614f85c1b2fd28e7c1`, qui renomme la variable en `scaleMode`. Nouvelle CI ciblant ce commit : `39019113903`. L'export direct SOLO-021 doit être lié strictement à ce nouveau SHA, et **pas** à l'échec initial.

**Retour arrière complet de l'essai SOLO-021** (après accord de Fab) : annuler **les deux commits** sur `feature/solo-coop-core-v1`, dans cet ordre : `git revert 04da08db80f543a081a7a3614f85c1b2fd28e7c1` puis `git revert f896211164481cede61d0073729f6eb3362be177`. Le premier restaure le test intermédiaire et le second annule à la fois renderer/test ; conserver les mémoires et cette note. Vérifier `git diff` et relancer CI. Ne jamais réinitialiser ni force-pusher la branche, ne jamais fusionner `main` sans validation.


### SOLO-021 — résultat définitif (17/10/2026)
- **Source exécutée** : `04da08db80f543a081a7a3614f85c1b2fd28e7c1`, code de rendu `f896211164481cede61d0073729f6eb3362be177`, état de référence précédant l'essai `cf6cc6e479ce854c3e64ddb00f6b9566d9cf52a9`.
- **CI verte** : GitHub Actions `39019113903` (logiciel SOLO, SDL2, Android), toutes les tâches requises SUCCESS, publication Release SKIPPED.
- **Export GitHub direct vert** : run `39019145459`, sous `temp/solo-wall-crisp-021/`.
- **APK de diagnostic** : `SpaceFortressVs-1.4.2-SOLO-WALL-CRISP-021-04da08d.apk`, 98 449 218 octets, SHA-256 `5a58885b9a76e52e4c88ceeffd044c5b0862ef421b0a4b4a8848b0dbf38cea43`.
- **AAB non signé** : `SpaceFortressVs-1.4.2-SOLO-WALL-CRISP-021-04da08d-unsigned.aab`, 95 742 237 octets, SHA-256 `3ae61f0554464c29cec1a0640ad7cc922ba7c047a18e6fae3a04412b4085d4fc`.
- **Comparaison à faire sur Samsung** : SOLO-020 avant ↔ SOLO-021 après, à zoom 1 et 2 et en mouvement ; noter netteté, effet pixel, raccord et répétition du rocher. Aucun résultat de qualité visuelle vécu ne peut être attribué au téléphone sans test réel.
- **Retour arrière sans perte de travail** : `git revert 04da08db80f543a081a7a3614f85c1b2fd28e7c1` puis `git revert f896211164481cede61d0073729f6eb3362be177` sur feature/solo-coop-core-v1, suivi de tests. Garder cette trace dans ordres-de-mission. Pas de `main` ni Release.

## SOLO-022 — ajout d'une zone de défilement ultra-lent, 17 octobre 2026

**Ordre explicite :** Fab a demandé « une zone de défilement arrière supplémentaire, de là où elle commence à centre plus un diamètre de vaisseau », **sans code** au premier échange. Ensuite, il a dit « Ok, go » : autorisation de coder et tester ce périmètre précis, pas de toucher aux autres modes.

**Référence avant SOLO-022 :** commit `9e064b552a6a7bead00817fb4bed04f4dd0d5288`, branche `feature/solo-coop-core-v1`. Le cumul existant inclut les correctifs SOLO-018 à 021, notamment les parois plus nettes. Toutes ces améliorations doivent rester.

**Contrat de cette correction :**
- Garder le centre précis sans croisière automatique jusqu'à **un diamètre** de vaisseau (distance radiale depuis le centre de l'écran).
- Ajouter une **nouvelle bande de défilement** entre **1 et 2 diamètres** : accélération `smoothstep` de 0 à 0,025 unité de vitesse radiale.
- Au seuil historique de **2 diamètres**, pas de décélération, pas de retour à zéro : deuxième interpolation `smoothstep` de 0,025 à 0,1 jusqu'à 65 % du cercle extérieur. Ensuite la courbe existante reste : 0,1→2 à 80 %, puis 2→6 au bord extérieur, sans changer la vitesse max.
- Même fonction `soloSoftCirclePan` pour l'intention du doigt tenu et le défilement réel de caméra ; pilotage acquis uniquement depuis la coque, pince/zoom, tir 8 directions, inertie, rebonds, collisions et minage inchangés. Ne modifier ni les PNG, ni les autres modes.
- Fonction `soloCirclePan` du prototype SOLO historique inchangée ; seuls `soloSoftCircleZoneSpeed` et ses seuils COOP-SOLO sont concernés.
- Tests de monotonie, continuité des quatre frontières, symétrie huit directions, mouvement caméra limité au déplacement physique et effet doigt tenu, écran 540×960, sans changer la génération de niveau.

**Retour arrière (avant exécution)** : une seule modification de source et de tests sera regroupée en un commit dédié (SHA à inscrire après création). Pour annuler SOLO-022 sans perdre SOLO-021, faire uniquement `git revert <SHA_DU_COMMIT_CODE_SOLO_022>` sur la branche SOLO après validation, puis relancer la CI. Ne jamais faire `reset --hard`, force-push, merge main, ni Release sans validation.


### SOLO-022 — commit source exact et retour arrière
Correction et tests regroupés dans **`6c79f88addd673c4be8a8efc853c091ee94ae675`** ; base `9e064b552a6a7bead00817fb4bed04f4dd0d5288`. Revenir au défilement SOLO-021 **sans retirer ses parois nettes** : sur `feature/solo-coop-core-v1`, contrôler le diff puis lancer `git revert 6c79f88addd673c4be8a8efc853c091ee94ae675`, résoudre tout conflit éventuel, relancer toute la CI. Les archives binaires et cette trace documentaire demeurent consultables. Ne pas utiliser `git reset --hard`, ni force-push, ni toucher main ou Release.

Workflow de vérification du code : GitHub Actions run `39040122861`. Export direct (si réussite intégrale du run exact) : `temp/solo-inner-cruise-022/` via workflow `.github/workflows/solo-inner-cruise-022-export.yml` (run `39040168281`). Avant résultats finaux, ne pas affirmer que l'APK existe ni que la CI est verte.

### SOLO-022 — validation finale et téléchargement direct — 17/10/2026
- Commit source exact `6c79f88addd673c4be8a8efc853c091ee94ae675`; CI `39040122861` **SUCCESS** pour C++/SDL2/Android, Release **SKIPPED**.
- Export GitHub `39040168281` **SUCCESS**. Fichiers réellement présents dans `temp/solo-inner-cruise-022/`, sommes SHA256 contrôlées par workflow et manifest.
- APK de diagnostic installable : `SpaceFortressVs-1.4.2-SOLO-INNER-CRUISE-022-6c79f88.apk`, 98 449 221 octets, SHA256 `540fdae8cc278550622bcfa4ba11d9ae7eccb774111a529bbc4365118b84cab8`.
- AAB non signé, pas destiné à Google Play : `SpaceFortressVs-1.4.2-SOLO-INNER-CRUISE-022-6c79f88-unsigned.aab`, 95 742 235 octets, SHA256 `130ab9ad27041aeb7711f86518a3b368063e1aab9c07902a220430c70b93b4ae`.
- La fonction de défilement démarre désormais à un diamètre, atteint 0,025 à deux diamètres, reste monotone jusqu'aux zones précédentes. Toujours aucune modification physique, des textures ou des modes historiques.
- **À valider par Fab sur Samsung** : sensation de lenteur à la nouvelle bande, absence d'arrêt à deux diamètres, cohabitation avec touches de tir et pincement. Retour arrière isolé : `git revert 6c79f88addd673c4be8a8efc853c091ee94ae675` sur la branche SOLO en conservant cet ordre de mission. Ni main, ni Release.


### SOLO-022 — résultat final vérifié, 17/10/2026
CI source **39040122861 SUCCESS** (Android + C++ + SDL2), export direct **39040168281 SUCCESS**. APK 98 449 221 octets sous `temp/solo-inner-cruise-022/SpaceFortressVs-1.4.2-SOLO-INNER-CRUISE-022-6c79f88.apk`, SHA-256 `540fdae8cc278550622bcfa4ba11d9ae7eccb774111a529bbc4365118b84cab8`. AAB non signé disponible au même endroit, 95 742 235 octets, SHA-256 `130ab9ad27041aeb7711f86518a3b368063e1aab9c07902a220430c70b93b4ae`. Tests Samsung de comportement réel non encore faits. Main et Release intacts. Revert isolé `git revert 6c79f88addd673c4be8a8efc853c091ee94ae675` sur la branche SOLO, sans reset ni force-push.

## SOLO-023 — suppression visuelle des rebords carrés (17 octobre 2026)

**Ordre de Fab :** « Code pas il faut habiller des rebord je ne veux plus de carré crée des planche d'habillage pour régler le pb dans toutes les textures map déjà créé », puis autorisation explicite « Ok code ça ».

**Référence avant l'essai :** `13e212927c3ef63e3b7f431e71cbee81b0b2a2f8`, branche `feature/solo-coop-core-v1`. SOLO-022, la netteté des parois SOLO-021 et tous les modes historiques restent intactes.

**Implementation visuelle :**
- Générateur déterministe versionné `scripts/generate-solo-edge-atlas.py` (uniquement bibliothèque standard Python), produisant une vraie planche PNG RGBA `assets/pict/solo/HabillageRebordsModeSolo.png` de **1536×512**, **48 pièces 128×128** : 16 combinaisons nord/est/sud/ouest de voisins Rock × 3 variations. Auto-traitement en couleurs conservant l'art des six matériaux historiques de `ParoisModeSolo.png`; alpha binaire 0 ou 255, géométrie érodée à bord dentelé et angles arrondis. Aucune texture d'origine remplacée.
- Un workflow de génération dédié, `.github/workflows/solo-023-build-edge-atlas.yml`, a créé et sauvegardé le PNG directement sur la branche SOLO (pas besoin de décompresser un ZIP). Source de construction `923cacb1e137fd12634afda5f533bfb849c47845`, asset ensuite présent sur la branche.
- `src/solo_edge_sprites.hpp` : charge la planche + le matériau source une seule fois, compose des textures de mur en fonction des 6 thèmes, 16 configurations de raccord, 3 variantes ; libération/recharge SDL intactes ; rendu triangulé à limites cohérentes en zoom fractionnaire.
- `src/coop_solo_scenery.hpp` : rendu des cellules **Rock** en silhouettes habillées seulement, selon les voisins réels. Les autres types de cellules, les fonds et le moteur des collisions restent inchangés ; ancien rendu conservé en secours si atlas manquant.
- `tests/coop_solo_scenery_regressions.cpp` : présence et taille des 288 textures thématiques, alpha réel d'un coin isolé, cœur opaque, double passage stable, plus les anciens tests de géométrie, défilement et ressources.

**Aucun changement de règles :** pas de modification de map, passages, collisions, ennemis, minage, caméra, champ de force, inertie, visée, textures fond, CLASSIQUE/DUEL/COOP ou `src/main.cpp`. Visuellement la roche se rétracte sur les bords de cellule ; le contour de collision demeure celui d'avant, donc vérifier la sensation de proximité et les angles sur Samsung.

**Retour arrière :** un commit de code principal sera référencé ci-après. Annuler seulement ce commit par `git revert <SHA_CODE_SOLO_023>` sur la branche feature, conserver la trace et le générateur/atlas comme historique réutilisable, et exécuter les tests. Aucun `git reset --hard`, force-push, merge main ou Release sans validation.



### SOLO-023 — commit exact du rendu et rollback indépendant
- Baseline **avant création de l'atlas** : `13e212927c3ef63e3b7f431e71cbee81b0b2a2f8`.
- Générateur/workflow (purement nouveau, aucune ancienne texture changée) : `923cacb1e137fd12634afda5f533bfb849c47845`.
- PNG généré et déposé par workflow **39057218668 SUCCESS** ; GitHub indique le fichier `assets/pict/solo/HabillageRebordsModeSolo.png` présent, blob `d667571e5d1c6cf816f0647029d5bff5a06ab353`, 260834 octets.
- **Commit exact de l'habillage logiciel** : `39d0a7c2b6969f546a7dc7537b3bcf993b097dcb`. Contient `src/solo_edge_sprites.hpp`, adaptation `src/coop_solo_scenery.hpp`, nouveaux tests et première trace documentaire. Toute la logique est localisée dans la vraie scène SOLO, les six thèmes et toutes les maps partageant ce renderer.
- Si Fab trouve l'habillage trop artificiel/pixelisé, **annuler le code sans perdre l'atlas ni les améliorations antérieures** par `git revert 39d0a7c2b6969f546a7dc7537b3bcf993b097dcb` sur `feature/solo-coop-core-v1`, résoudre d'éventuels conflits dans les documents à la main, conserver CETTE note historique, relancer la CI. Après cet unique revert, l'ancien affichage revient automatiquement ; supprimer l'atlas et le générateur **seulement** après ordre explicite, par commits séparés. Aucun reset/force-push, merge main ni Release.
- CI source `39057515808` et export `temp/solo-rock-rims-023/` via `solo-rock-rims-023-export.yml` ; vérification verte nécessaire avant de communiquer un lien APK direct.


### SOLO-023 — correction du test en CI et rollback intégral (complément)
Le CI Android initial `39057515808` a échoué dans l'étape d'évaluation de régressions **à cause d'un nom de variable dans le nouveau test** : `const auto red=pixels(r)` masquait le `Uint8 red` historique plus bas. Aucun problème de compilation connu dans le moteur de rebords. Correctif limité à `tests/coop_solo_scenery_regressions.cpp`, commit **`51752da4dd74fffc9b8c0a1448b2d6eb27fd3fcc`** (renomme en `redPixels` et `greenPixels`). Nouveau CI source **`39057829358`**, export conditionnel repointé sur ce SHA exact.

**Retour arrière complet SOLO-023** : après validation de Fab, sur `feature/solo-coop-core-v1` et dans cet ordre **`git revert 51752da4dd74fffc9b8c0a1448b2d6eb27fd3fcc`**, puis **`git revert 39d0a7c2b6969f546a7dc7537b3bcf993b097dcb`**, avec résolution éventuelle des conflits de documentation et relance CI. Les fichiers générateur et PNG peuvent être conservés sans utilisation par le moteur ; les cinq mémoires et cet historique ne doivent pas être supprimés. Ne jamais force-push / reset / main / Release.


### SOLO-023 — correction de la régression opacité (ne pas perdre cette trace)
- Suite d'essais : code `39d0a7c2b6969f546a7dc7537b3bcf993b097dcb`, premier correctif du test `51752da4dd74fffc9b8c0a1448b2d6eb27fd3fcc`, **second correctif test `57f74d8c4c7437d01cf84d7edea200a57bf4e672`**. CI source épinglé `39058137853`.
- Le build `39057829358` passait C++/SDL2 mais le test historique `pink==green` sur **l'intégralité du rectangle de roche** a échoué. C'est cohérent avec l'alpha transparent volontaire des bordures irrégulières : les coins ne doivent plus être opaques/carrés. Le nouveau test vérifie opacité indépendante du fond sur le **cœur de la roche**, puis transparence réelle des coins sur deux fonds colorés, tous zooms/échelles. La collision Rock reste intacte.
- **Rollback intégral logiciel SOLO-023, après accord Fab, sans toucher les autres versions** : dans l'ordre `git revert 57f74d8c4c7437d01cf84d7edea200a57bf4e672`, puis `git revert 51752da4dd74fffc9b8c0a1448b2d6eb27fd3fcc`, puis `git revert 39d0a7c2b6969f546a7dc7537b3bcf993b097dcb` sur `feature/solo-coop-core-v1`, avec éventuelle résolution des conflits de documents, puis régressions complètes. Conserver la planche `HabillageRebordsModeSolo.png` et son générateur comme assets historisés, à supprimer séparément uniquement si demandé. Conserver cette note et les mémoires. Jamais de reset/force push/main/Release sans validation.


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


### SOLO-023 — livraison définitive vérifiée, 17/10/2026
- Référence source code/graphisme validée : `48b30c4a6d7c200fc2979bb411a3a048b4a3348f` (code du rendu `39d0a7c2`, tests corrigés `51752da4`, `57f74d8c`, `48b30c4`). Générateur et planche réellement présents sur `feature/solo-coop-core-v1`, 48 sprites en atlas.
- Tests CI **`39058401438` SUCCESS** : C++/SDL2/Android ; Release volontairement SKIPPED. Export direct **`39058427941` SUCCESS**.
- Fichier réellement créé sur GitHub, APK installable `temp/solo-rock-rims-023/SpaceFortressVs-1.4.2-SOLO-ROCK-RIMS-023-48b30c4.apk`, **98 710 354 octets**, SHA-256 `8ef3769b08a200f382465907486f28c36d54b300294fa17fdafcbb8c0cdb0e71`. AAB non signé `SpaceFortressVs-1.4.2-SOLO-ROCK-RIMS-023-48b30c4-unsigned.aab`, **95 998 932 octets**, SHA-256 `df8a5097fe619a2316bb51cc8705bdbded916d3f92225af5dc2a1aacbdfe065b`. Workflow a vérifié les sommes.
- **Rollback complet du rendu/test SOLO-023** (si Fab l'approuve) : inverser seulement les commits `48b30c4a6d7c200fc2979bb411a3a048b4a3348f`, puis `57f74d8c4c7437d01cf84d7edea200a57bf4e672`, puis `51752da4dd74fffc9b8c0a1448b2d6eb27fd3fcc`, puis `39d0a7c2b6969f546a7dc7537b3bcf993b097dcb`, par `git revert` individuels sur la branche SOLO, gérer possibles conflits de documentation et relancer la CI. Conserver le PNG et générateur non utilisés, ainsi que cette trace. Ni main ni Release.
- Vérification perceptive sur téléphone Samsung encore nécessaire (raccords, lisibilité, zoom et collision ressentie). Ne pas présenter la qualité visuelle comme déjà jugée par Fab.

## SOLO-024 — vrais contours organiques + parallaxe seule près de la coque (17 octobre 2026)

**Autorisation :** après la critique « trop artificielle » sur SOLO-023 et la constatation « il manque le défilement arrière proche du vaisseau », Fab a autorisé explicitement « Ok go ».

**Baseline de sécurité :** `d245f0dda56e8021de3a77eb8be540dedc81a991` sur `feature/solo-coop-core-v1`. Aucun merge `main` ni Release.

**Partie A — contour de carte, pas par-tuile** : remplacer les silhouettes décoratives N/E/S/W répétées par une surface de rocher **continue dans l'espace monde**. Dans `src/solo_organic_rims.hpp`, un champ continu adoucit les angles sur plusieurs cellules puis crée de longues ondulations multi-fréquences sans remise à zéro au bord des carreaux. Cache de 24 morceaux de 8×8 cellules, échantillonnés à 48 texels par cellule, mémoire bornée, régénération d'un morceau quand les cellules changent, PNG source `ParoisModeSolo.png` conservé. Rendu binaire alpha (deux passes identiques). `src/coop_solo_scenery.hpp` charge ce rendu et conserve les 48 petites pièces SOLO-023 comme secours en cas de défaut. **La carte physique et ses collisions restent carrées** : adoucir l'image n'autorise jamais le passage à travers une roche.

**Partie B — mouvement du fond seul** : `soloBackgroundOnlyGain` active de 0,5 diamètre à 1 diamètre une progression `smoothstep` de 0 à 100 % de dérive des images d'arrière-plan. Entre 1 et 2 diamètres, la dérive supplémentaire revient progressivement à 0 tandis que la bande de défilement physique SOLO-022 démarre. `CoopSoloWorldBridge::backgroundDriftX/Y` ne modifie ni `cameraLeft/Top`, ni `toWorld`, ni collisions : seulement offsets de `fond4hlow.png` et `FondsParallaxeModeSolo.png` dans `drawBackground`. Calcul basé sur déplacement réel du vaisseau, pas le doigt : au repos le fond ne bouge pas ; au retour arrière le fond revient. Réinitialisé à la création de map.

**Tests :** contour continu avec/sans mur, connexion multi-cellules, changement de carte et cache, zoom et occlusion ; gain de parallaxe à 0,5/1/2 diamètres, raccords, dérive sans caméra, interdiction d'une dérive à 0 mouvement/dt ; anciens tests et mécanismes préservés.

**Retour arrière :** code texture organique (nouveau fichier) dans commit `210afab295dd2277986fe44b7377a4e9c2bfd5dd` ; commit d'intégration principal et éventuelles corrections CI seront ajoutés à cet ordre une fois connus. Après accord Fab, utiliser des `git revert` **en ordre chronologique inverse** sur les seuls commits de SOLO-024, sans reset, force-push, merge `main` ni Release. Laisser la trace.



### SOLO-024 — commits sources exacts, contrôle et revert
Base historique avant SOLO-024 : `d245f0dda56e8021de3a77eb8be540dedc81a991`. Nouveau renderer de contours continus `210afab295dd2277986fe44b7377a4e9c2bfd5dd`. Intégration des contours + dérive indépendante du fond + tests `599f6a82347f10f25ed9354b35c6f8d895f72ea4`. Correctif pour garantir l'alpha **binaire** et le masquage identique lors du second rendu devant les objets : `2946e9a30cfc9923ccd6371c16a2121c66abc1a3`. Suite test source exacte `39067948776`, export direct conditionnel `.github/workflows/solo-organic-024-export.yml`. La première CI `39067888937` a échoué à l'assertion d'identité des deux passages de mur du fait d'une interpolation linéaire ; le sampler a été remis en mode nearest sur les textures organiques seulement.

**Retour arrière complet, après validation explicite de Fab** : sur `feature/solo-coop-core-v1`, contrôler le diff, puis annuler les commits en **ordre inverse** `git revert 2946e9a30cfc9923ccd6371c16a2121c66abc1a3`, `git revert 599f6a82347f10f25ed9354b35c6f8d895f72ea4`, `git revert 210afab295dd2277986fe44b7377a4e9c2bfd5dd`. Résoudre éventuellement les conflits de ce document en **conservant la trace**, relancer tests. Ne pas supprimer les anciennes planches et ne pas réinitialiser la branche. CI/Android et qualité visuelle Samsung encore à confirmer.


### SOLO-024 — livraison vérifiée 17/10/2026
- Source définitive `2946e9a30cfc9923ccd6371c16a2121c66abc1a3`, CI complète Android/C++/SDL2 **39067948776 SUCCESS**, aucune Release (SKIPPED).
- Export direct **39068019221 SUCCESS**. Vrais binaires présents dans `temp/solo-organic-024/`.
- APK debug installable `SpaceFortressVs-1.4.2-SOLO-ORGANIC-024-2946e9a.apk`, **98 710 352 octets**, SHA-256 `25cff97cf6439b52d38d5de092d452f68cdd34c071308decde29d8e74aad930b`.
- AAB non signé `SpaceFortressVs-1.4.2-SOLO-ORGANIC-024-2946e9a-unsigned.aab`, **96 006 869 octets**, SHA-256 `4aec7e7ea8028551c2c1cf2362301c785c12996aac3496f45efe81927f315454`. Hash et ZIP/artefact source vérifiés dans workflow.
- À vérifier perceptivement sur Samsung : contours longs à l'intérieur/extérieur des roches, répétitions visuelles, absence d'artéfact d'occlusion, effets de défilement du fond dans zone de rayon 0,5→1 diamètre, raccord à 1→2 diamètres, collision logique inchangée. CI n'est pas preuve de confort visuel.
- Revert source si demandé par Fab : `git revert 2946e9a30cfc9923ccd6371c16a2121c66abc1a3`, puis `git revert 599f6a82347f10f25ed9354b35c6f8d895f72ea4`, puis `git revert 210afab295dd2277986fe44b7377a4e9c2bfd5dd`, branche `feature/solo-coop-core-v1` uniquement, résoudre les conflits de documentation sans supprimer cette trace, relancer CI. Ne pas réinitialiser, force-push, fusionner main ou faire Release.

## SOLO-025 — mission complémentaire matière et météores (18 octobre 2026)
Ordre Fab : « Parfait, on s'attaque à présent à l'aspect répétitif du motif », confirmé « Ok go go go » puis « Oui on y va ^^, bisous » et **« Adjoint les son d'explosion moins répétitif aussi Meteor explosion rock contre rock il y en a plein sur Djipie sers-toi »**.

Référence avant SOLO-025 `1750c5e47befd08df9133787811c5c0a6dbcf0a9`. Travail visuel : `91e024457ccc40cf543af8ddc147ea8a251d0e2b` ; variation de matériau en coordonnées monde, bandes répétées déformées en douceur, lueurs bleues désynchronisées. Travail audio : `b74b0624e1f04e8e7e8e13d2db43c76711e2fe2e` ; nouvelle banque de 4 vraies prises sur impacts roche-roche et destructions, `MeteorTakePicker` mélange les 4 sans répétition immédiate, ancien audio fallback. Traces exactes des quatre assets Djipie (nom, taille, hash) et origine dans `docs/solo-025-material-audio.md`.

**Confidentialité/licences :** Djipie privé. Jamais de WAV dans un commit public ni dans un artefact WAV distribué. À la compilation uniquement, récupérer les fichiers en environnement GitHub Actions autorisé via le secret `DJIPIE_READ_TOKEN` en lecture du repo privé. Contrôler SHA-256, inclure dans le jeu de diagnostic uniquement si droits applicables. Sans secret, le moteur revient proprement aux anciens sons ; ne pas affirmer que l'APK comporte les nouveaux WAV sans preuve du test d'inclusion.

**Retour arrière :** sur la seule branche `feature/solo-coop-core-v1`, après accord Fab, annuler en ordre inverse les commits de fonctionnalité documentés (et corrections ajoutées ultérieurement), vérifier `git diff`, relancer CI. Garder cette trace ; jamais reset --hard, force-push, fusion main ou Release sans validation.


### SOLO-025 — chaîne sonore privée opérationnelle sans secret inter-dépôts
Sur Djipie, une branche privée **`feature/solo-025-private-meteor-apk`** a été créée, sans modification de `main`. Workflow `.github/workflows/solo-025-private-meteor-apk.yml`, commit `d57710a0594b08ef475e5db7660e13918963adfa`, exécution GitHub Actions `39085282683`. Cette chaîne utilise le **GITHUB_TOKEN de Djipie lui-même** pour télécharger uniquement les quatre fichiers de sa Release privée, contrôler tailles et SHA-256, cloner en lecture seule le code SOLO-025 SpaceFortress testé (`b6e0a38cb98f0ec769cc3c4c1d409200c3da5b43`), produire un APK Android privé, vérifier la présence et les SHA-256 de ses quatre WAV dans l'APK, puis le déposer dans `Djipie/feature/solo-025-private-meteor-apk/temp/SpaceFortressVs-SOLO-025-Djipie/` si taille Git admissible. Ni ZIP à décompresser, ni Release, ni WAV autonomes dans Git. Le lien est accessible **uniquement avec authentification à Djipie**. Vérification CI privée à confirmer avant d'affirmer succès.

**Premier run SpaceFortress `39084636747`** : deux suites vertes mais erreur de compilation Android due à la référence à `sfCoop` encore indisponible dans `solo_field_runtime.hpp`. Correction séparée **`b6e0a38cb98f0ec769cc3c4c1d409200c3da5b43`** : signal `sfsolo::meteorRockPairPending` dans `src/solo_meteor_sound_state.hpp`, partagé explicitement entre physique roche et système son sans dépendre de l'ordre d'include. CI corrigée `39085021465`; export public `solo-variety-025-export.yml` épinglé sur ce commit. Le dépôt public affiche honnêtement si les WAV sont absents (secret de lecture `DJIPIE_READ_TOKEN` non configuré dans `SpaceFortressVs`). Un APK public sans eux ne doit **jamais** être décrit comme « avec quatre nouveaux sons ».

**Retour arrière source SOLO-025 vers SOLO-024**, uniquement après validation Fab : `git revert b6e0a38cb98f0ec769cc3c4c1d409200c3da5b43`, puis `git revert 8d1cb323a342247ccbe9d9d2704efc2d9e662d02`, puis `git revert b74b0624e1f04e8e7e8e13d2db43c76711e2fe2e`, puis `git revert 91e024457ccc40cf543af8ddc147ea8a251d0e2b`. Dans cet ordre inverse de dépendances, seulement sur `feature/solo-coop-core-v1`. Les fichiers de workflow livraisons autonomes peuvent être désactivés séparément. Conserver cet historique. Pas de reset --hard / force push / merge main / Release.



### SOLO-025 — fin des tests et APK SONORE PRIVÉ vérifié (18/10/2026)
- Build source exact `b6e0a38cb98f0ec769cc3c4c1d409200c3da5b43`, CI `39085021465` : **SUCCESS** (Android, SDL2, C++), Release publique SKIPPED. Correctif du signal roche-roche `b6e0a38` après échec C++ initial.
- Export public `temp/solo-variety-025/` : CI export réussie ; APK **0/4** nouvelles prises (jeton `DJIPIE_READ_TOKEN` non configuré dans SpaceFortress), donc SON HISTORIQUE SEULEMENT. Ne pas confondre les deux APK.
- Bonne solution privée sans jeton supplémentaire : workflow `greenpower2669/Djipie` branche privée `feature/solo-025-private-meteor-apk`, run **39085282683 SUCCESS**. Ce workflow récupère la source testée, les quatre WAV dans Djipie privé, contrôle les SHA256 originaux puis leurs 4 présences et SHA256 dans l'APK, compile dans le runner et range SEULEMENT l'APK installable dans `temp/SpaceFortressVs-SOLO-025-Djipie/`. APK nommé `SpaceFortressVs-1.4.2-SOLO-025-METEOR-4-b6e0a38.apk`, **99 919 413 octets**, SHA256 `f08b1f9c9df78e0562b063ffd20a9fdba672147762e25063ee0b4cf4d0df7c6e`. Git tree du dépôt privé et README / SHA256SUMS vérifiés. Aucun ZIP à décompresser sur le téléphone.
- Accès privé : le téléchargement est réservé aux utilisateurs connectés autorisés sur Djipie. APK diagnostic `com.greenpower2669.spacefortressvs.diagnostic` ; ne pas écraser le jeu historique, ne pas exposer le contenu de la sonothèque comme fichiers autonomes, vérifier droits avant diffusion publique.
- Retour arrière code SOLO-025 seulement : `git revert b6e0a38cb98f0ec769cc3c4c1d409200c3da5b43`, `git revert 8d1cb323a342247ccbe9d9d2704efc2d9e662d02`, `git revert b74b0624e1f04e8e7e8e13d2db43c76711e2fe2e`, `git revert 91e024457ccc40cf543af8ddc147ea8a251d0e2b`, avec validation et éventuels conflits documentaires à résoudre. L'export/workflow additionnel et la documentation restent historisés. Aucun reset, force push, main ou Release sans accord.
- Reste le retour visuel/sonore réel de Fab sur Samsung.

## SOLO-026 — mandat du 18 octobre 2026 : AUDIT SEULEMENT
**Fab n'autorise aucun changement du code, des assets, des tests ou des paramètres gameplay à ce stade.** Seule l'analyse et la mise à jour des cinq fichiers vivants du dépôt SOLO sont autorisées. Ne pas créer de build, APK, AAB, PR de code, merge `main` ni Release en invoquant cette mission.

Retours prioritaires à examiner : (1) roches présentes visuellement dans le cône mais pas minées, alors que la zone minière est plus courte que la zone d'aspiration affichée ; (2) annuler l'illusion de défilement proche limitée au fond (SOLO-024) au profit d'une translation cohérente du vrai labyrinthe et de la vitesse intermédiaire ; (3) impact contre paroi plus dangereux pour les PV **et** la réserve d'énergie selon vitesse de choc sans punir le glissement ; (4) réserve qui se recharge beaucoup trop vite, donc privilégier patience ou minage pour atteindre 100 %. Préserver contrôles, inertie, rebonds, météo météores fixes, boss, mode COOP historique, DUEL et confidentialité Djipie.

Sources audit : `src/kinetic_shield.hpp`, `src/legacy_field_runtime.hpp`, `src/coop_solo_world_draw.hpp`, `src/coop_solo_world_bridge.hpp`, `src/solo_viewport.hpp`, `src/coop_solo_scenery.hpp`, `src/campaign_runtime.hpp`, `src/ship_energy_rules.hpp`, `src/tactical_runtime.hpp`, `tests/coop_solo_world_bridge_regressions.cpp`. Diagnostic détaillé dans `debughistorical.md`, règles voulues dans `brain.md`, tâches `todo.md`, pointeur `brainmap.md`.

**Critères futurs (en attente d'autorisation séparée)** : recouvrement entre rendu cône et minage réel ; fonds, acteurs, murs et collision restent dans un seul référentiel physique ; toute l'échelle des vitesses progressives reste perceptible ; collisions à vitesse normale coûteuses avec anti-répétition ; recharge longue ou minage récompensé ; ne jamais livrer de fausse preuve de test Samsung. Conserver l'obligation historique de livrer un APK direct réellement installable (et AAB correctement nommé le cas échéant) via export GitHub `temp/` lors d'une future mission de code approuvée.

### SOLO-026 — GO code explicite de Fab (18/10/26) ; remplace la restriction audit seulement
Fab autorise les quatre modifications SOLO décrites dans brain.md et l'accès caché à l'écran de début/debug. **Précision immuable Fab : le cône continue de se réduire avec le niveau de difficulté**. Ajuster la zone minière SOLO au dessin du cône, mais ne pas neutraliser cette modulation ni modifier les modes historiques. Remplacer le défilement du fond seul par caméra cohérente et parallaxes uniquement lorsque la caméra se déplace, dès le premier anneau près du vaisseau. Collisions mur SOLO retirent PV + réserve selon la vitesse normale avec tolérance au glissement et cooldown. Recharge passive SOLO beaucoup plus lente : joueur attend ou mine pour atteindre 100%.
Démarrage debug Android : direct en SOLO, hub de diagnostic masqué et activable après **20 taps alternés tout en haut : gauche, droite, répétés dix fois**. Le hub conserve ses boutons/logs, et Android release doit garder le manifeste historique.
Branche limitée à `feature/solo-coop-core-v1`. Aucun merge vers main/release sans accord. Tester source exact C++/SDL2/Java/Android ; ne livrer fichier direct `.apk` + `-unsigned.aab` que si le workflow CI sur le même SHA est SUCCESS. Exporter via GitHub `temp/solo-026/` SHA256 contrôlé, pas de ZIP à décompresser. Les WAV Djipie privés nécessitent un build privé autorisé : ne pas annoncer 4/4 si l'APK public ne les contient pas. Aucune modification de Djipie demandée dans cette mission.

### SOLO-026 — livraison vérifiée CI et APK (18/10/2026)
Livraison techniquement vérifiée, sans main/release : source `788c03c`, CI `39132530943 SUCCESS`, GitHub export `39132531008 SUCCESS` ; commit binaires `1149f990`. Direct APK installable de diagnostic `temp/solo-026/SpaceFortressVs-1.4.2-SOLO-026-788c03c.apk` SHA256 `c6e4412ca0c867f2d37f18060a74cb4f99ec6bda26fe6577affe0c1b28950a96`. AAB `temp/solo-026/SpaceFortressVs-1.4.2-SOLO-026-788c03c-unsigned.aab` SHA256 `2ebc55ec5edcd07fbf58642fdd5eea5f9dd4c1b4fd70ee7d1cf8b38f836cc9de` ; non signé. Les quatre sons privés Djipie ne sont pas dans CET APK public (0/4) ; fallback historique uniquement. Pas de prétention de validation smartphone, toujours à faire avec Fab.
