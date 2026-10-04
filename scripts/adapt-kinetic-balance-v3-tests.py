from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]

def literal(path,old,new,count=1):
    p=ROOT/path
    text=p.read_text()
    if text.count(old)<count:
        raise AssertionError(f'{path}: missing {old[:100]!r}')
    p.write_text(text.replace(old,new,count))

# Historical shield/dust regressions now follow the approved resource priority.
literal('tests/restoration_regressions.hpp',
'''    setupTactics();Spritej1->nrj=30;Spritej1->pv=900;
    for(int i=0;i<29;++i) {auto *dust=new parts(Spritej1->x,Spritej1->y);dust->pv=600;particules.push_back(dust);}
    sfCollectDust();
    assert(std::abs(Spritej1->nrj-22.75f)<.02f && Spritej1->pv>908.0f && Spritej1->pv<=909.0f);''',
'''    setupTactics();Spritej1->nrj=30;Spritej1->pv=900;
    for(int i=0;i<31;++i) {auto *dust=new parts(Spritej1->x,Spritej1->y);dust->pv=600;particules.push_back(dust);}
    sfCollectDust();
    assert(Spritej1->nrj==0 && std::abs(Spritej1->pv-904.0f)<.02f);''')

# Exactly stationary direct overlaps no longer invent kinetic damage in either mode.
literal('tests/restoration_regressions.hpp',
        '    sfLegacyFieldStep(sfCoopAsteroidHurt);assert(Spritej1->pv<1000);',
        '    sfLegacyFieldStep(sfCoopAsteroidHurt);assert(Spritej1->pv==1000);')
literal('tests/restoration_regressions.hpp',
        '    sfLegacyFieldStep(nullptr);assert(Spritej1->pv<1000);',
        '    sfLegacyFieldStep(nullptr);assert(Spritej1->pv==1000);')

# White dust: reserve first; the first dust collected after full reserve heals strongly.
literal('tests/restoration_regressions.hpp',
'''    Spritej1->nrj=30;Spritej1->pv=900;
    for(int i=0;i<29;++i) {auto *dust=new parts(390,300);dust->pv=600;particules.push_back(dust);}
    sfCollectDust();
    assert(std::abs(Spritej1->nrj-22.75f)<.02f && Spritej1->pv<=909);''',
'''    Spritej1->nrj=30;Spritej1->pv=900;
    for(int i=0;i<31;++i) {auto *dust=new parts(390,300);dust->pv=600;particules.push_back(dust);}
    sfCollectDust();
    assert(Spritej1->nrj==0 && std::abs(Spritej1->pv-904.0f)<.02f);''')

# Direct asteroid test starts just above the hull and moves inward through it.
literal('tests/restoration_regressions.hpp',
'''    auto *impact=new sprite;impact->setv(390,400,20,20,0,0,1);impact->pv=1;sa1.push_back(impact);''',
'''    auto *impact=new sprite;impact->setv(390,390,20,20,0,0,1);impact->vx=0;impact->vy=10;impact->kineticStage=2;impact->pv=1;sa1.push_back(impact);''')
literal('tests/restoration_regressions.hpp',
'''    const float asteroidLegacyDamage=impact->w*impact->h*.05f;
    const float asteroidExpected=sfShieldDamage(asteroidLegacyDamage*SF_KINETIC_MASS_DAMAGE_FLOOR,25);''',
'''    const float asteroidExpected=sfShieldDamage(sfKineticRockSolution(impact,Spritej1,0).rawDamage,25);''')

# Red dust is now a visual tracer only: it persists and mutates no player stat.
literal('tests/restoration_regressions.hpp',
'''    // A boss hit makes red dust. Armed red dust never heals PV/energy: it adds heat,
    // i.e. weakens the shield, and leaves PV untouched by the pickup itself.''',
'''    // A boss hit makes red dust. Red dust is strictly visual: no PV, reserve or shield mutation.''')
literal('tests/restoration_regressions.hpp',
        '    assert(Spritej1->pv==pvBeforeRed && Spritej1->nrj>heatBeforeRed && red->pv==0);',
        '    assert(Spritej1->pv==pvBeforeRed && Spritej1->nrj==heatBeforeRed && red->pv>0);')

# Legacy field interaction must exercise a real inward asteroid impact, not a stationary overlap.
# Coop must also use the kinetic asteroid callback, never the boss/non-kinetic multiplier path.
literal('tests/legacy_field_regressions.cpp',
'''        Spritej1->setxywh(390,840,100,100);Spritej1->nrj=40;Spritej1->pv=1000;Spritej2->pv=0;
        fieldRock(390,840,100);
        sfLegacyFieldFrame(1.0f/60,coop ? sfCoopHurt : nullptr);
        assert(Spritej1->pv<1000 && Spritej1->nrj>40 && !particulesr.empty() && !explos.empty());''',
'''        Spritej1->setxywh(390,840,100,100);Spritej1->nrj=40;Spritej1->pv=1000;Spritej2->pv=0;
        auto *impact=fieldRock(390,790,100);
        impact->vy=sfKineticReferenceSpeed(sfArenaW)/60.0f;impact->kineticStage=2;
        sfLegacyFieldFrame(1.0f/60,coop ? sfCoopAsteroidHurt : nullptr);
        assert(Spritej1->pv<1000 && Spritej1->nrj>40 && !particulesr.empty() && !explos.empty());''')

# Keep v3 contracts in every future full regression run.
literal('scripts/test-regressions.sh',
'''python3 "$sf_repo/tests/test_kinetic_integration.py"
python3 "$sf_repo/tests/test_kinetic_surge_integration.py"
g++ -std=c++17 -O1 -I "$sf_repo/src" "$sf_repo/tests/kinetic_surge_regressions.cpp" -o "$sf_test_dir/kinetic-surge"
"$sf_test_dir/kinetic-surge"''',
'''python3 "$sf_repo/tests/test_kinetic_integration.py"
python3 "$sf_repo/tests/test_kinetic_surge_integration.py"
python3 "$sf_repo/tests/test_balance_v3_integration.py"
g++ -std=c++17 -O1 -I "$sf_repo/src" "$sf_repo/tests/kinetic_surge_regressions.cpp" -o "$sf_test_dir/kinetic-surge"
"$sf_test_dir/kinetic-surge"
g++ -std=c++17 -O1 -I "$sf_repo/src" "$sf_repo/tests/kinetic_balance_v3_regressions.cpp" -o "$sf_test_dir/kinetic-balance-v3"
"$sf_test_dir/kinetic-balance-v3"''')

print('Adapted historical regressions to kinetic balance v3 canon')