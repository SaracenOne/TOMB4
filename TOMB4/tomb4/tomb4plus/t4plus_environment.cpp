#include "../../tomb4/pch.h"

#include "../../global/types.h"
#include "t4plus_environment.h"

#include "../mod_config.h"

bool T4PlusIsRoomSwamp(ROOM_INFO* r) {
	if (get_game_mod_global_info()->trng_swamp_room_type_enabled) {
		if (r->flags & ROOM_SWAMP) {
			return true;
		}
	}

	return false;
}