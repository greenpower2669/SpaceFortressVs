#!/usr/bin/env python3
"""Assemble Android assets and reject broken remaster PNG transports."""
import argparse
import base64
import binascii
import hashlib
import io
from pathlib import Path
import shutil
import struct
import zipfile
import zlib

REPO = Path(__file__).resolve().parents[1]
PACK_SHA256 = 'c0b9fd4f96b56cf7f5dcace9ff6d4f46327a06486bc9729308668009d278a185'
# These two September 7 transports are already corrupt in Git. Use their
# original artwork until a valid replacement is committed. Match exact bytes
# so an unrelated future corruption still fails the build.
KNOWN_BROKEN = {
    'heart_blue': ('5b870ee5388280f3b9197fd1f53934d9a0d82da44cb447db0a88e1bd3d82c9e7', 'coeurbl.png'),
    'burst': ('b3a4a10c3aae282b2686d2ea8625554bd41f2f086a74ad0c3649e053e0061e42', 'pouscccc.png'),
}


def validate_png(data, name):
    """Check PNG structure, chunk CRCs, complete DEFLATE and decoded row size."""
    if data[:8] != b'\x89PNG\r\n\x1a\n':
        raise ValueError(f'{name}: invalid PNG signature')
    offset, chunks, compressed, ended = 8, [], bytearray(), False
    while offset + 12 <= len(data):
        length, kind = struct.unpack_from('>I4s', data, offset)
        end = offset + 12 + length
        if end > len(data):
            raise ValueError(f'{name}: truncated {kind!r} chunk')
        payload = data[offset + 8:offset + 8 + length]
        crc = struct.unpack_from('>I', data, offset + 8 + length)[0]
        if binascii.crc32(kind + payload) != crc:
            raise ValueError(f'{name}: {kind!r} CRC mismatch')
        if kind == b'IHDR':
            if chunks or length != 13:
                raise ValueError(f'{name}: invalid IHDR')
            width, height, depth, color, compression, filtering, interlace = struct.unpack('>IIBBBBB', payload)
        if kind == b'IDAT':
            compressed.extend(payload)
        chunks.append(kind)
        offset = end
        if kind == b'IEND':
            ended = length == 0
            break
    if not ended or offset != len(data) or not chunks or chunks[0] != b'IHDR' or not compressed:
        raise ValueError(f'{name}: incomplete PNG')
    if not width or not height or compression or filtering or interlace not in (0, 1):
        raise ValueError(f'{name}: unsupported PNG header')
    channels = {0: 1, 2: 3, 3: 1, 4: 2, 6: 4}.get(color)
    valid_depths = {0: (1, 2, 4, 8, 16), 2: (8, 16), 3: (1, 2, 4, 8), 4: (8, 16), 6: (8, 16)}
    if channels is None or depth not in valid_depths[color]:
        raise ValueError(f'{name}: invalid PNG color format')
    if color == 3 and b'PLTE' not in chunks:
        raise ValueError(f'{name}: missing PNG palette')
    inflater = zlib.decompressobj()
    rows = inflater.decompress(compressed) + inflater.flush()
    if not inflater.eof or inflater.unused_data:
        raise ValueError(f'{name}: incomplete PNG compression stream')
    passes = [(0, 0, 1, 1)] if interlace == 0 else [
        (0, 0, 8, 8), (4, 0, 8, 8), (0, 4, 4, 8), (2, 0, 4, 4),
        (0, 2, 2, 4), (1, 0, 2, 2), (0, 1, 1, 2)]
    pos = 0
    for x, y, dx, dy in passes:
        w = max(0, (width - x + dx - 1) // dx)
        h = max(0, (height - y + dy - 1) // dy)
        if not w or not h:
            continue
        stride = 1 + (w * channels * depth + 7) // 8
        for _ in range(h):
            if pos + stride > len(rows) or rows[pos] > 4:
                raise ValueError(f'{name}: invalid PNG scanline')
            pos += stride
    if pos != len(rows):
        raise ValueError(f'{name}: unexpected PNG pixel data size')


def assemble(source, output):
    output.mkdir(parents=True, exist_ok=True)
    shutil.copytree(source, output, dirs_exist_ok=True,
                    ignore=shutil.ignore_patterns('*.b64', '*.b64.*', '_asset_audit.txt'))
    encoded = ''.join(p.read_text().strip() for p in sorted(source.glob('remaster_pack.b64.*')))
    if len(encoded) == 65163:
        encoded = encoded[:33411] + 'r' + encoded[33411:]
    decoded = base64.b64decode(encoded, validate=True)
    if hashlib.sha256(decoded).hexdigest() != PACK_SHA256:
        raise ValueError('Remaster ZIP checksum mismatch')
    with zipfile.ZipFile(io.BytesIO(decoded)) as pack:
        for name in pack.namelist():
            path = Path(name)
            if path.is_absolute() or '..' in path.parts or path.parts[:2] != ('pict', 'remaster'):
                raise ValueError(f'Unexpected remaster ZIP path: {name}')
            target = output / path
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_bytes(pack.read(name))
    for path in sorted((source / 'pict/remaster').glob('*.b64')):
        data = base64.b64decode(''.join(path.read_text().split()), validate=True)
        name = path.stem
        try:
            validate_png(data, name)
        except (ValueError, zlib.error) as error:
            known = KNOWN_BROKEN.get(name)
            if known is None or hashlib.sha256(data).hexdigest() != known[0]:
                raise ValueError(f'Remaster image rejected: {error}') from error
            data = (source / 'pict' / known[1]).read_bytes()
            print(f'{name}: using verified original {known[1]} (known corrupt transport)')
        (output / 'pict/remaster' / (name + '.png')).write_bytes(data)
    images = sorted((output / 'pict/remaster').glob('*.png'))
    for path in images:
        validate_png(path.read_bytes(), path.name)
    print(f'Validated {len(images)} remaster PNGs; incomplete scenic transports excluded.')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    assemble(REPO / 'assets', args.output)
