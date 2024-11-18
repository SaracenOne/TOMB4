#include "../../tomb4/pch.h"

#include "../control.h"
#include "../collide.h"
#include "../lara.h"
#include "../gameflow.h"
#include "../objects.h"
#include "../effect2.h"

#include "../objects/creatures/guardian.h"
#include "../objects/creatures/hydra.h"
#include "../objects/vehicles/boat.h"

#include "../objects/general/tightrope.h"
#include "../objects/general/parallel_bars.h"

#include "../../tomb4/mod_config.h"

int32_t NGGetObjectIDForInventoryItemMemoryID(int32_t id) {
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

	int16_t rubber_boat_slot = game_mod_config.level_info[gfCurrentLevel].objects_info.rubber_boat_slot;
	if (rubber_boat_slot >= 0 && rubber_boat_slot < NUMBER_OBJECTS) {
		obj = &objects[rubber_boat_slot];
		obj->initialise = InitialiseBoat;
		obj->collision = BoatCollision;
		obj->control = RubberBoatControl;
		obj->draw_routine = DrawBoat;
		obj->save_position = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 4] |= Z_ROTATION_FLAG;
	}

	int16_t motor_boat_slot = game_mod_config.level_info[gfCurrentLevel].objects_info.motor_boat_slot;
	if (motor_boat_slot >= 0 && motor_boat_slot < NUMBER_OBJECTS) {
		obj = &objects[motor_boat_slot];
		obj->initialise = InitialiseBoat;
		obj->collision = BoatCollision;
		obj->control = MotorBoatControl;
		obj->draw_routine = DrawBoat;
		obj->save_position = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	int16_t laser_head_slot = game_mod_config.level_info[gfCurrentLevel].objects_info.laser_head_slot;
	if (laser_head_slot >= 0 && laser_head_slot < NUMBER_OBJECTS) {
		obj = &objects[laser_head_slot];
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
	}

	int16_t laser_head_base_slot = game_mod_config.level_info[gfCurrentLevel].objects_info.laser_head_base_slot;
	if (laser_head_base_slot >= 0 && laser_head_base_slot < NUMBER_OBJECTS) {
		obj = &objects[laser_head_base_slot];
		obj->control = ControlAnimatingSlots;
		obj->collision = ObjectCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_mesh = 1;
		bones[obj->bone_index] |= Y_ROTATION_FLAG;
		bones[obj->bone_index + 4] |= X_ROTATION_FLAG;
	}

	int16_t laser_head_tentacle_slot = game_mod_config.level_info[gfCurrentLevel].objects_info.laser_head_tentacle_slot;
	if (laser_head_tentacle_slot >= 0 && laser_head_tentacle_slot < NUMBER_OBJECTS) {
		obj = &objects[laser_head_tentacle_slot];
		obj->control = ControlAnimatingSlots;
		obj->collision = ObjectCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->save_mesh = 1;
		bones[obj->bone_index] |= Y_ROTATION_FLAG;
		bones[obj->bone_index + 4] |= X_ROTATION_FLAG;
	}

	int16_t hydra_slot = game_mod_config.level_info[gfCurrentLevel].objects_info.hydra_slot;
	if (hydra_slot >= 0 && hydra_slot < NUMBER_OBJECTS) {
		obj = &objects[hydra_slot];
		obj->initialise = InitialiseHydra;
		obj->control = HydraControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 30;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->bite_offset = 1024;
		obj->object_mip = (BLOCK_SIZE * 5);
		obj->intelligent = 1;
		obj->HitEffect = 3;
		obj->undead = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index] |= Y_ROTATION_FLAG;
		bones[obj->bone_index + 32] |= Y_ROTATION_FLAG;
		bones[obj->bone_index + 32] |= X_ROTATION_FLAG;
		bones[obj->bone_index + 32] |= Z_ROTATION_FLAG;
	}

	int16_t hydra_missile_slot = game_mod_config.level_info[gfCurrentLevel].objects_info.hydra_missile_slot;
	if (hydra_missile_slot >= 0 && hydra_missile_slot < NUMBER_OBJECTS) {
		obj = &objects[hydra_missile_slot];
		obj->control = ControlEnemyMissile;
		obj->draw_routine = (void(*)(ITEM_INFO*))1;
		obj->nmeshes = 0;
		obj->loaded = 1;
	}
}