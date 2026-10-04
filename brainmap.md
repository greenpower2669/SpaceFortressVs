# brainmap.md — SpaceFortressVs

## Carte rapide de reprise — 4 octobre 2026

### État Git / livraison

- Dépôt : `greenpower2669/SpaceFortressVs`
- Version publique : **v1.4.0**
- Release : `https://github.com/greenpower2669/SpaceFortressVs/releases/tag/v1.4.0`
- Commit de release : `98da8a01175d5091f487232f65872e92d213b321`
- Workflow Android de release : `37180765011` — GREEN complet.
- Merge `main` : `c50088092744d18ce50f3ef484a2d706aa76a276`
- Parents du merge :
  1. ancien `main` `45e43f9146010dd945a492393f7e44be2026ad7a`
  2. branche campagne nettoyée `098b88302ecf29654ae9a6c7d18a8352e7a1e5e8`
- Les 5 commits uniques de l'ancien `main` sont conservés.
- Vérification Android du `main` final : à exécuter avant clôture.

## Sources et responsabilités

### Historique
- `src/main.cpp`
  - source historique de référence ; ne pas réécrire.
  - blob canonique : `835059a0ecfe0f74708068b3259cad5db1cdb579`.

### Modes / accueil
- `src/game_mode.hpp` : modes duel/coop.
- `src/boss_danger.hpp` : sélecteur de danger HOME et coefficients internes 1/5/10/15/20.
- `src/start_ui.hpp` : dessin HOME + ligne de danger + bouton de lancement.
- `src/remaster_ai_fix.hpp` : interception tactile HOME et synchronisation du lancement.

### Campagne 200 / boss
- `src/campaign_runtime.hpp`
  - combat coop campagne ;
  - contact boss, projectiles, charges, HUD, sauvegarde du résultat ;
  - multiplicateur danger seulement sur les chemins non cinétiques.
- `src/boss_catalog.hpp` : 50 identités.
- `src/boss_difficulty_visuals.hpp` : signature visuelle 0/4/8/20 tentacules + auras.
- `src/campaign_save.hpp` : formats v1/v2, migration et Hall of Fame.

### Énergie / tir / bouclier historique
- `src/ship_energy.hpp`
  - `nrj=0` plein ; `nrj=50` épuisé ;
  - cadence, précision, dépense, bouclier historique.
- `src/tactical_runtime.hpp`
  - collecte poussière blanche ;
  - rendu des vagues cinétiques et effets tactiques ;
  - HUD tactique.

### Cinétique
- `src/kinetic_shield.hpp`
  - modèle physique partagé ;
  - masse, vitesse relative, fermeture, couches ;
  - surcharge deux doigts 2 s + état armé vulnérable ;
  - événements charge/prêt/décharge.
- `src/legacy_field_runtime.hpp`
  - astéroïdes réels du champ historique ;
  - interception par couches cinétiques ;
  - fragmentation/déviation ;
  - collision coque résiduelle ;
  - poussières rouges visuelles et poussières blanches non déviées.
- `src/remaster_runtime.hpp`
  - intégration runtime classique/remaster et audio cinétique.

## Flux gameplay canoniques

### Astéroïde → vaisseau

`astéroïde historique`
→ masse relative 0..1
→ vitesse relative
→ composante réellement fermante
→ dégâts bruts max 250 PV à référence complète
→ couche cinétique externe/interne
→ coût énergie cinétique
→ éventuelle fragmentation/déviation
→ si résiduel atteint réellement la coque : bouclier historique puis PV

Interdits :
- aucun × danger boss ;
- aucun plancher artificiel ;
- aucune double collision coque dans la même étape après interaction de couche.

### Charge explicite boss → vaisseau

`sfCoopBossContact` + `chargeActive`
→ `sfResolveKinetic`
→ couche cinétique selon réserve/surcharge
→ énergie + dégâts résiduels
→ bouclier historique/coque

Interdit : multiplier le résiduel cinétique par le danger boss.

### Contact ordinaire / attaques boss

projectile / rayon / vague / contact non cinétique
→ `sfBossDangerMultiplier()`
→ bouclier historique
→ PV

### Poussière blanche

collecte
→ si `nrj > 0` : recharge jusqu'à 0
→ sinon : soin PV fort
→ disparition de la particule collectée

### Poussière rouge

impact visuel
→ déplacement/vibration/déviation éventuels par les vagues
→ aucune mutation énergie/PV/bouclier

### Surcharge deux doigts

2e doigt DOWN
→ son charge
→ 0..2 s : puissance cinétique ×2 + arc-en-ciel
→ à 2 s : son prêt + bouclier cinétique OFF
→ attente vulnérable tant que le doigt reste posé
→ UP : purge astéroïdes + son décharge + retour au champ normal

UP avant 2 s
→ annulation surcharge
→ pas de purge complète
→ tap/tir préservé

## Danger HOME

Ordre cyclique :

1. `MOU DU GENOU` — ×1
2. `CHILL` — ×5
3. `ROCK N ROLL` — ×10 — défaut
4. `DUR A CUIRE` — ×15
5. `MACHINE DE GUERRE` — ×20

L'interface n'affiche pas le coefficient.

## Tests à connaître

- `tests/test_kinetic_integration.py`
- `tests/test_kinetic_surge_integration.py`
- `tests/test_balance_v3_integration.py`
- `tests/kinetic_regressions.cpp`
- `tests/kinetic_surge_regressions.cpp`
- `tests/kinetic_balance_v3_regressions.cpp`
- `tests/legacy_field_regressions.cpp`
- `tests/regressions.cpp`
- `scripts/test-regressions.sh`

## Packaging / release

- `android/version.properties` : 1.4.0 / code 10.
- `scripts/package-release.py` : vérifie manifest, ARM64, archives, PNG, certificat, SHA.
- `scripts/publish-release.py` : publication protégée par provenance du manifest et digests.
- `docs/releases/1.4.0.md` : notes publiques de la release.
- Workflow one-shot de publication supprimé après usage.

### Artefacts publics v1.4.0

- APK : 87 675 758 octets — `158bb86b07f353fcaeade3b65b522abf94f88e83feff2ab343f43d01d411d1d9`
- AAB : 85 168 535 octets — `4e2f8b2f6a0c671095598b03ea9a1825ce9555aa839cdf9a748fe311c7dbb4ae`
- build manifest : `ff450cbb9808c4080eef08ede519b26c9f04d404d3802eeffba82626b52022d9`
- `SHA256SUMS` : `d40a98a69f0d5ffbd6b9d8b90b1759de5395225d491f69ab3263ff3ea1c1b77f`
- certificat APK : `22943f8846ebaf3191d011b1d947883d66ff6f25e566c3a966b98f879f070172`
- certificat différent de v1.3.1 : APK de test uniquement.

## Invariants de livraison

- Pas de publication d'un artefact dont le manifest ne pointe pas vers le SHA ciblé.
- Un merge historique divergent se fait à deux parents, jamais par force-push.
- `main` doit recevoir une vérification fraîche après fusion.
- APK debug à signature différente : ne pas effacer les données Android.
