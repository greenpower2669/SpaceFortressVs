# debughistorical.md — SpaceFortressVs

## Pièges prouvés

Archives : `docs/archive/2026-10-spacefortress-v1.4.0-history.md` et `docs/archive/2026-10-spacefortress-help-tutorial-danger9-history.md`.

- **Android / astéroïdes invisibles** : confusion `std::setw` / `fablib::setw(W)` ; corriger la copie générée, pas `src/main.cpp`.
- **Énergie** : `nrj=0` plein, `nrj=50` épuisé.
- **Cinétique** : aucun dégât inventé à vitesse nulle ; danger hostile exclu de tous les chemins cinétiques ; éviter tout double impact couche+coque.
- **Poussières** : rouge visuelle uniquement ; blanche recharge puis soin et reste physiquement libre.
- **Surcharge** : le tuto doit compter le vrai maintien jusqu’à 2 s ; ne pas plafonner le temps cumulé à 0,25 s. Armé = champ OFF jusqu’au relâchement.
- **Danger** : utiliser le helper canonique sur le non-cinétique ; les tests ne doivent pas exiger une ancienne expression directe du multiplicateur.
- **Aide en jeu** : `help_live_bridge.hpp` reste le routeur final ; il doit consommer le doigt du `?` et conserver l’état vivant avant reprise.
- **Formats aide** : RAPIDE/DETAILLE = schémas fixes ; ANIME = horloge live. `help_format_bridge.hpp` isole cette différence.
- **Signature Android** : ne jamais conseiller désinstallation/effacement des données comme contournement.

Anciens SHA, logs et essais résolus restent dans les archives/Git, pas ici.
