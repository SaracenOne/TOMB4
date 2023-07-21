struct NG_ITEM_EXTRADATA {
	short frozen_ticks = 0;
};

#define NG_INPUT_LOCK_TIMER_COUNT 0x14
extern int ng_input_lock_timers[NG_INPUT_LOCK_TIMER_COUNT];

extern void NGStorePendingRoomNumber(int room_number);
extern int NGRestorePendingRoomNumber();
extern void NGUpdateCurrentTriggerRoomAndIndex(int new_room, int new_index);
extern void NGClearCurrentTriggerRoomAndIndex();
extern void NGStoreBackupTriggerRoomAndIndex();
extern void NGRestoreBackupTriggerRoomAndIndex();

extern void NGSetupExtraState();
extern void NGFrameFinishExtraState();
extern bool NGIsItemFrozen(unsigned int item_num);
extern void NGSetItemFreezeTimer(unsigned int item_num, int ticks);

extern bool NGIsOneShotTriggeredForTile();
extern bool NGCheckFloorStatePressedThisFrameOrLastFrame();
extern bool NGCheckFloorStatePressedThisFrameOrLastFrame();