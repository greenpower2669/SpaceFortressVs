# SpaceFortressVs — ordres de mission vivants
> Optimisé le 19/10/2026, branche `feature/solo-coop-core-v1`. **Seul le texte actif ci-dessous régit la prochaine mission** ; anciens ordres et autorisations d'époque restent intégralement dans [archive intégrale du 19/10](docs/memory-archive/2026-10-19-pre-compaction/ordres-de-mission.md).

## IMMUTABLE / À PRÉSERVER

- **LIVRAISON APK OBLIGATOIRE :** après CI verte, faire **extraire l’APK de l’archive GitHub Actions par GitHub sur ses serveurs**, puis déposer le véritable fichier `.apk` directement téléchargeable dans `temp/<mission>/` **sur la branche de travail**, avec lien direct et SHA-256. **Jamais de ZIP à télécharger/décompresser sur l’appareil ou le disque de Fab.**
- Identité release/store historique : `com.greenpower2669.spacefortressvs` ; ne pas renommer pour une mise à jour.
- Diagnostic existant : suffixe `.diagnostic`, soit `com.greenpower2669.spacefortressvs.diagnostic`.
- APK diagnostic debug ; AAB release livré NON SIGNÉ. Configuration de signature inchangée ; aucune clé d’upload ajoutée au dépôt. Ne jamais copier de clé privée dans le dépôt ou les logs.
- `src/main.cpp` protégé. Aucun merge main ni Release sans validation explicite Fab.
- Livrer APK identifiable sous temp avec source CI verte exacte et SHA256 ; préserver les diagnostics précédents.



## État de travail / autorisations
- Branche de développement SOLO : `feature/solo-coop-core-v1`. `main` publié reste `v1.4.2`; ni merge, ni publication de Release sans validation explicite de Fab.
- Dernière livraison diagnostique connue : **SOLO-034**, commit source `11fd5a7640704b7886a4647798c87c4c28ed4029` ; CI `39215282189` verte (C++, SDL2, Android), export direct `39215282005`. APK : `temp/solo-034/SpaceFortressVs-1.4.2-SOLO-034-11fd5a7.apk`, SHA-256 `9ea8aa7046c3e90b6706b38b3969e8b00db5befbd1ab484bc06e9f77971b6c58`. AAB non signé, distinct. 0/4 nouveaux WAV privés Meteor dans cette version publique : ancien son de secours.
- Le **pilotage actuel SOLO-033, conservé dans SOLO-034, a reçu un retour global positif de Fab le 19/10**. Ne pas le redéfinir ou retoucher pour une mission strictement graphique. Ce retour ne remplace pas des mesures physiques de vitesse ni une validation exhaustive.
- **Demande esthétique suivante : audit/idéation uniquement, pas de GO de codage**. Les masses rocheuses extérieures doivent être plus pleines, avec cratères variés. Sur la traverse rocheuse plate du bas, la transition doit être organique, sans frontière perceptible et **avec exactement le même langage de matière/texture que les roches internes du labyrinthe** : teinte, grain, relief et lumière harmonisés, sans répétition évidente.
- Pour cette mission documentaire d'optimisation : ne modifier **aucun code, asset, compilation, APK/AAB, collision ou comportement**. Une demande ultérieure de GO sera nécessaire pour fabriquer des textures ou modifier leur rendu.
- Protéger la collision/tuiles `Rock/Empty`, l'épaisseur utile des couloirs, la caméra, le fond, les sauvegardes, Hall of Fame, campagne 200, difficultés, sons, COOP/DUEL, zoom, HUD et commande du vaisseau.

## Contrat technique actuellement accepté, à préserver
- Vaisseau SOLO au doigt sur coque ou via mini-stick violet ; tir autonome par pad turquoise, inversion G/D des pads, inertie physique conservée.
- SOLO-033 : commande directe de vitesse **30 %** au rayon maximum ; réserve vectorielle du maintien stable jusqu'à **+100 points** dans la couronne externe, seuil ~90 %, plein effet vers ~98 % ; plafond final **130 %** de l'ancienne vitesse maximale, physique bornée. Virages, demi-tours, retour centre et relâchement restent maîtrisables. Caméra suivie du déplacement physique (et non du curseur). Ne pas faire revenir la vitesse immédiate à 100 %.
- SOLO-034 : épaississement **visuel** de rebord dans `src/solo_organic_rims.hpp::exteriorRelief`, sans extension de la silhouette alpha/collisions, ni remplacement de `ParoisModeSolo.png`. Nouveaux cratères non autorisés à coder pour le moment.

## Discipline FabCopilot
- Avant un **codage autorisé** : relire les 5 fichiers actifs en commençant par `brainmap.md`; consulter seulement les archives pertinentes si une décision historique ou régression l'exige.
- En cas de modification du moteur : revue des invariants, tests C++/SDL2/Android sur le même commit exact, preuves CI, mises à jour des mémoires, APK **.apk direct sur la branche** après extraction serveur et SHA256 ; ne pas confondre AAB non signé et APK installable.
- Aucune manipulation `main`, Release ou fusion, aucun reset/force-push ni suppression de diagnostics sans accord explicite.
- Historique complet des anciennes missions (y compris leurs GO **limités à leur périmètre**) : [archive intégrale du 19/10](docs/memory-archive/2026-10-19-pre-compaction/ordres-de-mission.md). Voir [archive intégrale du 19/10](docs/memory-archive/2026-10-19-pre-compaction/README.md) pour la méthode de recherche et de restauration.
