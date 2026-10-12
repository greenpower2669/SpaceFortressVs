# SOLO BONUS CORE-012 — conception et contrat

Autorisation Fab du 12 octobre 2026 : coder la brique complète et joindre le monde exploratoire au vrai moteur COOP. Référence 1c6c0b509bd7e74d5b41f086ce6c54c7adf3c8a2. Ni main/src/main.cpp, ni Release, ni changement de comportement historique. La mission explicite d’implémentation prime sur les anciens gels d’ajout de fonctionnalités ; les protections tourelles et vagues METEOR-009 restent en vigueur.

## Huit pouvoirs et règles explicites

Les paramètres non fixés par Fab sont des choix de réalisation configurables, pas des canons historiques.

| Bonus | Effet |
|---|---|
| PV | +350, plafond 1000 ; ne ressuscite pas |
| Énergie | retire 25 de nrj (0 plein, 50 épuisé), sans dépasser les bornes |
| Furtivité | 5 s de simulation ; coque à alpha 100 ; interdit acquisition/poursuite dirigée, conserve collisions et dangers déjà présents |
| Missiles | 3 s ; tir humain et cadence historique, nrj inchangée par le tir ; pas d’autotir |
| Prison | trois salves de trois orbes ; dégâts orbe 12, statut immobilisation 2 s sur impact |
| Glace | trois salves de trois orbes ; dégâts 12, facteur déplacement boss 0,4 pendant 4 s |
| Lave | trois salves de trois orbes ; dégâts 12 et brûlure 24 PV/s pendant 4 s |
| Météore | trois salves de trois attaques guidées ; dégâts 120 par impact, FX roche/explosion ; jamais de vague ni objet astéroïde autonome |

Furtivité et missiles coexistent. Recollecter le même pouvoir temporel rafraîchit sa durée depuis la collecte, sans addition. Un seul type d’orbe équipé : même type ajoute trois salves jusqu’à six ; autre type remplace par trois. Missiles temporaires prioritaires et gardent les salves en réserve. Une salve coûte une dépense historique de tir, une charge, trois projectiles ; aucun débit si cooldown, capacité insuffisante ou boss absent. Le canon historique autorise encore une orbe lente à nrj=50 ; cette règle reste intacte. Boss absent : tir historique reste possible, salves conservées. Statuts Prison/Glace/Lave coexistent ; répétition rafraîchit, sans multiplication du DPS. Prison prime sur Glace pour le mouvement. Pause : temps gelé. Retour/relaunch/changement de monde : reset explicite, aucun transfert historique.

## Distribution et accessibilité

Chaque rencontre distribue une fois par franchissement descendant des seuils 75 %, 50 %, 25 %, défaite, même si un gros impact en franchit plusieurs. Paquets : PV/énergie/prison ; furtivité/missiles/glace ; lave/météore/PV ; énergie/furtivité/missiles. Remontée des PV ne réarme pas les seuils. Réactivation volontaire d’un nouveau boss ouvre une nouvelle rencontre.

Pickups : collecte par contact coque, durée 45 s, plafond 24. Recherche bornée en cellules accessibles depuis le pilote en tenant compte de la coque, puis préférence près du boss. Pas de placement à travers une cloison ni collecte à travers mur. Échec de placement : reporter la demande tant qu’une place n’existe pas, jamais forcer dans un mur.

## Architecture

- solo_bonus_core.hpp : état pur par session, définitions, timers, loot et collecte, aucun global SDL/FabLib.
- coop_solo_bonus_runtime.hpp : adaptateur du vrai SfCoopState, tupl/tuplv historiques pour projectiles, statuts, dégâts, drops ; gated soloDiagnostic ou soloWorldMode.
- coop_solo_bonus_draw.hpp : textures existantes, quatre découpes explicites de la planche environnement, rendu commun arène/monde avec projection ; HUD compteurs et FX bornés.
- CoopSoloWorldBridge : géométrie des nouveaux tirs et sélection d’un emplacement accessible. Aucun autre intégrateur de vaisseau.
- L’arène témoin et le labyrinthe utilisent le même module. La réunion du monde et du combat passe par ce raccordement ; ne pas annoncer une nouvelle IA mobile ou une sauvegarde de partie complète.

La simulation conserve les positions monde. Les projectiles bonus utilisent la liste historique avec un tag SOLO et l’identifiant de rencontre ; ils ne poursuivent pas un boss ultérieur. Résolution continue des parois avant dégâts. Retirer un boss annule ses statuts/attaques en vol, mais laisse ses récompenses collectables.

## Vérification

Tests core (timers exacts, ressources, stacking, paquets, collecte/expiration), tests du vrai COOP (fire/cadence/nrj/guidage/dégâts/statuts/targeting, isolation CLASSIQUE/COOP, murs, reset), SDL pixels/assets et smoke arène/monde, historique intégral et Android. Prison et Glace opérationnels comme statuts ; démonstration sur boss mobile limitée à l’arène historique de test, aucune prétention d’IA mobile dans le labyrinthe.

APK diagnostic dans temp/solo-bonus-core-012/, nom avec version et SHA, AAB séparé, build exact CI vert et export vérifié. Performances : collections plafonnées, BFS une fois par paquet de drops, sélection excluant les emplacements occupés, textures chargées une fois, rendu hors écran filtré. Mesure réelle Samsung nécessaire pour garantir FPS/ergonomie matériel.

Les événements de seuil sont mémorisés immédiatement après chaque dégât, avant la recharge par poussière ; un boss SOLO à zéro PV ne peut plus être soigné. Collecte et accessibilité utilisent le même rayon de coque que le pilote monde. Le HUD SOLO commun remplace les deux copies du HUD COOP uniquement dans les sessions SOLO.
