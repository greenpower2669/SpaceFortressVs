# Audit de reprise — campagne 200 et identité visuelle (21-09-2026)

Base vérifiée : release v1.3.1, commit dcf80afb6eb117d405f46f03d8567d31a76b4b12. À cette date, aucun nouveau build GitHub Actions depuis le 20-09 ; les changements éventuellement locaux de Work ne sont pas visibles dans Git. Audit documentaire uniquement, aucun nouveau test exécuté.

## Fonctionnel / absent / régressé / incertain

- [x] Fonctionnel dans les tests v1.3.1 : 50 profils et sprites boss, 50 planètes/six fonds, respiration ±1 %, restauration coop→duel, tirs humains dans l'axe, protections des sauvegardes v1 et corrections partielles de visibilité/minage en duel.
- [ ] RÉGRESSÉ : coop tire automatiquement dans sfCoopMovePlayers ; reprendre une commande 1er doigt = déplacement, 2e tap = tir explicite, avec identifiants SDL_FingerID distincts, deux pilotes simultanés et aucun tir humain automatique. Orion conserve son tir autonome.
- [ ] RÉGRESSÉ : sfCoopResources crée un petit champ séparé plafonné à dix rochers, sans fragmentation historique ; réunifier autant que possible spawn/astéroïdes/collisions/minerai avec les procédures de Fab, sans transfert naïf d'objets entre boucles.
- [ ] INCERTAIN : champ duel perçu sur le bord gauche après collisions/fragmentation. Les simulations précédentes sont cinématiques uniquement ; reproduire les boucles complètes paysage/portrait, 30/60/120 Hz, plusieurs minutes.
- [ ] RÉGRESSÉ : sfCoopDefences tire en permanence. Activer temporairement via Bonus_de_tourelles.png attrapé, expiration/reprise, une collecte unique, énergie indépendante ; Tourelle.png uniquement à l'accueil.
- [ ] Vérifier l'énergie historique : nrj=0 plein, nrj=50 épuisé ; bouclier affaibli et missile à pleine énergie selon les règles de l'original. Tirs humains non guidés ; Orion et ennemis distincts.
- [ ] ABSENT : 50 boss × 4 difficultés (vif, endurant, vicieux, ultime) = 200 rencontres, mobilité et patterns différenciés. Séparer encounterId [0,199], bossIndex [0,49], difficulty [0,3] ; indexer tous les atlas exclusivement avec bossIndex.
- [ ] ABSENT : identité visuelle par difficulté sans 200 PNG : niveau 1 sans aura/tentacules ; niveaux 2/3/4 avec 4/8/20 tentacules animées, aura douce, reflets locaux. Même fonction de rendu/temps/paramètres en aperçu et combat, tentacules derrière le boss mais devant l'aura, étiquette textuelle de difficulté et réduction visuelle des vignettes sans changer leur identité.
- [ ] ABSENT : sauvegarde v2 et Hall/fin/progression jusqu'à 200. Migration v1 idempotente : préserver octets v1 et .bak, progrès, noms, scores et victoire en attente ; ne pas inventer de victoires aux nouvelles difficultés, protéger formats futurs et écritures interrompues.
- [ ] Vérifier interfaces portrait/paysage, doigts, marqueurs d'énergie, Jupiter, fonds, atlas, écran de fin stable, performances des 20 tentacules et reprises Android.
- [ ] BLOQUANT publication en tant que mise à jour : signature APK 1.3.1 incompatible avec 1.3.0 ; ne pas demander désinstallation/perte de Hall, retrouver la clé ou assurer un transfert explicite des données avant installation.
- [ ] Produire tests sur le code Android généré et la vraie boucle (pas seules trajectoires), SDL/UBSan, tests de migration/redémarrage, rendu identique sélection/combat, APK/AAB/PNG/chemins/signature, puis validation sur téléphone par Fab.
- [ ] Aucun transfert massif Base64. Petits lots avec résultats vérifiables ; documenter « code », « tests automatisés », « compilation » et « essai téléphone » séparément ; conserver src/main.cpp et les assets originaux.

## Ordre des lots, sans lancement automatique

- [ ] A : gestuelle/tirs/énergie coop et tests multitouch/IA.
- [ ] B : champ complet d'astéroïdes et minerai, diagnostics du duel.
- [ ] C : bonus temporaire et tourelles coop.
- [ ] D : modèle de rencontre 200 + sauvegarde v2/migration et parcours des écrans.
- [ ] E : moteur de rendu des 4 difficultés partagé sélection/combat, tests de lisibilité/performance.
- [ ] F : comportements et équilibrage progressifs des boss, tests intégrés, puis packaging/signature et essais téléphone.

---

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
