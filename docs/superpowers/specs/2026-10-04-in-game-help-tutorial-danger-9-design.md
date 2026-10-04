# SpaceFortressVs — aide in-game, tutoriel et danger 9 niveaux

Date : 4 octobre 2026
Branche : `feature/in-game-help-tutorial-danger-9`
Base : `main` après livraison 1.4.0

## 1. Intention

Ajouter une aide réellement utilisable par les joueurs, avec plusieurs profondeurs de lecture, des schémas animés et un tutoriel jouable intégré au jeu. En parallèle, étendre le réglage de danger de 5 à 9 niveaux et faire agir ce profil sur l'IA adverse du mode classique, sans transformer les projectiles ordinaires en tirs guidés.

Le lot doit préserver le moteur historique et les invariants de la 1.4.0, en particulier `src/main.cpp`, le modèle cinétique v3, la convention d'énergie et les poussières.

## 2. Décisions de produit validées

### Aide joueur

Le bouton `?` ouvre un centre d'aide avec quatre entrées :

1. **RAPIDE** — principe du jeu en quelques écrans ;
2. **DÉTAILLÉ** — règles, HUD, énergie, boucliers, poussières, astéroïdes, danger et stratégie ;
3. **ANIMÉ** — mêmes notions expliquées avec les vrais assets du jeu, des flèches, ondes, trajectoires et annotations SDL ;
4. **TUTORIEL** — exercices interactifs dans une arène d'entraînement isolée.

Le joueur choisit librement le format. Le bouton `?` est disponible depuis HOME et pendant une partie. En partie, l'aide suspend proprement la simulation et la fermeture reprend l'état précédent sans déclencher d'entrée parasite.

### Tutoriel

Le tutoriel est un sous-système sandbox possédant son propre état de simulation. Il réutilise les textures et les fonctions de mécanique partageables, mais ne modifie jamais les entités, statistiques, sauvegardes, progression de campagne ou Hall of Fame d'une vraie partie.

Modules :

- déplacement ;
- tir ;
- énergie et poussière blanche ;
- coque et bouclier historique ;
- astéroïdes et bouclier cinétique ;
- surcharge deux doigts de 2,00 s ;
- esquive et lecture des tirs ennemis ;
- IA classique et principe de l'anticipation.

Le joueur peut lancer un module seul ou `TOUT FAIRE`.

### Danger

Le réglage passe à neuf niveaux. Le niveau par défaut est le maximum.

| Index | Nom joueur | Dégâts internes | Cadence | Vitesse projectile | Part d'anticipation | Erreur angulaire max |
|---:|---|---:|---:|---:|---:|---:|
| 0 | MOU DU GENOU | ×1 | ×1.00 | ×1.00 | 0.20 | 12° |
| 1 | CHILL | ×5 | ×1.15 | ×1.08 | 0.30 | 10° |
| 2 | ROCK N ROLL | ×10 | ×1.30 | ×1.16 | 0.40 | 8° |
| 3 | DUR A CUIRE | ×15 | ×1.45 | ×1.25 | 0.52 | 6° |
| 4 | MACHINE DE GUERRE | ×20 | ×1.60 | ×1.34 | 0.64 | 4.5° |
| 5 | SANS PITIE | ×25 | ×1.80 | ×1.45 | 0.76 | 3° |
| 6 | CAUCHEMAR | ×30 | ×2.00 | ×1.56 | 0.86 | 2° |
| 7 | APOCALYPSE | ×35 | ×2.25 | ×1.68 | 0.94 | 1.25° |
| 8 | FIN DU MONDE | ×40 | ×2.50 | ×1.80 | 1.00 | 0.75° |

`FIN DU MONDE` est le défaut (`index 8`). Les coefficients restent des données internes : HOME affiche les noms, pas les valeurs numériques.

La cadence signifie une augmentation du nombre de tirs par unité de temps, donc les cooldowns sont divisés par le facteur de cadence. La vitesse projectile s'applique à la vitesse initiale des projectiles ordinaires concernés.

## 3. IA classique : anticipation sans guidage

L'IA adverse du mode classique calcule un point d'interception au moment où elle crée un tir.

Entrées du calcul :

- position du tireur ;
- position du vaisseau joueur ;
- vitesse instantanée du joueur ;
- vitesse initiale du projectile après application du profil de danger.

Le solveur cherche le premier temps d'interception positif de l'équation de rencontre projectile/cible. Si aucune solution positive raisonnable n'existe, il vise la position courante du joueur.

Le profil de danger détermine ensuite :

- la part d'anticipation appliquée entre position actuelle et point d'interception ;
- une petite erreur angulaire déterministe/aléatoire bornée selon le niveau.

**Invariant absolu : dès que le projectile est créé, son vecteur vitesse ne change plus pour poursuivre la cible.** Les tirs ordinaires restent rectilignes. Seules les armes déjà explicitement conçues comme guidées peuvent modifier leur direction après création.

Le danger classique agit sur :

- dégâts des tirs de l'IA adverse ;
- cadence de tir ;
- vitesse initiale des projectiles ;
- qualité de l'anticipation/précision.

Il ne doit pas modifier les tirs du joueur humain.

## 4. Coop/campagne

Le même profil de danger alimente les attaques hostiles non cinétiques du boss :

- dégâts ;
- cadence/période des salves lorsque le pattern possède un cooldown ;
- vitesse initiale des projectiles ;
- qualité d'anticipation des patterns qui visent un joueur.

Les patterns purement géométriques conservent leur géométrie ; seul leur rythme ou leur vitesse peut varier lorsqu'un paramètre correspondant existe.

Les astéroïdes, impacts cinétiques et charges explicitement cinétiques restent totalement hors profil de danger. Le modèle kinetic balance v3 reste inchangé.

## 5. Architecture choisie

### Option retenue — sous-systèmes séparés

Le système est divisé en trois responsabilités :

1. **profil de danger** : une source de vérité unique contenant nom, multiplicateur de dégâts, cadence, vitesse, anticipation et erreur ;
2. **centre d'aide** : navigation, pages, rendu de texte, illustrations et animations ;
3. **runtime tutoriel** : état sandbox, objectifs et interactions d'entraînement.

Cette option est retenue car elle évite d'alourdir `start_ui.hpp` et permet de tester les trois responsabilités indépendamment.

### Options rejetées

- **Page d'aide géante dans `start_ui.hpp`** : simple au début mais trop couplée aux entrées HOME et difficile à maintenir.
- **Documentation par vidéos pré-rendues** : bonne qualité visuelle mais APK plus lourd, moins adaptable aux résolutions et plus difficile à garder synchronisée avec le gameplay.

## 6. Fichiers et responsabilités prévues

Le plan d'implémentation pourra ajuster les noms exacts après lecture finale du code, mais les frontières sont les suivantes :

- `src/boss_danger.hpp` : devient la source de vérité du profil 9 niveaux tout en conservant les accesseurs historiques utiles ;
- nouveau composant d'aide, par exemple `src/help_runtime.hpp` : pages, navigation, dessins, chargement/réutilisation des textures ;
- nouveau composant tutoriel, par exemple `src/tutorial_runtime.hpp` : simulation sandbox et objectifs ;
- `src/start_ui.hpp` : bouton `?` HOME et point d'entrée visuel uniquement ;
- `src/remaster_ai_fix.hpp` : routage d'événements, ouverture/fermeture de l'aide en cours de partie et consommation des touches ;
- runtime classique/remaster approprié : adaptation du tir IA pour cadence, vitesse, dégâts et calcul d'interception sans modifier `src/main.cpp` historique ;
- `src/campaign_runtime.hpp` : application des champs cadence/vitesse/anticipation du profil aux attaques boss non cinétiques ;
- tests dédiés sous `tests/` et intégration dans `scripts/test-regressions.sh`.

`src/main.cpp` ne doit pas être modifié pour ce lot sauf preuve documentée qu'aucune couche remaster/runtime ne peut porter une exigence. La solution privilégiée doit rester extérieure à la source historique.

## 7. Centre d'aide

### RAPIDE

Objectif : moins d'une minute de lecture.

Pages :

- but du jeu ;
- se déplacer et tirer ;
- PV / énergie ;
- poussière blanche / rouge ;
- bouclier cinétique ;
- surcharge 2 doigts ;
- danger ;
- retour au jeu.

### DÉTAILLÉ

Pages :

- modes duel / IA / coop ;
- lecture du HUD ;
- convention énergie `nrj=0` plein / `nrj=50` épuisé expliquée en termes joueur sans exposer l'inversion interne ;
- coque et protections ;
- poussières ;
- astéroïdes et dégâts cinétiques ;
- vagues cinétiques ;
- surcharge et fenêtre de vulnérabilité après 2 s ;
- boss et patterns ;
- danger et évolution de l'IA ;
- stratégies et astuces ;
- section avancée.

### ANIMÉ

Les illustrations sont produites en temps réel avec les assets existants du jeu : vaisseaux, astéroïdes, poussières, missile, boss, tourelles et effets disponibles. SDL dessine les flèches, trajectoires, cercles, zones et légendes.

Exemples :

- projectile droit + point d'anticipation ;
- vague centre → extérieur ;
- poussière blanche allant énergie puis coque ;
- charge deux doigts de 0 à 2 s, état prêt, puis relâchement ;
- comparaison d'une esquive contre un tir direct et un tir anticipé.

Aucune vidéo supplémentaire n'est requise pour cette première version.

## 8. Navigation et accessibilité

- grandes zones tactiles ;
- texte à fort contraste ;
- police bitmap existante réutilisée ou étendue seulement si nécessaire ;
- bouton retour toujours identifiable ;
- précédent / suivant sur les pages longues ;
- indicateur de page ;
- aucun geste de documentation ne doit traverser vers le contrôleur du jeu ;
- Android BACK revient d'abord d'une page/module vers le centre d'aide, puis au contexte précédent.

## 9. Pause et reprise

Quand l'aide est ouverte depuis une partie :

- la simulation gameplay ne progresse plus ;
- aucun cooldown, projectile, astéroïde, charge cinétique ou chrono de campagne ne doit avancer ;
- les doigts actifs sont neutralisés proprement ;
- la fermeture rétablit la partie sans FINGERUP parasite ni tir involontaire.

Le tutoriel possède son propre temps et peut continuer à fonctionner pendant que la vraie partie reste suspendue.

## 10. Invariants 1.4.0 à préserver

- `src/main.cpp` historique protégé ;
- campagne 200 intacte ;
- `nrj=0` réserve pleine, `nrj=50` épuisée en interne ;
- poussière blanche : recharge puis soin ;
- poussière rouge : visuelle uniquement ;
- cinétique v3 : masse × vitesse relative × fermeture, sans plancher ;
- référence maximale 250 PV bruts pour l'impact de référence ;
- danger exclu des chemins cinétiques ;
- surcharge deux doigts 2,00 s, champ ×2 pendant charge, champ cinétique OFF après armement tant que le doigt reste posé, purge au relâchement prêt ;
- vagues centre → extérieur.

## 11. Tests d'acceptation

### Profil de danger

- exactement 9 niveaux ;
- ordre et noms exacts ;
- index par défaut = 8 ;
- cycle HOME correct ;
- coefficients internes exacts ;
- aucun coefficient numérique affiché sur HOME.

### IA classique

- à cible immobile, tir droit vers la cible ;
- à cible mobile, le point de tir se décale dans le sens de déplacement ;
- qualité d'anticipation augmente avec le danger ;
- vitesse du projectile respecte le plafond ×1.80 ;
- cadence respecte le plafond ×2.50 ;
- dégâts utilisent le multiplicateur du niveau ;
- le vecteur vitesse du projectile reste constant après création ;
- aucun effet du danger sur les tirs humains.

### Coop/campagne

- dégâts non cinétiques suivent le niveau ;
- cadence/vitesse/anticipation suivent le profil lorsqu'elles sont applicables ;
- astéroïdes et charges cinétiques donnent exactement les mêmes résultats qu'avant ce lot.

### Aide

- `?` HOME ouvre le centre ;
- `?` en partie suspend le gameplay ;
- navigation RAPIDE / DÉTAILLÉ / ANIMÉ / TUTORIEL ;
- BACK et RETOUR cohérents ;
- aucun événement tactile d'aide ne fuit vers le jeu ;
- assets manquants : fallback lisible, aucun crash.

### Tutoriel

- chaque module peut être lancé séparément ;
- `TOUT FAIRE` enchaîne les modules ;
- les objectifs réagissent aux vraies entrées du joueur ;
- aucune sauvegarde, statistique ou progression de vraie partie ne change ;
- retour au contexte précédent sans mutation gameplay.

### Régression complète

Après les tests ciblés :

- suite native/régressions existantes ;
- kinetic regressions ;
- legacy field regressions ;
- campagne/sauvegarde ;
- génération Android ;
- build APK + AAB ;
- contrôle packaging et assets.

## 12. Critère de fin

Le lot est prêt pour test téléphone lorsque :

1. tous les tests ciblés sont verts ;
2. toute la régression 1.4.0 est verte ;
3. un APK Android frais est produit depuis le SHA exact ;
4. les quatre mémoires vivantes sont synchronisées ;
5. aucun changement de `src/main.cpp` n'a été nécessaire, ou toute exception est explicitement justifiée et approuvée avant intégration.
