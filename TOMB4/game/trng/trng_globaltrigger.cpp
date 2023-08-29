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

bool NGExecuteSingleGlobalTrigger(int global_trigger_id, int inventory_object_id) {
	NG_GLOBAL_TRIGGER* global_trigger = &ng_levels[gfCurrentLevel].records->global_triggers_table[global_trigger_id].global_trigger;
	int record_id = ng_levels[gfCurrentLevel].records->global_triggers_table[global_trigger_id].record_id;

	bool global_trigger_condition_passed = false;

	unsigned short condition_trigger_group_id = global_trigger->condition_trigger_group;

	if (inventory_object_id == NO_ITEM) {
		// What the difference between GT_CONDITION_GROUP and GT_ALWAYS?
		switch (global_trigger->type) {
		case 0001: { // GT_USED_INVENTORY_ITEM
#ifndef SILENCE_EXCESSIVE_LOGS
			NGLOG(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "GT_USED_INVENTORY_ITEM globaltrigger type not yet supported!");
#endif
			global_trigger_condition_passed = false;
			break;
		}
		case 0x0003: { // GT_ENEMY_KILLED
			int enemy_id = ng_script_id_table[global_trigger->parameter];
			if (items[enemy_id].after_death > 0)
				global_trigger_condition_passed = true;
			break;
		}
		case 0x0004: { // GT_LARA_HP_LESS_THAN
			if (lara_item->hit_points < global_trigger->parameter)
				global_trigger_condition_passed = true;
			break;
		}
		case 0x0005: { // GT_LARA_HP_HIGHER_THAN
			if (lara_item->hit_points > global_trigger->parameter)
				global_trigger_condition_passed = true;
			break;
		}
		case 0x000a: { // GT_LARA_IS_POISONED
			if (lara_item->poisoned > global_trigger->parameter)
				global_trigger_condition_passed = true;
			break;
		}
		case 0x000b: { // GT_CONDITION_GROUP
			global_trigger_condition_passed = true;
			break;
		}
		case 0x000d: { // GT_COLLIDE_ITEM
			int result = NGIsLaraCollidingWithMoveableID(ng_script_id_table[global_trigger->parameter]);
			if (result >= 0) {
				global_trigger_condition_passed = true;
				ng_found_item_index = result;
			}
			break;
		}
		case 0x000e: { // GT_COLLIDE_SLOT
			int result = NGIsLaraCollidingWithMoveableSlot(global_trigger->parameter);
			if (result >= 0) {
				global_trigger_condition_passed = true;
				ng_found_item_index = result;
			}
			break;
		}
		case 0x000f: { // GT_COLLIDE_CREATURE
			int result = NGIsLaraCollidingWithCreature();
			if (result >= 0) {
				global_trigger_condition_passed = true;
				ng_found_item_index = result;
			}
			break;
		}
		case 0x0012: { // GT_COLLIDE_STATIC_SLOT
			int result = NGIsLaraCollidingWithStaticSlot(global_trigger->parameter);
			if (result >= 0) {
				global_trigger_condition_passed = true;
			}
			break;
		}
		case 0x0020: // GT_ALWAYS
			global_trigger_condition_passed = true;
			break;
		case 0x0027: // GT_SELECTED_INVENTORY_ITEM
			break;
		default:
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "Unimplemented GlobalTrigger type %u!", global_trigger->type);
			return false;
		}
	} else {
		switch (global_trigger->type) {
		case 0x0027: // GT_SELECTED_INVENTORY_ITEM
			if (inventory_object_id == global_trigger->parameter)
				global_trigger_condition_passed = true;
			break;
		}
	}

	bool management_replaced = false;

	// FGT_NOT_TRUE
	if (global_trigger->flags & 0x0002)
		global_trigger_condition_passed = !global_trigger_condition_passed;

	// FGT_PUSHING_COLLISION
	if (global_trigger->flags & 0x0004) {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "Unimplemented FGT_PUSHING_COLLISION!");
		return false;
	}

	// FGT_REMOVE_INPUT
	if (global_trigger->flags & 0x0010) {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "Unimplemented FGT_REMOVE_INPUT!");
		return false;
	}

	if (ng_global_trigger_states[record_id].is_disabled) {
		return false;
	}

	// FGT_REPLACE_MANAGEMENT
	if (global_trigger->flags & 0x0040) {
		if (global_trigger_condition_passed)
			management_replaced = true;
	}

	bool global_trigger_should_on_false_triggergroup = false;

	if (global_trigger_condition_passed) {
		if ((condition_trigger_group_id == 0xffff || NGTriggerGroupFunction(condition_trigger_group_id, 0))) {
			if (!ng_global_trigger_states[record_id].is_halted) {
				unsigned int perform_trigger_group_id = global_trigger->perform_trigger_group;

				if (perform_trigger_group_id != 0xffff) {
					NGTriggerGroupFunction(perform_trigger_group_id, 0);
				} else {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "Unimplemented 'default' perform parameter in GlobalTrigger");
				}

				// FGT_SINGLE_SHOT / FGT_SINGLE_SHOT_RESUMED
				if (global_trigger->flags & 0x0001 || global_trigger->flags & 0x0020)
					ng_global_trigger_states[record_id].is_halted = true;
			} else {
				// I assume a halted GlobalTrigger should run the on false triggergroup, but may need to check this.
				global_trigger_should_on_false_triggergroup = true;
			}
		} else {
			// FGT_SINGLE_SHOT_RESUMED
			if (global_trigger->flags & 0x0020)
				ng_global_trigger_states[record_id].is_halted = false;

			global_trigger_should_on_false_triggergroup = true;
		}
	} else {
		global_trigger_should_on_false_triggergroup = true;
	}

	if (global_trigger_should_on_false_triggergroup) {
		unsigned int on_false_trigger_group_id = global_trigger->on_false_trigger_group;
		if (on_false_trigger_group_id != 0xffff) {
			NGTriggerGroupFunction(on_false_trigger_group_id, 0);
		}
	}

	return management_replaced;
}