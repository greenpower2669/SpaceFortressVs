# Space Fortress 1.1.0 — anticipation, ressources et défenses

Base : `77b84a6`, version 1.0.3-scenic. Version Android : `1.1.0-tactics`,
versionCode 5. Le source historique `src/main.cpp` reste inchangé ; CMake
produit la copie Android avec des remplacements bornés et contrôlés.

## Comportements

- **Anticipation** : l'IA estime la vitesse du joueur à partir de ses positions
  observées, avec un lissage de 0,12 s. Elle vise une position future à au plus
  0,8 s, se déplace pour s'aligner, puis attend 0,16 s d'alignement avant de
  tirer. Ses décisions sont réévaluées toutes les 0,10 s. Un changement brutal
  de position ou une reprise après pause réinitialise l'estimation.
- **Évitement** : `fablib.hpp` fournit les vecteurs et l'azimut. Le pilote
  compare 19 vitesses possibles en prédisant leur rapprochement avec les
  astéroïdes vivants, pénalise les bords et limite son accélération. Les anciens
  blocs de poursuite/minage/forces ne déplacent plus simultanément l'IA.
- **Ressources** : lorsque sa réserve baisse, l'IA cherche une poussière
  accessible dans sa moitié de terrain. En l'absence de poussière convenable,
  elle peut se positionner derrière un astéroïde pour le miner. Le minage garde
  les impacts et poussières historiques ; un projectile consommé ne peut plus
  atteindre plusieurs astéroïdes pendant le même parcours ni réanimer un mort.
- **Recharge** : une poussière vivante ne peut être collectée qu'une fois.
  Si les deux forteresses se superposent, la plus proche la reçoit. Dans le
  moteur historique, `nrj` représente une dépense/chauffe : **réduire `nrj`
  recharge l'énergie**. La collecte applique `max(0, nrj*0.98 - 0.10*valeur)`
  et conserve la petite réparation historique, plafonnée à 1000 points de vie.
  Les poussières scintillent et un cercle signale la collecte. L'aide l'explique.
- **Tourelles** : trois par camp, aux positions des anciennes défenses de bord.
  Elles sortent à l'approche de l'ennemi, pivotent progressivement et tirent
  lorsqu'il a franchi le milieu. Déploiement d'environ 0,45 s, rétraction de
  0,77 s, temporisations de 0,64 à 0,84 s. Les tirs naissent à la bouche du
  canon, avec recul, éclair et son ; ils ne déclenchent pas un faux éclair
  sur la forteresse. Une marge d'hystérésis évite les ouvertures/fermetures
  rapides lorsqu'un joueur frôle la limite.
- **Jupiter** : déplacement lent et continu dans le rectangle réellement
  disponible à l'écran, avec deux sinusoïdes de fréquences différentes.
  Son diamètre reste stable, son disque reste entier et le mouvement ne se
  bloque pas contre un bord. C'est une animation de décor, pas un modèle orbital.

## Intégration et remise à zéro

`runtime_sync.hpp` fournit un verrou commun aux écritures de la simulation,
du rendu, des auxiliaires et de l'entrée tactile. Les attentes de cadence,
d'événements SDL et du retour au premier plan se font hors de ce verrou.
Les signaux de cadence et d'arrière-plan sont atomiques.

Une nouvelle partie, y compris via « Continue? », vide les projectiles,
poussières, explosions et traînées, remet à zéro les compteurs, le pilote,
les tourelles, les éclairs et le temps du décor. Le champ d'astéroïdes est
réinitialisé par le correctif existant. Cette protection ciblée ne constitue
pas une certification d'absence de toute course dans le moteur historique.

## Validation

Les tests hôtes utilisent les véritables SDL2/SDL2_image, un renderer logiciel,
UBSan et les itérateurs contrôlés de libstdc++. Ils vérifient :

- interception d'une cible fixe, mobile, trop rapide et téléportée ;
- absence de tir avant l'alignement et respect de la temporisation ;
- contournement d'une rencontre frontale à 30 et 60 images/s, en mesurant la
  distance minimale sur toute la trajectoire ;
- sélection du minage, collecte unique, recharge, plafonnement de la réparation ;
- déploiement, autorisation de tir, origine au canon, cadence et rétraction des
  tourelles pour les deux équipes ;
- remise à zéro des listes et effets, mouvement continu de Jupiter en portrait
  et paysage, ainsi que les régressions de stabilité et de textures précédentes.

Les trois tests Python d'assets et la vérification syntaxique de la copie
Android générée font également partie de la validation. Le workflow Android
exécute ces contrôles avant de produire APK et AAB.

L'aperçu facultatif `SPACEFORTRESS_TURRET_PREVIEW` utilise le véritable rendu SDL
des défenses sur le décor. Il ne simule pas une partie complète. Les collisions
très denses, l'équilibrage, le son et la reprise Android restent à évaluer sur
téléphone : aucun essai physique sur appareil n'est revendiqué ici. L'IA
anticipe un mouvement observé ; elle ne connaît pas les futures commandes.

## Fondements et sources primaires

- Craig W. Reynolds, *Steering Behaviors For Autonomous Characters* (GDC 1999) :
  <https://www.red3d.com/cwr/steer/gdc99/>. La séparation entre choix d'objectif,
  pilotage et déplacement, et la combinaison poursuite/évitement servent de
  référence de conception. Le choix de vitesses et les seuils sont propres au jeu.
- Interception à vitesse constante : résolution de
  `|cible + vitesse_cible*t - origine|² = vitesse_projectile²*t²`, puis borne
  temporelle. Si aucune racine future n'est exploitable, estimation de trajet
  bornée ; les changements de direction peuvent donc tromper l'IA.
- API officielle SDL : <https://wiki.libsdl.org/SDL2/SDL_RenderGeometry> pour
  les plaques et canons articulés, et
  <https://wiki.libsdl.org/SDL2/SDL_RenderGetViewport> pour les dimensions visibles.
- Primitives du projet : `src/fablib.hpp` (`enti::azim`, `tupl`, `tuplv`,
  `vlong`, `vtan`). Aucun réseau neuronal ni bibliothèque de navigation ajoutés.
