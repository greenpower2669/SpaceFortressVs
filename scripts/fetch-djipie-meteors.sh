#!/usr/bin/env bash
# Stage exactly four licensed private Djipie Release WAVs in CI ONLY.
# DO NOT commit raw files, print credentials or upload standalone audio.
set -euo pipefail
: "${GH_TOKEN:?Set the private Djipie read secret DJIPIE_READ_TOKEN}"
dir="assets/sounds"
tag="djipie-audio-prive-fx-explosions"
repo="greenpower2669/Djipie"
names=(
  Meteor_explosion_rock_contre_rock1_001.wav
  Meteor_explosion_rock_contre_rock2_002.wav
  Meteor_explosion_rock_contre_rock3_003.wav
  Meteor_explosion_rock_contre_rock4_004.wav
)
hashes=(
  98d08994f89f1f246158b74f4b1d0dc5d9edce79134d6d1a109bef3193b895d0
  2e4bec14e47c1cdaf657ce1b0d9a2a0faab99f89e34d2fc6d688243a7111bab4
  b69ce378105355f2357e29b16f86aa2b7b776bf2bab9202e142401aca058aab3
  aa68017c13662e71666473e243151463042d44d78d7fcdb11772524bd00b4de4
)
sizes=(321092 299924 317564 306980)
for i in 0 1 2 3; do
  filename="${names[$i]}"
  # gh handles the private Release's authenticated redirect.
  gh release download "$tag" --repo "$repo" --pattern "$filename" --dir "$dir" --clobber
  echo "${hashes[$i]}  $dir/$filename" | sha256sum --check --status
  test "$(stat -c '%s' "$dir/$filename")" -eq "${sizes[$i]}"
  python3 - "$dir/$filename" <<'PY'
from pathlib import Path
import sys
p=Path(sys.argv[1]).read_bytes()
assert p[:4] in (b'RIFF',b'RF64') and p[8:12] == b'WAVE', 'not WAV'
PY
done
echo "SOLO_025: verified all four private meteor WAVs for this build (not committed)"
