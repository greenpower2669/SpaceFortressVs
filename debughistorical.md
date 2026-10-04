# debughistorical.md — SpaceFortressVs

## Historique technique condensé — état au 4 octobre 2026

Ce fichier conserve les décisions et causes prouvées. Les détails plus anciens restent dans l'historique Git ; ne pas réintroduire un comportement à partir d'un souvenir non vérifié.

## 1 — Base historique / Android

### Astéroïdes invisibles en classique
- Symptôme téléphone : poussières/effets présents mais astéroïdes invisibles.
- Cause prouvée : confusion entre `std::setw` et la fonction historique `fablib::setw(W)` dans la source Android générée ; `W` restait nul et `inxy()` rejetait le rendu.
- Correctif : qualifier uniquement la copie Android générée ; `src/main.cpp` reste inchangé.
- Invariant : blob historique `src/main.cpp` = `835059a0ecfe0f74708068b3259cad5db1cdb579`.

### Signature Android
- Les APK CI utilisent une signature debug non persistante.
- v1.3.1 publiée : certificat `8abfc11c8bc4f9ac065eb5c086ad4e457290bcbbc1105017865368de7e565868`.
- build 1.4.0 de référence du 4/10 : certificat `e7fe36619f76ea2c5e0cede5ebd6eafc734e31ae8e6bd21de1f86e2d6268700c`.
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

## 7 — Séquence de validation v3

Étapes ayant servi à isoler le dernier échec :

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
14. Legacy field : seul `interactions` était rouge ; test rectifié comme impact entrant réel.
15. Suite complète ensuite verte.

Validation finale de référence avant réorganisation mémoire :
- HEAD `b56b883d2499fb41c4bd26cb39e067c527201f28`
- workflow Android `37166112893`
- build, packaging et artefacts : succès.

## 8 — Intégration `main`

Découverte le 4 octobre 2026 :
- `main` et la branche campagne ne partagent plus une histoire linéaire depuis le merge-base `57b401a341dc07ed6ebe2c790a7ab96f389c1e27`.
- `main` possède 5 commits uniques ; campagne 184 commits d'avance.
- Comparaison nette `main → campagne` : aucun fichier de `main` n'est supprimé dans la campagne ; la campagne contient les éléments Android/assets plus les ajouts actuels.
- Règle : fusion finale avec deux parents pour conserver l'histoire de `main`, pas de force-push remplaçant son histoire.

## 9 — Règle de clôture

Avant d'annoncer une release ou un merge comme terminé :
- exécuter une vérification fraîche du SHA concerné ;
- vérifier le SHA du build manifest ;
- vérifier la release et ses assets ;
- seulement ensuite fusionner `main` ;
- vérifier `main` après fusion.
