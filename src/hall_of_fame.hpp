#pragma once

#include <algorithm>
#include <cstdlib>
#include <string>

// Hall of Fame contract requested by Fab.
// Saved boss numbers are real campaign encounters in the inclusive 1..200 range.
// Danger is the REAL selected Boss Danger, persisted as 1..9. Zero means a
// legacy entry created before danger persistence existed and must not be guessed.
static std::string sfFameStars(int danger)
{
    if (danger<1 || danger>9) return "?";
    return std::string(danger,'*');
}

// Canonical formula:
// Points = |boss * (danger - minutes)| + boss * (danger - minutes)
// Evaluate minutes from the recorded seconds so 2:30 means exactly 2.5 minutes.
static int sfFamePoints(int boss,int danger,int seconds)
{
    if (danger<1 || danger>9) return 0;
    const long long safeBoss=std::clamp(boss,1,200);
    const long long safeSeconds=std::max(seconds,0);
    const long long scaled=safeBoss*(long long(danger)*60LL-safeSeconds);
    return int((std::llabs(scaled)+scaled)/60LL);
}

static bool sfFameRanksBefore(int bossA,int dangerA,int secondsA,int bossB,int dangerB,int secondsB)
{
    const int pointsA=sfFamePoints(bossA,dangerA,secondsA);
    const int pointsB=sfFamePoints(bossB,dangerB,secondsB);
    if (pointsA!=pointsB) return pointsA>pointsB;
    if (secondsA!=secondsB) return secondsA<secondsB;
    return false; // stable_sort preserves the durable save order for exact ties.
}

static std::string sfFameTime(int seconds)
{
    seconds=std::max(seconds,0);
    const int minutes=seconds/60,remainder=seconds%60;
    return std::to_string(minutes)+":"+(remainder<10 ? "0" : "")+std::to_string(remainder);
}
