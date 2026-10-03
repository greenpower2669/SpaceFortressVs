#!/usr/bin/env python3
from pathlib import Path
import math
import struct
import wave

ROOT=Path(__file__).resolve().parents[1]

def read(path): return (ROOT/path).read_text()
def write(path,text):
    p=ROOT/path;p.parent.mkdir(parents=True,exist_ok=True);p.write_text(text)
def replace_once(text,old,new,label):
    count=text.count(old)
    if count!=1: raise SystemExit(f'{label}: expected exactly 1 match, got {count}')
    return text.replace(old,new,1)

# --- pure kinetic state: exactly two seconds of overcharge, then deliberate vulnerability ---
p='src/kinetic_shield.hpp';t=read(p)
t=replace_once(t,
'''constexpr float SF_KINETIC_SURGE_HOLD_SECONDS = .35f;\nconstexpr float SF_KINETIC_SURGE_DURATION = 2.0f;\nconstexpr float SF_KINETIC_SURGE_POWER_MULTIPLIER = 2.0f;''',
'''constexpr float SF_KINETIC_SURGE_HOLD_SECONDS = 2.0f;\nconstexpr float SF_KINETIC_SURGE_POWER_MULTIPLIER = 2.0f;''','surge constants')
t=replace_once(t,
'''struct SfKineticSurgeState {\n    bool held=false,charged=false;\n    float heldSeconds=0,boostSeconds=0;\n};''',
'''struct SfKineticSurgeState {\n    bool held=false,charged=false;\n    float heldSeconds=0;\n};''','surge state')
t=replace_once(t,
'''static void sfKineticAdvanceSurges(float dt)\n{\n    if(dt<=0) return;\n    for(auto &s:sfKineticSurges) {\n        if(!s.held) continue;\n        s.heldSeconds+=dt;\n        if(!s.charged && s.heldSeconds>=SF_KINETIC_SURGE_HOLD_SECONDS) {\n            s.charged=true;s.boostSeconds=0;\n        } else if(s.charged && s.boostSeconds<SF_KINETIC_SURGE_DURATION) {\n            s.boostSeconds=std::min(SF_KINETIC_SURGE_DURATION,s.boostSeconds+dt);\n        }\n    }\n}\nstatic float sfKineticSurgePower(int owner)\n{\n    if(owner<0 || owner>1) return 1.0f;\n    const auto &s=sfKineticSurges[owner];\n    return s.held && s.charged && s.boostSeconds<SF_KINETIC_SURGE_DURATION ?\n        SF_KINETIC_SURGE_POWER_MULTIPLIER : 1.0f;\n}\nstatic bool sfKineticSurgeVisible(int owner)\n{\n    if(owner<0 || owner>1) return false;\n    const auto &s=sfKineticSurges[owner];\n    return s.held && s.charged && s.boostSeconds<SF_KINETIC_SURGE_DURATION;\n}\n''',
'''static void sfKineticAdvanceSurges(float dt)\n{\n    if(dt<=0) return;\n    for(auto &s:sfKineticSurges) {\n        if(!s.held) continue;\n        s.heldSeconds+=dt;\n        if(!s.charged && s.heldSeconds>=SF_KINETIC_SURGE_HOLD_SECONDS)\n            s.charged=true;\n    }\n}\nstatic float sfKineticSurgePower(int owner)\n{\n    if(owner<0 || owner>1) return 1.0f;\n    const auto &s=sfKineticSurges[owner];\n    if(!s.held) return 1.0f;\n    return s.charged ? 0.0f : SF_KINETIC_SURGE_POWER_MULTIPLIER;\n}\nstatic bool sfKineticSurgeVisible(int owner)\n{\n    if(owner<0 || owner>1) return false;\n    const auto &s=sfKineticSurges[owner];\n    return s.held && !s.charged;\n}\nstatic bool sfKineticSurgeVulnerable(int owner)\n{\n    if(owner<0 || owner>1) return false;\n    const auto &s=sfKineticSurges[owner];\n    return s.held && s.charged;\n}\n''','surge timing')
t=replace_once(t,
'powerMultiplier=std::clamp(powerMultiplier,1.0f,SF_KINETIC_SURGE_POWER_MULTIPLIER);',
'powerMultiplier=std::clamp(powerMultiplier,0.0f,SF_KINETIC_SURGE_POWER_MULTIPLIER);','zero-power vulnerable state')
write(p,t)

# --- five-level boss danger selector ---
write('src/boss_danger.hpp','''#pragma once\n\ninline constexpr float SF_BOSS_DANGER_LEVELS[5]={1.0f,5.0f,10.0f,15.0f,20.0f};\ninline int sfBossDangerIndex=2; // x10 is the canonical default.\n\nstatic float sfBossDangerMultiplier()\n{\n    if(sfBossDangerIndex<0) sfBossDangerIndex=0;\n    if(sfBossDangerIndex>4) sfBossDangerIndex=4;\n    return SF_BOSS_DANGER_LEVELS[sfBossDangerIndex];\n}\nstatic void sfBossDangerAdjust(int direction)\n{\n    if(direction<0 && sfBossDangerIndex>0) --sfBossDangerIndex;\n    if(direction>0 && sfBossDangerIndex<4) ++sfBossDangerIndex;\n}\n''')
p='src/game_mode.hpp';t=read(p)
t=replace_once(t,'#pragma once\n','#pragma once\n#include "boss_danger.hpp"\n','boss danger include')
write(p,t)

# --- home selector UI ---
p='src/start_ui.hpp';t=read(p)
mode_end='''    sfUiCenteredText(renderer, width,\n                     mode.y + (mode.h - 7 * buttonScale) / 2,\n                     modeText, buttonScale, 238, 248, 255);\n\n    SDL_Rect start = {\n        static_cast<int>(width * 0.10f), static_cast<int>(height * 0.56f),\n        static_cast<int>(width * 0.80f), static_cast<int>(height * 0.115f)\n    };'''
mode_new='''    sfUiCenteredText(renderer, width,\n                     mode.y + (mode.h - 7 * buttonScale) / 2,\n                     modeText, buttonScale, 238, 248, 255);\n\n    SDL_Rect danger = {\n        static_cast<int>(width * 0.10f), static_cast<int>(height * 0.515f),\n        static_cast<int>(width * 0.80f), static_cast<int>(height * 0.075f)\n    };\n    sfUiPanel(renderer, danger, 8, 24, 45, 235, 110, 95);\n    const std::string dangerText = std::string("< DANGER BOSS : X") +\n        std::to_string(int(sfBossDangerMultiplier())) + " >";\n    sfUiCenteredText(renderer,width,danger.y+(danger.h-7*base)/2,\n                     dangerText.c_str(),base,255,225,205);\n\n    SDL_Rect start = {\n        static_cast<int>(width * 0.10f), static_cast<int>(height * 0.615f),\n        static_cast<int>(width * 0.80f), static_cast<int>(height * 0.095f)\n    };'''
t=replace_once(t,mode_end,mode_new,'home danger panel')
t=replace_once(t,
'''    const float x = finger.x;\n    const float y = finger.y;\n    (void)x; // full-width buttons for now\n\n    if (sfUiScreen == SF_UI_HOME) {\n        if (y >= 0.39f && y <= 0.52f) {\n            setia = !setia;\n        } else if (y >= 0.545f && y <= 0.69f) {\n            sfUiStartMatch();\n        } else if (y >= 0.71f && y <= 0.85f) {\n            sfUiScreen = SF_UI_HELP;\n        }\n''',
'''    const float x = finger.x;\n    const float y = finger.y;\n\n    if (sfUiScreen == SF_UI_HOME) {\n        if (y >= 0.39f && y <= 0.505f) {\n            setia = !setia;\n        } else if (y >= 0.515f && y <= 0.60f) {\n            sfBossDangerAdjust(x < .5f ? -1 : 1);\n        } else if (y >= 0.61f && y <= 0.715f) {\n            sfUiStartMatch();\n        } else if (y >= 0.73f && y <= 0.85f) {\n            sfUiScreen = SF_UI_HELP;\n        }\n''','home touch ranges')
write(p,t)

# --- classic event path: audio starts with second finger, release decides cancel/deflagration ---
p='src/remaster_runtime.hpp';t=read(p)
t=replace_once(t,
'''static bool sfFireMain(int owner,const tupl *target);\n''',
'''static bool sfFireMain(int owner,const tupl *target);\nstatic void sfKineticAudioStartCharge(int owner);\nstatic void sfKineticAudioCancel(int owner);\nstatic void sfKineticAudioRelease(int owner);\n''','classic audio declarations')
t=replace_once(t,
'''                sfRmKineticFinger=fid;sfKineticSurgePress(1);\n''',
'''                sfRmKineticFinger=fid;sfKineticSurgePress(1);sfKineticAudioStartCharge(1);\n''','classic charge sound')
t=replace_once(t,
'''            const bool purge=sfKineticSurgeRelease(1);\n            if(purge) sfKineticPurgeAsteroids(1); else sfFireMain(1,nullptr);\n''',
'''            const bool purge=sfKineticSurgeRelease(1);\n            if(purge) {sfKineticAudioRelease(1);sfKineticPurgeAsteroids(1);}\n            else {sfKineticAudioCancel(1);sfFireMain(1,nullptr);}\n''','classic release sound')
write(p,t)

# --- tactical audio + rainbow visual feedback ---
p='src/tactical_runtime.hpp';t=read(p)
anchor='''inline bool sfFieldCollisionSound=false,sfFieldMiningSound=false;\nconstexpr int SF_TURRETS_PER_TEAM = 6;'''
audio='''inline bool sfFieldCollisionSound=false,sfFieldMiningSound=false;\n\nstruct SfKineticAudioState {\n    Mix_Chunk *charge=nullptr,*ready=nullptr,*release=nullptr;\n    std::array<int,2> chargeChannel{{-1,-1}};\n    std::array<bool,2> readySignaled{{false,false}};\n};\ninline SfKineticAudioState sfKineticAudio;\nstatic void sfKineticAudioEnsure()\n{\n    if(!sfKineticAudio.charge) sfKineticAudio.charge=Mix_LoadWAV("./resources/assets/sounds/kinetic_charge.wav");\n    if(!sfKineticAudio.ready) sfKineticAudio.ready=Mix_LoadWAV("./resources/assets/sounds/kinetic_ready.wav");\n    if(!sfKineticAudio.release) sfKineticAudio.release=Mix_LoadWAV("./resources/assets/sounds/kinetic_release.wav");\n    if(sfKineticAudio.charge) Mix_VolumeChunk(sfKineticAudio.charge,76);\n    if(sfKineticAudio.ready) Mix_VolumeChunk(sfKineticAudio.ready,102);\n    if(sfKineticAudio.release) Mix_VolumeChunk(sfKineticAudio.release,122);\n}\nstatic void sfKineticAudioHaltCharge(int owner)\n{\n    if(owner<0 || owner>1) return;\n    const int channel=sfKineticAudio.chargeChannel[owner];\n    if(channel>=0) Mix_HaltChannel(channel);\n    sfKineticAudio.chargeChannel[owner]=-1;\n}\nstatic void sfKineticAudioStartCharge(int owner)\n{\n    if(owner<0 || owner>1) return;\n    sfKineticAudioEnsure();sfKineticAudioHaltCharge(owner);sfKineticAudio.readySignaled[owner]=false;\n    if(sfKineticAudio.charge) sfKineticAudio.chargeChannel[owner]=Mix_PlayChannel(-1,sfKineticAudio.charge,0);\n}\nstatic void sfKineticAudioCancel(int owner)\n{\n    if(owner<0 || owner>1) return;\n    sfKineticAudioHaltCharge(owner);sfKineticAudio.readySignaled[owner]=false;\n}\nstatic void sfKineticAudioRelease(int owner)\n{\n    if(owner<0 || owner>1) return;\n    sfKineticAudioEnsure();sfKineticAudioHaltCharge(owner);sfKineticAudio.readySignaled[owner]=false;\n    if(sfKineticAudio.release) Mix_PlayChannel(-1,sfKineticAudio.release,0);\n}\nstatic void sfKineticAudioUpdate()\n{\n    sfKineticAudioEnsure();\n    for(int owner=0;owner<2;++owner) {\n        const auto &surge=sfKineticSurges[owner];\n        if(surge.held && surge.charged && !sfKineticAudio.readySignaled[owner]) {\n            sfKineticAudioHaltCharge(owner);\n            if(sfKineticAudio.ready) Mix_PlayChannel(-1,sfKineticAudio.ready,0);\n            sfKineticAudio.readySignaled[owner]=true;\n        } else if(!surge.held) {\n            sfKineticAudioHaltCharge(owner);sfKineticAudio.readySignaled[owner]=false;\n        }\n    }\n}\nstatic void sfKineticAudioReset()\n{\n    for(int owner=0;owner<2;++owner) sfKineticAudioCancel(owner);\n}\n\nconstexpr int SF_TURRETS_PER_TEAM = 6;'''
t=replace_once(t,anchor,audio,'tactical audio')
t=replace_once(t,
'''    sfPilot=SfPilot{}; sfObserved={}; sfPickupGlow={}; sfTurrets={}; sfKineticWaves.clear(); sfKineticWaveSerial=0;sfKineticResetSurges();\n''',
'''    sfPilot=SfPilot{}; sfObserved={}; sfPickupGlow={}; sfTurrets={}; sfKineticWaves.clear(); sfKineticWaveSerial=0;sfKineticAudioReset();sfKineticResetSurges();\n''','audio reset')
t=replace_once(t,
'''    if (sfUiScreen==SF_UI_GAME && !sfIsCoop()) sfKineticAdvanceSurges(sfFrameDt);\n''',
'''    if (sfUiScreen==SF_UI_GAME && !sfIsCoop()) {sfKineticAdvanceSurges(sfFrameDt);sfKineticAudioUpdate();}\n''','classic audio update')
ring_anchor='''static void sfTacticalRing(SDL_Renderer *renderer,tupl center,float radius,SDL_Color color)\n{\n    std::array<SDL_FPoint,49> points;\n    for (int i=0;i<=48;++i) {\n        const float angle=i*2*float(PI)/48;\n        points[i]={center.x+std::cos(angle)*radius,center.y+std::sin(angle)*radius};\n    }\n    SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);\n    SDL_RenderDrawLinesF(renderer,points.data(),int(points.size()));\n}\n'''
ring_new=ring_anchor+'''static SDL_Color sfKineticRainbowColor(float hue,Uint8 alpha)\n{\n    hue=std::fmod(hue,1.0f);if(hue<0) hue+=1.0f;\n    const float h=hue*6.0f;const int sector=int(h)%6;const float f=h-int(h);\n    const float q=1.0f-f;float r=0,g=0,b=0;\n    if(sector==0){r=1;g=f;} else if(sector==1){r=q;g=1;}\n    else if(sector==2){g=1;b=f;} else if(sector==3){g=q;b=1;}\n    else if(sector==4){r=f;b=1;} else {r=1;b=q;}\n    return {Uint8(255*r),Uint8(255*g),Uint8(255*b),alpha};\n}\nstatic void sfTacticalRainbowRing(SDL_Renderer *renderer,tupl center,float radius,float phase,Uint8 alpha)\n{\n    constexpr int segments=72;\n    for(int i=0;i<segments;++i) {\n        const float a0=i*2*float(PI)/segments,a1=(i+1)*2*float(PI)/segments;\n        const auto c=sfKineticRainbowColor(phase+i/float(segments),alpha);\n        SDL_SetRenderDrawColor(renderer,c.r,c.g,c.b,c.a);\n        SDL_RenderDrawLineF(renderer,center.x+std::cos(a0)*radius,center.y+std::sin(a0)*radius,\n                            center.x+std::cos(a1)*radius,center.y+std::sin(a1)*radius);\n    }\n}\n'''
t=replace_once(t,ring_anchor,ring_new,'rainbow helpers')
t=replace_once(t,
'''        const float pulse=.5f+.5f*std::sin(float(SDL_GetTicks64())*.010f+owner*1.7f);\n        const SDL_Color team=owner==0 ? SDL_Color{255,188,96,255} : SDL_Color{96,210,255,255};\n        const float outer=diameter*SF_KINETIC_MAX_SHIELD_DIAMETER*.5f;\n        const float inner=diameter*SF_KINETIC_INNER_MAX_RADIUS_SHIP_DIAMETERS;\n        sfTacticalRing(renderer,tupl(ship->x,ship->y),outer,SDL_Color{team.r,team.g,team.b,Uint8(78+34*pulse)});\n        sfTacticalRing(renderer,tupl(ship->x,ship->y),outer-2,SDL_Color{team.r,team.g,team.b,Uint8(36+18*pulse)});\n        sfTacticalRing(renderer,tupl(ship->x,ship->y),inner,SDL_Color{team.r,team.g,team.b,Uint8(58+26*pulse)});\n''',
'''        const float pulse=.5f+.5f*std::sin(float(SDL_GetTicks64())*.010f+owner*1.7f);\n        const float phase=std::fmod(float(SDL_GetTicks64())*.00028f+owner*.17f,1.0f);\n        const float outer=diameter*SF_KINETIC_MAX_SHIELD_DIAMETER*.5f;\n        const float inner=diameter*SF_KINETIC_INNER_MAX_RADIUS_SHIP_DIAMETERS;\n        sfTacticalRainbowRing(renderer,tupl(ship->x,ship->y),outer,phase,Uint8(112+36*pulse));\n        sfTacticalRainbowRing(renderer,tupl(ship->x,ship->y),outer-2,phase+.19f,Uint8(62+22*pulse));\n        sfTacticalRainbowRing(renderer,tupl(ship->x,ship->y),inner,phase+.37f,Uint8(92+28*pulse));\n''','rainbow surge drawing')
write(p,t)

# --- coop: dynamic non-kinetic boss danger, shared audio and exact home ranges ---
p='src/campaign_runtime.hpp';t=read(p)
t=replace_once(t,'static constexpr float SF_COOP_INCOMING_DAMAGE_MULTIPLIER = 15.0f;\n','', 'remove fixed x15')
t=t.replace('SF_COOP_INCOMING_DAMAGE_MULTIPLIER*damage','sfBossDangerMultiplier()*damage')
t=t.replace('SF_COOP_INCOMING_DAMAGE_MULTIPLIER*650.0f','sfBossDangerMultiplier()*650.0f')
if 'SF_COOP_INCOMING_DAMAGE_MULTIPLIER' in t: raise SystemExit('fixed coop multiplier still present')
t=replace_once(t,
'''        sfKineticSurgeCancel(owner);\n''',
'''        sfKineticAudioCancel(owner);sfKineticSurgeCancel(owner);\n''','death cancels kinetic audio')
t=replace_once(t,
'''    sfKineticAdvanceSurges(dt);\n    sfCoop.time+=dt;''',
'''    sfKineticAdvanceSurges(dt);sfKineticAudioUpdate();\n    sfCoop.time+=dt;''','coop ready sound update')
t=replace_once(t,
'''    sfCoop.fireFingers.clear();sfKineticResetSurges();\n''',
'''    sfCoop.fireFingers.clear();sfKineticAudioReset();sfKineticResetSurges();\n''','pause cancels audio')
t=replace_once(t,
'''            const bool purge=sfKineticSurgeRelease(owner);\n            if(purge) sfKineticPurgeAsteroids(owner); else sfCoopFire(owner);\n''',
'''            const bool purge=sfKineticSurgeRelease(owner);\n            if(purge) {sfKineticAudioRelease(owner);sfKineticPurgeAsteroids(owner);}\n            else {sfKineticAudioCancel(owner);sfCoopFire(owner);}\n''','coop release sound')
t=replace_once(t,
'''                sfCoop.fireFingers[finger]=owner;sfKineticSurgePress(owner);\n''',
'''                sfCoop.fireFingers[finger]=owner;sfKineticSurgePress(owner);sfKineticAudioStartCharge(owner);\n''','coop charge sound')
t=replace_once(t,
'''        if (y>=.71f && y<=.85f && x>=.5f) {sfLoadCampaign();sfFixRequestedScreen.store(SF_UI_HALL);}\n        else if (y>=.545f && y<=.69f && (sfSelectedMode==SF_COOP_LOCAL || sfSelectedMode==SF_COOP_AI)) {''',
'''        if (y>=.73f && y<=.85f && x>=.5f) {sfLoadCampaign();sfFixRequestedScreen.store(SF_UI_HALL);}\n        else if (y>=.61f && y<=.715f && (sfSelectedMode==SF_COOP_LOCAL || sfSelectedMode==SF_COOP_AI)) {''','coop home hit ranges')
write(p,t)

# --- three original synthetic WAV cues (PCM16 mono; no external licensing) ---
sounds=ROOT/'assets/sounds';sounds.mkdir(parents=True,exist_ok=True)
RATE=44100

def wav(name,seconds,sample_fn):
    frames=int(RATE*seconds);data=bytearray();phase=0.0
    state={'phase':0.0,'seed':0x51F0A7}
    for i in range(frames):
        x=max(-1.0,min(1.0,sample_fn(i/float(RATE),seconds,state)))
        data+=struct.pack('<h',int(x*32767))
    with wave.open(str(sounds/name),'wb') as out:
        out.setnchannels(1);out.setsampwidth(2);out.setframerate(RATE);out.writeframes(data)

def charge_sample(t,d,s):
    p=t/d;freq=105+920*(p**1.65);s['phase']+=2*math.pi*freq/RATE
    fade=min(1.0,t/.035,(d-t)/.025);amp=(.17+.48*p)*max(0.0,fade)
    return amp*(.72*math.sin(s['phase'])+.20*math.sin(s['phase']*2.01)+.08*math.sin(s['phase']*.503))

def ready_sample(t,d,s):
    env=math.exp(-t*13.0);return env*(.54*math.sin(2*math.pi*980*t)+.31*math.sin(2*math.pi*1470*t)+.15*math.sin(2*math.pi*2210*t))

def release_sample(t,d,s):
    p=t/d;freq=92-47*p;s['phase']+=2*math.pi*freq/RATE
    s['seed']=(1664525*s['seed']+1013904223)&0xffffffff;noise=((s['seed']>>8)&0xffff)/32767.5-1.0
    env=math.exp(-t*4.2);crack=math.exp(-t*18.0)
    return .70*env*math.sin(s['phase'])+.20*env*math.sin(s['phase']*2.03)+.30*crack*noise

wav('kinetic_charge.wav',2.0,charge_sample)
wav('kinetic_ready.wav',.24,ready_sample)
wav('kinetic_release.wav',.82,release_sample)

# --- living method documents ---
entries={
'brain.md':'\n- 2026-10-03 KINETIC AUDIO/DANGER: 2e doigt charge exactement 2 s avec champ cinétique x2 irisé; à 2.00 s signal CHARGE et protection cinétique coupée jusqu’au relâchement; relâchement chargé = déflagration + purge. Danger boss non cinétique réglable accueil x1/x5/x10/x15/x20, défaut x10.\n',
'brainmap.md':'\n- Kinetic surge canon: HOLD 0..2s => x2 + rainbow + charge sound; READY >=2s => kinetic shield OFF; release => blast/purge. Boss non-kinetic danger selector: 1/5/10/15/20, default 10.\n',
'debughistorical.md':'\n- 2026-10-03: ancien surge 0.35 s + boost 2 s remplacé par charge totale 2.00 s puis fenêtre volontaire sans bouclier jusqu’au relâchement. x15 boss fixe supprimé au profit du réglage non cinétique x1..x20 (défaut x10). src/main.cpp reste historique.\n',
'todo.md':'\n- [ ] TEST TELEPHONE: confirmer synchro son charge 2.00 s, signal prêt, coupure visuelle/protection après 2 s, déflagration au relâchement, irisation lisible et sélecteur DANGER BOSS x1/x5/x10/x15/x20.\n',
'ordres-de-mission.md':'\n## AVENANT 2026-10-03 — SURCHARGE AUDIO + DANGER BOSS\nCanon validé Fab: charge 2e doigt exactement 2 s, champ x2 irisé pendant la charge; à 2 s son prêt et bouclier cinétique OFF jusqu’au relâchement; relâchement chargé = son de déflagration + purge astéroïdes. Trois sons originaux synthétiques intégrés. Dégâts boss non cinétiques: défaut x10, sélection accueil x1/x5/x10/x15/x20. Le cinétique reste hors multiplicateur boss. Classique et coop homogènes. Aucun merge main/release sans validation téléphone.\n'}
for path,entry in entries.items():
    text=read(path)
    if entry.strip() not in text: write(path,text+entry)

# Guard historical source.
import hashlib
main=(ROOT/'src/main.cpp').read_bytes()
assert hashlib.sha1(main).hexdigest()=='835059a0ecfe0f74708068b3259cad5db1cdb579'
print('GREEN patch applied: kinetic audio, rainbow vulnerability and boss danger selector')
