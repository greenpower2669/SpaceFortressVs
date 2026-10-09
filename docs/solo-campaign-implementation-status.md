# SOLO / Académie — démarrage d'implémentation (9 octobre 2026)

Branche : `feature/solo-worlds-tutorials-v1` issue de CI391. Ne pas merger sur main ni publier de Release sans Fab.

## Décisions de Fab
- Défilement hybride, inertie spatiale amortie, commandes et réparations identiques aux autres modes.
- Campagne extensible par mondes, 20 cartes par monde ; terminer le monde précédent débloque le suivant.
- Départ bas, arrivée haut, vivant ; score selon principes COOP, nombre d'ennemis, temps et dégâts cumulés.
- Astéroïdes de recharge après les vagues ; en APOCALYPSE moins de ressources, ennemis plus robustes, davantage de points.
- Vrai boss de type COOP à vaincre à la fin de chaque carte, pas un mini-boss.
- Cartes RGB 9:16, versions immuables, minimap et futur éditeur tactile.
- Académie : 6 séquences pédagogiques regroupant 8 mini-jeux déjà spécifiés.

## Livré dans cette première tranche
- `src/solo_campaign_model.hpp` : modèle de carte RGB et palette, validation de départ/arrivée, inertie amortie, état pilote, calcul de score expérimental et déblocage de mondes.
- `tests/solo_campaign_model_regressions.cpp` : tests natifs autonomes.

## IMPORTANT : non encore livré
- Aucun écran SOLO, aucune carte jouable, aucun boss ni vague intégrés au moteur.
- Aucune intégration du score COOP réel : les points combat sont pour l'instant une **entrée externe**, la formule est un prototype à remplacer après audit du score COOP canonique. Multiplicateurs et ratio 60/40 non validés par Fab.
- Aucun mini-jeu interactif nouveau n'est encore livré. Le tutoriel existant reste inchangé.
- Pas de compilation Android validée pour cette branche.

## Ordre de travail
1. Audit du score COOP et des contrôles réels, puis remplacement du prototype par une adaptation fidèle.
2. Tests natifs et intégration du modèle de carte au runtime SOLO sans toucher au duel/COOP.
3. Carte pilote avec rendu, collisions, HUD et minimap.
4. Vagues, astéroïdes de recharge, boss final COOP, victoire, progression/sauvegarde.
5. Académie interactive 6 séquences / 8 mini-jeux, sandbox isolée.
6. Éditeur, validations dynamiques et variantes immuables de difficulté.
7. CI Android, APK test Fab, puis décision de merge/release.

## Tranche suivante — prototype graphique et pilotage
- `src/solo_renderer.hpp` : rendu SDL des pixels de carte, marqueur du vaisseau, coque et minimap.
- `src/solo_session.hpp` : déplacement, caméra, collisions, dégâts cumulés, réparation et verrou du boss.
- `src/solo_prototype.cpp` : prototype SDL autonome pilotable au doigt ou au clavier (flèches/WASD, R pour recommencer, Échap pour quitter). Le prototype est compilé en CI mais **n'est pas encore lancé depuis l'APK principal**.
- `tests/solo_renderer_regressions.cpp` et `tests/solo_session_regressions.cpp` ; compilation du prototype via `scripts/test-regressions.sh`.
- Une erreur de littéraux `\\n` dans le header de score a été identifiée dans les logs CI et corrigée.

**Limites fonctionnelles** : pas encore de vrais tirs, de vagues actives, de boss COOP, de sauvegarde de campagne ni d'intégration au menu. L'entrée tactile ne remplace pas les contrôles canoniques : elle est réservée au prototype autonome. La formule de score reste expérimentale.
