# Mission reçue de Fab — ASTRA-COOP-SOLO-ARCH-001

Reçue le 11 octobre 2026. Le texte ci-dessous est conservé séparément de la proposition technique. Il autorise un audit ; l'implémentation attend la validation de Fab.

---

MISSION ASTRA — SPACEFORTRESS

RENVERSEMENT D'ARCHITECTURE : CONSTRUIRE LE SOLO À PARTIR DU COOP

Dépôt : https://github.com/greenpower2669/SpaceFortressVs

Branche de référence : "feature/solo-worlds-tutorials-v1"

Demandeur : Fab

Méthode : FabCopilot — audit d'abord, puis implémentation après validation.

---

1. POURQUOI NOUS SOLLICITONS TON AIDE

Astra, nous avons besoin de ton expertise architecturale en C++.

Nous développons depuis plusieurs itérations un mode SOLO exploratoire dans SpaceFortressVs.

Nous avons réussi à construire :

- Un labyrinthe explorable avec plusieurs chemins.
- Des parois rocheuses et des environnements.
- Une caméra à zones circulaires concentriques.
- Un déplacement tactile avec vaisseau sous le doigt.
- Une progression SOLO avec relais, objectifs et sauvegardes.
- Une première adaptation des armes et du bouclier historiques.

Mais nous avons atteint les limites de cette approche.

Chaque fois que nous essayons de reproduire une mécanique du COOP dans notre moteur SOLO, nous devons réadapter le code, les animations, les dimensions, les collisions et les effets.

Le résultat est techniquement prometteur mais visuellement encore insuffisant.

Notre erreur est probablement d'avoir choisi le SOLO comme fondation.

Nous souhaitons maintenant faire exactement l'inverse.

---

2. NOUVELLE DÉCISION ARCHITECTURALE

PARTIR DU VÉRITABLE MODE COOP HISTORIQUE ET CONSTRUIRE LE NOUVEAU MODE SOLO AUTOUR DE LUI.

Nous voulons conserver le moteur de jeu COOP comme fondation fonctionnelle et graphique.

Il ne s'agit pas de reproduire son apparence, mais de réutiliser réellement son fonctionnement.

Évaluer la meilleure solution technique :

1. Une copie isolée du moteur COOP, adaptée au SOLO.
2. Une extraction des composants du COOP vers un moteur commun avec deux modes.
3. Une autre architecture plus robuste, à condition qu'elle évite de réécrire toutes les fonctionnalités historiques.

Choisir la solution qui limite le risque de régression et conserve au maximum le code déjà testé.

Nous ne voulons pas deux moteurs indépendants qu'il faudra synchroniser éternellement.

---

3. LE COOP DEVIENT NOTRE SOCLE

Le nouveau SOLO doit retrouver les mêmes fonctionnalités que le COOP existant :

Vaisseau

- Les véritables sprites animés du vaisseau.
- Les mêmes dimensions et proportions graphiques.
- Les mêmes animations et effets.
- Les mêmes comportements physiques, hors adaptations de navigation voulues pour le SOLO.

Combat

- Les véritables orbes d'énergie.
- Les véritables missiles, avec guidage et effets.
- La vraie mécanique de tir.
- La véritable réserve d'énergie "nrj".
- Les mêmes dépenses et recharges d'énergie.
- Les mêmes dégâts.
- Le véritable bouclier cinétique.
- Les mêmes réactions aux collisions et aux impacts.

Cône et supercharge

- Le véritable cône énergétique du COOP.
- La charge cinétique historique.
- L'aspiration des poussières.
- Le minage des astéroïdes.
- La vulnérabilité pendant la charge.
- La méga-décharge au relâchement après charge complète.
- Les vrais effets sonores et visuels.

Différence SOLO autorisée : le cône est orienté à 360° par le vecteur entre le vaisseau et la position de visée du second doigt.

Vie de l'univers

- Astéroïdes et débris.
- Poussières blanches et rouges.
- Explosions.
- Particules.
- Ondes cinétiques.
- Animations.
- Sons.
- Arrière-plans et effets spatiaux.
- Effets de dangers environnementaux.

Tout cela existe déjà partiellement ou totalement dans le moteur historique. Ne pas le réinventer avant d'avoir étudié sa réutilisation.

---

4. BOSS, MINIBOSS ET DIFFICULTÉS

Nous voulons exploiter le catalogue et les comportements historiques des boss COOP.

Cependant, le boss final n'apparaît plus immédiatement au début de l'arène.

Dans le SOLO exploratoire :

- Le joueur traverse les couloirs du labyrinthe.
- Il rencontre progressivement des dangers.
- Il peut affronter des miniboss ou ennemis intermédiaires.
- Il doit explorer, récolter des ressources et parfois revenir en arrière.
- Le boss principal attend plus loin, dans une région ou une salle dédiée.
- Son affrontement est déclenché par la progression dans le monde.

Réutiliser autant que possible :

- Le catalogue existant.
- Les sprites et animations des boss.
- Les comportements offensifs.
- Les phases de combat.
- Les dégâts et résistances.
- Les difficultés historiques de 1 à 9.
- Les sons et effets spécifiques.

Auditer quelles mécaniques sont directement réutilisables et lesquelles nécessitent une adaptation spatiale.

Ne pas inventer une nouvelle IA simpliste si l'IA historique peut être adaptée.

---

5. CE QUI CHANGE VRAIMENT : LE MONDE ET LA CAMÉRA

Le COOP fonctionne essentiellement dans une arène.

Nous voulons transformer cette arène en vaste monde explorable, sans perdre son moteur.

Le monde SOLO apporte :

- Une carte beaucoup plus grande que l'écran.
- De véritables parois et obstacles.
- Des couloirs larges.
- Des intersections et itinéraires multiples.
- Des zones rocheuses, glacées, énergétiques et volcaniques.
- La possibilité de revenir sur ses pas.
- Des salles et régions de combats.
- Des zones d'exploration avec ressources.

Caméra circulaire — RÈGLE VALIDÉE PAR FAB

Trois cercles concentriques centrés sur l'écran :

- Cercle intérieur : vitesse caméra 0.
- Première couronne : progression 0,1 à 2.
- Deuxième couronne : progression 2,1 à 6.
- Au-delà du grand cercle : vitesse 6 constante.

Le diamètre du grand cercle est égal au plus petit côté de l'écran moins une marge.

Le diamètre du petit cercle est égal à la moitié du grand.

Le troisième cercle est placé entre les deux.

Le vaisseau reste sous le doigt.

Le décor défile tant que le vaisseau reste dans une zone active, même avec un doigt immobile.

La caméra ne peut jamais contourner artificiellement les collisions.

Défi architectural central

Créer une séparation correcte entre :

- Coordonnées MONDE.
- Coordonnées ÉCRAN.
- Coordonnées de la CAMÉRA.
- Physique et collisions.
- Rendu graphique.
- Entrées tactiles.

Les projectiles, boss, particules, explosions, astéroïdes, armes et boucliers doivent fonctionner en coordonnées monde et être affichés correctement par la caméra.

Ne pas déplacer artificiellement chaque élément du COOP pour simuler un scrolling.

La caméra doit être une transformation de rendu, tandis que la simulation conserve ses coordonnées physiques cohérentes.

---

6. LES PAROIS DOIVENT ÊTRE INTÉGRÉES À CE SOCLE

Deux ressources originales ont été déposées dans le projet :

- "ParoisModeSolo.png"
- "Planchende sprites energie lave rocher glace fumée.png"

Elles ont déjà été copiées dans les ressources de la branche SOLO.

Les dernières captures Samsung montrent encore des défauts importants :

- Cases rocheuses visuellement séparées.
- Répétitions trop visibles.
- Coins et orientations artificiels.
- Jonctions discontinues.
- Décors ressemblant à des morceaux de planche collés.

Nous voulons des parois visuellement continues, intégrées à l'univers du COOP.

Réutiliser le rendu historique quand il est de meilleure qualité.

Ne pas imposer les planches existantes si leur structure ne permet pas un bon résultat ; dans ce cas, expliquer quelle préparation graphique serait nécessaire.

Conserver des collisions correctes, indépendantes du rendu.

---

7. IMPORTANT : UTILISER LA FABLIB SI POSSIBLE

Fab a développé sa propre bibliothèque C++ :

"src/fablib.hpp"

Elle comprend notamment :

- "tupl" pour les positions.
- "tuplv" pour les vecteurs de vitesse.
- "enti" pour certaines opérations spatiales.
- "vlong" et d'autres opérations géométriques.

Cette bibliothèque est utilisée dans l'architecture historique, notamment par l'intermédiaire de "t.hpp".

Astra, examine comment exploiter cette FabLib pour préserver les conventions originales de Fab.

Ne remplace pas inutilement toutes les structures historiques par de nouvelles classes mathématiques.

Attention toutefois : la bibliothèque contient également des variables globales. Il faut respecter les contraintes de compilation, d'initialisation et d'isolation du moteur.

Si la FabLib pose des limites, les identifier précisément et proposer des adaptations minimales.

---

8. POINTS D'ENTRÉE À AUDITER

Étudier en priorité :

- "src/main.cpp" — intégration historique du jeu.
- "src/campaign_runtime.hpp" — moteur et gameplay COOP.
- "src/tactical_runtime.hpp" — combat, projectiles et comportements tactiques.
- "src/kinetic_shield.hpp" — système cinétique partagé.
- "src/ship_energy.hpp" et "src/ship_energy_rules.hpp" — énergie et protection.
- "src/fablib.hpp" et "src/t.hpp" — fondations historiques.
- "src/boss_catalog.hpp" — catalogue des boss.
- "src/solo_prototype.cpp" — prototype actuel.
- "src/solo_campaign_model.hpp" — carte et représentation du monde.
- "src/solo_navigation.hpp" et "src/solo_viewport.hpp" — caméra et déplacement.
- "src/solo_scenery.hpp" — sélection des parois.
- "src/solo_campaign_controller.hpp" — progression SOLO.

Les noms sont fournis comme points de départ : inspecter le graphe réel des dépendances et la chaîne de compilation Android.

---

9. CONTRAINTES ABSOLUES

1. Ne pas modifier directement le comportement public des modes CLASSIQUE et COOP.
2. Ne pas modifier "main" sans autorisation.
3. Ne pas créer de Release.
4. Ne pas remplacer le fonctionnement historique par des approximations graphiques.
5. Conserver la compilation C++ et Android.
6. Éviter les duplications de variables globales, symboles et états partagés.
7. Ne pas forcer le COOP existant à fonctionner dans le SOLO avec des coefficients arbitraires.
8. Maintenir les cinq mémoires vivantes FabCopilot :
   "brain.md", "brainmap.md", "debughistorical.md", "todo.md", "ordres-de-mission.md".
9. Prévoir tests automatisés, CI et APK diagnostic sur branche de travail après autorisation.
10. Ne pas perdre le travail déjà réalisé sur la carte, la progression, la navigation circulaire et les sauvegardes SOLO.

---

10. CE QUE NOUS ATTENDONS DE TOI MAINTENANT

Avant de coder, Astra, réalise un véritable audit d'architecture.

Nous souhaitons recevoir :

A. Diagnostic

Pourquoi le moteur SOLO actuel nous oblige-t-il à recréer autant de comportements existants ?

B. Nouvelle architecture

Comment faire du COOP historique le moteur de référence du nouveau SOLO exploratoire ?

C. Plan de migration

Quelles parties du SOLO actuel conserver, remplacer ou abandonner ?

D. Gestion des états

Comment isoler les états, joueurs, boss, particules, textures et contrôles pour que les autres modes ne soient pas affectés ?

E. Caméra et coordonnées

Comment adapter proprement un moteur d'arène à un grand labyrinthe avec caméra circulaire et collisions ?

F. FabLib

Quels composants de la bibliothèque personnelle de Fab seraient réutilisables ?

G. Plan d'implémentation

Proposer des étapes courtes, vérifiables, testables sur Android et permettant de comparer visuellement le nouveau SOLO avec le vrai COOP.

Ne lance pas encore de grande réécriture. Nous voulons d'abord valider avec Fab la nouvelle fondation.

---

11. NOTRE PHILOSOPHIE

Nous avons déjà accompli beaucoup de travail.

Ce travail n'est pas perdu : il nous a permis de comprendre les contraintes des labyrinthes, de la caméra et de l'exploration.

Mais il ne faut plus reconstruire péniblement ce qui existe déjà.

Nous voulons prendre SpaceFortress COOP, lui donner un monde plus grand, des parois, une caméra exploratoire, des rencontres progressives et un boss final éloigné.

Le COOP doit rester le cœur vivant du jeu.

Le SOLO doit devenir une nouvelle façon de parcourir cet univers, et non une imitation de celui-ci.

Astra, aide-nous à choisir et mettre en place la bonne architecture C++ pour y parvenir.

— Fab & Djipie ❤️🚀
