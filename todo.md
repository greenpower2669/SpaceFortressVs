# todo.md — SpaceFortressVs

## Mission active — aide/tutoriel + danger 9

Branche : `feature/in-game-help-tutorial-danger-9`
Spec : `docs/superpowers/specs/2026-10-04-in-game-help-tutorial-danger-9-design.md`

### Design / méthode

- [x] Besoin joueur clarifié.
- [x] Architecture choisie : profil danger + centre d'aide + tutoriel sandbox séparés.
- [x] Design écrit et commité.
- [ ] Fab relit/valide la spec écrite.
- [ ] Écrire le plan d'implémentation détaillé.
- [ ] Exécuter en TDD.

### Danger 9 niveaux

- [ ] Étendre `boss_danger.hpp` à 9 profils.
- [ ] Défaut = `FIN DU MONDE` ×40.
- [ ] Ajouter cadence plafonnée jusqu'à ×2.50.
- [ ] Ajouter vitesse projectile plafonnée jusqu'à ×1.80.
- [ ] Ajouter paramètres d'anticipation/précision.
- [ ] Garder les coefficients invisibles sur HOME.
- [ ] Garder le sélecteur séparé du bouton lancement.

### IA classique

- [ ] Identifier précisément le point de création des tirs IA sans modifier `src/main.cpp` si une couche runtime peut porter le comportement.
- [ ] Calculer une interception à partir position/vitesse cible + vitesse projectile.
- [ ] Appliquer erreur/anticipation selon le niveau.
- [ ] Garantir vecteur projectile constant après création.
- [ ] Appliquer dégâts/cadence/vitesse uniquement à l'IA adverse.

### Aide `?`

- [ ] RAPIDE.
- [ ] DÉTAILLÉ.
- [ ] ANIMÉ avec assets existants + schémas SDL.
- [ ] Accès HOME.
- [ ] Accès en partie avec pause/reprise exacte.
- [ ] Consommation complète des événements tactiles d'aide.

### Tutoriel sandbox

- [ ] Déplacement.
- [ ] Tir.
- [ ] Énergie + poussière blanche.
- [ ] Coque/bouclier.
- [ ] Astéroïdes/cinétique.
- [ ] Surcharge deux doigts.
- [ ] Esquive/tirs IA.
- [ ] `TOUT FAIRE`.
- [ ] Prouver qu'aucune sauvegarde/stat/progression réelle ne change.

### Coop/campagne

- [ ] Étendre danger aux cadence/vitesse/anticipation des attaques non cinétiques applicables.
- [ ] Conserver exactement les chemins cinétiques hors danger.

### Tests / build

- [ ] RED ciblés danger 9.
- [ ] RED ciblés interception IA et absence de guidage.
- [ ] RED aide pause/reprise/inputs.
- [ ] RED sandbox tutoriel.
- [ ] GREEN ciblés.
- [ ] Régressions complètes 1.4.0.
- [ ] Build Android APK + AAB frais.
- [ ] Test téléphone Fab.

## Invariants permanents

- [ ] Ne pas appliquer le danger aux chemins cinétiques.
- [ ] Ne pas redonner de gameplay à la poussière rouge.
- [ ] Conserver `nrj=0` plein / `nrj=50` épuisé.
- [ ] Préserver `src/main.cpp` historique sauf preuve et approbation explicites.
- [ ] Pas de merge `main` ni release sans validation de Fab.
