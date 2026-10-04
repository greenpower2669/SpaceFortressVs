# debughistorical.md — SpaceFortressVs

## Résumé des causes prouvées — état livré 1.4.0

Archive complète : `docs/archive/2026-10-spacefortress-v1.4.0-history.md`.

### Astéroïdes invisibles Android
- Cause : confusion `std::setw` / `fablib::setw(W)` dans la source Android générée ; largeur historique nulle, rendu rejeté par `inxy()`.
- Correctif : qualification dans la copie Android générée uniquement.
- `src/main.cpp` reste inchangé, blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.

### Énergie
- Convention historique : `nrj=0` plein, `nrj=50` épuisé.
- Toute lecture future doit préserver ce sens.

### Cinétique v3
- Ancien problème : plancher artificiel + dépendance quadratique de vitesse.
- Canon : `250 × masseRelative × vitesseRelativeNormalisée × fermeture`.
- Masse/vitesse bornées, aucun plancher.
- Vitesse nulle = aucun dégât cinétique inventé.
- Danger boss exclu des chemins cinétiques.

### Dernier faux rouge historique
- `legacy_field_regressions` simulait un astéroïde immobile posé sur la coque et exigeait des dégâts.
- Test corrigé en impact réellement entrant et passage par `sfCoopAsteroidHurt`.
- La suite complète est ensuite devenue GREEN.

### Poussières
- Rouge : ancienne chaleur/recharge supprimée ; visuelle uniquement.
- Blanche : recharge jusqu'à `nrj=0`, puis soin ; non déviée.

### Surcharge
- Charge 2,00 s.
- Pendant charge : champ ×2 + irisation + son.
- Après 2 s tant que le doigt reste posé : bouclier cinétique OFF, vulnérabilité volontaire.
- Relâchement prêt : purge ; relâchement précoce : pas de purge complète.

### HOME danger boss
- Affichage par noms : `MOU DU GENOU`, `CHILL`, `ROCK N ROLL`, `DUR A CUIRE`, `MACHINE DE GUERRE`.
- Défaut `ROCK N ROLL` = ×10 interne.
- Sélecteur et lancement sont tactiquement séparés.

### Livraison / merge
- Release `v1.4.0` publiée depuis `98da8a01175d5091f487232f65872e92d213b321`.
- Merge à deux parents : `c50088092744d18ce50f3ef484a2d706aa76a276`.
- `main` revalidé sur `a30ca6ed4b4679ad9bd69ba8a145f602e9e3cb8b`, workflow `37181486670` GREEN complet.
- Fab confirme l'APK OK sur téléphone le 4/10/2026.

### Signature Android
- APK 1.4.0 publié : certificat `22943f8846ebaf3191d011b1d947883d66ff6f25e566c3a966b98f879f070172`.
- v1.3.1 : `8abfc11c8bc4f9ac065eb5c086ad4e457290bcbbc1105017865368de7e565868`.
- Différents : APK CI de test ; ne jamais conseiller de supprimer l'installation ou les données pour contourner cette incompatibilité.
