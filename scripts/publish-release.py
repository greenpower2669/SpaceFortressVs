#!/usr/bin/env python3
"""Publish verified files; never replace a different commit or existing asset."""
import hashlib
import json
import os
from pathlib import Path
import subprocess


def gh(*args, payload=None):
    return subprocess.run(['gh', *args], check=True, capture_output=True, text=True,
                          input=json.dumps(payload) if payload is not None else None).stdout


def api(path):
    return json.loads(gh('api', path))


def publish(folder):
    metadata_files = list(folder.glob('SpaceFortressVs-*-build.json'))
    if len(metadata_files) != 1:
        raise ValueError('Expected exactly one build manifest')
    metadata = json.loads(metadata_files[0].read_text())
    if metadata['commit'] != os.environ['GITHUB_SHA']:
        raise ValueError('Release files do not belong to this workflow commit')
    for name, expected in metadata['files'].items():
        path = folder / name
        if path.name != name or path.stat().st_size != expected['size']:
            raise ValueError('Invalid release asset path or size')
        with path.open('rb') as stream:
            if hashlib.file_digest(stream, 'sha256').hexdigest() != expected['sha256']:
                raise ValueError('Release asset checksum mismatch')
    tag = 'v' + metadata['version']
    repo = os.environ['GITHUB_REPOSITORY']
    endpoint = f'repos/{repo}/releases/tags/{tag}'
    try:
        release = api(endpoint)
    except subprocess.CalledProcessError as error:
        if '404' not in error.stderr:
            raise
        # Drafts may not be returned by the tag endpoint. A retry can resume a
        # matching draft without ever overwriting another version's assets.
        drafts = [r for r in api(f'repos/{repo}/releases?per_page=100') if r['tag_name'] == tag]
        if len(drafts) > 1:
            raise ValueError('Multiple releases use this version; refusing an ambiguous update')
        release = drafts[0] if drafts else json.loads(gh(
            'api', '--method', 'POST', f'repos/{repo}/releases', '--input', '-', payload={
                'tag_name': tag, 'target_commitish': metadata['commit'],
                'name': f"SpaceFortressVs {metadata['version']}",
                'body': (folder / 'release-notes.md').read_text(), 'draft': True,
            }))
    endpoint = f"repos/{repo}/releases/{release['id']}"
    if release['target_commitish'] != metadata['commit']:
        raise ValueError('This version already refers to another commit; increment the version')
    files = [folder / name for name in metadata['files']] + [metadata_files[0], folder / 'SHA256SUMS']
    existing = {a['name']: a for a in release['assets']}
    for path in files:
        with path.open('rb') as stream:
            expected = 'sha256:' + hashlib.file_digest(stream, 'sha256').hexdigest()
        asset = existing.get(path.name)
        if asset:
            if asset.get('digest') != expected or asset['state'] != 'uploaded':
                raise ValueError('Existing release asset differs; refusing replacement')
        elif not release['draft']:
            raise ValueError('Published release is incomplete; refusing to silently change it')
        else:
            gh('release', 'upload', tag, str(path), '--repo', repo)
    # Check all uploaded bytes before making the release public.
    verified = {a['name']: a for a in api(endpoint)['assets']}
    for path in files:
        with path.open('rb') as stream:
            expected = 'sha256:' + hashlib.file_digest(stream, 'sha256').hexdigest()
        if verified.get(path.name, {}).get('digest') != expected:
            raise ValueError('Uploaded release checksum could not be verified')
    if release['draft']:
        gh('release', 'edit', tag, '--repo', repo, '--draft=false', '--latest')
    print(api(endpoint)['html_url'])


if __name__ == '__main__':
    publish(Path('dist/release'))
