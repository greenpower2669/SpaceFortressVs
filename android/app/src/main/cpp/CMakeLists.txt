cmake_minimum_required(VERSION 3.22.1)

project(SpaceFortressVs LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../../.." ABSOLUTE)
set(THIRD_PARTY "${REPO_ROOT}/third_party")

if(NOT EXISTS "${THIRD_PARTY}/SDL/CMakeLists.txt")
    message(FATAL_ERROR "SDL2 sources missing. Run scripts/fetch-sdl.sh before Gradle.")
endif()

# SDL2 core
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory("${THIRD_PARTY}/SDL" "${CMAKE_BINARY_DIR}/SDL" EXCLUDE_FROM_ALL)

# SDL2_image: keep the formats used by the historical assets, avoid unrelated codecs.
set(SDL2IMAGE_VENDORED ON CACHE BOOL "" FORCE)
set(SDL2IMAGE_INSTALL OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_TESTS OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_AVIF OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_JXL OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_WEBP OFF CACHE BOOL "" FORCE)
set(SDL2IMAGE_PNG ON CACHE BOOL "" FORCE)
set(SDL2IMAGE_JPG ON CACHE BOOL "" FORCE)
set(SDL2IMAGE_TIF ON CACHE BOOL "" FORCE)
add_subdirectory("${THIRD_PARTY}/SDL_image" "${CMAKE_BINARY_DIR}/SDL_image" EXCLUDE_FROM_ALL)

# SDL2_mixer: the game uses WAV, OGG and MP3.
set(SDL2MIXER_VENDORED ON CACHE BOOL "" FORCE)
set(SDL2MIXER_INSTALL OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_TESTS OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_FLAC OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_MOD OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_MIDI OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_OPUS OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_WAVPACK OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_MP3 ON CACHE BOOL "" FORCE)
set(SDL2MIXER_OGG ON CACHE BOOL "" FORCE)
add_subdirectory("${THIRD_PARTY}/SDL_mixer" "${CMAKE_BINARY_DIR}/SDL_mixer" EXCLUDE_FROM_ALL)

# The historical source includes <SDL2/...>. The upstream SDL2 family exposes
# its headers from include/, so build the expected SDL2/ compatibility directory
# without touching the old game source.
set(SDL2_COMPAT "${CMAKE_BINARY_DIR}/compat/SDL2")
file(MAKE_DIRECTORY "${SDL2_COMPAT}")
file(GLOB SDL2_HEADERS "${THIRD_PARTY}/SDL/include/*.h")
file(COPY ${SDL2_HEADERS} DESTINATION "${SDL2_COMPAT}")
file(COPY "${THIRD_PARTY}/SDL_image/include/SDL_image.h" DESTINATION "${SDL2_COMPAT}")
file(COPY "${THIRD_PARTY}/SDL_mixer/include/SDL_mixer.h" DESTINATION "${SDL2_COMPAT}")

# Android compatibility patch for two historical IA bugs. Keep src/main.cpp
# byte-for-byte intact and compile a generated Android-only copy instead.
set(LEGACY_MAIN "${REPO_ROOT}/src/main.cpp")
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

file(WRITE "${ANDROID_MAIN}" "${GAME_MAIN}")

add_library(main SHARED
    "${ANDROID_MAIN}"
    "${CMAKE_CURRENT_LIST_DIR}/sdl_entry.cpp"
)

target_include_directories(main PRIVATE
    "${REPO_ROOT}/src"
    "${CMAKE_BINARY_DIR}/compat"
    "${THIRD_PARTY}/SDL/include"
    "${THIRD_PARTY}/SDL_image/include"
    "${THIRD_PARTY}/SDL_mixer/include"
)

target_compile_definitions(main PRIVATE GL_GLEXT_PROTOTYPES)

target_link_libraries(main PRIVATE
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer
    android
    log
)
