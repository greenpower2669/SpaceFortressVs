# debughistorical.md — SpaceFortressVs

## Pièges prouvés / à préserver
- Hall : ne jamais confondre difficulté campagne 4 niveaux et Danger Boss HOME 1..9.
- Anciennes entrées v1/v2 : `danger=0` = inconnu; ne jamais l'inventer ni l'envoyer au serveur.
- Points Hall : formule Fab avec secondes comme source du temps.
- Sync global : progression campagne locale et état réseau restent dans deux fichiers séparés.
- Sync global : ne jamais avancer le curseur avant persistance durable d'une page.
- Sync global : un `submissionId` est créé une fois par victoire locale et réutilisé à chaque retry/redémarrage.
- Sync global : la réconciliation au Hall doit fermer la fenêtre crash entre victoire durable et création de file réseau.
- Clés : aucune vraie clé de jeu dans Git/logs/tests/mémoires; aucune clé admin dans l'APK.
- Android astéroïdes : corriger copie générée, jamais `src/main.cpp`.
- Énergie : `nrj=0` plein, `nrj=50` épuisé.
- Cinétique/poussières : conserver les invariants validés; rouge sans effet gameplay.
