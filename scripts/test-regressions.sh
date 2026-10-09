#!/usr/bin/env bash
set -euo pipefail
sf_repo="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
sf_test_dir="$(mktemp -d)"
trap 'rm -rf "$sf_test_dir"' EXIT

python3 "$sf_repo/scripts/prepare-assets.py" --output "$sf_test_dir/resources/assets"
python3 "$sf_repo/tests/test_assets.py"
g++ -std=c++17 -O1 -I "$sf_repo/src" "$sf_repo/tests/solo_campaign_model_regressions.cpp" -o "$sf_test_dir/solo-model"
"$sf_test_dir/solo-model"
g++ -std=c++17 -O1 -I "$sf_repo/src" "$sf_repo/tests/solo_map_generator_regressions.cpp" -o "$sf_test_dir/solo-map"
g++ -std=c++17 -O1 -I "$sf_repo/src" "$sf_repo/tests/solo_session_regressions.cpp" -o "$sf_test_dir/solo-session"
"$sf_test_dir/solo-session"
"$sf_test_dir/solo-map"
python3 "$sf_repo/tests/test_release.py"
python3 "$sf_repo/tests/test_scenic_integration.py"
python3 "$sf_repo/tests/test_kinetic_integration.py"
python3 "$sf_repo/tests/test_kinetic_surge_integration.py"
python3 "$sf_repo/tests/test_balance_v3_integration.py"
python3 "$sf_repo/tests/test_campaign_danger_integration.py"
g++ -std=c++17 -O1 -I "$sf_repo/src" "$sf_repo/tests/danger9_regressions.cpp" -o "$sf_test_dir/danger9"
"$sf_test_dir/danger9"
g++ -std=c++17 -O1 -I "$sf_repo/src" "$sf_repo/tests/kinetic_surge_regressions.cpp" -o "$sf_test_dir/kinetic-surge"
"$sf_test_dir/kinetic-surge"
g++ -std=c++17 -O1 -I "$sf_repo/src" "$sf_repo/tests/kinetic_balance_v3_regressions.cpp" -o "$sf_test_dir/kinetic-balance-v3"
"$sf_test_dir/kinetic-balance-v3"
g++ -std=c++17 -O1 -I "$sf_repo/src" "$sf_repo/tests/scenic_mix_regressions.cpp" -o "$sf_test_dir/scenic-mix"
g++ -std=c++17 -O1 -I "$sf_repo/src" "$sf_repo/tests/kinetic_regressions.cpp" -o "$sf_test_dir/kinetic"
"$sf_test_dir/kinetic"
"$sf_test_dir/scenic-mix"
g++ -std=c++17 -O1 -ffunction-sections -fdata-sections -I "$sf_repo/src" \
    "$sf_repo/tests/campaign_format_regressions.cpp" -Wl,--gc-sections -o "$sf_test_dir/campaign-format"
"$sf_test_dir/campaign-format"
g++ -std=c++17 -O1 -I "$sf_repo/src" "$sf_repo/tests/hall_sync_regressions.cpp" -o "$sf_test_dir/hall-sync"
"$sf_test_dir/hall-sync"
read -r -a sf_sdl_cflags <<< "$(pkg-config --cflags sdl2 SDL2_image SDL2_mixer)"
read -r -a sf_sdl_libs <<< "$(pkg-config --libs sdl2 SDL2_image SDL2_mixer)"
g++ -std=c++17 -O1 -g -ffunction-sections -fdata-sections -I "$sf_repo/src" -I "$sf_repo/tests/include" \
    "${sf_sdl_cflags[@]}" "$sf_repo/tests/classic_danger_regressions.cpp" \
    -Wl,--gc-sections "${sf_sdl_libs[@]}" -pthread -o "$sf_test_dir/classic-danger"
(cd "$sf_test_dir" && ./classic-danger)
g++ -std=c++17 -O1 -g -ffunction-sections -fdata-sections -I "$sf_repo/src" -I "$sf_repo/tests/include" \
    "${sf_sdl_cflags[@]}" "$sf_repo/tests/help_runtime_regressions.cpp" \
    -Wl,--gc-sections "${sf_sdl_libs[@]}" -pthread -o "$sf_test_dir/help-runtime"
(cd "$sf_test_dir" && ./help-runtime)
g++ -std=c++17 -O1 -g -ffunction-sections -fdata-sections -I "$sf_repo/src" -I "$sf_repo/tests/include" \
    "${sf_sdl_cflags[@]}" "$sf_repo/tests/help_live_regressions.cpp" \
    -Wl,--gc-sections "${sf_sdl_libs[@]}" -pthread -o "$sf_test_dir/help-live"
(cd "$sf_test_dir" && ./help-live)
g++ -std=c++17 -O1 -g -ffunction-sections -fdata-sections -I "$sf_repo/src" -I "$sf_repo/tests/include" \
    "${sf_sdl_cflags[@]}" "$sf_repo/tests/tutorial_regressions.cpp" \
    -Wl,--gc-sections "${sf_sdl_libs[@]}" -pthread -o "$sf_test_dir/tutorial"
(cd "$sf_test_dir" && ./tutorial)
g++ -std=c++17 -O1 -g -D_GLIBCXX_DEBUG -fsanitize=undefined -fno-sanitize-recover=all \
    -ffunction-sections -fdata-sections -I "$sf_repo/src" -I "$sf_repo/tests/include" \
    "${sf_sdl_cflags[@]}" "$sf_repo/tests/regressions.cpp" \
    -Wl,--gc-sections "${sf_sdl_libs[@]}" -pthread -o "$sf_test_dir/regressions"
(cd "$sf_test_dir" && ./regressions)
(cd "$sf_test_dir" && cmake -DREPO_ROOT="$sf_repo" -P "$sf_repo/scripts/prepare-legacy-source.cmake")
(cd "$sf_test_dir" && cmake -DREPO_ROOT="$sf_repo" -P "$sf_repo/scripts/patch-classic-danger.cmake")
grep -Fq "::setw(static_cast<float>(DM.w));seth(DM.h);" "$sf_test_dir/generated/main_android_compat.cpp"
grep -Fq "sfClassicIncomingNonKinetic(e,Spritej2,misspvminus(Spritej2))" "$sf_test_dir/generated/main_android_compat.cpp"
grep -Fq "sfClassicIncomingNonKinetic(e,Spritej2,pvminus(Spritej2))" "$sf_test_dir/generated/main_android_compat.cpp"
test "$(grep -Fc 'sfClassicIncomingNonKinetic(' "$sf_test_dir/generated/main_android_compat.cpp")" -eq 2
g++ -std=c++17 -D__ANDROID__ -Werror=return-type -fsyntax-only -I "$sf_repo/src" \
    -I "$sf_repo/tests/include" "${sf_sdl_cflags[@]}" \
    "$sf_test_dir/generated/main_android_compat.cpp"
g++ -std=c++17 -O1 -g -D_GLIBCXX_DEBUG -fsanitize=undefined -fno-sanitize-recover=all \
    -ffunction-sections -fdata-sections -I "$sf_repo/src" -I "$sf_repo/tests/include" \
    -I "$sf_test_dir/generated" "${sf_sdl_cflags[@]}" "$sf_repo/tests/legacy_field_regressions.cpp" \
    -Wl,--gc-sections "${sf_sdl_libs[@]}" -pthread -o "$sf_test_dir/legacy-field-regressions"
(cd "$sf_test_dir" && ./legacy-field-regressions)
g++ -std=c++17 -O1 -g -D_GLIBCXX_DEBUG -fsanitize=undefined -fno-sanitize-recover=all \
    -ffunction-sections -fdata-sections -I "$sf_repo/src" -I "$sf_repo/tests/include" \
    -I "$sf_test_dir/generated" "${sf_sdl_cflags[@]}" "$sf_repo/tests/kinetic_dust_regressions.cpp" \
    -Wl,--gc-sections "${sf_sdl_libs[@]}" -pthread -o "$sf_test_dir/kinetic-dust-regressions"
(cd "$sf_test_dir" && ./kinetic-dust-regressions)
