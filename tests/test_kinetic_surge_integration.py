from pathlib import Path

root=Path(__file__).resolve().parents[1]
kinetic=(root/'src/kinetic_shield.hpp').read_text()
field=(root/'src/legacy_field_runtime.hpp').read_text()
classic=(root/'src/remaster_runtime.hpp').read_text()
coop=(root/'src/campaign_runtime.hpp').read_text()
tactical=(root/'src/tactical_runtime.hpp').read_text()
ui=(root/'src/start_ui.hpp').read_text()

assert 'SF_KINETIC_MAX_SHIELD_DIAMETER = 2.0f' in kinetic
assert 'SF_KINETIC_SURGE_HOLD_SECONDS = 2.0f' in kinetic
assert 'SF_KINETIC_SURGE_POWER_MULTIPLIER = 2.0f' in kinetic
assert 'sfKineticSurgePress' in kinetic
assert 'sfKineticSurgeRelease' in kinetic
assert 'sfKineticSurgePower' in kinetic
assert 'sfKineticPurgeAsteroids' in field
assert 'sfKineticSurgePress(1)' in classic
assert 'sfKineticSurgeRelease(1)' in classic
assert 'sfKineticPurgeAsteroids(1)' in classic
assert 'sfKineticSurgePress(owner)' in coop
assert 'sfKineticSurgeRelease(owner)' in coop
assert 'sfKineticPurgeAsteroids(owner)' in coop
assert 'sfNormalizeClassicShipScale' not in tactical
assert 'sfKineticSurgeVisible' in tactical

# The charged state is deliberately vulnerable until release.
assert 'sfKineticSurgePower(owner)' in field
assert 'powerMultiplier=std::clamp(powerMultiplier,0.0f,SF_KINETIC_SURGE_POWER_MULTIPLIER)' in kinetic

# Shared synthetic/packaged audio cues and iridescent charge feedback.
assert 'kinetic_charge.wav' in tactical
assert 'kinetic_ready.wav' in tactical
assert 'kinetic_release.wav' in tactical
assert 'sfKineticRainbowColor' in tactical

# Boss danger is a start-screen setting, defaults to x10, and never re-enters kinetic damage.
assert (root/'src/boss_danger.hpp').exists()
boss=(root/'src/boss_danger.hpp').read_text()
assert '1.0f,5.0f,10.0f,15.0f,20.0f' in boss.replace(' ','')
assert 'sfBossDangerIndex=2' in boss.replace(' ','')
assert 'sfBossDangerMultiplier()' in coop
assert 'SF_COOP_INCOMING_DAMAGE_MULTIPLIER = 15.0f' not in coop
assert 'DANGER BOSS' in ui

for name in ('kinetic_charge.wav','kinetic_ready.wav','kinetic_release.wav'):
    assert (root/'assets'/'sounds'/name).exists(), name

print('PASS: two-second vulnerable kinetic surge, audio, rainbow feedback and boss danger selector are wired')
