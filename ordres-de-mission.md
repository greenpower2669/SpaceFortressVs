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
