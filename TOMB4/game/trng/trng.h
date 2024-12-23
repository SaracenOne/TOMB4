#pragma once

#include "../../global/types.h"
#include "../../tomb4/mod_config.h"

#define NGLE_INDEX 0x4000
#define MASK_NGLE_INDEX  0x3FFF

#define NGLE_START_SIGNATURE 0x474e
#define NGLE_END_SIGNATURE 0x454c474e

#define NG_TICKS_PER_SECOND 30

#define NG_READ_8(src_buffer, src_offset) src_buffer[src_offset]; \
offset += sizeof(int8_t)

#define NG_READ_16(src_buffer, src_offset) (uint16_t)((uint8_t)src_buffer[src_offset]) | ((uint16_t)(src_buffer[src_offset + 1])) << 8; \
src_offset += sizeof(int16_t)

#define NG_READ_32(src_buffer, src_offset) (uint32_t)(((uint8_t)src_buffer[src_offset]) | (((uint32_t)(uint8_t)src_buffer[src_offset + 1]) << 8) | (((uint32_t)(uint8_t)src_buffer[src_offset + 2]) << 16) | (((uint32_t)(uint8_t)src_buffer[src_offset + 3]) << 24)); \
src_offset += sizeof(int32_t)

#define NG_READ_FLOAT(src_buffer, src_offset) (*(float*)&src_buffer[src_offset]); \
src_offset += sizeof(float)

#define NG_READ_FIXED_STRING(src_buffer, dst_buffer, buffer_size, src_offset) memcpy(dst_buffer, src_buffer + src_offset, buffer_size); src_offset += buffer_size

#define SILENCE_EXCESSIVE_LOGS // Debug macro to silence log commands which are commonly called every frame.

#define NG_DEGREE(i) (DEGREES_TO_ROTATION(i))

#define SCANF_HEAVY				0x100
#define SCANF_TEMP_ONE_SHOT		0x200 
#define SCANF_BUTTON_ONE_SHOT	0x400 
#define SCANF_YET_TO_PERFORM    0x800
#define SCANF_SCRIPT_TRIGGER	0x1000
#define SCANF_DIRECT_CALL		0x2000
#define SCANF_FLOOR_DATA		0x4000
#define SCANF_ANIM_COMMAND		0x8000

struct NGOldTrigger {
	uint16_t flags;
	uint32_t offset_floor_data;
};

enum NG_DIRECTIONS {
	NG_NORTH = 0,
	NG_EAST,
	NG_SOUTH,
	NG_WEST,
	NG_UP,
	NG_DOWN
};

struct NGLevelInfo {
	bool ngle_footer_found = false;
	bool is_ngle_level = false;
	bool is_using_ngle_triggers = false;
	bool is_using_global_sound_map = false;
};

extern NGLevelInfo ng_level_info[MOD_LEVEL_COUNT];

extern int32_t ng_floor_id_size;
extern int8_t *ng_floor_id_table;

extern int32_t ng_script_id_count;
extern int32_t ng_room_remap_count;
extern int32_t ng_static_id_count;

struct NGAnimatedTexture {
	bool test;
	uint8_t total_uv_rotations;
	uint16_t total_range_ng;
	uint16_t total_info_range_animation[40];
	uint16_t from_tex[40];
	uint16_t to_tex[40];
	uint16_t size_default;
};

extern NGAnimatedTexture ng_animated_texture;

#define NG_SCRIPT_ID_TABLE_SIZE 8192
struct NGScriptIDTableEntry {
	int16_t script_index;
};
extern NGScriptIDTableEntry ng_script_id_table[NG_SCRIPT_ID_TABLE_SIZE];

#define NG_ROOM_REMAP_TABLE_SIZE 2048
struct NGRoomRemapTableEntry {
	int16_t room_index;
};
extern NGRoomRemapTableEntry ng_room_remap_table[NG_ROOM_REMAP_TABLE_SIZE];

#define NG_STATIC_ID_TABLE_SIZE 8192
struct NGStaticTableEntry {
	int16_t remapped_room_index;
	int16_t mesh_id;
};
extern NGStaticTableEntry ng_static_id_table[NG_STATIC_ID_TABLE_SIZE];

extern void NGPreloadAllLevelInfo(uint32_t valid_level_count);
extern void NGLoadLevelInfo(FILE* level_fp);

extern int32_t NGGetPluginIDForFloorData(int16_t *floor_data_ptr);


// Move the item in a direction by the number of units
extern void NGMoveItemByUnits(uint16_t item_id, NG_DIRECTIONS direction, int32_t units);
extern void NGMoveItemHorizontalByUnits(uint16_t item_id, int16_t angle, int32_t units);
extern void NGMoveItemVerticalByUnits(uint16_t item_id, int32_t units);

extern void NGRotateItemX(uint16_t item_id, int16_t rotation);
extern void NGRotateItemY(uint16_t item_id, int16_t rotation);

// Statics

// Move the item in an angle by the number of units
extern void NGStaticItemByUnits(uint16_t static_id, NG_DIRECTIONS direction, int32_t units);
extern void NGMoveStaticHorizontalByUnits(uint16_t static_id, int16_t angle, int32_t units);
extern void NGMoveStaticVerticalByUnits(uint16_t static_id, int32_t units);

extern GAME_VECTOR NGGetGameVectorForStatic(uint16_t static_id);

extern void NGRotateStaticX(uint16_t static_id, int16_t rotation);
extern void NGRotateStaticY(uint16_t static_id, int16_t rotation);

extern int32_t NGFloat2Int(float x);
extern bool NGIsSourcePositionNearTargetPos(PHD_3DPOS *source_pos, PHD_3DPOS *target_pos, int32_t distance, bool ignore_y);
extern bool NGIsSourcePositionLessThanDistanceToTargetPosition(PHD_3DPOS *source_pos, PHD_3DPOS *target_pos, int32_t distance, bool ignore_y);

extern void NGSetItemAnimation(uint16_t item_id, uint32_t animation, bool update_state_id, bool update_next_state_id, bool update_speed, bool force_reset);

extern void NGLevelSetup();

extern void NGUpdateAllItems();
extern void NGUpdateAllStatics();
extern void NGDrawPhase();

extern bool NGIsItemFrozen(uint32_t item_num);

extern void NGFrameStart();
extern void NGPreFrameFinish();
extern void NGFrameFinish();

extern bool NGIsUsingNGNewTriggers();
extern bool NGIsUsingNGAnimCommands();
extern bool NGIsUsingNGTimerfields();

extern void NGSetCurrentDrawItemNumber(int32_t item_num);
extern int32_t NGGetCurrentDrawItemNumber();

extern int32_t NGFindIndexForLaraStartPosWithMatchingOCB(uint32_t ocb);

extern bool NGLaraHasInfiniteAir();

extern bool NGTestSelectedInventoryObjectAndManagementReplaced(int32_t inventory_object_id);
extern void NGSetUsedInventoryObject(int32_t inventory_object_id);
extern void NGSetUsedSmallMedipack();
extern void NGSetUsedLargeMedipack();

extern void NGInit();
extern void NGCleanup();

enum NGLogType {
	NG_LOG_TYPE_PRINT,
	NG_LOG_TYPE_POSSIBLE_INACCURACY,
	NG_LOG_TYPE_UNIMPLEMENTED_FEATURE,
	NG_LOG_TYPE_ERROR,
};

extern void NGLog(NGLogType type, const char *s, ...);

extern void NGStoreLastFloorAddress(int16_t *p_floor_last_address);
extern int16_t *NGGetLastFloorAddress();

extern void NGStoreFloorTriggerNow(int16_t *p_trigger_now);
extern int16_t *NGGetFloorTriggerNow();

extern void NGStoreIsHeavyTesting(bool p_is_heavy_testing);
extern bool NGGetIsHeavyTesting();

extern void NGStoreLastItemMovedIndex(int16_t item_num);
extern int16_t NGGetLastMovedItemIndex();

extern void NGStoreItemIndexEnabledTrigger(int16_t item_num);
extern int16_t NGGetItemIndexEnabledTrigger();

extern void NGStoreItemIndexCurrent(int16_t item_num);
extern int16_t NGGetItemIndexCurrent();

extern void NGStoreItemIndexConditional(int16_t index);
int16_t NGGetItemIndexConditional();

extern void NGStoreInsideConditionCount(int32_t count);
extern int32_t NGGetInsideConditionCount();

extern void NGStoreIsInsideDummyTrigger(bool is_dummy);
extern bool NGGetIsInsideDummyTrigger();

extern void NGStoreLastTriggerTimer(int32_t timer);
extern int32_t NGGetLastTriggerTimer();

extern void NGStoreTestConditionsFound(bool found);
extern bool NGGetTestConditionsFound();

extern void NGStoreSaveTriggerButtons(uint32_t trigger_buttons);
extern uint32_t NGGetSaveTriggerButtons();

extern void NGStoreTestDummyFailed(bool failed);
extern bool NGGetTestDummyFailed();

extern int32_t stored_condition_count;
extern bool stored_is_inside_dummy_trigger;
extern int16_t *stored_last_floor_address;
extern int16_t *stored_base_floor_trigger_now;
extern bool stored_is_heavy_testing;
extern int16_t stored_last_item_index;
extern int16_t stored_item_index_enabled_trigger;
extern int16_t stored_item_index_current;
extern int16_t stored_item_index_conditional;
extern int32_t stored_last_trigger_timer;
extern bool stored_test_conditions_found;
extern uint32_t stored_save_trigger_buttons;
extern bool stored_test_dummy_failed;

extern int32_t NGCalculateTriggerTimer(int16_t *data, int32_t timer);
extern bool NGUsingLegacyNGTriggerBehaviour();

extern void NGAddItemMoved(int32_t item_id);