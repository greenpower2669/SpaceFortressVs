# Archive — kinetic dust impact v4 — 2026-10-05

## Périmètre

Mission Fab : poussières blanches produites par destruction d’astéroïdes et réaction visuelle/physique des poussières rouges au champ cinétique, sans modification de l’équilibrage des dégâts.

- Base : `feature/help-tutorial-danger-9-canon` @ `76bab7bac571df6f42f4ae76011fc61ab2b0c8ee`.
- Branche dédiée : `feature/kinetic-dust-impact-v4`.
- `src/main.cpp` historique protégé ; blob de référence `835059a0ecfe31cf46833ea6c065a2381a7277c3`.
- Aucun merge `main`, aucune release.

## Audit réel

Chemins inspectés : `src/kinetic_shield.hpp`, `src/legacy_field_runtime.hpp`, `src/legacy_field_primitives.hpp`, `src/remaster_runtime.hpp`, `src/tactical_runtime.hpp`, `src/t.hpp`, tests et génération Android.

Constats de départ :
- purge : émission via `partsforiw()` à rendement fixe ;
- collision astéroïde↔astéroïde : même émission fixe ;
- destruction par champ normal : pas de ressource blanche dédiée ;
- rouge : déviation cinétique existante, sans consommation/flash local canonique.

## TDD

### RED

Commit `afbd28da13d81409d083ef7cc1f8a93010ceaec0` — `test: define kinetic dust impact contract`.

Workflow `37240892431` : les suites antérieures passent puis `tests/kinetic_dust_regressions.cpp` échoue volontairement à la compilation sur les nouveaux symboles absents (`SfKineticDustCause`, helpers de rendement/couleur/réaction rouge, état des flashes). Cette rupture valide que le nouveau test précède l’implémentation.

### GREEN

Implémentation : commit `663811dbb0b4f55a0674a8e92a40edfbc8dc3534`.

Alignement du test legacy avec le nouveau rendement proportionnel de collision : commit `8f2ee5ed61372f2647ae7284abdf9a0df7d8ebbf`.

**SHA gameplay exact figé : `8f2ee5ed61372f2647ae7284abdf9a0df7d8ebbf`.**

Workflow Android `37241618498`, job `111551304971` : régressions, compilation Android, APK, AAB, vérification et packaging réussis ; étape `publish-release` SKIPPED.

Artefact CI :
- nom : `SpaceFortressVs-Android-444` ;
- id : `6545681985` ;
- taille archive : `27011213` octets ;
- digest : `sha256:399dcf6808a83fe4071ab91d2b9b9d2465c0a6e2c6f5950824482687a936fd6e`.

## Canon implémenté

### Blanc

- Cause `SurgePurge` : rendement `1.0`, quantité proportionnelle à l’aire/taille réelle, vitesse initiale nulle.
- Cause `AsteroidCollision` : rendement `0.1`, quantité proportionnelle, vitesse suivant principalement le vecteur incident du détruit avec faible dispersion tangentielle.
- Cause `KineticField` : rendement `0.1`, même règle de projection.
- Helper central de destruction : émission seulement si l’astéroïde est encore vivant (`pv>0`), puis `pv=0` ; une destruction ne peut donc pas réémettre dans un chemin suivant.
- Le minage projectile reste historique et séparé.
- Les vagues cinétiques continuent d’ignorer la poussière blanche.

### Rouge

- Réutilisation de la réponse cinétique existante à partir du vecteur incident et de la normale locale du champ.
- Contact : progression de couleur jaune chaud → orange → rouge vif ; micro-impact local court.
- Majorité des particules consumée de façon déterministe ; petite fraction survivante déviée par la résultante du champ avec faible wobble cohérent.
- Aucun changement PV, énergie, danger, bouclier ou multiplicateur gameplay.

## Régressions dédiées

`tests/kinetic_dust_regressions.cpp` vérifie :
- fractions 100 % / 10 % / 10 % ;
- proportionnalité petit/gros astéroïde ;
- purge : émission unique, vitesse blanche nulle ;
- collision : quantité 10 %, projection alignée au vecteur pré-impact, pas de duplication ;
- champ normal : 10 %, projection incidente, objet déjà mort sans seconde émission ;
- rouge : consommation partielle, survivants déviés, flash local enregistré, blanc inchangé, PV/énergie joueur inchangés.

Les suites existantes conservent les garanties surcharge exacte 2 s, cinétique v3/HOME, zéro vitesse, aide/tuto/Danger 9/campagne 200 et génération Android réelle.

## Validation restante

Validation physique par Fab sur téléphone : densité/lecture visuelle du blanc 100 % vs 10 %, caractère quasi statique de la purge, trajectoire des fragments blancs 10 %, rendu local des flashes rouges et sensation de poussée des survivants. Aucun merge/release avant cet accord.
