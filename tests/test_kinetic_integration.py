from pathlib import Path
root=Path(__file__).resolve().parents[1]
k=(root/'src/kinetic_shield.hpp').read_text()
c=(root/'src/campaign_runtime.hpp').read_text()
l=(root/'src/legacy_field_runtime.hpp').read_text()
t=(root/'src/tactical_runtime.hpp').read_text()
main=(root/'src/main.cpp').read_text()
assert 'SF_KINETIC_COOP_DAMAGE_MULTIPLIER' not in k
assert 'SF_KINETIC_COOP_DAMAGE_MULTIPLIER' not in c
assert 'static_assert(SF_COOP_INCOMING_DAMAGE_MULTIPLIER' not in c
# Non-kinetic coop attacks use the player-selected boss danger multiplier.
assert 'const float incoming=sfBossDangerMultiplier()*damage;' in c
assert 'const float incomingPerSecond=sfBossDangerMultiplier()*650.0f;' in c
assert 'SF_COOP_INCOMING_DAMAGE_MULTIPLIER' not in c
# Kinetic asteroid and boss charge paths never use the boss danger multiplier.
assert 'static void sfCoopAsteroidHurt' in c and 'const float incoming=legacyDamage;' in c
charge=c[c.index('static void sfCoopBossContact'):c.index('static void sfCoopAsteroidHurt')]
assert 'sfBossDangerMultiplier()*solved.residualDamage' not in charge
assert 'const float incoming=solved.residualDamage;' in charge
# Both classic and coop share the legacy field resolver; no permanent pulse rings remain.
assert 'sfLegacyFieldFrame' in l and 'sfKineticTryLayer' in l
assert 'sfKineticWaves' in t and 'sfKineticWaveRadiusAt' in t
assert 'pulse.outer' not in t and 'pulse.inner' not in t
# White dust is observed for diagnostics only and is never fed through the physical response helper.
assert 'sfKineticRespondDust(false' in l
assert 'sfKineticRespondDust(true' not in l
assert 'type=white' in l and 'deflected=false' in l
assert 'KINETIC_IMPACT' in l and 'KINETIC_WAVE' in l and 'KINETIC_DUST' in l
assert 'KINETIC_IMPACT' in c
# Historical source is still not rewritten by this feature.
assert '#include <iostream>' in main
