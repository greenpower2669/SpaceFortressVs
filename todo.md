# todo.md — SpaceFortressVs : tâches vivantes
> Réduit le 19/10/2026. Les **431 cases historiquement cochées, 231 anciennes cases ouvertes et leurs preuves** sont intégralement préservées dans [archive intégrale du 19/10](docs/memory-archive/2026-10-19-pre-compaction/todo.md). Ne pas transformer automatiquement des tâches anciennes sans preuve en « terminées ». Si une ancienne fonction redevient active, consulter ce journal archivé et réévaluer dans le code.

## État vérifié
- [x] Archives d'origine des cinq mémoires créées et vérifiées par empreinte Git dans `docs/memory-archive/2026-10-19-pre-compaction/`.
- [x] Les versions SOLO-027→034 ont une histoire CI/export enregistrée dans les archives (ne pas reprendre les anciennes cases « CI en attente » comme état courant).
- [x] SOLO-033 : commande directe 30 %, maintien directionnel ajoute graduellement +100 points, plafond 130 % historique, activation près du bord 90–98 %, conservation inertie/caméra/collisions.
- [x] SOLO-034 : parois visuellement plus épaisses via `exteriorRelief` sans modification de l'occupation Rock/Empty ; CI Android+C++/SDL2 `39215282189` GREEN ; export direct `39215282005` GREEN, commit exact `11fd5a7640704b7886a4647798c87c4c28ed4029`.
- [x] Retour utilisateur du 19/10 : **pilotage récent jugé positivement par Fab**, conserver comme référence.
- [x] Observation cratères / transition des roches détaillée, suivie du GO de Fab pour SOLO-035.

## Mission graphique SOLO-035 — **GO CODAGE par Fab le 19/10**
- [ ] En cours suite au GO de Fab : auditer le rendu de toutes les formations rocheuses du labyrinthe et proposer un **remplissage rocheux extérieur** intégré, avec cratères irréguliers de tailles/profondeurs variées.
- [ ] Garantir texture **identique/compatible** avec l'existant (palette, granulométrie, éclairage, relief). Assurer transition imperceptible entre **le plat rocheux horizontal du bas**, les rebords et les zones cratérisées ; pas de bordure plaquée, répétition ou motif carré.
- [ ] Pendant l'implémentation autorisée : conserver la collision/tuiles `Rock/Empty`, largeur jouable, matériau interne, zoom/parallaxe, caméra, contrôle SOLO-033, sons et autres modes. Ajouter tests continuité visuelle et non-régression du masque Rock.
- [ ] Source et tests SOLO-035 autorisés ; aucun merge vers main/Release. Ne déclarer APK disponible qu'après CI et extraction GitHub vérifiées.

## Validation terrain / réserves
- [ ] Vérifier si nécessaire sur Samsung, sans présumer validés : mesure physique vitesse à 1/2/4/8 s, manche/pad, virage/demi-tour, collisions/murs, difficulté APOCALYPSE et inversion G/D. **Retour positif global actuel**, pas certification exhaustive.
- [ ] Vérifier rendu SOLO-034 sur différents couloirs et zooms : épaississement perceptible, transitions, luminosité et lisibilité de tous les parcours (capture reçue, davantage de roche souhaitée).
- [ ] Distinguer sons publics `0/4` WAV Meteor privés dans SOLO-034 et APK privée Djipie SOLO-025 `4/4`. Toute nouvelle intégration sonore nécessite accès autorisé et preuve.
- [ ] Avant toute reprise d'une ancienne mission non déclarée close, revoir les **anciennes cases ouvertes** dans archive, comparer à la CI/code réel, puis créer des tâches courantes pertinentes.
- [ ] Toute nouvelle livraison de code autorisée : 3 suites CI au même SHA, `.apk` direct sous `temp/<mission>/` sur branche, SHA256 contrôlé ; pas de ZIP chez Fab ; AAB si prévu et distinguer non-signé ; pas de fusion `main` / Release sans Fab.

### SOLO-035 — GO Fab, incrustation cratères (19/10/2026)
Fab autorise désormais le **codage** du remplissage rocheux cratérisé après audit et compaction des mémoires. Ne pas créer un second matériau : utiliser exactement la source `ParoisModeSolo.png`, le warping de `soloRockMaterialAt` et la lumière existante de `SoloOrganicRims`. Le nouvel `src/solo_crater_relief.hpp` module uniquement les RGB des texels Rock : grandes et petites dépressions déterministes en coordonnées mondiales, centres/ellipses irréguliers, lèvres éclairées et creux plus sombres, atténuation douce dans la matière autour des ouvertures. Le bas plat, les bords latéraux et les masses internes doivent se confondre sans raccord. `Rock/Empty`, silhouette alpha, collisions, largeur des passages, `ShipMotion`, commande 30%+100 points, caméra, sons et modes historiques intouchés. Tests SDL de continuité sur les frontières de chunk et de contraste, puis CI C++/SDL2/Android du même commit, et export APK direct sous `temp/solo-035/` avec SHA256. Aucune fusion main/release.
