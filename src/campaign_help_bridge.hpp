#pragma once

// Small bridge kept after campaign_runtime.hpp so remaster_ai_fix.hpp can ask
// the campaign to pause/resume for the in-game help screen without knowing the
// campaign state's private enum/struct layout.
static bool sfCampaignPauseForHelp()
{
    if (!sfIsCoop() || sfCoop.phase!=SfCoopPhase::Combat) return false;
    sfCampaignSuspend();
    return true;
}

static void sfCampaignResumeFromHelp()
{
    if (sfIsCoop() && sfCoop.phase==SfCoopPhase::Paused) sfCoop.phase=SfCoopPhase::Combat;
}
