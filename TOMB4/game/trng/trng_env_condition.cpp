#include "../../tomb4/pch.h"
#include "trng_env_condition.h"
#include "trng_script_parser.h"
#include "trng.h"
#include "../lara.h"
#include "../control.h"
#include "trng_test_position.h"
#include "trng_triggergroup.h"

int NGProportionDistance(int increment, int distance) {
	return (int)((float)increment * ((float)distance / 1024.0f));
}

void NGCalculateIncrement(short orientation, int* inc_x_out, int* inc_z_out, int distance) {
	int inc_x, inc_z;
	int Indice;

	if (distance == 0) {
		*inc_x_out = 0;
		*inc_z_out = 0;
		return;
	}

	Indice = orientation >> 3;
	Indice &= 0x1FFE;

	inc_x = rcossin_tbl[Indice] << 12;
	inc_z = rcossin_tbl[Indice + 1] << 12;
	inc_x = inc_x >> 14;
	inc_z = inc_z >> 14;
	if (distance != 1024) {
		inc_x = NGProportionDistance(inc_x, distance);
		inc_z = NGProportionDistance(inc_z, distance);
	}

	*inc_x_out = inc_x;
	*inc_z_out = inc_z;
}


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

		result.is_valid = false;
		result.seek_item = -1;
		result.test_position_id = -1;

		return result;
	}

	if (check_hortogonal) {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TestEnvConditionTriplet: hortogonal detection not yet supported!", triplet->env_condition);

		result.is_valid = false;
		result.seek_item = -1;
		result.test_position_id = -1;

		return result;
	}

	unsigned int env_condition_switch_value = triplet->env_condition & 0x7f;
	switch (env_condition_switch_value) {
		case 0: {
			result.is_valid = true;
			break;
		}
		case ENV_NO_BLOCK_IN_FRONT:
		case ENV_NO_BLOCK_AT_RIGHT:
		case ENV_NO_BLOCK_AT_LEFT:
		case ENV_NO_BLOCK_BACK: {
			short orientation = lara_item->pos.y_rot;
			long coord_x = lara_item->pos.x_pos;
			long coord_y = lara_item->pos.y_pos;
			long coord_z = lara_item->pos.z_pos;
			short distance_test = 768;
			if (triplet->distance_for_env != -1) {
				distance_test = triplet->distance_for_env;
			}

			short room_num = lara_item->room_number;
			FLOOR_INFO *floor_info = GetFloor(coord_x, coord_y, coord_z, &room_num);
			int height = GetHeight(floor_info, coord_x, coord_y, coord_z);

			switch (env_condition_switch_value) {
				case ENV_NO_BLOCK_AT_RIGHT:
					orientation += 0x4000;
					break;
				case ENV_NO_BLOCK_AT_LEFT:
					orientation -= 0x4000;
					break;
				case ENV_NO_BLOCK_BACK:
					orientation += 0x8000;
					break;
			}

			int inc_x;
			int inc_z;
			NGCalculateIncrement(orientation, &inc_x, &inc_z, 1024);

			coord_x += inc_x;
			coord_z += inc_z;

			floor_info = GetFloor(coord_x, coord_y, coord_z, &room_num);
			height = GetHeight(floor_info, coord_x, coord_y, coord_z);

			if (height == NO_HEIGHT) {
				result.is_valid = false;
				break;
			}

			if ((lara_item->pos.y_pos - height) >= distance_test) {
				result.is_valid = false;
				break;
			}

			int ceiling = GetCeiling(floor_info, coord_x, coord_y, coord_z);
			if ((lara_item->pos.y_pos - distance_test) < ceiling) {
				result.is_valid = false;
				break;
			}

			result.is_valid = true;
			break;
		}
		case ENV_HOLE_FLOOR_IN_FRONT:
		case ENV_HOLE_FLOOR_AT_RIGHT:
		case ENV_HOLE_FLOOR_AT_LEFT:
		case ENV_HOLE_FLOOR_BACK: {
			short orientation = lara_item->pos.y_rot;
			long coord_x = lara_item->pos.x_pos;
			long coord_y = lara_item->pos.y_pos;
			long coord_z = lara_item->pos.z_pos;
			short distance_test = 768;
			if (triplet->distance_for_env != -1) {
				distance_test = triplet->distance_for_env;
			}

			switch (env_condition_switch_value) {
				case ENV_HOLE_FLOOR_AT_RIGHT:
					orientation += 0x4000;
					break;
				case ENV_HOLE_FLOOR_AT_LEFT:
					orientation -= 0x4000;
					break;
				case ENV_HOLE_FLOOR_BACK:
					orientation += 0x8000;
					break;
			}

			int inc_x;
			int inc_z;
			NGCalculateIncrement(orientation, &inc_x, &inc_z, 1024);

			coord_x += inc_x;
			coord_z += inc_z;

			short room_num = lara_item->room_number;

			FLOOR_INFO* floor_info = GetFloor(coord_x, coord_y, coord_z, &room_num);
			int height = GetHeight(floor_info, coord_x, coord_y, coord_z);

			if (height == NO_HEIGHT) {
				result.is_valid = false;
				break;
			}

			if ((height - lara_item->pos.y_pos) < distance_test) {
				result.is_valid = false;
				break;
			}

			int ceiling = GetCeiling(floor_info, coord_x, coord_y, coord_z);
			if ((lara_item->pos.y_pos - distance_test) < ceiling) {
				result.is_valid = false;
				break;
			}

			result.is_valid = true;
			break;
		}
		case ENV_MULT_CONDITION: {
			result.is_valid = true;

			NG_MULTI_ENV_CONDITION* multi_env_cond = &current_multi_env_conditions[triplet->distance_for_env];
			for (int i = 0; i < multi_env_cond->env_condition_triplet_count; i++) {
				TestEnvConditionTripletResult sub_result = TestEnvConditionTriplet(&multi_env_cond->env_condition_triplet_array[i], set_alignment_variables);

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
		case ENV_DISTANCE_CEILING: {
			FLOOR_INFO *floor = GetFloor(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, &lara_item->room_number);
			long ceiling = GetCeiling(floor, lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);

			long distance_to_ceiling = -(ceiling - lara_item->pos.y_pos);

			if (distance_to_ceiling >= triplet->distance_for_env) {
				result.is_valid = true;
			} else {
				result.is_valid = false;
			}

			break;
		}
		case ENV_MULT_OR_CONDITION: {
			result.is_valid = false;

			NG_MULTI_ENV_CONDITION* multi_env_cond = &current_multi_env_conditions[triplet->distance_for_env];
			for (int i = 0; i < multi_env_cond->env_condition_triplet_count; i++) {
				TestEnvConditionTripletResult sub_result = TestEnvConditionTriplet(&multi_env_cond->env_condition_triplet_array[i], set_alignment_variables);

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
		case ENV_FRAME_NUMBER: {
			if ((lara_item->frame_number - anims[lara_item->anim_number].frame_base) == triplet->distance_for_env) {
				result.is_valid = true;
			} else {
				result.is_valid = false;
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
		case ENV_ROOM_IS: {
			if (lara_item->room_number == triplet->distance_for_env) {
				result.is_valid = true;
			} else {
				result.is_valid = false;
			}

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
		case ENV_CONDITION_TRIGGER_GROUP: {
			result.is_valid = NGTriggerGroupFunction(triplet->distance_for_env, TRIGGER_GROUP_EXECUTION_MULTIPLE);

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

bool TestMultiEnvCondition(int multi_env_condition_id, bool evaluate_as_or) {
	bool is_valid;
	if (evaluate_as_or)
		is_valid = false;
	else
		is_valid = true;

	NG_MULTI_ENV_CONDITION* multi_env_cond = &current_multi_env_conditions[multi_env_condition_id];
	for (int i = 0; i < multi_env_cond->env_condition_triplet_count; i++) {
		TestEnvConditionTripletResult sub_result = TestEnvConditionTriplet(&multi_env_cond->env_condition_triplet_array[i], false);

		if (evaluate_as_or) {
			if (sub_result.is_valid) {
				is_valid = true;
				break;
			}
		} else {
			if (!sub_result.is_valid) {
				is_valid = false;
				break;
			}
		}
	}
	return is_valid;
}