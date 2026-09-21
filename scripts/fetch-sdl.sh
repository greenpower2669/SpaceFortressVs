#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
THIRD_PARTY="${ROOT}/third_party"
mkdir -p "${THIRD_PARTY}"

clone_repo() {
  local url="$1"
  local tag="$2"
  local dest="$3"

  if [[ -d "${dest}/.git" ]]; then
    echo "Using existing $(basename "${dest}")"
    return
  fi

  rm -rf "${dest}"
  git clone \
    --depth 1 \
    --branch "${tag}" \
    --recurse-submodules \
    --shallow-submodules \
    "${url}" "${dest}"
}

clone_repo "https://github.com/libsdl-org/SDL.git" \
  "release-2.32.10" "${THIRD_PARTY}/SDL"

clone_repo "https://github.com/libsdl-org/SDL_image.git" \
  "release-2.8.12" "${THIRD_PARTY}/SDL_image"

clone_repo "https://github.com/libsdl-org/SDL_mixer.git" \
  "release-2.8.2" "${THIRD_PARTY}/SDL_mixer"

echo "SDL2 dependencies ready."
