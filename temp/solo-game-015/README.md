# SpaceFortress — SOLO GAME-015 (DIAGNOSTIC)

Branche : `feature/solo-coop-core-v1`. Aucun merge main ni Release publique.

Source `f5b4ce4f82e99057d041767d982914e903afbf7f`; CI Android/C++/SDL2 `38724287864` SUCCESS; publish-release SKIPPED.
- Atlas animé: `assets/pict/solo/SoloBonusAnimations.png`, 1536x768, 4x8 frames de 192 px, SHA256 `1a337adb15a6a3901b76da566dcd26a1d2d76c26893864a37a85e97b63950001`.
- `SpaceFortressVs-1.4.2-SOLO-GAME-015-f5b4ce4.apk`: 95023204 octets, SHA256 `f5138def32cdb8553e86006f6e079ce6a61a979b764c6c62f1757aca61ca8ec7`.
- `SpaceFortressVs-1.4.2-SOLO-GAME-015-f5b4ce4-unsigned.aab`: 92378949 octets, SHA256 `1b4d9a9bffb7ade770ec033bd25162c20c440c9a01db71ad8613c2156aee0e7c`.

## Lancer le vrai SOLO

Dans l'accueil diagnostic, choisir **JOUER : SOLO / COOP / DUEL**.
Dans le vrai sélecteur historique, toucher le choix de mode jusqu'à **SOLO**, puis **JOUER**.
ANCIEN PROTOTYPE SOLO et SOLO BONUS 012 restent des entrées de diagnostic distinctes.

## Contenu

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

1. Lancement depuis le vrai sélecteur, retour Android et relancement.
2. Vaisseau sous le doigt, caméra continue, minage et commandes de tir humain.
3. Quatre rencontres : placement, lisibilité, dégâts et zéro bonus après les minibosses.
4. Antre : zoom/ondulation lisible, sans tir involontaire ni bouton de test dans l'arène.
5. Mise en arrière-plan/reprise pendant la transition, sans perte de PV/énergie/pouvoirs.
6. Boss final, quatre seuils de récompenses, collecte puis TERMINER et niveau suivant.
7. Fluidité réelle et jauges au-dessus de la barre Android ; aucun FPS téléphone n'est encore certifié.
