# SpaceFortressVs 1.3.1 — plan et suivi

Base : release 7ce2c949249cd087c047744ce12ed44f892f1319 ; reprise depuis
82e6798847b1101f3441fe437ad6816a05e6f6e6. Audit accepté par Fab le 20 septembre 2026.

## Périmètre autorisé

Corriger les régressions démontrées, sans remplacer les vecteurs de Fab, le
champ historique, les tourelles de combat ni les cinquante identités des boss.
Conserver `src/main.cpp` comme référence : ses petits raccords Android restent
dans `scripts/prepare-legacy-source.cmake` et sont testés sur le source généré.
Ne pas modifier les PNG ni le traitement Base64 existant. Le fichier local
`assets/pict/fond4hlz.png` est déjà modifié à la reprise ; `fond4hlow.png`
apparaît aussi modifié localement. Les deux sont exclus de ce lot et conservés.

## Étapes et critères

1. Vaisseaux : reproduire la transition coop → duel avec le vrai état initial
   (`frames=10`), puis restaurer l'état complet. Adapter le calcul ancré sur
   `sw/sh` de `vib()` pour une respiration visuelle de ±1 % / 3 secondes, sans
   déplacer le centre ni les collisions. Garder le refroidissement historique.
   Vérifier cycles, redémarrages, 30/60/120 Hz et les valeurs historiques de k0.
2. Tirs : reproduire le verrouillage humain en coop, conserver l'axe du camp
   pour les humains et l'interception d'Orion. Vérifier les vitesses après un
   changement de direction de la cible ; garder les missiles et tourelles.
3. Sauvegardes : reproduire « principal inconnu + secours valide », préserver
   les deux fichiers, vérifier aussi une modification du principal après le
   chargement. Pour un fichier corrompu récupérable, conserver ses octets dans
   une copie distincte avant la restauration. Écrire primaire et secours par
   temporaires synchronisés et renommages. Format v1, noms, Hall et victoire en
   attente inchangés. Refuser un candidat invalide avant toute écriture.
4. Astéroïdes : exécuter les procédures du source Android généré dans un test
   natif. Reproduire l'asymétrie de visibilité et le minage nul en paysage ;
   corriger seulement la largeur de référence et la division entière. Observer
   les trajectoires pendant plusieurs minutes sans attribuer à tort le ressenti
   « bord gauche » à une cause non reproduite. Fragmentation et population
   demeurent inchangées dans ce lot.
5. Vérifier tests natifs/UBSan, Python, source Android généré, puis versionner
   1.3.1 et documenter le résultat réel dans todo.md. Contrôler la continuité de
   signature avant de conseiller une mise à jour sur le téléphone existant.

## Revue ciblée

- Un second retour de coop ne doit pas réintroduire les compteurs de la coop.
- Les dimensions visuelles ne doivent modifier ni minage ni rayon d'évitement.
- Les deux humains restent distincts d'Orion, y compris après un changement de mode.
- Une sauvegarde inconnue, un échec de copie ou un chemin inaccessible conserve
  les données lisibles et les octets originaux ; aucune réinitialisation Android.
- Les tests des astéroïdes doivent exécuter le raccord généré, pas une copie
  indépendante de sa formule.

## Suites distinctes

Campagne 200, migration v2, mobilité enrichie des boss et bonus flottant restent
à réaliser dans les lots suivants. Ordre des difficultés et bonus à valider.
`Tourelle.png` est réservé à la décoration d'accueil ; `Bonus_de_tourelles.png`
au futur bonus. Ajoutés depuis l'audit sur `main` (74c94c1), ils sont repris
directement par Git, sans transformation. La tourelle RGBA anime maintenant
l'accueil avec un léger balancement, recul et de brefs éclats dessinés par SDL.
Le bonus RGB contient un damier dans ses pixels : sa transparence devra être
traitée avec Fab avant son activation future, sans modifier l'original ici.
Le packaging vérifie les deux fichiers octet pour octet dans APK et AAB.

Blobs Git originaux :
- Tourelle : `4ebe6d211448e81f3ec1ca90717f5a37b9dd1064`.
- Bonus : `b0faa026319a5ecddb88dfd35164b6b3c7ca0077`.

## Journal

- Préparation : branche locale `fix/1.3.1-runtime`, espace isolé existant.
- Les tests locaux utilisent SDL2 et SDL2_image déjà compilés ; les chemins
  audio sont compilés mais les essais sonores nécessitent le téléphone.
- Les tests ciblés ont échoué sur le code non corrigé pour la restauration
  des compteurs, les tirs humains coop, la sauvegarde inconnue avec secours,
  l'archivage avant réparation, la respiration, la visibilité et le minage.
- Après correction, les tests natifs/UBSan passent, y compris les 50 profils,
  victoires, noms et rechargements. Les simulations de combats donnent :
  boss 1 victoire en 12,3 s ; 10 en 23,7 s ; 25 en 54,9 s ; 40 en 103,2 s ;
  50 défaite en 30,4 s. Ce résultat ne constitue pas un équilibrage humain.
- Le champ historique seul, sans collisions/minage/fragmentation, est observé
  sur 20 graines × 5 minutes à 30/60/120 Hz dans les deux orientations.
  Les quatre quarts horizontaux reçoivent chacun 24,0 à 26,1 % des positions
  visibles. La cause complète du ressenti « bord gauche » reste ouverte.
- Le rendu d'accueil a révélé un chevauchement titre/sous-titre en paysage :
  taille du texte et motif orbital désormais bornés aussi par la hauteur.
- L'APK v1.3.0 a été téléchargé et son SHA-256 comparé à la release. Son
  certificat SHA-256 est
  `822915992d833e32f555fd4332ff56bba0c5319bc0a359e1925811fe12a86c2b`.
  Le workflow utilise actuellement une signature debug non persistée. Le
  certificat de 1.3.1 sera consigné dans son manifeste de livraison ; une
  mise à jour sur l'installation existante n'est pas encore garantie.
