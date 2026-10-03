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


## Carte d'audit téléphone — 26 septembre 2026

### Régression classique
Champ astéroïdes présent physiquement/effets visibles
→ **rendu des sprites astéroïdes absent**
→ auditer la chaîne de dessin classique séparément de la simulation.

### Coop
`sfCoopEmit / rendu projectiles`
→ missile déclenché mais apparence plasma.

`sfCoopDrawArena / HUD pilotes`
→ PV pilotes non visibles.

`sfCoopDefences + profils boss`
→ DPS tourelles vs PV/résistance boss trop élevé.

`collision boss-vaisseau`
→ contact continu doit consumer rapidement énergie puis PV.

`visée IA + mouvement projectiles`
→ prédiction autorisée au départ ; poursuite en vol réservée aux tirs explicitement guidés.

`ship_energy + dégâts + collecte poussières/minerais`
→ chaîne à auditer ensemble : impact → bouclier/énergie → PV → récupération → cadence/précision.

`collision astéroïde-vaisseau`
→ dégâts PV actuels jugés trop faibles sur téléphone.

Ne pas déduire une cause racine à partir de cette carte : elle localise les sous-systèmes à inspecter et les interactions à mesurer.


## Complément HUD téléphone
Joueur haut coop validé RAS
→ logique contrôle/tir à préserver.

HUD joueur haut
→ rotation/lecture à 180°
→ lisible depuis le côté opposé de l'écran.

Vie boss
→ barre visuelle sans valeur numérique requise
→ lecture miroir/inversée côté joueur rouge/haut
→ code couleur vert plein → rouge vide.


## Lot correctif 1.4.x validé
`prepare-legacy-source.cmake`
→ désambiguïsation W Android
→ rendu astéroïdes classique conservant main.cpp intact.

`campaign_runtime.hpp`
→ missile historique
→ cooldown + dispersion initiale selon énergie
→ contact boss continu
→ callback impact astéroïde
→ HUD miroir/180°
→ tag tourelles séparé.

`tactical_runtime.hpp`
→ récupération minerai atténuée.

`tests/*`
→ rendu classique réel
→ missile/HUD
→ chaîne difficulté/énergie/contact.


## Preuve CI du lot
`fbb1ff92907821217ff94d847d9f2bbf4683636d` → run 36266178070 → 57 PASS → Gradle PASS → packaging PASS → artefact 10914467015. Test téléphone Fab reste requis pour le ressenti réel et le placement HUD.


## Dégâts entrants coop ×5
`boss projectile / beam / wave` → `sfCoopHurt` → bouclier actuel → ×5 PV.
`contact boss` → énergie/chaleur historique → formule continue → ×5 PV.
`champ astéroïdes` → callback `sfCoopAsteroidHurt` → bouclier actuel → ×5 PV.
Aucun changement : énergie, i-frames, dégâts vers boss, PV boss, mode classique, `src/main.cpp`.


Test callback astéroïde
→ `sfLegacyFieldStep(sfCoopAsteroidHurt)` pour mesurer l'impact seul
→ ne pas inclure `sfCollectDust()` dans l'assertion exacte ×5
→ récupération poussière reste testée séparément.

`sprite::setv(…,20,20,…)` → hauteur historique randomisée → test surface = `impact->w*impact->h`, jamais 20×20 supposé.


## Preuve CI D-140-10
`7e624256d8f50d07a63e92c9c3851ff2b6048ace` → run 36940059444 → 58 PASS → ×5 confirmé sur pilotes/modes/spéciaux/contact/astéroïdes → Gradle PASS → packaging PASS → artefact 11199572849.


## D-140-11 — dégâts ×15 et économie de poussières
Fab demande ×3 supplémentaire sur le ×5 validé : multiplicateur coop total ×15 après protection du bouclier. Les poussières blanches historiques peuvent soigner un boss blessé lorsqu'il les touche ; les munitions boss spéciales kind 1/2/3 peuvent aussi les ramasser et transmettre le soin. Le collecteur éligible le plus proche gagne et un boss à pleine vie ne consomme pas la poussière. Les impacts boss sur un vaisseau génèrent des poussières rouges ; elles ne rendent ni PV ni énergie et, après armement, ajoutent seulement un peu de chaleur `nrj`, donc usent le bouclier. Les collisions d'astéroïdes conservent leur émission rouge historique. `src/main.cpp` reste intact.


### Preuve fraîche D-140-11 APK
Gameplay testé : `23d0e3e1354dec2cadf7b40c7a4f10a8b751a585`. Workflow `37095128530` : régressions complètes GREEN puis build Android APK+AAB réussi. Artefact : `SpaceFortressVs-x15-dust-APK-AAB-37095128530`. Le premier essai avait échoué uniquement sur la validation d'un wrapper Gradle tiers SDL téléchargé trop tôt ; l'ordre CI a été corrigé sans changement gameplay. Aucun merge `main`, aucune release ; validation téléphone reste à Fab.


## D-140-12 — bouclier commun
DUEL Android généré + COOP → sfApplyShieldImpact → protection linéaire → PV + usure. Contact coop → pas interne 240 Hz. Poussière blanche → -0.25 nrj pleine. Rouge coop → petite usure seulement. main.cpp historique inchangé.


### D-140-13 scenic architecture
`scenic_mix.hpp` → 20 bases → 100 mixed candidates → campaign 200 / classic shuffled bag. Campaign: `sfDrawCampaignSpace(encounter)` + `sfBossTravelProgress`. Classic: `sfRmSyncUiEngineState` selects once on GAME entry; generated legacy source calls `sfRmDrawClassicScenicMap` after historical backdrop layers.

- D-140-14: kinetic_shield.hpp -> ship_energy.hpp -> legacy_field_runtime.hpp (classic+coop); campaign_runtime.hpp adds charge-only boss kinetics; tactical renderer shows transient two-ring ripples.

### D-140-15
`kinetic_shield.hpp` = shared low-speed-safe mass×v² core + compact speed ranges + transient multi-wave state. `legacy_field_runtime.hpp` = both-mode asteroid interception/fragmentation/red dust. `campaign_runtime.hpp` = charge-only boss kinetics without ×15; ordinary contact/projectiles keep non-kinetic ×15. White dust remains physically untouched.

## Kinetic surge / two-finger field — 2026-10-03
- Canon shared by classic + coop: maximum kinetic field diameter = 2.0 ship diameters.
- Short second-finger tap keeps firing; long hold (0.35 s) enters a transparent visible surge.
- Surge doubles kinetic dissipation for at most 2.0 s. Release after activation purges every asteroid inside the max kinetic zone into historical white resource dust.
- White dust is never physically deflected by kinetic waves; red dust remains reactive.
- Normal absorption waves are more transparent; surge aura shows inner + outer circles.
- Classic hull dimensions remain historical; only the kinetic field diameter expands.
- No main merge/release before Fab phone validation.

- Kinetic surge canon: HOLD 0..2s => x2 + rainbow + charge sound; READY >=2s => kinetic shield OFF; release => blast/purge. Boss non-kinetic danger selector: 1/5/10/15/20, default 10.
