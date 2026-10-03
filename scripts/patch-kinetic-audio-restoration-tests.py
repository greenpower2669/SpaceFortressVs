#!/usr/bin/env python3
from pathlib import Path
p=Path('tests/restoration_regressions.hpp')
t=p.read_text()

def rep(old,new,label):
    global t
    n=t.count(old)
    if n!=1: raise SystemExit(f'{label}: expected 1 match, got {n}')
    t=t.replace(old,new,1)

rep('''    sfKineticResetSurges();sfKineticSurgePress(0);\n    sfKineticAdvanceSurges(SF_KINETIC_SURGE_HOLD_SECONDS-.01f);\n    assert(!sfKineticSurgeVisible(0) && sfKineticSurgePower(0)==1.0f);\n    sfKineticAdvanceSurges(.02f);\n    assert(sfKineticSurgeVisible(0) && sfKineticSurgePower(0)==2.0f);\n    sfKineticAdvanceSurges(SF_KINETIC_SURGE_DURATION+.01f);\n    assert(!sfKineticSurgeVisible(0) && sfKineticSurgePower(0)==1.0f);\n    assert(sfKineticSurgeRelease(0));\n''','''    sfKineticResetSurges();sfKineticSurgePress(0);\n    sfKineticAdvanceSurges(SF_KINETIC_SURGE_HOLD_SECONDS-.01f);\n    assert(sfKineticSurgeVisible(0) && sfKineticSurgePower(0)==2.0f && !sfKineticSurgeVulnerable(0));\n    sfKineticAdvanceSurges(.02f);\n    assert(!sfKineticSurgeVisible(0) && sfKineticSurgePower(0)==0.0f && sfKineticSurgeVulnerable(0));\n    assert(sfKineticSurgeRelease(0));\n''','surge restoration')

rep('''    assert(Spritej1->pv==1000 && Spritej2->pv==850);\n    assert(std::abs(Spritej1->nrj-15.0f)<.01f && Spritej2->nrj==50);''','''    assert(Spritej1->pv==1000 && Spritej2->pv==900);\n    assert(std::abs(Spritej1->nrj-10.0f)<.01f && Spritej2->nrj==50);''','default x10 shield regression')

rep('''static void testCoopIncomingDamageMultiplier()\n{\n    for(bool ai:{false,true})''','''static void testCoopIncomingDamageMultiplier()\n{\n    sfBossDangerIndex=2;\n    assert(sfBossDangerMultiplier()==10.0f);\n    for(bool ai:{false,true})''','danger default setup')

t=t.replace('SF_COOP_INCOMING_DAMAGE_MULTIPLIER*sfShieldDamage(100,heat)','sfBossDangerMultiplier()*sfShieldDamage(100,heat)')
t=t.replace('SF_COOP_INCOMING_DAMAGE_MULTIPLIER*beamDamage','sfBossDangerMultiplier()*beamDamage')
t=t.replace('SF_COOP_INCOMING_DAMAGE_MULTIPLIER*waveDamage','sfBossDangerMultiplier()*waveDamage')
if 'SF_COOP_INCOMING_DAMAGE_MULTIPLIER' in t:
    # The remaining occurrence is the old x15 assertion/comment block, replaced next.
    pass
rep('''    // D-140-11 x15 remains only for the validated NON-KINETIC coop attack/contact paths.\n    assert(SF_COOP_INCOMING_DAMAGE_MULTIPLIER==15.0f);\n''','''    // Boss danger changes only the validated NON-KINETIC coop attack/contact paths.\n    assert(sfBossDangerMultiplier()==10.0f);\n    sfBossDangerAdjust(1);assert(sfBossDangerMultiplier()==15.0f);\n    sfBossDangerAdjust(1);assert(sfBossDangerMultiplier()==20.0f);\n    sfBossDangerAdjust(-1);assert(sfBossDangerMultiplier()==15.0f);\n    sfBossDangerIndex=2;assert(sfBossDangerMultiplier()==10.0f);\n''','danger selector restoration')
rep('''    std::puts("PASS: coop incoming damage is x15 and white/red dust economy is active for boss, special ammo and shields");''','''    std::puts("PASS: coop boss danger is configurable x1/x5/x10/x15/x20 while kinetic and dust economy stay separate");''','danger pass message')

if 'SF_KINETIC_SURGE_DURATION' in t: raise SystemExit('stale surge duration remains')
if 'SF_COOP_INCOMING_DAMAGE_MULTIPLIER' in t: raise SystemExit('stale fixed boss multiplier remains')
p.write_text(t)
print('PASS: restoration regressions adapted to two-second vulnerable surge and configurable boss danger')
