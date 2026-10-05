# debughistorical.md — SpaceFortressVs

## Pièges prouvés / à préserver
- Hall : ne jamais confondre difficulté campagne 4 niveaux et Danger Boss HOME 1..9.
- Anciennes entrées v1/v2 : `danger=0` = inconnu; ne jamais l'inventer ni l'envoyer.
- Points Hall : formule Fab avec secondes comme source du temps.
- Sync : save campagne et état réseau séparés; global ne modifie jamais `cleared/selected/pending`.
- Sync : `submissionId` immuable après création; retry réutilise l'identité.
- Sync : page réseau validée/stagée puis écrite durablement avant avance du cursor.
- Sync : callback vieux cycle/cursor doit être ignoré; erreur réseau/auth/5xx garde pending/cache/cursor.
- Sync auto : aucun timer; une seule pagination et un seul upload in-flight par `submissionId`.
- Sync UI : observer les transitions à la frontière finale des événements, pas depuis `sfCampaignDrawHall()`.
- HTTPS/JNI : Java seul encode/décode JSON; C++ reçoit des valeurs typées, vérifie cycle/cursor et persiste. Échec JNI doit dégrader vers local-only, jamais faire crasher le jeu.
- Clés : aucune vraie clé de jeu dans Git/logs/tests/mémoires; aucune clé admin dans l'APK.
- Android astéroïdes : corriger copie générée, jamais `src/main.cpp`.
- Énergie : `nrj=0` plein, `nrj=50` épuisé.
