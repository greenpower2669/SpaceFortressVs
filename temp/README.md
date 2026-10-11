# SpaceFortress — SOLO sur le vrai moteur COOP

Fichiers extraits (sans ZIP) depuis la CI VERTE :
https://github.com/greenpower2669/SpaceFortressVs/actions/runs/38167735625

Branche : feature/solo-coop-core-v1
Commit source testé : b9072d24fd0b7452a88f8ec4cccb4b70e9cf51ae

- SpaceFortressVs-1.4.2.apk : APK diagnostic installable, identité
  Android indépendante : com.greenpower2669.spacefortressvs.diagnostic.
- SpaceFortressVs-1.4.2-unsigned.aab : bundle NON SIGNÉ, ne pas
  tenter de l'installer comme une APK sur le téléphone.
- SpaceFortressVs-1.4.2-build.json : manifeste avec empreintes.
- SHA256SUMS : empreintes vérifiées via sha256sum -c.
- release-notes.md : notes du build source.

L'APK inclut le troisième bouton de diagnostic :
SOLO : VRAI MOTEUR COOP. Le menu de sélection CLASSIQUE qui recouvrait
l'arène a été corrigé dans le commit b9072d24 (présentation SDL directe).
Encore une arène sans labyrinthe ni caméra.

Ne pas fusionner temp/ dans main ni publier de Release. Ces binaires
restent dans l'historique Git tant qu'ils ne sont pas purgés.
