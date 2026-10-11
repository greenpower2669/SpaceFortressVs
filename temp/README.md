# SpaceFortress — SOLO sur le vrai moteur COOP

Fichiers extraits (sans ZIP) depuis la CI VERTE :
https://github.com/greenpower2669/SpaceFortressVs/actions/runs/38171993509

Branche : feature/solo-coop-core-v1
Commit source testé : 82825ed5e2889ac630fe2379183bc068f0105d01

- SpaceFortressVs-1.4.2.apk : APK diagnostic installable, identité
  Android indépendante : com.greenpower2669.spacefortressvs.diagnostic.
- SpaceFortressVs-1.4.2-unsigned.aab : bundle NON SIGNÉ, ne pas
  tenter de l'installer comme une APK sur le téléphone.
- SpaceFortressVs-1.4.2-build.json : manifeste avec empreintes.
- SHA256SUMS : empreintes vérifiées via sha256sum -c.
- release-notes.md : notes du build source.

L'APK inclut le troisième bouton de diagnostic :
SOLO : VRAI MOTEUR COOP. Le menu CLASSIQUE superposé a été corrigé.
La charge sonore démarre à 0,20 seconde avec le moteur audio COOP,
puis READY à 2,00 s et un fond discret jusqu'au relâchement.
Le mode MONDE TEST active le vrai COOP dans le labyrinthe SOLO :
premier doigt = déplacement contre les parois, caméra concentrique
basée sur la vraie position, deuxième doigt = tir/cône 360.
L'arène témoin BOSS TEST reste disponible pour comparaison.
Le parcours reste EXPERIMENTAL : pas encore de progression,
boss final, collisions projectiles/parois, ni atlas définitifs.

Ne pas fusionner temp/ dans main ni publier de Release. Ces binaires
restent dans l'historique Git tant qu'ils ne sont pas purgés.
