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

enum NG_ACTION_TYPE {
	TURN_ANIMATING_CLOCKWISE_BY_DEGREES = 1,
	PERFORM_FLIPEFFECT_ON_ITEM = 13,
	FORCE_ANIMATION_0_TO_31_ON_ITEM = 15,
	FORCE_ANIMATION_32_TO_63_ON_ITEM = 16,
	FORCE_ANIMATION_64_TO_95_ON_ITEM = 17,
	OPEN_OR_CLOSE_DOOR_ITEM = 26,
	HURT_ENEMY = 38,
	FREEZE_ENEMY_FOR_SECONDS = 58,
	UNFREEZE_ENEMY_WITH_EFFECT = 59,
	MOVE_ITEM_UP_BY_UNITS_X8 = 63,
	MOVE_ITEM_DOWN_BY_UNITS_X8 = 64,
	MOVE_ITEM_WEST_BY_UNITS_X8 = 65,
	MOVE_ITEM_NORTH_BY_UNITS_X8 = 66,
	MOVE_ITEM_EAST_BY_UNITS_X8 = 67,
	MOVE_ITEM_SOUTH_BY_UNITS_X8 = 68,
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

extern void NGStorePendingRoomNumber(int room_number);
extern int NGRestorePendingRoomNumber();
extern void NGUpdateCurrentTriggerRoomAndIndex(int new_room, int new_index);
extern void NGClearCurrentTriggerRoomAndIndex();
extern void NGStoreBackupTriggerRoomAndIndex();
extern void NGRestoreBackupTriggerRoomAndIndex();

extern int NGActionTrigger(unsigned short param, unsigned short extra, bool skip_checks);
extern void NGSetup();

extern void NGItemUpdate(unsigned int item_num);
extern void NGFrameStartUpdate();

extern bool NGIsItemFrozen(unsigned int item_num);

extern void NGFrameFinish();

extern void NGLoaderHeader(char* gfScriptFile, unsigned int offset, unsigned int len);

extern bool NGUseNGFlipEffects();
extern bool NGUseNGActions();

extern void NGUpdateFloorstateData(bool update_oneshot);

extern void NGInit();
extern void NGCleanup();