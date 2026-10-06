from pathlib import Path
import subprocess

campaign=Path("src/campaign_runtime.hpp").read_text()
boss=Path("src/boss_difficulty_visuals.hpp").read_text()
remaster=Path("src/remaster_runtime.hpp").read_text()
compat=Path("scripts/prepare-legacy-source.cmake").read_text()
assert 'sfDrawCampaignSpace(renderer,sfCoop.encounter' in campaign
assert 'sfDrawCampaignSpace(renderer,sfCoop.boss' not in campaign
assert 'sfBossTravelProgress(sfCoop.encounter)' in campaign
assert 'sfScenicProfileForEncounter(encounter)' in campaign
assert 'sfProgressRgb(encounter)' in boss
assert 'sfScenicBagNext' in remaster
assert 'campaign/nebulae.png' in remaster and 'campaign/planets.png' in remaster
assert 'sfRmDrawClassicScenicMap(renderer);' in compat
blob=subprocess.check_output(['git','hash-object','src/main.cpp'],text=True).strip()
assert blob == '835059a0ecfe0f74708068b3259cad5db1cdb579', blob
