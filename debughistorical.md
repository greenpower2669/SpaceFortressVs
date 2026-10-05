# debughistorical.md — SpaceFortressVs

## Pièges prouvés

Archives : `docs/archive/2026-10-spacefortress-v1.4.0-history.md`, `docs/archive/2026-10-spacefortress-help-tutorial-danger9-history.md`, `docs/archive/2026-10-05-kinetic-dust-impact-v4.md` et `docs/archive/2026-10-05-hall-of-fame-danger9.md`.

- **Hall / deux difficultés distinctes** : ne jamais déduire le Danger Boss HOME depuis l’affrontement ou la difficulté de campagne. `VIF/ENDURANT/VICIEUX/ULTIME` et les 9 Danger Boss sont deux systèmes différents.
- **Hall / ancien historique** : les sauvegardes v1/v2 n’ont jamais stocké le Danger Boss. Leur `danger=0` signifie inconnu ; ne jamais reconstruire ou inventer 1..9 à partir du boss, du temps ou de l’ancien score.
- **Hall / points** : utiliser le danger réellement sauvegardé et la formule Fab `|boss*(danger-minutes)| + boss*(danger-minutes)` ; les secondes sont la source du temps.
- **Android / astéroïdes invisibles** : confusion `std::setw` / `fablib::setw(W)` ; corriger la copie générée, jamais `src/main.cpp`.
- **Énergie** : `nrj=0` plein, `nrj=50` épuisé.
- **Cinétique** : aucun dégât inventé à vitesse nulle ; Danger HOME exclu ; éviter tout double impact couche+coque.
- **Destruction / blanc** : toute destruction cinétique productrice de ressource passe par le helper canonique et son garde `pv>0` ; ne jamais réintroduire purge + collision + fragmentation comme émissions cumulatives.
- **Minage** : reste un chemin historique séparé ; ne pas l’assimiler à une destruction cinétique 10/100 %.
- **Blanc** : recharge puis soin et reste physiquement libre ; aucune déviation par vague.
- **Rouge** : réaction cinétique visuelle/mouvement seulement ; ne jamais toucher PV, énergie, danger ou équilibrage depuis ce chemin.
- **Surcharge** : maintien réel 2 s ; champ ×2 pendant charge, OFF une fois armé, purge au relâchement.
- **Aide en jeu** : `help_live_bridge.hpp` consomme le doigt du `?` et conserve l’état vivant avant reprise.
- **Signature Android** : ne jamais conseiller désinstallation/effacement des données comme contournement.

Anciens SHA, rouges intermédiaires, logs et essais résolus restent dans les archives/Git, pas ici.
