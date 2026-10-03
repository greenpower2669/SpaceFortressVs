#!/usr/bin/env python3
from pathlib import Path

root=Path(__file__).resolve().parents[1]

def replace_once(rel,old,new,label):
    p=root/rel
    text=p.read_text()
    count=text.count(old)
    if count!=1:
        raise SystemExit(f'{label}: expected 1 occurrence, found {count}')
    p.write_text(text.replace(old,new,1))

def append_once(rel,marker,block):
    p=root/rel
    text=p.read_text()
    if marker in text:
        return
    p.write_text(text.rstrip()+"\n\n"+block.rstrip()+"\n")

# ------------------------------------------------------------------
# Shared kinetic core
# ------------------------------------------------------------------
replace_once('src/kinetic_shield.hpp',
'''constexpr float SF_KINETIC_MAX_SHIELD_DIAMETER = 1.58f;\nconstexpr float SF_KINETIC_INNER_MAX_RADIUS_SHIP_DIAMETERS = .575f;\nconstexpr float SF_KINETIC_BOSS_BASE_DAMAGE = 50.0f;\nconstexpr float SF_KINETIC_WAVE_DURATION = .27f;''',
'''constexpr float SF_KINETIC_MAX_SHIELD_DIAMETER = 2.0f;\nconstexpr float SF_KINETIC_INNER_MAX_RADIUS_SHIP_DIAMETERS = .575f;\nconstexpr float SF_KINETIC_BOSS_BASE_DAMAGE = 50.0f;\nconstexpr float SF_KINETIC_WAVE_DURATION = .27f;\nconstexpr float SF_KINETIC_SURGE_HOLD_SECONDS = .35f;\nconstexpr float SF_KINETIC_SURGE_DURATION = 2.0f;\nconstexpr float SF_KINETIC_SURGE_POWER_MULTIPLIER = 2.0f;''',
'kinetic max diameter and surge constants')

replace_once('src/kinetic_shield.hpp',
'''inline std::vector<SfKineticWave> sfKineticWaves;\ninline unsigned sfKineticWaveSerial=0;\n\nstatic float sfKineticReferenceSpeed(float arenaWidth)''',
'''inline std::vector<SfKineticWave> sfKineticWaves;\ninline unsigned sfKineticWaveSerial=0;\nstruct SfKineticSurgeState {\n    bool held=false,charged=false;\n    float heldSeconds=0,boostSeconds=0;\n};\ninline std::array<SfKineticSurgeState,2> sfKineticSurges{};\n\nstatic void sfKineticSurgePress(int owner)\n{\n    if(owner<0 || owner>1) return;\n    auto &s=sfKineticSurges[owner];\n    s=SfKineticSurgeState{};s.held=true;\n}\nstatic void sfKineticSurgeCancel(int owner)\n{\n    if(owner<0 || owner>1) return;\n    sfKineticSurges[owner]=SfKineticSurgeState{};\n}\nstatic bool sfKineticSurgeRelease(int owner)\n{\n    if(owner<0 || owner>1) return false;\n    const bool purge=sfKineticSurges[owner].charged;\n    sfKineticSurges[owner]=SfKineticSurgeState{};\n    return purge;\n}\nstatic void sfKineticAdvanceSurges(float dt)\n{\n    if(dt<=0) return;\n    for(auto &s:sfKineticSurges) {\n        if(!s.held) continue;\n        s.heldSeconds+=dt;\n        if(!s.charged && s.heldSeconds>=SF_KINETIC_SURGE_HOLD_SECONDS) {\n            s.charged=true;s.boostSeconds=0;\n        } else if(s.charged && s.boostSeconds<SF_KINETIC_SURGE_DURATION) {\n            s.boostSeconds=std::min(SF_KINETIC_SURGE_DURATION,s.boostSeconds+dt);\n        }\n    }\n}\nstatic float sfKineticSurgePower(int owner)\n{\n    if(owner<0 || owner>1) return 1.0f;\n    const auto &s=sfKineticSurges[owner];\n    return s.held && s.charged && s.boostSeconds<SF_KINETIC_SURGE_DURATION ?\n        SF_KINETIC_SURGE_POWER_MULTIPLIER : 1.0f;\n}\nstatic bool sfKineticSurgeVisible(int owner)\n{\n    if(owner<0 || owner>1) return false;\n    const auto &s=sfKineticSurges[owner];\n    return s.held && s.charged && s.boostSeconds<SF_KINETIC_SURGE_DURATION;\n}\nstatic void sfKineticResetSurges()\n{\n    sfKineticSurges={};\n}\nstatic int sfKineticPurgeAsteroids(int owner);\n\nstatic float sfKineticReferenceSpeed(float arenaWidth)''',
'surge state and helpers')

replace_once('src/kinetic_shield.hpp',
'''static SfKineticSolution sfApplyKineticLayer(SfKineticSolution out,SfKineticLayer layer,float energyFraction)\n{\n    energyFraction=std::clamp(energyFraction,0.0f,1.0f);\n    const float maximum=layer==SfKineticLayer::Outer ? SF_KINETIC_OUTER_DISSIPATION :\n                        layer==SfKineticLayer::Inner ? SF_KINETIC_INNER_DISSIPATION : 0.0f;\n    out.appliedLayer=layer;\n    out.dissipationFraction=std::clamp(maximum*energyFraction,0.0f,.97f);''',
'''static SfKineticSolution sfApplyKineticLayer(SfKineticSolution out,SfKineticLayer layer,float energyFraction,float powerMultiplier=1.0f)\n{\n    energyFraction=std::clamp(energyFraction,0.0f,1.0f);\n    powerMultiplier=std::clamp(powerMultiplier,1.0f,SF_KINETIC_SURGE_POWER_MULTIPLIER);\n    const float maximum=layer==SfKineticLayer::Outer ? SF_KINETIC_OUTER_DISSIPATION :\n                        layer==SfKineticLayer::Inner ? SF_KINETIC_INNER_DISSIPATION : 0.0f;\n    out.appliedLayer=layer;\n    out.dissipationFraction=std::clamp(maximum*energyFraction*powerMultiplier,0.0f,.995f);''',
'surge power in kinetic layer')

# ------------------------------------------------------------------
# Asteroid field: boost + release purge into historical white dust
# ------------------------------------------------------------------
replace_once('src/legacy_field_runtime.hpp',
'''static void sfKineticEmitRedDust(int owner,int count,float ring)\n{\n    if(owner<0 || owner>1 || count<=0) return;\n    auto *ship=owner==0 ? Spritej1 : Spritej2;\n    for(int i=0;i<count && particulesr.size()<1000;++i) {\n        const float a=(i+.5f)*2*float(PI)/count+owner*.41f;\n        auto *dust=new parts(ship->x+std::cos(a)*ring,ship->y+std::sin(a)*ring);\n        dust->pv=600;\n        dust->vx=std::cos(a)*sfArenaW*.72f;\n        dust->vy=std::sin(a)*sfArenaW*.72f;\n        particulesr.push_back(dust);\n    }\n}\n''',
'''static void sfKineticEmitRedDust(int owner,int count,float ring)\n{\n    if(owner<0 || owner>1 || count<=0) return;\n    auto *ship=owner==0 ? Spritej1 : Spritej2;\n    for(int i=0;i<count && particulesr.size()<1000;++i) {\n        const float a=(i+.5f)*2*float(PI)/count+owner*.41f;\n        auto *dust=new parts(ship->x+std::cos(a)*ring,ship->y+std::sin(a)*ring);\n        dust->pv=600;\n        dust->vx=std::cos(a)*sfArenaW*.72f;\n        dust->vy=std::sin(a)*sfArenaW*.72f;\n        particulesr.push_back(dust);\n    }\n}\nstatic int sfKineticPurgeAsteroids(int owner)\n{\n    if(owner<0 || owner>1) return 0;\n    auto *ship=owner==0 ? Spritej1 : Spritej2;\n    if(!ship || ship->pv<=0) return 0;\n    const float diameter=sfKineticShipDiameter(ship);\n    const float radius=diameter*SF_KINETIC_MAX_SHIELD_DIAMETER*.5f;\n    int purged=0;\n    for(auto *rock:sa1) {\n        if(!rock || rock->pv<=0) continue;\n        const float rockRadius=std::max(rock->w,rock->h)*.5f;\n        if(vlong(rock->x-ship->x,rock->y-ship->y)>radius+rockRadius) continue;\n        partsforiw(rock,ship); // Historical white resource dust: never deflected by the field.\n        sfFieldImpact(rock,ship);\n        rock->pv=0;++purged;\n    }\n    if(purged>0) {\n        sfKineticTriggerWave(owner,SF_KINETIC_MAX_SHIELD_DIAMETER*.5f,1.0f);\n        sfKineticEmitRedDust(owner,std::min(12,3+purged),diameter*.46f);\n        SDL_Log("KINETIC_PURGE owner=%d asteroids=%d maxRadius=%.3f",owner,purged,radius);\n    }\n    return purged;\n}\n''',
'asteroid purge')

replace_once('src/legacy_field_runtime.hpp',
'''const auto solved=sfApplyKineticLayer(raw,layer,sfKineticEnergyFraction(ship->nrj));''',
'''const auto solved=sfApplyKineticLayer(raw,layer,sfKineticEnergyFraction(ship->nrj),sfKineticSurgePower(owner));''',
'asteroid surge power')

# ------------------------------------------------------------------
# Tactical shared simulation + visuals + smaller classic hulls
# ------------------------------------------------------------------
replace_once('src/tactical_runtime.hpp',
'''static void sfTacticsReset()\n{\n    sfPilot=SfPilot{}; sfObserved={}; sfPickupGlow={}; sfTurrets={}; sfKineticWaves.clear(); sfKineticWaveSerial=0;''',
'''static constexpr float SF_CLASSIC_SHIP_DIAMETER_RATIO=.15f;\nstatic void sfNormalizeClassicShipScale()\n{\n    if(sfIsCoop() || sfUiScreen!=SF_UI_GAME) return;\n    const float target=std::max(1.0f,std::min(sfArenaW,sfArenaH)*SF_CLASSIC_SHIP_DIAMETER_RATIO);\n    for(auto *ship:{Spritej1,Spritej2}) {\n        if(!ship) continue;\n        const float current=std::max({ship->w,ship->h,ship->sw,ship->sh});\n        if(current<=target+.01f || current<=0) continue;\n        const float scale=target/current;\n        ship->w*=scale;ship->h*=scale;ship->sw*=scale;ship->sh*=scale;ship->startup();\n    }\n}\n\nstatic void sfTacticsReset()\n{\n    sfPilot=SfPilot{}; sfObserved={}; sfPickupGlow={}; sfTurrets={}; sfKineticWaves.clear(); sfKineticWaveSerial=0;sfKineticResetSurges();''',
'classic scale and surge reset')

replace_once('src/tactical_runtime.hpp',
'''    sfArenaW=std::max(1,viewport.w); sfArenaH=std::max(1,viewport.h);\n    const Uint64 now=SDL_GetTicks64();''',
'''    sfArenaW=std::max(1,viewport.w); sfArenaH=std::max(1,viewport.h);\n    sfNormalizeClassicShipScale();\n    const Uint64 now=SDL_GetTicks64();''',
'classic scale in frame')

replace_once('src/tactical_runtime.hpp',
'''    if (sfUiScreen==SF_UI_GAME) sfSceneSeconds+=sfFrameDt;\n    if (sfUiScreen==SF_UI_GAME && !setgui) { sfRegenerateHull(Spritej1,sfFrameDt); sfRegenerateHull(Spritej2,sfFrameDt); }''',
'''    if (sfUiScreen==SF_UI_GAME) sfSceneSeconds+=sfFrameDt;\n    if (sfUiScreen==SF_UI_GAME && !sfIsCoop()) sfKineticAdvanceSurges(sfFrameDt);\n    if (sfUiScreen==SF_UI_GAME && !setgui) { sfRegenerateHull(Spritej1,sfFrameDt); sfRegenerateHull(Spritej2,sfFrameDt); }''',
'classic surge simulation')

replace_once('src/tactical_runtime.hpp',
'''static void sfDrawKineticEffects(SDL_Renderer *renderer)\n{\n    if (!renderer || sfUiScreen!=SF_UI_GAME || sfKineticWaves.empty()) return;\n    SDL_BlendMode previous;SDL_GetRenderDrawBlendMode(renderer,&previous);\n    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);\n    for(const auto &wave:sfKineticWaves) {''',
'''static void sfDrawKineticEffects(SDL_Renderer *renderer)\n{\n    if (!renderer || sfUiScreen!=SF_UI_GAME) return;\n    const bool surgeVisible=sfKineticSurgeVisible(0) || sfKineticSurgeVisible(1);\n    if(sfKineticWaves.empty() && !surgeVisible) return;\n    SDL_BlendMode previous;SDL_GetRenderDrawBlendMode(renderer,&previous);\n    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);\n    for(int owner=0;owner<2;++owner) if(sfKineticSurgeVisible(owner)) {\n        const auto *ship=owner==0 ? Spritej1 : Spritej2;\n        if(!ship || ship->pv<=0) continue;\n        const float diameter=std::max(1.0f,std::max({ship->sw,ship->sh,ship->w,ship->h}));\n        const float pulse=.5f+.5f*std::sin(float(SDL_GetTicks64())*.010f+owner*1.7f);\n        const SDL_Color team=owner==0 ? SDL_Color{255,188,96,255} : SDL_Color{96,210,255,255};\n        const float outer=diameter*SF_KINETIC_MAX_SHIELD_DIAMETER*.5f;\n        const float inner=diameter*SF_KINETIC_INNER_MAX_RADIUS_SHIP_DIAMETERS;\n        sfTacticalRing(renderer,tupl(ship->x,ship->y),outer,SDL_Color{team.r,team.g,team.b,Uint8(78+34*pulse)});\n        sfTacticalRing(renderer,tupl(ship->x,ship->y),outer-2,SDL_Color{team.r,team.g,team.b,Uint8(36+18*pulse)});\n        sfTacticalRing(renderer,tupl(ship->x,ship->y),inner,SDL_Color{team.r,team.g,team.b,Uint8(58+26*pulse)});\n    }\n    for(const auto &wave:sfKineticWaves) {''',
'surge aura rendering')

replace_once('src/tactical_runtime.hpp',
'''SDL_Color{team.r,team.g,team.b,Uint8(std::clamp(190.0f*envelope,0.0f,220.0f))});''',
'''SDL_Color{team.r,team.g,team.b,Uint8(std::clamp(105.0f*envelope,0.0f,135.0f))});''',
'more transparent absorption waves')

# ------------------------------------------------------------------
# Classic second-finger long press; short tap still fires.
# ------------------------------------------------------------------
replace_once('src/remaster_runtime.hpp',
'''#include "scenic_mix.hpp"''',
'''#include "scenic_mix.hpp"\n#include "kinetic_shield.hpp"\n\nstatic bool sfFireMain(int owner,const tupl *target);''',
'classic forward declarations')

replace_once('src/remaster_runtime.hpp',
'''static SDL_FingerID sfRmJ2Finger = 0;\nstatic float sfRmJ2LastX = 0.0f;''',
'''static SDL_FingerID sfRmJ2Finger = 0;\nstatic SDL_FingerID sfRmKineticFinger = -1;\nstatic float sfRmJ2LastX = 0.0f;''',
'classic kinetic finger')

replace_once('src/remaster_runtime.hpp',
'''    sfRmTrackJ2 = false;\n    if (Spritej1) { Spritej1->ctrl = false; Spritej1->id = 100; }''',
'''    sfRmTrackJ2 = false;\n    sfRmKineticFinger=-1;sfKineticSurgeCancel(1);\n    if (Spritej1) { Spritej1->ctrl = false; Spritej1->id = 100; }''',
'classic home cancels surge')

replace_once('src/remaster_runtime.hpp',
'''        setgui = true;\n        sfRmTrackJ2 = false;''',
'''        setgui = true;\n        sfRmTrackJ2 = false;\n        sfRmKineticFinger=-1;sfKineticSurgeCancel(1);''',
'classic non-game cancels surge')

replace_once('src/remaster_runtime.hpp',
'''        const float py = event->tfinger.y * th;\n        if (setia && !hit1 && !hit2 && py > HEIGHT * 0.5f &&\n            Spritej2 && Spritej2->id == 100) {\n            sfRmTrackJ2 = true;\n            sfRmJ2Finger = fid;\n            sfRmUpdateJ2Velocity(event->tfinger, true);\n        }\n    } else if (event->type == SDL_FINGERMOTION) {\n        if (sfRmTrackJ2 && event->tfinger.fingerId == sfRmJ2Finger)\n            sfRmUpdateJ2Velocity(event->tfinger, false);\n    } else if (event->type == SDL_FINGERUP) {\n        const SDL_FingerID fid = event->tfinger.fingerId;''',
'''        const float py = event->tfinger.y * th;\n        if (setia && !hit1 && !hit2 && py > HEIGHT * 0.5f &&\n            Spritej2 && Spritej2->id == 100) {\n            if(sfRmTrackJ2 && fid!=sfRmJ2Finger && sfRmKineticFinger<0) {\n                sfRmKineticFinger=fid;sfKineticSurgePress(1);\n                event->type=SDL_USEREVENT;return result;\n            }\n            if(!sfRmTrackJ2) {\n                sfRmTrackJ2 = true;\n                sfRmJ2Finger = fid;\n                sfRmUpdateJ2Velocity(event->tfinger, true);\n            }\n        }\n    } else if (event->type == SDL_FINGERMOTION) {\n        if(event->tfinger.fingerId==sfRmKineticFinger) {event->type=SDL_USEREVENT;return result;}\n        if (sfRmTrackJ2 && event->tfinger.fingerId == sfRmJ2Finger)\n            sfRmUpdateJ2Velocity(event->tfinger, false);\n    } else if (event->type == SDL_FINGERUP) {\n        const SDL_FingerID fid = event->tfinger.fingerId;\n        if(fid==sfRmKineticFinger) {\n            const bool purge=sfKineticSurgeRelease(1);\n            if(purge) sfKineticPurgeAsteroids(1); else sfFireMain(1,nullptr);\n            sfRmKineticFinger=-1;event->type=SDL_USEREVENT;return result;\n        }''',
'classic second-finger surge gesture')

# ------------------------------------------------------------------
# Coop/campaign: same second-finger gesture and same kinetic power.
# ------------------------------------------------------------------
replace_once('src/campaign_runtime.hpp',
'''solved=sfApplyKineticLayer(raw,raw.suggestedLayer,sfKineticEnergyFraction(ship->nrj));''',
'''solved=sfApplyKineticLayer(raw,raw.suggestedLayer,sfKineticEnergyFraction(ship->nrj),sfKineticSurgePower(owner));''',
'boss charge surge power')

replace_once('src/campaign_runtime.hpp',
'''static void sfCoopTick(float dt)\n{\n    if (sfCoop.phase!=SfCoopPhase::Combat) return;\n    sfCoop.time+=dt;''',
'''static void sfCoopTick(float dt)\n{\n    if (sfCoop.phase!=SfCoopPhase::Combat) return;\n    sfKineticAdvanceSurges(dt);\n    sfCoop.time+=dt;''',
'coop surge simulation')

replace_once('src/campaign_runtime.hpp',
'''static void sfCampaignStart()\n{\n    sfLoadCampaign();sfCoop=SfCoopState{};''',
'''static void sfCampaignStart()\n{\n    sfLoadCampaign();sfCoop=SfCoopState{};sfKineticResetSurges();''',
'campaign start surge reset')

replace_once('src/campaign_runtime.hpp',
'''static void sfCampaignSuspend()\n{\n    for (auto &control : sfCoop.controls) {control.down=false;control.finger=-1;control.velocity.set(0,0);}\n    sfCoop.fireFingers.clear();''',
'''static void sfCampaignSuspend()\n{\n    for (auto &control : sfCoop.controls) {control.down=false;control.finger=-1;control.velocity.set(0,0);}\n    sfCoop.fireFingers.clear();sfKineticResetSurges();''',
'campaign suspend surge reset')

replace_once('src/campaign_runtime.hpp',
'''        for(auto i=sfCoop.fireFingers.begin();i!=sfCoop.fireFingers.end();) {\n            if(i->second==owner) i=sfCoop.fireFingers.erase(i);else ++i;\n        }''',
'''        for(auto i=sfCoop.fireFingers.begin();i!=sfCoop.fireFingers.end();) {\n            if(i->second==owner) i=sfCoop.fireFingers.erase(i);else ++i;\n        }\n        sfKineticSurgeCancel(owner);''',
'death cancels surge')

replace_once('src/campaign_runtime.hpp',
'''    if (event->type==SDL_FINGERUP) sfCoop.fireFingers.erase(finger);\n    if (event->type==SDL_FINGERDOWN) {''',
'''    if (event->type==SDL_FINGERUP) {\n        auto binding=sfCoop.fireFingers.find(finger);\n        if(binding!=sfCoop.fireFingers.end()) {\n            const int owner=binding->second;\n            const bool purge=sfKineticSurgeRelease(owner);\n            if(purge) sfKineticPurgeAsteroids(owner); else sfCoopFire(owner);\n            sfCoop.fireFingers.erase(binding);\n        }\n    }\n    if (event->type==SDL_FINGERDOWN) {''',
'coop release chooses tap fire or purge')

replace_once('src/campaign_runtime.hpp',
'''                sfCoop.fireFingers[finger]=owner;sfCoopFire(owner);''',
'''                sfCoop.fireFingers[finger]=owner;sfKineticSurgePress(owner);''',
'coop second finger press')

replace_once('src/campaign_runtime.hpp',
'''    sfDrawTacticalEffects(renderer);\n    if (sfCoop.bonusLife>0 && textures.bonus) {''',
'''    sfDrawTacticalEffects(renderer);\n    sfDrawKineticEffects(renderer);\n    if (sfCoop.bonusLife>0 && textures.bonus) {''',
'coop kinetic visuals')

# ------------------------------------------------------------------
# Regression updates: tap now fires on release; long press/purge are verified.
# ------------------------------------------------------------------
replace_once('tests/restoration_regressions.hpp',
'''    a=sfCoopFinger(SDL_FINGERDOWN,1003,.4f,.2f);\n    b=sfCoopFinger(SDL_FINGERDOWN,1004,.6f,.8f);\n    sfFixHandleEvent(&a);sfFixHandleEvent(&b);\n    assert(sfCoop.shots.size()==2);\n    assert(sfCoop.shots[0].kind==4 && sfCoop.shots[1].kind==4);\n    assert(Spritej1->nrj==11 && Spritej2->nrj==11);\n    for(int i=0;i<120;++i) sfCoopMovePlayers(1.0f/60);\n    assert(sfCoop.shots.size()==2); // Holding or moving the firing finger never repeats.\n    a=sfCoopFinger(SDL_FINGERMOTION,1003,.8f,.8f);sfFixHandleEvent(&a);\n    assert(sfCoop.controls[0].finger==1001 && sfCoop.controls[1].finger==1002);\n    a=sfCoopFinger(SDL_FINGERUP,1003,.8f,.8f);sfFixHandleEvent(&a);\n    a=sfCoopFinger(SDL_FINGERDOWN,1003,.4f,.2f);sfFixHandleEvent(&a);\n    assert(sfCoop.shots.size()==3 && sfCoop.shots.back().kind==0);''',
'''    a=sfCoopFinger(SDL_FINGERDOWN,1003,.4f,.2f);\n    b=sfCoopFinger(SDL_FINGERDOWN,1004,.6f,.8f);\n    sfFixHandleEvent(&a);sfFixHandleEvent(&b);\n    assert(sfCoop.shots.empty());\n    a=sfCoopFinger(SDL_FINGERUP,1003,.4f,.2f);\n    b=sfCoopFinger(SDL_FINGERUP,1004,.6f,.8f);\n    sfFixHandleEvent(&a);sfFixHandleEvent(&b);\n    assert(sfCoop.shots.size()==2);\n    assert(sfCoop.shots[0].kind==4 && sfCoop.shots[1].kind==4);\n    assert(Spritej1->nrj==11 && Spritej2->nrj==11);\n    for(int i=0;i<120;++i) sfCoopMovePlayers(1.0f/60);\n    assert(sfCoop.shots.size()==2);\n    a=sfCoopFinger(SDL_FINGERDOWN,1003,.4f,.2f);sfFixHandleEvent(&a);\n    a=sfCoopFinger(SDL_FINGERMOTION,1003,.8f,.8f);sfFixHandleEvent(&a);\n    assert(sfCoop.controls[0].finger==1001 && sfCoop.controls[1].finger==1002);\n    a=sfCoopFinger(SDL_FINGERUP,1003,.8f,.8f);sfFixHandleEvent(&a);\n    assert(sfCoop.shots.size()==3 && sfCoop.shots.back().kind==0);''',
'tap fire deferred to release')

replace_once('tests/restoration_regressions.hpp',
'''    sfCoopBossContact(0,1.0f/60);assert(Spritej1->nrj==afterHeat && Spritej1->pv==afterPv);\n    std::puts("PASS: shared kinetic layers, velocity damage, charge gating and reserve-driven hull regeneration");''',
'''    sfCoopBossContact(0,1.0f/60);assert(Spritej1->nrj==afterHeat && Spritej1->pv==afterPv);\n\n    sfKineticResetSurges();sfKineticSurgePress(0);\n    sfKineticAdvanceSurges(SF_KINETIC_SURGE_HOLD_SECONDS-.01f);\n    assert(!sfKineticSurgeVisible(0) && sfKineticSurgePower(0)==1.0f);\n    sfKineticAdvanceSurges(.02f);\n    assert(sfKineticSurgeVisible(0) && sfKineticSurgePower(0)==2.0f);\n    sfKineticAdvanceSurges(SF_KINETIC_SURGE_DURATION+.01f);\n    assert(!sfKineticSurgeVisible(0) && sfKineticSurgePower(0)==1.0f);\n    assert(sfKineticSurgeRelease(0));\n\n    setupTactics();sfFixResetAsteroidField();sfFieldRemainder=0;\n    Spritej2->setxywh(390,1200,100,100);Spritej2->sw=Spritej2->sh=100;Spritej2->startup();\n    auto *nearRock=new sprite;nearRock->setv(470,1200,20,20,0,0,1);nearRock->w=nearRock->h=nearRock->sw=nearRock->sh=20;nearRock->pv=1;sa1.push_back(nearRock);\n    auto *farRock=new sprite;farRock->setv(650,1200,20,20,0,0,1);farRock->w=farRock->h=farRock->sw=farRock->sh=20;farRock->pv=1;sa1.push_back(farRock);\n    auto *white=new parts(410,1200);white->pv=600;white->vx=7;white->vy=-3;particules.push_back(white);\n    const float whiteVx=white->vx,whiteVy=white->vy;\n    const int purged=sfKineticPurgeAsteroids(1);\n    assert(purged==1 && nearRock->pv==0 && farRock->pv>0);\n    assert(white->pv==600 && white->vx==whiteVx && white->vy==whiteVy);\n\n    setupTactics();Spritej1->w=Spritej1->h=Spritej1->sw=Spritej1->sh=300;Spritej1->startup();\n    sfNormalizeClassicShipScale();\n    assert(std::max({Spritej1->w,Spritej1->h,Spritej1->sw,Spritej1->sh})<=sfArenaW*SF_CLASSIC_SHIP_DIAMETER_RATIO+.01f);\n    std::puts("PASS: shared kinetic layers, surge, purge, compact classic hulls and reserve-driven hull regeneration");''',
'surge purge regression coverage')

replace_once('scripts/test-regressions.sh',
'''python3 "$sf_repo/tests/test_kinetic_integration.py"\ng++ -std=c++17 -O1 -I "$sf_repo/src" "$sf_repo/tests/scenic_mix_regressions.cpp" -o "$sf_test_dir/scenic-mix"''',
'''python3 "$sf_repo/tests/test_kinetic_integration.py"\npython3 "$sf_repo/tests/test_kinetic_surge_integration.py"\ng++ -std=c++17 -O1 -I "$sf_repo/src" "$sf_repo/tests/kinetic_surge_regressions.cpp" -o "$sf_test_dir/kinetic-surge"\n"$sf_test_dir/kinetic-surge"\ng++ -std=c++17 -O1 -I "$sf_repo/src" "$sf_repo/tests/scenic_mix_regressions.cpp" -o "$sf_test_dir/scenic-mix"''',
'permanent surge tests')

# ------------------------------------------------------------------
# Living memories / mission history
# ------------------------------------------------------------------
entry='''## Kinetic surge / two-finger field — 2026-10-03\n- Canon shared by classic + coop: maximum kinetic field diameter = 2.0 ship diameters.\n- Short second-finger tap keeps firing; long hold (0.35 s) enters a transparent visible surge.\n- Surge doubles kinetic dissipation for at most 2.0 s. Release after activation purges every asteroid inside the max kinetic zone into historical white resource dust.\n- White dust is never physically deflected by kinetic waves; red dust remains reactive.\n- Normal absorption waves are more transparent; surge aura shows inner + outer circles.\n- Classic hulls are capped to the coop-like 15% arena-minimum diameter.\n- No main merge/release before Fab phone validation.'''
for rel in ('brain.md','brainmap.md','debughistorical.md','todo.md','ordres-de-mission.md'):
    append_once(rel,'Kinetic surge / two-finger field — 2026-10-03',entry)
