# debughistorical.md — SpaceFortressVs

## Historique technique condensé — état au 4 octobre 2026

Ce fichier conserve les décisions et causes prouvées. Les détails plus anciens restent dans l'historique Git ; ne pas réintroduire un comportement à partir d'un souvenir non vérifié.

## 1 — Base historique / Android

### Astéroïdes invisibles en classique
- Symptôme téléphone : poussières/effets présents mais astéroïdes invisibles.
- Cause prouvée : confusion entre `std::setw` et la fonction historique `fablib::setw(W)` dans la source Android générée ; `W` restait nul et `inxy()` rejetait le rendu.
- Correctif : qualifier uniquement la copie Android générée ; `src/main.cpp` reste inchangé.
- Invariant : blob historique `src/main.cpp` = `835059a0ecfe0f74708068b3259cad5db1cdb579`.

### Signature Android finale 1.4.0
- Les APK CI utilisent une signature debug non persistante.
- v1.3.1 publiée : certificat `8abfc11c8bc4f9ac065eb5c086ad4e457290bcbbc1105017865368de7e565868`.
- v1.4.0 publiée : certificat `22943f8846ebaf3191d011b1d947883d66ff6f25e566c3a966b98f879f070172`.
- `compatibleWithV131=false` dans le manifest public.
- Conséquence : ne jamais conseiller désinstallation/effacement des données pour installer l'APK de test.

## 2 — Coop / énergie / HUD

### Tir humain et IA
- Les tirs humains ordinaires restent rectilignes après le départ.
- L'IA peut anticiper la cible au départ ; seuls les projectiles explicitement guidés corrigent ensuite leur trajectoire.
- Le missile coop a retrouvé sa texture dédiée historique.

### Énergie
- Sémantique historique confirmée : `nrj=0` plein, `nrj=50` épuisé.
- Cadence et dispersion initiale dépendent de la réserve.
- Les protections ne doivent pas inverser cette convention.

### HUD
- Joueur haut testé téléphone : contrôle/tir RAS.
- HUD haut tourné à 180°.
- Vie boss dupliquée pour lecture des deux côtés ; gradient vert→rouge.

## 3 — Modèle cinétique : évolution et canon final

### Ancien problème
- Les impacts pouvaient être perçus comme trop faibles ou au contraire létaux selon les chemins.
- Un modèle ancien ajoutait un plancher et une dépendance quadratique de vitesse, créant des dégâts non nuls même dans des cas presque statiques.
- Certains tests historiques plaçaient un astéroïde immobile directement sur la coque et exigeaient pourtant des dégâts ; ce contrat était incohérent avec une vraie loi cinétique.

### Canon v3 validé
- Astéroïdes : dégâts bruts = `250 PV max × masseRelative × vitesseRelativeNormalisée × fermeture`.
- `masseRelative` bornée à 0..1.
- Pas de plancher de dégâts.
- Un contact strictement immobile n'invente plus de dégâts cinétiques.
- Les charges explicites du boss utilisent le même principe de vitesse relative/fermeture.
- Le multiplicateur de danger boss n'est jamais appliqué aux dégâts cinétiques.

### Régression `legacy_field_regressions`
- Le dernier échec avant GREEN venait du scénario `Legacy interactions`.
- Le test créait un astéroïde stationnaire sur la coque et attendait `pv < 1000`.
- Correction du test : créer un véritable impact entrant avec vitesse et `kineticStage` adaptés, et utiliser `sfCoopAsteroidHurt` pour la coop.
- Résultat : le test correspond à la physique approuvée au lieu de forcer un faux impact.

## 4 — Couches cinétiques et double application

- Une couche externe/interne qui dissipe réellement un impact marque l'interaction.
- L'astéroïde ne doit pas traverser dans la même étape jusqu'à une collision coque complète comme si aucune couche n'avait agi.
- La fragmentation/déviation conserve la matière tant que les limites de population le permettent.
- En fallback de population, la vitesse résiduelle est réduite et déviée au lieu de dupliquer la matière.

## 5 — Vagues, surcharge et poussières

### Vagues
- Ancien rendu : anneaux/pulses pouvant sembler permanents ou peu lisibles.
- Canon : chaque vague naît au centre du vaisseau et son front se déplace vers l'extérieur.
- Pendant la charge de surcharge, les fronts peuvent être irisés ; hors surcharge ils gardent la couleur d'équipe avec un front nettement visible.

### Surcharge deux doigts
- Charge : 2,00 s.
- Pendant charge : champ ×2 + arc-en-ciel + son de charge.
- Après 2 s et avant relâchement : état prêt mais bouclier cinétique OFF ; vulnérabilité volontaire.
- Relâchement prêt : purge des astéroïdes proches + décharge sonore.
- Relâchement précoce : pas de purge complète ; comportement tap/tir préservé.

### Poussière rouge
- Anciennes idées de chaleur/recharge supprimées du canon.
- État final : visuel uniquement ; aucune mutation de PV, réserve ou bouclier.

### Poussière blanche
- Priorité : recharge jusqu'à `nrj=0`.
- Une fois la réserve pleine, les collectes suivantes soignent fortement la coque.
- La poussière blanche n'est jamais physiquement déviée par le champ cinétique.

## 6 — Danger boss HOME

- Ancien multiplicateur général non cinétique coop : ×15.
- Nouveau défaut : ×10.
- Sélection cyclique HOME : ×1 / ×5 / ×10 / ×15 / ×20.
- Noms visibles : `MOU DU GENOU`, `CHILL`, `ROCK N ROLL`, `DUR A CUIRE`, `MACHINE DE GUERRE`.
- Les coefficients restent cachés dans l'interface.
- Le sélecteur et le bouton de lancement ont des zones tactiles disjointes.

## 7 — Validation v3 et publication

Séquence de preuve avant release :

1. Python assets : vert.
2. Python release : vert.
3. Python scenic : vert.
4. Python kinetic : vert.
5. Python kinetic surge : vert.
6. Python kinetic balance v3 : vert.
7. C++ kinetic surge : vert.
8. C++ kinetic balance v3 : vert.
9. C++ scenic mix : vert.
10. C++ kinetic shared model : vert.
11. C++ campaign format : vert.
12. C++ full SDL regressions : vert.
13. Android compatibility syntax : vert.
14. Legacy field : interaction historique rectifiée comme vrai impact entrant ; vert.
15. Gradle APK/AAB + packaging : vert.

Publication finale :
- HEAD construit : `98da8a01175d5091f487232f65872e92d213b321`.
- workflow : `37180765011`.
- release : `v1.4.0`, publique, non draft, non prerelease.
- APK : 87 675 758 octets, SHA `158bb86b07f353fcaeade3b65b522abf94f88e83feff2ab343f43d01d411d1d9`.
- AAB : 85 168 535 octets, SHA `4e2f8b2f6a0c671095598b03ea9a1825ce9555aa839cdf9a748fe311c7dbb4ae`.
- Le workflow one-shot de publication a été supprimé après succès.

## 8 — Intégration `main`

Découverte préalable :
- `main` et la branche campagne divergeaient depuis le merge-base `57b401a341dc07ed6ebe2c790a7ab96f389c1e27`.
- `main` possédait 5 commits uniques ; campagne 184+ commits d'avance.
- Un force-push aurait effacé cette histoire : interdit.

Fusion réalisée :
- merge commit : `c50088092744d18ce50f3ef484a2d706aa76a276`.
- parent 1 : ancien `main` `45e43f9146010dd945a492393f7e44be2026ad7a`.
- parent 2 : campagne nettoyée `098b88302ecf29654ae9a6c7d18a8352e7a1e5e8`.
- arbre : celui du code campagne/release vérifié, sans le workflow temporaire de publication.
- aucune réécriture forcée de `main`.

## 9 — Règle de clôture

Avant d'annoncer la clôture complète :
- vérifier Android sur le `main` fusionné et documenté ;
- inscrire SHA + workflow final verts dans les 4 mémoires ;
- garder la validation téléphone comme seule étape fonctionnelle humaine restante.
