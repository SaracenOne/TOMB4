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

enum NG_FLIPEFFECTS {
	DISABLE_INPUT_FOR_TIME = 51,
	ENABLE_INPUT = 52,
	PLAY_CD_TRACK_ON_CHANNEL_1 = 68,
	REMOVE_WEAPONS_OR_FLARES_FROM_LARAS_HANDS = 83,
	FORCE_LARA_ANIMATION_0_255_OF_SLOT_ANIMATION = 77,
	FORCE_LARA_ANIMATION_256_512_OF_SLOT_ANIMATION = 80,
	DISARM_LARA = 96,
	PLAY_CD_TRACK_ON_CHANNEL_2 = 129,
	ATTRACT_LARA_IN_DIRECTION_ON_GROUND = 134,
	SET_VOLUME_OF_AUDIO_TRACK_ON_CHANNEL = 133,
	ATTRACT_LARA_IN_DIRECTION_IN_AIR = 153,
	ATTRACT_LARA_IN_DIRECTION_ON_GROUND_AND_IN_AIR = 158,
	CAMERA_SHOW_BLACK_SCREEN_FOR_SECONDS_WITH_FINAL_CURTAIN_EFFECT = 367,
	CAMERA_SET_CINEMA_EFFECT_TYPE_FOR_SECONDS = 369,
	SET_LARA_HOLSTER_TYPE = 407
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

extern void NGActionTrigger(unsigned short param, unsigned short extra);
extern void NGSetup();

extern void NGItemUpdate(unsigned int item_num);
extern void NGFrameStartUpdate();

extern bool NGIsItemFrozen(unsigned int item_num);
extern void NGFlipEffect(unsigned short param, short extra, bool oneshot);

extern void NGFrameFinish();

extern void NGLoaderHeader(char* gfScriptFile, unsigned int offset, unsigned int len);

extern bool NGUseNGFlipEffects();
extern bool NGUseNGActions();

extern void NGUpdateFloorstateData(bool update_oneshot);