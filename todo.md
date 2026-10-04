# todo.md — SpaceFortressVs

## Priorité actuelle — 4 octobre 2026

### À faire maintenant

- [ ] Revalider Android sur le HEAD contenant la réorganisation des mémoires et les notes 1.4.0 actualisées.
- [ ] Vérifier que le `-build.json` produit pointe vers ce SHA exact.
- [ ] Publier la release GitHub **v1.4.0** avec APK, AAB, manifest de build et `SHA256SUMS` issus de ce même SHA.
- [ ] Vérifier la release publique : tag, cible, assets, tailles et digests.
- [ ] Ensuite seulement fusionner `fix/gameplay-campaign-200` dans `main`.
- [ ] La fusion doit conserver les 5 commits uniques de `main` comme deuxième histoire de merge ; aucun écrasement de l'historique.
- [ ] Vérifier `main` après fusion avec une exécution fraîche.
- [ ] Synchroniser une dernière fois les 4 mémoires avec le SHA de release et le SHA final de `main`.

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
- [x] Contact boss non cinétique rendu fortement dépendant du danger choisi.
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
- [x] Suite complète Python + C++ + SDL/UBSan + compat Android + legacy field verte sur le build de référence `b56b883d2499fb41c4bd26cb39e067c527201f28`.
- [x] Gradle APK/AAB + packaging de référence verts : workflow `37166112893`.

## Artefacts de référence avant republication

- APK : `SpaceFortressVs-1.4.0.apk`
  - taille : 87 675 757 octets
  - SHA-256 : `ff64f6a4a9a6b6ddbeea347d708e38b2b43e0d10ce9865205c415f717e9bcb66`
- AAB : `SpaceFortressVs-1.4.0-unsigned.aab`
  - taille : 85 168 535 octets
  - SHA-256 : `4e2f8b2f6a0c671095598b03ea9a1825ce9555aa839cdf9a748fe311c7dbb4ae`
- Ces artefacts prouvent l'état `b56b883d…`, mais la release à publier doit être reconstruite après cette réorganisation afin que son manifest corresponde exactement au SHA publié.

## Interdits

- [ ] Ne pas forcer `main` en remplaçant son histoire.
- [ ] Ne pas publier des binaires d'un SHA différent du `target_commitish` de la release.
- [ ] Ne pas retoucher `src/main.cpp` pour ces sujets.
- [ ] Ne pas appliquer le danger boss aux chemins cinétiques.
- [ ] Ne pas redonner de gameplay à la poussière rouge.
- [ ] Ne pas conseiller de désinstaller l'application ou d'effacer ses données à cause de la signature debug différente.
