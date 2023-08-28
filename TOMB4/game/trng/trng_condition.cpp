#include "../../tomb4/pch.h"

#include "../../specific/function_stubs.h"
#include "../../specific/dxshell.h"
#include "../../specific/file.h"
#include "../control.h"
#include "../effects.h"
#include "../objects.h"
#include "../draw.h"
#include "../door.h"
#include "../items.h"
#include "../lot.h"
#include "../lara.h"
#include "../newinv.h"
#include "trng.h"
#include "trng_arithmetic.h"
#include "trng_condition.h"
#include "trng_extra_state.h"
#include "../savegame.h"
#include "../lara_states.h"
#include "trng_triggergroup.h"
#include "../../tomb4/tomb4plus/inventory.h"

bool NGCondition(short param, unsigned char extra, short timer) {
	switch (timer) {
	case INVENTORY_ITEM_IS_MISSING: {
		return T4PlusGetInventoryCount(param) == 0;
		break;
	}
	case INVENTORY_ITEM_IS_PRESENT: {
		return T4PlusGetInventoryCount(param) > 0;
	}
	case INVENTORY_ITEM_HAS_AT_LEAST: {
		return T4PlusGetInventoryCount(param) >= extra;
		break;
	}
	case INVENTORY_ITEM_HAS_LESS_THAN: {
		return T4PlusGetInventoryCount(param) < extra;
		break;
	}
	case VERTICAL_TRIGGER_ZONE: {
		short *bounds = GetBoundsAccurate(lara_item);
		int item_top_y = lara_item->pos.y_pos + bounds[2];
		int item_bottom_y = lara_item->pos.y_pos; // + bounds[3];

		int bottom_trigger_bounds = lara_item->floor - (param * 128);
		int top_trigger_bounds = bottom_trigger_bounds - ((extra + 1) * 128);

		return (item_top_y <= bottom_trigger_bounds && item_bottom_y >= top_trigger_bounds);
	}
	case VERTICAL_TRIGGER_ZONE_INVERSE: {
		short* bounds = GetBoundsAccurate(lara_item);
		int item_top_y = lara_item->pos.y_pos + bounds[2];
		int item_bottom_y = lara_item->pos.y_pos; // + bounds[3];

		int bottom_trigger_bounds = lara_item->floor - (param * 128);
		int top_trigger_bounds = bottom_trigger_bounds - ((extra + 1) * 128);

		return (item_top_y < top_trigger_bounds || item_bottom_y > bottom_trigger_bounds);
	}
	case VERTICAL_TRIGGER_ANTI_ZONE: {
		short* bounds = GetBoundsAccurate(lara_item);
		int item_top_y = lara_item->pos.y_pos + bounds[2];
		int item_bottom_y = lara_item->pos.y_pos; // + bounds[3];

		int bottom_trigger_bounds = lara_item->floor - (param * 128);
		int top_trigger_bounds = bottom_trigger_bounds - ((extra + 1) * 128);

		return !(item_top_y < bottom_trigger_bounds && item_bottom_y > top_trigger_bounds);
	}
	case KEYBOARD_SCANCODE_IS_CURRENTLY: {
		int scancode = param;
		// TODO: find the difference between multishot and singleshot
		switch (extra) {
			// Inactive single shot
			case 0: {
				return keymap[scancode] == 0;
			}
			// Active single shot
			case 1: {
				return keymap[scancode] != 0;
			}
			// Inactive multi shot
			case 2: {
				return keymap[scancode] == 0;
			}
			// Inactive single shot
			case 3: {
				return keymap[scancode] != 0;
			}
		}

		return false;
	}
	case KEYBOARD_COMMAND_GAME_IS_CURRENTLY: {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "KEYBOARD_COMMAND_GAME_IS_CURRENTLY is not currently implemented!");
		return false;
	}
	case MULTIPLE_CONDITION_OF_X_TRIGGERGROUP_SCRIPT_COMMAND: {
		return NGTriggerGroupFunction(param, 0);
	}
	case MULTIPLE_CONDITION_OF_X_MULTIENVCONDITION_SCRIPT_COMMAND: {
#ifndef SILENCE_EXCESSIVE_LOGS
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "MULTIPLE_CONDITION_OF_X_MULTIENVCONDITION_SCRIPT_COMMAND is not currently implemented!");
#endif
		return false;
	}
	case CREATURE_IS_CURRENTLY: {
		switch (extra) {
			case 0x03: {
				return items[param].status == ITEM_ACTIVE;
			}
			default: {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CREATURE_IS_CURRENTLY is not currently implemented!");
				break;
			}
		}

		return false;
	}
	case LARA_HAS_FOUND_AT_LEAST_X_SECRETS:
		return savegame.Game.Secrets >= param;
	case LARA_HAS_FOUND_EXACTLY_X_SECRETS:
		return savegame.Game.Secrets == param;
	case KEYPAD_LAST_NUMBER_TYPED_IN_KEYPAD_IS_X_VALUE: {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "KEYPAD_LAST_NUMBER_TYPED_IN_KEYPAD_IS_X_VALUE is not currently implemented!");
		return false;
	}
	case TIMER_TIMER_SCREEN_VALUE_IS_Y_THAN_X_SECONDS: {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TIMER_TIMER_SCREEN_VALUE_IS_Y_THAN_X_SECONDS is not currently implemented!");
		return false;
	}
	case CREATURE_CURRENT_ANIMATION_0_31_IS:
	case CREATURE_CURRENT_ANIMATION_32_63_IS:
	case CREATURE_CURRENT_ANIMATION_64_95_IS:
		return items[param].anim_number - objects[items[param].object_number].anim_index == param;
	case CREATURE_IS_CURRENTLY_OF_STATE: {
		return items[param].current_anim_state == extra;
	}
	// Lara status is enabled/disabled
	case LARA_STATUS_IS_ENABLED_OR_DISABLED: {
		switch (param) {
			// Poisoned
			case 2:
				if ((lara_item->poisoned) == (bool)extra)
					return true;
				break;
			// Lara touching floor
			case 4:
				if ((lara_item->pos.y_pos == lara_item->floor) == (bool)extra)
					return true;
				break;
			default:
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: Unimplemented NGCondition Lara Status %u!", param);
				break;
		}
		break;
	}
	case LARA_IS_TOUCHING_MOVEABLE_ID: {
		int result = NGIsLaraCollidingWithMoveableID(param);
		if (result >= 0) {
			ng_found_item_index = result;
			return true;
		}
		break;
	}
	case LARA_IS_TOUCHING_MOVEABLE_SLOT: {
		int result = NGIsLaraCollidingWithMoveableSlot(param);
		if (result >= 0) {
			ng_found_item_index = result;
			return true;
		}
		break;
	}
	case LARA_IS_TOUCHING_CREATURE_TYPE: {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: LARA_IS_TOUCHING_CREATURE_TYPE unimplemented!");
		break;
	}
	case LARA_IS_VITALITY_IS_X_THAN: {
		switch (extra) {
			// Equal than
			case 0: {
				return lara_item->hit_points == param;
			}
			// Higher than
			case 1: {
				return lara_item->hit_points > param;
			}
			// Less than
			case 2: {
				return lara_item->hit_points < param;
			}
			default: {
				NGLog(NG_LOG_TYPE_ERROR, "NGCondition: LARA_IS_VITALITY_IS_X_THAN unknown param!");
			}
		}
		return false;
	}
	case LARA_IS_PERFORMING_ANIMATION: {
		if (lara_item->anim_number - objects[LARA].anim_index == param) {
			return true;
		} else {
			return false;
		}
		break;
	}
	case LARA_IS_STATE: {
		return lara_item->current_anim_state == param;
	}
	case ANIMTEXTURE_THE_X_ANIMRANGE_TEXTURE_IS_ENABLED_OR_DISABLED: {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ANIMTEXTURE_THE_X_ANIMRANGE_TEXTURE_IS_ENABLED_OR_DISABLED is not currently implemented!");
		return false;
	}
	case LARA_IS_TOUCHING_STATIC_SLOT: {
		int result = NGIsLaraCollidingWithStaticID(param);
		if (result >= 0) {
			return true;
		}
		break;
	}
	case LARA_IS_TOUCHING_STATIC_ID: {
		int result = NGIsLaraCollidingWithStaticID(param);
		if (result >= 0) {
			return true;
		}
		break;
	}
	case LARA_IS_HOLDING_OR_DRIVING_ITEMS: {
		switch (param) {
			// Holding pistols
			case 1: {
				return lara.gun_type == WEAPON_PISTOLS && lara.gun_status == LG_READY;
			}
			case 2: {
				return lara.gun_type == WEAPON_REVOLVER && lara.gun_status == LG_READY;
			}
			case 3: {
				return lara.gun_type == WEAPON_UZI && lara.gun_status == LG_READY;
			}
			case 4: {
				return lara.gun_type == WEAPON_SHOTGUN && lara.gun_status == LG_READY;
			}
			case 5: {
				return lara.gun_type == WEAPON_GRENADE && lara.gun_status == LG_READY;
			}
			case 6: {
				return lara.gun_type == WEAPON_CROSSBOW && lara.gun_status == LG_READY;
			}
			case 7: {
				return lara.gun_type == WEAPON_FLARE;
			}
			case 8: {
				return lara.gun_type == WEAPON_TORCH && !lara.LitTorch;
			}
			case 9: {
				return lara.gun_type == WEAPON_TORCH && lara.LitTorch;
			}
			case 10: {
				if (lara.vehicle != NO_ITEM)
					return items[lara.vehicle].object_number == JEEP;
				break;
			}
			case 11: {
				if (lara.vehicle != NO_ITEM)
					return items[lara.vehicle].object_number == MOTORBIKE;
				break;
			}
			case 12: {
				if (lara.vehicle != NO_ITEM)
					return items[lara.vehicle].object_number == RUBBER_BOAT;
				break;
			}
			case 13: {
				if (lara.vehicle != NO_ITEM)
					return items[lara.vehicle].object_number == MOTOR_BOAT;
				break;
			}
			case 14: {
				return lara.RopePtr != -1;
			}
			case 15: {
				return lara_item->current_anim_state == AS_POLESTAT ||
					lara_item->current_anim_state == AS_POLEUP ||
					lara_item->current_anim_state == AS_POLEDOWN ||
					lara_item->current_anim_state == AS_POLELEFT ||
					lara_item->current_anim_state == AS_POLERIGHT;
			}
			case 16: {
				return lara.gun_type == WEAPON_TORCH;
			}
			case 17: {
				if (lara.vehicle != NO_ITEM)
					return items[lara.vehicle].object_number == KAYAK;
				break;
			}
			default: {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: LARA_IS_HOLDING_OR_DRIVING_ITEMS unimplemented type: %u!", param);
			}
		}

		return false;
		break;
	}
	case NUMERIC_VALUE_IS_LESS_THAN: {
		if (extra < NGNumericGetVariable(param))
			return true;
		else
			return false;
	}
	case NUMERIC_VALUE_IS_EQUAL_OR_GREATER_TO: {
		if (extra >= NGNumericGetVariable(param))
			return true;
		else
			return false;
	}
	case NUMERIC_VALUE_IS_EQUAL_TO: {
		if (extra == NGNumericGetVariable(param))
			return true;
		else
			return false;
	}
	case LARA_IS_LESS_OR_EVEN_CLICKS_DISTANT_TO_MOVEABLE: {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: LARA_IS_LESS_OR_EVEN_CLICKS_DISTANT_TO_MOVEABLE unimplemented!");
		return false;
		break;
	}
	case LARA_IS_IN_ROOM_TYPE: {
		ROOM_INFO* r = &room[lara_item->room_number];
		if (r) {
			int room_flags = r->flags;
			switch (param) {
				case 0: {
					return room_flags & ROOM_UNDERWATER;
				}
				case 2: {
					return room_flags & ROOM_SKYBOX;
				}
				case 4: {
					return room_flags & 0x10;
				}
				case 5: {
					return room_flags & ROOM_NOT_INSIDE; // TODO: check this
				}
				case 10: {
					return room_flags & ROOM_SNOW;
				}
				case 11: {
					return room_flags & ROOM_RAIN;
				}
				case 12: {
					// TODO: check if this is actually cold water
					return room_flags & ROOM_COLD;
				}
				default: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: LARA_IS_IN_ROOM_TYPE unsupported room type!");
					break;
				}
			}
			return false;
			break;
		}
	}
	case LARA_IS_TOUCHING_MOVEABLE_WITH_MESH_NUMBER: {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: LARA_IS_TOUCHING_MOVEABLE_WITH_MESH_NUMBER unimplemented!");
		return false;
		break;
	}
	default:
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: %u unimplemented!", timer);
		break;
	};

	return false;
};