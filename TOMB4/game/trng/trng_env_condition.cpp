#include "../../tomb4/pch.h"
#include "trng_env_condition.h"
#include "trng_script_parser.h"
#include "trng.h"
#include "../lara.h"
#include "../control.h"
#include "trng_test_position.h"

int TestEnvConditionTriplet(NG_MULTI_ENV_TRIPLET* triplet) {
	if (triplet->env_condition == 0xffff)
		return 0;

	if (triplet->env_condition & ~(0xe07f + ENV_NON_TRUE + ENV_POS_HORTOGONAL)) {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TestEnvConditionTriplet: unsupported mask features!", triplet->env_condition);
		return -1;
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

	int ret_val = 0;

	unsigned int env_condition_switch_value = triplet->env_condition & 0x7f;
	switch (env_condition_switch_value) {
		case 0: {
			ret_val = 0;
			break;
		}
		case ENV_MULT_CONDITION: {
			NG_MULTI_ENV_CONDITION* multi_env_cond = &current_multi_env_conditions[triplet->distance_for_env];
			for (int i = 0; i < multi_env_cond->env_condition_triplet_count; i++) {
				if (TestEnvConditionTriplet(&multi_env_cond->env_condition_triplet_array[i]) < 0) {
					ret_val = -1;
				}
			}
			break;
		}
		case ENV_FREE_HANDS: {
			if ((lara.gun_status == LG_NO_ARMS || lara.gun_status == LG_FLARE))
				ret_val = 0;
			else
				ret_val = -1;

			break;
		}
		case ENV_WALL_HOLE_IN_FRONT: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TestEnvConditionTriplet: ENV_WALL_HOLE_IN_FRONT is unimplemented!");

			ret_val = 0;

			break;
		}
		case ENV_ITEM_TEST_POSITION: {
			ITEM_INFO *item = NULL;
			short item_number;

			ret_val = -1;
			for (item_number = room[lara_item->room_number].item_number; item_number != NO_ITEM; item_number = item->next_item) {
				item = &items[item_number];

				NG_TEST_POSITION &test_position = current_test_positions[triplet->distance_for_env];

				if (NGTestLaraPosition(&test_position, item, lara_item) == true) {
					ret_val = item_number;
				}
			}
			break;
		}
		case ENV_LARA_IN_MICRO_STRIP: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TestEnvConditionTriplet: ENV_LARA_IN_MICRO_STRIP is unimplemented!");
			ret_val = 0;

			break;
		}
		default: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TestEnvConditionTriplet: Unimplemented environment condition: %u!", env_condition_switch_value);
			ret_val = -1;

			break;
		}
	}

	if (triplet->env_condition & ENV_NON_TRUE) {
		if (ret_val < 0)
			ret_val = 0;
		else
			ret_val = -1;
	}

	return ret_val;
}