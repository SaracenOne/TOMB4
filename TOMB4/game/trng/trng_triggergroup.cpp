#include "../../tomb4/pch.h"

#include "trng.h"
#include "trng_condition.h"
#include "trng_flipeffect.h"
#include "trng_extra_state.h"
#include "trng_script_parser.h"

bool NGTriggerGroupFunction(unsigned int trigger_group_id, unsigned char execution_type) {
	// Multiple performing
	// Execution Type 0
	if (execution_type == 0) {

	}
	// Single performing
	// Execution Type 1
	else if (execution_type == 1) {

	}
	// Continous Performing
	// Execution Type 2
	else if (execution_type == 2) {
		NGSetTriggerGroupContinuous(trigger_group_id, true);
	}
	else {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "Unknown TriggerGroup execution type not implemented yet!");
		return false;
	}

	NG_TRIGGER_GROUP& trigger_group = current_trigger_groups[trigger_group_id];
	int index = 0;

	bool parsed_first_operation = false;
	bool operation_result = false;

	while (index < NG_TRIGGER_GROUP_DATA_SIZE) {
		// Check of unsupported TGROUP flags
		if (trigger_group.data[index].first_field & TGROUP_SINGLE_SHOT_RESUMED ||
			trigger_group.data[index].first_field & TGROUP_USE_EXECUTOR_ITEM_INDEX ||
			trigger_group.data[index].first_field & TGROUP_USE_ITEM_USED_BY_LARA_INDEX ||
			trigger_group.data[index].first_field & TGROUP_USE_OWNER_ANIM_ITEM_INDEX ||
			trigger_group.data[index].first_field & TGROUP_USE_TRIGGER_ITEM_INDEX) {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "Unsupported TGROUP flags detected!");
			return false;
		}

		if (trigger_group.data[index].first_field & TGROUP_SINGLE_SHOT) {
			if (trigger_group.oneshot_triggered) {
				operation_result = false;
			}
			else {
				trigger_group.oneshot_triggered = true;
			}
		}

		if (trigger_group.data[index].first_field & TGROUP_ELSE) {
			// ELSE
			if (operation_result == true) {
				break;
			} else {
				parsed_first_operation = false;
				operation_result = false;
			}
		}

		if (trigger_group.data[index].first_field == 0x0000)
			break;

		if ((!(trigger_group.data[index].first_field & TGROUP_OR) && (operation_result == true || !parsed_first_operation)) ||
			trigger_group.data[index].first_field & TGROUP_OR) {
			bool current_result = false;

			// ActionNG (statics)
			if ((trigger_group.data[index].first_field & 0xF000) == 0x4000) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ActionNG (statics) unsupported!");
			}
			// ActionNG
			else if ((trigger_group.data[index].first_field & 0xF000) == 0x5000) {
				if (trigger_group.data[index].first_field & TGROUP_USE_FOUND_ITEM_INDEX) {
					current_result = NGAction(ng_found_item_index, trigger_group.data[index].third_field & 0x7fff, true) != -1;
				} else {
					current_result = NGAction(ng_script_id_table[trigger_group.data[index].second_field], trigger_group.data[index].third_field & 0x7fff, true) != -1;
				}
			}
			// ConditionNG
			else if ((trigger_group.data[index].first_field & 0xF000) == 0x8000) {
				if (trigger_group.data[index].first_field & TGROUP_USE_FOUND_ITEM_INDEX) {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TGROUP_USE_FOUND_ITEM_INDEX used on condition");
				} else {
					current_result = NGCondition(trigger_group.data[index].second_field, (trigger_group.data[index].third_field >> 8) & 0xff, trigger_group.data[index].third_field & 0xff);
				}
			}
			// ConditionNG (item id)
			else if ((trigger_group.data[index].first_field & 0xF000) == 0x9000) {
				if (trigger_group.data[index].first_field & TGROUP_USE_FOUND_ITEM_INDEX) {
					current_result = NGCondition(ng_found_item_index, (trigger_group.data[index].third_field >> 8) & 0xff, trigger_group.data[index].third_field & 0xff);
				} else {
					current_result = NGCondition(ng_script_id_table[trigger_group.data[index].second_field], (trigger_group.data[index].third_field >> 8) & 0xff, trigger_group.data[index].third_field & 0xff);

				}
			}
			// Flipeffect
			else if ((trigger_group.data[index].first_field & 0xF000) == 0x2000) {
				if (trigger_group.data[index].first_field & TGROUP_USE_FOUND_ITEM_INDEX) {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TGROUP_USE_FOUND_ITEM_INDEX used on flipeffect");
				} else {
					current_result = NGFlipEffect(trigger_group.data[index].second_field, trigger_group.data[index].third_field & 0x7fff, false, true);
				}
			}
			// End
			else if (trigger_group.data[index].first_field == 0x0000) {
				break;
			}
			else {
				NGLog(NG_LOG_TYPE_ERROR, "Unknown triggergroup command!");
				operation_result = false;
				break;
			}

			if (trigger_group.data[index].first_field & TGROUP_NOT)
				current_result = !current_result;

			if (trigger_group.data[index].first_field & TGROUP_OR) {
				if (current_result == true)
					operation_result = true;
			}
			else {
				operation_result = current_result;
			}

			// Does single performing bypass all checks?
			if (execution_type == 1) {
				operation_result = true;
			}

			parsed_first_operation = true;
		}
		index++;
	}

	return operation_result;
}
