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

enum NG_DIRECTIONS
{
	NG_NORTH,
	NG_EAST,
	NG_SOUTH,
	NG_WEST,
	NG_UP,
	NG_DOWN
};

extern bool is_ngle_level;

#define NG_SCRIPT_ID_TABLE_SIZE 8192
extern short ng_script_id_table[NG_SCRIPT_ID_TABLE_SIZE];

extern void NGLoadInfo(FILE* level_fp);

extern void NGStorePendingRoomNumber(int room_number);
extern int NGRestorePendingRoomNumber();
extern void NGUpdateCurrentTriggerRoomAndIndex(int new_room, int new_index);
extern void NGClearCurrentTriggerRoomAndIndex();
extern void NGStoreBackupTriggerRoomAndIndex();
extern void NGRestoreBackupTriggerRoomAndIndex();

// Move the item in a direction by the number of units
extern void NGMoveItemByUnits(unsigned short item_id, NG_DIRECTIONS direction, unsigned int units);

extern int NGAction(unsigned short param, unsigned short extra, bool first_frame);
extern int NGActionTrigger(unsigned short param, unsigned short extra, unsigned short timer);
extern void NGSetup();

extern void NGUpdateAllItems();
extern void NGFrameStartUpdate();
extern void NGDrawPhase();

extern bool NGIsItemFrozen(unsigned int item_num);

extern void NGFrameFinish();

extern bool NGUseNGConditionals();
extern bool NGUseNGFlipEffects();
extern bool NGUseNGActions();

extern void NGUpdateFloorstateData(bool update_oneshot);

extern void NGInit();
extern void NGCleanup();