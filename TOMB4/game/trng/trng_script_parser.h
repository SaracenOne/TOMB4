#pragma once

#include "trng_extra_state.h"

#define MAX_NG_LEVELS 64

enum CUST_ENUMS {
	CUST_DISABLE_SCREAMING_HEAD = 0x01,
	CUST_SET_SECRET_NUMBER = 0x02,
	CUST_SET_CREDITS_LEVEL = 0x03,
	CUST_DISABLE_FORCING_ANIM_96 = 0x04,
	CUST_ROLLINGBALL_PUSHING = 0x05,
	CUST_NEW_SOUND_ENGINE = 0x06,
	CUST_SPEED_MOVING = 0x07,
	CUST_SHATTER_RANGE = 0x08,
	CUST_WEAPON = 0x09,
	CUST_AMMO = 0x0a,
	CUST_SHOW_AMMO_COUNTER = 0x0b,
	CUST_SET_INV_ITEM = 0x0c,
	CUST_SET_JEEP_KEY_SLOT = 0x0d,
	CUST_STATIC_TRANSPARENCY = 0x0e,
	CUST_SET_STATIC_DAMAGE = 0x0f,
	CUST_LOOK_TRASPARENT = 0x10,
	CUST_HAIR_TYPE = 0x11,
	CUST_KEEP_DEAD_ENEMIES = 0x12,
	CUST_SET_OLD_CD_TRIGGER = 0x13,
	CUST_ESCAPE_FLY_CAMERA = 0x14,
	CUST_PAUSE_FLY_CAMERA = 0x15,
	CUST_TEXT_ON_FLY_SCREEN = 0x16,
	CUST_CD_SINGLE_PLAYBACK = 0x17,
	CUST_ADD_DEATH_ANIMATION = 0x18,
	CUST_BAR = 0x19,
	CUST_NO_TIME_IN_SAVELIST = 0x1a,
	CUST_PARALLEL_BARS = 0x1b,
	CUST_CAMERA = 0x1c,
	CUST_DISABLE_MISSING_SOUNDS = 0x1d,
	CUST_INNER_SCREENSHOT = 0x1e,
	CUST_FMV_CUTSCENE = 0x1f,
	CUST_FIX_WATER_FOG_BUG = 0x20,
	CUST_SAVE_LOCUST = 0x21,
	CUST_LIGHT_OBJECT = 0x22,
	CUST_HARPOON = 0x23,
	CUST_SCREENSHOT_CAPTURE = 0x24,
	CUST_RAIN = 0x25,
	CUST_TR5_UNDERWATER_COLLISIONS = 0x26,
	CUST_DARTS = 0x27,
	CUST_FLARE = 0x28,
	CUST_SET_TEXT_COLOR = 0x29,
	CUST_SET_STILL_COLLISION = 0x2a,
	CUST_WATERFALL_SPEED = 0x2b,
	CUST_ROLLING_BOAT = 0x2c,
	CUST_SFX = 0x2d,
	CUST_TITLE_FMV = 0x2e,
	CUST_KEEP_LARA_HP = 0x2f,
	CUST_BINOCULARS = 0x30,
	CUST_BACKGROUND = 0x31,
	CUST_DISABLE_PUSH_AWAY_ANIMATION = 0x32,
	CUST_SLOT_FLAGS = 0x34,
	CUST_FIX_BUGS = 0x35,
	CUST_SHATTER_SPECIFIC = 0x36
};

enum BUGF_ENUMS {
	BUGF_TRANSPARENT_WHITE_ON_FOG = 0x01,
	BUGF_DART_NO_POISON_LARA = 0x02,
	BUGF_LAND_WATER_SFX_ENEMIES = 0x04
};

enum TGROUP_FLAGS {
	TGROUP_USE_FOUND_ITEM_INDEX = 0x01,
	TGROUP_USE_TRIGGER_ITEM_INDEX = 0x02,
	TGROUP_COMMAND = 0x03,
	TGROUP_USE_OWNER_ANIM_ITEM_INDEX = 0x04,
	TGROUP_SINGLE_SHOT_RESUMED = 0x08, // Might also be TGROUP_AND in early versions
	TGROUP_OR = 0x10,
	TGROUP_NOT = 0x20,
	TGROUP_ELSE = 0x40,
	TGROUP_USE_EXECUTOR_ITEM_INDEX = 0x100,
	TGROUP_SINGLE_SHOT = 0x400,
	TGROUP_USE_ITEM_USED_BY_LARA_INDEX = 0x800,
};

struct NG_GLOBAL_TRIGGER {
	unsigned short flags = 0x0;
	unsigned short type = 0x00;
	int parameter = 0x00;
	unsigned short condition_trigger_group = 0x00;
	unsigned short perform_trigger_group = 0x00;
	unsigned short on_false_trigger_group = 0x00;
};

struct NG_GLOBAL_TRIGGER_RECORD {
	unsigned short record_id = 0;
	NG_GLOBAL_TRIGGER global_trigger;
};

struct NG_TRIGGER_GROUP_DATA {
	unsigned short first_field = 0x00;
	unsigned short plugin_id = 0;

	unsigned short second_field_lower = 0x00;
	unsigned short second_field_upper = 0x00;
	
	unsigned short third_field_lower = 0x00;
	unsigned short third_field_upper = 0x00;
};

#define NG_TRIGGER_GROUP_DATA_SIZE 0xff
struct NG_TRIGGER_GROUP {
	NG_TRIGGER_GROUP_DATA data[NG_TRIGGER_GROUP_DATA_SIZE];
	bool oneshot_triggered = false;
};

struct NG_TRIGGER_GROUP_RECORD {
	unsigned short record_id = 0;
	NG_TRIGGER_GROUP trigger_group;
};

struct NG_ORGANIZER_APPOINTMENT {
	unsigned int time = 0;
	unsigned short trigger_group = 0;
};

#define NG_ORGANIZER_MAX_APPOINTMENTS 4096
struct NG_ORGANIZER {
	short flags = 0;
	short parameters = 0; // Unused
	unsigned int appointment_count = 0;
	NG_ORGANIZER_APPOINTMENT appointments[NG_ORGANIZER_MAX_APPOINTMENTS];
};

struct NG_ORGANIZER_RECORD {
	unsigned short record_id = 0;
	NG_ORGANIZER organizer;
};

#define NG_ITEM_GROUP_MAX_LIST 4096
struct NG_ITEM_GROUP {
	short item_list[NG_ITEM_GROUP_MAX_LIST];
};

struct NG_ITEM_GROUP_RECORD {
	unsigned short record_id = 0;
	NG_ITEM_GROUP item_group;
};

struct NG_LEVEL_RECORD_DATA {
	int global_trigger_count = 0;
	NG_GLOBAL_TRIGGER_RECORD* global_triggers_table = NULL;
	int trigger_group_count = 0;
	NG_TRIGGER_GROUP_RECORD* trigger_group_table = NULL;
	int organizer_count = 0;
	NG_ORGANIZER_RECORD* organizer_table = NULL;
	int item_group_count = 0;
	NG_ITEM_GROUP_RECORD* item_group_table = NULL;
};

struct NG_MOVE_ITEM {
	unsigned short flags;
	unsigned short index_item;
	unsigned short direction;
	unsigned short distance;
	unsigned short speed;
	short moving_sound;
	short final_sound;
};

struct NG_MOVE_ITEM_RECORD {
	unsigned short record_id = 0;
	NG_MOVE_ITEM move_item;
};

struct NG_BIG_NUMBER_RECORD {
	unsigned short record_id = 0;
	unsigned short big_number;
};

struct NG_LEVEL_PARAMS {
	int big_number_count = 0;
	NG_GLOBAL_TRIGGER_RECORD *big_number_table = NULL;
};

struct NG_LEVEL {
	bool new_audio_system = false;
	bool old_cd_trigger_system = true;
	NG_LEVEL_RECORD_DATA *records = NULL;
	NG_LEVEL_PARAMS* params = NULL;
};

extern NG_LEVEL ng_levels[MAX_NG_LEVELS];

enum NG_ADD_EFFECT_TYPE {
	NG_ADD_NOTHING,
	NG_ADD_FLAME,
	NG_ADD_SMOKE,
	NG_ADD_BLOOD,
	NG_ADD_MIST,
	NG_ADD_LIGHT_FLAT,
	NG_ADD_LIGHT_BLINK,
	NG_ADD_LIGHT_SPOT,
	NG_ADD_LIGHT_GLOVE,
};

extern NG_GLOBAL_TRIGGER current_global_triggers[MAX_NG_GLOBAL_TRIGGERS];
extern NG_TRIGGER_GROUP current_trigger_groups[MAX_NG_TRIGGER_GROUPS];
extern NG_ORGANIZER current_organizers[MAX_NG_ORGANIZERS];
extern NG_ITEM_GROUP current_item_groups[MAX_NG_ITEM_GROUPS];

#define MAX_NG_MOVE_ITEM_PARAMS 9999
#define MAX_NG_BIG_NUMBER_PARAMS 9999

// Params
extern NG_MOVE_ITEM current_move_item_params[MAX_NG_MOVE_ITEM_PARAMS];
extern unsigned short current_big_numbers_params[MAX_NG_BIG_NUMBER_PARAMS];

extern void NGInitLevelArray();
extern void NGLoadTablesForLevel(unsigned int level);
extern void NGScriptCleanup();
extern void NGReadNGGameflowInfo(char* gfScriptFile, unsigned int offset, unsigned int len);