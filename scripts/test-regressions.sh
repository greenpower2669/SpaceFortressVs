#!/usr/bin/env bash
set -euo pipefail
sf_repo="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
sf_test_dir="$(mktemp -d)"
trap 'rm -rf "$sf_test_dir"' EXIT

python3 "$sf_repo/scripts/prepare-assets.py" --output "$sf_test_dir/resources/assets"
python3 "$sf_repo/tests/test_assets.py"
read -r -a sf_sdl_cflags <<< "$(pkg-config --cflags sdl2 SDL2_image SDL2_mixer)"
read -r -a sf_sdl_libs <<< "$(pkg-config --libs sdl2 SDL2_image SDL2_mixer)"
g++ -std=c++17 -O1 -g -fsanitize=undefined -fno-sanitize-recover=all \
    -ffunction-sections -fdata-sections -I "$sf_repo/src" -I "$sf_repo/tests/include" \
    "${sf_sdl_cflags[@]}" "$sf_repo/tests/regressions.cpp" \
    -Wl,--gc-sections "${sf_sdl_libs[@]}" -pthread -o "$sf_test_dir/regressions"
(cd "$sf_test_dir" && ./regressions)
# Compile exactly the generated compatibility source used by Android.
(cd "$sf_test_dir" && cmake -DREPO_ROOT="$sf_repo" -P "$sf_repo/scripts/prepare-legacy-source.cmake")
g++ -std=c++17 -D__ANDROID__ -Werror=return-type -fsyntax-only -I "$sf_repo/src" \
    -I "$sf_repo/tests/include" "${sf_sdl_cflags[@]}" \
    "$sf_test_dir/generated/main_android_compat.cpp"
