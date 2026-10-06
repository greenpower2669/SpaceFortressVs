# brain.md — SpaceFortressVs

## État canonique vivant
- Dernière Release publique : `v1.4.1`, commit/tag `450423c41c4cef6c348f49af698767016a0528fd`.
- `main` de départ mission v1.4.2 : `21b1ff3592ce6f531da58fad6c102049a313325d`.
- Branche active : `feature/kinetic-energy-fatigue-duel-v142`.
- PR active : #6 (draft pendant développement).
- `src/main.cpp` historique reste strictement protégé, blob `835059a0ecfe0f74708068b3259cad5db1cdb579`.

## Hall global — livré / hors périmètre
- Hall global v1 fonctionne et a été validé téléphone : `SYNC OK`, `GLOBAL 1 + LOCAL 0`.
- Secret jeu injecté au build via `SPACEFORTRESS_HOF_API_KEY`; jamais dans Git/logs/mémoires.
- Aucun changement Hall autorisé dans le lot v1.4.2.

## Surcharge cinétique v2 — base livrée v1.4.1
- <0,30 s : comportement classique, sans cercle irisé.
- 0,30–2,00 s : surcharge visible ×2.
- À 2 s : armé, champ cinétique OFF jusqu’au relâchement.
- Relâchement armé : blast/purge 3,0× + son EMP.

## Mission active — fatigue énergie + duel classique — cible v1.4.2
- Fab demande une absorption cinétique plus faible quand la réserve baisse : `effectiveEnergy = pow(energyFraction, 1.20)`.
- `nrj=0` = plein ; `nrj=50` = vide. Ratios visés : 50 % ≈ 43,5 %, 25 % ≈ 18,9 %, 10 % ≈ 6,3 %, 0 % = 0 % de l’efficacité nominale.
- La surcharge ×2 s’applique après la courbe énergie ; à 2 s l’état armé reste à puissance 0 jusqu’au relâchement.
- Vague visuelle seulement : durée ≈0,27 s pleine énergie, vers ≈0,50 s réserve vide ; collision/impact physique inchangé.
- Vagues deviennent plus visibles et chaudes quand l’énergie baisse ; sous 10 % : rouge lumineux clignotant ~4,5 Hz, sans flash HUD/écran.
- CLASSIQUE DUEL local : le premier doigt reste mouvement historique ; le second doigt de chaque moitié pilote le même état de surcharge partagé. Les deux joueurs sont indépendants.
- DUEL IA conserve son chemin remaster propriétaire owner 1 ; COOP conserve ses chemins existants.
- Android : routage du duel local dans la copie générée via `classic_duel_surge.hpp` + patch étroit ; `src/main.cpp` reste intact.
- Overlay visuel énergie via `kinetic_energy_visuals.hpp`, dessiné après le rendu cinétique existant ; purement graphique.

## TDD / preuves
- RED initial : workflow `37517885303` sur les attentes de duel classique avant implémentation (échec attendu à `sfKineticSurgePress(0)`).
- Branche/code en cours : tests énergie sévère, durée vague, duel deux owners et warning visuel ajoutés.
- GREEN complet Android + APK/AAB + secret hygiene requis avant merge/release.

## Release cible
- Cible : `v1.4.2`, `versionCode 12`, uniquement après CI fraîche GREEN.
- Fab a demandé une Release directe une fois le lot entièrement GREEN.
- Ne jamais écraser v1.4.1 ; ne pas publier avant vérification `src/main.cpp` + artefacts + SHA256SUMS.
