# todo.md — SpaceFortressVs

## Hall global sync v1
- [x] Spec + plan TDD validés; branche dédiée.
- [x] Tasks 1–7 : stockage, UUID/payload, pagination/cache, hooks, HTTPS Java, JNI, Hall fusionné.
- [x] Task 8 : CI complète GREEN sur workflow `37372279557`, tentative 2; artifact configuré `11374640381`; secret absent de Git; `src/main.cpp` protégé.
- [x] Incident de livraison `SYNC NON CONFIGUREE` expliqué : ancien APK sans clé remis par erreur.
- [x] Fab a testé le bon APK configuré : `SYNC OK`, `GLOBAL 1 + LOCAL 0`.

## Mini-fix surcharge cinétique v2 — pré-release
- [x] Design borné validé par Fab : délai visuel/x2 0,30 s, charge armée 2,00 s, blast/purge 3,0×, son EMP.
- [x] TDD RED : workflow `37424970512` échoue sur la constante de délai absente, comme attendu.
- [x] Implémenter seuil 0,30 s sans changer le champ normal 2,0×.
- [x] Porter uniquement le blast/purge armé à 3,0×.
- [x] Ajouter le transport du nouveau son EMP et son décodage contrôlé dans l’asset Android `kinetic_release.wav`.
- [x] Garder la même logique CLASSIQUE + COOP et supprimer le sentinel RED temporaire.
- [x] Corriger le checksum du transport EMP sans toucher au gameplay : SHA `43326ec5ba92d40b2378b0877775bce28d21b1b4`.
- [x] CI Android complète GREEN : workflow `37427270384` / run 320, toutes étapes `build-android` réussies; `publish-release` SKIPPED.
- [x] `src/main.cpp` revérifié au blob protégé `835059a0ecfe0f74708068b3259cad5db1cdb579`.
- [x] Artifact candidat disponible : `SpaceFortressVs-1.4.0-release-files`, id `11395476858`, digest `sha256:b1f6640c65d4d90237dc477df72212dcad46b63678faca6270deb584d7436cc3`.
- [ ] Télécharger l’APK configuré du workflow final et vérifier son SHA-256 avant livraison à Fab.
- [ ] Validation téléphone Fab : appui <0,30 s sans irisation, charge 0,30–2 s, READY à 2 s, blast 3× + son EMP, gros astéroïdes purgés.
- [ ] Après validation seulement : merge `main` puis Release sur ordre explicite de Fab.

## Autres validations téléphone encore ouvertes
- [ ] Hall Danger 1★/9★ sur nouvelles victoires si Fab veut les recontrôler.
- [ ] Effets poussières cinétiques hors mini-fix v2.

## Invariants
- [x] `src/main.cpp` protégé.
- [x] Aucun merge main / aucune release.
