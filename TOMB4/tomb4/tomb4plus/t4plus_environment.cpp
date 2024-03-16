#include "../../tomb4/pch.h"

#include "../../global/types.h"
#include "t4plus_environment.h"

bool T4PlusIsRoomSwamp(ROOM_INFO* r) {
	if (r->flags & ROOM_SWAMP) {
		return true;
	}

	return false;
}