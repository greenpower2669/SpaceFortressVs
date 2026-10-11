#include <iostream>
//#include <chrono>
//#include <thread>
#include <unistd.h>
#include <cstdlib>
#include <chrono>
//
#include <list>
//#include <t.cs>
#include <cmath>

extern bool sdlstarted;
//#include <boost/thread/thread.hpp>
//#include <ctime>




 
	int w, h; // texture width & height
	int pw, ph; // transporteur xy sur event
	
	
	// Initialize 
	

void th2()
{
    
    
    std::cout << "  -2-  ";//<< std::endl;
  
    
 }

	

// Android compatibility shim for the historical ./resources/assets/... paths.
// It only changes the Android loading boundary; desktop/source paths stay intact.
#include <android_asset_compat.hpp>
#include <runtime_sync.hpp>
#include <game_mode.hpp>

// 2026 UI shim: procedural start/help screens and input interception.
// Included here (after t.hpp in main.cpp) so SDL and sprite are already defined.
#include <start_ui.hpp>
#include <help_format_bridge.hpp>
#include <tutorial_runtime.hpp>

// 2026 remaster runtime: gear gesture, VFX and occasional shooting stars.
#include <remaster_runtime.hpp>

// Restore the historical rich galaxy rendering while keeping the remaster VFX.
#include <remaster_visual_restore.hpp>

// Final Android bridge: remove the remaster velocity override, serialize menu
// -> game IA state changes on the render thread and reset the historical IA
// cascade at every new match.
#include <remaster_ai_fix.hpp>

// Final framing/render pass: complete round sun/planet, no dark aura fringe,
// orange+blue gear art and no visually truncated ring fortresses.
#include <remaster_frame_fix.hpp>

// Invalidate remaster texture references when the legacy renderer is recreated.
#include <remaster_lifecycle_fix.hpp>

// Guarded Android source patches use these gameplay safety helpers.
#include <legacy_game_safety.hpp>
#include <tactical_runtime.hpp>
#include <kinetic_energy_visuals.hpp>
#include <classic_danger_runtime.hpp>
#include <legacy_field_primitives.hpp>
#include <legacy_field_runtime.hpp>
#include <classic_duel_surge.hpp>

// Preserve the campaign renderer byte-for-byte and replace only the visible
// Hall of Fame entry point. start_ui.hpp already calls sfCampaignDrawHall()
// through game_mode.hpp's forward declaration; the legacy body stays available
// for regression archaeology under its explicit legacy name.
#define sfCampaignDrawHall sfCampaignDrawHallLegacy
#include <campaign_runtime.hpp>
#undef sfCampaignDrawHall
#include <hall_of_fame_runtime.hpp>

// The final help bridge is intentionally last: it needs the fully defined
// campaign state to pause/resume a live match without restarting it.
#include <help_live_bridge.hpp>

// Automatic Hall sync observes final input routing after the help/campaign
// bridge. It never adds network work to the renderer and leaves main.cpp intact.
#include <hall_sync_ui_bridge.hpp>

// Diagnostic COOP/SOLO lives INSIDE the historical single translation unit
// (FabLib globals stay unique), but must NOT inherit the historical UI's
// SDL_RenderPresent macro. That wrapper calls sfFixApplyUiRequests(), whose
// default requested screen is HOME, then paints the CLASSIC selection menu
// OVER the already-running SOLO arena. This is the Samsung sound-but-menu bug.
//
// Scope the real SDL presentation API only to the diagnostic header. Restore
// the macro immediately afterward: Classic/COOP/Help retain the *exact* same
// present interception and navigation semantics.
#ifdef SDL_RenderPresent
#pragma push_macro("SDL_RenderPresent")
#undef SDL_RenderPresent
#define SF_COOP_SOLO_RESTORE_PRESENT_HOOK 1
#endif
#include <coop_solo_arena_runtime.hpp>
#ifdef SF_COOP_SOLO_RESTORE_PRESENT_HOOK
#pragma pop_macro("SDL_RenderPresent")
#undef SF_COOP_SOLO_RESTORE_PRESENT_HOOK
#endif
