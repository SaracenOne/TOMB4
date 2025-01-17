#pragma once

enum AnimationKey1Code {
	KEY1_UP = 0x0001,
	KEY1_DOWN = 0x0002,
	KEY1_LEFT = 0x0004,
	KEY1_RIGHT = 0x0008,
	KEY1_JUMP = 0x0010,
	KEY1_DRAW_WEAPON = 0x0020,
	KEY1_ACTION = 0x0040,
	KEY1_WALK = 0x0080,
	KEY1_LOOK = 0x0200,
	KEY1_ROLL = 0x1000,

	KEY1_RELEASED = 0x8000,
};

enum AnimationKey2Code {
	KEY2_USE_FLARE = 0x0008,
	KEY2_DUCK = 0x2000,
	KEY2_DASH = 0x4000,
};

enum FANFlags {
	FAN_SET_FREE_HANDS = 0x0001,
	FAN_START_FROM_EXTRA_FRAME = 0x0002,
	FAN_SET_NEUTRAL_STATE_ID =0x0004,
	FAN_KEYS_AS_SCANCODE = 0x0008,
	FAN_DISABLE_PUSHAWAY = 0x0010,
	FAN_KEEP_NEXT_STATEID = 0x0020,
	FAN_ENABLE_GRAVITY = 0x0040,
	FAN_DISABLE_GRAVITY = 0x0080,
	FAN_PERFORM_TRIGGER_GROUP = 0x0100,
	FAN_SET_FREE_HANDS_TEMP = 0x0200,
	FAN_SET_BUSY_HANDS = 0x0400,
	FAN_ALIGN_TO_ENV_POS = 0x0800,
	FAN_SET_ADDEFFECT = 0x1000,
	FAN_RANDOM = 0x2000,
	FAN_SET_LARA_PLACE = 0x4000,
};

extern void NGProcessAnimations();