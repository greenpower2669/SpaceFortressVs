# SpaceFortress — SOLO TURRETS OFF-011 (DIAGNOSTIC)

Nouveau build extrait UNIQUEMENT après CI complète verte :
https://github.com/greenpower2669/SpaceFortressVs/actions/runs/38209069899

Source compilée et testée : ad5cb08e945bf919e10abe77131a4c873ccadea6
Branche : feature/solo-coop-core-v1
APK Android diagnostic : SpaceFortressVs-1.4.2.apk
AAB NON SIGNÉ : SpaceFortressVs-1.4.2-unsigned.aab
Empreintes : SHA256SUMS (contrôlées après extraction)
Identité Android : com.greenpower2669.spacefortressvs.diagnostic

RETOUR SAMSUNG : les vagues de météorites ajoutées dans METEOR-009
restent coincées dans des parois. NE PAS déclarer le placement ou
leur trajectoire corrigés/validés sur téléphone.
STABILISATION : vagues supplémentaires entièrement SUSPENDUES.
Les astéroïdes historiques, le minage/dust/recharge, le vrai pilote,
caméra, arme, bouclier et sons charge 0,20 s / READY 2 s sont préservés.
Le CLASSIQUE et COOP historiques ne changent pas.
Audit: point libre à la naissance != trajet libéré; la physique
des rochers historiques n'applique pas les parois ROCK du labyrinthe.
Des rochers historiques peuvent encore être affectés: investigation
distincte encore ouverte, contrôle physique Samsung requis.
BOSS TEST/OFF : mannequin immobile uniquement, PAS rencontre finale.
SOLO-011 : le diagnostic entier (ARENE et MONDE) n'active JAMAIS
le bonus ni les tourelles défensives. Correction du chemin ARENE
boss=ON : garde soloDiagnostic, purge des tirs tourelle kind=5,
inhibition du rendu tactique historique. L'activation du boss en
MONDE TEST ne doit pas réintroduire d'entités.
Vérifications automatiques avec/sans boss, nouveau démarrage,
CLASSIQUE/COOP conservés. Confirmation physique Samsung attendue.
Le HUD SOLO-011 identifie cette édition malgré version 1.4.2.

Les anciens fichiers temp/SpaceFortressVs-1.4.2.apk et
temp/SpaceFortressVs-1.4.2-unsigned.aab et
temp/solo-world-cleanup-008/ et
temp/solo-world-meteor-009/ et
temp/solo-world-stabilization-010/ sont CONSERVÉS.
Aucune fusion main ni Release publique. Validation Samsung requise.
