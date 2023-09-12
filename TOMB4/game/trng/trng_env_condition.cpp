#include "../../tomb4/pch.h"
#include "trng_env_condition.h"
#include "trng_script_parser.h"
#include "trng.h"
#include "../lara.h"
#include "../control.h"
#include "trng_test_position.h"

TestEnvConditionTripletResult TestEnvConditionTriplet(NG_MULTI_ENV_TRIPLET* triplet, bool set_alignment_variables) {
	TestEnvConditionTripletResult result;

	if (triplet->env_condition == 0xffff) {
		result.is_valid = true;
		result.seek_item = -1;
		result.test_position_id = -1;

		return result;
	}

	if (triplet->env_condition & ~(0xe07f + ENV_NON_TRUE + ENV_POS_HORTOGONAL)) {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TestEnvConditionTriplet: unsupported mask features!", triplet->env_condition);

		result.is_valid = false;
		result.seek_item = -1;
		result.test_position_id = -1;

		return result;
	}

	bool check_forward_strip = triplet->env_condition & ENV_POS_STRIP_1;
	bool check_middle_strip = triplet->env_condition & ENV_POS_STRIP_2;
	bool check_back_strip = triplet->env_condition & ENV_POS_STRIP_3;
	bool check_hortogonal = triplet->env_condition & ENV_POS_HORTOGONAL;

	if (check_forward_strip || check_middle_strip || check_back_strip) {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TestEnvConditionTriplet: sector strip detection not yet supported!", triplet->env_condition);
	}

	if (check_hortogonal) {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TestEnvConditionTriplet: hortogonal detection not yet supported!", triplet->env_condition);
	}

	unsigned int env_condition_switch_value = triplet->env_condition & 0x7f;
	switch (env_condition_switch_value) {
		case 0: {
			result.is_valid = true;
			break;
		}
		case ENV_MULT_CONDITION: {
			result.is_valid = true;

			NG_MULTI_ENV_CONDITION* multi_env_cond = &current_multi_env_conditions[triplet->distance_for_env];
			for (int i = 0; i < multi_env_cond->env_condition_triplet_count; i++) {
				TestEnvConditionTripletResult sub_result = TestEnvConditionTriplet(&multi_env_cond->env_condition_triplet_array[i], true);

				if (!sub_result.is_valid) {
					result.is_valid = false;
					break;
				} else {
					if (set_alignment_variables) {
						// This is probably incorrect. Investigate behaviour...
						if (result.seek_item == -1) {
							result.seek_item = sub_result.seek_item;
						}
						if (result.test_position_id == -1) {
							result.test_position_id = sub_result.test_position_id;
						}
					}
				}
			}
			break;
		}
		case ENV_MULT_OR_CONDITION: {
			result.is_valid = false;

			NG_MULTI_ENV_CONDITION* multi_env_cond = &current_multi_env_conditions[triplet->distance_for_env];
			for (int i = 0; i < multi_env_cond->env_condition_triplet_count; i++) {
				TestEnvConditionTripletResult sub_result = TestEnvConditionTriplet(&multi_env_cond->env_condition_triplet_array[i], true);

				if (sub_result.is_valid) {
					result.is_valid = true;
				} else {
					if (set_alignment_variables) {
						// This is probably incorrect. Investigate behaviour...
						if (result.seek_item == -1) {
							result.seek_item = sub_result.seek_item;
						}
						if (result.test_position_id == -1) {
							result.test_position_id = sub_result.test_position_id;
						}
					}
				}
			}
			break;
		}
		case ENV_FREE_HANDS: {
			if ((lara.gun_status == LG_NO_ARMS || lara.gun_status == LG_FLARE))
				result.is_valid = true;
			else
				result.is_valid = false;

			break;
		}
		case ENV_WALL_HOLE_IN_FRONT: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TestEnvConditionTriplet: ENV_WALL_HOLE_IN_FRONT is unimplemented!");

			result.is_valid = false;

			break;
		}
		case ENV_ITEM_TEST_POSITION: {
			ITEM_INFO *item = NULL;
			short item_number;

			result.is_valid = false;
			for (item_number = room[lara_item->room_number].item_number; item_number != NO_ITEM; item_number = item->next_item) {
				item = &items[item_number];
				int test_position_id = triplet->distance_for_env;

				NG_TEST_POSITION &test_position = current_test_positions[test_position_id];

				if (NGTestLaraPosition(&test_position, item, lara_item) == true) {
					result.is_valid = true;
					result.seek_item = item_number;
					result.test_position_id = test_position_id;
				}
			}
			break;
		}
		case ENV_LARA_IN_MICRO_STRIP: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TestEnvConditionTriplet: ENV_LARA_IN_MICRO_STRIP is unimplemented!");
			result.is_valid = false;

			break;
		}
		default: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TestEnvConditionTriplet: Unimplemented environment condition: %u!", env_condition_switch_value);
			result.is_valid = false;

			break;
		}
	}

	if (triplet->env_condition & ENV_NON_TRUE) {
		result.is_valid = !result.is_valid;
	}

	return result;
}