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
package_spec = importlib.util.spec_from_file_location('packager', Path(__file__).resolve().parents[1] / 'scripts/package-release.py')
packager = importlib.util.module_from_spec(package_spec)
package_spec.loader.exec_module(packager)


class SignatureContinuity(unittest.TestCase):
    def test_same_certificate_and_package_allow_an_update(self):
        status = packager.update_identity('com.greenpower2669.spacefortressvs', 10,
            '8abfc11c8bc4f9ac065eb5c086ad4e457290bcbbc1105017865368de7e565868')
        self.assertTrue(status['compatibleWithV131'])

    def test_ephemeral_debug_key_is_not_an_update(self):
        status = packager.update_identity('com.greenpower2669.spacefortressvs', 10, 'a'*64)
        self.assertFalse(status['compatibleWithV131'])
        self.assertIn('SIGNATURE', status['message'])

    def test_matching_key_does_not_hide_wrong_package_or_version(self):
        certificate = '8abfc11c8bc4f9ac065eb5c086ad4e457290bcbbc1105017865368de7e565868'
        for package, code in [('other.app', 10), ('com.greenpower2669.spacefortressvs', 8)]:
            with self.subTest(package=package, code=code), self.assertRaises(ValueError):
                packager.update_identity(package, code, certificate)


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
