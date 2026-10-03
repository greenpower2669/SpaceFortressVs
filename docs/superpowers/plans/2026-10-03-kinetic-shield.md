# Shared Kinetic Shield Implementation Plan

**Goal:** Add shared kinetic impact physics, charge-only boss kinetics, visual two-layer ripples, and reserve-driven hull regeneration to classic and coop.

**Architecture:** Pure kinetic math lives in `kinetic_shield.hpp`; `ship_energy.hpp` owns reserve/hull integration; the shared historical asteroid field owns interception/fragmentation; campaign owns boss charge state. Android classic remains a generated compatibility copy so `src/main.cpp` is untouched.

**Tests:** Pure vector/mass/speed/layer regression first; native integration covers classic+coop field, charge one-hit gating and hull regen; full existing native suite and generated Android source syntax must remain green.

**Constraints:** no merge, no release, no `src/main.cpp` edit, x15 once, white/red dust semantics preserved.
