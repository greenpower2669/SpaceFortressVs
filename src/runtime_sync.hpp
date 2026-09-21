#pragma once
#include <atomic>
#include <mutex>

// Shared legacy state is guarded across rendering, simulation and input.
// Never hold this mutex while waiting for an SDL event or foreground resume.
inline std::recursive_mutex sfGameMutex;
inline bool sfGameReady = false;
inline double sfSceneSeconds = 0.0;
static void sfTacticsReset();
