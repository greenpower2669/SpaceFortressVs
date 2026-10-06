# todo.md — SpaceFortressVs

## Livré
- [x] Hall global v1 validé téléphone et publié dans v1.4.1.
- [x] Surcharge cinétique v2 : 0,30 s / 2,00 s / blast 3,0× / EMP.
- [x] Release publique `v1.4.1` vérifiée.

## Mission active — v1.4.2 fatigue cinétique + duel classique
- [x] Créer branche `feature/kinetic-energy-fatigue-duel-v142` depuis `main` `21b1ff3592ce6f531da58fad6c102049a313325d`.
- [x] Ouvrir PR #6 en draft.
- [x] TDD RED : workflow `37517885303` échoue comme prévu avant le câblage duel owner 0.
- [x] Ajouter courbe d’efficacité `pow(energyFraction,1.20)`.
- [x] Ajouter durée de vague visuelle ~0,27 s pleine → ~0,50 s vide.
- [x] Ajouter warning visuel équipe→orange→rouge, flash rouge <10 %.
- [x] Ajouter bridge CLASSIQUE DUEL local second doigt pour owner 0 et owner 1, sans toucher `src/main.cpp`.
- [x] Garder DUEL IA et COOP sur leurs chemins existants.
- [x] Tests de courbe/durée/owners/overlay ajoutés.
- [ ] Obtenir une CI complète GREEN sur le HEAD code + docs.
- [ ] Corriger uniquement les défauts démontrés par tests/CI.
- [ ] Vérifier `src/main.cpp` blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- [ ] Préparer `VERSION_NAME=1.4.2`, `VERSION_CODE=12` et notes Release.
- [ ] Relancer CI fraîche GREEN de préparation release.
- [ ] Review finale PR #6 / secret hygiene / aucun changement Hall.
- [ ] Merge PR #6 vers `main` après GREEN.
- [ ] Publier `v1.4.2` avec APK/AAB/SHA256SUMS et vérifier la Release publique.

## Invariants
- [x] `src/main.cpp` non modifié à ce stade.
- [x] Hall, Danger 9, campagne 200, progression et poussières hors périmètre.
- [x] Fab a autorisé une Release directe de ce lot une fois entièrement GREEN.


## Fermeture v1.4.2
- [x] CI 335 a détecté le cleanup duel / capture énergie manquants.
- [x] Correctif minimal codé + tests renforcés.
- [ ] Nouvelle CI Android complète GREEN.
- [ ] Vérifier `src/main.cpp` au blob canonique.
- [ ] Merge PR #6 puis publication v1.4.2 conformément à l'ordre Fab une fois GREEN.
