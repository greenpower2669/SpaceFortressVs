#!/usr/bin/env python3
"""Name and verify the exact Android files before publishing a version."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import zipfile

ROOT = Path(__file__).resolve().parents[1]


def version():
    values = dict(line.split('=', 1) for line in
                  (ROOT / 'android/version.properties').read_text().splitlines()
                  if line and not line.startswith('#'))
    name, code = values['VERSION_NAME'], int(values['VERSION_CODE'])
    if not re.fullmatch(r'\d+\.\d+\.\d+', name) or code < 1:
        raise ValueError('Invalid Android version')
    return name, code


def digest(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def package():
    name, code = version()
    prefix = f'SpaceFortressVs-{name}'
    apk = ROOT / 'android/app/build/outputs/apk/debug/app-debug.apk'
    aab = ROOT / 'android/app/build/outputs/bundle/release/app-release.aab'
    sdk = Path(os.environ.get('ANDROID_HOME') or os.environ['ANDROID_SDK_ROOT'])
    build_tools = sdk / 'build-tools/36.0.0'
    badging = subprocess.check_output([str(build_tools / 'aapt'), 'dump', 'badging', str(apk)], text=True)
    if f"versionName='{name}'" not in badging or f"versionCode='{code}'" not in badging:
        raise ValueError('APK manifest version does not match version.properties')
    subprocess.run([str(build_tools / 'apksigner'), 'verify', str(apk)], check=True)
    for source, manifest, asset_prefix in [(apk, 'AndroidManifest.xml', 'assets/'),
                                           (aab, 'base/manifest/AndroidManifest.xml', 'base/assets/')]:
        with zipfile.ZipFile(source) as archive:
            files = archive.namelist()
            if manifest not in files or not any(p.endswith('/arm64-v8a/libmain.so') for p in files):
                raise ValueError(f'Missing Android manifest or game library: {source.name}')
            for asset in ('sun.png', 'planet.png'):
                if asset_prefix + 'resources/assets/pict/remaster/' + asset not in files:
                    raise ValueError(f'Missing scenic asset: {asset}')
            if archive.testzip() is not None:
                raise ValueError(f'Corrupt Android archive: {source.name}')
    output = ROOT / 'dist/release'
    output.mkdir(parents=True, exist_ok=True)
    destinations = []
    for source, suffix in [(apk, '.apk'), (aab, '-unsigned.aab')]:
        destination = output / (prefix + suffix)
        shutil.copy2(source, destination)
        destinations.append(destination)
    commit = os.environ.get('GITHUB_SHA') or subprocess.check_output(
        ['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip()
    metadata = {
        'game': 'SpaceFortressVs', 'version': name, 'versionCode': code, 'commit': commit,
        'apk': 'installable ARM64, debug signing', 'aab': 'release bundle, unsigned',
        'files': {p.name: {'sha256': digest(p), 'size': p.stat().st_size} for p in destinations},
    }
    build = output / (prefix + '-build.json')
    build.write_text(json.dumps(metadata, ensure_ascii=False, indent=2) + '\n')
    destinations.append(build)
    (output / 'SHA256SUMS').write_text(''.join(f'{digest(p)}  {p.name}\n' for p in destinations))
    notes = ROOT / f'docs/releases/{name}.md'
    (output / 'release-notes.md').write_text(notes.read_text() + f'\nCommit : `{commit}`.\n')
    print(f'Verified {prefix}: versionCode {code}, APK signature, ZIP integrity, game library and assets')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--metadata', action='store_true')
    args = parser.parse_args()
    if args.metadata:
        name, _ = version()
        print(f'version={name}\ntag=v{name}\nartifact=SpaceFortressVs-{name}-release-files')
    else:
        package()
