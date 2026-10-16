# SOLO-018 — Rebond immédiat et pinch direct (16 octobre 2026)

## Retour Samsung Fab
L'inertie PHYSICS-017 fonctionne très bien en déplacement libre. Défaut constaté : vaisseau collé contre une paroi, aucune réponse ressentie pendant un moment, rebond apparent tardif. Le premier doigt commençait parfois hors coque et capturait le pilotage, ce qui empêchait le zoom spontané.

## Diagnostic
`src/campaign_runtime.hpp` annule `control.velocity` sur position refusée. `moveWithoutTunnelling` renvoie seulement la position, aucune vitesse ni normale. Le test moteur imposait `velocity.vx==0`. C'est la cause immédiate de l'arrêt, mais le temps de blocage exact Samsung n'a pas été mesuré.

## Correctif candidat 8875905
- `CoopSoloWorldBridge::moveWithBounce` : collision balayée, normale, restitution normale 46 % au choc, conservation tangente 94 %, intégration temps restant et récupération bornée d'une légère pénétration.
- `SfCoopState::soloWorldBounce` : callback spécifique SOLO ; aucun remplacement global des résolveurs de projectiles/ennemis.
- `sfCoopMovePlayers` : conservation de vitesse réfléchie et filtrage du seul objectif dirigé vers la paroi pendant 0,20 s, sans empêcher un geste de sortie ; bords de l'arène finale amortis aussi.
- `runCoopSoloSession` : mouvement au premier DOWN **sur coque** seulement, geste hors coque pour arme ; deux doigts hors coque déclenchent un pinch exclusif ×1..×2, avec annulation de charge du premier doigt et nettoyage des identités tactiles. Aucun appui de présélection préalable.
- Tests corrigés : recul, glissement oblique, absence de traversée, pince direct et libération des deux doigts. Préserver les autres tests historiques.

## Vérification nécessaire
CI Android/C++/SDL2 du commit `8875905fc3b6c39d8162fa668670e279a1b2e6d7` : run `38962472820`, état en attente lors de la consignation. Un résultat CI n'est pas une validation du ressenti mobile.

Vérifier sur Samsung : pilotage coque, deux doigts hors coque sans tir, glissement oblique, rebond immédiat face au mur à faible/grande vitesse, maintien du doigt pressant le mur, commande de dégagement, coins, zoom ×1/×2, reprise et transition arène. Aucun nouveau son ou ressource, aucune publication ni merge main sans accord.

## Première CI : échec ciblé et relance
`38962472820` : solo-core SUCCESS, SDL2-ui SUCCESS, Android arrêt des régressions sur une assertion de signe final pour une vitesse artificielle `100000 px/s`. Le sens final après plusieurs impacts ne prouve pas l'absence de rebond ; le test a été remplacé par les invariants anti-traversée et vitesse finie, le rebond unique/oblique restant vérifié par le test autonome `coop_solo_world_bridge_regressions.cpp`. Commit `a85ca0dbd8e94e463d11e42b5b2cefb32ab31116`, CI `38963255438` en attente.
