# SpaceFortress — SOLO BONUS CORE-012 (DIAGNOSTIC)

Branche : feature/solo-coop-core-v1. Aucun merge main ni Release publique.
Source `95743f2386b220bb692112197c4ddee02d27ae2b` ; [CI Android/C++/SDL2 verte](https://github.com/greenpower2669/SpaceFortressVs/actions/runs/38299079636), publication Release SKIPPED.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-012-95743f2.apk` : 94168883 octets, SHA256 `c61d7e39ecb075d69db863e7cc964021bb9c3ed31ccf0c0597b01b1ffaa09248`.
- `SpaceFortressVs-1.4.2-SOLO-BONUS-012-95743f2-unsigned.aab` : 91525121 octets, SHA256 `a4433fc2f09cae480ab407f1997139821602caa71c0c0f8a61941baceeed6289`.

APK : `SpaceFortressVs-1.4.2-SOLO-BONUS-012-95743f2.apk`, identité `com.greenpower2669.spacefortressvs.diagnostic`.
AAB : `SpaceFortressVs-1.4.2-SOLO-BONUS-012-95743f2-unsigned.aab`, NON SIGNÉ ; launcher release historique.
Le menu SOLO de test est celui de l'APK diagnostic, pas celui de l'AAB release.
Empreintes après extraction : SHA256SUMS ; manifeste joint avec commit et CI source.
Tous les diagnostics précédents sont conservés.

## Essai Samsung

Ouvrir **SOLO BONUS 012 : MOTEUR COOP**, puis **MONDE TEST** pour le labyrinthe.
**BOSS TEST/OFF** commande le boss ; le boss du MONDE reste stationnaire de test.
Le même système de bonus fonctionne dans l'arène et dans le labyrinthe.
Les récompenses tombent à 75/50/25 % PV et après défaite ; collecte par contact.
Les salves restent commandées par le joueur, sans tir automatique.

- PV : +350, plafond 1000, aucune résurrection.
- Énergie : chaleur nrj -25, bornée à 0..50 (0 = pleine réserve).
- Furtivité : 5 s, coque semi-transparente et acquisition dirigée suspendue ; collisions physiques conservées.
- Missiles : 3 s sans coût énergétique, cadence et commande humaines historiques.
- Prison : impact 12 et statut immobilisation 2 s.
- Glace : impact 12 et statut vitesse x0,4 pendant 4 s.
- Lave : impact 12 puis 24 dégâts/s pendant 4 s, sans cumul du débit.
- Météore : attaque ciblée, impact 120 et effets propres ; aucune vague automatique METEOR-009.

Reprise d'un pouvoir temporisé : durée réinitialisée, jamais additionnée.
Même type d'orbes : +3 salves, maximum 6 ; nouveau type : remplace par 3 salves.
Chaque salve contient 3 projectiles guidés et paie une seule dépense historique.
Missiles gratuits prioritaires sans consommer l'inventaire d'orbes.
Boss OFF : tirs ordinaires possibles, charges d'orbes conservées.
Pause : durées gelées. Changement arène/monde et relancement : bonus remis à zéro.
Collectibles : durée 45 s, maximum 24 simultanés, positions accessibles à la coque.

## Limites vérifiées et travail restant

Tests purs, moteur réel, collisions/collecte, rendu SDL2 et compilation Android verts.
Prison/Glace modifient le mouvement du boss d'arène dans les tests automatisés.
Aucun ralentissement mobile n'est démontré sur le boss stationnaire du labyrinthe.
FPS/ergonomie réels Samsung encore à contrôler ; logs SOLO_BONUS_012 moyenne/max disponibles.
Astéroïdes historiques, minage/recharge conservés ; leur défaut face aux parois reste ouvert.
Nouvelles vagues METEOR-009 suspendues. Aucune tourelle ni ancien bonus à tourelles en SOLO.
src/main.cpp inchangé ; modes historiques protégés par gardes SOLO et régressions.
Carte/progression/sauvegardes antérieures conservées ; raccord complet à la future campagne encore distinct.
