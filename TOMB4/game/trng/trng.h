#pragma once

#define SILENCE_EXCESSIVE_LOGS // Debug macro to silence log commands which are commonly called every frame.

#define NG_DEGREE(i) (i * 182)

enum NG_DEGREES {
	DEGREES_45 = 0,
	DEGREES_90,
	DEGREES_135,
	DEGREES_180,
	DEGREES_225,
	DEGREES_270,
	DEGREES_315,
	DEGREES_360
};

enum NG_DIRECTIONS {
	NG_NORTH = 0,
	NG_EAST,
	NG_SOUTH,
	NG_WEST,
	NG_UP,
	NG_DOWN
};

extern bool is_ngle_level;

extern int ng_floor_id_size;
extern char *ng_floor_id_table;

#define NG_SCRIPT_ID_TABLE_SIZE 8192
extern short ng_script_id_table[NG_SCRIPT_ID_TABLE_SIZE];

#define NG_ROOM_REMAP_TABLE_SIZE 2048
extern short ng_room_remap_table[NG_ROOM_REMAP_TABLE_SIZE];

#define NG_STATIC_ID_TABLE_SIZE 8192
struct NGStaticTableEntry {
	short remapped_room_index;
	short mesh_id;
};
extern NGStaticTableEntry ng_static_id_table[NG_STATIC_ID_TABLE_SIZE];

extern void NGLoadInfo(FILE* level_fp);

extern void NGStorePendingRoomNumber(int room_number);
extern int NGRestorePendingRoomNumber();
extern void NGUpdateCurrentTriggerRoomAndIndex(int new_room, int new_index);
extern void NGClearCurrentTriggerRoomAndIndex();
extern void NGStoreBackupTriggerRoomAndIndex();
extern void NGRestoreBackupTriggerRoomAndIndex();

extern int NGGetPluginIDForFloorData(short* floor_data_ptr);

// Move the item in a direction by the number of units
extern void NGMoveItemByUnits(unsigned short item_id, NG_DIRECTIONS direction, unsigned int units);
extern void NGMoveItemHorizontalByUnits(unsigned short item_id, short angle, unsigned int units);
extern void NGMoveItemVerticalByUnits(unsigned short item_id, unsigned int units);

extern void NGRotateItemX(unsigned short item_id, short rotation);
extern void NGRotateItemY(unsigned short item_id, short rotation);

extern void NGSetup();

extern void NGUpdateAllItems();
extern void NGFrameStartUpdate();
extern void NGDrawPhase();

extern bool NGIsItemFrozen(unsigned int item_num);

extern void NGFrameFinish();

extern bool NGUseNGConditionals();
extern bool NGUseNGFlipEffects();
extern bool NGUseNGActions();
extern bool NGUseNGAnimCommands();

extern void NGSetCurrentDrawItemNumber(int item_num);
extern int NGGetCurrentDrawItemNumber();

extern void NGUpdateFlipeffectFloorstateData(bool heavy);
extern void NGUpdateActionFloorstateData(bool heavy);
extern void NGUpdateOneshot();

extern int NGFindIndexForLaraStartPosWithMatchingOCB(unsigned int ocb);

extern bool NGLaraHasInfiniteAir();

extern bool NGTestUseInventoryObjectAndManagementReplaced(short object_id);

extern void NGInit();
extern void NGCleanup();

enum NGLogType {
	NG_LOG_TYPE_PRINT,
	NG_LOG_TYPE_UNIMPLEMENTED_FEATURE,
	NG_LOG_TYPE_ERROR,
};

extern void NGLog(NGLogType type, const char* s, ...);