# Archive intégrale pré-compaction — SpaceFortressVs — 19/10/2026

**Branche :** `feature/solo-coop-core-v1`.
**Objectif :** alléger les cinq mémoires actives sans perdre aucune information, retour arrière, validation, preuve CI, mission historique, tâche ou autorisation d'époque. Archive **en lecture seule** ; ce n'est pas la mission active.

## Copies intégrales conservées

| Fichier historique | Caractères | Blob Git SHA-1 original et vérifié |
| --- | ---: | --- |
| [ordres-de-mission.md](./ordres-de-mission.md) | 154460 | `423e37382b9957c78dcc8da2fdc700bec4ee738e` |
| [brain.md](./brain.md) | 149377 | `a71b54d1b8cc3bf1f269bdc12dec856879fef99f` |
| [brainmap.md](./brainmap.md) | 105938 | `085a0e883ce513fc2c5ff62f15efc1691bac0afb` |
| [debughistorical.md](./debughistorical.md) | 127727 | `c10ad0cd5294d422cf39adb2fbbcfce26620f420` |
| [todo.md](./todo.md) | 115259 | `7400948cf705ee3528a66adf40f865a8e6a17c09` |

Ces copies ont été créées **avant** toute réduction des fichiers à la racine, et chaque blob archivé a été comparé à son original : même SHA Git.

## Reprise de travail

1. Lire seulement `/brainmap.md` (carte courte), puis `/ordres-de-mission.md`, `/brain.md`, `/debughistorical.md` et `/todo.md` actifs selon la mission.
2. Ne consulter les copies ci-dessus que si une ancienne mission, une preuve, un incident ou une procédure de rollback nécessite sa version complète.
3. La section `IMMUTABLE / À PRÉSERVER` active conserve mot à mot ses règles originelles ; ne jamais présumer que des GO de code historiques sont valides pour une demande future. Suivre le dernier ordre explicite de Fab.
4. Anciennes cases TODO non cochées = **historiquement non résolues ou sans clôture prouvée**, pas automatiquement annulées. Si elles reviennent dans le périmètre, réexaminer leur état dans le code et la CI.
5. Ne pas confondre tests automatisés verts, fichiers APK exportés et validation tactile réelle de Fab. Les versions de branche SOLO ne sont pas publiées dans `main` sans son approbation.
6. Nouvel état au 19/10 : Fab exprime un retour **positif** sur les commandes SOLO-033 réutilisées dans SOLO-034 ; les futures textures extérieures à cratères sont **une intention esthétique seulement, sans GO de codage** et doivent se fondre dans la roche actuelle.

## Restaurer ou étudier un ancien état

Comparer `<fichier racine>` à `docs/memory-archive/2026-10-19-pre-compaction/<fichier>`; pour un retour historique documentaire, réintroduire uniquement les sections pertinentes avec validation, **sans restaurer aveuglément un ordre obsolète**. Les SHA d'origine figurent ci-dessus.
