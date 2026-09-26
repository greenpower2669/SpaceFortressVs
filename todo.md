# SpaceFortressVs 1.4.x — lot correctif validé et en vérification

## Lot autorisé par Fab
- [x] D-140-01 : corriger l’initialisation W dans la copie Android générée sans modifier main.cpp.
- [x] D-140-02 : rendu missile historique distinct du plasma.
- [x] D-140-04 : HUD boss dédoublé, vert→rouge ; PV/énergie pilotes sur leurs côtés ; joueur haut à 180°.
- [x] D-140-03 : réduire la recharge de bouclier excessive via minerai.
- [x] D-140-06 : contact boss continu, énergie puis PV.
- [x] D-140-08 : cadence + précision initiale liées à l’énergie, sans auto-tir ni guidage ordinaire.
- [x] D-140-09 : impacts astéroïdes successifs non masqués par i-frame projectile, chaleur de surface historique.
- [x] D-140-05 : attribution séparée des tirs tourelles ; aucun buff PV boss arbitraire.
- [x] D-140-07 : préserver prédiction initiale et trajectoire rectiligne des tirs ordinaires.

## Vérifications à obtenir
- [x] CI complète fraîche sur `fbb1ff92907821217ff94d847d9f2bbf4683636d` : workflow 36266178070.
- [x] Régression rendu réel astéroïde classique : PASS.
- [x] Missile/HUD + recréation renderer : PASS.
- [x] Cadence/dispersion, tir refusé sans coût, trajectoire rectiligne : PASS.
- [x] Contact boss à 30/60/120 Hz : PASS.
- [x] Collision astéroïde et récupération minerai : PASS.
- [x] Build APK/AAB + packaging : PASS. Certificat debug `19e25032...` différent de la v1.3.1 publiée.
- [ ] Essai téléphone Fab après livraison : vérifier astéroïdes visibles, missile, HUD miroir et nouvel équilibre.

## Validation physique connue
- [x] Joueur bas : tir au tap RAS.
- [x] Joueur haut : contrôle/tir RAS.
- [ ] Quatre doigts réellement simultanés à confirmer à deux joueurs.

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
