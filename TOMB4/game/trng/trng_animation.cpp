#include "../../tomb4/pch.h"
#include "../gameflow.h"
#include "trng_script_parser.h"
#include "trng_animation.h"
#include "../../specific/input.h"
#include "../lara.h"
#include "trng_env_condition.h"

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

		for (int i = 0; i < animation->state_or_animation_condition_count; i++) {
			if (animation->state_or_animation_condition_array[i] < 0) {
				// Animation indicies
				if (lara_item->anim_number == -animation->state_or_animation_condition_array[i]) {
					is_valid = true;
				}
			}
			else {
				// State IDs
				if (lara_item->current_anim_state == animation->state_or_animation_condition_array[i]) {
					is_valid = true;
				}
			}
		}

		if (animation->environment.env_condition == 0xffff ||
			animation->environment.env_condition == 0) {
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
		}
	}
}

void NGProcessAnimations() {
	if (ng_levels[gfCurrentLevel].records) {
		int animation_count = ng_levels[gfCurrentLevel].records->animation_count;
		for (int i = 0; i < animation_count; i++) {
			int record_id = ng_levels[gfCurrentLevel].records->animation_table[i].record_id;
			NG_ANIMATION *animation = &ng_levels[gfCurrentLevel].records->animation_table[record_id].record;
			NGTestAnimation(animation);
		}
	}
}