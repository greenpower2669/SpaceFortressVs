# debughistorical.md — SpaceFortressVs

## Pièges prouvés à garder en mémoire

Historique détaillé : `docs/archive/2026-10-spacefortress-v1.4.0-history.md`.

- **Android / astéroïdes invisibles** : confusion `std::setw` / `fablib::setw(W)` dans la source Android générée. Corriger la copie générée, pas `src/main.cpp`.
- **Énergie inversée par rapport à l'intuition** : `nrj=0` plein, `nrj=50` épuisé.
- **Cinétique v3** : aucun plancher de dégâts ; vitesse nulle = zéro dégât cinétique inventé ; danger boss exclu des chemins cinétiques.
- **Double impact interdit** : une interaction réelle avec une couche cinétique ne doit pas retomber sur une collision coque complète dans la même étape.
- **Poussières** : rouge = visuel uniquement ; blanche = recharge puis soin et n'est pas déviée.
- **Surcharge** : 2 s ; pendant charge champ ×2 ; une fois armée, champ OFF jusqu'au relâchement ; relâchement armé = purge.
- **HOME** : le sélecteur de danger ne doit jamais lancer le combat ; `ROCK N ROLL` = défaut ×10 interne.
- **Signature Android** : une signature de test différente n'autorise jamais à conseiller désinstallation ou effacement des données.

Tout bug clôturé, ancien SHA, logs CI, essais abandonnés et anciennes valeurs restent dans l'archive/Git et ne doivent pas être recopiés ici.
