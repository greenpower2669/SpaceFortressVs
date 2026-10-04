# brain.md — SpaceFortressVs

## État canonique courant — 4 octobre 2026

- Dépôt : `greenpower2669/SpaceFortressVs`.
- Version livrée : **1.4.0**, versionCode **10**.
- Release publique : **v1.4.0**.
- SHA exact publié : `98da8a01175d5091f487232f65872e92d213b321`.
- Merge réel à deux parents dans `main` : `c50088092744d18ce50f3ef484a2d706aa76a276`.
- Dernier SHA de code/documentation vérifié sur `main` : `a30ca6ed4b4679ad9bd69ba8a145f602e9e3cb8b`.
- Workflow `main` : `37181486670` — GREEN complet.
- Fab confirme l'APK **OK sur téléphone** le 4 octobre 2026.
- `src/main.cpp` historique reste inchangé, blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.

Archive détaillée du lot 1.4.0 : `docs/archive/2026-10-spacefortress-v1.4.0-history.md`.

## Mission active — aide/tutoriel + danger 9

- Branche : `feature/in-game-help-tutorial-danger-9`.
- Design validé par Fab puis écrit dans `docs/superpowers/specs/2026-10-04-in-game-help-tutorial-danger-9-design.md`.
- Aucun code produit tant que la spec écrite n'a pas été relue/confirmée selon la méthode.
- Centre `?` prévu : RAPIDE / DÉTAILLÉ / ANIMÉ / TUTORIEL.
- Tutoriel : sandbox in-game isolé des vraies sauvegardes/stats/progressions.
- Danger : 9 niveaux, de ×1 à ×40 ; défaut **FIN DU MONDE ×40**.
- Nouveaux noms : `SANS PITIE`, `CAUCHEMAR`, `APOCALYPSE`, `FIN DU MONDE`.
- En classique, le danger agit aussi sur les tirs de l'IA adverse : dégâts + cadence plafonnée + vitesse plafonnée + qualité d'anticipation.
- Les tirs ordinaires de l'IA restent **strictement droits après leur création** : anticipation uniquement au départ, jamais de guidage en vol.
- Astéroïdes et charges cinétiques restent hors multiplicateur de danger.

## Canon gameplay à préserver

- Campagne : 200 affrontements = 50 boss × 4 difficultés.
- `nrj=0` = réserve pleine ; `nrj=50` = réserve épuisée.
- Poussière blanche : recharge jusqu'à `nrj=0`, puis soin fort.
- Poussière rouge : **visuelle uniquement**, zéro effet gameplay.
- Chaque clic sur le sélecteur danger change uniquement le niveau ; seul `LANCER LA PARTIE` démarre le combat.
- Astéroïdes : loi linéaire masse × vitesse relative × fermeture, référence max 250 PV bruts.
- Toutes les vagues cinétiques naissent au centre et progressent vers l'extérieur.
- Surcharge deux doigts : 2,00 s ; ×2 pendant charge ; irisation + son ; après 2 s bouclier cinétique OFF jusqu'au relâchement ; relâchement prêt = purge.

## Discipline permanente

- Fab décide et teste ; Astra analyse ; Sol code/teste/build.
- Toujours lire le code réel avant modification.
- Préserver `src/main.cpp` historique.
- Garder synchronisés `brain.md`, `brainmap.md`, `debughistorical.md`, `todo.md`.
- `ordres-de-mission.md` reste le contrat de mission.
- Utiliser l'archive et l'historique Git pour les détails anciens plutôt que regonfler la mémoire vivante.
- Ne jamais conseiller désinstallation/effacement des données pour contourner une signature Android différente.

## Livraison 1.4.0

APK publié :
- `SpaceFortressVs-1.4.0.apk`
- 87 675 758 octets
- SHA-256 `158bb86b07f353fcaeade3b65b522abf94f88e83feff2ab343f43d01d411d1d9`

AAB publié :
- `SpaceFortressVs-1.4.0-unsigned.aab`
- 85 168 535 octets
- SHA-256 `4e2f8b2f6a0c671095598b03ea9a1825ce9555aa839cdf9a748fe311c7dbb4ae`

La signature diffère de la v1.3.1 ; APK CI de test uniquement.
