#include "../../tomb4/pch.h"

#include "../../specific/function_stubs.h"
#include "../control.h"
#include "../effects.h"
#include "../objects.h"
#include "trng.h"
#include "trng_extra_state.h"

// Move the item in a direction by the number of units
void NGMoveItemByUnits(unsigned short item_id, NG_DIRECTIONS direction, unsigned int units) {
	switch (direction) {
	case NG_NORTH: {
		items[item_id].pos.z_pos += units;
		return;
	}
	case NG_EAST: {
		items[item_id].pos.x_pos += units;
		return;
	}
	case NG_SOUTH: {
		items[item_id].pos.z_pos -= units;
		return;
	}
	case NG_WEST: {
		items[item_id].pos.x_pos += units;
		return;
	}
	case NG_UP: {
		items[item_id].pos.y_pos -= units;
		return;
	}
	case NG_DOWN: {
		items[item_id].pos.y_pos += units;
		return;
	}
	}
}

void NGForceItemAnimation(unsigned short item_id, unsigned int animation) {
	items[item_id].anim_number = objects[items[item_id].object_number].anim_index + animation;
	items[item_id].frame_number = anims[items[item_id].anim_number].frame_base;
}

void NGHurtEnemy(unsigned short item_id, unsigned short damage) {
	if (items[item_id].hit_points > 0) {
		items[item_id].hit_points -= damage;
	}
}

void NGActionTrigger(unsigned short param, unsigned short extra) {
	unsigned char action_type = (unsigned char)extra & 0xff;
	unsigned char action_data = (unsigned char)(extra >> 8) & 0xff;

	switch (action_type) {
		case PERFORM_FLIPEFFECT_ON_ITEM: {
			effect_routines[flipeffect](&items[param]);
			break;
		}
		case FREEZE_ENEMY_FOR_SECONDS: {
			unsigned short item_id = param;
			if (!NGIsItemFrozen(param)) {
				if (!NGCheckFloorStatePressedThisFrameOrLastFrame()) {
					if (action_data == 0) {
						NGSetItemFreezeTimer(item_id, -1);
					}
					else {
						NGSetItemFreezeTimer(item_id, action_data * 30);
					}
				}
			}
			break;
		}
		case UNFREEZE_ENEMY_WITH_EFFECT: {
			if (!NGCheckFloorStatePressedThisFrameOrLastFrame()) {
				unsigned short item_id = param;
				if (action_data != 0x00) {
					Log(0, "Unimplemented action data for UNFREEZE_ENEMY_WITH_EFFECT");
				}

				if (NGIsItemFrozen(param)) {
					// TODO: action_data signifies a special effect when unfreezeing
					NGSetItemFreezeTimer(item_id, 0);
				}
			}
			break;
		}
		case HURT_ENEMY: {
			if (!NGCheckFloorStatePressedThisFrameOrLastFrame()) {
				NGHurtEnemy(param, action_data & 0x7f);
			}
			break;
		}
		case FORCE_ANIMATION_0_TO_31_ON_ITEM: {
			if (!NGCheckFloorStatePressedThisFrameOrLastFrame()) {
				NGForceItemAnimation(param, action_data & 0x1f);
			}
			break;
		}
		case FORCE_ANIMATION_32_TO_63_ON_ITEM: {
			if (!NGCheckFloorStatePressedThisFrameOrLastFrame()) {
				NGForceItemAnimation(param, (action_data & 0x1f) + 32);
			}
			break;
		}
		case FORCE_ANIMATION_64_TO_95_ON_ITEM: {
			if (!NGCheckFloorStatePressedThisFrameOrLastFrame()) {
				NGForceItemAnimation(param, (action_data & 0x1f) + 64);
			}
			break;
		}
		case MOVE_ITEM_UP_BY_UNITS_X8: {
			if (!NGCheckFloorStatePressedThisFrameOrLastFrame())
				NGMoveItemByUnits(param, NG_UP, 8 * ((action_data)+1));
			break;
		}
		case MOVE_ITEM_DOWN_BY_UNITS_X8: {
			if (!NGCheckFloorStatePressedThisFrameOrLastFrame())
				NGMoveItemByUnits(param, NG_DOWN, 8 * ((action_data)+1));
			break;
		}
		case MOVE_ITEM_WEST_BY_UNITS_X8: {
			if (!NGCheckFloorStatePressedThisFrameOrLastFrame())
				NGMoveItemByUnits(param, NG_WEST, 8 * ((action_data)+1));
			break;
		}
		case MOVE_ITEM_NORTH_BY_UNITS_X8: {
			if (!NGCheckFloorStatePressedThisFrameOrLastFrame())
				NGMoveItemByUnits(param, NG_NORTH, 8 * ((action_data)+1));
			break;
		}
		case MOVE_ITEM_EAST_BY_UNITS_X8: {
			if (!NGCheckFloorStatePressedThisFrameOrLastFrame())
				NGMoveItemByUnits(param, NG_EAST, 8 * ((action_data)+1));
			break;
		}
		case MOVE_ITEM_SOUTH_BY_UNITS_X8: {
			if (!NGCheckFloorStatePressedThisFrameOrLastFrame())
				NGMoveItemByUnits(param, NG_SOUTH, 8 * ((action_data)+1));
			break;
		}
		default:
			printf("Unimplemented NGTrigger %u\n", action_type);
			break;
		};
};