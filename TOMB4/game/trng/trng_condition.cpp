#include "../../tomb4/pch.h"

#include "../../specific/3dmath.h"
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
#include "../lara_states.h"
#include "../newinv.h"
#include "../savegame.h"

#include "trng.h"
#include "trng_arithmetic.h"
#include "trng_condition.h"
#include "trng_env_condition.h"
#include "trng_extra_state.h"
#include "trng_triggergroup.h"

#include "../../tomb4/tomb4plus/t4plus_inventory.h"
#include "../../tomb4/tomb4plus/t4plus_objects.h"

#include "../../specific/input.h"
#include "../../specific/dxsound.h"

#define SECTOR_SIZE 1024

enum GRID_FRAGMENT_TYPE {
	FRAGMENT_TYPE_SQUARE_FRAGMENT,
	FRAGMENT_TYPE_HORIZONTAL_STRIP_FRAGMENT,
	FRAGMENT_TYPE_VERTICAL_STRIP_FRAGMENT,
	FRAGMENT_TYPE_TWO_CROSSED_STRIPES,
	FRAGMENT_TYPE_DIAGONAL_UPPER_RIGHT_LOWER_LEFT_PASSING_LINE, // DIAGONAL_BOTTOM_UP_PASSING_POINT
	FRAGMENT_TYPE_DIAGONAL_UPPER_LEFT_LOWER_RIGHT_PASSING_LINE, // DIAGONAL_TOP_DOWN_PASSING_POINT
	FRAGMENT_TYPE_TWO_CROSS_DIAGONALS_PASSING_LINE
};

bool NGIsCreatureActive(short item_num) {
	ITEM_INFO* item = &items[item_num];

	if (item->flags & IFL_CLEARBODY) {
		return false;
	}

	if (item->status == ITEM_INVISIBLE) {
		return false;
	}

	return true;
}

bool NGIsTriggerActive(ITEM_INFO* item) {
	if (!(item->flags & IFL_CODEBITS)) {
		return false;
	}

	return true;
}

bool NGGridFragmentCondition(int x_pos, int y_pos, int grid_size, int x_target_coordinate, int y_target_coordinate, GRID_FRAGMENT_TYPE grid_fragment_type, bool inverted) {
	int fragment_size = SECTOR_SIZE / grid_size;
	// Flipped these around to match
	int touching_fragment_x = x_pos / fragment_size;
	int touching_fragment_y = y_pos / fragment_size;

	if (grid_fragment_type == FRAGMENT_TYPE_SQUARE_FRAGMENT) {
		return (touching_fragment_x == x_target_coordinate && touching_fragment_y == y_target_coordinate) != inverted;
	} else if (grid_fragment_type == FRAGMENT_TYPE_VERTICAL_STRIP_FRAGMENT) {
		return (touching_fragment_x == x_target_coordinate) != inverted;
	} else if (grid_fragment_type == FRAGMENT_TYPE_HORIZONTAL_STRIP_FRAGMENT) {
		return (touching_fragment_y == y_target_coordinate) != inverted;
	} else if (grid_fragment_type == FRAGMENT_TYPE_TWO_CROSSED_STRIPES) {
		return (touching_fragment_x == x_target_coordinate || touching_fragment_y == y_target_coordinate) != inverted;
	} else if (grid_fragment_type == FRAGMENT_TYPE_DIAGONAL_UPPER_RIGHT_LOWER_LEFT_PASSING_LINE) {
		return (touching_fragment_x + touching_fragment_y == x_target_coordinate + y_target_coordinate) != inverted;
	} else if (grid_fragment_type == FRAGMENT_TYPE_DIAGONAL_UPPER_LEFT_LOWER_RIGHT_PASSING_LINE) {
		return (touching_fragment_x - touching_fragment_y == x_target_coordinate - y_target_coordinate) != inverted;
	} else if (grid_fragment_type == FRAGMENT_TYPE_TWO_CROSS_DIAGONALS_PASSING_LINE) {
		return ((touching_fragment_x + touching_fragment_y == x_target_coordinate + y_target_coordinate) || (touching_fragment_x - touching_fragment_y == x_target_coordinate - y_target_coordinate)) != inverted;
	} else {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGGridFragmentCondition: grid fragment type unsupported %u!", (unsigned int)grid_fragment_type);
	}
	
	return false;
}

bool NGGridFragmentConditionTrigger(int param, unsigned char extra, int grid_size) {
	int lara_sector_displacement_y = (lara_item->pos.x_pos & 0x3ff);
	int lara_sector_displacement_x = (lara_item->pos.z_pos & 0x3ff);

	bool inverted = extra & 0x02;
	bool pad_trigger = extra & 0x01;

	if (pad_trigger) {
		if (lara_item->pos.y_pos != lara_item->floor)
			return false;
	}

	if (param >= 0 && param < 16) {
		int index = param;
		return NGGridFragmentCondition(lara_sector_displacement_x, lara_sector_displacement_y, grid_size, index / 4, index % 4, FRAGMENT_TYPE_SQUARE_FRAGMENT, inverted);
	} else if (param >= 32 && param < 36) {
		int index = param - 32;
		return NGGridFragmentCondition(lara_sector_displacement_x, lara_sector_displacement_y, grid_size, 0, index, FRAGMENT_TYPE_HORIZONTAL_STRIP_FRAGMENT, inverted);
	} else if (param >= 48 && param < 52) {
		int index = param - 48;
		return NGGridFragmentCondition(lara_sector_displacement_x, lara_sector_displacement_y, grid_size, index, 0, FRAGMENT_TYPE_VERTICAL_STRIP_FRAGMENT, inverted);
	} else if (param >= 96 && param < 112) {
		int index = param - 96;
		return NGGridFragmentCondition(lara_sector_displacement_x, lara_sector_displacement_y, grid_size, index / 4, index % 4, FRAGMENT_TYPE_TWO_CROSSED_STRIPES, inverted);
	} else if (param >= 128 && param < 144) {
		int index = param - 128;
		return NGGridFragmentCondition(lara_sector_displacement_x, lara_sector_displacement_y, grid_size, index / 4, index % 4, FRAGMENT_TYPE_DIAGONAL_UPPER_RIGHT_LOWER_LEFT_PASSING_LINE, inverted);
	} else if (param >= 144 && param < 160) {
		int index = param - 144;
		return NGGridFragmentCondition(lara_sector_displacement_x, lara_sector_displacement_y, grid_size, index / 4, index % 4, FRAGMENT_TYPE_DIAGONAL_UPPER_LEFT_LOWER_RIGHT_PASSING_LINE, inverted);
	} else if (param >= 192 && param < 208) {
		int index = param - 192;
		return NGGridFragmentCondition(lara_sector_displacement_x, lara_sector_displacement_y, grid_size, index / 4, index % 4, FRAGMENT_TYPE_TWO_CROSS_DIAGONALS_PASSING_LINE, inverted);
	} else {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGGridFragmentTrigger: param %u unsupported!", param);
	}

	return false;
}

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
	case LARA_IS_PERFORMING_X_ACTION: {
		bool is_lara_performing = !extra;
		switch (param) {
			case 0: // Climbing
				if ((
					lara_item->current_anim_state >= AS_CLIMBSTNC &&
					lara_item->current_anim_state <= AS_CLIMBDOWN) == is_lara_performing) {
					return true;
				}
				return false;
				break;
			case 1: // Swimming underwater
				if ((
					lara_item->current_anim_state == AS_TREAD ||
					lara_item->current_anim_state == AS_SWIM ||
					lara_item->current_anim_state == AS_GLIDE) == is_lara_performing) {
					return true;
				}
				return false;
				break;
			case 2: // Floating on water
				if ((
					lara_item->current_anim_state == AS_SURFTREAD ||
					lara_item->current_anim_state == AS_SURFSWIM ||
					lara_item->current_anim_state == AS_SURFBACK ||
					lara_item->current_anim_state == AS_SURFLEFT ||
					lara_item->current_anim_state == AS_SURFRIGHT) == is_lara_performing) {
					return true;
				}
				return false;
				break;
			case 3: // Falling
				if ((lara_item->current_anim_state == AS_FASTFALL ||
					lara_item->current_anim_state == AS_FALLBACK) == is_lara_performing) {
					return true;
				}
				return false;
				break;
			case 4: // Jumping
				if ((
					lara_item->current_anim_state == AS_BACKJUMP ||
					lara_item->current_anim_state == AS_UPJUMP ||
					lara_item->current_anim_state == AS_FORWARDJUMP) == is_lara_performing) {
					return true;
				}
				return false;
				break;
			case 5: // Moving on all fours
				if ((
					lara_item->current_anim_state == AS_ALL4S ||
					lara_item->current_anim_state == AS_CRAWL ||
					lara_item->current_anim_state == AS_ALL4TURNL ||
					lara_item->current_anim_state == AS_ALL4TURNR ||
					lara_item->current_anim_state == AS_CRAWLBACK ||
					lara_item->current_anim_state == AS_DUCK ||
					lara_item->current_anim_state == AS_DUCKROLL ||
					lara_item->current_anim_state == AS_DUCKROTL ||
					lara_item->current_anim_state == AS_DUCKROTR) == is_lara_performing) {
					return true;
				}
				return false;
				break;
			case 6: // Sliding
				if ((lara_item->current_anim_state == AS_SLIDE ||
					lara_item->current_anim_state == AS_SLIDEBACK) == is_lara_performing) {
					return true;
				}
				return false;
				break;
			case 7: // Rolling
				if ((lara_item->current_anim_state == AS_FASTBACK) == is_lara_performing) {
					return true;
				}
				break;
			case 8: // Running
				if ((lara_item->current_anim_state == AS_RUN) == is_lara_performing) {
					return true;
				}
				return false;
				break;
			case 9: // Walking
				if ((lara_item->current_anim_state == AS_WALK) == is_lara_performing) {
					return true;
				}
				return false;
				break;
			case 10: // Dying
				if ((lara_item->current_anim_state == AS_DEATH) == is_lara_performing) {
					return true;
				}
				return false;
				break;
			case 11: // Stopping
				if ((lara_item->current_anim_state == AS_STOP) == is_lara_performing) {
					return true;
				}
				return false;
				break;
			case 12: // Monkeying
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_IS_PERFORMING_X_ACTION monkeying not currently implemented!");
				return !is_lara_performing;
				break;
			case 13: // Pushing block
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_IS_PERFORMING_X_ACTION pushing block not currently implemented!");
				return !is_lara_performing;
				break;
			case 14: // Pulling block
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_IS_PERFORMING_X_ACTION pulling block not currently implemented!");
				return !is_lara_performing;
				break;
			case 15: // Pulling or pushing block
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_IS_PERFORMING_X_ACTION pulling or pushing block not currently implemented!");
				return !is_lara_performing;
				break;
			case 16: // Swimming underwater or on water
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_IS_PERFORMING_X_ACTION swimming underwater or on water not currently implemented!");
				return !is_lara_performing;
				break;
			case 17: // Shooting
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_IS_PERFORMING_X_ACTION shooting not currently implemented!");
				return !is_lara_performing;
				break;
			case 18: // Hanging
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_IS_PERFORMING_X_ACTION hanging not currently implemented!");
				return !is_lara_performing;
				break;
			case 19: // Dripping
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_IS_PERFORMING_X_ACTION dripping not currently implemented!");
				return !is_lara_performing;
				break;
		}

		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_IS_PERFORMING_X_ACTION is not currently implemented!");
		return false;
	}
	case FRAGMENTED_TRIGGGER_CHECK_IN_WAY_IF_LARA_IS_IN_FRAGMENT_OF_2X2_SECTOR_GRID: {
		return NGGridFragmentConditionTrigger(param, extra, 2);
	}
	case FRAGMENTED_TRIGGGER_CHECK_IN_WAY_IF_LARA_IS_IN_FRAGMENT_OF_3X3_SECTOR_GRID: {
		return NGGridFragmentConditionTrigger(param, extra, 3);
	}
	case FRAGMENTED_TRIGGGER_CHECK_IN_WAY_IF_LARA_IS_IN_FRAGMENT_OF_4X4_SECTOR_GRID: {
		return NGGridFragmentConditionTrigger(param, extra, 4);
	}
	case VERTICAL_TRIGGER_ZONE: {
		// TODO: make more accurate to TRNG
		short *bounds = GetBoundsAccurate(lara_item);
		int item_top_y = lara_item->pos.y_pos + bounds[2];
		int item_bottom_y = lara_item->pos.y_pos; // + bounds[3];

		int bottom_trigger_bounds = lara_item->floor - (param * 128);
		int top_trigger_bounds = bottom_trigger_bounds - ((extra + 1) * 128);

		return (item_top_y <= bottom_trigger_bounds && item_bottom_y >= top_trigger_bounds);
	}
	case VERTICAL_TRIGGER_ZONE_INVERSE: {
		// TODO: make more accurate to TRNG
		short* bounds = GetBoundsAccurate(lara_item);
		int item_top_y = lara_item->pos.y_pos + bounds[2];
		int item_bottom_y = lara_item->pos.y_pos; // + bounds[3];

		int bottom_trigger_bounds = lara_item->floor - (param * 128);
		int top_trigger_bounds = bottom_trigger_bounds - ((extra + 1) * 128);

		return (item_top_y < top_trigger_bounds || item_bottom_y > bottom_trigger_bounds);
	}
	case VERTICAL_TRIGGER_ANTI_ZONE: {
		// TODO: make more accurate to TRNG
		short* bounds = GetBoundsAccurate(lara_item);
		int item_top_y = lara_item->pos.y_pos + bounds[2];
		int item_bottom_y = lara_item->pos.y_pos; // + bounds[3];

		int bottom_trigger_bounds = lara_item->floor - (param * 128);
		int top_trigger_bounds = bottom_trigger_bounds - ((extra + 1) * 128);

		return !(item_top_y < bottom_trigger_bounds && item_bottom_y > top_trigger_bounds);
	}
	case KEYBOARD_SCANCODE_IS_CURRENTLY: {
		int scancode = param;
#ifdef USE_SDL
		scancode = convert_tomb_keycode_to_sdl_scancode(scancode);
		if (!keymap) {
			return false;
		}
		if (scancode == SDLK_UNKNOWN) {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "Unknown keyboard scancode %u!", scancode);
			return false;
		}
#endif
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
	case CREATURE_IS_CURRENTLY: {
		switch (extra) {
			// Enemy is dead
			case 0x00: {
				return (items[param].status == ITEM_DEACTIVATED);
			}
			// Enemy has not yet been activated
			case 0x01: {
				return (items[param].status == ITEM_INVISIBLE);
			}
			// Enemy is living
			case 0x02: {
				return NGIsCreatureActive(param);
			}
			// Enemy is active
			case 0x03: {
				return (NGIsCreatureActive(param) && NGIsTriggerActive(&items[param]));
			}
			// Enemy is not active
			case 0x04: {
				return (!NGIsCreatureActive(param) || !NGIsTriggerActive(&items[param]));
			}
			default: {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CREATURE_IS_CURRENTLY %u is not currently implemented!", extra);
				break;
			}
		}

		return false;
	}
	case MULTIPLE_CONDITION_OF_X_TRIGGERGROUP_SCRIPT_COMMAND: {
		return NGTriggerGroupFunction(param, TRIGGER_GROUP_EXECUTION_MULTIPLE);
	}
	case MULTIPLE_CONDITION_OF_X_MULTIENVCONDITION_SCRIPT_COMMAND: {
		return TestMultiEnvCondition(param, extra);
	}
	case LARA_HAS_FOUND_AT_LEAST_X_SECRETS: {
		return savegame.Game.Secrets >= param;
	}
	case LARA_HAS_FOUND_EXACTLY_X_SECRETS: {
		return savegame.Game.Secrets == param;
	}
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
		return items[param].anim_number - objects[items[param].object_number].anim_index == extra;
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
		return false;
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
		switch (param) {
			// Mortal creatures
			case 0x00: {
				int result = NGIsLaraCollidingWithCreature(NG_CREATURE_TYPE_MORTAL);
				if (result >= 0) {
					ng_found_item_index = result;
					return true;
				}
				break;
			}
			// Immortal creatures
			case 0x01: {
				int result = NGIsLaraCollidingWithCreature(NG_CREATURE_TYPE_IMMORTAL);
				if (result >= 0) {
					ng_found_item_index = result;
					return true;
				}
				break;
			}
			// Friends
			case 0x02: {
				int result = NGIsLaraCollidingWithCreature(NG_CREATURE_TYPE_FRIEND);
				if (result >= 0) {
					ng_found_item_index = result;
					return true;
				}
				break;
			}
			default: {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: LARA_IS_TOUCHING_CREATURE_TYPE %u unimplemented!", param);
				break;
			}
		}

		return false;
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
		if (lara_item->anim_number - objects[T4PlusGetLaraSlotID()].anim_index == param) {
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
					return items[lara.vehicle].object_number == T4PlusGetJeepSlotID();
				break;
			}
			case 11: {
				if (lara.vehicle != NO_ITEM)
					return items[lara.vehicle].object_number == T4PlusGetMotorbikeSlotID();
				break;
			}
			case 12: {
				if (lara.vehicle != NO_ITEM)
					return items[lara.vehicle].object_number == T4PlusGetRubberBoatSlotID();
				break;
			}
			case 13: {
				if (lara.vehicle != NO_ITEM)
					return items[lara.vehicle].object_number == T4PlusGetMotorBoatSlotID();
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
	case CREATURE_THE_X_CREATURE_HAS_THE_TRANSPARENCY_LEVEL: {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: CREATURE_THE_X_CREATURE_HAS_THE_TRANSPARENCY_LEVEL unimplemented type: 0x%02x!", extra);
		break;
	}
	case VARIABLES_THE_X_NUMERIC_VERIABLE_IS_EQUAL_OR_GREATER_TO_BIG_NUMBER_VALUE: {
		if (NGNumericGetVariable(param) >= current_big_numbers[extra].big_number)
			return true;
		else
			return false;
	}
	case VARIABLES_THE_X_NUMERIC_VERIABLE_IS_LESS_THAN_BIG_NUMBER_VALUE: {
		if (NGNumericGetVariable(param) < current_big_numbers[extra].big_number)
			return true;
		else
			return false;
	}
	case VARIABLES_THE_X_NUMERIC_VERIABLE_IS_EQUAL_TO_BIG_NUMBER_VALUE: {
		if (NGNumericGetVariable(param) == current_big_numbers[extra].big_number)
			return true;
		else
			return false;
	}
	case VARIABLES_THE_X_NUMERIC_VARIABLE_IS_EQUAL_OR_GREATER_TO: {
		if (NGNumericGetVariable(param) >= extra)
			return true;
		else
			return false;
	}
	case VARIABLES_THE_X_NUMERIC_VARIABLE_IS_LESS_THAN: {
		if (NGNumericGetVariable(param) < extra)
			return true;
		else
			return false;
	}
	case VARIABLES_THE_X_NUMERIC_VARIABLE_IS_EQUAL_TO: {
		if (NGNumericGetVariable(param) == extra)
			return true;
		else
			return false;
	}
	case VARIABLES_THE_X_NUMERIC_VARIABLE_HAS_THE_BIT_SET: {
		if (NGNumericGetVariable(param) & (1 << extra))
			return true;
		else
			return false;
	}
	case VARIABLES_THE_X_NUMERIC_VARIABLE_HAS_THE_BIT_CLEAR: {
		if (!(NGNumericGetVariable(param) & (1 << extra)))
			return true;
		else
			return false;
	}
	case VARIABLES_THE_CURRENT_VALUE_VARIABLE_IS_GREATER_OR_EQUAL_THAN_X_VALUE: {
		if (ng_current_value >= param)
			return true;
		else
			return false;
	}
	case VARIABLES_THE_CURRENT_VALUE_VARIABLE_IS_LESS_THAN_X_VALUE: {
		if (ng_current_value < param)
			return true;
		else
			return false;
	}
	case VARIABLES_THE_CURRENT_VALUE_VARIABLE_IS_EQUAL_THAN_X_VALUE: {
		if (ng_current_value == param)
			return true;
		else
			return false;
	}
	case LARA_IS_LESS_OR_EVEN_CLICKS_DISTANT_TO_MOVEABLE: {
		return NGIsSourcePositionLessThanDistanceToTargetPosition(&lara_item->pos, &items[param].pos, extra * 256, false);
		break;
	}
	case LARA_IS_LESS_OR_EVEN_UNITS_DISTANT_TO_MOVEABLE: {
		return NGIsSourcePositionLessThanDistanceToTargetPosition(&lara_item->pos, &items[param].pos, extra, false);
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
					return room_flags & ROOM_SWAMP;
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
		}
		break;
	}
	case LARA_IS_TOUCHING_MOVEABLE_WITH_MESH_NUMBER: {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: LARA_IS_TOUCHING_MOVEABLE_WITH_MESH_NUMBER unimplemented!");
		return false;
		break;
	}
	case SOUND_THE_X_SOUND_EFFECT_IS_PLAYING: {
		return DSIsSamplePlaying(param);
		break;
	}
	default:
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGCondition: %u unimplemented!", timer);
		break;
	};

	return false;
};