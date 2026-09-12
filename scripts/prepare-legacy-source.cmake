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

file(WRITE "${ANDROID_MAIN}" "${GAME_MAIN}")
