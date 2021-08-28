#include "game.h"

#include <macro.h>
#include <winternal.h>
#include <pattern_scan.h>
#include <misc_utils.h>

static bool w_aob_scan(utils::ldr_data_table_entry *mod, void **out_result, const char *sig, const char *mask)
{
	auto result = utils::aob_scan(mod->dll_base, mod->size_of_image, sig, mask);
	if (!result)
		return false;

	*out_result = result;
	return true;
}

bool game::init()
{
	#pragma warning(disable: 6011)
	DEBUG_COUT("\nStarting to load values...");

	// Modules
	utils::ldr_data_table_entry *mod_unity_player;

	// Signature Results
	void *sig_player_map_coord;

	// Load modules
	DEBUG_COUT("\nLOAD MODULES:");
	if (!DEBUG_CON_C_LOG(L"UnityPlayer.dll", utils::ldr_data_table_entry_find(L"UnityPlayer.dll", mod_unity_player))
	) {
		return false;
	}

	// Load signatures
	DEBUG_COUT("\nLOAD SIGNATURES:");
	if (!DEBUG_CON_C_LOG(L"game::player_map_coords", w_aob_scan(mod_unity_player, &sig_player_map_coord,                        "\xF2\x0F\x11\x0D\x00\x00\x00\x00\x48\x83\xC4\x00\x5B\xC3\x48\x8D\x0D", "xxxx????xxx?xxxxx"))
	||  !DEBUG_CON_C_LOG(L"game::get_object",        w_aob_scan(mod_unity_player, reinterpret_cast<void **>(&game::get_object), "\x48\x8b\xc4\x48\x89\x48\x00\x55\x41\x54",                             "xxxxxx?xxx"))
	) {
		return false;
	}

	game::player_map_coords = reinterpret_cast<game::structs::player_map_coords *>(utils::calc_rel_address_32(sig_player_map_coord, 0x4));

	return true;
	#pragma warning(default: 6011)
}