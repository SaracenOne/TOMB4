#include "../../tomb4/pch.h"
#include "../gameflow.h"
#include "trng_script_parser.h"
#include "trng_animation.h"
#include "../../specific/input.h"
#include "../lara.h"
#include "trng_env_condition.h"
#include "trng_triggergroup.h"
#include "trng.h"
#include "trng_test_position.h"
#include "../control.h"
#include "../collide.h"
#include "../lara_states.h"
#include "../../specific/3dmath.h"

#define NG_ANIM_TEST_INPUT(animation_flag, input_flag, return_variable) \
if (animation->key_1 & animation_flag) { \
	if (animation->key_1 & KEY1_RELEASED) {\
		if (input & input_flag) { \
			return_variable = false; \
		} \
	} else { \
		if (!(input & input_flag)) { \
			return_variable = false; \
		} \
	} \
}

void NGTestAnimation(NG_ANIMATION *animation) {
	if (animation) {
		bool is_valid = false;

		// Add newly support flags here...
		if (animation->fan_flags & ~(FAN_PERFORM_TRIGGER_GROUP | FAN_ALIGN_TO_ENV_POS | FAN_SET_BUSY_HANDS | FAN_SET_FREE_HANDS)) {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGTestAnimation: Unsupported FAN_ flags detected!");
			return;
		}

		if (animation->key_2 != 0 && animation->key_2 != 0xffff) {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGTestAnimation: key_2 support missing!");
			return;
		}

		if (animation->state_or_animation_condition_count == 0) {
			is_valid = true;
		} else {
			for (int i = 0; i < animation->state_or_animation_condition_count; i++) {
				if (animation->state_or_animation_condition_array[i] < 0) {
					// Animation indicies
					if (lara_item->anim_number == -animation->state_or_animation_condition_array[i]) {
						is_valid = true;
					}
				} else {
					// State IDs
					if (lara_item->current_anim_state == animation->state_or_animation_condition_array[i]) {
						is_valid = true;
					}
				}
			}
		}
		if (is_valid) {
			if (1) {
				NG_ANIM_TEST_INPUT(KEY1_UP, IN_FORWARD, is_valid);
				NG_ANIM_TEST_INPUT(KEY1_DOWN, IN_BACK, is_valid);
				NG_ANIM_TEST_INPUT(KEY1_LEFT, IN_LEFT, is_valid);
				NG_ANIM_TEST_INPUT(KEY1_RIGHT, IN_RIGHT, is_valid);
				NG_ANIM_TEST_INPUT(KEY1_JUMP, IN_JUMP, is_valid);
				NG_ANIM_TEST_INPUT(KEY1_DRAW_WEAPON, IN_DRAW, is_valid);
				NG_ANIM_TEST_INPUT(KEY1_ACTION, IN_ACTION, is_valid);
				NG_ANIM_TEST_INPUT(KEY1_WALK, IN_WALK, is_valid);
				NG_ANIM_TEST_INPUT(KEY1_LOOK, IN_LOOK, is_valid);
				NG_ANIM_TEST_INPUT(KEY1_ROLL, IN_ROLL, is_valid);
			}
		}

		if (is_valid) {
			TestEnvConditionTripletResult result = TestEnvConditionTriplet(&animation->environment, animation->fan_flags & FAN_ALIGN_TO_ENV_POS);

			if (result.is_valid) {
				if (animation->fan_flags & FAN_SET_BUSY_HANDS) {
					lara.gun_status = LG_HANDS_BUSY;
				}

				if (animation->fan_flags & FAN_ALIGN_TO_ENV_POS) {
					ng_animation_target_item = result.seek_item;
					ng_animation_target_test_position = result.test_position_id;

					if (ng_animation_target_item >= 0 && ng_animation_target_test_position >= 0) {
						NG_TEST_POSITION* test_position = &current_test_positions[ng_animation_target_test_position];
						PHD_VECTOR target_position = NGCalculatePositionForTestPosition(test_position);

						lara.GeneralPtr = 0;

						{
							ITEM_INFO* item = &items[ng_animation_target_item];
							if (!NGTestLaraDistance(&target_position, item, lara_item)) {
								if (!NGMoveLara(false)) {
									result.is_valid = false;
								}
							}
							else {
								AlignLaraPosition(&target_position, item, lara_item);
							}
						}
					} else {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGTestAnimation: Not valid item or test position found to align to!");
					}
				}

				if (result.is_valid) {
					ng_animation_target_item = -1;
					ng_animation_target_test_position = -1;

					if (animation->fan_flags & FAN_PERFORM_TRIGGER_GROUP) {
						NGTriggerGroupFunction(animation->animation_index, 0);
					} else {
						lara_item->anim_number = animation->animation_index;
						lara_item->frame_number = anims[animation->animation_index].frame_base;
					}

					if (animation->fan_flags & FAN_SET_FREE_HANDS) {
						lara.gun_status = LG_NO_ARMS;
					}
				}
			}
		}
	}
}

void NGProcessAnimations() {
	NGMoveLara(true);

	if (ng_levels[gfCurrentLevel].records) {
		int animation_count = ng_levels[gfCurrentLevel].records->animation_count;
		for (int i = 0; i < animation_count; i++) {
			int record_id = ng_levels[gfCurrentLevel].records->animation_table[i].record_id;
			NG_ANIMATION *animation = &ng_levels[gfCurrentLevel].records->animation_table[record_id].record;
			NGTestAnimation(animation);
		}
	}
}