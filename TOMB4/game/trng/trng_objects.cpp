#include "../../tomb4/pch.h"

#include "../control.h"
#include "../collide.h"
#include "../lara.h"
#include "../gameflow.h"
#include "../objects.h"

#include "objects/trng_tightrope.h"
#include "objects/trng_parallel_bars.h"
#include "../../tomb4/mod_config.h"
#include "objects/trng_boat.h"

int NGGetObjectIDForInventoryItemMemoryID(int id) {
	switch (id) {
		case 0:
			return UZI_ITEM;
		case 1:
			return PISTOLS_ITEM;
		case 2:
			return SHOTGUN_ITEM;
		case 3:
			return SIXSHOOTER_ITEM;
		default:
			return NO_ITEM;
	}
}

void NGSetupNGObjects() {
	OBJECT_INFO* obj;

	obj = &objects[TIGHT_ROPE];
	obj->initialise = InitialiseTightRope;
	obj->collision = TightRopeCollision;
	obj->draw_routine = 0;
	obj->using_drawanimating_item = 0;
	obj->save_flags = 1;

	obj = &objects[PARALLEL_BARS];
	obj->collision = ParallelBarsCollision;

	obj = &objects[RUBBER_BOAT];
	obj->initialise = InitialiseBoat;
	obj->collision = BoatCollision;
	obj->control = RubberBoatControl;
	obj->draw_routine = DrawBoat;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;
	bones[obj->bone_index + 4] |= 0x10;

	obj = &objects[MOTOR_BOAT];
	obj->initialise = InitialiseBoat;
	obj->collision = BoatCollision;
	obj->control = MotorBoatControl;
	obj->draw_routine = DrawBoat;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[game_mod_config.level_info[gfCurrentLevel].objects_info.rubber_boat_slot];
	obj->initialise = InitialiseBoat;
	obj->collision = BoatCollision;
	obj->control = RubberBoatControl;
	obj->draw_routine = DrawBoat;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;
	bones[obj->bone_index + 4] |= 0x10;

	obj = &objects[game_mod_config.level_info[gfCurrentLevel].objects_info.motor_boat_slot];
	obj->initialise = InitialiseBoat;
	obj->collision = BoatCollision;
	obj->control = MotorBoatControl;
	obj->draw_routine = DrawBoat;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;
}