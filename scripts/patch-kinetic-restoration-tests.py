#!/usr/bin/env python3
from pathlib import Path
p=Path(__file__).resolve().parents[1]/'tests/restoration_regressions.hpp'
t=p.read_text()

def one(old,new,label):
    global t
    n=t.count(old)
    if n!=1: raise SystemExit(f'{label}: expected 1 occurrence, found {n}')
    t=t.replace(old,new,1)

one('const float outer=100*SF_KINETIC_OUTER_RADIUS_DIAMETERS;',
    'const float outer=100*SF_KINETIC_MAX_SHIELD_DIAMETER*.5f;',
    'outer range')
one('assert(Spritej1->pv==1000 && Spritej1->nrj>0 && sfKineticPulses[0].outer>0 && !particulesr.empty());',
    'assert(Spritej1->pv==1000 && Spritej1->nrj>0 && !sfKineticWaves.empty() && !particulesr.empty());',
    'asteroid wave')
one('assert(sfCoop.chargeHit[0] && afterHeat>0 && sfKineticPulses[0].outer>0);',
    'assert(sfCoop.chargeHit[0] && afterHeat>0 && !sfKineticWaves.empty());',
    'boss charge wave')
one('const float asteroidExpected=SF_COOP_INCOMING_DAMAGE_MULTIPLIER*sfShieldDamage(asteroidLegacyDamage,25);',
    'const float asteroidExpected=sfShieldDamage(asteroidLegacyDamage*SF_KINETIC_MASS_DAMAGE_FLOOR,25);',
    'kinetic asteroid no x15')
one('// D-140-11: Fab asks for another x3 over the verified x5 => x15 total.\n    assert(SF_COOP_INCOMING_DAMAGE_MULTIPLIER==15.0f);',
    '// D-140-11 x15 remains only for the validated NON-KINETIC coop attack/contact paths.\n    assert(SF_COOP_INCOMING_DAMAGE_MULTIPLIER==15.0f);',
    'x15 comment')
p.write_text(t)
