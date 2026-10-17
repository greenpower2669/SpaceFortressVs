# debughistorical.md — SpaceFortressVs

## Pièges permanents
- `src/main.cpp` historique ne doit jamais être modifié ; blob canonique `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- Hall : save campagne et état réseau séparés ; `submissionId` immuable ; page vide + `hasMore=true` avance durablement le cursor ; aucune vraie clé dans Git/logs/tests/mémoires.
- Livraison Actions : plusieurs artifacts peuvent porter le même nom ; toujours vérifier tentative/ID/SHA-256.
- Surcharge : champ normal max 2,0 diamètres ; blast armé 3,0 ; <0,30 s aucune irisation/x2 ; à 2 s champ OFF jusqu’au relâchement.
- EMP : asset de déflagration livré depuis v1.4.1 ; ne pas le ré-authored sans nouvel ordre.

## Clôture v1.4.2 — 2026-10-06
- PR #6 mergée dans `main` au commit `414b23cd2e787325b723fe7b0b6bd84fac02494b`.
- Release `v1.4.2` publiée sur `5c3bedbd592f7bf4c50b830d26d6a0da49048813`.
- Workflow publication `37530458354` GREEN ; job `publish-release` SUCCESS.
- APK SHA-256 `b5604ba8f103a351e62beb0751893d6ffdcc0d9549d6c75cb2545ea061ea8282`.
- AAB SHA-256 `4a996b60cc5bca9d874a7d2897c3ec83d095e3f5d9a989a192df3979647d74ea`.
- Release contient APK, AAB non signé, build.json et SHA256SUMS.
- Signature APK différente de v1.3.1 : ne pas désinstaller/effacer les données pour forcer un test de mise à jour.

## Pièges cinétiques v1.4.2
- Courbe canonique : `pow(energyFraction,1.20)`. Ne jamais transformer `nrj` lui-même.
- La surcharge ×2 est appliquée après la fatigue. Ne pas faire `pow(energy×2,1.2)`.
- Le state `charged` à 2 s reste prioritaire : puissance cinétique 0 jusqu’au release.
- Le ralentissement concerne uniquement `SfKineticWave.duration`. Ne jamais retarder collision, fragmentation, dégâts, poussière ou purge.
- Une vague capture son état énergétique à sa création ; elle ne doit pas changer de vitesse si `nrj` change ensuite.
- Ne pas déduire l’énergie visuelle depuis la force de couche/surcharge ; passer explicitement `sfKineticEnergyFraction(ship->nrj)`.
- Warning graphique sous 10 % : rouge clignotant ~4,5 Hz, graphique uniquement.
- CLASSIQUE DUEL local : intercepter uniquement le second doigt dans la copie Android générée ; premier doigt reste au moteur historique.
- En quittant le duel local, annuler uniquement les owners effectivement possédés par le bridge : `sfKineticSurgeCancel(owner)` + `sfKineticAudioCancel(owner)`.
- DUEL IA garde son chemin owner 1 ; ne pas le doubler.

## Aides et tutoriel — point de vigilance 09/10/2026
- Un texte d'aide peut devenir trompeur sans changement moteur : en v1.4.2 la poussière rouge réagit aux ondes, sans bonus ; le blanc peut aussi soigner un boss blessé en coop.
- Le deuxième doigt sert au tir bref et à la charge ; >0,30 s champ ×2, à 2 s champ désactivé jusqu'au relâchement. Ne pas décrire la purge comme un simple tir.
- L'aide en partie doit conserver le circuit suspendre / reprendre sans altérer la vraie partie. Les mini-jeux tutoriels proposés sont encore hors code.
