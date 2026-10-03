# Shared Kinetic Shield Design — 2026-10-03

- One kinetic core for classic and coop; `src/main.cpp` stays historical.
- Relative velocity is object minus ship; normal impact speed drives `mass * speed^2`.
- Stationary impacts retain a mass-only factor of 1. Asteroid mass stays linear in area.
- Outer field is about two ship diameters and handles fast bodies; inner field is 1.25 diameters and handles medium bodies; slow bodies may pass.
- Kinetic dissipation consumes only a small percentage-equivalent of shared `nrj`; residual energy alone reaches the existing linear energy shield.
- Red dust reveals the active rings and vibrates while crossing them; it never recursively creates a kinetic hit.
- Boss kinetic damage exists only during an explicit charge. Boss mass rises continuously from 1.0 to 2.8 over 200 encounters; ordinary contact keeps its historical damage.
- Hull regeneration is continuous with available reserve and receives a strong near-full bonus: `6*E^2 + 18*smoothstep(.9,1,E)` PV/s.
- Coop incoming multiplier remains x15 exactly once.
