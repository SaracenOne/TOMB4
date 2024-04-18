#include "../../tomb4/pch.h"

#include "../../global/types.h"
#include "t4plus_environment.h"

#include "../mod_config.h"
#include "../../game/gameflow.h"

bool T4PlusIsRoomSwamp(ROOM_INFO* r) {
	int swamp_flag = get_game_mod_level_environment_info(gfCurrentLevel)->room_swamp_flag;

	if (swamp_flag) {
		if (r->flags & swamp_flag) {
			return true;
		}
	}

	return false;
}

bool T4PlusIsRoomCold(ROOM_INFO* r) {
	int cold_flag = get_game_mod_level_environment_info(gfCurrentLevel)->room_cold_flag;

	if (cold_flag) {
		if (r->flags & cold_flag) {
			return true;
		}
	}

	return false;
}

bool T4PlusIsRoomDamage(ROOM_INFO *r) {
	int damage_flag = get_game_mod_level_environment_info(gfCurrentLevel)->room_damage_flag;

	if (damage_flag) {
		if (r->flags & damage_flag) {
			return true;
		}
	}

	return false;
}