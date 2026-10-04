# todo.md — SpaceFortressVs

## Priorité actuelle — 4 octobre 2026

### Livraison / intégration

- [x] Réorganiser et synchroniser `brain.md`, `brainmap.md`, `debughistorical.md`, `todo.md`.
- [x] Revalider Android sur le HEAD de publication `98da8a01175d5091f487232f65872e92d213b321`.
- [x] Vérifier que le `-build.json` pointe vers ce SHA exact.
- [x] Publier la release GitHub **v1.4.0** avec APK, AAB, manifest de build et `SHA256SUMS` de ce même SHA.
- [x] Vérifier la release publique : tag, cible, assets, tailles et digests.
- [x] Fusionner `fix/gameplay-campaign-200` dans `main` sans écraser les 5 commits uniques de l'ancien `main`.
- [x] Créer un vrai merge à deux parents : `c50088092744d18ce50f3ef484a2d706aa76a276`.
- [ ] Vérifier Android sur le `main` fusionné et documenté avec une exécution fraîche.
- [ ] Inscrire le SHA/workflow GREEN final de `main` dans les quatre mémoires.

### Validation téléphone après livraison

- [ ] Le sélecteur de danger HOME passe bien par les 5 noms et ne lance pas le combat lorsqu'on le touche.
- [ ] `ROCK N ROLL` est bien la difficulté par défaut.
- [ ] Les dégâts astéroïdes sont cohérents : pas de dégât inventé à vitesse nulle, gros impact de référence plafonné à 250 PV bruts avant protections.
- [ ] La poussière rouge est purement visuelle.
- [ ] La poussière blanche recharge d'abord complètement l'énergie puis soigne.
- [ ] Toutes les vagues sont lisibles centre→extérieur.
- [ ] Surcharge : 2 s de charge, arc-en-ciel, son prêt, fenêtre vulnérable sans bouclier cinétique, puis purge au relâchement.
- [ ] Relâchement avant 2 s : pas de purge complète, tap/tir conservé.
- [ ] Stabilité coop / classique, veille-reprise, son et performances.

## Terminé — lot campagne 200 / kinetic balance v3

- [x] Campagne 200 : 50 boss × 4 difficultés.
- [x] Migration sauvegardes v1→v2 et Hall of Fame.
- [x] Signatures visuelles difficultés 0/4/8/20 tentacules + auras.
- [x] Rendu astéroïdes Android restauré sans modifier `src/main.cpp`.
- [x] Missile coop visuellement distinct.
- [x] HUD coop double orientation + vie boss.
- [x] Tirs ordinaires non guidés en vol ; prédiction initiale IA conservée.
- [x] Cadence/précision liées à la réserve historique.
- [x] Contact boss non cinétique dépend du danger choisi.
- [x] Astéroïdes et charges explicites séparés du multiplicateur danger boss.
- [x] Modèle cinétique linéaire masse × vitesse relative × fermeture.
- [x] Suppression du plancher de dégâts cinétiques.
- [x] Dégât brut astéroïde de référence plafonné à 250 PV.
- [x] Couches cinétiques empêchent une double collision coque dans la même étape.
- [x] Poussière rouge rendue strictement visuelle.
- [x] Poussière blanche : énergie d'abord, soin ensuite.
- [x] Vagues centre→extérieur renforcées visuellement.
- [x] Surcharge 2 doigts : 2 s, ×2 pendant charge, arc-en-ciel, sons, vulnérabilité après armement, purge au relâchement.
- [x] Danger HOME : `MOU DU GENOU`, `CHILL`, `ROCK N ROLL`, `DUR A CUIRE`, `MACHINE DE GUERRE`.
- [x] Défaut danger = ×10 / `ROCK N ROLL`.

## Release publique v1.4.0

- URL : `https://github.com/greenpower2669/SpaceFortressVs/releases/tag/v1.4.0`
- cible : `98da8a01175d5091f487232f65872e92d213b321`
- workflow Android : `37180765011`
- APK : 87 675 758 octets — SHA-256 `158bb86b07f353fcaeade3b65b522abf94f88e83feff2ab343f43d01d411d1d9`
- AAB : 85 168 535 octets — SHA-256 `4e2f8b2f6a0c671095598b03ea9a1825ce9555aa839cdf9a748fe311c7dbb4ae`
- build manifest : SHA-256 `ff450cbb9808c4080eef08ede519b26c9f04d404d3802eeffba82626b52022d9`
- `SHA256SUMS` : SHA-256 `d40a98a69f0d5ffbd6b9d8b90b1759de5395225d491f69ab3263ff3ea1c1b77f`
- certificat APK : `22943f8846ebaf3191d011b1d947883d66ff6f25e566c3a966b98f879f070172`
- signature différente de v1.3.1 : APK de test uniquement.

## Interdits permanents

- [ ] Ne pas forcer `main` en remplaçant son histoire.
- [ ] Ne pas publier des binaires d'un SHA différent du `target_commitish` de la release.
- [ ] Ne pas retoucher `src/main.cpp` pour ces sujets.
- [ ] Ne pas appliquer le danger boss aux chemins cinétiques.
- [ ] Ne pas redonner de gameplay à la poussière rouge.
- [ ] Ne pas conseiller de désinstaller l'application ou d'effacer ses données à cause de la signature debug différente.
