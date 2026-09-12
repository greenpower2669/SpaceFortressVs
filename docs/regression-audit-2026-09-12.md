# Space Fortress — audit des versions récentes

Base de reprise : `ui/start-screen-v1`, commit `cf2466c` du 8 septembre 2026.
`main` est toujours la version historique de février 2024. Les correctifs
s'appliquent à la branche du remaster Android et conservent `src/main.cpp`.

## Versions comparées

| Révision | État constaté |
| --- | --- |
| `c215337` | Images d'interface stockées en base64 ; leur décodage est ajouté plus tard. |
| `0ad717d` | Transport du bouclier rouge corrigé ; cœur bleu et explosion encore corrompus. |
| `cf2466c` | Réinitialisation des astéroïdes et suivi des textures ajoutés ; références conservées après destruction du renderer. |

## Corrections 1.0.1-stability

- **Images corrompues** : le cœur bleu a un IDAT invalide ; l'explosion a une
  longueur de chunk incohérente. Leurs empreintes exactes déclenchent l'emploi
  des images historiques valides. Le build vérifie les 18 PNG du remaster
  (CRC, flux zlib complet et dimensions des lignes). Une nouvelle corruption
  non reconnue fait échouer le build. Les lots `scenic_round` et `scenic_v3`
  incomplets restent exclus ; leur présence dans Git ne prouve pas leur usage.
- **Chargement** : retour au fichier historique si un remaster est absent ou
  illisible, avec normalisation de la casse du bouclier bleu.
- **Reprise d'affichage** : retrait des références aux textures détruites,
  par renderer, et rechargement des effets de tir et de l'engrenage orange.
  SDL détruit les textures associées lorsque leur renderer est détruit :
  <https://wiki.libsdl.org/SDL2/SDL_DestroyRenderer>.
- **Retour Android** : `SDLK_AC_BACK` est une valeur d'énumération de SDL2 ;
  le tester avec `#ifdef` supprimait le code de gestion. Il est désormais
  traité directement dans le gestionnaire final.
- **Multitouch** : tous les doigts ayant commencé dans le menu ou sur un
  engrenage sont suivis jusqu'au relâchement. Les gestes interrompus par
  l'arrière-plan sont annulés, ainsi que les contrôles actifs. Un Retour juste
  après Jouer annule aussi le lancement différé.
- **Calculs** : vecteur nul traité sans division par zéro, entrées de la
  trigonométrie bornées pour les arrondis, composante Y du constructeur réparée.
- **Source Android générée** : initialisation de tous les seuils de particules,
  retours explicites des fonctions non void, reconfiguration si `main.cpp`
  change. Les corrections précédentes de `suiveur` et de la vitesse Y de l'IA
  sont conservées.

## Vérification et limites

Les tests hôtes utilisent SDL2 et SDL2_image réels, un renderer logiciel, les
en-têtes du jeu et UBSan. Ils couvrent douze recréations du renderer, deux
renderers distincts, le chargement de secours, les vecteurs et les événements
de menu/Retour/multitouch. La source Android générée passe une vérification de
syntaxe avec les retours manquants traités comme erreurs.

Ces tests ne remplacent pas une partie sur un appareil Android. Restent à
vérifier : mise en arrière-plan/reprise réelle, deux parties successives, IA,
collisions, son et cadrage sur l'écran du téléphone. Le moteur historique
partage encore des états entre plusieurs threads ; cet audit ne certifie pas
l'absence de courses de données. La remise à zéro actuelle ne vide pas tous
les projectiles/effets de la partie précédente. Leur correction demande une
synchronisation des producteurs et de la boucle de jeu, avant toute promesse
de réinitialisation complète. La refonte graphique supplémentaire attend la
validation de cette base jouable.
