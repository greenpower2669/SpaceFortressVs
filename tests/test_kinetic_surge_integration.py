from pathlib import Path

root=Path(__file__).resolve().parents[1]
kinetic=(root/'src/kinetic_shield.hpp').read_text()
field=(root/'src/legacy_field_runtime.hpp').read_text()
classic=(root/'src/remaster_runtime.hpp').read_text()
coop=(root/'src/campaign_runtime.hpp').read_text()
tactical=(root/'src/tactical_runtime.hpp').read_text()

assert 'SF_KINETIC_MAX_SHIELD_DIAMETER = 2.0f' in kinetic
assert 'SF_KINETIC_SURGE_HOLD_SECONDS' in kinetic
assert 'SF_KINETIC_SURGE_DURATION' in kinetic
assert 'SF_KINETIC_SURGE_POWER_MULTIPLIER' in kinetic
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
print('PASS: shared two-finger kinetic surge is wired in classic and coop')
