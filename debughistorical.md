# debughistorical.md — SpaceFortressVs

## Pièges prouvés / à préserver
- Hall : ne jamais confondre difficulté campagne 4 niveaux et Danger Boss HOME 1..9.
- Anciennes entrées v1/v2 : `danger=0` = inconnu; ne jamais l'inventer.
- Points Hall : formule Fab avec secondes comme source du temps.
- Sync global : progression campagne locale et cache réseau doivent rester dans deux fichiers séparés.
- Sync global : ne jamais avancer le curseur avant persistance durable d'une page.
- Sync global : `submissionId` d'une victoire doit survivre aux retries/redémarrages.
- Secret : aucune vraie clé API dans Git, logs, tests ou mémoires; aucune clé admin dans l'APK.
- Android astéroïdes : corriger copie générée, jamais `src/main.cpp`.
- Énergie : `nrj=0` plein, `nrj=50` épuisé.
- Cinétique/poussières : conserver les invariants validés; rouge sans effet gameplay.
