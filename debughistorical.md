# debughistorical.md — SpaceFortressVs

## Résumé des causes prouvées — base livrée 1.4.0

Archive complète : `docs/archive/2026-10-spacefortress-v1.4.0-history.md`.

### Astéroïdes invisibles Android
- Cause : confusion `std::setw` / `fablib::setw(W)` dans la source Android générée ; largeur historique nulle, rendu rejeté par `inxy()`.
- Correctif : qualification dans la copie Android générée uniquement.
- `src/main.cpp` reste inchangé, blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.

### Énergie
- Convention historique : `nrj=0` plein, `nrj=50` épuisé.

### Cinétique v3
- Canon : `250 × masseRelative × vitesseRelativeNormalisée × fermeture`.
- Masse/vitesse bornées, aucun plancher.
- Vitesse nulle = aucun dégât cinétique inventé.
- Danger exclu des chemins cinétiques.

### Poussières
- Rouge : visuelle uniquement.
- Blanche : recharge jusqu'à `nrj=0`, puis soin ; non déviée.

### Surcharge
- Charge 2,00 s.
- Pendant charge : champ ×2 + irisation + son.
- Après 2 s tant que le doigt reste posé : bouclier cinétique OFF.
- Relâchement prêt : purge ; relâchement précoce : pas de purge complète.

### Livraison / merge
- Release `v1.4.0` publiée depuis `98da8a01175d5091f487232f65872e92d213b321`.
- Merge à deux parents : `c50088092744d18ce50f3ef484a2d706aa76a276`.
- `main` revalidé sur `a30ca6ed4b4679ad9bd69ba8a145f602e9e3cb8b`, workflow `37181486670` GREEN complet.
- Fab confirme l'APK OK sur téléphone le 4/10/2026.

## Mission active — points à ne pas perdre pendant l'implémentation

Branche : `feature/in-game-help-tutorial-danger-9`.
Spec : `docs/superpowers/specs/2026-10-04-in-game-help-tutorial-danger-9-design.md`.

- Le danger historique 5 niveaux est remplacé par 9 niveaux ; défaut futur `FIN DU MONDE` ×40.
- En classique, l'IA doit anticiper la trajectoire du joueur **avant le tir**, puis le projectile ordinaire doit rester droit avec un vecteur vitesse constant.
- Ne jamais convertir l'amélioration d'anticipation en guidage après lancement.
- Cadence et vitesse augmentent avec le danger mais restent plafonnées (×2.50 et ×1.80).
- Le danger ne doit jamais contaminer les astéroïdes ou charges cinétiques.
- Le centre d'aide ouvert pendant une partie doit geler la simulation et consommer ses événements tactiles.
- Le tutoriel doit avoir un état sandbox séparé : aucune statistique, sauvegarde, progression ou entité de vraie partie ne doit être mutée.
- Les illustrations animées réutilisent les assets existants et des primitives SDL ; pas de nouvelles vidéos requises pour cette version.

Ce bloc est une garde de régression pour le nouveau lot, pas un journal de bugs déjà observés.
