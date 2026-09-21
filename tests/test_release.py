"""Check that publication rejects tampering and never replaces another build."""
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

spec = importlib.util.spec_from_file_location('publisher', Path(__file__).resolve().parents[1] / 'scripts/publish-release.py')
publisher = importlib.util.module_from_spec(spec)
spec.loader.exec_module(publisher)


class ReleaseGuards(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.folder = Path(self.temp.name)
        self.apk = self.folder / 'SpaceFortressVs-1.2.0.apk'
        self.apk.write_bytes(b'verified fixture')
        self.manifest = self.folder / 'SpaceFortressVs-1.2.0-build.json'
        self.data = {'version': '1.2.0', 'commit': 'a'*40, 'files': {
            self.apk.name: {'sha256': hashlib.sha256(self.apk.read_bytes()).hexdigest(),
                            'size': self.apk.stat().st_size}}}
        self.manifest.write_text(json.dumps(self.data))
        env = patch.dict(os.environ, {'GITHUB_SHA': 'a'*40, 'GITHUB_REPOSITORY': 'owner/game'})
        env.start(); self.addCleanup(env.stop)

    def test_tampered_asset_never_reaches_github(self):
        self.apk.write_bytes(b'modified fixture')
        with patch.object(publisher, 'gh') as remote:
            with self.assertRaisesRegex(ValueError, 'checksum'):
                publisher.publish(self.folder)
            remote.assert_not_called()

    def test_different_build_never_reaches_github(self):
        self.data['commit'] = 'b'*40
        self.manifest.write_text(json.dumps(self.data))
        with patch.object(publisher, 'gh') as remote:
            with self.assertRaisesRegex(ValueError, 'workflow commit'):
                publisher.publish(self.folder)
            remote.assert_not_called()

    def test_existing_version_is_not_replaced(self):
        response = {'id': 42, 'target_commitish': 'b'*40, 'assets': [], 'draft': False}
        with patch.object(publisher, 'gh', return_value=json.dumps(response)) as remote:
            with self.assertRaisesRegex(ValueError, 'another commit'):
                publisher.publish(self.folder)
            self.assertEqual(remote.call_count, 1)
            self.assertEqual(remote.call_args.args[:1], ('api',))
            self.assertNotIn('--method', remote.call_args.args)


if __name__ == '__main__':
    unittest.main()
