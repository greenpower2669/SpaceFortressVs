# Campagne coopérative et anticipation — 1.3.0

## Intention

Fab demande que l'IA et les tourelles suivent un double invisible qui prolonge
le mouvement de leur cible, et une campagne coopérative de cinquante monstres
animés, avec des arènes variées et un Hall of Fame après victoire.

Le mode coop est local sur un écran, à deux humains ou avec un partenaire IA.
La progression et le classement sont locaux. Il n'y a pas de serveur ni de
multijoueur réseau. La campagne complète comprend cinquante rencontres ; les
familles d'attaques sont réutilisées et combinées selon le palier et la phase.

## Anticipation

`SfVelocityGhost` contient position et vitesse observée. L'interception résout
`|cible + vitesse * t - origine| = vitesse_tir * t`, choisit une racine positive
et borne l'horizon. En l'absence d'interception valable, le calcul utilise une
estimation bornée. Le point n'est jamais ajouté aux sprites ou aux collisions.
Les armes principales IA tirent réellement vers ce point ; la chasse emploie
ses coordonnées X et Y. Les tourelles partagent le même calcul à partir de
leur bouche. Les vitesses de tir sont exprimées en pixels par seconde.

En coop, les déplacements, tirs et collisions sont simulés par sous-pas de
1/90 s au plus, sous le verrou existant. L'observation de vitesse intervient
après le déplacement, une seule fois par sous-pas. Les anciennes boucles de
physique passent leur tour pendant la coop. Les collisions de projectiles
utilisent le segment parcouru entre deux positions.

Orion anticipe le boss, choisit un flanc, évalue plusieurs vitesses pour éviter
projectiles, rayons, ondes, astéroïdes et bords, récupère les poussières sûres
quand son énergie baisse et cherche à secourir son partenaire. La prédiction
suppose un mouvement continu : un changement de direction peut la tromper.

## Cinquante rencontres

Le catalogue canonique est `src/boss_catalog.hpp`. Pour l'indice `i` de 0 à 49 :

| Paramètre | Valeur |
| --- | --- |
| Vie du boss | `900 + 72*i + 2*i*i` |
| Intervalle de salve | `1.75 - 0.021*i` secondes |
| Vitesse de projectile | `(0.24 + 0.004*i) * largeur` par seconde |
| Dégâts de base | `12 + 0.6*i` |
| Famille / palier | `i % 10` / `i / 10` |
| Changement de phase | à 65 % et 30 % de vie |
| Enrage final | intervalle multiplié par `0.82` |

Les dix familles attaquent par éventails anticipés, spirales, anneaux troués,
rayons préparés, mines, croix tournantes, spores à guidage limité, vagues
sinusoïdales, ondes expansives et salves depuis des satellites. Les deuxième
et troisième phases alternent la famille principale avec une autre attaque.
Le premier boss apprend l'esquive ; les suivants augmentent les paramètres
de difficulté. Le classement exact de difficulté ressentie reste à affiner
avec des parties humaines : les attaques demandent des gestes différents.

Chaque monstre utilise une déformation de maillage 9 × 9, spécifique à sa
famille, avec respiration, membres mobiles, oscillation, coloration à l'impact
et contraction/fondu avec fragments à la mort. Les animations ne sont pas
cinquante vidéos : le moteur anime cinquante illustrations distinctes.

Les deux vaisseaux tirent automatiquement, avec un coût énergétique propre.
Douze tourelles alliées tirent gratuitement pour le vaisseau. Les astéroïdes
abattus donnent sept poussières : chacune rend quatre unités d'énergie et
huit points de vie au seul allié le plus proche qui la collecte. Trois secours
sont partagés par combat, avec deux secondes de proximité requises.

## Sauvegarde et interface

`SDL_GetPrefPath("greenpower2669", "SpaceFortressVs")/campaign-v1.dat` contient
la progression, la mission suivante, les noms, toutes les victoires et une
éventuelle victoire attendant les noms. Une victoire est écrite avant la
saisie ; sa validation conserve son identifiant et empêche les doublons.
Les noms de deux pilotes et de l'équipe sont obligatoires (24 caractères
chacun). Les accents sont conservés dans le fichier, puis translittérés pour
la police bitmap existante. Le clavier SDL gère saisie, composition et retour.

Le fichier temporaire est écrit, synchronisé, puis renommé ; une copie d'un
fichier primaire valide est conservée. Un échec laisse le résultat à l'écran
avec possibilité de réessayer. Un fichier inconnu ou illisible n'est jamais
remplacé silencieusement par une sauvegarde vide. Le Hall est paginé, classé
par boss puis score et durée. Aucun record n'est supprimé automatiquement.

Les cinquante cartes sont accessibles depuis les modes coop de l'accueil.
Une carte verrouillée ne lance pas de combat. Retour Android et arrière-plan
effacent les contacts en cours ; le combat reste en pause jusqu'à Reprendre.
Les dimensions et animations des vaisseaux sont restaurées au retour en duel.

## Assets et provenance

Trois images ont été créées avec ImageGen pour cette version, copiées telles
quelles dans `assets/pict/campaign/`. Les briefs ci-dessous décrivent les
intentions de génération ; ils ne sont pas une transcription des prompts.

| Fichier | Contenu et brief | SHA-256 |
| --- | --- | --- |
| `bosses.png` | Cinquante monstres spatiaux vus de dessus, dix familles organiques/minérales/mécaniques, cinq paliers visuels, fond transparent ; 1774 × 887 RGBA | `7578025e0035e0fab352fa52157f12fc83f94d3a02b489c76aaba8753d9d98c6` |
| `planets.png` | Mondes variés, océans, glaces, laves, gaz, surfaces extraterrestres, fond transparent ; 1774 × 887 RGBA | `bf6d7ceaeebf8446f0c06d9505f7d5fbedc8ab4a553b40e3a42da1f5cebf3e62` |
| `nebulae.png` | Six ambiances spatiales sombres cyan, ambre, violet, jade, glace et rouge, sans texte ; 1536 × 1024 RGB | `2269038fc19b60dca57de87e934611d17ef434498f851d6eee42b114e9e885f4` |

La planche de planètes produite contient 55 cellules, dont cinquante sont
utilisées. Cinq planètes à anneaux débordants sont exclues. Le rendu prélève
un disque dans chaque cellule. Les monstres ont des cellules mesurées dans
la planche, car ses gouttières ne forment pas une grille parfaitement régulière.
Six nébuleuses sont partagées avec cadrages, étoiles et dérives propres aux
rencontres ; il ne s'agit pas de cinquante images de fond indépendantes.
L'assemblage valide les PNG et la publication compare les trois SHA-256
contenus dans l'APK et l'AAB aux sources testées.

## Intégration et vérification

Le fichier historique `src/main.cpp` reste inchangé. Les branchements Android
sont effectués par remplacements gardés dans `scripts/prepare-legacy-source.cmake`.
Le rechargement du renderer après arrière-plan reste placé avant l'entrée
coop ; son cache de textures est invalidé avec les autres caches existants.

`tests/campaign_regressions.hpp` exerce les cinquante profils, les cinquante
maillages animés, l'interception aux trois fréquences, la collecte unique,
l'absence de tirs amis, les rayons annoncés, les secours, la pause et les
contacts, les transitions de menu, cinq combats sans modification de vie,
puis cinquante derniers coups et cinquante inscriptions durables relues.
Ce dernier test valide la progression, pas l'équilibrage humain de 50 combats.
Le rendu est vérifié avec SDL logiciel ; les performances et interactions
réelles Android nécessitent encore un essai sur téléphone.

La récupération du 19 septembre a permis de rejouer ces contrôles. Les cinq
combats simulés durent 8,3 s (boss 1, victoire), 17,9 s (10, victoire),
41,6 s (25, victoire), 85,7 s (40, victoire), 31,2 s (50, défaite).
Deux régressions supplémentaires reproduisent les corps trop grands en
paysage et l'absence de minerais après une collision destructrice. Elles
passent après correction ; le minage n'attribue jamais deux fois la ressource.
Les dimensions initiales sont vérifiées à 360 × 780, 780 × 360,
709 × 1536 et 1536 × 709. Les illustrations ImageGen sont inchangées.

L'icône de lancement est vectorielle, construite dans les ressources Android :
anneau de forteresse métallique, moitié orange, moitié bleue, réacteur central.
Elle possède un fond et un premier plan adaptatifs sur Android 8+, ainsi qu'un
drawable composé pour Android 6–7. Aucune nouvelle image raster n'est générée.

## Références primaires consultées

- Craig Reynolds, [Pursuit and Evasion](https://www.red3d.com/cwr/steer/PursueEvade.html) : prédiction à partir de la position et de la vitesse.
- Craig Reynolds, [Steering Behaviors for Autonomous Characters](https://www.red3d.com/cwr/steer/gdc99/) : poursuite et évitement combinés.
- SDL2, [SDL_RenderGeometry](https://wiki.libsdl.org/SDL2/SDL_RenderGeometry) : triangles texturés et couleur/alpha par sommet.
- SDL2, [SDL_GetPrefPath](https://wiki.libsdl.org/SDL2/SDL_GetPrefPath) : chemin de sauvegarde propre à l'application.
- SDL2, [SDL_StartTextInput](https://wiki.libsdl.org/SDL2/SDL_StartTextInput) et [Text Input](https://wiki.libsdl.org/SDL2/Tutorials-TextInput) : clavier, Unicode et composition.
