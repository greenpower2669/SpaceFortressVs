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
