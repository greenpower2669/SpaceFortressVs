from pathlib import Path
root=Path(__file__).resolve().parents[1]
ui=(root/'src/start_ui.hpp').read_text()
fix=(root/'src/remaster_ai_fix.hpp').read_text()
danger=(root/'src/boss_danger.hpp').read_text()
campaign=(root/'src/campaign_runtime.hpp').read_text()
tactical=(root/'src/tactical_runtime.hpp').read_text()
legacy=(root/'src/legacy_field_runtime.hpp').read_text()
kinetic=(root/'src/kinetic_shield.hpp').read_text()

# The player sees names only; each click cycles the selector and never launches a match.
for label in ['MOU DU GENOU','CHILL','ROCK N ROLL','DUR A CUIRE','MACHINE DE GUERRE']:
    assert label in danger
assert 'sfBossDangerName()' in ui
assert 'sfBossDangerNext();' in ui
assert 'sfBossDangerNext();' in fix
home=fix[fix.index('if (requestedScreen == SF_UI_HOME)'):fix.index('} else if (requestedScreen == SF_UI_HELP')]
assert home.index('sfBossDangerNext();') < home.index('sfFixLaunchPending.store(true);')

# Red dust is visual-only: no heat/damage collector remains in coop.
assert 'SF_COOP_RED_DUST_HEAT' not in campaign
assert 'sfCoopCollectRedDust();' not in campaign

# White dust restores energy all the way to full first, then becomes a strong hull resource.
assert 'SF_WHITE_DUST_ENERGY_RESTORE' in tactical
assert 'SF_WHITE_DUST_FULL_ENERGY_HEAL' in tactical
assert 'if (heatBefore>0)' in tactical
assert 'else winner->pv=std::min(1000.0f,winner->pv+SF_WHITE_DUST_FULL_ENERGY_HEAL*value);' in tactical

# Asteroid kinetics are linear and capped at 25% of max hull at reference max speed.
assert 'SF_KINETIC_ASTEROID_MAX_HULL_DAMAGE = 250.0f' in kinetic
assert 'impactRatio*impactRatio' not in kinetic
assert 'closingFactor' in kinetic
assert 'std::clamp(sfKineticMassFactorFromArea(area,sfArenaH),0.0f,1.0f)' in legacy
assert 'SF_KINETIC_ASTEROID_MAX_HULL_DAMAGE' in legacy

# A field interception cannot immediately fall through into the historical hull collision.
assert 'bool interacted=false;' in legacy
assert 'if(interacted) continue;' in legacy

# Both modes use the same centre-to-outside wave renderer, now with stronger fronts.
assert 'sfDrawKineticEffects(renderer);' in campaign
assert 'sfKineticWaveFrontAlpha' in tactical
assert 'sfTacticalRing(renderer,tupl(ship->x,ship->y),std::max(1.0f,radius-3.0f)' in tactical

print('PASS: selector, neutral red dust, white resource priority, linear asteroid cap and shared visible waves are wired')
