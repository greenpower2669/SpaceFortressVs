# SpaceFortressVs 1.2.0 — incursions et publication

Base de travail : 1.1.0 (`b606d4a`). Version 1.2.0, code Android 6.
Ce document conserve les règles demandées par Fab et les choix de réglage.

## Règles du jeu

- La jauge d'énergie appartient uniquement au vaisseau. Tirer depuis une
  tourelle ne la modifie pas. Les dégâts reçus et le tir du vaisseau gardent leur
  effet ; collecter des poussières recharge. `nrj` est une dépense/chauffe,
  donc une valeur basse correspond à une bonne réserve.
- Quand `nrj < 23` et les points de vie sont au moins 650, l'IA peut prendre
  une posture agressive de quelques secondes. Les occasions sont espacées de
  4 à 7 s ; elles ne déclenchent pas systématiquement une attaque. Elle avance
  davantage, anticipe et tire plus vite après s'être alignée. La posture cesse
  si la réserve ou les points de vie deviennent insuffisants.
- Dans cette posture, un astéroïde vivant qui avance vers le camp ennemi à une
  vitesse rattrapable peut devenir une cible de minage. L'IA le suit par son
  identifiant stable, en restant derrière lui. Elle tire sur sa position prévue
  et traverse les poussières laissées dans son sillage.
- Ce suivi autorise une courte incursion au-delà du milieu de terrain. Après
  au plus une seconde passée chez l'ennemi, une profondeur de 57 % du terrain,
  une cible perdue/inversée, une réserve insuffisante ou un tir menaçant, elle
  abandonne la cible et rentre. Une limite de 10 s évite les poursuites infinies.
  Un délai de 8 s avant une nouvelle incursion évite les allers-retours immédiats.
- La retraite vise une zone sûre vers 29 % de la hauteur. Le pilote compare les
  trajectoires face aux projectiles ennemis, avec une prévision de 0,65 s, en
  plus des astéroïdes. Le retour termine sous 34 % de la hauteur. Les positions
  sont déplacées progressivement : aucune téléportation à la frontière.

## Tourelles

Six tourelles par camp : deux à l'arrière, deux sur les flancs et deux près de
la frontière. Leur déploiement et leur rétraction restent animés.

Chaque tourelle possède une `virtualTarget` invisible, calculée depuis la
position et la vitesse observées de l'adversaire. L'interception tient compte
du temps de trajet depuis le canon, avec un horizon borné à 0,9 s. La rotation
est progressive ; le tir exige l'alignement et l'entrée effective de l'ennemi
dans la zone. Cette cible virtuelle n'est pas un sprite ni une cible physique.

La vitesse des tirs passe de 0,95 à 1,5 largeur d'écran/s. Les intervalles
passent de 0,64–0,84 s à 0,38–0,555 s, décalés entre tourelles. Leur budget de
72 projectiles par camp est indépendant des 50 projectiles possibles du
vaisseau. La limite tactique des tirs principaux de l'IA reste séparée.

Les tirs défensifs conservent une vitesse en pixels/s ; leur déplacement se
convertit au temps de la frame. La collision balaie également le segment
parcouru pour détecter un passage entre deux images. L'effet énergétique sur
la cible est normalisé, sans dépendre de la durée de cette frame. Les éclairs
des armes principales restent séparés des éclairs des tourelles.

## Validation et limites

Tests natifs avec SDL réel, UBSan et itérateurs contrôlés :

- suivi d'un identifiant d'astéroïde, entrée chez l'ennemi, abandon et retour à
  30/60 images/s ; arrêt de l'agressivité quand l'énergie baisse ;
- retraite déclenchée par un projectile, destruction de la cible suivie sans
  accès invalide et esquive mesurée sur toute la trajectoire ;
- cibles virtuelles en avance sur la position observée, cadence et vitesse
  augmentées, budgets séparés, réserve des deux vaisseaux inchangée par les
  tirs défensifs ;
- passage d'un projectile rapide entre deux frames et normalisation de son
  déplacement et de l'effet sur la cible ;
- tous les contrôles de stabilité, de collecte, de rendu et de Jupiter de 1.1.0.

Les contrôles automatiques exercent des scénarios déterminés ; ils ne
garantissent pas l'esquive de toute configuration dense. Le minage et la
production des poussières conservent la mécanique historique. Le ressenti,
les collisions très denses, le son et la reprise Android restent à vérifier
sur téléphone. Aucun essai physique sur appareil n'est revendiqué ici.

## Règles de version et de livraison

- `android/version.properties` est la source unique de version pour Gradle et
  les noms des fichiers. Incrémenter la version et le code pour la livraison
  suivante et fournir `docs/releases/<version>.md`.
- Après tests et compilation, le paquet vérifie le manifeste et la signature
  de l'APK, les archives, le moteur ARM64 et les astres embarqués. Les APK/AAB
  reçoivent le nom du jeu et la version. SHA-256 et commit accompagnent les fichiers.
- Une compilation réussie de la branche `ui/start-screen-v1` peut publier cette
  version en release GitHub. Les pull requests construisent et vérifient les
  fichiers, sans publier. Le job de publication vérifie le commit et les
  empreintes, transfère les fichiers dans un brouillon puis rend la release publique.
- Une version publiée ne doit jamais être remplacée par un autre commit ou
  d'autres octets. Les tests de publication refusent un paquet altéré, un autre
  commit et la réutilisation d'une version existante.
- L'APK est installable avec la signature de test de la compilation ; l'AAB de
  production reste non signé tant que la clé d'upload n'est pas configurée.
  Publier sur GitHub n'est pas envoyer l'application sur Google Play.

## Sources primaires

- C. W. Reynolds, *Steering Behaviors For Autonomous Characters*, GDC 1999 :
  <https://www.red3d.com/cwr/steer/gdc99/>. Référence pour séparer objectif,
  pilotage et déplacement, puis combiner poursuite, évitement et retraite.
  Les seuils de cette version sont des choix de jeu, pas des valeurs du papier.
- GitHub, authentification des workflows :
  <https://docs.github.com/en/actions/tutorials/authenticate-with-github_token>.
  Le droit d'écriture est limité au job qui publie la release.
- GitHub CLI, releases et transfert de fichiers :
  <https://cli.github.com/manual/gh_release_create> et
  <https://cli.github.com/manual/gh_release_upload>.
