#pragma once

#include "../../global/types.h"
#include "../../tomb4/mod_config.h"

#define NG_READ_8(scr_buffer, scr_offset) scr_buffer[scr_offset]; \
offset += sizeof(char)

#define NG_READ_16(scr_buffer, scr_offset) (unsigned short)((unsigned char)scr_buffer[scr_offset]) | ((unsigned short)(scr_buffer[scr_offset + 1])) << 8; \
scr_offset += sizeof(short)

#define NG_READ_32(scr_buffer, scr_offset) (unsigned int)(((unsigned char)scr_buffer[scr_offset]) | (((unsigned int)(unsigned char)scr_buffer[scr_offset + 1]) << 8) | (((unsigned int)(unsigned char)scr_buffer[scr_offset + 2]) << 16) | (((unsigned int)(unsigned char)scr_buffer[scr_offset + 3]) << 24)); \
scr_offset += sizeof(int)

#define SILENCE_EXCESSIVE_LOGS // Debug macro to silence log commands which are commonly called every frame.

#define NG_DEGREE(i) (i * 182)

enum NG_DEGREES_CODE {
	DEGREES_CODE_45 = 0,
	DEGREES_CODE_90,
	DEGREES_CODE_135,
	DEGREES_CODE_180,
	DEGREES_CODE_225,
	DEGREES_CODE_270,
	DEGREES_CODE_315,
	DEGREES_CODE_360
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

extern int ng_floor_id_size;
extern char *ng_floor_id_table;

extern int ng_script_id_count;
extern int ng_room_remap_count;
extern int ng_static_id_count;

#define NG_SCRIPT_ID_TABLE_SIZE 8192
struct NGScriptIDTableEntry {
	short script_index;
};
extern NGScriptIDTableEntry ng_script_id_table[NG_SCRIPT_ID_TABLE_SIZE];

#define NG_ROOM_REMAP_TABLE_SIZE 2048
struct NGRoomRemapTableEntry {
	short room_index;
};
extern NGRoomRemapTableEntry ng_room_remap_table[NG_ROOM_REMAP_TABLE_SIZE];

#define NG_STATIC_ID_TABLE_SIZE 8192
struct NGStaticTableEntry {
	short remapped_room_index;
	short mesh_id;
};
extern NGStaticTableEntry ng_static_id_table[NG_STATIC_ID_TABLE_SIZE];

extern void NGPreloadAllLevelInfo();
extern void NGLoadLevelInfo(FILE* level_fp);

extern void NGStorePendingRoomNumber(int room_number);
extern int NGRestorePendingRoomNumber();
extern void NGUpdateCurrentTriggerRoomAndIndex(int new_room, int new_index);
extern void NGClearCurrentTriggerRoomAndIndex();
extern void NGStoreBackupTriggerRoomAndIndex();
extern void NGRestoreBackupTriggerRoomAndIndex();

extern int NGGetPluginIDForFloorData(short* floor_data_ptr);

// Move the item in a direction by the number of units
extern void NGMoveItemByUnits(unsigned short item_id, NG_DIRECTIONS direction, int units);
extern void NGMoveItemHorizontalByUnits(unsigned short item_id, short angle, int units);
extern void NGMoveItemVerticalByUnits(unsigned short item_id, int units);

extern void NGRotateItemX(unsigned short item_id, short rotation);
extern void NGRotateItemY(unsigned short item_id, short rotation);

// Statics

// Move the item in an angle by the number of units
extern void NGStaticItemByUnits(unsigned short static_id, NG_DIRECTIONS direction, int units);
extern void NGMoveStaticHorizontalByUnits(unsigned short static_id, short angle, int units);
extern void NGMoveStaticVerticalByUnits(unsigned short static_id, int units);

extern void NGRotateStaticX(unsigned short static_id, short rotation);
extern void NGRotateStaticY(unsigned short static_id, short rotation);

extern int NGFloat2Int(float x);
extern bool NGIsSourcePositionNearTargetPos(PHD_3DPOS *source_pos, PHD_3DPOS *target_pos, int32_t distance, bool ignore_y);
extern bool NGIsSourcePositionLessThanDistanceToTargetPosition(PHD_3DPOS *source_pos, PHD_3DPOS *target_pos, int32_t distance, bool ignore_y);

extern void NGSetItemAnimation(uint16_t item_id, uint32_t animation, bool update_state_id, bool update_next_state_id, bool update_speed, bool force_reset);

extern void NGSetup();

extern void NGUpdateAllItems();
extern void NGUpdateAllStatics();
extern void NGDrawPhase();

extern bool NGIsItemFrozen(unsigned int item_num);

extern void NGFrameStart();
extern void NGFrameFinish();

extern bool NGIsUsingNGConditionals();
extern bool NGIsUsingNGFlipEffects();
extern bool NGIsUsingNGActions();
extern bool NGIsUsingNGAnimCommands();

extern void NGSetCurrentDrawItemNumber(int item_num);
extern int NGGetCurrentDrawItemNumber();

extern void NGUpdateFlipeffectFloorstateData(bool heavy);
extern void NGUpdateActionFloorstateData(bool heavy);

extern void NGUpdateFlipeffectOneshot();
extern void NGUpdateActionOneshot();

extern int NGFindIndexForLaraStartPosWithMatchingOCB(unsigned int ocb);

extern bool NGLaraHasInfiniteAir();

extern bool NGTestSelectedInventoryObjectAndManagementReplaced(int inventory_object_id);
extern void NGSetUsedInventoryObject(int inventory_object_id);
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

extern void NGLog(NGLogType type, const char* s, ...);