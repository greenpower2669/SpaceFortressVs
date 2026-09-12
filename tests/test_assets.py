import base64
import importlib.util
from pathlib import Path
import unittest
import zlib

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('assets', ROOT / 'scripts/prepare-assets.py')
assets = importlib.util.module_from_spec(spec)
spec.loader.exec_module(assets)


class AssetTests(unittest.TestCase):
    def test_original_replacements_are_valid(self):
        for _, filename in assets.KNOWN_BROKEN.values():
            assets.validate_png((ROOT / 'assets/pict' / filename).read_bytes(), filename)

    def test_recent_corrupt_transports_are_rejected(self):
        for name in assets.KNOWN_BROKEN:
            path = ROOT / 'assets/pict/remaster' / (name + '.b64')
            data = base64.b64decode(''.join(path.read_text().split()))
            with self.subTest(name=name), self.assertRaises((ValueError, zlib.error)):
                assets.validate_png(data, name)

    def test_truncation_and_changed_pixel_bytes_are_rejected(self):
        data = (ROOT / 'assets/pict/remaster/planet.png').read_bytes()
        assets.validate_png(data, 'planet')
        corrupt = bytearray(data)
        corrupt[data.index(b'IDAT') + 10] ^= 1
        for broken in (data[:-5], bytes(corrupt), data + b'trailing'):
            with self.assertRaises(ValueError):
                assets.validate_png(broken, 'damaged planet')


if __name__ == '__main__':
    unittest.main()
