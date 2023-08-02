enum NGTimerTrackerType {
	TTT_ONLY_SHOW_SECONDS = 0,
	TTT_SECONDS_AND_ONE_DECIMAL_POINT_SEPERATOR,
	TTT_SECONDS_AND_TWO_DECIMAL_POINT_SEPERATOR,
	TTT_SECONDS_AND_ONE_DECIMAL_COLON_SEPERATOR,
	TTT_SECONDS_AND_TWO_DECIMAL_COLON_SEPERATOR,
	TTT_SECONDS_WITH_THREE_NOUGHTS,
};

extern void NGStorePendingRoomNumber(int room_number);
extern int NGRestorePendingRoomNumber();
extern void NGUpdateCurrentTriggerRoomAndIndex(int new_room, int new_index);
extern void NGClearCurrentTriggerRoomAndIndex();
extern void NGStoreBackupTriggerRoomAndIndex();
extern void NGRestoreBackupTriggerRoomAndIndex();

extern void NGSetupExtraState();
extern void NGFrameFinishExtraState();

extern int NGValidateInputAgainstLockTimers(int input);
extern bool NGValidateInputSavegame();
extern bool NGValidateInputLoadgame();
extern bool NGValidateInputWeaponHotkeys();

extern void NGDisableInputForTime(unsigned char input, int ticks);
extern void NGEnableInput(unsigned char input);

extern bool NGIsItemFrozen(unsigned int item_num);
extern void NGSetItemFreezeTimer(unsigned int item_num, int ticks);
extern short NGGetAutoRotationPerFrame(unsigned int item_num);
extern void NGSetAutoRotationPerFrame(unsigned int item_num, short degress_per_frame);

extern short NGGetItemNorthSouthTimer(unsigned int item_num);
extern void NGSetItemNorthSouthTimer(unsigned int item_num, short timer);
extern short NGGetItemEastWestTimer(unsigned int item_num);
extern void NGSetItemEastWestTimer(unsigned int item_num, short timer);
extern short NGGetItemUpDownTimer(unsigned int item_num);
extern void NGSetItemUpDownTimer(unsigned int item_num, short timer);

extern bool NGIsItemCollisionDisabled(unsigned int item_num);
extern void NGDisableItemCollision(unsigned int item_num);
extern void NGEnableItemCollision(unsigned int item_num);

extern void NGSetCurtainTimer(int ticks);
extern void NGSetCinemaTypeAndTimer(int type, int ticks);

extern void NGSetDisplayTimerForMoveableWithType(int item_id, NGTimerTrackerType timer_type);

extern void NGDrawPhase();

extern bool NGIsOneShotTriggeredForTile();
extern bool NGCheckFloorStatePressedThisFrameOrLastFrame();
extern bool NGCheckFloorStatePressedThisFrameOrLastFrame();