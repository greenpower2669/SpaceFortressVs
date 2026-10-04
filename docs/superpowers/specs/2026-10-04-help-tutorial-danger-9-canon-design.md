# SpaceFortressVs — aide, tutoriel et danger 9 niveaux — design canonique

Date : 4 octobre 2026
Branche : `feature/help-tutorial-danger-9-canon`
Base : `main` au SHA `703676aaa3ae16a0ef415802dbb9ea24a229e61c`

## 1. Objectif

Ajouter une documentation intégrée au jeu, accessible depuis HOME et pendant une partie, avec trois niveaux de détail choisis par le joueur, plus un tutoriel in-game séparé et guidé pas à pas. Étendre en parallèle le sélecteur de danger de 5 à 9 niveaux et appliquer ce multiplicateur aux attaques hostiles non cinétiques concernées, y compris les tirs de l'IA en mode classique.

Le lot doit préserver le moteur historique, notamment `src/main.cpp`, le modèle cinétique v3, la campagne 200, les poussières et la convention d'énergie.

## 2. Aide intégrée

Un bouton `?` est disponible :
- sur l'écran HOME ;
- pendant une partie.

Il ouvre un centre d'aide proposant quatre entrées :
- `RAPIDE` ;
- `DETAILLE` ;
- `ANIME` ;
- `TUTORIEL`.

Les trois formats d'aide sont choisis librement par le joueur. Le choix est conservé pour les ouvertures suivantes. Le format par défaut est `ANIME`, considéré comme le niveau maximal de détail.

### RAPIDE

Antisèche courte : but du jeu, déplacement, tir, HUD, PV, énergie, poussières, bouclier cinétique, surcharge deux doigts, danger.

### DETAILLE

Explications complètes :
- HUD ;
- PV/coque ;
- énergie, avec la convention interne `nrj=0` plein et `nrj=50` épuisé expliquée en termes joueur ;
- poussière blanche ;
- poussière rouge ;
- bouclier historique ;
- bouclier cinétique ;
- vagues cinétiques ;
- surcharge deux doigts ;
- fenêtre de vulnérabilité après 2,00 s ;
- modes classique / IA / coop ;
- campagne 200 ;
- difficulté/danger ;
- astuces de jeu.

### ANIME

Même contenu que `DETAILLE`, enrichi de pages animées temps réel utilisant les assets du jeu et des schémas SDL :
- vrais vaisseaux / boss / astéroïdes / poussières / missiles quand disponibles ;
- flèches, trajectoires, zones, barres et annotations ;
- animation d'une vague centre -> extérieur ;
- animation de la surcharge 0..2 s, état prêt puis relâchement ;
- démonstration énergie -> poussière blanche -> soin ;
- comparaison poussière blanche / rouge ;
- exemple de HUD commenté.

Aucune vidéo supplémentaire n'est requise pour cette première version.

## 3. Aide ouverte en cours de partie

Quand `?` est ouvert pendant une vraie partie :
- la simulation réelle est suspendue ;
- aucun projectile, astéroïde, cooldown, chrono campagne ou charge cinétique ne progresse ;
- les doigts actifs sont neutralisés proprement ;
- les interactions de l'aide ne traversent jamais vers le gameplay ;
- fermer l'aide reprend exactement la partie, sans tir ou `FINGERUP` parasite.

Android BACK remonte d'abord dans l'aide, puis revient au contexte précédent.

## 4. Tutoriel in-game séparé

`TUTORIEL` est distinct des trois formats d'aide. Il lance une session d'entraînement in-game avec guidage pas à pas et surbrillance des zones/gestes attendus.

Étapes canoniques :
1. déplacement ;
2. tir ;
3. lecture du HUD ;
4. énergie ;
5. PV/coque et protections ;
6. poussière blanche et poussière rouge ;
7. astéroïdes et bouclier cinétique ;
8. maintien du deuxième doigt pendant 2,00 s ;
9. état prêt / vulnérabilité ;
10. relâchement et purge ;
11. difficulté/danger.

Le tutoriel peut proposer `TOUT FAIRE` ou un module individuel. Il réutilise les mécaniques et assets du jeu, mais ne modifie jamais progression campagne, sauvegarde, Hall of Fame ni statistiques d'une vraie partie.

## 5. Danger : 9 niveaux

Le sélecteur conserve le principe actuel : l'interface affiche des noms, jamais les coefficients numériques.

| Index | Nom visible | Multiplicateur interne |
|---:|---|---:|
| 0 | `MOU DU GENOU` | x1 |
| 1 | `CHILL` | x5 |
| 2 | `ROCK N ROLL` | x10 |
| 3 | `DUR A CUIRE` | x15 |
| 4 | `MACHINE DE GUERRE` | x20 |
| 5 | `CA VA PIQUER` | x25 |
| 6 | `SANS PITIE` | x30 |
| 7 | `ENFER STELLAIRE` | x35 |
| 8 | `APOCALYPSE` | x40 |

Le défaut reste `ROCK N ROLL` (`index 2`, x10 interne).

Ce sélecteur de danger est distinct des 4 difficultés de campagne 200. Les règles propres aux 4 difficultés de campagne restent inchangées.

## 6. Portée exacte du multiplicateur

Le profil de danger agit sur les dégâts des attaques hostiles **non cinétiques** concernées.

### Coop / campagne

Le comportement existant est conservé et étendu aux 9 niveaux : projectiles, rayons, vagues ou contacts non cinétiques du boss utilisent le multiplicateur choisi.

### Classique / IA

Quand un vaisseau est contrôlé par l'IA, les projectiles hostiles qu'il tire contre le joueur appliquent le multiplicateur choisi au dégât non cinétique infligé.

Le danger ne doit pas :
- multiplier les tirs du joueur humain ;
- modifier la cadence de tir ;
- modifier la vitesse initiale des projectiles ;
- modifier la précision, l'anticipation ou l'algorithme de l'IA ;
- transformer un projectile ordinaire en projectile guidé ;
- modifier astéroïdes, charges cinétiques ou autre chemin cinétique.

Le lot ajoute donc des niveaux de **dégâts**, pas une nouvelle courbe de cadence/vitesse/précision.

## 7. Architecture retenue

Séparer les responsabilités :
- `src/boss_danger.hpp` : source unique des 9 noms et multiplicateurs ;
- nouveau composant d'aide, par exemple `src/help_runtime.hpp` : navigation, contenu, rendu rapide/détaillé/animé, bouton `?`, retour au contexte précédent ;
- nouveau composant tutoriel, par exemple `src/tutorial_runtime.hpp` : session guidée isolée ;
- `src/start_ui.hpp` : intégration visuelle HOME, sans devenir un monolithe ;
- `src/remaster_ai_fix.hpp` : routage tactile HOME / HELP / GAME et suspension/reprise ;
- couche runtime classique/remaster : classification du tir hostile IA et application du danger au dégât non cinétique ;
- `src/campaign_runtime.hpp` : conserve l'application boss non cinétique avec la table 9 niveaux ;
- tests dédiés sous `tests/`, intégrés à `scripts/test-regressions.sh`.

`src/main.cpp` reste byte-for-byte historique sauf preuve documentée qu'aucune couche existante ne peut porter une exigence, auquel cas Fab doit approuver explicitement l'exception avant toute modification.

## 8. Accessibilité et navigation

- grandes zones tactiles ;
- fort contraste ;
- texte lisible avec la police bitmap existante ou son extension ;
- précédent / suivant ;
- indicateur de page ;
- bouton retour toujours visible ;
- fallback lisible si une image/texture d'aide manque ;
- aucun crash si un asset d'illustration est indisponible.

## 9. Invariants 1.4.0 à préserver

- campagne 200 intacte ;
- `nrj=0` plein / `nrj=50` épuisé ;
- poussière blanche = recharge puis soin ;
- poussière rouge = visuelle uniquement ;
- cinétique v3 inchangé ;
- danger exclu des chemins cinétiques ;
- surcharge deux doigts = 2,00 s, champ x2 pendant charge, champ cinétique OFF après armement tant que le doigt reste posé, purge au relâchement prêt ;
- vagues centre -> extérieur ;
- `src/main.cpp` protégé.

## 10. Tests d'acceptation

### Danger
- exactement 9 niveaux, ordre et noms exacts ;
- valeurs internes x1/x5/x10/x15/x20/x25/x30/x35/x40 ;
- défaut = `ROCK N ROLL` ;
- aucun coefficient numérique affiché dans l'UI ;
- cycle HOME correct ;
- aucun changement de cadence/vitesse/précision dû au danger.

### Classique IA
- à danger x1, dégât hostile de référence inchangé ;
- à x5..x40, seul le dégât hostile non cinétique est multiplié ;
- tir humain inchangé ;
- projectile ordinaire reste rectiligne ;
- astéroïde et cinétique identiques à la 1.4.0.

### Aide
- `?` HOME ouvre le centre ;
- `?` en jeu ouvre l'aide et suspend la vraie partie ;
- `RAPIDE`, `DETAILLE`, `ANIME` fonctionnent ;
- `ANIME` est le choix par défaut ;
- préférence de format conservée ;
- navigation BACK/RETOUR cohérente ;
- aucun événement tactile ne fuit vers le jeu ;
- fallback propre sur asset manquant.

### Tutoriel
- lancement séparé ;
- étapes guidées dans l'ordre ;
- modules individuels et `TOUT FAIRE` ;
- aucune progression/sauvegarde/Hall of Fame modifiée ;
- retour propre au contexte précédent.

### Régression
- suite native ;
- kinetic regressions ;
- legacy field regressions ;
- campagne/sauvegarde ;
- génération Android ;
- build APK + AAB ;
- packaging/assets.

## 11. Discipline de livraison

- branche dédiée uniquement ;
- aucun merge `main` ni release sans nouvel accord de Fab ;
- synchroniser `brain.md`, `brainmap.md`, `debughistorical.md`, `todo.md` avec le nouvel état sans les regonfler ;
- archiver les détails de mission et preuves de tests hors des quatre mémoires vivantes ;
- validation finale téléphone par Fab.
