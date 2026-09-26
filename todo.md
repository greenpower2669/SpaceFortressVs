# SpaceFortressVs 1.4.0 — relais Sol, vérification fraîche en cours

## Référence préservée
- [x] Branche distante récupérée : `fix/gameplay-campaign-200`.
- [x] Commit de sauvegarde Astra confirmé : `9b8cae378f66d461dd8a29f9504972f648b2c40c`.
- [x] Arbre Git confirmé par Sol : `92cf3b03b2aeaf30da6d57b85f126f89cf37caf4`.
- [x] Aucun développement recommencé de mémoire ; le travail récupéré est conservé.
- [x] Version préparée confirmée : 1.4.0 / code 10.

## FAB Copilot
- [x] Conserver intégralement l'historique de `todo.md`.
- [x] Reconstruire `brain.md`, `brainmap.md` et `debughistorical.md` à partir du code, du plan, des notes de release et des tests présents.
- [x] Distinguer explicitement les résultats rapportés par Astra des vérifications exécutées par Sol.
- [ ] Après le workflow frais, mettre à jour les quatre mémoires ensemble avec le SHA réellement testé, les résultats et les éventuels blocages.

## Vérification Sol
- [x] Vérifier la branche, le commit et l'arbre distants.
- [x] Vérifier par lecture du code : campagne 200, mapping 50×4, rendu partagé sélection/combat, 0/4/8/20 tentacules, commandes multitouch, énergie historique, bonus tourelles, champ partagé et migration v1/v2.
- [x] Vérifier que le sélecteur utilise des tailles de texte bornées par la hauteur et dispose de tests portrait/paysage et de lancement du combat 200.
- [ ] Exécuter fraîchement `scripts/test-regressions.sh` via le workflow Android sur le HEAD de reprise.
- [ ] Si un blocage est reproduit, corriger uniquement sa cause avec test de régression et synchroniser les quatre mémoires dans le même commit.

## Build Android
- [ ] Compiler `SpaceFortressVs-1.4.0.apk`.
- [ ] Compiler `SpaceFortressVs-1.4.0-unsigned.aab`.
- [ ] Vérifier PNG, chemins, bibliothèque ARM64, ZIP et empreintes.
- [ ] Récupérer les artefacts du workflow réellement réussi.

## Signature
- [ ] Comparer le certificat du nouvel APK au certificat publié v1.3.1 `8abfc11c8bc4f9ac065eb5c086ad4e457290bcbbc1105017865368de7e565868`.
- [ ] Si différent : marquer explicitement l'APK incompatible avec une mise à jour directe et ne demander aucune désinstallation/suppression de données.

## Validation téléphone réservée à Fab
- [ ] quatre doigts simultanés ;
- [ ] toucher après veille/reprise ;
- [ ] champ historique vivant avec collisions/minage/fragmentation ;
- [ ] énergie, missile, bonus et tourelles ;
- [ ] son et performances ;
- [ ] équilibre des quatre difficultés ;
- [ ] rendu réel sélection/combat.

## Interdictions de cette mission
- Aucun merge dans `main`.
- Aucune publication de release sans nouvel accord de Fab.
- Aucun ajout fonctionnel ni refonte.
- Préserver `src/main.cpp`, les sauvegardes, les 50 portraits, l'écran de fin stable, l'icône et les assets.
- Aucun transfert massif Base64.


## Historique précédent

# SpaceFortressVs 1.3.1 — lot correctif

- [x] Restaurer l'état complet des vaisseaux après la coop ; respiration
  discrète à taille nominale, sans croissance cumulative ni changement de collision.
- [x] Séparer tirs humains rectilignes et visée prédictive d'Orion en coop.
- [x] Protéger les sauvegardes inconnues avec secours valide, les changements
  après chargement et les octets corrompus ; écritures primaire/secours atomiques.
- [x] Corriger la largeur de visibilité des astéroïdes et le minage en paysage
  dans le source généré, sans modifier le main.cpp historique.
- [x] Observer le champ historique à 30/60/120 Hz : 20 graines × 5 minutes,
  deux orientations. Répartition équilibrée pour les trajectoires seules.
- [x] Intégrer les deux PNG originaux de main (74c94c1), sans transformation.
  Tourelle.png anime uniquement l'accueil ; le bonus est présent mais inactif.
- [x] Borner les textes de l'accueil aussi par la hauteur en paysage.
- [x] Vérifier les régressions natives/UBSan, les six tests Python et le
  source Android généré. Contrôler le rendu de l'accueil et ses textures.
- [x] Préparer version 1.3.1/code 9, noms APK/AAB, icône inchangée et vérification
  octet pour octet des nouveaux PNG dans les deux paquets.
- [x] Confirmer le build APK/AAB 1.3.1 et sa livraison GitHub : run 35503643572,
  commit dcf80af, release v1.3.1 publiée avec empreintes vérifiées.
- [x] Comparer les certificats APK : celui de la 1.3.1 publiée diffère de la 1.3.0.
  Une mise à jour directe sur l'installation existante est impossible avec cet APK.
- [ ] Retrouver la clé de signature de l'installation 1.3.0 et configurer une
  signature pérenne pour les mises à jour. Le workflow debug ne conserve pas sa
  clé. Ne pas désinstaller ni effacer les données Android pour contourner le refus.
- [ ] Essais téléphone : champ complet avec collisions/minage/fragmentation,
  animation, clavier, multitouch, son, reprise, performances, équilibre humain.
- [ ] Suite : 200 rencontres (50 identités × 4 difficultés), migration v2,
  mobilité/tactiques des boss et bonus temporaire. Valider l'ordre des difficultés.
- [ ] Bonus_de_tourelles.png est RGB avec damier intégré : convenir du détourage
  avant son activation en jeu ; original conservé.

Plan et preuves : docs/maintenance-1.3.1.md et docs/delivery.md. Release publiée :
https://github.com/greenpower2669/SpaceFortressVs/releases/tag/v1.3.1

## Historique 1.3.0

- [x] Récupérer la campagne complète et les trois atlas originaux.
- [x] Vérifier les cibles futures des IA/tourelles et la poursuite à 30/60/120 fps.
- [x] Vérifier 50 monstres animés, 50 planètes et six décors de nébuleuses.
- [x] Vérifier coop locale/IA, progression, noms et Hall of Fame persistant.
- [x] Corriger les dimensions en paysage et les minerais après collision.
- [x] Rejouer les régressions SDL/UBSan, les six tests Python et la compilation
  syntaxique du source Android généré ; contrôler les rendus portrait/paysage.
- [x] Ajouter une icône Android adaptative et conserver les noms APK/AAB versionnés.
- [x] Extraire les deux ZIP avec GitHub Actions sur main (commit 9ab4179), puis
  intégrer les 12 PNG manquants à la branche 1.3.0, sans modifier le traitement
  Base64. Les 21 PNG et les quatre ressources de l'icône ont été comparés.
- [x] Corriger le blocage SDK : demander platform-tools sans le paquet tools
  introuvable ; ne pas masquer un échec précoce par un journal de build absent.
- [x] Vérifier la compilation APK/AAB et la publication GitHub de v1.3.0.
  Build et publication réussis : run 35470197812, commit 7ce2c94.
  Les tests de régression natifs et Python, la signature APK et les empreintes
  des fichiers publiés ont été vérifiés. Le build de la PR a également réussi.
- [ ] Essai sur téléphone : toucher simultané, clavier, son, reprise Android,
  performances et équilibrage avec deux joueurs humains.

Livraison publiée : https://github.com/greenpower2669/SpaceFortressVs/releases/tag/v1.3.0
APK signé avec la clé de test Android ; AAB non signé. Le Hall of Fame reste
local au téléphone. Ne pas effacer les données de l'application pour le conserver.
