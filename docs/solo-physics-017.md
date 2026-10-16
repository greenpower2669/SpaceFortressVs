# SOLO PHYSICS-017 — diagnostic

Branche `feature/solo-coop-core-v1`, prolongement de PARALLAX-016.
État : implémentation et vérification technique en cours ; Samsung non validé.
Aucun merge main, aucune Release, aucune nouvelle ressource graphique ou sonore.

## Fonctionnement

- Inertie intégrée dans les coordonnées et la vitesse du véritable pilote SOLO, dans le labyrinthe et l'arène finale. Les constantes interpolent continûment Danger 0..8 ; la vitesse instantanée augmente le délai de réaction. Intégration à sous-pas de 1/240 s au maximum.
- Lâcher le doigt conserve l'élan. Le balayage de la coque contre les tuiles bloque la composante normale et permet le glissement. La caméra utilise les zones circulaires puis une marge de visibilité, sans attacher le vaisseau au doigt.
- Appui bref sur la coque : repère de sélection. Deux doigts suivants : pincement ×1..×2. Un appui seul après sélection la retire. Les doigts d'un pincement sont consommés jusqu'à leur relâchement. Un appui sur la coque qui devient un glissement rend la main au pilotage.
- Zoom : projection uniquement. Dimensions physiques, armes, collision, carte et références de vitesse inchangées. Parallaxe 0,08/0,34/1,00 conservée ; passe de masquage limitée aux roches opaques, après tous les effets du monde et avant le HUD.
- Gisements déterministes répartis sur les cellules accessibles, dégagés des parois et des autres gisements. Les paires au repos ne reçoivent aucune correction physique. Impulsions dissipatives pondérées par l'aire, balayages des objets mobiles, amortissement puis vitesse exactement nulle. Fragments acceptés uniquement dans les espaces libres.
- Exploration : pas de générateur mobile. Arène finale : météores historiques avec placement contrôlé, balayage des collisions et champ cinétique réel. L'ancien générateur expérimental METEOR-009 reste OFF.
- 10 à 12 mini-boss selon le parcours, neuf zones irrégulières plus une meute de deux à quatre. Gardes, chasseurs, patrouilles/interceptions et appuis latéraux ; navigation sur les passages franchissables par leur coque, perception locale et mémoire limitée. Rôles et destinations distincts, espacement physique. 180 PV ; aucun bonus ni seuil de récompense du boss final.
- Sons environnementaux : décision au point d'émission, dans le viewport et avec ligne de vue libre depuis la coque. Les événements silencieux sont abandonnés immédiatement. Les commandes et impacts du pilote restent audibles ; la simulation distante continue.
- Transition : commandes, charge et vitesse annulées ; PV, énergie, bonus et progression conservés. Schéma de sauvegarde inchangé.

## Vérification automatisée

Les tests sont raccordés à `scripts/test-regressions.sh` et à la CI Android existante.

| Domaine | Couverture |
| --- | --- |
| Inertie | Neuf dangers, accélération, dépassement réel de cible, inversion, dérive, 30/60/120 FPS, pilotage réel dans le monde et l'arène |
| Caméra/collision | Relâchement sans doigt, marge de visibilité, très long balayage contre mur, glissement, round-trip écran/monde |
| Zoom | Sélection, plage ×1..×2, doigts réservés, interleaving A coque/B pilotage/A relâché, vitesse de croisière indépendante du zoom |
| Roches | 10 000 contacts statiques, réveil, dissipation, repos stable, minage/poussières, mur et traversée complète d'un gisement en un pas |
| Rencontres | 10..12 ancres accessibles sur 18 parcours, contournement de paroi, chasse réelle dans carte générée aux neuf dangers, alerte de meute, oubli, zéro récompense |
| Audio | Visible, derrière paroi, hors écran, collision pilote ; aucune modification d'asset |
| Rendu | SDL2 réel, transparence/parallaxe réversible, murs à différentes échelles, transparences non compositées deux fois, smoke du vrai sélecteur SOLO |
| Héritage | Modes historiques, armes, bonus, minage/énergie, 200 boss, sauvegardes/Hall, pause/reprise et transition ; contrôles C++/UBSan et compilation Android |

Revue indépendante : corrigés l'oubli d'inertie dans l'arène, la perte d'un UP tactile appartenant à la sélection, l'ordre des effets/masque opaque et le couplage zoom/vitesse. Le test de croisière 10 s à largeur 780 donne respectivement 13 027,85 / 18 066,49 / 22 815,14 unités monde aux dangers 0/4/8, identiques à ×1 et ×2.

## Essai Samsung indispensable

Entrer par **JOUER : SOLO / COOP / DUEL**, puis le **SOLO** du véritable sélecteur.

1. En MOU DU GENOU, puis APOCALYPSE : petits gestes, accélération longue, retour brutal du doigt, relâchement, virage et mur à grande vitesse. Vérifier le dépassement réel et le suivi de caméra.
2. Miner deux gisements voisins ; les laisser au repos. Tester impact, poussière, recharge, fragments et retour au calme.
3. Parcourir les rencontres et une meute ; vérifier la chasse entre couloirs, la perte de trace, la lisibilité, le compteur réel et l'absence de bonus.
4. Comparer collisions visibles, hors écran et derrière un mur ; garder tirs/charges/impacts du pilote audibles.
5. Tap coque, pincement, relâchement dans les deux ordres ; répéter pendant une dérive rapide. Vérifier ×1 minimum, ×2 maximum, absence de tir/charge involontaire et HUD fixe.
6. Entrer dans l'antre en dérive ; vérifier neutralisation, transition, boss, météores et récompenses finales. Collecter, terminer et relancer pour la sauvegarde.
7. Pause/reprise et rotation, lisibilité des décors aux deux zooms, fluidité et température sur téléphone. Aucun résultat FPS/thermique Samsung n'est présumé.

Les nouveaux bruitages restent entièrement reportés. L'intégration définitive reste soumise à l'accord de Fab après cet essai.
