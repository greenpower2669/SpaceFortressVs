# brainmap.md — SpaceFortressVs

## Carte de reprise
`fix/gameplay-campaign-200` → sauvegarde Astra `9b8cae378f66d461dd8a29f9504972f648b2c40c` → arbre `92cf3b03b2aeaf30da6d57b85f126f89cf37caf4`.

La reprise Sol ne redéveloppe pas la campagne. Elle documente, vérifie, construit et contrôle la signature.

## Architecture fonctionnelle

`src/boss_catalog.hpp`
→ 50 identités historiques
→ 4 difficultés
→ 200 profils d'affrontement
→ `sfBossIndex = encounter % 50`
→ `sfDifficultyIndex = encounter / 50`.

`src/boss_difficulty_visuals.hpp`
→ `sfDrawEncounterBoss`
→ utilisé en sélection ET en combat
→ aura + tentacules + reflets
→ niveaux : 0 / 4 / 8 / 20 tentacules.

`src/campaign_runtime.hpp`
→ sélection 20 pages × 10 combats
→ lancement/reprise/victoire
→ contrôles multitouch
→ tirs/missiles
→ bonus flottant
→ activation temporaire des tourelles
→ HUD énergie
→ rendu de campagne
→ Hall of Fame.

`src/ship_energy.hpp`
→ sémantique historique de `nrj`
→ coût des tirs
→ missile à pleine réserve
→ dégâts du bouclier selon la réserve.

`src/legacy_field_primitives.hpp`
+
`src/legacy_field_runtime.hpp`
→ primitives du champ historique
→ astéroïdes
→ impacts/collisions
→ fragmentation
→ minage
→ minerai
→ même runtime duel/coop.

`src/campaign_save.hpp`
→ décodage v1/v2
→ limite v1=50 / v2=200
→ archive/préservation
→ écriture atomique
→ Hall + noms + victoire en attente.

## Tests

`scripts/test-regressions.sh`
→ validation assets/Python
→ fixture format campagne
→ suite native SDL/UBSan
→ génération source Android historique
→ syntaxe Android
→ suite champ historique.

`tests/restoration_regressions.hpp`
→ deux pilotes / quatre doigts
→ tir au tap
→ missile pleine réserve
→ pause/annulation des doigts
→ tourelles passives sans bonus
→ bonus, réserve tourelles, expiration et bouclier.

`tests/difficulty_regressions.hpp`
→ 200 ouvertures de combat
→ difficulté progressive
→ 200 rendus
→ 0/4/8/20 tentacules
→ niveau 1 pixels historiques inchangés
→ sélection portrait/paysage
→ lancement du combat 200.

`tests/legacy_field_regressions.cpp`
→ visibilité/minage
→ fragmentation réelle
→ minerai/collecte
→ collision vaisseau duel/coop
→ champ complet portrait/paysage à 30/60/120 Hz.

`tests/campaign_format_regressions.cpp`
→ fixture v1 réelle
→ noms UTF-8
→ victoire en attente
→ round-trip v2 jusqu'à 200.

## Chaîne Android

`.github/workflows/android-build.yml`
→ checkout HEAD de branche
→ `scripts/test-regressions.sh`
→ Gradle assembleDebug + bundleRelease
→ `scripts/package-release.py`
→ contrôle version/package/certificat/assets/archives
→ artefact `SpaceFortressVs-1.4.0-release-files`.

Publication automatique : NON sur `fix/gameplay-campaign-200`.
Le job de publication est limité à `ui/start-screen-v1`; aucun merge/release n'est autorisé dans cette mission.

## Mémoire FAB Copilot
Tout changement futur de code ou d'état vérifié doit mettre à jour ensemble :
- `brain.md`
- `brainmap.md`
- `debughistorical.md`
- `todo.md`
dans le même cycle/commit.


## État de livraison vérifié par Sol
`2ca6aa46e732e9d9e86e9889ad2d215117c1f16e`
→ workflow 36246639403
→ tests 9 Python + 32 groupes natifs + 21 groupes champ : PASS
→ Gradle : PASS
→ packaging : PASS
→ artefact `SpaceFortressVs-1.4.0-release-files`
→ APK + AAB produits
→ certificat 1.4.0 `9817ba2bdf29226c72529ec161f8124f11cbee89b2748ca440b1e38148e01781`
≠ certificat 1.3.1 `8abfc11c8bc4f9ac065eb5c086ad4e457290bcbbc1105017865368de7e565868`
→ mise à jour directe Android incompatible.

Le HEAD documentaire post-vérification peut être supérieur à `2ca6aa46e732e9d9e86e9889ad2d215117c1f16e` car la synchronisation finale des quatre mémoires utilise `[skip ci]`. Le SHA de code effectivement testé reste `2ca6aa46e732e9d9e86e9889ad2d215117c1f16e`.
