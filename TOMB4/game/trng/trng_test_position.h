#pragma once
#include "trng_script_parser.h"

enum TestPositionFlags {
	TPOS_DOUBLE_HORIENT = 0x0001,
	TPOS_FOUR_HORIENT = 0x0002,
	TPOS_TEST_ITEM_INDEX = 0x0004,
	TPOS_FAST_ALIGNMENT = 0x0008,
	TPOS_OPPOSITE_FACING = 0x0010,
};

long NGTestLaraDistance(PHD_VECTOR* target, ITEM_INFO* item, ITEM_INFO* l);
PHD_VECTOR NGCalculatePositionForTestPosition(NG_TEST_POSITION* test_position);
bool NGTestLaraPosition(NG_TEST_POSITION* test_position, ITEM_INFO* item, ITEM_INFO* l);
bool NGMoveLara(bool set_stop_animation_when_goal_reached);