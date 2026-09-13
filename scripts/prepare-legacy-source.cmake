# Android compatibility patch for two historical IA bugs. Keep src/main.cpp
# byte-for-byte intact and compile a generated Android-only copy instead.
set(LEGACY_MAIN "${REPO_ROOT}/src/main.cpp")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${LEGACY_MAIN}")
set(ANDROID_MAIN "${CMAKE_BINARY_DIR}/generated/main_android_compat.cpp")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/generated")
file(READ "${LEGACY_MAIN}" GAME_MAIN)

set(SUIVEUR_OLD "bool suiveur(){\n\tif (Suiveur->pv==0) return true;\n\t}")
set(SUIVEUR_NEW "bool suiveur(){\n\tif (Suiveur->pv==0) return true;\n\treturn false;\n\t}")
string(FIND "${GAME_MAIN}" "${SUIVEUR_OLD}" SUIVEUR_POS)
if(SUIVEUR_POS EQUAL -1)
    message(FATAL_ERROR "Historical suiveur() pattern not found; refusing silent IA patch drift")
endif()
string(REPLACE "${SUIVEUR_OLD}" "${SUIVEUR_NEW}" GAME_MAIN "${GAME_MAIN}")

set(IACALC_OLD "iacalc->v.vx=lockast.vx;iacalc->v.setvi();")
set(IACALC_NEW "iacalc->v.vy=lockast.vy;iacalc->v.setvi();")
string(FIND "${GAME_MAIN}" "${IACALC_OLD}" IACALC_POS)
if(IACALC_POS EQUAL -1)
    message(FATAL_ERROR "Historical IA vx/vy pattern not found; refusing silent IA patch drift")
endif()
string(REPLACE "${IACALC_OLD}" "${IACALC_NEW}" GAME_MAIN "${GAME_MAIN}")

# Expose the old per-loop follower gate to the Android compatibility layer so a
# new match can truly start from the same IA state every time.
set(SUIVEUR_MAJ_OLD "bool suiveurmajaf=true;")
set(SUIVEUR_MAJ_NEW "bool &suiveurmajaf=SpaceFortressLegacySuiveurMajaf;")
string(FIND "${GAME_MAIN}" "${SUIVEUR_MAJ_OLD}" SUIVEUR_MAJ_POS)
if(SUIVEUR_MAJ_POS EQUAL -1)
    message(FATAL_ERROR "Historical suiveurmajaf pattern not found; refusing silent IA patch drift")
endif()
string(REPLACE "${SUIVEUR_MAJ_OLD}" "${SUIVEUR_MAJ_NEW}" GAME_MAIN "${GAME_MAIN}")

# The particle renderer reads s[0] through s[15], but the old initialization
# stopped at s[10]. Initialize the complete array in the Android copy.
set(PARTICLES_OLD "for (int i=0;i<11;i++){")
string(FIND "${GAME_MAIN}" "${PARTICLES_OLD}" PARTICLES_POS)
if(PARTICLES_POS EQUAL -1)
    message(FATAL_ERROR "Historical particle threshold pattern not found")
endif()
string(REPLACE "${PARTICLES_OLD}" "for (int i=0;i<20;i++){" GAME_MAIN "${GAME_MAIN}")

# Both functions have non-void return types. Falling off the end is undefined
# after Android renames main to SDL_main, even when callers ignore the result.
set(GEAR_OLD "bool isrouaged(){\n\t    timerisrouage=2;\n\t    isrouage2=true;\n  }")
set(GEAR_NEW "bool isrouaged(){\n\t    timerisrouage=2;\n\t    isrouage2=true;\n    return isrouage2;\n  }")
string(FIND "${GAME_MAIN}" "${GEAR_OLD}" GEAR_POS)
string(FIND "${GAME_MAIN}" "}//main" MAIN_END_POS)
if(GEAR_POS EQUAL -1 OR MAIN_END_POS EQUAL -1)
    message(FATAL_ERROR "Historical return-value patch pattern not found")
endif()
string(REPLACE "${GEAR_OLD}" "${GEAR_NEW}" GAME_MAIN "${GAME_MAIN}")
string(REPLACE "}//main" "return 0;\n}//main" GAME_MAIN "${GAME_MAIN}")

# Additional crash guards verified against the real runtime helpers.
macro(sf_patch_once label before after)
    string(FIND "${GAME_MAIN}" "${before}" sf_patch_position)
    if(sf_patch_position EQUAL -1)
        message(FATAL_ERROR "Historical ${label} pattern not found")
    endif()
    string(REPLACE "${before}" "${after}" GAME_MAIN "${GAME_MAIN}")
endmacro()

sf_patch_once("zero-energy aligned IA shot"
    "rand()%(50*int(Spritej1->nrj\n      ))"
    "SpaceFortressRandomBelow(50*int(Spritej1->nrj))")

sf_patch_once("zero-energy asteroid IA shot"
    "rand()%(10*int(Spritej1->nrj))"
    "SpaceFortressRandomBelow(10*int(Spritej1->nrj))")

sf_patch_once("safe particulesr erase"
    "for(auto i=particulesr.begin();i!=particulesr.end();){\n          parts *e=*i;\n          \n      if (particulesr.size()>1000/k0) e->pv=0;    \n      if (e->pv==0) {\n          particulesr.erase(i);delete e; \n       }\n      i++;\n    }"
    "SpaceFortressPruneParticles(particulesr, 1000/k0);")

sf_patch_once("safe particules erase"
    "for(auto i=particules.begin();i!=particules.end();){\n          parts *e=*i;\n         if (particules.size()>1000/k0) e->pv=0;      \n      if (e->pv==0) {\n          particules.erase(i);delete e; \n       }\n      i++;\n    }"
    "SpaceFortressPruneParticles(particules, 1000/k0);")

# Replace bounded historical blocks, refusing to silently patch another layout.
macro(sf_replace_region label begin_marker end_marker replacement)
    string(FIND "${GAME_MAIN}" "${begin_marker}" sf_begin)
    if(sf_begin EQUAL -1)
        message(FATAL_ERROR "Historical ${label} start not found")
    endif()
    string(SUBSTRING "${GAME_MAIN}" ${sf_begin} -1 sf_tail)
    string(FIND "${sf_tail}" "${end_marker}" sf_length)
    if(sf_length EQUAL -1)
        message(FATAL_ERROR "Historical ${label} end not found")
    endif()
    math(EXPR sf_end "${sf_begin}+${sf_length}")
    string(SUBSTRING "${GAME_MAIN}" 0 ${sf_begin} sf_prefix)
    string(SUBSTRING "${GAME_MAIN}" ${sf_end} -1 sf_suffix)
    set(GAME_MAIN "${sf_prefix}${replacement}${sf_suffix}")
endmacro()

sf_replace_region("blue main fire" "void tirerj2(){" "//  SDL_RenderDrawPoints" "void tirerj2(){ sfFireMain(1); }\n\n")
sf_replace_region("orange main fire" "void tirerj1(){" "// impact réajustement" "void tirerj1(){ sfFireMain(0); }\n\n")
sf_replace_region("invisible border guns" "//maj coo j1 et 2 pour calc" "//\tdefence j1j2fin" "sfTacticsBeginFrame(renderer);\n")
sf_replace_region("old IA pursuit" "lockast.setxywh(-100,-100,1,1);" "if ((incra1)<8)" "// Tactical pilot owns anticipation, steering and firing.\n")
sf_replace_region("old IA mining/follower" "if (inab(e->y,Spritej1->y,H*0.7)" "e->ast();" "")
sf_replace_region("old IA force cascade" "if ((not setgui) and setia){\n\n    if (lockast.x!=-100)" "    //parts\n    //delete lockast" "")
sf_replace_region("dust collection" "for (auto ps:particules) {" "    //parts\n    rnd1=" "sfCollectDust();\n\n")
sf_patch_once("mining only live objects" "if (colee(p,e))" "if (p->pv>0 && e->pv>0 && colee(p,e))")
sf_patch_once("elapsed-time defence projectiles" "e->updatetir();" "sfAdvanceProjectile(e);")
sf_patch_once("swept orange ship impacts" "if (colee(Spritej1,e))" "if (e->pv>0 && (colee(Spritej1,e) || sfDefensiveShotCrosses(Spritej1,e)))")
sf_patch_once("swept blue ship impacts" "if (colee(Spritej2,e))" "if (e->pv>0 && (colee(Spritej2,e) || sfDefensiveShotCrosses(Spritej2,e)))")
sf_patch_once("swept asteroid impacts" "e->pv>0 && colee(p,e)" "e->pv>0 && (colee(p,e) || sfDefensiveShotCrosses(e,p))")
sf_replace_region("defence impact heat independent of frame rate" "float nrjminus(sprite *e){" "float vectoriser(" "float nrjminus(sprite *e){ return sfShotHeat(e); }\n\n")
sf_patch_once("orange shot count" "tirj1-=1;" "tirj1=std::max(0,tirj1-1);")
sf_patch_once("blue shot count" "tirj2-=1;" "tirj2=std::max(0,tirj2-1);")
sf_patch_once("single IA movement" "\tSpritej1->unctrl();" "\tif (!setia) Spritej1->unctrl();")
sf_patch_once("visible turret effects" " SDL_RenderPresent(renderer);" " sfDrawTacticalEffects(renderer);\n SDL_RenderPresent(renderer);")

# Lists, ships and state resets share one guard across their legacy producers.
# Schedule waits and blocking SDL events remain outside that guard.
foreach(sf_flag tics tics1 tics2 tics3 tics4)
    sf_patch_once("atomic ${sf_flag}" "bool ${sf_flag};" "std::atomic<bool> ${sf_flag}{false};")
endforeach()
sf_patch_once("atomic foreground" "bool apap=false;" "std::atomic<bool> apap{false};")
sf_patch_once("render setup guard" "void threadfps(double fps2)\n{" "void threadfps(double fps2)\n{\n    std::unique_lock<std::recursive_mutex> sfSetupLock(sfGameMutex);\n    sfGameReady=false;")
sf_patch_once("render readiness" "while(runed){\n\t//std::cout << \"fps " "sfGameReady=true;\nsfSetupLock.unlock();\nwhile(runed){\n\t//std::cout << \"fps ")
sf_patch_once("frame guard" "\ttics2=true;\n\tusleep(intlatence*3);" "\ttics2=true;\n\tusleep(intlatence*3);\n    std::unique_lock<std::recursive_mutex> sfFrameLock(sfGameMutex);")
sf_patch_once("physics guard" "    //th1();\n    Spritej1->collide();" "    std::lock_guard<std::recursive_mutex> sfPhysicsLock(sfGameMutex);\n    if (!sfGameReady || apap || setgui) continue;\n    if (!setia) Spritej1->collide();")
sf_patch_once("aux1 guard" "\ttics3=true;\n\tusleep(intlatence*4);" "\ttics3=true;\n\tusleep(intlatence*4);\n    std::lock_guard<std::recursive_mutex> sfAux1Lock(sfGameMutex);\n    if (!sfGameReady || apap || setgui) continue;")
sf_patch_once("aux2 guard" "\ttics4=true;\n\tusleep(intlatence*2);" "\ttics4=true;\n\tusleep(intlatence*2);\n    std::lock_guard<std::recursive_mutex> sfAux2Lock(sfGameMutex);\n    if (!sfGameReady || apap || setgui) continue;")
sf_patch_once("input mutation guard" "while ( SDL_WaitEvent ( &e )) {" "while ( SDL_WaitEvent ( &e )) {\n        std::lock_guard<std::recursive_mutex> sfInputLock(sfGameMutex);\n        if (!sfGameReady) continue;")
sf_patch_once("foreground wait releases guard" "if (apap){while (apap){usleep(intlatence*5);}" "if (apap){sfFrameLock.unlock(); while (apap){usleep(intlatence*5);} sfFrameLock.lock();")
sf_replace_region("continue resets complete match" "iago->xy.setxy(W*025,H*0.25);" "loosej2->pv=0;}}" "sfFixResetMatchState();\n            taux2b=0;\n            ")

file(WRITE "${ANDROID_MAIN}" "${GAME_MAIN}")
