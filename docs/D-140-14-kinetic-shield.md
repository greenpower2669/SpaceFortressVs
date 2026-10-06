# Shared Kinetic Shield Design — 2026-10-03

- One kinetic core for classic and coop; `src/main.cpp` stays historical.
- Relative velocity is object minus ship; normal impact speed drives `mass * speed^2`.
- Stationary impacts retain a mass-only factor of 1. Asteroid mass stays linear in area.
- Outer field reaches up to about two ship diameters and handles fast bodies; inner field is 1.25 diameters and handles medium bodies; slow bodies may pass.
- Kinetic dissipation consumes only a small percentage-equivalent of shared `nrj`; residual energy alone reaches the existing linear energy shield.
- Red dust reveals the active rings and vibrates while crossing them; it never recursively creates a kinetic hit. Historical white resource dust is never deflected by the kinetic field.
- Boss kinetic damage exists only during an explicit charge. Boss mass rises continuously from 1.0 to 2.8 over 200 encounters; ordinary contact keeps its historical damage.
- Hull regeneration is continuous with available reserve and receives a strong near-full bonus: `6*E^2 + 18*smoothstep(.9,1,E)` PV/s.
- Coop incoming multiplier remains x15 exactly once.

## Two-finger kinetic surge

- Shared by classic and coop.
- A short second-finger tap keeps the historical firing action, now resolved on release.
- Holding the second finger for 0.35 s activates the surge; kinetic dissipation is doubled for at most 2.0 s.
- Releasing after activation purges asteroids currently inside the maximum kinetic zone into the historical white resource-dust effect.
- Normal absorption waves are deliberately more transparent; the surge keeps a visible pulsing inner/outer aura.
- Classic ship dimensions remain historical: only the kinetic field diameter expands.
- Native targeted tests and the full regression suite passed before gameplay commit `c646db199e2aadeb0d99e554ada5a0b0ea06b697`.
- No merge to `main` and no release before Fab phone validation.
