#pragma once

#include "trng_script_parser.h"

enum NGEnvironmentType {
	ENV_NO_BLOCK_IN_FRONT = 0x0001,
	ENV_HANG_WITH_FEET = 0x0002,
	ENV_HOLE_FLOOR_IN_FRONT = 0x0003,
	ENV_MONKEY_CEILING = 0x0004,
	ENV_CLIMB_WALL_IN_FRONT = 0x0005,
	ENV_CLIMB_WALL_AT_RIGHT = 0x0006,
	ENV_CLIMB_WALL_AT_LEFT = 0x0007,
	ENV_HOLE_IN_FRONT_CEILING_CLIMB = 0x0008,
	ENV_HOLE_BACK_CEILING_CLIMB = 0x0009,
	ENV_NO_BLOCK_AT_RIGHT = 0x000a,
	ENV_NO_BLOCK_AT_LEFT = 0x000b,
	ENV_HOLE_FLOOR_AT_RIGHT = 0x000c,
	ENV_HOLE_FLOOR_AT_LEFT = 0x000d,
	ENV_HOLE_FLOOR_BACK = 0x000e,
	ENV_NO_BLOCK_BACK = 0x000f,
	ENV_CLIMB_WALL_BACK = 0x0010,
	ENV_SUPPORT_IN_FRONT_WALL = 0x0011,
	ENV_SUPPORT_IN_RIGHT_WALL = 0x0012,
	ENV_SUPPORT_IN_LEFT_WALL = 0x0013,
	ENV_SUPPORT_IN_BACK_WALL = 0x0014,
	ENV_ITEM_EXTRA_IN_FRONT = 0x0015,
	ENV_ITEM_EXTRA_OVER = 0x0016,
	ENV_ITEM_EXTRA_UNDER = 0x0017,
	ENV_MULT_CONDITION = 0x0018,
	ENV_HANG_LEFT_IN_CORNER = 0x0019,
	ENV_HANG_LEFT_OUT_CORNER = 0x001a,
	ENV_HANG_RIGHT_IN_CORNER = 0x001b,
	ENV_HANG_RIGHT_OUT_CORNER = 0x001c,
	ENV_HANG_LEFT_SPACE = 0x001d,
	ENV_HANG_RIGHT_SPACE = 0x001e,
	ENV_DISTANCE_CEILING = 0x001f,
	ENV_CLIMB_LEFT_IN_CORNER = 0x0020,
	ENV_CLIMB_LEFT_OUT_CORNER = 0x0021,
	ENV_CLIMB_RIGHT_IN_CORNER = 0x0022,
	ENV_CLIMB_RIGHT_OUT_CORNER = 0x0023,
	ENV_CLIMB_LEFT_SPACE = 0x0024,
	ENV_CLIMB_RIGHT_SPACE = 0x0025,
	ENV_MULT_OR_CONDITION = 0x0026,
	ENV_DISTANCE_FLOOR = 0x0027,
	ENV_FRAME_NUMBER = 0x0028,
	ENV_VERTICAL_ORIENT = 0x0029,
	ENV_ON_VEHICLE = 0x002a,
	ENV_FREE_HANDS = 0x002b,
	ENV_UNDERWATER = 0x002c,
	ENV_FLOATING = 0x002d,
	ENV_ONLAND = 0x002e,
	ENV_IS_STILL = 0x002f,
	ENV_ANIM_COMPLETE = 0x0030,
	ENV_FLYING_UP = 0x0031,
	ENV_FLYING_DOWN = 0x0032,
	ENV_WALL_HOLE_IN_FRONT = 0x0033,
	ENV_IN_LEFT_SIDE_SECTOR = 0x0034,
	ENV_IN_RIGHT_SIDE_SECTOR = 0x0035,
	ENV_ITEM_EXTRA_AT_LEFT = 0x0036,
	ENV_ITEM_EXTRA_AT_RIGHT = 0x0037,
	ENV_ITEM_TEST_POSITION = 0x0038,
	ENV_CONDITION_TRIGGER_GROUP = 0x003a,
	ENV_ROOM_IS = 0x003b,
	ENV_PLAYER_IS_SLEEPING = 0x003c,
	ENV_PLAYER_WOKE_UP = 0x003d,
	ENV_DISTANCE_NORTH_WALL = 0x003e,
	ENV_DISTANCE_SOUTH_WALL = 0x003f,
	ENV_DISTANCE_EAST_WALL = 0x0040,
	ENV_DISTANCE_WEST_WALL = 0x0041,
	ENV_LARA_IN_MICRO_STRIP = 0x0042,
	ENV_NO_BOX_IN_FRONT = 0x0043,
	ENV_NO_BOX_AT_LEFT = 0x0044,
	ENV_NO_BOX_AT_RIGHT = 0x0045,
	ENV_NO_BOX_BACK = 0x0046,
	ENV_ENEMY_SEE_LARA = 0x0047,
	ENV_FRAME_RANGE = 0x0048,

	ENV_NON_TRUE = 0x0080,

	ENV_POS_LEFT_CORNER = 0x0100,
	ENV_POS_RIGHT_CORNER = 0x200,
	ENV_POS_CENTRAL = 0x0400,
	ENV_POS_HORTOGONAL = 0x0800,
	ENV_POS_IN_THE_MIDDLE = 0x1000,
	ENV_POS_STRIP_3 = 0x2000,
	ENV_POS_STRIP_2 = 0x4000,
	ENV_POS_STRIP_1 = 0x8000,
};

struct TestEnvConditionTripletResult {
	bool is_valid = false;
	int seek_item = -1;
	int test_position_id = -1;
};

TestEnvConditionTripletResult TestEnvConditionTriplet(NG_MULTI_ENV_TRIPLET* triplet, bool set_alignment_variables, int32_t item_index);
bool TestMultiEnvCondition(int multi_env_condition_id, bool evaluate_as_or, int32_t item_index);