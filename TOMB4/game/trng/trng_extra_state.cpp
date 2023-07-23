#include "../../tomb4/pch.h"

#include "../../specific/function_stubs.h"
#include "../control.h"
#include "trng.h"
#include "trng_extra_state.h"

unsigned int ng_room_offset_table[0xff];

NG_ITEM_EXTRADATA *ng_items_extradata = NULL;

int current_ng_trigger_index = -1;
int current_ng_trigger_room = -1;
int backup_ng_trigger_index = -1;
int backup_ng_trigger_room = -1;
int pending_ng_room = -1;

int floorstate_data_size = 0;
char* ng_oneshot_floorstate = NULL;
char* ng_last_floorstate = NULL;
char* ng_current_floorstate = NULL;

int ng_input_lock_timers[NG_INPUT_LOCK_TIMER_COUNT];

void NGStorePendingRoomNumber(int room_number) {
	pending_ng_room = room_number;
}

int NGRestorePendingRoomNumber() {
	return pending_ng_room;
}

void NGUpdateCurrentTriggerRoomAndIndex(int new_room, int new_index) {
	current_ng_trigger_index = new_index;
	current_ng_trigger_room = new_room;
}

void NGClearCurrentTriggerRoomAndIndex() {
	current_ng_trigger_index = -1;
	current_ng_trigger_room = -1;
}

// These may be needed since Lara's trigger index derived from earlier may get overwritten by something.
void NGStoreBackupTriggerRoomAndIndex() {
	backup_ng_trigger_index = current_ng_trigger_index;
	backup_ng_trigger_room = current_ng_trigger_room;
}

void NGRestoreBackupTriggerRoomAndIndex() {
	current_ng_trigger_index = backup_ng_trigger_index;
	current_ng_trigger_room = backup_ng_trigger_room;
}

bool NGIsOneShotTriggeredForTile() {
	int index = ng_room_offset_table[current_ng_trigger_room] + current_ng_trigger_index;

	bool result = ng_oneshot_floorstate[index];

	return result;
}

bool NGCheckFloorStatePressedThisFrameOrLastFrame() {
	int index = ng_room_offset_table[current_ng_trigger_room] + current_ng_trigger_index;

	if (ng_current_floorstate[index] || ng_last_floorstate[index])
		return true;

	return false;
}

int NGValidateInputAgainstLockTimers(int input) {
	return input;
}

void NGItemUpdate(unsigned int item_num) {
	if (ng_items_extradata[item_num].frozen_ticks > 0) {
		ng_items_extradata[item_num].frozen_ticks--;
	}
}

bool NGIsItemFrozen(unsigned int item_num) {
	if ((unsigned short)(ng_items_extradata[item_num].frozen_ticks) > 0) {
		return true;
	}

	return false;
}

void NGSetItemFreezeTimer(unsigned int item_num, int ticks) {
	ng_items_extradata[item_num].frozen_ticks = ticks;
}

void NGUpdateFloorstateData(bool update_oneshot) {
	int index = ng_room_offset_table[current_ng_trigger_room] + current_ng_trigger_index;

	// Since this is flagged as oneshot, store the oneshot data here.
	if (update_oneshot) {
		ng_oneshot_floorstate[index] = true;
	}
	ng_current_floorstate[index] = true;
}

void NGSetupExtraState() {
	ng_items_extradata = (NG_ITEM_EXTRADATA*)game_malloc(ITEM_COUNT * sizeof(NG_ITEM_EXTRADATA));
	memset(ng_items_extradata, 0x00, ITEM_COUNT * sizeof(NG_ITEM_EXTRADATA));

	floorstate_data_size = 0;
	for (int i = 0; i < number_rooms; i++) {
		ng_room_offset_table[i] = floorstate_data_size;
		floorstate_data_size += room[i].x_size * room[i].y_size;
	}

	ng_oneshot_floorstate = (char*)game_malloc(floorstate_data_size);
	ng_last_floorstate = (char*)game_malloc(floorstate_data_size);
	ng_current_floorstate = (char*)game_malloc(floorstate_data_size);

	memset(ng_oneshot_floorstate, 0x00, floorstate_data_size);
	memset(ng_last_floorstate, 0x00, floorstate_data_size);
	memset(ng_current_floorstate, 0x00, floorstate_data_size);

	memset(ng_input_lock_timers, 0x00, sizeof(ng_input_lock_timers));
}

void NGFrameFinishExtraState() {
	memcpy(ng_last_floorstate, ng_current_floorstate, floorstate_data_size);
	memset(ng_current_floorstate, 0x00, floorstate_data_size);
}
