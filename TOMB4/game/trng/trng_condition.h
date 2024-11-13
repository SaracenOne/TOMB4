#pragma once

enum NG_CONDITION_TYPE {
	INVENTORY_ITEM_IS_MISSING = 1,
	INVENTORY_ITEM_IS_PRESENT = 2,
	INVENTORY_ITEM_HAS_AT_LEAST = 3,
	INVENTORY_ITEM_HAS_LESS_THAN = 4,
	LARA_IS_PERFORMING_X_ACTION = 5,
	FRAGMENTED_TRIGGGER_CHECK_IN_WAY_IF_LARA_IS_IN_FRAGMENT_OF_2X2_SECTOR_GRID = 6,
	FRAGMENTED_TRIGGGER_CHECK_IN_WAY_IF_LARA_IS_IN_FRAGMENT_OF_3X3_SECTOR_GRID = 7,
	FRAGMENTED_TRIGGGER_CHECK_IN_WAY_IF_LARA_IS_IN_FRAGMENT_OF_4X4_SECTOR_GRID = 8,
	VERTICAL_TRIGGER_ZONE = 9,
	VERTICAL_TRIGGER_ZONE_INVERSE = 10,
	VERTICAL_TRIGGER_ANTI_ZONE = 11,
	KEYBOARD_SCANCODE_IS_CURRENTLY = 12,
	KEYBOARD_COMMAND_GAME_IS_CURRENTLY = 13,
	CREATURE_IS_CURRENTLY = 14,
	MULTIPLE_CONDITION_OF_X_TRIGGERGROUP_SCRIPT_COMMAND = 15,
	MULTIPLE_CONDITION_OF_X_MULTIENVCONDITION_SCRIPT_COMMAND = 16,
	LARA_HAS_FOUND_AT_LEAST_X_SECRETS = 17,
	LARA_HAS_FOUND_EXACTLY_X_SECRETS = 18,
	KEYPAD_LAST_NUMBER_TYPED_IN_KEYPAD_IS_X_VALUE = 19,
	TIMER_TIMER_SCREEN_VALUE_IS_Y_THAN_X_SECONDS = 20,
	CREATURE_CURRENT_ANIMATION_0_31_IS = 21,
	CREATURE_IS_CURRENTLY_OF_STATE = 22,
	CREATURE_CURRENT_ANIMATION_32_63_IS = 23,
	CREATURE_CURRENT_ANIMATION_64_95_IS = 24,
	LARA_STATUS_IS_ENABLED_OR_DISABLED = 25,
	LARA_IS_TOUCHING_MOVEABLE_ID = 26,
	LARA_IS_TOUCHING_MOVEABLE_SLOT = 27,
	LARA_IS_TOUCHING_CREATURE_TYPE = 28,
	LARA_IS_VITALITY_IS_X_THAN = 29,
	LARA_IS_PERFORMING_ANIMATION = 30,
	LARA_IS_STATE = 31,
	ANIMTEXTURE_THE_X_ANIMRANGE_TEXTURE_IS_ENABLED_OR_DISABLED = 32,
	LARA_IS_TOUCHING_STATIC_SLOT = 33,
	LARA_IS_TOUCHING_STATIC_ID = 34,
	LARA_IS_HOLDING_OR_DRIVING_ITEMS = 35,
	CREATURE_THE_X_CREATURE_HAS_THE_TRANSPARENCY_LEVEL = 37,
	VARIABLES_THE_X_NUMERIC_VERIABLE_IS_EQUAL_OR_GREATER_TO_BIG_NUMBER_VALUE = 38,
	VARIABLES_THE_X_NUMERIC_VERIABLE_IS_LESS_THAN_BIG_NUMBER_VALUE = 39,
	VARIABLES_THE_X_NUMERIC_VERIABLE_IS_EQUAL_TO_BIG_NUMBER_VALUE = 40,
	VARIABLES_THE_X_NUMERIC_VARIABLE_IS_EQUAL_OR_GREATER_TO = 41,
	VARIABLES_THE_X_NUMERIC_VARIABLE_IS_LESS_THAN = 42,
	VARIABLES_THE_X_NUMERIC_VARIABLE_IS_EQUAL_TO = 43,
	VARIABLES_THE_X_NUMERIC_VARIABLE_HAS_THE_BIT_SET = 44,
	VARIABLES_THE_X_NUMERIC_VARIABLE_HAS_THE_BIT_CLEAR = 45,
	VARIABLES_THE_CURRENT_VALUE_VARIABLE_IS_GREATER_OR_EQUAL_THAN_X_VALUE = 46,
	VARIABLES_THE_CURRENT_VALUE_VARIABLE_IS_LESS_THAN_X_VALUE = 47,
	VARIABLES_THE_CURRENT_VALUE_VARIABLE_IS_EQUAL_THAN_X_VALUE = 48,
	LARA_IS_LESS_OR_EVEN_CLICKS_DISTANT_TO_MOVEABLE = 54,
	LARA_IS_LESS_OR_EVEN_UNITS_DISTANT_TO_MOVEABLE = 55,
	LARA_IS_IN_ROOM_TYPE = 81,
	LARA_IS_TOUCHING_MOVEABLE_WITH_MESH_NUMBER = 83,
	SOUND_THE_X_SOUND_EFFECT_IS_PLAYING = 85,
};

#define NG_MAX_OLD_CONDITIONS 512

extern NGOldTrigger old_conditions[NG_MAX_OLD_CONDITIONS];
extern int32_t old_condition_count;

extern int NGRunCondition(uint16_t plugin_id, uint16_t condition_number, uint16_t index, uint16_t extra_buttons, bool *test_restore, bool *test_skips, int *repeat_type, uint16_t flags);
extern int NGRunConditionTrigger(uint16_t *current_floor_data);

extern bool NGIsValidConditionForDummy(int32_t condition_number, int32_t main_argument, bool test_first);
extern bool NGAnalyzeDummyCondition(uint16_t *current_floor_data);

extern void NGProcessConditions();