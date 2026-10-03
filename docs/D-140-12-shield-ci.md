# D-140-12 — vérification Android du bouclier partagé

Code gameplay à vérifier : `738199b38ef68aa83810c137af9c45a13fc69c6a`.

Contrat : même bouclier linéaire en duel classique Android et campagne coop, ×15 coop appliqué une seule fois avant protection, usure dépendante de l'impact, poussière blanche pleine = 0,25 nrj récupéré, `src/main.cpp` historique inchangé.

Ce commit documentaire sert aussi à déclencher la CI Android normale, car un push effectué depuis GitHub Actions ne peut pas déclencher automatiquement un autre workflow avec le token interne.

Aucun merge main. Aucune release. Validation finale sur téléphone par Fab.
