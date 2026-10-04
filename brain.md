# brain.md — SpaceFortressVs

## État canonique — 4 octobre 2026

- Dépôt : `greenpower2669/SpaceFortressVs`.
- Version publique : **1.4.0**, versionCode **10**.
- Release GitHub **v1.4.0 publiée et vérifiée** : `https://github.com/greenpower2669/SpaceFortressVs/releases/tag/v1.4.0`.
- Commit exact construit et ciblé par la release : `98da8a01175d5091f487232f65872e92d213b321`.
- Workflow Android de publication : `37180765011` — tests, Gradle et packaging verts.
- `main` a été fusionné par un vrai merge à deux parents : `c50088092744d18ce50f3ef484a2d706aa76a276`.
- Parents du merge : ancien `main` `45e43f9146010dd945a492393f7e44be2026ad7a` + branche campagne nettoyée `098b88302ecf29654ae9a6c7d18a8352e7a1e5e8`.
- Les 5 commits historiquement uniques de `main` sont donc conservés ; aucun force-push n'a remplacé son histoire.
- `src/main.cpp` historique reste strictement inchangé, blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- Vérification Android fraîche du `main` fusionné : **à exécuter avant clôture**.

## Contrat de travail permanent

- Fab décide et valide sur téléphone.
- Astra analyse / rédige les missions ; Sol exécute code, tests et builds.
- Les quatre mémoires vivantes `brain.md`, `brainmap.md`, `debughistorical.md`, `todo.md` doivent rester synchronisées.
- `ordres-de-mission.md` reste le contrat de mission.
- Ne jamais recoder un comportement historique depuis la mémoire : lire le code réel.
- Préserver `src/main.cpp` autant que possible ; les adaptations Android se font dans la copie générée/runtime dédié.
- Ne jamais demander de désinstaller l'application ni d'effacer les données pour contourner un problème de signature.

## Campagne et présentation

- 200 affrontements = 50 boss × 4 difficultés de campagne.
- Sauvegardes v1 et v2 lues ; migration v1→v2 avec conservation progression, noms, Hall of Fame et victoire en attente.
- Difficultés visuelles campagne : niveau 1 sans aura ; niveaux 2/3/4 avec aura verte/jaune/rouge et 4/8/20 tentacules.
- Même logique de rendu en sélection et en combat.
- HUD coop : informations lisibles depuis les deux côtés, joueur haut à 180°, vie boss miroir, gradient vert→rouge.
- Missile coop utilise l'apparence historique dédiée ; les tirs ordinaires restent rectilignes après leur visée initiale.

## Énergie, boucliers et poussières

- Sémantique historique `nrj` : **0 = réserve pleine**, **50 = réserve épuisée**.
- Cadence et précision se dégradent avec la réserve.
- Le bouclier historique dépend de cette réserve.
- Poussière blanche : recharge d'abord réellement la réserve jusqu'à `nrj=0`, puis soigne fortement les PV sur les poussières suivantes.
- Poussière rouge : **strictement visuelle** ; aucune recharge, aucun soin, aucune chaleur, aucune mutation de gameplay. Elle peut être animée/déviée par les vagues cinétiques.

## Modèle cinétique canonique v3

- Astéroïdes et charges explicites du boss utilisent le modèle cinétique partagé ; ils ne reçoivent jamais le multiplicateur de danger boss.
- Dégâts astéroïde : loi linéaire **masse × vitesse relative × composante de fermeture**.
- Masse normalisée dans `[0,1]`, vitesse bornée à la vitesse de référence, pas de plancher artificiel de dégâts.
- Référence maximale : plus gros astéroïde, vitesse relative de référence et fermeture complète = **250 PV**, soit 25 % de la coque canonique à 1000 PV avant protections.
- Les couches cinétiques dissipent l'impact avant le bouclier historique/la coque.
- Une interaction réelle avec une couche ne retombe pas immédiatement sur une collision coque complète dans la même étape.
- Toutes les vagues cinétiques visibles naissent au centre du vaisseau et progressent vers l'extérieur.

## Surcharge cinétique deux doigts

- Maintien du deuxième doigt : charge de **2,00 s**.
- Pendant `0 < t < 2 s` : champ cinétique ×2, rendu irisé arc-en-ciel, son de charge.
- À `t >= 2 s` tant que le doigt reste posé : état armé, son « prêt », **aucun bouclier cinétique** ; fenêtre de vulnérabilité volontaire.
- Relâchement après charge complète : décharge sonore + purge des astéroïdes dans le rayon prévu ; la matière purgée produit la poussière blanche historique.
- Relâchement avant 2 s : annulation de la charge et comportement tap/tir préservé, sans décharge complète.

## Danger boss sélectionnable depuis HOME

Les coefficients sont internes ; l'interface montre uniquement les noms :

- `MOU DU GENOU` = ×1
- `CHILL` = ×5
- `ROCK N ROLL` = ×10 — défaut
- `DUR A CUIRE` = ×15
- `MACHINE DE GUERRE` = ×20

Règles :
- chaque tap sur la ligne de danger passe au niveau suivant ;
- la zone tactile est séparée du bouton de lancement ;
- le multiplicateur s'applique aux attaques/contact **non cinétiques** du boss ;
- il ne s'applique ni aux astéroïdes ni aux charges cinétiques explicites.

## Release publique v1.4.0 — artefacts canoniques

La release cible exactement `98da8a01175d5091f487232f65872e92d213b321` et contient :

- `SpaceFortressVs-1.4.0.apk`
  - taille : **87 675 758 octets**
  - SHA-256 : `158bb86b07f353fcaeade3b65b522abf94f88e83feff2ab343f43d01d411d1d9`
- `SpaceFortressVs-1.4.0-unsigned.aab`
  - taille : **85 168 535 octets**
  - SHA-256 : `4e2f8b2f6a0c671095598b03ea9a1825ce9555aa839cdf9a748fe311c7dbb4ae`
- `SpaceFortressVs-1.4.0-build.json`
  - SHA-256 : `ff450cbb9808c4080eef08ede519b26c9f04d404d3802eeffba82626b52022d9`
- `SHA256SUMS`
  - SHA-256 : `d40a98a69f0d5ffbd6b9d8b90b1759de5395225d491f69ab3263ff3ea1c1b77f`

Identité APK :
- certificat 1.4.0 publié : `22943f8846ebaf3191d011b1d947883d66ff6f25e566c3a966b98f879f070172` ;
- certificat v1.3.1 publié : `8abfc11c8bc4f9ac065eb5c086ad4e457290bcbbc1105017865368de7e565868` ;
- signatures différentes : APK 1.4.0 de test, non installable comme mise à jour directe de la v1.3.1.

## Intégration `main`

- Ancienne divergence : merge-base `57b401a341dc07ed6ebe2c790a7ab96f389c1e27`, 5 commits uniques côté `main`, 184+ côté campagne.
- Fusion réalisée sans force : commit `c50088092744d18ce50f3ef484a2d706aa76a276`.
- L'arbre du merge correspond à l'arbre vérifié de la branche campagne après retrait du workflow one-shot de publication.
- Le workflow de publication temporaire `publish-1.4.0-once.yml` a été retiré après succès.

## Reste à faire avant clôture

1. Vérification Android fraîche du `main` fusionné avec les mémoires finales.
2. Inscrire le SHA et le workflow verts de cette vérification dans les quatre mémoires.
3. Validation téléphone finale de Fab : danger HOME, sensation des dégâts astéroïdes, poussières, vagues centre→extérieur, surcharge arc-en-ciel/sons/vulnérabilité, stabilité générale.
