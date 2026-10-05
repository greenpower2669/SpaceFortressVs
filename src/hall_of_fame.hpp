#pragma once

#include <algorithm>
#include <cstdlib>
#include <string>

// Hall of Fame contract requested by Fab.
// Saved boss numbers are real campaign encounters in the inclusive 1..200 range.
static int sfFameDanger(int boss)
{
    return (std::clamp(boss,1,200)-1)/50+1;
}

static std::string sfFameStars(int danger)
{
    return std::string(std::clamp(danger,1,4),'*');
}

// Canonical formula:
// Points = |boss * (danger - minutes)| + boss * (danger - minutes)
// Evaluate minutes from the recorded seconds so 2:30 means exactly 2.5 minutes.
static int sfFamePoints(int boss,int seconds)
{
    const long long safeBoss=std::clamp(boss,1,200);
    const long long safeSeconds=std::max(seconds,0);
    const long long danger=sfFameDanger(int(safeBoss));
    const long long scaled=safeBoss*(danger*60LL-safeSeconds);
    return int((std::llabs(scaled)+scaled)/60LL);
}

static bool sfFameRanksBefore(int bossA,int secondsA,int bossB,int secondsB)
{
    const int pointsA=sfFamePoints(bossA,secondsA);
    const int pointsB=sfFamePoints(bossB,secondsB);
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
