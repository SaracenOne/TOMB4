#include "../../tomb4/pch.h"

#include "../control.h"
#include "../lara.h"
#include "../gameflow.h"

#include "trng.h"
#include "trng_condition.h"
#include "trng_flipeffect.h"
#include "trng_extra_state.h"
#include "trng_script_parser.h"
#include "trng_triggergroup.h"
#include "trng_globaltrigger.h"
#include "../../specific/input.h"
#include "../../specific/dxshell.h"
#include "../newinv.h"

bool NGExecuteSingleGlobalTrigger(int global_trigger_id, int selected_inventory_object_id) {
	NG_GLOBAL_TRIGGER* global_trigger = &ng_levels[gfCurrentLevel].records->global_trigger_table[global_trigger_id].record;
	int record_id = ng_levels[gfCurrentLevel].records->global_trigger_table[global_trigger_id].record_id;

	bool global_trigger_condition_passed = false;

	unsigned short condition_trigger_group_id = global_trigger->condition_trigger_group;

	if (global_trigger_id == 7) {
		printf("");
	}

	if (selected_inventory_object_id == NO_ITEM) {
		// What the difference between GT_CONDITION_GROUP and GT_ALWAYS?
		switch (global_trigger->type) {
		case GT_USED_INVENTORY_ITEM: {
			if (ng_used_inventory_object_for_frame != NO_ITEM) {
				if (ng_used_inventory_object_for_frame == global_trigger->parameter)
					global_trigger_condition_passed = true;
			}
			break;
		}
		case GT_USED_BIG_MEDIPACK: {
			if (ng_used_large_medipack) {
				global_trigger_condition_passed = true;
			}
			break;
		}
		case GT_USED_LITTLE_MEDIPACK: {
			if (ng_used_small_medipack) {
				global_trigger_condition_passed = true;
			}
			break;
		}
		case GT_ENEMY_KILLED: {
			int enemy_id = ng_script_id_table[global_trigger->parameter].script_index;
			if (items[enemy_id].after_death > 0)
				global_trigger_condition_passed = true;
			break;
		}
		case GT_LARA_HP_LESS_THAN: {
			if (lara_item->hit_points < global_trigger->parameter)
				global_trigger_condition_passed = true;
			break;
		}
		case GT_LARA_HP_HIGHER_THAN: {
			if (lara_item->hit_points > global_trigger->parameter)
				global_trigger_condition_passed = true;
			break;
		}
		case GT_LARA_POISONED: {
			if (lara_item->poisoned > (ulong)global_trigger->parameter)
				global_trigger_condition_passed = true;
			break;
		}
		case GT_CONDITION_GROUP: {
			global_trigger_condition_passed = true;
			break;
		}
		case GT_COLLIDE_ITEM: {
			int result = NGIsLaraCollidingWithMoveableID(ng_script_id_table[global_trigger->parameter].script_index);
			if (result >= 0) {
				global_trigger_condition_passed = true;
				ng_found_item_index = result;
			}
			break;
		}
		case GT_COLLIDE_SLOT: {
			int result = NGIsLaraCollidingWithMoveableSlot(global_trigger->parameter);
			if (result >= 0) {
				global_trigger_condition_passed = true;
				ng_found_item_index = result;
			}
			break;
		}
		case GT_COLLIDE_CREATURE: {
			int result = NGIsLaraCollidingWithCreature(NG_CREATURE_TYPE_ANY);
			if (result >= 0) {
				global_trigger_condition_passed = true;
				ng_found_item_index = result;
			}
			break;
		}
		case GT_COLLIDE_STATIC_SLOT: {
			int result = NGIsLaraCollidingWithStaticSlot(global_trigger->parameter);
			if (result >= 0) {
				global_trigger_condition_passed = true;
			}
			break;
		}
		case GT_KEYBOARD_CODE: {
			if (IsKeyPressed(global_trigger->parameter) != 0) {
				global_trigger_condition_passed = true;
			}
			break;
		}
		case GT_ALWAYS:
			global_trigger_condition_passed = true;
			break;
		case GT_TRNG_G_TIMER_EQUALS:
			// We don't evaluate AT ALL if the timer is inactive
			if (ng_global_timer_frame_increment == 0)
				return false;

			if (ng_global_timer == global_trigger->parameter) {
				global_trigger_condition_passed = true;
			}
			break;
		case GT_TRNG_L_TIMER_EQUALS:
			// We don't evaluate AT ALL if the timer is inactive
			if (ng_local_timer_frame_increment == 0)
				return false;

			if (ng_local_timer == global_trigger->parameter) {
				global_trigger_condition_passed = true;
			}
			break;
		case GT_SELECTED_INVENTORY_ITEM:
			return false;
		default:
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "Unimplemented GlobalTrigger type %u!", global_trigger->type);
			return false;
		}
	} else {
		switch (global_trigger->type) {
		case GT_SELECTED_INVENTORY_ITEM:
			if (selected_inventory_object_id == global_trigger->parameter)
				global_trigger_condition_passed = true;
			break;
		}
	}

	bool management_replaced = false;

	if (global_trigger->flags & FGT_NOT_TRUE)
		global_trigger_condition_passed = !global_trigger_condition_passed;

	if (global_trigger->flags & FGT_PUSHING_COLLISION) {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "Unimplemented FGT_PUSHING_COLLISION!");
		return false;
	}

	if (global_trigger->flags & FGT_REMOVE_INPUT) {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "Unimplemented FGT_REMOVE_INPUT!");
		return false;
	}

	if (ng_global_trigger_states[record_id].is_disabled) {
		return false;
	}

	if (global_trigger->flags & FGT_REPLACE_MANAGEMENT) {
		if (global_trigger_condition_passed)
			management_replaced = true;
	}

	bool global_trigger_should_on_false_triggergroup = false;

	if (global_trigger_condition_passed) {
		if ((condition_trigger_group_id == 0xffff || NGTriggerGroupFunction(condition_trigger_group_id, TRIGGER_GROUP_EXECUTION_MULTIPLE))) {
			if (!ng_global_trigger_states[record_id].is_halted) {
				unsigned int perform_trigger_group_id = global_trigger->perform_trigger_group;

				if (perform_trigger_group_id != 0xffff) {
					NGTriggerGroupFunction(perform_trigger_group_id, TRIGGER_GROUP_EXECUTION_MULTIPLE);
				} else {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "Unimplemented 'default' perform parameter in GlobalTrigger");
				}

				if (global_trigger->flags & FGT_SINGLE_SHOT || global_trigger->flags & FGT_SINGLE_SHOT_RESUMED)
					ng_global_trigger_states[record_id].is_halted = true;
			} else {
				// I assume a halted GlobalTrigger should run the on false triggergroup, but may need to check this.
				global_trigger_should_on_false_triggergroup = true;
			}
		} else {
			if (global_trigger->flags & FGT_SINGLE_SHOT_RESUMED)
				ng_global_trigger_states[record_id].is_halted = false;

			global_trigger_should_on_false_triggergroup = true;
		}
	} else {
		global_trigger_should_on_false_triggergroup = true;
	}

	if (global_trigger_should_on_false_triggergroup) {
		unsigned int on_false_trigger_group_id = global_trigger->on_false_trigger_group;
		if (on_false_trigger_group_id != 0xffff) {
			NGTriggerGroupFunction(on_false_trigger_group_id, TRIGGER_GROUP_EXECUTION_MULTIPLE);
		}
	}

	return management_replaced;
}