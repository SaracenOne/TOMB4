#pragma once

#include "../sound.h"

enum NGTimerTrackerType {
	TTT_ONLY_SHOW_SECONDS = 0,
	TTT_SECONDS_AND_ONE_DECIMAL_POINT_SEPERATOR,
	TTT_SECONDS_AND_TWO_DECIMAL_POINT_SEPERATOR,
	TTT_SECONDS_AND_ONE_DECIMAL_COLON_SEPERATOR,
	TTT_SECONDS_AND_TWO_DECIMAL_COLON_SEPERATOR,
	TTT_SECONDS_WITH_THREE_NOUGHTS
};

#define MAX_NG_GLOBAL_TRIGGERS 9999
#define MAX_NG_TRIGGER_GROUPS 9999
#define MAX_NG_ORGANIZERS 4999
#define MAX_NG_ITEM_GROUPS 999

struct NG_GLOBAL_TRIGGER_STATE {
	bool is_disabled = false;
	bool is_halted = false;
};

struct NG_TRIGGER_GROUP_STATE {
	bool continuous = false;
	bool one_shot = false;
};

struct NG_ORGANIZER_STATE {
	bool is_enabled = false;
	int current_tick = 0;
};

extern NG_GLOBAL_TRIGGER_STATE ng_global_trigger_states[MAX_NG_GLOBAL_TRIGGERS];
extern NG_TRIGGER_GROUP_STATE ng_trigger_group_states[MAX_NG_TRIGGER_GROUPS];
extern NG_ORGANIZER_STATE ng_organizer_states[MAX_NG_ORGANIZERS];

extern int ng_found_item_index;;

extern int ng_looped_sound_state[NumSamples];

extern int current_value;
extern int global_alfa;
extern int global_beta;
extern int global_delta;
extern int global_timer;
extern int local_alfa;
extern int local_beta;
extern int local_delta;
extern int local_timer;
extern int last_input_number;

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

extern short NGGetItemNorthSouthUnits(unsigned int item_num);
extern void NGSetItemNorthSouthUnits(unsigned int item_num, short units);
extern short NGGetItemEastWestUnits(unsigned int item_num);
extern void NGSetItemEastWestUnits(unsigned int item_num, short units);
extern short NGGetItemUpDownUnits(unsigned int item_num);
extern void NGSetItemUpDownUnits(unsigned int item_num, short units);

extern void NGSetItemMovementSpeed(unsigned int item_num, unsigned int movement_speed);
extern void NGSetItemMovementInProgressSound(unsigned int item_num, int sound_effect_id);
extern void NGSetItemMovementFinishedSound(unsigned int item_num, int sound_effect_id);

extern void NGAddLaraCollision(int item_number);
extern void NGClearLaraCollisions();
extern int NGIsLaraCollidingWithItem(int item_number);
extern int NGIsLaraCollidingWithSlot(int slot_number);
extern int NGIsLaraCollidingWithCreature();

extern bool NGIsItemCollisionDisabled(unsigned int item_num);
extern void NGDisableItemCollision(unsigned int item_num);
extern void NGEnableItemCollision(unsigned int item_num);

extern void NGSetCurtainTimer(int ticks);
extern void NGSetCinemaTypeAndTimer(int type, int ticks);

extern bool NGIsTriggerGroupContinuous(int trigger_group_id);
extern void NGSetTriggerGroupContinuous(int trigger_group_id, bool is_continuous);

extern void NGToggleOrganizer(int organizer_id, bool is_enabled);

extern void NGSetDisplayTimerForMoveableWithType(int item_id, NGTimerTrackerType timer_type);

extern void NGDrawPhase();

extern bool NGIsOneShotTriggeredForTile();
extern bool NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(bool heavy);

extern bool NGCheckActionFloorStatePressedThisFrameOrLastFrame(bool heavy);