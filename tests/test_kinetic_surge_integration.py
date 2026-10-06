from pathlib import Path

root=Path(__file__).resolve().parents[1]
kinetic=(root/'src/kinetic_shield.hpp').read_text()
field=(root/'src/legacy_field_runtime.hpp').read_text()
classic=(root/'src/remaster_runtime.hpp').read_text()
duel=(root/'src/classic_duel_surge.hpp').read_text()
visual=(root/'src/kinetic_energy_visuals.hpp').read_text()
classic_patch=(root/'scripts/patch-classic-danger.cmake').read_text()
coop=(root/'src/campaign_runtime.hpp').read_text()
tactical=(root/'src/tactical_runtime.hpp').read_text()
ui=(root/'src/start_ui.hpp').read_text()
prepare=(root/'scripts/prepare-assets.py').read_text()
th2=(root/'src/th2.h').read_text()

assert 'SF_KINETIC_MAX_SHIELD_DIAMETER = 2.0f' in kinetic
assert 'SF_KINETIC_SURGE_VISIBLE_DELAY_SECONDS = .30f' in kinetic
assert 'SF_KINETIC_SURGE_HOLD_SECONDS = 2.0f' in kinetic
assert 'SF_KINETIC_SURGE_BLAST_DIAMETER = 3.0f' in kinetic
assert 'SF_KINETIC_SURGE_POWER_MULTIPLIER = 2.0f' in kinetic
assert 'SF_KINETIC_ENERGY_FATIGUE_EXPONENT = 1.20f' in kinetic
assert 'sfKineticEffectiveEnergyFraction' in kinetic
assert 'sfKineticSurgePress' in kinetic
assert 'sfKineticSurgeRelease' in kinetic
assert 'sfKineticSurgePower' in kinetic
assert 's.heldSeconds>=SF_KINETIC_SURGE_VISIBLE_DELAY_SECONDS' in kinetic

assert 'sfKineticPurgeAsteroids' in field
assert 'SF_KINETIC_SURGE_BLAST_DIAMETER*.5f' in field
assert 'sfKineticTriggerWave(owner,SF_KINETIC_SURGE_BLAST_DIAMETER*.5f,1.0f)' in field

# The same shared surge state drives classic duel and coop/campaign input paths.
# AI duel keeps its owner-1 remaster path; local duel explicitly supports both owners.
assert 'sfKineticSurgePress(1)' in classic
assert 'sfKineticSurgeRelease(1)' in classic
assert 'sfKineticPurgeAsteroids(1)' in classic
for owner in ('0','1'):
    assert f'sfKineticSurgePress({owner})' in duel
    assert f'sfKineticSurgeRelease({owner})' in duel
    assert f'sfKineticPurgeAsteroids({owner})' in duel
assert 'sfClassicDuelSurgeHandleEvent' in duel
assert 'sfClassicDuelSurgeHandleEvent(e,tid,ty)' in classic_patch
assert '#include <classic_duel_surge.hpp>' in th2
assert 'sfKineticSurgePress(owner)' in coop
assert 'sfKineticSurgeRelease(owner)' in coop
assert 'sfKineticPurgeAsteroids(owner)' in coop
assert 'sfNormalizeClassicShipScale' not in tactical
assert 'sfKineticSurgeVisible' in tactical

# Leaving local duel while a second finger is held must cancel only the local-duel
# bridge state; it must not leak a charged shield into the next mode.
assert 'sfKineticSurgeCancel(owner)' in duel
assert 'sfKineticAudioCancel(owner)' in duel

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

# Low energy is a visual-only warning overlay; collision timing stays in the shared model.
assert 'SF_KINETIC_LOW_ENERGY_WARNING_FRACTION' in kinetic
assert 'SF_KINETIC_LOW_ENERGY_FLASH_HZ' in kinetic
assert 'SF_KINETIC_LOW_ENERGY_WAVE_DURATION' in kinetic
assert 'sfKineticWaveEnergyFraction' in visual
assert 'sfKineticEnergyWaveColor' in visual
assert 'sfKineticEnergyFlashFactor' in visual
assert 'sfDrawKineticEnergyWarningOverlay' in visual
assert 'sfDrawKineticEnergyWarningOverlay(renderer)' in classic_patch
assert '#include <kinetic_energy_visuals.hpp>' in th2
# Android runtime waves must capture the true ship reserve, not re-infer it from
# layer strength (which can be affected by surge/layer choice).
assert '#define sfKineticTriggerWave(owner,radius,strength)' in th2
assert 'sfKineticEnergyFraction' in th2

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
