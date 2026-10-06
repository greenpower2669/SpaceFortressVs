# Archive — SpaceFortressVs 1.4.0 — campagne 200 / kinetic balance v3

Archive de clôture créée le 4 octobre 2026. Elle conserve les faits techniques, décisions, bugs, validations et livraisons retirés des quatre mémoires vivantes afin que `brain.md`, `brainmap.md`, `debughistorical.md` et `todo.md` restent courts et opérationnels.

## 1. Références principales

- Dépôt : `greenpower2669/SpaceFortressVs`.
- Branche de développement historique du lot : `fix/gameplay-campaign-200`.
- Version livrée : **1.4.0**, versionCode **10**.
- Source historique protégée : `src/main.cpp`, blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- Release publique : `v1.4.0`.
- SHA exact publié : `98da8a01175d5091f487232f65872e92d213b321`.
- Workflow de publication : `37181070556` — succès.
- Merge réel à deux parents dans `main` : `c50088092744d18ce50f3ef484a2d706aa76a276`.
- Vérification fraîche de `main` après merge : SHA `a30ca6ed4b4679ad9bd69ba8a145f602e9e3cb8b`, workflow `37181486670` — succès complet.
- Fab confirme ensuite l'APK **OK sur téléphone** le 4 octobre 2026.

## 2. Pourquoi l'historique de branche était spécial

`main` et `fix/gameplay-campaign-200` avaient divergé depuis le merge-base `57b401a341dc07ed6ebe2c790a7ab96f389c1e27`.

Au moment de la clôture :
- `main` possédait 5 commits uniques ;
- la branche campagne possédait 184 commits d'avance.

La fusion n'a donc pas été faite par force-push ni remplacement de l'histoire. Un vrai commit de merge à deux parents a été créé afin de préserver les 5 commits historiques de `main` tout en prenant l'arbre fonctionnel vérifié de la campagne.

## 3. Campagne 200

Canon livré :
- 200 affrontements = 50 boss × 4 difficultés ;
- `sfBossIndex(encounter) = encounter % 50` ;
- `sfDifficultyIndex(encounter) = encounter / 50` ;
- sauvegardes v1 et v2 lues ;
- migration v1→v2 sans perte de progression, noms UTF-8, Hall of Fame ni victoire en attente ;
- signature visuelle des niveaux : 0 / 4 / 8 / 20 tentacules, avec auras verte / jaune / rouge à partir du niveau 2 ;
- même rendu de boss en sélection et en combat.

## 4. Retour téléphone historique et correctifs associés

Les essais téléphone successifs avaient révélé :
- astéroïdes invisibles en classique ;
- missile coop affiché comme un plasma ;
- survie trop facile / récupération trop forte ;
- HUD pilotes insuffisant ;
- tourelles perçues comme trop destructrices ;
- contact boss trop peu dangereux ;
- impression de guidage sur tirs alliés ;
- énergie insuffisamment couplée à la cadence/précision ;
- impacts d'astéroïdes mal équilibrés.

Corrections livrées :
- restauration du rendu astéroïdes Android sans modifier `src/main.cpp` ;
- texture missile historique dédiée ;
- HUD coop double orientation, joueur haut à 180°, vie boss miroir vert→rouge ;
- cadence et dispersion initiale liées à la réserve historique ;
- tirs ordinaires rectilignes après le départ ;
- chemins cinétiques séparés des dégâts non cinétiques du boss ;
- modèle cinétique v3 détaillé ci-dessous.

## 5. Cause du bug des astéroïdes invisibles

Cause prouvée : dans la source Android générée, la résolution de `setw` tombait sur `std::setw` au lieu de la fonction historique `fablib::setw(W)`. La largeur historique restait donc nulle et `inxy()` rejetait le rendu.

Correctif : qualification dans la copie Android générée uniquement. `src/main.cpp` historique n'a pas été modifié.

## 6. Sémantique énergie historique

Invariant important :
- `nrj = 0` = réserve pleine ;
- `nrj = 50` = réserve épuisée.

Conséquences :
- cadence et précision maximales à réserve pleine ;
- dégradation progressive à mesure que `nrj` augmente ;
- le bouclier historique dépend de la réserve ;
- ne jamais convertir cette convention en pourcentage direct sans adapter le sens.

## 7. Poussières

### Poussière blanche

Canon final :
- ressource gameplay ;
- recharge d'abord la réserve jusqu'à `nrj=0` ;
- une fois la réserve pleine, les collectes suivantes restaurent fortement la coque ;
- jamais physiquement déviée par le champ cinétique.

### Poussière rouge

Anciennes idées de chaleur, dégâts ou recharge supprimées.

Canon final : **strictement visuelle** :
- aucun dégât ;
- aucune consommation d'énergie ;
- aucun soin ;
- aucune modification de bouclier ;
- aucune statistique gameplay ;
- peut seulement vibrer, se déplacer ou être déviée pour matérialiser les fronts de champ.

## 8. Danger boss depuis HOME

Le sélecteur n'affiche plus les coefficients techniques. Chaque clic sur sa ligne change le niveau affiché et ne lance jamais le combat. Seul le bouton `LANCER LA PARTIE` démarre la partie.

Ordre cyclique :
1. `MOU DU GENOU` = ×1
2. `CHILL` = ×5
3. `ROCK N ROLL` = ×10 — défaut
4. `DUR A CUIRE` = ×15
5. `MACHINE DE GUERRE` = ×20

Le multiplicateur s'applique uniquement aux attaques/contact **non cinétiques** du boss. Il ne s'applique jamais aux astéroïdes ni aux charges explicitement cinétiques.

## 9. Kinetic balance v3

L'ancien modèle pouvait combiner plancher de dégâts et dépendance quadratique de vitesse, créant des dégâts artificiels même dans des situations presque statiques.

Canon final :

`dégâts bruts = 250 PV × masseRelative × vitesseRelativeNormalisée × fermeture`

avec :
- `masseRelative` bornée dans `[0,1]` ;
- vitesse normalisée bornée à la vitesse de référence ;
- `fermeture` = composante réellement dirigée vers le vaisseau ;
- aucun plancher artificiel ;
- aucun carré de vitesse.

Référence : plus gros astéroïde, vitesse de référence, plein face = **250 PV bruts**, soit 25 % d'une coque canonique à 1000 PV avant protections.

Un objet strictement immobile n'invente donc aucun dégât cinétique.

## 10. Chaîne d'impact cinétique

Astéroïde réel
→ masse / vitesse relative / fermeture
→ dégâts bruts
→ couches cinétiques externe/interne
→ dépense / dissipation cinétique
→ fragmentation ou déviation éventuelle
→ si un résiduel atteint réellement la coque : bouclier historique puis PV.

Interdits :
- aucun multiplicateur danger boss sur cette chaîne ;
- aucune double application « interaction de couche + collision coque pleine » dans la même étape ;
- aucune collision statique fictive utilisée comme preuve de dégâts.

## 11. Dernière régression historique corrigée

Le dernier échec de `legacy_field_regressions.cpp` venait d'un test qui plaçait un astéroïde immobile directement sur la coque et exigeait `pv < 1000`.

Ce test contredisait la nouvelle physique. Il a été corrigé pour simuler un véritable impact entrant, avec vitesse vers la coque et passage par `sfCoopAsteroidHurt` côté coop.

Après correction, toutes les suites sont devenues vertes.

## 12. Vagues cinétiques

Canon final commun classique + coop :
- toutes les vagues naissent au centre du vaisseau ;
- elles progressent clairement vers l'extérieur ;
- elles grandissent jusqu'à leur rayon d'interception ;
- elles s'effacent progressivement ;
- plusieurs fronts peuvent se chevaucher ;
- plus de cercle statique apparaissant directement à son rayon final.

Le rendu vise un effet de « rouleau compresseur » centre→extérieur.

## 13. Surcharge cinétique deux doigts

Canon final :
- maintien du deuxième doigt : charge **2,00 s** ;
- pendant `0 < t < 2 s` : puissance cinétique ×2, rendu arc-en-ciel irisé, son de charge ;
- à 2 s : son bref de prêt puis **coupure complète du bouclier cinétique tant que le doigt reste posé** ;
- cette vulnérabilité après armement est volontaire ;
- relâchement après charge complète : décharge sonore + purge des astéroïdes proches ;
- relâchement avant 2 s : pas de purge complète, comportement tap/tir préservé.

Sons dédiés :
- `kinetic_charge.wav` ;
- `kinetic_ready.wav` ;
- `kinetic_release.wav`.

## 14. Validation technique avant release

HEAD de publication : `98da8a01175d5091f487232f65872e92d213b321`.

Workflow Android : `37180765011` — succès sur :
- Python intégration/assets/release/scenic/kinetic ;
- régressions C++ ciblées ;
- suite SDL/UBSan complète ;
- compatibilité source Android générée ;
- champ historique ;
- Gradle APK + AAB ;
- vérification ARM64, assets, archives, versions et signature ;
- packaging des fichiers de release.

Artefact vérifié : `SpaceFortressVs-1.4.0-release-files`, id `11295174639`.

## 15. Release publique v1.4.0

Release GitHub : `v1.4.0`, cible exacte `98da8a01175d5091f487232f65872e92d213b321`.

Fichiers publiés :
- `SpaceFortressVs-1.4.0.apk`
  - taille : **87 675 758 octets**
  - SHA-256 : `158bb86b07f353fcaeade3b65b522abf94f88e83feff2ab343f43d01d411d1d9`
- `SpaceFortressVs-1.4.0-unsigned.aab`
  - taille : **85 168 535 octets**
  - SHA-256 : `4e2f8b2f6a0c671095598b03ea9a1825ce9555aa839cdf9a748fe311c7dbb4ae`
- `SpaceFortressVs-1.4.0-build.json`
  - SHA-256 : `ff450cbb9808c4080eef08ede519b26c9f04d404d3802eeffba82626b52022d9`
- `SHA256SUMS`
  - digest GitHub : `d40a98a69f0d5ffbd6b9d8b90b1759de5395225d491f69ab3263ff3ea1c1b77f`

Certificat APK publié :
`22943f8846ebaf3191d011b1d947883d66ff6f25e566c3a966b98f879f070172`

Certificat de référence v1.3.1 :
`8abfc11c8bc4f9ac065eb5c086ad4e457290bcbbc1105017865368de7e565868`

Les signatures diffèrent. L'APK 1.4.0 CI est donc un APK de test non installable comme mise à jour directe de la v1.3.1 existante. Ne jamais demander de désinstaller l'ancienne application ni d'effacer ses données pour contourner ce point.

## 16. Merge et validation de main

Merge à deux parents :
`c50088092744d18ce50f3ef484a2d706aa76a276`.

Après synchronisation documentaire, le SHA `main` testé fut :
`a30ca6ed4b4679ad9bd69ba8a145f602e9e3cb8b`.

Workflow `37181486670` :
- régressions runtime : GREEN ;
- Gradle APK/AAB : GREEN ;
- contrôle du build : GREEN ;
- packaging/versionnage : GREEN.

Fab confirme ensuite l'APK **OK** sur téléphone. Cette validation utilisateur clôt le lot sans nouvelle correction gameplay demandée.

## 17. Règles à conserver pour les futures reprises

- Lire le code réel avant toute modification ; ne pas recoder depuis la mémoire.
- Préserver `src/main.cpp` historique.
- Garder `brain.md`, `brainmap.md`, `debughistorical.md` et `todo.md` courts et synchronisés.
- Utiliser cette archive et l'historique Git lorsqu'un ancien détail devient nécessaire.
- `ordres-de-mission.md` reste le contrat de mission.
- Ne pas appliquer le danger boss aux chemins cinétiques.
- Ne pas redonner de gameplay à la poussière rouge.
- Ne pas modifier la convention historique `nrj` sans migration explicite.
- Ne pas republier un binaire si son `-build.json` ne correspond pas exactement au SHA de la release.
