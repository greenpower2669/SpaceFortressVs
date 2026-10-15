#pragma once
// One engine/session host; this wrapper grants only developer capabilities.
#include "coop_solo_session_runtime.hpp"
extern "C" int runCoopSoloArena(){return runCoopSoloSession(true);}
extern "C" int runSoloGame(){return runCoopSoloSession(false);}
