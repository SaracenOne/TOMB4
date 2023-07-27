#include "../../tomb4/pch.h"

#include "../../global/types.h"
#include "../../specific/function_stubs.h"
#include "../../specific/3dmath.h"
#include "../control.h"
#include "../effects.h"
#include "../objects.h"
#include "../lara.h"
#include "../tomb4fx.h"
#include "../text.h"
#include "trng.h"
#include "trng_extra_state.h"

unsigned int ng_room_offset_table[0xff];

struct NG_ITEM_EXTRADATA {
	short frozen_ticks = 0;
};

NG_ITEM_EXTRADATA *ng_items_extradata = NULL;

// TODO: In the original, there's some behaviour which allows multiple timers to run
// at once, displaying the last activated on until it runs out. Needs investigation.
#define TIMER_TRACKER_TIMEOUT 30
NGTimerTrackerType timer_tracker_type = TTT_ONLY_SHOW_SECONDS;
int timer_tracker = -1;
int timer_tracker_remaining_until_timeout = TIMER_TRACKER_TIMEOUT;

int ng_cinema_timer = -1;
int ng_cinema_type = 0;

int current_ng_trigger_index = -1;
int current_ng_trigger_room = -1;
int backup_ng_trigger_index = -1;
int backup_ng_trigger_room = -1;
int pending_ng_room = -1;

int floorstate_data_size = 0;
char* ng_oneshot_floorstate = NULL;
char* ng_last_floorstate = NULL;
char* ng_current_floorstate = NULL;

enum TRNG_INPUT {
	TRNG_INPUT_UP,
	TRNG_INPUT_DOWN,
	TRNG_INPUT_LEFT,
	TRNG_INPUT_RIGHT,
	TRNG_INPUT_DUCK,
	TRNG_INPUT_DASH,
	TRNG_INPUT_WALK,
	TRNG_INPUT_JUMP,
	TRNG_INPUT_ACTION,
	TRNG_INPUT_DRAW_WEAPON,
	TRNG_INPUT_USE_FLARE,
	TRNG_INPUT_LOOK,
	TRNG_INPUT_ROLL,
	TRNG_INVENTORY_AND_DESELECT,
	TRNG_STEP_LEFT,
	TRNG_STEP_RIGHT,
	TRNG_PAUSE,
	TRNG_SAVE_GAME,
	TRNG_LOAD_GAME,
	TRNG_WEAPON_KEYS,
	TRNG_INPUT_COUNT
};

#define NG_INPUT_LOCK_TIMER_COUNT TRNG_INPUT_COUNT
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

// This method is not accurate since it seems like rollingballs can interrupted the check.
bool NGCheckFloorStatePressedThisFrameOrLastFrame() {
	int index = ng_room_offset_table[current_ng_trigger_room] + current_ng_trigger_index;

	if (ng_current_floorstate[index] || ng_last_floorstate[index])
		return true;


	return false;
}

int NGValidateInputAgainstLockTimers(int input) {
	for (int i = 0; i < TRNG_SAVE_GAME; i++) {
		if (ng_input_lock_timers[i] != 0) {
			switch (i) {
				case TRNG_INPUT_UP:
					input &= ~IN_FORWARD;
					break;
				case TRNG_INPUT_DOWN:
					input &= ~IN_BACK;
					break;
				case TRNG_INPUT_LEFT:
					input &= ~IN_LEFT;
					break;
				case TRNG_INPUT_RIGHT:
					input &= ~IN_RIGHT;
					break;
				case TRNG_INPUT_DUCK:
					input &= ~IN_DUCK;
					break;
				case TRNG_INPUT_DASH:
					input &= ~IN_SPRINT;
					break;
				case TRNG_INPUT_WALK:
					input &= ~(IN_WALK | IN_LSTEP | IN_RSTEP); // TRNG bug?
					break;
				case TRNG_INPUT_JUMP:
					input &= ~IN_JUMP;
					break;
				case TRNG_INPUT_ACTION:
					input &= ~(IN_ACTION | IN_SELECT);
					break;
				case TRNG_INPUT_DRAW_WEAPON:
					input &= ~IN_DRAW;
					break;
				case TRNG_INPUT_USE_FLARE:
					input &= ~IN_FLARE;
					break;
				case TRNG_INPUT_LOOK:
					input &= ~IN_LOOK;
					break;
				case TRNG_INPUT_ROLL:
					input &= ~IN_ROLL;
					break;
				case TRNG_INVENTORY_AND_DESELECT:
					input &= ~(IN_OPTION | IN_DESELECT);
					break;
				case TRNG_STEP_LEFT:
					input &= ~(IN_WALK | IN_LSTEP | IN_RSTEP); // TRNG bug?
					break;
				case TRNG_STEP_RIGHT:
					input &= ~(IN_WALK | IN_LSTEP | IN_RSTEP); // TRNG bug?
					break;
				case TRNG_PAUSE:
					input &= ~IN_PAUSE;
					break;
				default:
					printf("Invalid input type\n");
					break;
			}
		}
	}

	return input;
}

bool NGValidateInputSavegame() {
	return ng_input_lock_timers[TRNG_SAVE_GAME] == 0;
}

bool NGValidateInputLoadgame() {
	return ng_input_lock_timers[TRNG_LOAD_GAME] == 0;
}

bool NGValidateInputWeaponHotkeys() {
	return ng_input_lock_timers[TRNG_WEAPON_KEYS] == 0;
}

void NGDisableInputForTime(unsigned char input, int ticks) {
	if (input > NG_INPUT_LOCK_TIMER_COUNT) {
		printf("Invalid input id!");
		return;
	}

	int final_ticks = -1;
	if (ticks > 0) {
		final_ticks = ticks;
	}

	if (input == 0) {
		for (int i = 0; i < NG_INPUT_LOCK_TIMER_COUNT; i++) {
			ng_input_lock_timers[i] = final_ticks;
		}
	} else {
		ng_input_lock_timers[input - 1] = final_ticks;
	}
}

void NGEnableInput(unsigned char input) {
	if (input == 0) {
		for (int i = 0; i < NG_INPUT_LOCK_TIMER_COUNT; i++) {
			ng_input_lock_timers[i] = 0;
		}
	} else {
		ng_input_lock_timers[input - 1] = 0;
	}
}

void NGItemUpdate(unsigned int item_num) {
	if (ng_items_extradata[item_num].frozen_ticks > 0) {
		ng_items_extradata[item_num].frozen_ticks--;
	}
}

void NGFrameStartUpdate() {
	if (ng_cinema_timer > 0 || ng_cinema_type > 0) {
		switch (ng_cinema_type) {
			case 0: { // None / curtain effect
				SetFadeClipImmediate(150);
				break;
			// TODO: these are not accurate
			} case 1: { // Tiny
				SetFadeClipImmediate(8);
				break;
			} case 2: { // Middle
				SetFadeClipImmediate(16);
				break;
			} case 3: { // Big
				SetFadeClipImmediate(32);
				break;
			} case 4: { // Huge
				SetFadeClipImmediate(64);
				break;
			} case 5: { // Fissure
				SetFadeClipImmediate(128);
				break;
			}
		}
		ng_cinema_timer--;
		if (ng_cinema_timer == 0) {
			ng_cinema_type = 0;
			SetFadeClip(0, 1);
			ng_cinema_timer--;
		}
	}

	// Input locks
	for (int i = 0; i < NG_INPUT_LOCK_TIMER_COUNT; i++) {
		if (ng_input_lock_timers[i] > 0) {
			ng_input_lock_timers[i] -= 1;
		}
	}

}

void NGDrawPhase() {
	if (timer_tracker >= 0) {
		if (items[timer_tracker].timer > 0) {
			timer_tracker_remaining_until_timeout = TIMER_TRACKER_TIMEOUT;
		} else {
			timer_tracker_remaining_until_timeout--;
		}

		if (timer_tracker_remaining_until_timeout > 0) {
			char format_buffer[64];
			int remainder = items[timer_tracker].timer % 30;
			int seconds = items[timer_tracker].timer / 30;

			switch (timer_tracker_type) {
				case TTT_ONLY_SHOW_SECONDS:
					sprintf(format_buffer, "%d", seconds);
					break;
				case TTT_SECONDS_AND_ONE_DECIMAL_POINT_SEPERATOR:
					sprintf(format_buffer, "%d.%01d", seconds, remainder);
					break;
				case TTT_SECONDS_AND_TWO_DECIMAL_POINT_SEPERATOR:
					sprintf(format_buffer, "%d.%02d", seconds, remainder);
					break;
				case TTT_SECONDS_AND_ONE_DECIMAL_COLON_SEPERATOR:
					sprintf(format_buffer, "%d:%01d", seconds, remainder);
					break;
				case TTT_SECONDS_AND_TWO_DECIMAL_COLON_SEPERATOR:
					sprintf(format_buffer, "%d:%02d", seconds, remainder);
					break;
				case TTT_SECONDS_WITH_THREE_NOUGHTS:
					sprintf(format_buffer, "%03d", seconds);
					break;
				default:
					sprintf(format_buffer, "%d", items[timer_tracker].timer);
					break;
			}
			PrintString(phd_centerx, phd_winymax - font_height * 0.25, 0, format_buffer, FF_CENTER);
		}
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

void NGSetCurtainTimer(int ticks) {
	if (ng_cinema_type == 0) {
		ng_cinema_timer = ticks;
	}
}

void NGSetCinemaTypeAndTimer(int type, int ticks) {
	ng_cinema_type = type;
	ng_cinema_timer = ticks;
}

void NGSetDisplayTimerForMoveableWithType(int item_id, NGTimerTrackerType new_timer_tracker_type) {
	timer_tracker = item_id;
	timer_tracker_type = new_timer_tracker_type;
	timer_tracker_remaining_until_timeout = TIMER_TRACKER_TIMEOUT;
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
	timer_tracker_type = TTT_ONLY_SHOW_SECONDS;
	timer_tracker = -1;
	timer_tracker_remaining_until_timeout = TIMER_TRACKER_TIMEOUT;

	ng_items_extradata = (NG_ITEM_EXTRADATA*)game_malloc(ITEM_COUNT * sizeof(NG_ITEM_EXTRADATA));
	memset(ng_items_extradata, 0x00, ITEM_COUNT * sizeof(NG_ITEM_EXTRADATA));

	ng_cinema_timer = -1;
	ng_cinema_type = 0;

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
