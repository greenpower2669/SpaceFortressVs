# Livraison Android

Préférence explicite de Fab : terminer chaque livraison par un lien vers la page Release GitHub de la version publiée, qui regroupe APK, AAB et notes. Toujours indiquer le statut réel : ne pas annoncer une version prête avant la réussite de sa compilation et de sa publication.

La reconstruction 1.3.0 repart du commit publié 55b9a6a (1.2.1). Après récupération de l'environnement local, l'implémentation complète du commit local 91fce86 a été restaurée sur l'historique de rebuild/1.3.0, avec ses trois atlas ImageGen d'origine. Leur provenance et leurs empreintes figurent dans docs/campaign-1.3.0.md.

Les tests natifs ont été réexécutés sur les fichiers récupérés. Deux défauts supplémentaires ont été reproduits puis corrigés : dimensions des corps en paysage et minerais absents après destruction d'un astéroïde par collision. Les contrôles SDL/UBSan, six tests Python, source Android générée et rendus portrait/paysage passent localement. L'icône de lancement Android est un dessin vectoriel natif original.

Conserver les noms de fichiers SpaceFortressVs-VERSION.apk et SpaceFortressVs-VERSION-unsigned.aab. Conserver le source historique src/main.cpp intact. Consigner les travaux restant à faire dans todo.md et ne pas confondre simulations et essais physiques sur téléphone.
