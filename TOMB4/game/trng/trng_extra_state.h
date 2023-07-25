struct NG_ITEM_EXTRADATA {
	short frozen_ticks = 0;
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
extern void NGSetCurtainTimer(int ticks);
extern void NGSetCinemaTypeAndTimer(int type, int ticks);

extern bool NGIsOneShotTriggeredForTile();
extern bool NGCheckFloorStatePressedThisFrameOrLastFrame();
extern bool NGCheckFloorStatePressedThisFrameOrLastFrame();