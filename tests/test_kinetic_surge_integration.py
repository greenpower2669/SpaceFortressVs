from pathlib import Path

root=Path(__file__).resolve().parents[1]
kinetic=(root/'src/kinetic_shield.hpp').read_text()
field=(root/'src/legacy_field_runtime.hpp').read_text()
classic=(root/'src/remaster_runtime.hpp').read_text()
coop=(root/'src/campaign_runtime.hpp').read_text()
tactical=(root/'src/tactical_runtime.hpp').read_text()
ui=(root/'src/start_ui.hpp').read_text()
prepare=(root/'scripts/prepare-assets.py').read_text()

assert 'SF_KINETIC_MAX_SHIELD_DIAMETER = 2.0f' in kinetic
assert 'SF_KINETIC_SURGE_VISIBLE_DELAY_SECONDS = .30f' in kinetic
assert 'SF_KINETIC_SURGE_HOLD_SECONDS = 2.0f' in kinetic
assert 'SF_KINETIC_SURGE_BLAST_DIAMETER = 3.0f' in kinetic
assert 'SF_KINETIC_SURGE_POWER_MULTIPLIER = 2.0f' in kinetic
assert 'sfKineticSurgePress' in kinetic
assert 'sfKineticSurgeRelease' in kinetic
assert 'sfKineticSurgePower' in kinetic
assert 's.heldSeconds>=SF_KINETIC_SURGE_VISIBLE_DELAY_SECONDS' in kinetic

assert 'sfKineticPurgeAsteroids' in field
assert 'SF_KINETIC_SURGE_BLAST_DIAMETER*.5f' in field
assert 'sfKineticTriggerWave(owner,SF_KINETIC_SURGE_BLAST_DIAMETER*.5f,1.0f)' in field

# The same shared surge state drives classic duel and coop/campaign input paths.
# Both classic duel owners must be wired independently to the shared 0.30/2.00 s state.
for owner in ('0','1'):
    assert f'sfKineticSurgePress({owner})' in classic
    assert f'sfKineticSurgeRelease({owner})' in classic
    assert f'sfKineticPurgeAsteroids({owner})' in classic
assert 'sfKineticSurgePress(owner)' in coop
assert 'sfKineticSurgeRelease(owner)' in coop
assert 'sfKineticPurgeAsteroids(owner)' in coop
assert 'sfNormalizeClassicShipScale' not in tactical
assert 'sfKineticSurgeVisible' in tactical

# The charged state remains deliberately vulnerable until release.
assert 'sfKineticSurgePower(owner)' in field
assert 'powerMultiplier=std::clamp(powerMultiplier,0.0f,SF_KINETIC_SURGE_POWER_MULTIPLIER)' in kinetic

# Shared packaged audio cues and iridescent charge feedback. The release cue is
# re-authored as the EMP blast through the text-safe Android asset transport.
assert 'kinetic_charge.wav' in tactical
assert 'kinetic_ready.wav' in tactical
assert 'kinetic_release.wav' in tactical
assert 'sfKineticRainbowColor' in tactical
assert (root/'assets/sounds/kinetic_release_emp.b64').exists()
assert 'kinetic_release_emp.b64' in prepare
assert "EMP_RELEASE_SHA256" in prepare

# Low energy must be visible as a kinetic warning without changing collision timing.
assert 'sfKineticEnergyWaveColor' in tactical
assert 'sfKineticEnergyFlashFactor' in tactical
assert 'SF_KINETIC_LOW_ENERGY_WARNING_FRACTION' in kinetic
assert 'SF_KINETIC_LOW_ENERGY_FLASH_HZ' in kinetic
assert 'sfKineticWaveEnergyFraction' in tactical

# Danger is a start-screen setting, defaults to ROCK N ROLL / x10 internally,
# and non-kinetic hostile damage uses the single canonical helper.
assert (root/'src/boss_danger.hpp').exists()
boss=(root/'src/boss_danger.hpp').read_text()
compact=boss.replace(' ','').replace('\n','')
assert '1.0f,5.0f,10.0f,15.0f,20.0f,25.0f,30.0f,35.0f,40.0f' in compact
assert 'sfBossDangerIndex=2' in compact
assert 'sfApplyHostileDanger(damage)' in coop
assert 'sfApplyHostileDanger(650.0f)' in coop
assert 'SF_COOP_INCOMING_DAMAGE_MULTIPLIER = 15.0f' not in coop
assert 'DANGER BOSS' in ui

# Kinetic residual/contact and asteroid paths must stay outside danger scaling.
contact=coop[coop.index('static void sfCoopBossContact'):coop.index('static void sfCoopAsteroidHurt')]
assert 'const float incoming=solved.residualDamage;' in contact
assert 'sfApplyHostileDanger(solved.residualDamage)' not in contact
asteroid=coop[coop.index('static void sfCoopAsteroidHurt'):coop.index('static void sfCoopMovePlayers')]
assert 'const float incoming=legacyDamage;' in asteroid
assert 'sfApplyHostileDanger' not in asteroid
assert 'sfApplyHostileDanger' not in kinetic
assert 'sfApplyHostileDanger' not in field

for name in ('kinetic_charge.wav','kinetic_ready.wav','kinetic_release.wav'):
    assert (root/'assets'/'sounds'/name).exists(), name

print('PASS: energy-fatigued kinetic field, dual-owner classic charge, warning visuals and shared two-second surge contract')
