# Livraison Android

Préférence explicite de Fab : terminer chaque livraison par un lien vers la page Release GitHub de la version publiée, qui regroupe APK, AAB et notes. Toujours indiquer le statut réel : ne pas annoncer une version prête avant la réussite de sa compilation et de sa publication.

La reconstruction 1.3.0 repart du commit publié 55b9a6a (1.2.1). Après récupération de l'environnement local, l'implémentation complète du commit local 91fce86 a été restaurée sur l'historique de rebuild/1.3.0, avec ses trois atlas ImageGen d'origine. Leur provenance et leurs empreintes figurent dans docs/campaign-1.3.0.md.

Les tests natifs ont été réexécutés sur les fichiers récupérés. Deux défauts supplémentaires ont été reproduits puis corrigés : dimensions des corps en paysage et minerais absents après destruction d'un astéroïde par collision. Les contrôles SDL/UBSan, six tests Python, source Android générée et rendus portrait/paysage passent localement. L'icône de lancement Android est un dessin vectoriel natif original.

Les deux ZIP ont ensuite été extraits sur main par GitHub Actions (run 35469747467, commit 9ab4179). Les 12 PNG qui n'existaient auparavant qu'en sources encodées sont maintenant intégrés à la branche 1.3.0, sans nouveau transfert d'images et sans modification du traitement Base64. Les ZIP sont conservés sur main pour que Fab les supprime lui-même. Le blocage du premier build 1.3.0 (run 35451931168) est corrigé dans le workflow : setup-android installe platform-tools sans demander le paquet tools introuvable.

La version 1.3.0 est publiée : https://github.com/greenpower2669/SpaceFortressVs/releases/tag/v1.3.0. Le run 35470197812 a validé les tests natifs/Python, compilé APK et AAB, vérifié la signature APK et l'intégrité des archives, puis publié les fichiers du commit 7ce2c949249cd087c047744ce12ed44f892f1319 avec leurs empreintes SHA-256. Le run de PR 35470200302 a également réussi. APK ARM64 : 84 321 052 octets ; AAB non signé : 81 851 081 octets. Les essais physiques sur téléphone restent à faire, comme indiqué dans todo.md.

Conserver les noms de fichiers SpaceFortressVs-VERSION.apk et SpaceFortressVs-VERSION-unsigned.aab. Conserver le source historique src/main.cpp intact. Consigner les travaux restant à faire dans todo.md et ne pas confondre simulations et essais physiques sur téléphone.
