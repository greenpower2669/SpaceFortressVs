# brainmap.md — SpaceFortressVs

## Carte rapide de reprise — 4 octobre 2026

### Dépôt / branches

- Dépôt : `greenpower2669/SpaceFortressVs`
- Branche campagne : `fix/gameplay-campaign-200`
- Branche cible finale : `main`
- `main` et la branche campagne sont historiquement divergentes.
  - merge-base : `57b401a341dc07ed6ebe2c790a7ab96f389c1e27`
  - `main` possède 5 commits uniques.
  - campagne possède 184 commits d'avance.
- Règle de fusion finale : créer un vrai merge conservant les deux parents ; l'arbre fonctionnel doit rester celui de la branche campagne vérifiée, sauf conflit prouvé nécessitant une résolution explicite.

## Sources et responsabilités

### Historique
- `src/main.cpp`
  - source historique de référence ; ne pas réécrire.
  - blob canonique vérifié : `835059a0ecfe0f74708068b3259cad5db1cdb579`.

### Modes / accueil
- `src/game_mode.hpp` : modes duel/coop.
- `src/boss_danger.hpp` : sélecteur de danger HOME et coefficients internes 1/5/10/15/20.
- `src/start_ui.hpp` : dessin HOME + ligne de danger + bouton de lancement.
- `src/remaster_ai_fix.hpp` : interception tactile HOME et synchronisation du lancement.

### Campagne 200 / boss
- `src/campaign_runtime.hpp`
  - combat coop campagne ;
  - contact boss, projectiles, charges, HUD, sauvegarde du résultat ;
  - multiplicateur danger appliqué seulement aux chemins non cinétiques.
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
  - événements de charge/prêt/décharge.
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
→ calcul masse relative 0..1
→ vitesse relative
→ composante réellement fermante
→ dégâts bruts max 250 PV à référence complète
→ couche cinétique externe/interne
→ coût énergie cinétique
→ éventuelle fragmentation/déviation
→ si résiduel atteint réellement la coque : bouclier historique puis PV

Interdits :
- aucun × danger boss ;
- aucun plancher artificiel de dégâts ;
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
→ si `nrj > 0` : recharge la réserve jusqu'à 0
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

Dernière validation complète connue avant cette réorganisation : workflow Android `37166112893`, HEAD `b56b883d2499fb41c4bd26cb39e067c527201f28`, succès complet.

## Packaging / release

- `android/version.properties` : 1.4.0 / code 10.
- `scripts/package-release.py` : vérifie manifest, ARM64, archives, PNG, certificat, SHA.
- `scripts/publish-release.py` : refuse de remplacer une autre cible ou un asset différent ; crée d'abord un draft, vérifie les digests, puis publie.
- `docs/releases/1.4.0.md` : source des notes publiques de release.
- Release v1.4.0 encore à publier après la nouvelle vérification du HEAD mémoire.

## Invariants de livraison

- Pas de publication d'un artefact dont le manifest `-build.json` ne pointe pas vers le SHA exact du workflow de publication.
- Pas de merge `main` avant release publique vérifiée.
- Après merge : lancer une vérification fraîche de `main`.
- APK 1.4.0 signé debug avec certificat différent de v1.3.1 : test uniquement, ne pas effacer les données Android.
