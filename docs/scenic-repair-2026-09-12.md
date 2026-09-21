# Space Fortress 1.0.3 — Soleil et Jupiter

## Cause observée

La capture Android présente un Soleil coupé à angle droit et un morceau de
planète bleue à la place de Jupiter. Les fichiers réellement embarqués avaient
seulement 128 × 121 et 216 × 240 pixels, avec une silhouette déjà tronquée.
Le recadrage supplémentaire du renderer retirait encore 5,5 % / 2,5 % de chaque
côté. Les limites employaient `WIDTH/HEIGHT`, alors que `main.cpp` agrandit ces
dimensions du monde d'environ 10 % par rapport à l'affichage.

## Correction

- Nouveaux sprites complets en PNG RGBA 768 × 768, halo transparent pour le
  Soleil, bandes crème/orange et Grande Tache rouge pour Jupiter.
- La zone visible vient de `SDL_RenderGetViewport`, qui tient compte de
  l'échelle du renderer. Les astres sont contenus dans cette zone avec marge.
- Suppression du recadrage destructif et des cercles d'aura superposés.
  Les PNG portent leur propre transparence ; filtrage linéaire à leur chargement.
- Les deux PNG canoniques prennent priorité sur les anciens transports dans
  le paquet Android. Les autres visuels et le source historique restent intacts.

## Vérification

Les tests natifs avec SDL2/SDL2_image réels et UBSan réussissent. Les nouveaux
tests inspectent les pixels dessinés sur des surfaces 720 × 1560, 1080 × 2340
et 640 × 360, avec les dimensions du monde artificiellement plus grandes.
Ils exigent une silhouette circulaire visible et une marge aux quatre bords,
en haut à gauche comme en bas à droite. Les douze recréations du renderer
vérifient désormais aussi les textures de Jupiter. Les trois tests Python et
la vérification syntaxique du source Android généré réussissent également.

L'aperçu de contrôle est produit par le même chemin `SDL_RenderCopy` que le
jeu, sur son fond de galaxie. Il montre uniquement les astres, sans simulation
de partie. Aucune validation physique sur téléphone n'a été effectuée ici.
Les limites de synchronisation du moteur documentées dans l'audit 1.0.2
restent applicables.

## Références techniques

- https://wiki.libsdl.org/SDL2/SDL_RenderGetViewport
- https://wiki.libsdl.org/SDL2/SDL_SetTextureScaleMode
- https://wiki.libsdl.org/SDL2/SDL_SetTextureBlendMode

## Provenance des images

Génération avec l'outil intégré Imagegen, suivie uniquement d'une réduction
de la toile complète à 768 × 768 avec Pillow/Lanczos, en conservant l'alpha.
Fichiers intégrés : `assets/pict/remaster/sun.png` et
`assets/pict/remaster/planet.png` (Jupiter).

Prompt Soleil :

```text
Use case: stylized-concept. Asset type: a production-ready transparent RGBA PNG sprite for the mobile space game Space Fortress. Create ONE isolated full round SUN, straight-on spherical view. High-quality detailed solar granulation and swirling orange-gold plasma with a warm yellow-white surface, several small elegant solar prominences and a soft naturally fading corona. Beautiful realistic game art, smooth fine detail at 1024x1024, not pixel art. Full solar disc centered occupying about 68 percent of the square width; all prominences and glow remain within the canvas with at least 8 percent completely transparent padding on every side. Genuine alpha-transparent background: no black matte, no checkerboard painted into pixels, no background stars or galaxy. Absolutely complete circular silhouette, no crop, no rectangular flat edges, no frame. No text, no watermark, no additional planet, no lens flare spikes. Legible at 300 pixels in a galaxy background.
```

Prompt Jupiter :

```text
Use case: stylized-concept. Asset type: a production-ready transparent RGBA PNG sprite for the mobile space game Space Fortress. Create ONE isolated fully visible JUPITER, front-on almost full phase, centered spherical gas giant, detailed flowing cream-white, tan, burnt orange and brown latitudinal cloud bands, recognizable Great Red Spot in the lower-right quadrant, subtle curved perspective at the limb. Beautiful realistic game art with fine cloud detail, smooth shading and a narrow soft warm atmospheric rim. Lighting from upper-left, soft shaded lower-right edge, still clearly visible entire disc. Square 1024x1024 image; circular planet occupies 76 percent of canvas width; at least 10 percent totally transparent padding all around. Genuine alpha-transparent background: no black matte, no painted checkerboard, no stars or galaxy. Full whole planet not a cropped quadrant, no rings, no blue oceans, no text, no labels, no watermark. Legible at 300 pixels.
```
