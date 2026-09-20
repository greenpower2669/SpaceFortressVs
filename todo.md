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
