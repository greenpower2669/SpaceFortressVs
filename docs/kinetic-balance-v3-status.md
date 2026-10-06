# Kinetic balance v3 — statut de validation

Date : 2026-10-04
Branche : `fix/gameplay-campaign-200`
Gameplay vérifié : `59a5e7bd1bc43e7934c867d0be7f12bb4aaa235b`

## Canon implémenté

- Sélecteur HOME par noms uniquement : MOU DU GENOU, CHILL, ROCK N ROLL, DUR A CUIRE, MACHINE DE GUERRE. Un clic change uniquement la sélection ; seul LANCER LA PARTIE démarre le combat. Coefficients internes x1/x5/x10/x15/x20, ROCK N ROLL par défaut.
- Le multiplicateur de danger boss reste réservé aux dégâts hostiles non cinétiques.
- Astéroïdes : formule cinétique linéaire masse relative x vitesse relative x composante de rapprochement, bornée ; plus gros astéroïde à vitesse de référence et plein contact = 25 % des 1000 PV canoniques avant protections. Aucun plancher de dégâts à l'arrêt.
- Le résidu cinétique de contact passe par le bouclier énergétique normal, sans double dégât historique.
- Poussière rouge : strictement visuelle, sans effet PV/énergie/bouclier.
- Poussière blanche : recharge la réserve jusqu'à `nrj=0`, puis soigne fortement les PV sur les collectes suivantes.
- Toutes les vagues cinétiques utilisent un front nettement visible centre vers extérieur puis disparition progressive. En surcharge, les fronts sont irisés.
- Surcharge deuxième doigt inchangée : charge 2 s à puissance x2, signal chargé puis protection cinétique coupée tant que le doigt reste posé ; relâchement chargé = décharge/purge.

## Vérifications

Le workflow Kinetic balance v3 GREEN a validé le test ciblé, la suite native complète et l'intégrité de `src/main.cpp` avant le commit gameplay. Un ancien test du champ plaçait un astéroïde immobile directement sur la coque ; il a été corrigé pour simuler un vrai impact entrant et utiliser `sfCoopAsteroidHurt` en coop.

Validation téléphone encore requise avant tout merge `main` ou publication de release.
