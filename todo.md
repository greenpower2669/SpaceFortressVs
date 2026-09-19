# SpaceFortressVs 1.3.0

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
- [ ] Vérifier la compilation APK/AAB et la publication GitHub de v1.3.0.
- [ ] Essai sur téléphone : toucher simultané, clavier, son, reprise Android,
  performances et équilibrage avec deux joueurs humains.

Livraison attendue : https://github.com/greenpower2669/SpaceFortressVs/releases/tag/v1.3.0
APK signé avec la clé de test Android ; AAB non signé. Le Hall of Fame reste
local au téléphone. Ne pas effacer les données de l'application pour le conserver.
