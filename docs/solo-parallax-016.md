# SOLO PARALLAX-016

## Contrat et portée

Fond graphique du vrai SOLO GAME-015, même rendu dans MONDE TEST. Le moteur,
la carte, la caméra circulaire, les collisions, les acteurs, le minage et les
bonus ne changent pas. L'arène finale conserve son rendu historique.
`src/main.cpp`, CLASSIQUE, DUEL et COOP sont intacts. Aucune Release ni merge main.
Les demandes concernant les astéroïdes stationnaires, la couronne lente de caméra
et les nouvelles poursuites de miniboss ne font pas partie de cette livraison.

## Composition

| Plan | Ressource | Déplacement relatif à la caméra |
|---|---|---|
| Ciel lointain | fond4hlow.png historique | 0,08 |
| Minéral ajouré | FondsParallaxeModeSolo.png | 0,34 |
| Parois physiques | ParoisModeSolo.png existant | 1 |
| Environnement | EnvironnementsModeSolo.png existant | 1 |

Les facteurs sont des choix de présentation, pas des coefficients physiques.
Le retour de caméra restitue les mêmes pixels. Pas de défilement autonome.
Le ciel est visible au travers des vrais pixels alpha0 de la nouvelle planche.
La répétition en miroir partage les texels de bord. Les neuf ouvertures sont
différentes ; la planche couvre 2,5 largeurs d'arène et n'est jamais étirée sur
une immense texture du monde.

Les cellules Rock sont opaques. La matière provient des bandes peintes du
concept-sheet, sans titres ni panneaux. Les bevels restent à l'intérieur des
contours de collision ; aucun faux passage arrondi n'est créé dans un mur.
Les effets lave/énergie/glace proviennent des décors existants. Aucun astéroïde,
projectile, objet minable ou danger supplémentaire n'est créé par le renderer.
Les motifs en miroir restent perceptibles : le jugement visuel Samsung demeure
nécessaire, sans promettre une finition photoréaliste.

## Architecture et budget

- `solo_scenery_atlas.hpp` : rectangles source partagés, sans Session.
- `solo_scenery_assets.hpp` : extraction identique à celle du prototype,
  ressources détenues par renderer, sans globals.
- `coop_solo_scenery.hpp` : trois plans, visible seulement, géométrie SDL2.
- `coop_solo_session_runtime.hpp` : branche monde uniquement ; rechargement sur
  SDL_RENDER_DEVICE_RESET, libération avant destruction du renderer.
- `solo_renderer.hpp` : ancien SoloAssets hérite du propriétaire commun ; même
  découpe et mêmes effets alpha dans l'ancien prototype.

Pas d'allocation d'image, chargement disque ou traitement de pixels à chaque
frame. Nombre de cellules dessiné borné par l'écran. Textures supplémentaires
d'environ20Mo décompressés, dont environ3Mo de découpes de parois communes
encore inutilisées dans le nouveau renderer. Pas de FPS Android revendiqué.

La géométrie emploie un éventail de quatre triangles par fragment : le raccourci
rectangle/copie du renderer logiciel laissait des rangées vides ; le scissor
entier intermédiaire faisait de même à échelle fractionnaire. Aucun scissor
par cellule dans la version finale. Les UV sont bornés pour éviter les rejets
SDL dus à de minuscules valeurs négatives aux grandes coordonnées monde.

## Nouvelle ressource

Chemin : `assets/pict/solo/FondsParallaxeModeSolo.png`.
PNG RGBA1254×1254,3 376 564octets,21,81% de pixels entièrement transparents.
SHA256 : `4becaec70bf7f23d823c3eccbd902b86aa4dbb6942115a1389409bc6a7f7df44`.
Générée avec l'outil ImageGen intégré, `transparent_background=true`, copiée
sans retouche ni recompression. Aucun atlas existant remplacé.

Prompt de génération :

> Create a production-ready 2D game environment texture atlas named FondsParallaxeModeSolo.png for a top-down space exploration game. One single large square mineral plate, desired 2048x2048, covering the complete canvas edge to edge. Orthographic top-down, no perspective. Dark charcoal gray rocky asteroid material with muted cool blue highlights, layered irregular stone and subdued fine mineral texture, matching a classic detailed painted space shooter. The plate is pierced by 9 varied rounded irregular cavern apertures, different sizes, distributed naturally across the image. The interiors of these apertures MUST BE REAL FULLY TRANSPARENT ALPHA, empty cutouts. Broad openings, approximately 35 percent of image area transparent. The openings have naturally rounded scalloped rocky edges, subtle bevel highlights and recessed dark shadows painted ON THE ROCK, never opaque fills inside the openings. Show only the intermediate mineral layer. NO starfield, NO galaxy, NO colored background, NO checkerboard painted into pixels, NO labels, NO text, NO grid, NO distinct sprite panels, NO ships, NO objects. Keep entire outer perimeter opaque continuous rock, with no holes touching the image boundary, for mirrored tiling. Irregular openings and material variation avoid obvious repetition. Quiet low-contrast background artwork so gameplay walls and ships remain legible. Export as transparent PNG retaining fully transparent holes.

Le résultat réel est1254² et21,81% de transparence, contrairement aux dimensions
et proportions souhaitées dans le prompt ; ces valeurs réelles sont acceptées
et validées automatiquement.

## Vérification

`tests/coop_solo_scenery_regressions.cpp`, exécuté par la suite standard :
chargement réel, alpha, retour exact de caméra, carte intacte, lave traversable,
comparaison rectangle/32cellules à1/.75/2048÷4800/1.25, murs sur deux fonds,
caméra fractionnaire, viewport/clip/scale/blend/couleur, capture haute densité
de l'antre, release idempotent et reload. Revue indépendante du lifecycle,
de la mutualisation et de l'isolation historique effectuée ; défaut de raccord
signalé puis couvert par les tests ci-dessus.

CI complète et livraison Android : voir les cinq mémoires et le futur manifeste
`temp/solo-parallax-016/`. Validation Samsung nécessaire : lisibilité du vaisseau,
continuité perçue, mouvement indépendant des fonds, minage/combat, entrée de
l'antre, pause/reprise et fluidité.


## SOLO-023 — superposition des rebords (évolution du rendu)
Le fond `fond4hlow.png` et `FondsParallaxeModeSolo.png` ne changent pas. Le rendu `Rock` utilise désormais la planche 48 silhouettes `HabillageRebordsModeSolo.png` en complément de la bande-source des six thèmes `ParoisModeSolo.png`. La carte jouable et le frustum n'ont pas changé ; le nouveau dégradé de surface est précalculé au chargement, jamais à chaque frame. Le vieux rendu rectangulaire reste un secours automatique si les textures supplémentaires manquent. Voir `ordres-de-mission.md` SOLO-023 pour la procédure de revert, à exécuter uniquement après validation. Test final perceptif Samsung requis.


### SOLO-023 — livraison finale vérifiée, 17 octobre 2026
- Code du nouvel habillage `39d0a7c2b6969f546a7dc7537b3bcf993b097dcb`, correction premier test `51752da4dd74fffc9b8c0a1448b2d6eb27fd3fcc`, deuxième `57f74d8c4c7437d01cf84d7edea200a57bf4e672`, troisième **`48b30c4a6d7c200fc2979bb411a3a048b4a3348f`** (supprime une ancienne assertion RGB littérale devenue incompatible avec la roche recomposée ; l'opacité et les coins transparents restent testés).
- **CI source entière verte** run `39058401438` : suite SOLO, SDL2 et Android ; Release publique SKIPPED.
- **Export direct vert** run `39058427941`, `temp/solo-rock-rims-023/` ; les fichiers réels ont été vérifiés par métadonnées Git et leurs SHA256.
- APK diagnostic : `SpaceFortressVs-1.4.2-SOLO-ROCK-RIMS-023-48b30c4.apk`, **98 710 354 octets**, SHA256 `8ef3769b08a200f382465907486f28c36d54b300294fa17fdafcbb8c0cdb0e71`.
- AAB non signé : `SpaceFortressVs-1.4.2-SOLO-ROCK-RIMS-023-48b30c4-unsigned.aab`, **95 998 932 octets**, SHA256 `df8a5097fe619a2316bb51cc8705bdbded916d3f92225af5dc2a1aacbdfe065b`.
- **Rollback complet code/test, après validation utilisateur** : sur feature/solo-coop-core-v1, faire DANS CET ORDRE
  1. `git revert 48b30c4a6d7c200fc2979bb411a3a048b4a3348f`
  2. `git revert 57f74d8c4c7437d01cf84d7edea200a57bf4e672`
  3. `git revert 51752da4dd74fffc9b8c0a1448b2d6eb27fd3fcc`
  4. `git revert 39d0a7c2b6969f546a7dc7537b3bcf993b097dcb`
  Résoudre si nécessaire conflits de mémoire et **conserver la présente note d'historique**, puis CI. Atlas et générateur restent conservés mais inutilisés, retrait séparé seulement si explicitement demandé.
- Point à vérifier par Fab sur Samsung : la fin des carrés, la répétition éventuelle du matériau rocheux, la sensation de collision près des rebords, le zoom/pan et la fluidité. Validation perceptive non revendiquée avant essai réel.
- Aucun merge main, aucun tag/Release.
