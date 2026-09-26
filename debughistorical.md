# debughistorical.md — SpaceFortressVs

## Règle
Ce fichier sépare les bugs/régressions, leurs causes ou hypothèses, les corrections présentes, et les preuves réellement exécutées. Les résultats rapportés par une autre session ne deviennent jamais des preuves Sol sans relance.

## Historique v1.3.1
Base de reprise : `dcf80afb6eb117d405f46f03d8567d31a76b4b12`.

Correctifs déjà historiques : respiration bornée, restauration coop→duel, tirs humains rectilignes, sauvegardes protégées, visibilité/minage du champ, accueil animé.

Limite connue de signature :
- v1.3.0 publiée : certificat `822915992d833e32f555fd4332ff56bba0c5319bc0a359e1925811fe12a86c2b`.
- v1.3.1 publiée : certificat `8abfc11c8bc4f9ac065eb5c086ad4e457290bcbbc1105017865368de7e565868`.
Ces certificats diffèrent ; une installation existante ne doit jamais être supprimée pour contourner ce problème.

## Régressions restaurées pour 1.4.0
Les documents et tests récupérés identifient les régressions suivantes dans l'état antérieur :
- coop : champ simplifié au lieu du champ historique complet ;
- toucher : tir humain lié au cooldown au lieu d'un tap distinct ;
- missile coop absent ;
- bouclier découplé de la réserve historique ;
- tourelles coop déployées sans bonus ;
- campagne limitée à 50 ;
- absence de signature visuelle progressive des quatre difficultés.

Le code de la sauvegarde `9b8cae378f66d461dd8a29f9504972f648b2c40c` contient les restaurations correspondantes :
- champ partagé duel/coop ;
- quatre doigts et tir au tap ;
- missile à pleine réserve ;
- bouclier dépendant de `nrj` ;
- bonus temporaire de tourelles avec réserve propre ;
- 200 affrontements ;
- sauvegarde v2 et migration v1 ;
- rendu partagé difficulté 0/4/8/20.

## Récupération Astra → Sol — 26 septembre 2026
Astra a retrouvé le HEAD local antérieur `9ac9763032ba296e843a1f626ea318affd73b551`, puis a sauvegardé l'état de travail et transféré un arbre identique vers GitHub.

Référence distante à utiliser :
- commit : `9b8cae378f66d461dd8a29f9504972f648b2c40c`
- arbre : `92cf3b03b2aeaf30da6d57b85f126f89cf37caf4`
- branche : `fix/gameplay-campaign-200`.

Sol a vérifié directement via GitHub que le commit et l'arbre correspondent à ces valeurs.

Aucun merge ni release n'a été fait pendant la récupération.

## Incident mémoire FAB Copilot
Au moment de la sauvegarde Astra :
- `todo.md` existait ;
- `brain.md`, `brainmap.md` et `debughistorical.md` étaient absents.

Cause documentaire : ces trois mémoires n'avaient pas été créées/synchronisées dans le workspace récupéré. Elles sont reconstruites maintenant à partir des preuves Git, du code, du plan, de la release et des tests présents. Ne pas réécrire l'histoire en disant qu'elles existaient auparavant.

## État des preuves avant la relance Sol

### Rapport Astra uniquement
- 32 groupes SDL/UBSan.
- 21 groupes du champ historique.
- migration v1/v2.
- neuf tests Python.
- 200 rendus.
- cinq combats simulés et 200 fins d'affrontement automatisées.

Ces résultats ne sont pas encore comptés comme vérification fraîche Sol.

### Vérifications Sol effectuées
- branche/commit/arbre Git distants confirmés ;
- présence des implémentations principales et des tests confirmée par lecture du HEAD récupéré ;
- version 1.4.0/code 10 confirmée ;
- workflow de test/build et contrôle de signature inspectés.

### Vérification fraîche à venir
Le premier commit FAB Copilot de reprise doit déclencher le workflow Android sur `fix/gameplay-campaign-200`. Le résultat de ce workflow remplacera les affirmations de passation par des preuves fraîches pour les tests, le build, les paquets et la signature.

## Règle de correction
Si un test frais échoue : reproduire, déterminer la cause, corriger uniquement cette cause avec test de régression, puis mettre à jour les quatre mémoires dans le même commit. Aucun ajout de fonctionnalité ni refonte opportuniste.


## Vérification fraîche Sol terminée — 26 septembre 2026
Workflow : https://github.com/greenpower2669/SpaceFortressVs/actions/runs/36246639403
SHA testé : `2ca6aa46e732e9d9e86e9889ad2d215117c1f16e`.

Résultats frais :
- 9 tests Python réussis ;
- 32 groupes natifs SDL/UBSan réussis ;
- 21 groupes du champ historique réussis ;
- sélecteur portrait/paysage et lancement combat 200 réussis ;
- 200 rendus et signature visuelle 0/4/8/20 réussis ;
- 200 ouvertures de combat automatisées réussies ;
- 200 coups finaux / victoires durables / Hall après reload réussis ;
- build APK/AAB réussi ;
- packaging, assets, bibliothèque ARM64 et intégrité ZIP réussis.

Aucun blocage logiciel reproduit pendant cette relance : aucune correction de code n'a donc été ajoutée.

Signature Android réellement observée :
- 1.4.0 : `9817ba2bdf29226c72529ec161f8124f11cbee89b2748ca440b1e38148e01781`;
- 1.3.1 publiée : `8abfc11c8bc4f9ac065eb5c086ad4e457290bcbbc1105017865368de7e565868`.
Cause de l'incompatibilité de mise à jour : certificats différents. Ne pas contourner par désinstallation ni suppression des données.

Artefact vérifié : https://github.com/greenpower2669/SpaceFortressVs/actions/runs/36246639403/artifacts/10907503442
APK : `2045bc64083b624e14ecec034df350b03eaa0eeac269d145d171506d5b4f3b81`, 87372144 octets.
AAB : `a30870bece5759ddd4feb2ee4d1cf84a0e06822798afbea1e7624438d727939c`, 84880594 octets.

Les validations physiques sur téléphone restent hors de portée des tests CI et doivent être faites par Fab.
