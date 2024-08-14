#pragma once

enum T4PColdBreath {
	COLD_BREATH_DISABLED,
	COLD_BREATH_ENABLED_IN_COLD_ROOMS,
	COLD_BREATH_ENABLED_OUTSIDE_AND_IN_COLD_ROOMS
};

extern bool T4PlusIsRoomSwamp(ROOM_INFO *r);
extern bool T4PlusIsRoomCold(ROOM_INFO *r);
extern bool T4PlusIsRoomDamage(ROOM_INFO *r);
extern bool T4PlusDoesRoomCauseColdBreath(ROOM_INFO* r);
