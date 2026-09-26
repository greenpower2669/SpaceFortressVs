# Restauration 1.4.0 — corrections locales vérifiées, livraison en attente

- Inventaire et décisions : docs/superpowers/plans/2026-09-20-gameplay-200.md.
- Codé : taps humains, missiles pleine réserve, bouclier dépendant de nrj,
  bonus flottant/expiration/réserves de tourelles, champ historique partagé,
  200 rencontres en quatre tours, format v2 avec archive v1, apparence
  procédurale commune sélection/combat (auras, reflets, 0/4/8/20 tentacules).
- Bases : v1.3.1 dcf80afb6eb117d405f46f03d8567d31a76b4b12 ; historique
  7ce2c949249cd087c047744ce12ed44f892f1319 ; branche fix/gameplay-campaign-200.
- Vérifié : 32 groupes natifs SDL/UBSan, 21 groupes du vrai champ Android
  généré, fixture v1/v2 autonome, neuf tests Python et syntaxe Android.
  Cinq combats complets simulés, 200 ouvertures et 200 victoires par coup final
  enregistré/rechargé ; 200 rendus ; migration durable avec victoire en attente.
- Vérifié visuellement : portraits conservés, mêmes auras/reflets/tentacules
  en sélection/combat, absence d’aura au niveau 1, textes portrait/paysage.
- Préparé : 1.4.0/code 10, noms APK/AAB, icône conservée et comparaison
  obligatoire du certificat réel avec la 1.3.1 dans le packaging.
- [ ] Autoriser explicitement le push vers le dépôt GitHub : le contrôle
  automatique a rejeté l’envoi externe pour autorisation jugée insuffisante.
  Aucun code de cette correction n’a été envoyé à ce stade.
- [ ] Lancer et vérifier compilation Android APK/AAB, PNG et signature réels.
  Les tests natifs et la vérification syntaxique ne remplacent pas ce build.
- [ ] Retrouver la clé privée correspondant à la 1.3.1 installée pour permettre
  une mise à jour ; le certificat seul ne suffit pas. Ne pas désinstaller ni
  effacer les données pour contourner une signature différente.
- [ ] Publier seulement les fichiers issus du build vérifié.
- [ ] Fab : essais sur téléphone (quatre doigts, veille/reprise, champ vivant,
  énergie, bonus/tourelles, son, performance et équilibre des quatre niveaux).
- Dispositions portrait/paysage testées nativement ; verrouillage Android
  portrait conservé. La rotation dynamique n’est pas validée.
- Aucun gameplay validé sur téléphone et aucun nouvel APK déclaré prêt.

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
