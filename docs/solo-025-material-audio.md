# SOLO-025 — Variation de roche et explosions roche contre roche

L'arrière-plan et la silhouette organique SOLO-024 restent identiques. Le matériau issu de `ParoisModeSolo.png` est désormais déformé **en coordonnées monde**, avec mélange d'échantillons orientés différemment, zones de luminosité lentes, accents bleus beaucoup moins réguliers et aucun reset d'échantillonnage au bord des cases ou des chunks.

## Quatre WAV privés retrouvés dans Djipie

Source exacte : `greenpower2669/Djipie`, Release **djipie-audio-prive-fx-explosions**. Ils sont indépendants de l'index de 447 membres classés.

| Fichier | Taille | SHA-256 |
| --- | ---: | --- |
| Meteor_explosion_rock_contre_rock1_001.wav | 321092 | 98d08994f89f1f246158b74f4b1d0dc5d9edce79134d6d1a109bef3193b895d0 |
| Meteor_explosion_rock_contre_rock2_002.wav | 299924 | 2e4bec14e47c1cdaf657ce1b0d9a2a0faab99f89e34d2fc6d688243a7111bab4 |
| Meteor_explosion_rock_contre_rock3_003.wav | 317564 | b69ce378105355f2357e29b16f86aa2b7b776bf2bab9202e142401aca058aab3 |
| Meteor_explosion_rock_contre_rock4_004.wav | 306980 | aa68017c13662e71666473e243151463042d44d78d7fcdb11772524bd00b4de4 |

### Chaîne de livraison sûre

**Ne JAMAIS ajouter les WAV dans les commits publics**. Les assets appartiennent à une Release privée et exigent une vérification des droits avant diffusion publique du jeu. Le build GitHub Actions peut les lire uniquement par le secret **`DJIPIE_READ_TOKEN`**, jeton lecture seule couvrant `greenpower2669/Djipie`. Il les télécharge dans **l'espace temporaire du runner** avec `gh release download`, contrôle les quatre SHA-256, les fichiers WAV et tailles, puis les copie dans l'APK via `scripts/prepare-assets.py` / Gradle. Le contrôle final inspecte le contenu du ZIP APK et valide les SHA-256, sans exporter les WAV indépendamment. Aucun secret dans les logs.

Sans secret, build et tests fonctionnent avec les anciens `explo1/explo2.ogg`. **Un APK compilé sans ce jeton n'a pas les nouveaux sons** et ne doit pas être annoncé comme « nouvelles explosions intégrées ». Le journal GitHub affiche `DJIPIE_METEOR_TAKES` égal à 0 ou 4. Rien n'est téléchargé depuis Internet par le téléphone.

### Déclenchements

- Contacts véritables **roche contre roche** : quatre WAV en ordre mélangé sans deux répétitions consécutives, canal dédié 9.
- Destruction/explosion d'astéroïdes : mêmes quatre nouvelles prises, mélange sur canal dédié 8.
- Impacts contre **mur / champ de force / coque** : identité précédente conservée.
- Variations de volume discrètes, pas de changement de cadence, collision, santé, physique ou difficulté.
- Si aucun WAV autorisé ne peut être chargé, retour aux sons historiques ; aucune dépendance réseau runtime.

Référence de retour arrière et tests : voir `ordres-de-mission.md` et `todo.md`.
