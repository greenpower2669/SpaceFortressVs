# brain.md — SpaceFortressVs

## État canonique — 4 octobre 2026

- Dépôt : `greenpower2669/SpaceFortressVs`.
- Branche de travail validée : `fix/gameplay-campaign-200`.
- Version : **1.4.0**, versionCode **10**.
- HEAD Android entièrement vérifié avant réorganisation mémoire : `b56b883d2499fb41c4bd26cb39e067c527201f28`.
- Workflow Android vert : `37166112893`.
- `src/main.cpp` historique reste strictement inchangé, blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- Release publique **v1.4.0 : à publier après cette synchronisation mémoire**.
- Fusion `main` : **à faire uniquement après publication de la release**, autorisation explicite de Fab donnée le 4 octobre 2026.
- `main` est historiquement divergent : 5 commits uniques côté `main`, 184 commits d'avance côté campagne. La fusion finale doit conserver les deux histoires ; ne jamais forcer `main` sur l'arbre de campagne sans parent de merge.

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
- Poussière blanche : ressource ; elle recharge d'abord réellement la réserve jusqu'à `nrj=0`, puis soigne fortement les PV sur les poussières suivantes.
- Poussière rouge : **strictement visuelle** ; aucune recharge, aucun soin, aucune chaleur, aucune mutation de gameplay. Elle peut être animée/déviée par les vagues cinétiques.

## Modèle cinétique canonique v3

- Astéroïdes et charges explicites du boss utilisent le modèle cinétique partagé ; ils ne reçoivent jamais le multiplicateur de danger boss.
- Dégâts astéroïde : loi linéaire **masse × vitesse relative × composante de fermeture**.
- Masse normalisée dans `[0,1]`, vitesse bornée à la vitesse de référence, pas de plancher artificiel de dégâts.
- Référence maximale : plus gros astéroïde, vitesse relative de référence et fermeture complète = **250 PV**, soit 25 % de la coque canonique à 1000 PV avant protections.
- Les couches cinétiques dissipent l'impact avant le bouclier historique/la coque.
- Une interaction réelle avec une couche ne doit pas retomber immédiatement sur une collision coque complète dans la même étape.
- Toutes les vagues cinétiques visibles naissent au centre du vaisseau et progressent vers l'extérieur ; plus de cercles permanents fixes.

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

## Vérification fraîche de référence

Workflow `37166112893`, HEAD `b56b883d2499fb41c4bd26cb39e067c527201f28` :

- assets et suites Python : succès ;
- régressions cinétiques ciblées : succès ;
- suite native SDL/UBSan complète : succès ;
- source Android générée et compilation syntaxique : succès ;
- régressions du champ historique : succès ;
- Gradle APK + AAB : succès ;
- packaging et vérification des archives/assets/signature : succès ;
- publication automatique : volontairement skippée sur la branche campagne.

Artefacts vérifiés :
- `SpaceFortressVs-1.4.0.apk` — 87 675 757 octets — SHA-256 `ff64f6a4a9a6b6ddbeea347d708e38b2b43e0d10ce9865205c415f717e9bcb66`.
- `SpaceFortressVs-1.4.0-unsigned.aab` — 85 168 535 octets — SHA-256 `4e2f8b2f6a0c671095598b03ea9a1825ce9555aa839cdf9a748fe311c7dbb4ae`.
- certificat APK : `e7fe36619f76ea2c5e0cede5ebd6eafc734e31ae8e6bd21de1f86e2d6268700c`.
- certificat v1.3.1 publié : `8abfc11c8bc4f9ac065eb5c086ad4e457290bcbbc1105017865368de7e565868`.
- signatures différentes : APK 1.4.0 de test, non installable comme mise à jour directe de la v1.3.1.

## Ce qui reste après cette synchronisation

1. Refaire une vérification Android sur le HEAD contenant ces mémoires et les notes de release actualisées.
2. Publier **v1.4.0** avec les artefacts produits par ce HEAD exact.
3. Vérifier la release publique et ses checksums.
4. Fusionner ensuite `fix/gameplay-campaign-200` dans `main` en conservant les 5 commits uniques de `main` comme deuxième histoire de merge.
5. Vérifier `main` après fusion.
6. Validation téléphone finale de Fab : danger HOME, sensation des dégâts astéroïdes, poussières, vagues centre→extérieur, surcharge arc-en-ciel/sons/vulnérabilité, stabilité générale.
