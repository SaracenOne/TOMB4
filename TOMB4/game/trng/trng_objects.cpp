#include "../../tomb4/pch.h"

#include "../control.h"
#include "../collide.h"
#include "../lara.h"
#include "../gameflow.h"
#include "../objects.h"

#include "objects/trng_tightrope.h"
#include "objects/trng_parallel_bars.h"
#include "objects/trng_boat.h"
#include "objects/trng_guardian.h"

#include "../../tomb4/mod_config.h"

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
	bones[obj->bone_index + 4] |= Z_ROTATION_FLAG;

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
	bones[obj->bone_index + 4] |= Z_ROTATION_FLAG;

	obj = &objects[game_mod_config.level_info[gfCurrentLevel].objects_info.motor_boat_slot];
	obj->initialise = InitialiseBoat;
	obj->collision = BoatCollision;
	obj->control = MotorBoatControl;
	obj->draw_routine = DrawBoat;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[game_mod_config.level_info[gfCurrentLevel].objects_info.laser_head_slot];
	obj->initialise = InitialiseGuardian;
	obj->control = GuardianControl;
	obj->collision = CreatureCollision;
	obj->explodable_meshbits = 6;
	obj->HitEffect = 3;
	obj->save_position = 1;
	obj->save_hitpoints = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;
	obj->save_mesh = 1;

	obj = &objects[game_mod_config.level_info[gfCurrentLevel].objects_info.laser_head_base_slot];
	obj->control = ControlAnimatingSlots;
	obj->collision = ObjectCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[game_mod_config.level_info[gfCurrentLevel].objects_info.laser_head_tentacle_slot];
	obj->control = ControlAnimatingSlots;
	obj->collision = ObjectCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;
}