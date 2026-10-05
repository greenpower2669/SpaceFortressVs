# debughistorical.md — SpaceFortressVs

## Pièges prouvés / à préserver
- Hall : ne jamais confondre difficulté campagne 4 niveaux et Danger Boss HOME 1..9.
- Anciennes entrées v1/v2 : `danger=0` = inconnu; ne jamais l'inventer ni l'envoyer.
- Points Hall : formule Fab avec secondes comme source du temps.
- Sync : save campagne et état réseau séparés; global ne modifie jamais `cleared/selected/pending`.
- Sync : `submissionId` immuable après création; retry réutilise l'identité.
- Sync : page réseau validée/stagée puis écrite durablement avant avance du cursor.
- Sync : une page vide avec `hasMore=true` doit quand même persister `nextCursor` puis continuer.
- Sync : callback vieux cycle/cursor doit être ignoré; erreur réseau/auth/5xx garde pending/cache/cursor.
- Sync auto : aucun timer; une seule pagination et un seul upload in-flight par `submissionId`.
- Sync UI : observer les transitions à la frontière finale des événements, jamais depuis `sfCampaignDrawHall()`.
- Hall rendu : points globaux viennent du serveur; points locaux de `sfFamePoints`; pending local sans rang serveur; danger 0 reste visible/inconnu.
- HTTPS/JNI : Java seul encode/décode JSON; C++ reçoit des valeurs typées. Échec JNI doit dégrader vers local/cache, jamais faire crasher le jeu.
- Secret CI : ne jamais afficher la valeur; test/build sans clé d’abord, scan exact en mémoire, puis build final avec secret Actions optionnel.
- Clés : aucune vraie clé de jeu dans Git/logs/tests/mémoires; aucune clé admin dans l'APK.
- Livraison Actions : un rerun peut produire plusieurs artifacts portant le même nom. Toujours identifier l'artifact par tentative/ID et vérifier le SHA-256 du fichier remis au téléphone.
- Incident 2026-10-06 : `SYNC NON CONFIGUREE` provenait de l’APK sans clé de la tentative précédente (`11369888063`, SHA-256 `d7a24c1f1f50b569f45bb79cf2ad75ef2be834cd9d8ed5a95b38fe6edb0c72fe`) remis par erreur, alors que l’artifact configuré de la tentative 2 est `11374640381` (APK SHA-256 `0ca76a074b8148578b95a5b10a1fd32fad41d1c27cb46454ff2cd15d2133ab87`). Ne pas corriger le code pour ce symptôme avant re-test du bon APK.
- Android astéroïdes : corriger copie générée, jamais `src/main.cpp`.
- Énergie : `nrj=0` plein, `nrj=50` épuisé.
