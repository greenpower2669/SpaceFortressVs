from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]

def replace(path, old, new, count=1):
    p=ROOT/path
    text=p.read_text()
    actual=text.count(old)
    if actual < count:
        raise AssertionError(f'{path}: expected at least {count} occurrences, found {actual}: {old[:80]!r}')
    text=text.replace(old,new,count)
    p.write_text(text)

def replace_all(path, old, new):
    p=ROOT/path
    text=p.read_text()
    if old not in text:
        raise AssertionError(f'{path}: missing {old[:80]!r}')
    p.write_text(text.replace(old,new))

# ---------------------------------------------------------------------------
# Boss danger: named selector, coefficients remain internal.
# ---------------------------------------------------------------------------
(ROOT/'src/boss_danger.hpp').write_text('''#pragma once

inline constexpr float SF_BOSS_DANGER_LEVELS[5]={1.0f,5.0f,10.0f,15.0f,20.0f};
inline constexpr const char *SF_BOSS_DANGER_NAMES[5]={
    "MOU DU GENOU","CHILL","ROCK N ROLL","DUR A CUIRE","MACHINE DE GUERRE"
};
inline int sfBossDangerIndex=2; // ROCK N ROLL / x10 is the canonical default.

static float sfBossDangerMultiplier()
{
    sfBossDangerIndex=std::clamp(sfBossDangerIndex,0,4);
    return SF_BOSS_DANGER_LEVELS[sfBossDangerIndex];
}
static const char *sfBossDangerName()
{
    sfBossDangerIndex=std::clamp(sfBossDangerIndex,0,4);
    return SF_BOSS_DANGER_NAMES[sfBossDangerIndex];
}
static void sfBossDangerNext()
{
    sfBossDangerIndex=(sfBossDangerIndex+1)%5;
}
static void sfBossDangerAdjust(int direction)
{
    if(direction<0 && sfBossDangerIndex>0) --sfBossDangerIndex;
    if(direction>0 && sfBossDangerIndex<4) ++sfBossDangerIndex;
}
''')

# ---------------------------------------------------------------------------
# Linear kinetic core: 25% max hull reference, no velocity-squared floor.
# ---------------------------------------------------------------------------
replace('src/kinetic_shield.hpp',
'''inline constexpr float SF_KINETIC_MASS_DAMAGE_FLOOR = .01f;
inline constexpr float SF_KINETIC_ENERGY_COST_SCALE = .00001f;''',
'''inline constexpr float SF_KINETIC_MASS_DAMAGE_FLOOR = .01f; // retained for save/test compatibility only
inline constexpr float SF_KINETIC_ASTEROID_MAX_HULL_DAMAGE = 250.0f;
inline constexpr float SF_KINETIC_ENERGY_COST_SCALE = .00001f;''')
replace('src/kinetic_shield.hpp',
'''    const float impactRatio=out.impactSpeed/referenceSpeed;
    out.relativeRatio=out.relativeSpeed/referenceSpeed;
    out.massFactor=std::max(0.0f,massFactor);
    // A tiny mass-only floor keeps an exactly static overlap finite, while actual closing speed dominates.
    out.speedFactor=SF_KINETIC_MASS_DAMAGE_FLOOR+impactRatio*impactRatio;
    out.rawDamage=std::max(0.0f,baseDamage)*out.massFactor*out.speedFactor;''',
'''    out.relativeRatio=out.relativeSpeed/referenceSpeed;
    out.massFactor=std::max(0.0f,massFactor);
    const float speedRatio=std::clamp(out.relativeRatio,0.0f,1.0f);
    const float closingFactor=out.relativeSpeed>.0001f
        ? std::clamp(out.impactSpeed/out.relativeSpeed,0.0f,1.0f) : 0.0f;
    // Linear kinetic damage: mass x relative speed x actual inward/closing component.
    out.speedFactor=speedRatio*closingFactor;
    out.rawDamage=std::max(0.0f,baseDamage)*out.massFactor*out.speedFactor;''')

# ---------------------------------------------------------------------------
# Shared asteroid resolver + no same-frame kinetic/historical double hit.
# ---------------------------------------------------------------------------
replace('src/legacy_field_runtime.hpp',
'''    const float area=std::max(1.0f,rock->w*rock->h);
    const float refArea=sfKineticReferenceArea(sfArenaH);
    return sfResolveKinetic(refArea*.05f,sfKineticMassFactorFromArea(area,sfArenaH),
        {rock->vx*60.0f,rock->vy*60.0f},{sfObserved[owner].velocity.vx,sfObserved[owner].velocity.vy},''',
'''    const float area=std::max(1.0f,rock->w*rock->h);
    const float massRelative=std::clamp(sfKineticMassFactorFromArea(area,sfArenaH),0.0f,1.0f);
    return sfResolveKinetic(SF_KINETIC_ASTEROID_MAX_HULL_DAMAGE,massRelative,
        {rock->vx*60.0f,rock->vy*60.0f},{sfObserved[owner].velocity.vx,sfObserved[owner].velocity.vy},''')
replace('src/legacy_field_runtime.hpp',
'''static bool sfKineticTryLayer(sprite *rock,sprite *ship,int owner,SfKineticLayer layer,
                        const SfKineticSolution &solution)
{
    if(!rock || rock->pv<=0 || !ship) return false;
''',
'''static bool sfKineticTryLayer(sprite *rock,sprite *ship,int owner,SfKineticLayer layer,
                        const SfKineticSolution &solution,bool *interacted=nullptr)
{
    if(interacted) *interacted=false;
    if(!rock || rock->pv<=0 || !ship) return false;
''')
replace('src/legacy_field_runtime.hpp',
'''    if(solved.dissipationFraction<=.001f) return false;
    sfAddShipHeat(ship,solved.energyCost);''',
'''    if(solved.dissipationFraction<=.001f) return false;
    if(interacted) *interacted=true;
    sfAddShipHeat(ship,solved.energyCost);''')
replace('src/legacy_field_runtime.hpp',
'''  if(rock->kineticStage==0 && raw.suggestedLayer==SfKineticLayer::Outer && travelled<=outer) {
      rock->kineticStage=1;
      if(sfKineticTryLayer(rock,ship,owner,SfKineticLayer::Outer,raw)) continue;
  }
  if(rock->pv<=0) continue;
  if(rock->kineticStage<2 && raw.selectedRange>0 && travelled<=inner) {
      rock->kineticStage=2;
      if(sfKineticTryLayer(rock,ship,owner,SfKineticLayer::Inner,raw)) continue;
  }
''',
'''  bool interacted=false;
  if(rock->kineticStage==0 && raw.suggestedLayer==SfKineticLayer::Outer && travelled<=outer) {
      rock->kineticStage=1;
      if(sfKineticTryLayer(rock,ship,owner,SfKineticLayer::Outer,raw,&interacted)) continue;
      if(interacted) continue;
  }
  if(rock->pv<=0) continue;
  interacted=false;
  if(rock->kineticStage<2 && raw.selectedRange>0 && travelled<=inner) {
      rock->kineticStage=2;
      if(sfKineticTryLayer(rock,ship,owner,SfKineticLayer::Inner,raw,&interacted)) continue;
      if(interacted) continue;
  }
''')

# ---------------------------------------------------------------------------
# Red dust becomes purely visual in coop.
# ---------------------------------------------------------------------------
replace('src/campaign_runtime.hpp',
'''static constexpr float SF_COOP_BOSS_DUST_HEAL_FRACTION = .0015f;
static constexpr float SF_COOP_RED_DUST_HEAT = .08f;
static constexpr float SF_COOP_RED_DUST_ARMED_PV = 560.0f;
''',
'''static constexpr float SF_COOP_BOSS_DUST_HEAL_FRACTION = .0015f;
''')
start='''static void sfCoopCollectRedDust()
{
    if (sfCoop.phase!=SfCoopPhase::Combat) return;
    for(auto *dust:particulesr) {
        if (dust->pv<=0 || dust->pv>SF_COOP_RED_DUST_ARMED_PV) continue;
        const float value=std::clamp(dust->pv/600.0f,0.0f,1.0f);
        for(int owner=0;owner<2;++owner) {
            auto *ship=sfCoopShip(owner);
            if (ship->pv<=0) continue;
            if (vlong(dust->x-ship->x,dust->y-ship->y)>=sfCoopShipRadius()*.82f) continue;
            // Red dust is hostile: no PV/energy recharge. More nrj means less shield reserve.
            sfAddShipHeat(ship,SF_COOP_RED_DUST_HEAT*value);
            dust->pv=0;
            break;
        }
    }
}

'''
replace('src/campaign_runtime.hpp',start,'')
replace('src/campaign_runtime.hpp',
'''static void sfCoopResources(float dt)
{
    sfLegacyFieldFrame(dt,sfCoopAsteroidHurt);
    sfCoopCollectRedDust();
}''',
'''static void sfCoopResources(float dt)
{
    sfLegacyFieldFrame(dt,sfCoopAsteroidHurt);
}''')

# ---------------------------------------------------------------------------
# White dust: fill reserve exactly first, then strong hull recovery.
# ---------------------------------------------------------------------------
replace('src/tactical_runtime.hpp',
'''static void sfCollectDust()
{
''',
'''inline constexpr float SF_WHITE_DUST_ENERGY_RESTORE = 1.0f;
inline constexpr float SF_WHITE_DUST_FULL_ENERGY_HEAL = 4.0f;

static void sfCollectDust()
{
''')
replace('src/tactical_runtime.hpp',
'''            const float value=std::clamp(dust->pv*k0/600,0.0f,1.0f);
            // nrj is depletion/heat: a lower value means MORE available energy.
            winner->nrj=sfShipHeat(sfShipHeat(winner->nrj)-.25f*value);
            winner->pv=std::min(1000.0f,winner->pv+.30f*value);
            dust->pv=0; sfPickupGlow[owner]=.65f;''',
'''            const float value=std::clamp(dust->pv*k0/600,0.0f,1.0f);
            // nrj is depletion/heat: 0 means a genuinely full reserve.
            const float heatBefore=sfShipHeat(winner->nrj);
            if (heatBefore>0) winner->nrj=sfShipHeat(heatBefore-SF_WHITE_DUST_ENERGY_RESTORE*value);
            else winner->pv=std::min(1000.0f,winner->pv+SF_WHITE_DUST_FULL_ENERGY_HEAL*value);
            dust->pv=0; sfPickupGlow[owner]=.65f;''')

# ---------------------------------------------------------------------------
# Make every kinetic wave a conspicuous centre->outside moving front.
# ---------------------------------------------------------------------------
marker='''static void sfDrawKineticEffects(SDL_Renderer *renderer)
{'''
helper='''static Uint8 sfKineticWaveFrontAlpha(float progress,float strength)
{
    progress=std::clamp(progress,0.0f,1.0f);
    strength=std::clamp(strength,0.0f,1.0f);
    const float birth=std::clamp(progress/.055f,0.0f,1.0f);
    const float fade=std::pow(std::max(0.0f,1.0f-progress),.55f);
    return Uint8(std::clamp(245.0f*birth*fade*(.50f+.50f*strength),0.0f,245.0f));
}

static void sfDrawKineticEffects(SDL_Renderer *renderer)
{'''
replace('src/tactical_runtime.hpp',marker,helper)
replace('src/tactical_runtime.hpp',
'''        const float p=sfKineticWaveProgressAt(wave,wave.age);
        const float envelope=std::sin(float(PI)*p)*wave.strength;
        if(envelope<=.01f) continue;
        const SDL_Color team=wave.owner==0 ? SDL_Color{255,188,96,255} : SDL_Color{96,210,255,255};
        sfTacticalRing(renderer,tupl(ship->x,ship->y),radius,
            SDL_Color{team.r,team.g,team.b,Uint8(std::clamp(105.0f*envelope,0.0f,135.0f))});''',
'''        const float p=sfKineticWaveProgressAt(wave,wave.age);
        const Uint8 frontAlpha=sfKineticWaveFrontAlpha(p,wave.strength);
        if(frontAlpha<3) continue;
        if(sfKineticSurgeVisible(wave.owner)) {
            const float phase=std::fmod(float(SDL_GetTicks64())*.00042f+wave.owner*.17f+p*.55f,1.0f);
            sfTacticalRainbowRing(renderer,tupl(ship->x,ship->y),radius,phase,frontAlpha);
            sfTacticalRainbowRing(renderer,tupl(ship->x,ship->y),std::max(1.0f,radius-3.0f),phase+.16f,Uint8(frontAlpha*.72f));
            sfTacticalRainbowRing(renderer,tupl(ship->x,ship->y),std::max(1.0f,radius-6.0f),phase+.31f,Uint8(frontAlpha*.38f));
        } else {
            const SDL_Color team=wave.owner==0 ? SDL_Color{255,188,96,255} : SDL_Color{96,210,255,255};
            sfTacticalRing(renderer,tupl(ship->x,ship->y),radius,SDL_Color{team.r,team.g,team.b,frontAlpha});
            sfTacticalRing(renderer,tupl(ship->x,ship->y),std::max(1.0f,radius-3.0f),SDL_Color{team.r,team.g,team.b,Uint8(frontAlpha*.68f)});
            sfTacticalRing(renderer,tupl(ship->x,ship->y),std::max(1.0f,radius-6.0f),SDL_Color{255,255,255,Uint8(frontAlpha*.30f)});
        }''')

# ---------------------------------------------------------------------------
# Home selector: every click cycles one step, never launches the battle.
# ---------------------------------------------------------------------------
replace('src/start_ui.hpp',
'''    const std::string dangerText = std::string("< DANGER BOSS : X") +
        std::to_string(int(sfBossDangerMultiplier())) + " >";''',
'''    const std::string dangerText = std::string("< DANGER BOSS : ") + sfBossDangerName() + " >";''')
replace('src/start_ui.hpp','''            sfBossDangerAdjust(x < .5f ? -1 : 1);''','''            sfBossDangerNext();''')
replace('src/remaster_ai_fix.hpp',
'''                if (y >= 0.39f && y <= 0.52f) {
                    sfSelectedMode=(sfSelectedMode+1)%4;
                    sfFixRequestedIa.store(sfModeHasAi(sfSelectedMode));
                } else if (y >= 0.545f && y <= 0.69f) {
                    sfFixLaunchPending.store(true);
                } else if (y >= 0.71f && y <= 0.85f) {
                    sfFixRequestedScreen.store(SF_UI_HELP);
                }''',
'''                if (y >= 0.39f && y <= 0.505f) {
                    sfSelectedMode=(sfSelectedMode+1)%4;
                    sfFixRequestedIa.store(sfModeHasAi(sfSelectedMode));
                } else if (y >= 0.515f && y <= 0.60f) {
                    sfBossDangerNext();
                } else if (y >= 0.615f && y <= 0.715f) {
                    sfFixLaunchPending.store(true);
                } else if (y >= 0.735f && y <= 0.85f) {
                    sfFixRequestedScreen.store(SF_UI_HELP);
                }''')

# ---------------------------------------------------------------------------
# Adapt historical regressions to the newly approved canon.
# ---------------------------------------------------------------------------
replace('tests/kinetic_regressions.cpp',
'''    assert(still.rawDamage>0 && still.rawDamage<5.0f);''',
'''    assert(still.rawDamage<.001f);''')
replace('tests/kinetic_regressions.cpp',
'''    const auto pass=sfResolveKinetic(100.0f,20.0f,{0,ref*.05f},{0,0},0,-1,ref);
    assert(pass.selectedRange==0 && pass.suggestedLayer==SfKineticLayer::None && pass.rawDamage<30.0f);''',
'''    const auto pass=sfResolveKinetic(100.0f,1.0f,{0,ref*.05f},{0,0},0,-1,ref);
    assert(pass.selectedRange==0 && pass.suggestedLayer==SfKineticLayer::None && pass.rawDamage<10.0f);''')

replace_all('tests/restoration_regressions.hpp',
'''    for(int i=0;i<29;++i) {auto *dust=new parts(Spritej1->x,Spritej1->y);dust->pv=600;particules.push_back(dust);}
    sfCollectDust();
    assert(std::abs(Spritej1->nrj-22.75f)<.02f && Spritej1->pv>908.0f && Spritej1->pv<=909.0f);''',
'''    for(int i=0;i<31;++i) {auto *dust=new parts(Spritej1->x,Spritej1->y);dust->pv=600;particules.push_back(dust);}
    sfCollectDust();
    assert(Spritej1->nrj==0 && std::abs(Spritej1->pv-904.0f)<.02f);''')
replace_all('tests/restoration_regressions.hpp',
'''    sfLegacyFieldStep(sfCoopAsteroidHurt);assert(Spritej1->pv<1000);''',
'''    sfLegacyFieldStep(sfCoopAsteroidHurt);assert(Spritej1->pv==1000);''')
replace_all('tests/restoration_regressions.hpp',
'''    sfLegacyFieldStep(nullptr);assert(Spritej1->pv<1000);''',
'''    sfLegacyFieldStep(nullptr);assert(Spritej1->pv==1000);''')
replace('tests/restoration_regressions.hpp',
'''    for(int i=0;i<29;++i) {auto *dust=new parts(390,300);dust->pv=600;particules.push_back(dust);}
    sfCollectDust();
    assert(std::abs(Spritej1->nrj-22.75f)<.02f && Spritej1->pv<=909);''',
'''    for(int i=0;i<31;++i) {auto *dust=new parts(390,300);dust->pv=600;particules.push_back(dust);}
    sfCollectDust();
    assert(Spritej1->nrj==0 && std::abs(Spritej1->pv-904.0f)<.02f);''')
replace('tests/restoration_regressions.hpp',
'''    const float asteroidLegacyDamage=impact->w*impact->h*.05f;
    const float asteroidExpected=sfShieldDamage(asteroidLegacyDamage*SF_KINETIC_MASS_DAMAGE_FLOOR,25);''',
'''    const float asteroidExpected=sfShieldDamage(sfKineticRockSolution(impact,Spritej1,0).rawDamage,25);''')
replace('tests/restoration_regressions.hpp',
'''    // A boss hit makes red dust. Armed red dust never heals PV/energy: it adds heat,
    // i.e. weakens the shield, and leaves PV untouched by the pickup itself.''',
'''    // A boss hit makes red dust. Red dust is now strictly visual: no PV, energy or shield mutation.''')
replace('tests/restoration_regressions.hpp',
'''    assert(Spritej1->pv==pvBeforeRed && Spritej1->nrj>heatBeforeRed && red->pv==0);''',
'''    assert(Spritej1->pv==pvBeforeRed && Spritej1->nrj==heatBeforeRed && red->pv>0);''')

# Permanent inclusion of the new v3 RED->GREEN contracts in the full suite.
replace('scripts/test-regressions.sh',
'''python3 "$sf_repo/tests/test_kinetic_surge_integration.py"
g++ -std=c++17 -O1 -I "$sf_repo/src" "$sf_repo/tests/kinetic_surge_regressions.cpp" -o "$sf_test_dir/kinetic-surge"
"$sf_test_dir/kinetic-surge"''',
'''python3 "$sf_repo/tests/test_kinetic_surge_integration.py"
python3 "$sf_repo/tests/test_balance_v3_integration.py"
g++ -std=c++17 -O1 -I "$sf_repo/src" "$sf_repo/tests/kinetic_surge_regressions.cpp" -o "$sf_test_dir/kinetic-surge"
"$sf_test_dir/kinetic-surge"
g++ -std=c++17 -O1 -I "$sf_repo/src" "$sf_repo/tests/kinetic_balance_v3_regressions.cpp" -o "$sf_test_dir/kinetic-balance-v3"
"$sf_test_dir/kinetic-balance-v3"''')

# ---------------------------------------------------------------------------
# Living documentation.
# ---------------------------------------------------------------------------
notes={
'brain.md':'''\n## 2026-10-03 — Kinetic balance v3\nCanon partagé classique+coop : astéroïde max à vitesse de référence et plein face = 25% PV max avant bouclier énergétique; vitesse linéaire et composante de rapprochement. Poussière rouge strictement visuelle. Poussière blanche remplit d’abord la réserve jusqu’à 100%, puis soigne fortement. Toutes les vagues sont des fronts centre→extérieur très visibles. Difficulté boss affichée par noms seulement et cyclée à chaque clic.\n''',
'brainmap.md':'''\n- Kinetic balance v3 → cœur partagé duel/coop → dégâts linéaires 25% max → résidu comme phaser → red dust visuel → white dust énergie puis soin → vagues centre→extérieur renforcées → danger boss nommé/cyclique.\n''',
'debughistorical.md':'''\n## 2026-10-03 — Diagnostic sélecteur / poussières / cinétique\nLe bridge final `remaster_ai_fix.hpp` recouvrait la zone DANGER avec la zone lancement (`0.545..0.69`), expliquant qu’un toucher difficulté lance une battle. La poussière rouge coop appliquait encore `SF_COOP_RED_DUST_HEAT=.08`, expliquant la perte de réserve. La poussière blanche retirait seulement 0.25 de `nrj` par poussière et soignait simultanément 0.30 PV; elle ne pouvait donc pas produire le cycle énergie pleine puis soin fort demandé. La formule cinétique conservait `impactRatio²` et un plancher de masse; remplacés par masse×vitesse×rapprochement linéaires.\n''',
'todo.md':'''\n## Kinetic balance v3 — 2026-10-03\n- [x] Sélecteur danger par noms; chaque clic cycle, aucun clic ne lance une battle.\n- [x] Astéroïdes : formule linéaire, maximum 25% PV max au cas de référence, résidu traité via bouclier énergétique.\n- [x] Empêcher le double comptage interception cinétique + collision historique immédiate.\n- [x] Poussière rouge purement visuelle; poussière blanche énergie à 100% puis soin fort.\n- [x] Fronts de vagues centre→extérieur beaucoup plus visibles, rendu partagé classique/coop.\n- [ ] Validation téléphone Fab : ressenti dégâts, recharge/soin, poussières, vagues et sélecteur.\n''',
'ordres-de-mission.md':'''\n## Mission — Kinetic balance v3 (2026-10-03)\nAppliquer aux deux modes le canon validé par Fab : sélecteur nommé cyclique sans lancement implicite; coefficients boss internes; cinétique astéroïdes linéaire plafonnée à 25% PV max au cas de référence; contact résiduel comme phaser; red dust 100% visuel; white dust énergie complète puis soin fort; toutes les vagues centre→extérieur renforcées. Aucun merge main/release avant test téléphone.\n'''
}
for path,append in notes.items():
    p=ROOT/path
    p.write_text(p.read_text()+append)

print('Applied kinetic balance v3 implementation and regression updates')
