#!/usr/bin/env python3
import pathlib
import re
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "src" / "campaign_runtime.hpp").read_text(encoding="utf-8")


def body(start: str, end: str) -> str:
    match = re.search(re.escape(start) + r"(.*?)" + re.escape(end), SOURCE, re.S)
    if not match:
        raise AssertionError(f"cannot locate {start!r} .. {end!r}")
    return match.group(1)


class CampaignDangerIntegration(unittest.TestCase):
    def test_discrete_non_kinetic_hits_use_canonical_helper(self):
        hurt = body("static void sfCoopHurt(int owner,float damage)", "static void sfCoopPattern(int pattern)")
        self.assertIn("sfApplyHostileDanger(damage)", hurt)
        self.assertNotIn("sfBossDangerMultiplier()", hurt)

    def test_ordinary_boss_contact_uses_same_helper(self):
        contact = body("static void sfCoopBossContact(int owner,float dt)", "static void sfCoopAsteroidHurt(int owner,float legacyDamage)")
        self.assertIn("sfApplyHostileDanger(650.0f)", contact)
        self.assertNotIn("sfBossDangerMultiplier()", contact)

    def test_kinetic_asteroid_path_stays_outside_danger(self):
        asteroid = body("static void sfCoopAsteroidHurt(int owner,float legacyDamage)", "static void sfCoopMovePlayers(float dt)")
        self.assertIn("const float incoming=legacyDamage", asteroid)
        self.assertNotIn("sfApplyHostileDanger", asteroid)
        self.assertNotIn("sfBossDangerMultiplier", asteroid)


if __name__ == "__main__":
    unittest.main()
