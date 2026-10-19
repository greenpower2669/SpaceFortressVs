# brainmap.md — SpaceFortressVs, carte légère
> Entrée unique pour chaque mission. **Ne pas charger d'archives par défaut.** Optimisé le 19/10/2026 ; voir [index des archives](docs/memory-archive/2026-10-19-pre-compaction/README.md).

## État en une lecture
- `main` public : **v1.4.2** ; SOLO en cours : `feature/solo-coop-core-v1`, dernier APK diagnostique **SOLO-034** (SOLO-035 en développement, pas encore livré) (source `11fd5a7`, CI 39215282189 verte, export 39215282005).
- Fab a donné un **retour global positif sur les dernières commandes SOLO-033/034**. Protéger le pilotage : 30 % direct + réserve de maintien progressive +100 points, max 130 % historique ; même inertie/caméra/collisions.
- **Mission SOLO-035 autorisée par Fab : habillage graphique rocheux en cours** : roche extérieure pleine avec cratères variés, fusion imperceptible dans la texture rocheuse actuelle, en particulier la grande traverse plate du bas. Aucune altération de collisions/largeur des couloirs.
- **Toujours relire IMMUTABLE avant de coder** : GitHub extrait l'APK après CI et dépose le vrai `.apk` direct dans `temp/<mission>/`, SHA256 ; jamais de ZIP à décompresser par Fab. Pas de merge main/Release sans son accord. `src/main.cpp` historique interdit (blob `835059a0ecfe0f74708068b3259cad5db1cdb579`).

## Tiroirs actifs (lire uniquement ceux nécessaires)
| Besoin | Entrée |
| --- | --- |
| Autorisations en vigueur, invariants, livraison / rollbacks | [`ordres-de-mission.md`](ordres-de-mission.md) |
| Architecture, moteur SOLO, contrôle, roches, données techniques courantes | [`brain.md`](brain.md) |
| Diagnostics, erreurs de paradigme, retours Samsung, pièges | [`debughistorical.md`](debughistorical.md) |
| Travail encore à effectuer, validation nécessaire, historique de cases anciennes | [`todo.md`](todo.md) |
| Ancien état intégral des 5 mémoires avant optimisation | [archive du 19 octobre](docs/memory-archive/2026-10-19-pre-compaction/README.md) |

## Emplacements du moteur actuel
- Contrôle : `src/solo_fire_pad.hpp`, `src/solo_ship_motion.hpp`, `src/coop_solo_session_runtime.hpp`.
- Caméra/monde : `src/coop_solo_world_bridge.hpp`, `src/coop_solo_scenery.hpp`.
- Géologie : `src/solo_organic_rims.hpp` (`exteriorRelief`), `src/solo_rock_material_variation.hpp`, `src/solo_crater_relief.hpp`, atlas `ParoisModeSolo.png`.
- Sons Meteor : WAV privés absents **0/4** de l'APK public SOLO-034 ; anciens sons présents. Autres modes et Hall restent protégés.

## Reprise efficace
1. Lire **cette carte**, puis `ordres-de-mission.md` et uniquement les mémoires vivantes nécessaires au changement ; pour coder, appliquer la méthode FabCopilot et lire les cinq fichiers actifs.
2. Si un détail ancien est indispensable (hash, rollback, vieille CI, ancienne demande), suivre l'archive ciblée, **pas les cinq archives**.
3. Consigner nouvelles validations, diagnostics et tâches au bon endroit sans recopier la même preuve 5 fois. Aucun ancien GO ne vaut nouveau GO.

### SOLO-035 — GO Fab, incrustation cratères (19/10/2026)
Fab autorise désormais le **codage** du remplissage rocheux cratérisé après audit et compaction des mémoires. Ne pas créer un second matériau : utiliser exactement la source `ParoisModeSolo.png`, le warping de `soloRockMaterialAt` et la lumière existante de `SoloOrganicRims`. Le nouvel `src/solo_crater_relief.hpp` module uniquement les RGB des texels Rock : grandes et petites dépressions déterministes en coordonnées mondiales, centres/ellipses irréguliers, lèvres éclairées et creux plus sombres, atténuation douce dans la matière autour des ouvertures. Le bas plat, les bords latéraux et les masses internes doivent se confondre sans raccord. `Rock/Empty`, silhouette alpha, collisions, largeur des passages, `ShipMotion`, commande 30%+100 points, caméra, sons et modes historiques intouchés. Tests SDL de continuité sur les frontières de chunk et de contraste, puis CI C++/SDL2/Android du même commit, et export APK direct sous `temp/solo-035/` avec SHA256. Aucune fusion main/release.
