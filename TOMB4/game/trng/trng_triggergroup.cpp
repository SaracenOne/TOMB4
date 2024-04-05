#include "../../tomb4/pch.h"

#include "trng.h"
#include "trng_action.h"
#include "trng_condition.h"
#include "trng_flipeffect.h"
#include "trng_extra_state.h"
#include "trng_script_parser.h"
#include "trng_triggergroup.h"

#include "../../tomb4/mod_config.h"
#include "../../specific/file.h"
#include "../control.h"
#include "../camera.h"

// There is some ambiguity if the implementation of SINGLE_SHOT_RESUMED is implemented correctly for secondary data blocks
bool NGISTriggerGroupDataResumed(NG_TRIGGER_GROUP_DATA* data) {
	return ((data->first_field & TGROUP_SINGLE_SHOT_RESUMED) &&
		is_mod_trng_version_equal_or_greater_than_target(1, 2, 2, 7));
}

bool NGTriggerGroupFunction(unsigned int trigger_group_id, unsigned char execution_type) {
	if (execution_type == TRIGGER_GROUP_EXECUTION_CONTINUOUS) {
		NGSetTriggerGroupContinuous(trigger_group_id, true);
	} else if (execution_type != TRIGGER_GROUP_EXECUTION_MULTIPLE && execution_type != TRIGGER_GROUP_EXECUTION_SINGLE) {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "Unknown TriggerGroup execution type not implemented yet!");
		return false;
	}

	// Workaround for hack which allows the camera and camera target to be selected out of order when called from a TriggerGroup.
	short selected_camera = -1;
	short selected_target = -1;

	NG_TRIGGER_GROUP& trigger_group = current_trigger_groups[trigger_group_id];
	int index = 0;

	bool parsed_first_operation = false;
	bool operation_result = false;

	if (trigger_group.data[index].first_field == 0x0000) {
		NGLog(NG_LOG_TYPE_ERROR, "Attempted to execute NULL TriggerGroup (%u)!", trigger_group_id);
	}

	while (index < trigger_group.data_size) {
		// Check of unsupported TGROUP flags
		if (trigger_group.data[index].first_field & TGROUP_USE_EXECUTOR_ITEM_INDEX ||
			trigger_group.data[index].first_field & TGROUP_USE_ITEM_USED_BY_LARA_INDEX ||
			trigger_group.data[index].first_field & TGROUP_USE_OWNER_ANIM_ITEM_INDEX ||
			trigger_group.data[index].first_field & TGROUP_USE_TRIGGER_ITEM_INDEX) {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "Unsupported TGROUP flags detected!");
			return false;
		}

		if ((trigger_group.data[index].first_field & TGROUP_SINGLE_SHOT) ||
			NGISTriggerGroupDataResumed(&trigger_group.data[index])) {
			if (trigger_group.oneshot_triggered) {
				return false;
			} else {
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

			if (trigger_group.data[index].plugin_id != 0) {
				int t4_plugin_id = NGGetT4PluginID(trigger_group.data[index].plugin_id);
				char *plugin_string = NGGetPluginString(trigger_group.data[index].plugin_id);

				if ((trigger_group.data[index].first_field & 0xF000) == 0x8000 || (trigger_group.data[index].first_field & 0xF000) == 0x9000) {
					current_result = false;
				} else {
					current_result = true;
				}

				if (t4_plugin_id != -1) {
					// Flipeffect
					if ((trigger_group.data[index].first_field & 0xF000) == 0x2000) {
						if (trigger_group.data[index].first_field & TGROUP_USE_FOUND_ITEM_INDEX) {
							NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TGROUP_USE_FOUND_ITEM_INDEX used on flipeffect");
						} else {
							current_result = NGFlipEffect(trigger_group.data[index].second_field_lower, trigger_group.data[index].third_field_lower & 0x7fff, false, true);
						}

						if (!current_result) {
							NGLog(NG_LOG_TYPE_ERROR, "Flipeffect returned false!");
							current_result = true;
						}
					} else {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "Plugin callback is unsupported!");
					}
				} else {
					if (plugin_string) {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "Plugin triggers are not yet supported (trigger_id: %u, plugin:%s, first_field:0x%x, second_field:%u, third_field:0x%x)",
							trigger_group_id,
							plugin_string,
							trigger_group.data[index].first_field,
							((int)trigger_group.data[index].second_field_upper << 16 | (int)trigger_group.data[index].second_field_lower),
							((int)trigger_group.data[index].third_field_upper << 16 | (int)trigger_group.data[index].third_field_lower));
					} else {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "Plugin triggers are not yet supported (trigger_id: %u, plugin_id:%u, first_field:0x%x, second_field:%u, third_field:0x%x)",
							trigger_group_id,
							trigger_group.data[index].plugin_id,
							trigger_group.data[index].first_field,
							((int)trigger_group.data[index].second_field_upper << 16 | (int)trigger_group.data[index].second_field_lower),
							((int)trigger_group.data[index].third_field_upper << 16 | (int)trigger_group.data[index].third_field_lower));
					}
				}
			} else {
				// ActionNG
				if (trigger_group.data[index].first_field & TGROUP_ACTION) {
					if (trigger_group.data[index].first_field & TGROUP_MOVEABLE) {

						short item_id = NO_ITEM;
						if (trigger_group.data[index].first_field & TGROUP_USE_FOUND_ITEM_INDEX) {
							item_id = ng_found_item_index;
						} else {
							item_id = ng_script_id_table[trigger_group.data[index].second_field_lower].script_index;
						}

						NGAction(item_id, trigger_group.data[index].third_field_lower & 0x7fff, NG_TRIGGER_FLAG_HEAVY | NG_TRIGGER_FLAG_SCRIPT_TRIGGERED);

						// Workaround to some weird behaviour which allows cameras and targets to be assigned out of order from a script trigger.
						unsigned char action_type = (unsigned char)(trigger_group.data[index].third_field_lower & 0x7fff) & 0xff;
						switch (action_type) {
							case ACTIVATE_CAMERA_WITH_TIMER: {
								if (item_id >= number_cameras) {
									NGLog(NG_LOG_TYPE_ERROR, "Invalid camera number.");
									break;
								}

								if (camera.fixed[item_id].flags & 0x100)
									break;

								selected_camera = item_id;
								break;
							}
							case SET_MOVEABLE_AS_TARGET_FOR_CAMERA: {
								if (item_id >= ITEM_COUNT) {
									NGLog(NG_LOG_TYPE_ERROR, "Invalid camera target.");
									break;
								}

								selected_target = item_id;
								break;
							}
						}

						if (!current_result) {
							current_result = true;
						}
					} else {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ActionNG (statics) unsupported!");
						if (!current_result) {
							current_result = true;
						}
					}
				} else if (trigger_group.data[index].first_field & TGROUP_CONDITION_TRIGGER) {
					// ConditionNG (item id)
					if (trigger_group.data[index].first_field & TGROUP_MOVEABLE) {
						if (trigger_group.data[index].first_field & TGROUP_USE_FOUND_ITEM_INDEX) {
							current_result = NGCondition(ng_found_item_index, (trigger_group.data[index].third_field_lower >> 8) & 0xff, trigger_group.data[index].third_field_lower & 0xff);
						}
						else {
							current_result = NGCondition(ng_script_id_table[trigger_group.data[index].second_field_lower].script_index, (trigger_group.data[index].third_field_lower >> 8) & 0xff, trigger_group.data[index].third_field_lower & 0xff);

						}
					} else {
						if (trigger_group.data[index].first_field & TGROUP_USE_FOUND_ITEM_INDEX) {
							NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TGROUP_USE_FOUND_ITEM_INDEX used on condition");
						} else {
							current_result = NGCondition(trigger_group.data[index].second_field_lower, (trigger_group.data[index].third_field_lower >> 8) & 0xff, trigger_group.data[index].third_field_lower & 0xff);
						}
					}
				} else if (trigger_group.data[index].first_field & TGROUP_FLIPEFFECT) {
					if (trigger_group.data[index].first_field & TGROUP_USE_FOUND_ITEM_INDEX) {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TGROUP_USE_FOUND_ITEM_INDEX used on flipeffect");
					} else {
						current_result = NGFlipEffect(trigger_group.data[index].second_field_lower, trigger_group.data[index].third_field_lower & 0x7fff, false, true);
					}

					if (!current_result) {
						NGLog(NG_LOG_TYPE_ERROR, "Flipeffect returned false!");
						current_result = true;
					}
				} else if (trigger_group.data[index].first_field == 0x0000) {
					break;
				} else {
					NGLog(NG_LOG_TYPE_ERROR, "Unknown triggergroup command!");
					operation_result = false;
					break;
				}
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

	if (operation_result == true) {
		trigger_group.was_executed = true;
	}

	if (selected_camera != NO_ITEM && selected_target != NO_ITEM) {
		camera.item = &items[selected_target];
	}

	return operation_result;
}

void NGProcessTriggerGroups() {
	for (int i = 0; i < MAX_NG_TRIGGER_GROUPS; i++) {
		for (int j = 0; j < current_trigger_groups[i].data_size; j++) {
			if (NGISTriggerGroupDataResumed(&current_trigger_groups[i].data[j])) {
				if (!current_trigger_groups[i].was_executed) {
					current_trigger_groups[i].oneshot_triggered = false;
				}
			}
		}

		current_trigger_groups[i].was_executed = false;

		if (NGIsTriggerGroupContinuous(i)) {
			NGTriggerGroupFunction(i, TRIGGER_GROUP_EXECUTION_MULTIPLE);
		}
	}
}