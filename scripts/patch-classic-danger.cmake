# Apply the danger-9 classic IA overlay after the historical Android source has
# been generated. Keeping this as a second, narrow pass avoids modifying the
# long-lived historical compatibility patch stack for an orthogonal feature.
if(NOT DEFINED REPO_ROOT)
    get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANDROID_MAIN)
    set(ANDROID_MAIN "${CMAKE_BINARY_DIR}/generated/main_android_compat.cpp")
endif()
if(NOT EXISTS "${ANDROID_MAIN}")
    message(FATAL_ERROR "Generated Android source not found: ${ANDROID_MAIN}")
endif()

file(READ "${ANDROID_MAIN}" SF_DANGER_MAIN)

macro(sf_danger_patch_once label before after)
    string(FIND "${SF_DANGER_MAIN}" "${before}" sf_danger_position)
    if(sf_danger_position EQUAL -1)
        message(FATAL_ERROR "Classic danger patch drift: ${label} pattern not found")
    endif()
    string(REPLACE "${before}" "${after}" SF_DANGER_MAIN "${SF_DANGER_MAIN}")
endmacro()

# Only owner-0 hostile projectiles hitting the blue human are multiplied, and
# classification is performed by sfClassicIncomingNonKinetic(). Asteroids and
# all kinetic paths stay untouched.
sf_danger_patch_once("blue missile non-kinetic hull damage"
    "Spritej2->pv-=sfApplyShieldImpact(Spritej2,misspvminus(Spritej2));"
    "Spritej2->pv-=sfApplyShieldImpact(Spritej2,sfClassicIncomingNonKinetic(e,Spritej2,misspvminus(Spritej2)));")
sf_danger_patch_once("blue normal non-kinetic hull damage"
    "Spritej2->pv-=sfApplyShieldImpact(Spritej2,pvminus(Spritej2));"
    "Spritej2->pv-=sfApplyShieldImpact(Spritej2,sfClassicIncomingNonKinetic(e,Spritej2,pvminus(Spritej2)));")

file(WRITE "${ANDROID_MAIN}" "${SF_DANGER_MAIN}")
