# SpaceFortress — SOLO PARALLAX-016 (DIAGNOSTIC)

Branche : `feature/solo-coop-core-v1`. Aucun merge main ni Release publique.

Source `2a933a60639d890fd6168befb8ce1784ba9a18c8`; CI Android/C++/SDL2 `38837760766` SUCCESS; publish-release SKIPPED.
- Atlas animé: `assets/pict/solo/SoloBonusAnimations.png`, 1536x768, 4x8 frames de 192 px, SHA256 `1a337adb15a6a3901b76da566dcd26a1d2d76c26893864a37a85e97b63950001`.
- Fond ajouré : `assets/pict/solo/FondsParallaxeModeSolo.png`, RGBA1254x1254, SHA256 `4becaec70bf7f23d823c3eccbd902b86aa4dbb6942115a1389409bc6a7f7df44`.
- `SpaceFortressVs-1.4.2-SOLO-PARALLAX-016-2a933a6.apk`: 98400063 octets, SHA256 `62a029ca828a85783f324301bcb5d1411cfa8f343875f7e5ebf4108b5ce8515f`.
- `SpaceFortressVs-1.4.2-SOLO-PARALLAX-016-2a933a6-unsigned.aab`: 95691410 octets, SHA256 `1c18b7d99a45816a2332cd77415927cbf26cbb93cc1346897160c18eafc76703`.

## Lancer le vrai SOLO

Dans l'accueil diagnostic, choisir **JOUER : SOLO / COOP / DUEL**.
Dans le vrai sélecteur historique, toucher le choix de mode jusqu'à **SOLO**, puis **JOUER**.
ANCIEN PROTOTYPE SOLO et SOLO BONUS 012 restent des entrées de diagnostic distinctes.

## Contenu

- NOUVEAU : ciel historique lointain, fond minéral avec neuf ouvertures alpha réelles.
- Parallaxe indépendante : ciel 0,08, minéral 0,34, parois/acteurs 1,0 ; aucun changement de pilotage.
- ParoisModeSolo et EnvironnementsModeSolo raccordés au vrai SOLO ; murs pleins, géométrie inchangée.
- Raccords testés à plusieurs échelles, dont la capture réduite de l'antre ; rechargement textures après reset.
- Labyrinthe existant avec quatre miniboss espacés sur le chemin accessible vers l'antre.
- Miniboss : 180 PV, poursuite courte, avertissement puis salve x3 ; aucun bonus, même à la mort.
- Pilotage tactile, caméra circulaire, énergie, minage et cinétique historiques conservés.
- Antre identifié : transition de 1 s de temps actif, zoom/ondulation, puis arène finale.
- Même moteur et initialiseur de boss que BOSS TEST ; aucun bouton diagnostic en vrai SOLO.
- Boss final seul : bonus à 75/50/25 % et à la mort, éjections et animations BONUS-014 conservées.
- PV, énergie et pouvoirs conservés lors du passage ; transition sans consommation de leur durée.
- Après victoire, laisser les récompenses arriver, les ramasser puis toucher TERMINER.
- Sauvegarde SOLO compatible : le prochain lancement ouvre le premier niveau non terminé.
La position au milieu d'un niveau n'est pas sauvegardée.
- Aucune tourelle SOLO ; METEOR-009 reste désactivé. Pas de correction annoncée des anciennes
collisions d'astéroïdes avec les parois.
- `src/main.cpp` inchangé ; mécanismes CLASSIQUE/DUEL/COOP protégés.

## À vérifier physiquement sur Samsung

1. Vrai sélecteur SOLO : vérifier textures des murs, trous vers le ciel et retour exact du décor en revenant sur ses pas.
2. Vaisseau sous le doigt, caméra continue, minage et commandes de tir humain.
3. Quatre rencontres : placement, lisibilité, dégâts et zéro bonus après les minibosses.
4. Antre : zoom/ondulation lisible, sans tir involontaire ni bouton de test dans l'arène.
5. Mise en arrière-plan/reprise pendant la transition, sans perte de PV/énergie/pouvoirs.
6. Boss final, quatre seuils de récompenses, collecte puis TERMINER et niveau suivant.
7. Aspect des répétitions de parois, fluidité réelle et jauges au-dessus de la barre Android ; aucun FPS téléphone n'est encore certifié.
