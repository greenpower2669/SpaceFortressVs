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

    def test_danger_does_not_change_attack_generation_or_player_fire(self):
        pattern = body("static void sfCoopPattern(int pattern)", "static float sfCoopRisk(tupl position,tuplv velocity,Uint64 ignoredAsteroidId=0)")
        fire = body("static bool sfCoopFire(int owner)", "static constexpr float SF_COOP_CHARGE_DURATION")
        tick = body("static void sfCoopTick(float dt)", "static void sfCampaignStart()")
        for section in (pattern, fire, tick):
            self.assertNotIn("sfBossDanger", section)
            self.assertNotIn("sfApplyHostileDanger", section)


if __name__ == "__main__":
    unittest.main()
