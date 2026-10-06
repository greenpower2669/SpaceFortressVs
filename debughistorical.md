# debughistorical.md — SpaceFortressVs

## Pièges permanents
- `src/main.cpp` historique ne doit jamais être modifié ; blob canonique `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- Hall : save campagne et état réseau séparés ; `submissionId` immuable ; page vide + `hasMore=true` avance durablement le cursor ; aucune vraie clé dans Git/logs/tests/mémoires.
- Livraison Actions : plusieurs artifacts peuvent porter le même nom ; toujours vérifier tentative/ID/SHA-256.
- Surcharge v2 : champ normal max 2,0 diamètres ; blast armé 3,0 ; <0,30 s aucune irisation/x2 ; à 2 s champ OFF jusqu’au relâchement.
- EMP : asset de déflagration déjà livré v1.4.1, ne pas le ré-authored dans v1.4.2.

## Clôture v1.4.1
- Release `v1.4.1` publiée et vérifiée ; commit/tag `450423c41c4cef6c348f49af698767016a0528fd`.
- Workflow publication `37490579073` GREEN + `publish-release` SUCCESS.
- APK SHA-256 `4d4f10f324a0b9929397b14f79fb36f4faae9b48a8a027f9ce4fdc864c014da8`.
- AAB SHA-256 `5e20d2a5bae408237b6a25a06e87623809b4258f48392f745100dd77f34eb087`.
- Ne jamais remplacer les bytes d’une Release existante sous le même tag.

## Mission v1.4.2 — fatigue énergie / duel classique
- Ne pas appliquer la nouvelle courbe à `nrj` lui-même : elle ne transforme que l’efficacité cinétique. La réserve historique reste 0=pleine, 50=vide.
- Courbe canonique demandée : `pow(energyFraction,1.20)`. À 50 % ≈0,435 ; 25 % ≈0,189 ; 10 % ≈0,063 ; vide = 0.
- La surcharge ×2 est appliquée après la fatigue. Ne pas faire `pow(energy×2,1.2)` ; cela rendrait la surcharge artificiellement trop forte.
- Le state `charged` à 2 s reste prioritaire : puissance cinétique 0 jusqu’au release.
- Le ralentissement concerne uniquement `SfKineticWave.duration`. Ne jamais retarder collision, fragmentation, dégâts, poussière ou purge pour suivre l’animation.
- Une vague capture son état énergétique à sa création ; elle ne doit pas accélérer/ralentir en cours de vie si `nrj` change.
- Le fallback des anciens appels 3 arguments de `sfKineticTriggerWave` dérive l’énergie visuelle depuis la force historique `.28 + .72*x`; les nouveaux comportements physiques restent dans `sfApplyKineticLayer`.
- Warning graphique : couleur équipe→orange→rouge ; sous 10 % rouge clignotant ~4,5 Hz. Le clignotement est graphique seulement, jamais un multiplicateur physique et jamais un flash plein écran/HUD.
- CLASSIQUE DUEL local : ne pas recoder `src/main.cpp`. Intercepter uniquement le second doigt dans la copie Android générée. Premier doigt reste livré au moteur historique.
- Les fingers de surcharge sont par-owner. Un troisième doigt est ignoré/consommé ; le release d’un owner ne touche jamais l’autre.
- DUEL IA garde son chemin owner 1 déjà présent dans `remaster_runtime.hpp`; ne pas le doubler.

## TDD v1.4.2
- RED initial : workflow `37517885303` / run 326 attendu en échec car le duel local owner 0 n’était pas encore câblé.
- Les tests doivent vérifier : courbe 100/50/25/10/0, surcharge après courbe, durée 0,27→~0,50, deux owners duel, overlay warning, invariants Danger/Hall/COOP.
- Avant merge/release : exiger une CI fraîche GREEN du HEAD final, secret hygiene, APK+AAB+packaging et blob `main.cpp` canonique.
