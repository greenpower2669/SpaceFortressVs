# Livraison Android

Préférence explicite de Fab : terminer chaque livraison par un lien vers la page Release GitHub de la version publiée, qui regroupe APK, AAB et notes. Toujours indiquer le statut réel : ne pas annoncer une version prête avant la réussite de sa compilation et de sa publication.

La reconstruction 1.3.0 repart du commit publié 55b9a6a (1.2.1). Après récupération de l'environnement local, l'implémentation complète du commit local 91fce86 a été restaurée sur l'historique de rebuild/1.3.0, avec ses trois atlas ImageGen d'origine. Leur provenance et leurs empreintes figurent dans docs/campaign-1.3.0.md.

Les tests natifs ont été réexécutés sur les fichiers récupérés. Deux défauts supplémentaires ont été reproduits puis corrigés : dimensions des corps en paysage et minerais absents après destruction d'un astéroïde par collision. Les contrôles SDL/UBSan, six tests Python, source Android générée et rendus portrait/paysage passent localement. L'icône de lancement Android est un dessin vectoriel natif original.

Les deux ZIP ont ensuite été extraits sur main par GitHub Actions (run 35469747467, commit 9ab4179). Les 12 PNG qui n'existaient auparavant qu'en sources encodées sont maintenant intégrés à la branche 1.3.0, sans nouveau transfert d'images et sans modification du traitement Base64. Les ZIP sont conservés sur main pour que Fab les supprime lui-même. Le blocage du premier build 1.3.0 (run 35451931168) est corrigé dans le workflow : setup-android installe platform-tools sans demander le paquet tools introuvable.

La version 1.3.0 est publiée : https://github.com/greenpower2669/SpaceFortressVs/releases/tag/v1.3.0. Le run 35470197812 a validé les tests natifs/Python, compilé APK et AAB, vérifié la signature APK et l'intégrité des archives, puis publié les fichiers du commit 7ce2c949249cd087c047744ce12ed44f892f1319 avec leurs empreintes SHA-256. Le run de PR 35470200302 a également réussi. APK ARM64 : 84 321 052 octets ; AAB non signé : 81 851 081 octets. Les essais physiques sur téléphone restent à faire, comme indiqué dans todo.md.

Conserver les noms de fichiers SpaceFortressVs-VERSION.apk et SpaceFortressVs-VERSION-unsigned.aab. Conserver le source historique src/main.cpp intact. Consigner les travaux restant à faire dans todo.md et ne pas confondre simulations et essais physiques sur téléphone.

## Correctifs 1.3.1

Les correctifs sont intégrés par la PR #2, commit de fusion
`dcf80afb6eb117d405f46f03d8567d31a76b4b12`. L'arbre Git
`663d53d76663d4d27eb14d49a76df0b6bfebf733` correspond exactement aux fichiers
vérifiés localement. Le run de PR 35491186804 a réussi les tests natifs/UBSan,
Python, la compilation Android APK/AAB et les contrôles de packaging.

La respiration des vaisseaux, le retour coop/duel, les tirs humains en coop,
les sauvegardes, la visibilité du champ historique et le minage en paysage
sont corrigés. Les deux PNG fournis sur main sont conservés octet pour octet :
Tourelle.png anime seulement l'accueil, Bonus_de_tourelles.png reste destiné
au bonus futur. Les tourelles en combat conservent leur rendu et leurs règles.
La campagne reste à cinquante boss ; les deux cents rencontres sont à suivre.

La comparaison des certificats révèle un problème de continuité Android :
la 1.3.0 utilise `822915992d833e32f555fd4332ff56bba0c5319bc0a359e1925811fe12a86c2b`,
l'APK de PR 1.3.1 utilise `c793e6a71e3923370a63ca193c821111169f5d90cd533d43b184b55bec3fc487`.
Le workflow crée une clé debug non persistée ; cet APK ne peut donc pas mettre
à jour directement la 1.3.0. La clé de signature de la version installée est
nécessaire pour une mise à jour compatible. Ne pas désinstaller le jeu ni
effacer les données Android : le Hall of Fame est local.

La release 1.3.1 est publiée :
https://github.com/greenpower2669/SpaceFortressVs/releases/tag/v1.3.1.
Le run 35503643572 a compilé et vérifié le commit de fusion dcf80af, puis
publié les fichiers. APK : 87 355 768 octets, SHA-256
`7daf3d34bc2aac45c91743ffcfa5f8d57b9509fac52bd3cc21c05a6c24fa4d99`.
AAB non signé : 84 864 999 octets, SHA-256
`14c86f20ddead8094a0294923d035f599d54ee1f86a33eeb8d34e80cc1773466`.
Les originaux Tourelle.png et Bonus_de_tourelles.png ont été comparés octet
pour octet dans les deux paquets. Noms versionnés et icône existante conservés.

Le certificat de l'APK **publié**, distinct de celui du build de PR, est
`8abfc11c8bc4f9ac065eb5c086ad4e457290bcbbc1105017865368de7e565868`.
Il diffère lui aussi de la 1.3.0 : la mise à jour directe reste impossible
avec cette signature. Les fichiers sont disponibles pour essais, mais une
installation existante doit rester intacte tant que la continuité de signature
ou une solution de transfert explicite de ses sauvegardes n'est pas établie.
Les tests sur téléphone et la campagne 200 restent listés dans todo.md.
