# brain.md — SpaceFortressVs

## Référence de reprise
- Dépôt : `greenpower2669/SpaceFortressVs`.
- Branche de travail : `fix/gameplay-campaign-200`.
- Sauvegarde Astra : commit `9b8cae378f66d461dd8a29f9504972f648b2c40c`.
- Arbre Git vérifié par Sol : `92cf3b03b2aeaf30da6d57b85f126f89cf37caf4`.
- Base fonctionnelle antérieure : v1.3.1, commit `dcf80afb6eb117d405f46f03d8567d31a76b4b12`.
- Version préparée : 1.4.0, versionCode 10.

Cette mémoire a été reconstruite par Sol le 26 septembre 2026 à partir du code récupéré, de `todo.md`, de `docs/superpowers/plans/2026-09-20-gameplay-200.md`, de `docs/releases/1.4.0.md` et des tests présents. Les trois mémoires `brain.md`, `brainmap.md` et `debughistorical.md` étaient absentes de la sauvegarde Astra ; ne jamais prétendre qu'elles étaient synchronisées auparavant.

## Contrat fonctionnel à préserver
- Préserver le moteur historique, `src/main.cpp`, les sauvegardes, les 50 portraits de boss et l'écran de fin stable.
- Aucun transfert massif Base64 et aucun remplacement gratuit des assets historiques.
- Aucun ajout fonctionnel ni refonte dans cette reprise : corriger uniquement un blocage reproduit par des vérifications fraîches.
- Aucun merge dans `main` et aucune publication de release sans nouvel accord explicite de Fab.
- Livrables Android attendus après vérification : `SpaceFortressVs-1.4.0.apk` et `SpaceFortressVs-1.4.0-unsigned.aab`.
- Ne jamais demander de désinstaller l'application ni d'effacer ses données pour contourner une incompatibilité de signature.

## Comportement présent dans le code récupéré

### Campagne 200
- 200 affrontements = 50 identités × 4 difficultés.
- `sfBossIndex(encounter) = encounter % 50`.
- `sfDifficultyIndex(encounter) = encounter / 50`.
- Difficultés : VIF, ENDURANT, VICIEUX, ULTIME.
- La progression, la sélection, les victoires en attente et le Hall of Fame couvrent jusqu'à 200 rencontres.

### Signature visuelle des difficultés
- Même fonction de rendu en sélection et en combat : `sfDrawEncounterBoss`.
- Niveau 1 : aucune aura, 0 tentacule.
- Niveau 2 : aura verte, 4 tentacules.
- Niveau 3 : aura jaune, 8 tentacules.
- Niveau 4 : aura rouge, 20 tentacules.
- Les reflets restent localisés afin de conserver le portrait original au centre.
- Le sélecteur affiche aussi le niveau et les informations de boss sans générer 200 portraits distincts.

### Coop, tirs, énergie et tourelles
- Premier doigt de chaque pilote : déplacement.
- Doigts supplémentaires : tir au tap ; maintien/déplacement du doigt de tir ne doit pas produire d'auto-tir.
- Tirs humains ordinaires : axe du camp ; pas de guidage en vol.
- Missile : tir spécial à réserve pleine selon la sémantique historique `nrj`.
- `nrj=0` signifie réserve pleine ; `nrj=50` réserve épuisée.
- Le bouclier dépend de la réserve historique.
- Les tourelles coop restent inactives sans bonus.
- Le bonus flottant les active temporairement ; leur réserve d'énergie est distincte de celle des pilotes.

### Champ historique partagé
- Duel et coop utilisent le même propriétaire de champ historique.
- Astéroïdes mobiles, collisions, fragmentation, minage, minerai, impacts et collecte restent basés sur les primitives historiques.
- Les limites de population sont bornées par le runtime.

### Sauvegardes
- Lecture des formats v1 et v2.
- v1 reste limitée à 50 rencontres ; v2 couvre 200.
- Migration v1→v2 sans perte des noms UTF-8, scores, Hall of Fame ni victoire en attente.
- Conservation durable de l'original avant réparation/migration ; les formats inconnus restent protégés.

## Preuves et niveaux de confiance

### Rapporté par Astra — passation, pas preuve fraîche Sol
Astra rapporte avant sauvegarde : 32 groupes SDL/UBSan réussis, 21 groupes du champ historique, migration v1/v2, neuf tests Python, 200 rendus du sélecteur et simulations de combats. Ces résultats servent de contexte mais ne doivent jamais être présentés comme exécutés par Sol.

### Vérifié directement par Sol avant CI
- La branche distante existe et pointe sur `9b8cae378f66d461dd8a29f9504972f648b2c40c`.
- Le commit possède exactement l'arbre `92cf3b03b2aeaf30da6d57b85f126f89cf37caf4`.
- Le source récupéré contient la campagne 200, le rendu 0/4/8/20 partagé sélection/combat, les commandes tactiles, l'énergie historique, le bonus tourelles, le champ partagé et la migration v1/v2.
- `android/version.properties` contient 1.4.0 / code 10.
- `android-build.yml` exécute `scripts/test-regressions.sh`, construit APK/AAB, puis vérifie/versionne les paquets.
- `package-release.py` compare le certificat du nouvel APK au certificat publié de la v1.3.1.

### Vérifications fraîches Sol encore requises
- Exécution CI de `scripts/test-regressions.sh` sur le nouveau HEAD contenant ces mémoires.
- Vérification du dernier ajustement des textes du sélecteur, y compris portrait/paysage.
- Compilation APK/AAB.
- Intégrité des PNG, bibliothèques et archives.
- Comparaison réelle du certificat 1.4.0 à celui de la v1.3.1.
- Essais physiques sur téléphone : quatre doigts, reprise après veille, champ vivant, énergie, bonus/tourelles, son, performances et équilibrage des quatre difficultés.
