# SpaceFortress — SOLO BONUS-014 (DIAGNOSTIC)

Branche : `feature/solo-coop-core-v1`. Aucun merge main ni Release publique.

Source `f9323f3cd239e9027d964e38db369a9217ef02cb`; CI Android/C++/SDL2 `38420099395` SUCCESS; publish-release SKIPPED.
- Atlas animé: `assets/pict/solo/SoloBonusAnimations.png`, 1536x768, 4x8 frames de 192 px, SHA256 `1a337adb15a6a3901b76da566dcd26a1d2d76c26893864a37a85e97b63950001`.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-014-f9323f3.apk`: 94989574 octets, SHA256 `0dfe83dbb1f2716e33442a2d2933356b1a6582180a85a9c2fed5a77b1ebe59ff`.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-014-f9323f3-unsigned.aab`: 92342299 octets, SHA256 `564b0c9427bd776ed93bcb32d3012d86d232f3354ffded5d85b020cfd7424965`.

## Correctifs inclus

- Animation de production `SoloBonusAnimations.png` : 4 lignes x 8 frames.
PRISON = cage rocheuse; GLACE = cristallisation; LAVE = magma; METEORE = roche + fumée.
- Les projectiles utilisent uniquement les frames d'approche; les frames de fermeture/impact
sont réservées au contact avec le boss afin d'éviter un effet final avant l'impact.
- Prison/Glace/Lave restent visuellement autour du boss pendant leur statut actif.
- Explosions d'astéroïdes : quatre variantes bornées, dont rééchantillonnage à 0,97x et 1,03x
(tonalité + tempo ensemble) et faible variation de volume, exclusivement sur le canal astéroïde.
- HUD SOLO : les barres PV/ENERGIE propres au diagnostic utilisent maintenant le safe-bottom inset;
HUD boss inchangé.
- `TEST BONUS OFF` cache les boutons sans pause/reset.
- `TEST BONUS ON` affiche PV, ENERGIE, FURTIF, MISSILES, PRISON, GLACE, LAVE, METEORE + TOUS X8.
- `TOUS X8` est atomique : les huit bonus sont placés dans un même lot ou aucun n'est validé
jusqu'à ce qu'une configuration accessible soit disponible.
- Les bonus sont toujours éjectés depuis le boss; WORLD conserve la protection contre les murs.
- Tourelles SOLO interdites; METEOR-009 reste désactivé.

## À vérifier physiquement sur Samsung

1. Les deux jauges joueur sont entièrement au-dessus de la navigation Android.
2. TEST BONUS peut être ouvert/fermé sans interrompre le pilotage.
3. Chaque pouvoir offensif arrive avec son animation et l'effet final apparaît seulement au boss.
4. PRISON forme visuellement une cage rocheuse autour du boss pendant l'immobilisation.
5. TOUS X8 éjecte visuellement les huit bonus ensemble.
6. Plusieurs explosions d'astéroïdes restent naturelles tout en étant moins répétitives.
