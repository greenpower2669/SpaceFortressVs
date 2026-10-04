# Énergie et tirs — correction 1.2.1

Signalement de Fab : cristal bleu anormalement bas, alors que son repère
devrait rester près des icônes centrales ; tirs étranges dans l'autre camp.
Base : version 1.2.0, commit `9a16264`. Version corrective : 1.2.1, code 7.

## Causes vérifiées

Le code historique utilise `nrj` comme chaleur/dépense : zéro correspond à
100 % de réserve, 50 à son épuisement. Les dégâts pouvaient dépasser 50,
alors que seul le tir principal plafonnait cette valeur. Le HUD utilisait
directement `HEIGHT * (1/2.1 + nrj * .0009)` côté bleu et une formule miroir
côté orange. À 200, le cristal sortait largement de la zone des icônes.
La capture seule ne permet pas de connaître la réserve exacte à cet instant.

Les missiles multipliaient leur vitesse par 1,15 à chaque dessin et déplaçaient
leur position une deuxième fois. Leur effet sur l'énergie dépendait du carré
de cette vitesse sans limite. Les bounds de dessin étaient calculés avant le
déplacement des projectiles : un impact pouvait donc précéder leur position
visible. Ces chemins historiques subsistaient dans les dernières versions.

## Corrections

- `ship_energy.hpp` centralise la plage 0–50 et l'ajout de chaleur. Les impacts
  et la collecte la respectent ; une normalisation au début de frame protège
  aussi l'entrée du pilote et les calculs de dégâts.
- Le marqueur interpole entre les centres des repères du bouclier et du canon,
  avec un facteur borné. Il respecte ces limites même si la valeur reçue est
  ancienne ou invalide et si la taille de l'icône est animée.
- Les missiles passent au déplacement en secondes, de 0,65 à 1,5 largeur
  d'écran/s, avec accélération limitée et guidage transversal plafonné. Ils
  gardent leur direction avant après dépassement de la cible et expirent après
  quatre secondes au plus. Ils ne changent plus d'état pendant le dessin.
- Leur coût sur la cible vaut 8 unités de chaleur. Un projectile de tourelle
  vaut 2 unités, indépendamment de son angle, de la résolution et de la frame.
  Le coût des tirs normaux reste plafonné à 3. Le tireur défensif ne touche pas
  à la réserve de son vaisseau.
- Le rectangle et l'angle du projectile utilisent sa position/vitesse actuelles.
  La traînée du missile reste un effet visuel, sans créer de minerais.
- Le balayage de collision déjà utilisé pour la défense couvre aussi les
  missiles et les tirs principaux. Les projectiles consommés ne frappent plus.

`src/main.cpp` reste intact. Les substitutions Android vérifient les blocs
historiques attendus avant de produire la source compilée.

## Validation

- Tests des marqueurs des deux camps à 0, 25, 50 et 200 unités, valeurs hors
  plage, trois tailles d'écran, dimensions d'icônes différentes et redémarrage.
- Impacts répétés et recharge : réserve bornée, vaisseau allié inchangé.
- Missiles des deux camps à 30/60/120 images/s et trois facteurs historiques
  `k0` : vitesse bornée, distances comparables, coordonnées visibles à jour.
- Collision entre deux frames, impact défensif horizontal/vertical identique,
  huit dessins successifs sans modification de la simulation ni de la réserve.
- Prévisualisations SDL avec les vrais assets à pleine énergie et après une
  surcharge de 200. L'ensemble des tests antérieurs reste exécuté.

Les tests hôtes n'équivalent pas à une partie sur téléphone. Ils ne certifient
pas tout l'équilibrage des dégâts ni tous les ordres de collision possibles
du moteur historique dans un champ d'astéroïdes dense.
