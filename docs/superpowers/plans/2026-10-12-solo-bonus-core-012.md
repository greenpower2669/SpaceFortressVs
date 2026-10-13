# SOLO BONUS CORE-012 Implementation Plan

> For agentic workers: use superpowers:executing-plans. Astra implémente ; Sol vérifie indépendamment les tests/Android conformément à la mission.

**Goal:** Huit bonus réels sur le moteur COOP, collectables dans le labyrinthe et l’arène SOLO.
**Architecture:** État bonus par SfCoopState ; adaptateur simulation et renderer commun aux deux vues. Aucune dépendance vers l’ancien bonus à tourelles.
**Tech Stack:** C++17, SDL2, Android ABI arm64-v8a existants.
**Spec:** ../specs/2026-10-12-solo-bonus-core-012.md

## Global Constraints

Pas de main/src/main.cpp, merge main ou Release. Météorites nouvelles METEOR-009 suspendues. Contrôles/caméra/cône historiques préservés. Cinq mémoires dans le même commit fonctionnel. Pas de faux tests ni promesse de validation Samsung.

## Review Focus

- Plusieurs seuils traversés sur un seul impact : chaque paquet une fois.
- Boss retiré/recréé : pas de statut ou projectile ancien sur le nouveau boss.
- Cloison entre bonus et joueur : emplacement accessible et collecte sans traversée.
- Missiles+orbes+furtivité simultanés : coût et compteurs stables, timers en pause gelés.
- Renderer recréé/relaunch : textures détruites, aucun état ni alpha partagé résiduel.

## Tâches

- [x] 1. Sol écrit tests/solo_bonus_core_regressions.cpp et observe absence du module ; Astra implémente src/solo_bonus_core.hpp avec API State::grant/advance/drops/collect/consumeSalvo/reset. Tests précis dans le fichier de Sol, valeurs du contrat ci-dessus.
- [x] 2. Ajouter tests/solo_bonus_runtime_regressions.hpp dans tests/regressions.cpp. Adapter campaign_runtime.hpp : état/tag, tir humain, vrai guidage, targeting furtif, mouvement boss avec statuts, boucle ressources/loot. Nouveau coop_solo_bonus_runtime.hpp pour les fonctions bornées. Vérifier les chemins historiques non-SOLO inchangés.
- [x] 3. Ajouter géométrie de segment et placement connecté dans coop_solo_world_bridge.hpp, tests contact/large coque/cul-de-sac/world bounds ; brancher les callbacks dans coop_solo_arena_runtime.hpp.
- [x] 4. Ajouter coop_solo_bonus_draw.hpp, réutiliser PNG inspectés et vraie animation du vaisseau ; brancher arène/monde, HUD, cleanup textures. Tests SDL et smoke réel.
- [x] 5. Sol revoit les huit effets intégrés, exécute régressions complètes disponibles, vérifie source Android générée ; corriger causes sans affaiblir assertions.
- [x] 6. Actualiser mémoires, commit/push sous lease de branche, CI Android exacte ; exporter APK/AAB dans nouveau dossier après succès, vérifier hashes et source ; remettre liens et limites matérielles.

Commandes de vérification : scripts/test-regressions.sh ; C++ pur nouveau test avec g++ -std=c++17 -Isrc ; workflow android-build.yml pour la branche puis solo-coop-export-to-temp.yml avec run+SHA exacts et nouveau dossier.
