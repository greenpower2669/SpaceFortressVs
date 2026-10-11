# SpaceFortress — SOLO sur le vrai moteur COOP

Fichiers extraits (sans ZIP) depuis la CI VERTE :
https://github.com/greenpower2669/SpaceFortressVs/actions/runs/38169721589

Branche : feature/solo-coop-core-v1
Commit source testé : 9927f0148486eefb5f49a201b9fd0602151c1905

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
Le pont de phase 2 (carte, collisions, projection, caméra circulaire)
est compilé et testé mais PAS encore activé dans l'arène jouable.

Ne pas fusionner temp/ dans main ni publier de Release. Ces binaires
restent dans l'historique Git tant qu'ils ne sont pas purgés.
