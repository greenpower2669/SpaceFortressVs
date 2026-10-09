# SpaceFortressVs — proposition de mini-jeux du tutoriel (NON CODÉE)
Date : 9 octobre 2026

## Objet et statut
**Proposition à soumettre à Fab avant toute implémentation.** Le tutoriel actuel `src/tutorial_runtime.hpp` reste inchangé. Les futurs mini-jeux s'appuieraient sur le bac à sable existant, sans toucher aux vrais PV, progression 200, statistiques, sauvegardes, Hall of Fame, réseau ni moteurs de duel/coop.

## Parcours conseillé
1. **Académie des pilotes — Slalom orbital (30–60 s libre)** : guider un vaisseau dans 3 portes lumineuses avec le premier doigt. Réussite : franchir les portes sans se téléporter ; feedback visuel par porte. Aucune pression chronométrique obligatoire.
2. **École des tireurs — Trois cibles mobiles** : maintenir le premier doigt pour piloter, toucher brièvement avec le second pour tirer sur 3 cibles simples. Réussite : 3 impacts ; rappel qu'un appui long ne produit pas une rafale. En mode joueur + IA, montrer séparément ce que fait l'IA.
3. **Tableau de bord — Sauver la coque** : partir d'une jauge simulée qui baisse ; repérer coque, réserve d'énergie et bouclier. Réussite : sélectionner les bons indicateurs après 2 scénarios « impact »/« tir ». Aucun vrai dégât infligé.
4. **Chasse aux poussières — Mineur ou fossoyeur ?** : collecter 5 particules blanches en évitant les rouges. Une deuxième manche fait apparaître un boss blessé qui peut voler le blanc. Réussite : recharger sa réserve sans nourrir le boss ; visualiser les effets réels (blanc ressource, rouge réagit au cinétique mais ne soigne pas).
5. **Bouclier cinétique — Météorites en approche** : montrer une petite puis une grosse roche à vitesses relatives différentes, et les vagues centre→extérieur. Réussite : survivre à 3 vagues en gérant une réserve simulée pleine puis basse ; voir la vague virer à l'orange/rouge. Ne pas exposer une fausse invulnérabilité.
6. **Deux secondes de courage — Charge, alerte, purge** : second doigt maintenu ; avant 0,30 s tir bref, de 0,30 à 2 s champ doublé, à 2 s champ OFF et vulnérabilité, relâchement pour purge. Réussite : déclencher une purge dans une zone de roches. Indicateur de charge et vibration/son facultatifs ; répétition illimitée.
7. **Sauvetage coop — Ne laisse pas ton allié !** : rejoindre un équipier factice KO et rester à proximité 2 secondes, avec guide de distance. Réussite : réveiller le partenaire ; montrer « au lieu d'attaquer, protéger » pour le coéquipier IA sans prétendre qu'il est humain.
8. **Examen de la flotte — Mini-boss d'entraînement** : micro-arène combinant mouvement, tir, poussières, cinétique et secours. Trois objectifs plutôt qu'une élimination forcée : survivre, ramasser du blanc, placer une surcharge. Réussite : 3 objectifs terminés ; aucune écriture dans la campagne ou le Hall.

## Interaction, rythme et accessibilité
- Entrées : **TOUT FAIRE**, **MISSIONS COURTES**, **MODULE LIBRE** ; pas de progression forcée.
- Chaque séquence : écran « objectif » court, démonstration animée, essai guidé, confirmation de réussite, **REJOUER / SUIVANT / QUITTER**.
- Grandes zones tactiles, police ajustée à l'écran, contraste renforcé ; option « explication détaillée » persistante dans la session. Pas de limite de temps hors véritable maintien des deux doigts 2 secondes.
- Panneau d'aide (?) accessible pendant chaque mission ; reprise de la séquence factice sans relancer la vraie partie.
- Une erreur doit expliquer le geste attendu et autoriser la répétition sans malus ni score public.
- Priorité : garder la main joueur/IA claire ; les modes duel local/IA et coop local/IA doivent être distingués dans les textes.
- Toute animation/physique tutoriel reste isolée. Réutiliser PNG/SFX existants et prévoir dessin SDL de repli si asset absent.

## Contrôles à écrire si Fab valide l'idée
- Tests de frontières tactiles ; deux doigts indépendants ; passage 0,30 s et 2,00 s ; sortie avec doigt tenu ; BACK ; réouverture d'une vraie partie sans état modifié.
- Tests de réussite/rejouabilité par module, mode portrait/paysage et fallback textures ; aucun effet sur PV réels, campagnes, Hall ou persistance.
- Validation visuelle et fonctionnelle sur Android par Fab, puis seulement décision de merge / release.

**Hors périmètre de cette proposition :** implémentation des mini-jeux, correction vélocité, duel IA, mise en place de la signature Play, modification du gameplay.
