# SpaceFortressVs 1.4.0 — relais Sol vérifié, essais téléphone en attente

## Référence et FAB Copilot
- [x] Sauvegarde Astra préservée : `9b8cae378f66d461dd8a29f9504972f648b2c40c`, arbre `92cf3b03b2aeaf30da6d57b85f126f89cf37caf4`.
- [x] Quatre mémoires FAB Copilot reconstruites/synchronisées sans prétendre qu'elles existaient chez Astra.
- [x] SHA de code testé par Sol : `2ca6aa46e732e9d9e86e9889ad2d215117c1f16e`.
- [x] Aucun ajout fonctionnel ni refonte ; aucun blocage frais n'a nécessité de correction.

## Vérification fraîche
- [x] `scripts/test-regressions.sh` exécuté par GitHub Actions sur le SHA testé.
- [x] 9 tests Python réussis.
- [x] 32 groupes natifs SDL/UBSan réussis.
- [x] 21 groupes du champ historique généré Android réussis.
- [x] Sélecteur : portrait/paysage, onglets et lancement de l'affrontement 200 réussis.
- [x] 200 rendus : niveau 1 historique, auras/reflets et 0/4/8/20 tentacules réussis.
- [x] 200 ouvertures de combat automatisées réussies.
- [x] 200 coups finaux / victoires durables / Hall après reload réussis.

## Android
- [x] Build Gradle APK/AAB réussi.
- [x] `SpaceFortressVs-1.4.0.apk` produit : 87372144 octets, SHA-256 `2045bc64083b624e14ecec034df350b03eaa0eeac269d145d171506d5b4f3b81`.
- [x] `SpaceFortressVs-1.4.0-unsigned.aab` produit : 84880594 octets, SHA-256 `a30870bece5759ddd4feb2ee4d1cf84a0e06822798afbea1e7624438d727939c`.
- [x] PNG, chemins, bibliothèque ARM64, ZIP et packaging vérifiés.
- [x] Artefact GitHub Actions : https://github.com/greenpower2669/SpaceFortressVs/actions/runs/36246639403/artifacts/10907503442

## Signature
- [x] Certificat APK 1.4.0 : `9817ba2bdf29226c72529ec161f8124f11cbee89b2748ca440b1e38148e01781`.
- [x] Certificat publié 1.3.1 : `8abfc11c8bc4f9ac065eb5c086ad4e457290bcbbc1105017865368de7e565868`.
- [x] Certificats différents : la 1.4.0 est un APK de test et n'est pas une mise à jour directe compatible.
- [x] Ne jamais demander de désinstaller l'installation existante ni d'effacer ses données.

## Essais téléphone réservés à Fab
- [ ] quatre doigts simultanés ;
- [ ] veille/reprise puis toucher ;
- [ ] champ historique vivant, collisions/minage/fragmentation ;
- [ ] énergie, missile, bonus et tourelles ;
- [ ] son et performances ;
- [ ] équilibre des quatre difficultés ;
- [ ] rendu réel sélection/combat.

## Fin de relais
- [x] Aucun merge dans `main`.
- [x] Aucune release publiée.
- [x] Workflow vérifié : https://github.com/greenpower2669/SpaceFortressVs/actions/runs/36246639403
- [ ] Publication d'une release uniquement après nouvel accord explicite de Fab.

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
