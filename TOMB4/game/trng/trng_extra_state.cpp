#include "../../tomb4/pch.h"

#include "../../global/types.h"
#include "../../specific/function_stubs.h"
#include "../../specific/3dmath.h"
#include "../control.h"
#include "../gameflow.h"
#include "../effects.h"
#include "../objects.h"
#include "../lara.h"
#include "../tomb4fx.h"
#include "../sound.h"
#include "../text.h"

#include "trng.h"
#include "trng_extra_state.h"
#include "trng_animation.h"
#include "trng_condition.h"
#include "trng_flipeffect.h"
#include "trng_script_parser.h"
#include "trng_organizer.h"
#include "trng_triggergroup.h"
#include "trng_globaltrigger.h"
#include "../../specific/file.h"

// Includes tightrope state variables
NG_LARA_EXTRASTATE ng_lara_extrastate;

unsigned int ng_room_offset_table[0xff];

struct NG_MOVEMENT_INFO {
	short horizontal_rotation_speed = 0;
	short vertical_rotation_speed = 0;
	int horizontal_rotation_remaining = 0;
	int vertical_rotation_remaining = 0;
	short horizontal_movement_speed = 0;
	short vertical_movement_speed = 0;
	short movement_in_progress_sound = -1;
	short movement_finished_sound = -1;
	short move_horizontal_angle = 0;
	int move_horizontal_remaining_units = 0;
	int move_vertical_remaining_units = 0;
	int move_horizontal_repeat_units = 0;
	int move_vertical_repeat_units = 0;
};

// NG_ITEM_EXTRADATA is persistent supllementary data used by TRNG triggers.
// The state here can subseqeuently be serialized as additional data for savegames.
struct NG_ITEM_EXTRADATA {
	short frozen_ticks = 0;
	bool collison_disabled = false; // Will only disable the ObjectCollision routine. Doors and enemies stll have collision.
	unsigned int mesh_visibility_mask = 0xffffffff;
	short after_death_override = 0;

	NG_MOVEMENT_INFO movement_info;
};

NG_ITEM_EXTRADATA *ng_items_extradata = NULL;
NG_MOVEMENT_INFO *ng_statics_movement_info = NULL;

void NGResetMovementInfo(NG_MOVEMENT_INFO* movement_info) {
	movement_info->horizontal_rotation_speed = 0;
	movement_info->vertical_rotation_speed = 0;
	movement_info->horizontal_rotation_remaining = 0;
	movement_info->vertical_rotation_remaining = 0;
	movement_info->horizontal_movement_speed = 0;
	movement_info->vertical_movement_speed = 0;
	movement_info->movement_in_progress_sound = -1;
	movement_info->movement_finished_sound = -1;
	movement_info->move_horizontal_angle = 0;
	movement_info->move_horizontal_remaining_units = 0;
	movement_info->move_vertical_remaining_units = 0;
	movement_info->move_horizontal_repeat_units = 0;
	movement_info->move_vertical_repeat_units = 0;
}

void NGResetItemExtraData(int item_number) {
	NG_ITEM_EXTRADATA *current_extradata = &ng_items_extradata[item_number];

	if (current_extradata) {
		current_extradata->frozen_ticks = 0;
		current_extradata->collison_disabled = false; // Will only disable the ObjectCollision routine. Doors and enemies stll have collision.

		current_extradata->mesh_visibility_mask = 0xffffffff;
		current_extradata->after_death_override = 0;

		NGResetMovementInfo(&current_extradata->movement_info);
	} else {
		NGLog(NG_LOG_TYPE_ERROR, "NGResetItemExtraData: invalid item_number!");
	}
}

int ng_animation_current_animation = -1;
short ng_animation_prev_hands_state = LG_NO_ARMS;
int ng_animation_target_item = -1;
int ng_animation_target_test_position = -1;

NG_GLOBAL_TRIGGER_STATE ng_global_trigger_states[MAX_NG_GLOBAL_TRIGGERS];
NG_TRIGGER_GROUP_STATE ng_trigger_group_states[MAX_NG_TRIGGER_GROUPS];
NG_ORGANIZER_STATE ng_organizer_states[MAX_NG_ORGANIZERS];

// TODO: In the original, there's some behaviour which allows multiple timers to run
// at once, displaying the last activated on until it runs out. Needs investigation.
#define TIMER_TRACKER_TIMEOUT 30
NGTimerTrackerType timer_tracker_type = TTT_ONLY_SHOW_SECONDS;
int timer_tracker = -1;
int timer_tracker_remaining_until_timeout = 0;

int ng_cinema_timer = -1;
int ng_cinema_type = 0;

struct TriggerState {
	int index = -1;
	int room = -1;
};

#define BACKUP_TRIGGER_STATE_COUNT 2
TriggerState ng_backup_trigger_state[BACKUP_TRIGGER_STATE_COUNT];
int ng_backup_trigger_state_count = 0;

TriggerState ng_current_trigger_state;

int pending_ng_room = -1;

int ng_floorstate_data_size = 0;
char *ng_flipeffect_oneshot_floorstate = NULL;
char *ng_action_oneshot_floorstate = NULL;

int ng_last_flipeffect_floor_trigger = -1;
int ng_current_flipeffect_floor_trigger = -1;

int ng_last_action_floor_trigger = -1;
int ng_current_action_floor_trigger = -1;

int ng_heavy_last_flipeffect_floor_trigger = -1;
int ng_heavy_current_flipeffect_floor_trigger = -1;

int ng_heavy_last_action_floor_trigger = -1;
int ng_heavy_current_action_floor_trigger = -1;

int lara_damage_resistence = 1000;

bool ng_lara_infinite_air = false;

// Timers
int ng_global_timer = 0;
char ng_global_timer_frame_increment = 0;
NGTimerPosition ng_global_timer_position = NG_TIMER_POSITION_INVISIBLE;
int ng_global_timer_time_until_hide = 0;

int ng_local_timer = 0;
char ng_local_timer_frame_increment = 0;
NGTimerPosition ng_local_timer_position = NG_TIMER_POSITION_INVISIBLE;
int ng_local_timer_time_until_hide = 0;

// Level
int pending_level_load_timer = -1;
int pending_level_load_id = 0;

// Variables
int ng_current_value = 0;
int ng_global_alfa = 0;
int ng_global_beta = 0;
int ng_global_delta = 0;
int ng_local_alfa = 0;
int ng_local_beta = 0;
int ng_local_delta = 0;
int ng_last_input_number = 0;

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

int ng_looped_sound_state[NumSamples];

#define NG_INPUT_TIMER_COUNT TRNG_INPUT_COUNT
int ng_input_lock_timers[NG_INPUT_TIMER_COUNT];
int ng_input_simulate_timers[NG_INPUT_TIMER_COUNT];

void NGStorePendingRoomNumber(int room_number) {
	pending_ng_room = room_number;
}

int NGRestorePendingRoomNumber() {
	return pending_ng_room;
}

void NGUpdateCurrentTriggerRoomAndIndex(int new_room, int new_index) {
	ng_current_trigger_state.index = new_index;
	ng_current_trigger_state.room = new_room;
}

void NGClearCurrentTriggerRoomAndIndex() {
	ng_current_trigger_state.index = -1;
	ng_current_trigger_state.room = -1;
}

int NGGetPluginIDForFloorData(short *floor_data_ptr) {
	int index = floor_data_ptr - floor_data;
	if (ng_floor_id_table) {
		if (index < ng_floor_id_size) {
			int plugin_id = ng_floor_id_table[index];
			return plugin_id;
		} else {
			NGLog(NG_LOG_TYPE_ERROR, "NGGetPluginIDForFloorData: Overflow!");
		}
	}
	return 0;
}

// These may be needed since Lara's trigger index derived from earlier may get overwritten by something.
void NGStoreBackupTriggerRoomAndIndex() {
	ng_backup_trigger_state[ng_backup_trigger_state_count].index = ng_current_trigger_state.index;
	ng_backup_trigger_state[ng_backup_trigger_state_count].room = ng_current_trigger_state.room;

	ng_backup_trigger_state_count++;
	if (ng_backup_trigger_state_count >= BACKUP_TRIGGER_STATE_COUNT) {
		NGLog(NG_LOG_TYPE_ERROR, "NGStoreBackupTriggerRoomAndIndex: Overflow!");
		exit(-1);
	}
}

void NGRestoreBackupTriggerRoomAndIndex() {
	ng_current_trigger_state.index = ng_backup_trigger_state[ng_backup_trigger_state_count - 1].index;
	ng_current_trigger_state.room = ng_backup_trigger_state[ng_backup_trigger_state_count - 1].room;

	ng_backup_trigger_state_count--;
	if (ng_backup_trigger_state_count < 0) {
		NGLog(NG_LOG_TYPE_ERROR, "NGRestoreBackupTriggerRoomAndIndex: Underflow!");
		exit(-1);
	}
}

bool NGIsFlipeffectOneShotTriggeredForTile() {
	int index = ng_room_offset_table[ng_current_trigger_state.room] + ng_current_trigger_state.index;

	bool result = ng_flipeffect_oneshot_floorstate[index];

	return result;
}

bool NGIsActionOneShotTriggeredForTile() {
	int index = ng_room_offset_table[ng_current_trigger_state.room] + ng_current_trigger_state.index;

	bool result = ng_action_oneshot_floorstate[index];

	return result;
}

// This method is not accurate since it seems like rollingballs can interrupted the check.
bool NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(bool is_heavy_triggered) {
	int index = ng_room_offset_table[ng_current_trigger_state.room] + ng_current_trigger_state.index;

	if (!is_heavy_triggered) {
		if (ng_current_flipeffect_floor_trigger == index || ng_last_flipeffect_floor_trigger == index)
			return true;
	} else {
		if (ng_heavy_current_flipeffect_floor_trigger == index || ng_heavy_last_flipeffect_floor_trigger == index)
			return true;
	}

	return false;
}

extern bool NGCheckActionFloorStatePressedThisFrameOrLastFrame(bool is_heavy_triggered) {
	int index = ng_room_offset_table[ng_current_trigger_state.room] + ng_current_trigger_state.index;

	if (!is_heavy_triggered) {
		if (ng_current_action_floor_trigger == index || ng_last_action_floor_trigger == index)
			return true;
	} else {
		if (ng_heavy_current_action_floor_trigger == index || ng_heavy_last_action_floor_trigger == index)
			return true;
	}

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
					NGLog(NG_LOG_TYPE_ERROR, "Invalid input type %u!", i);
					break;
			}
		}
	}

	return input;
}

int NGApplySimulatedInput(int input) {
	for (int i = 0; i < TRNG_SAVE_GAME; i++) {
		if (ng_input_simulate_timers[i] != 0) {
			switch (i) {
			case TRNG_INPUT_UP:
				input |= IN_FORWARD;
				break;
			case TRNG_INPUT_DOWN:
				input |= IN_BACK;
				break;
			case TRNG_INPUT_LEFT:
				input |= IN_LEFT;
				break;
			case TRNG_INPUT_RIGHT:
				input |= IN_RIGHT;
				break;
			case TRNG_INPUT_DUCK:
				input |= IN_DUCK;
				break;
			case TRNG_INPUT_DASH:
				input |= IN_SPRINT;
				break;
			case TRNG_INPUT_WALK:
				input |= IN_WALK;
				break;
			case TRNG_INPUT_JUMP:
				input |= IN_JUMP;
				break;
			case TRNG_INPUT_ACTION:
				input |= IN_ACTION;
				break;
			case TRNG_INPUT_DRAW_WEAPON:
				input |= IN_DRAW;
				break;
			case TRNG_INPUT_USE_FLARE:
				input |= IN_FLARE;
				break;
			case TRNG_INPUT_LOOK:
				input |= IN_LOOK;
				break;
			case TRNG_INPUT_ROLL:
				input |= IN_ROLL;
				break;
			case TRNG_INVENTORY_AND_DESELECT:
				input |= (IN_OPTION | IN_DESELECT);
				break;
			case TRNG_STEP_LEFT:
				input |= IN_LSTEP;
				break;
			case TRNG_STEP_RIGHT:
				input |= IN_LSTEP;
				break;
			case TRNG_PAUSE:
				input |= IN_PAUSE;
				break;
			default:
				NGLog(NG_LOG_TYPE_ERROR, "Invalid input type %u!", i);
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
	if (input > NG_INPUT_TIMER_COUNT) {
		NGLog(NG_LOG_TYPE_ERROR, "NGDisableInputForTime: Invalid input type %u!", input);
		return;
	}

	int final_ticks = -1;
	if (ticks > 0) {
		final_ticks = ticks;
	}

	if (input == 0) {
		for (int i = 0; i < NG_INPUT_TIMER_COUNT; i++) {
			ng_input_lock_timers[i] = final_ticks;
		}
	} else {
		ng_input_lock_timers[input - 1] = final_ticks;
	}
}

void NGSimulateInputForTime(unsigned char input, int ticks) {
	if (input > NG_INPUT_TIMER_COUNT) {
		NGLog(NG_LOG_TYPE_ERROR, "NGSimulateInputForTime: Invalid input type %u!", input);
		return;
	}

	int final_ticks = -1;
	if (ticks > 0) {
		final_ticks = ticks;
	}

	if (input == 0) {
		for (int i = 0; i < NG_INPUT_TIMER_COUNT; i++) {
			ng_input_simulate_timers[i] = final_ticks;
		}
	} else {
		ng_input_simulate_timers[input - 1] = final_ticks;
	}
}

void NGEnableInput(unsigned char input) {
	if (input == 0) {
		for (int i = 0; i < NG_INPUT_TIMER_COUNT; i++) {
			ng_input_lock_timers[i] = 0;
		}
	} else {
		ng_input_lock_timers[input - 1] = 0;
	}
}

void NGHandleItemRotation(unsigned int item_num) {
	if (NGGetItemHorizontalRotationRemaining(item_num)) {
		int rotate_by_amount = NGGetItemHorizontalRotationSpeed(item_num);
		int remaining_rotation_units = NGGetItemHorizontalRotationRemaining(item_num);
		if (
			(remaining_rotation_units >= 0 && rotate_by_amount > remaining_rotation_units) || 
			(remaining_rotation_units < 0 && rotate_by_amount < remaining_rotation_units)) {
			rotate_by_amount = remaining_rotation_units;
		}

		NGRotateItemY(item_num, NGGetItemHorizontalRotationSpeed(item_num));
		NGSetItemHorizontalRotationRemaining(item_num, remaining_rotation_units - rotate_by_amount);

		if (NGGetItemHorizontalRotationRemaining(item_num) == 0) {
			// Reset everything
			NGSetItemHorizontalRotationSpeed(item_num, 0);
		}
	} else {
		NGRotateItemY(item_num, NGGetItemHorizontalRotationSpeed(item_num));
	}

	if (NGGetItemVerticalRotationRemaining(item_num)) {
		int rotate_by_amount = NGGetItemVerticalRotationSpeed(item_num);
		int remaining_rotation_units = NGGetItemVerticalRotationRemaining(item_num);
		if (
			(remaining_rotation_units >= 0 && rotate_by_amount > remaining_rotation_units) ||
			(remaining_rotation_units < 0 && rotate_by_amount < remaining_rotation_units)) {
			rotate_by_amount = remaining_rotation_units;
		}

		NGRotateItemX(item_num, NGGetItemVerticalRotationSpeed(item_num));
		NGSetItemVerticalRotationRemaining(item_num, remaining_rotation_units - rotate_by_amount);

		if (NGGetItemVerticalRotationRemaining(item_num) == 0) {
			// Reset everything
			NGSetItemVerticalRotationSpeed(item_num, 0);
		}
	} else {
		NGRotateItemX(item_num, NGGetItemVerticalRotationSpeed(item_num));
	}
}

void NGHandleItemMovement(unsigned int item_num) {
	if (NGGetItemHorizontalMovementRemainingUnits(item_num)) {
		int move_by_amount = NGGetItemHorizontalMovementSpeed(item_num);
		int remaining_movement_units = NGGetItemHorizontalMovementRemainingUnits(item_num);
		if (
			(remaining_movement_units >= 0 && move_by_amount > remaining_movement_units) ||
			(remaining_movement_units < 0 && move_by_amount < remaining_movement_units)) {
			move_by_amount = remaining_movement_units;
		}

		NGMoveItemHorizontalByUnits(item_num, ng_items_extradata[item_num].movement_info.move_horizontal_angle, move_by_amount);
		NGSetItemHorizontalMovementRemainingUnits(item_num, remaining_movement_units - move_by_amount);

		if (NGGetItemHorizontalMovementRemainingUnits(item_num) == 0) {
			if (NGGetItemMovementFinishedSound(item_num) != -1)
				SoundEffect(NGGetItemMovementFinishedSound(item_num), &items[item_num].pos, 0);

			// Reset everything or loop
			if (NGGetItemHorizontalMovementRepeatUnits(item_num) != 0) {
				NGSetItemHorizontalMovementRemainingUnits(item_num, NGGetItemHorizontalMovementRepeatUnits(item_num));
				NGSetItemHorizontalMovementAngle(item_num, NGGetItemHorizontalMovementAngle(item_num) + NG_DEGREE(180));
			} else {
				NGSetItemHorizontalMovementSpeed(item_num, 0);
			}
		} else {
			if (NGGetItemMovementInProgressSound(item_num) != -1)
				SoundEffect(NGGetItemMovementInProgressSound(item_num), &items[item_num].pos, 0);
		}
	}

	if (NGGetItemVerticalMovementRemainingUnits(item_num)) {
		int move_by_amount = NGGetItemVerticalMovementSpeed(item_num);
		int remaining_movement_units = NGGetItemVerticalMovementRemainingUnits(item_num);
		if (
			(remaining_movement_units >= 0 && move_by_amount > remaining_movement_units) ||
			(remaining_movement_units < 0 && move_by_amount < remaining_movement_units)) {
			move_by_amount = remaining_movement_units;
		}

		NGMoveItemVerticalByUnits(item_num, move_by_amount);
		NGSetItemVerticalMovementRemainingUnits(item_num, remaining_movement_units - move_by_amount);

		if (NGGetItemVerticalMovementRemainingUnits(item_num) == 0) {
			if (NGGetItemMovementFinishedSound(item_num) != -1)
				SoundEffect(NGGetItemMovementFinishedSound(item_num), &items[item_num].pos, 0);

			// Reset everything or loop
			if (NGGetItemVerticalMovementRepeatUnits(item_num) != 0) {
				NGSetItemVerticalMovementSpeed(item_num, -NGGetItemVerticalMovementSpeed(item_num));
				NGSetItemVerticalMovementRemainingUnits(item_num, NGGetItemVerticalMovementRepeatUnits(item_num));
				NGSetItemVerticalMovementRepeatUnits(item_num, -NGGetItemVerticalMovementRepeatUnits(item_num));
			}
			else {
				NGSetItemVerticalMovementSpeed(item_num, 0);
			}
		} else {
			if (NGGetItemMovementInProgressSound(item_num) != -1)
				SoundEffect(NGGetItemMovementInProgressSound(item_num), &items[item_num].pos, 0);
		}
	}
}

// Statics

void NGHandleStaticRotation(unsigned int static_num) {
	if (NGGetStaticHorizontalRotationRemaining(static_num)) {
		int rotate_by_amount = NGGetStaticHorizontalRotationSpeed(static_num);
		int remaining_rotation_units = NGGetStaticHorizontalRotationRemaining(static_num);
		if (
			(remaining_rotation_units >= 0 && rotate_by_amount > remaining_rotation_units) ||
			(remaining_rotation_units < 0 && rotate_by_amount < remaining_rotation_units)) {
			rotate_by_amount = remaining_rotation_units;
		}

		NGRotateStaticY(static_num, NGGetStaticHorizontalRotationSpeed(static_num));
		NGSetStaticHorizontalRotationRemaining(static_num, remaining_rotation_units - rotate_by_amount);

		if (NGGetStaticHorizontalRotationRemaining(static_num) == 0) {
			// Reset everything
			NGSetStaticHorizontalRotationSpeed(static_num, 0);
		}
	}
	else {
		NGRotateStaticY(static_num, NGGetStaticHorizontalRotationSpeed(static_num));
	}

	if (NGGetStaticVerticalRotationRemaining(static_num)) {
		int rotate_by_amount = NGGetStaticVerticalRotationSpeed(static_num);
		int remaining_rotation_units = NGGetStaticVerticalRotationRemaining(static_num);
		if (
			(remaining_rotation_units >= 0 && rotate_by_amount > remaining_rotation_units) ||
			(remaining_rotation_units < 0 && rotate_by_amount < remaining_rotation_units)) {
			rotate_by_amount = remaining_rotation_units;
		}

		NGRotateStaticX(static_num, NGGetStaticVerticalRotationSpeed(static_num));
		NGSetStaticVerticalRotationRemaining(static_num, remaining_rotation_units - rotate_by_amount);

		if (NGGetStaticVerticalRotationRemaining(static_num) == 0) {
			// Reset everything
			NGSetStaticVerticalRotationSpeed(static_num, 0);
		}
	}
	else {
		NGRotateStaticX(static_num, NGGetStaticVerticalRotationSpeed(static_num));
	}
}

void NGHandleStaticMovement(unsigned int static_num) {
	if (NGGetStaticHorizontalMovementRemainingUnits(static_num)) {
		int move_by_amount = NGGetStaticHorizontalMovementSpeed(static_num);
		int remaining_movement_units = NGGetStaticHorizontalMovementRemainingUnits(static_num);
		if (
			(remaining_movement_units >= 0 && move_by_amount > remaining_movement_units) ||
			(remaining_movement_units < 0 && move_by_amount < remaining_movement_units)) {
			move_by_amount = remaining_movement_units;
		}

		NGMoveStaticHorizontalByUnits(static_num, ng_statics_movement_info[static_num].move_horizontal_angle, move_by_amount);
		NGSetStaticHorizontalMovementRemainingUnits(static_num, remaining_movement_units - move_by_amount);

		if (NGGetStaticHorizontalMovementRemainingUnits(static_num) == 0) {
			if (NGGetStaticMovementFinishedSound(static_num) != -1)
				SoundEffect(NGGetStaticMovementFinishedSound(static_num), &items[static_num].pos, 0);

			// Reset everything or loop
			if (NGGetStaticHorizontalMovementRepeatUnits(static_num) > 0) {
				NGSetStaticHorizontalMovementRemainingUnits(static_num, NGGetStaticHorizontalMovementRepeatUnits(static_num));
				NGSetStaticHorizontalMovementAngle(static_num, NGGetStaticHorizontalMovementAngle(static_num) + NG_DEGREE(180));
			} else {
				NGSetStaticHorizontalMovementSpeed(static_num, 0);
			}
		}
		else {
			if (NGGetStaticMovementInProgressSound(static_num) != -1)
				SoundEffect(NGGetStaticMovementInProgressSound(static_num), &items[static_num].pos, 0);
		}
	}

	if (NGGetStaticVerticalMovementRemainingUnits(static_num)) {
		int move_by_amount = NGGetStaticVerticalMovementSpeed(static_num);
		int remaining_movement_units = NGGetStaticVerticalMovementRemainingUnits(static_num);
		if (
			(remaining_movement_units >= 0 && move_by_amount > remaining_movement_units) ||
			(remaining_movement_units < 0 && move_by_amount < remaining_movement_units)) {
			move_by_amount = remaining_movement_units;
		}

		NGMoveStaticVerticalByUnits(static_num, move_by_amount);
		NGSetStaticVerticalMovementRemainingUnits(static_num, remaining_movement_units - move_by_amount);

		if (NGGetStaticVerticalMovementRemainingUnits(static_num) == 0) {
			if (NGGetStaticMovementFinishedSound(static_num) != -1)
				SoundEffect(NGGetStaticMovementFinishedSound(static_num), &items[static_num].pos, 0);

			// Reset everything or loop
			if (NGGetStaticVerticalMovementRepeatUnits(static_num) > 0) {
				NGSetStaticVerticalMovementSpeed(static_num, -NGGetStaticVerticalMovementSpeed(static_num));
				NGSetStaticVerticalMovementRemainingUnits(static_num, NGGetStaticVerticalMovementRepeatUnits(static_num));
				NGSetStaticVerticalMovementRepeatUnits(static_num, -NGGetStaticVerticalMovementRepeatUnits(static_num));
			} else {
				NGSetStaticVerticalMovementSpeed(static_num, 0);
			}
		}
		else {
			if (NGGetStaticMovementInProgressSound(static_num) != -1)
				SoundEffect(NGGetStaticMovementInProgressSound(static_num), &items[static_num].pos, 0);
		}
	}
}

void NGUpdateAllItems() {
	for (int i = 0; i < ITEM_COUNT; i++) {
		if (ng_items_extradata[i].frozen_ticks > 0) {
			ng_items_extradata[i].frozen_ticks--;
		}

		if (NGGetItemHorizontalRotationSpeed(i) || NGGetItemVerticalRotationSpeed(i)) {
			NGHandleItemRotation(i);
		}

		if (NGGetItemHorizontalMovementRemainingUnits(i) || NGGetItemVerticalMovementRemainingUnits(i)) {
			NGHandleItemMovement(i);
		}
	}
}

void NGUpdateAllStatics() {
	for (int i = 0; i < ng_static_id_count; i++) {
		if (NGGetStaticHorizontalRotationSpeed(i) || NGGetStaticVerticalRotationSpeed(i)) {
			NGHandleStaticRotation(i);
		}

		if (NGGetStaticHorizontalMovementRemainingUnits(i) || NGGetStaticVerticalMovementRemainingUnits(i)) {
			NGHandleStaticMovement(i);
		}
	}
}

int ng_found_item_index = -1;

#define MAX_LARA_COLLISONS 16
int ng_lara_moveable_collisions[MAX_LARA_COLLISONS];
int ng_lara_moveable_collision_size = 0;

#define MAX_LARA_STATIC_COLLISONS 16
int ng_lara_static_collisions[MAX_LARA_STATIC_COLLISONS];
int ng_lara_static_collision_size = 0;

void NGAddLaraMoveableCollision(int item_number) {
	if (ng_lara_moveable_collision_size >= MAX_LARA_COLLISONS-1)
		return;
	
	for (int i = 0; i < ng_lara_moveable_collision_size; i++) {
		if (ng_lara_moveable_collisions[i] == item_number) {
			return;
		}
	}

	ng_lara_moveable_collisions[ng_lara_moveable_collision_size] = item_number;
	ng_lara_moveable_collision_size++;
}

extern void NGAddLaraStaticCollision(int room_number, int mesh_number) {
	if (ng_lara_static_collision_size >= MAX_LARA_STATIC_COLLISONS-1)
		return;

	int current_static_id = -1;

	// Very slow, replace this with caching
	for (int i = 0; i < NG_STATIC_ID_TABLE_SIZE; i++) {
		if (ng_room_remap_table[ng_static_id_table[i].remapped_room_index].room_index == room_number && ng_static_id_table[i].mesh_id == mesh_number) {
			current_static_id = i;
			break;
		}
	}

	if (current_static_id >= 0) {
		for (int i = 0; i < ng_lara_static_collision_size; i++) {
			if (ng_lara_static_collisions[i] == current_static_id) {
				return;
			}
		}

		ng_lara_static_collisions[ng_lara_static_collision_size] = current_static_id;
		ng_lara_static_collision_size++;
	}
}

void NGClearLaraCollisions() {
	ng_lara_moveable_collision_size = 0;
	ng_lara_static_collision_size = 0;
}

int NGIsLaraCollidingWithMoveableID(int item_number) {
	for (int i = 0; i < ng_lara_moveable_collision_size; i++) {
		if (ng_lara_moveable_collisions[i] == item_number) {
			return ng_lara_moveable_collisions[i];
		}
	}

	return -1;
}

int NGIsLaraCollidingWithMoveableSlot(int slot_number) {
	for (int i = 0; i < ng_lara_moveable_collision_size; i++) {
		if (items[ng_lara_moveable_collisions[i]].object_number == slot_number) {
			return ng_lara_moveable_collisions[i];
		}
	}

	return -1;
}

// Note, in TRNG, a 'friend' seems to only refer to Von Croy and the Guide, but not 'Troops'. I don't know exactly how this
// is determined, but I'm only able to assume at this point that it's dictated by object ID. If this is wrong, please let me know.
bool NGIsObjectFriendType(int object_id) {
	if (object_id == VON_CROY || object_id == GUIDE) {
		return true;
	} else {
		return false;
	}
}

int NGIsLaraCollidingWithCreature(NGCreatureType creature_type) {
	for (int i = 0; i < ng_lara_moveable_collision_size; i++) {
		if (objects[items[ng_lara_moveable_collisions[i]].object_number].intelligent) {
			switch (creature_type) {
				case NG_CREATURE_TYPE_ANY: {
					return ng_lara_moveable_collisions[i];
				}
				case NG_CREATURE_TYPE_MORTAL: {
					int object_id = items[ng_lara_moveable_collisions[i]].object_number;

					if(!objects[object_id].undead && !NGIsObjectFriendType(object_id)) {
						return ng_lara_moveable_collisions[i];
					}
					break;
				}
				case NG_CREATURE_TYPE_IMMORTAL: {
					int object_id = items[ng_lara_moveable_collisions[i]].object_number;

					if(objects[object_id].undead && !NGIsObjectFriendType(object_id)) {
						return ng_lara_moveable_collisions[i];
					}
					break;
				}
				case NG_CREATURE_TYPE_FRIEND: {
					int object_id = items[ng_lara_moveable_collisions[i]].object_number;

					if(NGIsObjectFriendType(object_id)) {
						return ng_lara_moveable_collisions[i];
					}
					break;
				}
				default: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGIsLaraCollidingWithCreature: creature_type %u unimplemented!", creature_type);
					break;
				}
			}
		}
	}

	return -1;
}

int NGIsLaraCollidingWithStaticID(int id) {
	for (int i = 0; i < ng_lara_static_collision_size; i++) {
		if (ng_lara_static_collisions[i] == id) {
			return ng_lara_static_collisions[i];
		}
	}

	return -1;
}

int NGIsLaraCollidingWithStaticSlot(int slot) {
	for (int i = 0; i < ng_lara_static_collision_size; i++) {
		int static_id = ng_lara_static_collisions[i];

		if (static_id >= 0) {
			NGStaticTableEntry* entry = &ng_static_id_table[static_id];

			if (entry->remapped_room_index >= 0 && entry->remapped_room_index < NG_ROOM_REMAP_TABLE_SIZE) {
				if (ng_room_remap_table[entry->remapped_room_index].room_index >= 0 && ng_room_remap_table[entry->remapped_room_index].room_index < number_rooms) {

					if (entry->mesh_id >= 0 && room[ng_room_remap_table[entry->remapped_room_index].room_index].num_meshes) {

						if (room[ng_room_remap_table[entry->remapped_room_index].room_index].mesh[entry->mesh_id].static_number == slot) {
							return static_id;
						}
					}
				}
			}
		}
	}

	return -1;
}

bool NGProcessGlobalTriggers(int inventory_object_id) {
	bool management_replaced = false;
	if (ng_levels[gfCurrentLevel].records) {
		int global_trigger_count = ng_levels[gfCurrentLevel].records->global_trigger_count;
		for (int i = 0; i < global_trigger_count; i++) {
			if (NGExecuteSingleGlobalTrigger(i, inventory_object_id)) {
				management_replaced = true;
			}
		}
	}

	return management_replaced;
}

void NGFrameStartUpdate() {
	NGProcessAnimations();

	NGProcessGlobalTriggers(NO_ITEM);
	NGProcessTriggerGroups();
	NGProcessOrganizers();

	if (pending_level_load_timer >= 0) {
		if (pending_level_load_timer == 0) {
			gfLevelComplete = pending_level_load_id;
			gfRequiredStartPos = 0;
		}
		pending_level_load_timer--;
	}

	// If a timer has reached zero, reset its incrementation value
	if (ng_local_timer <= 0)
		ng_local_timer_frame_increment = 0;
	if (ng_global_timer <= 0)
		ng_global_timer_frame_increment = 0;

	NGClearLaraCollisions();

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

	// Input Locks and Input Simulators
	for (int i = 0; i < NG_INPUT_TIMER_COUNT; i++) {
		if (ng_input_lock_timers[i] > 0) {
			ng_input_lock_timers[i] -= 1;
		}
		if (ng_input_simulate_timers[i] > 0) {
			ng_input_simulate_timers[i] -= 1;
		}
	}

	// Looping sounds
	for (int i = 0; i < NumSamples; i++) {
		if (ng_looped_sound_state[i] > 0) {
			SoundEffect(i, NULL, SFX_ALWAYS);
			ng_looped_sound_state[i] -= 1;
		}
	}

	// Cold and damage rooms
	{
		ROOM_INFO* r = &room[lara_item->room_number];
		if (r) {
			int room_flags = r->flags;

			// Lara is in a damage room.
			if (room_flags & 0x10) {
				// TODO
			} else {

			}

			// Lara is in a damage room.
			if (room_flags & ROOM_COLD) {
				// TODO
			}
			else {

			}
		}
	}

	// Timers
	ng_global_timer += ng_global_timer_frame_increment;
	if (ng_global_timer < 0)
		ng_global_timer = 0;

	if (ng_global_timer_time_until_hide > 0)
		ng_global_timer_time_until_hide--;

	ng_local_timer += ng_local_timer_frame_increment;
	if (ng_local_timer < 0)
		ng_local_timer = 0;

	if (ng_local_timer_time_until_hide > 0)
		ng_local_timer_time_until_hide--;
}

void NGDrawTimer(int timer, NGTimerPosition timer_position, int timer_time_until_hide) {
	if (timer_time_until_hide != 0) {
		if (timer_position != NG_TIMER_POSITION_INVISIBLE) {
			char format_buffer[80];
			int remainder = timer % 30;
			int seconds = timer / 30;

			sprintf(format_buffer, "%.2d:%.2d:%.1d", seconds / 60, seconds % 60, (334 * (remainder)) / 1000);

			// TODO: the scaling of the text is not correct at all
			switch (timer_position) {
			case NG_TIMER_POSITION_BOTTOM_CENTER:
				PrintString(phd_centerx, phd_winymax - font_height * 0.25, 0, format_buffer, FF_CENTER);
				break;
			case NG_TIMER_POSITION_TOP_CENTER:
				PrintString(phd_centerx, font_height, 0, format_buffer, FF_CENTER);
				break;
			case NG_TIMER_POSITION_CENTER_CENTER:
				PrintString(phd_centerx, phd_centery - font_height * 0.5, 0, format_buffer, FF_CENTER);
				break;
			case NG_TIMER_POSITION_TOP_LEFT:
				PrintString(0, font_height, 0, format_buffer, 0);
				break;
			case NG_TIMER_POSITION_TOP_RIGHT:
				PrintString(phd_winxmax, font_height, 0, format_buffer, FF_RJUSTIFY);
				break;
			case NG_TIMER_POSITION_BOTTOM_LEFT:
				PrintString(0, phd_winymax - font_height * 0.25, 0, format_buffer, 0);
				break;
			case NG_TIMER_POSITION_BOTTOM_RIGHT:
				PrintString(phd_winxmax, phd_winymax - font_height * 0.25, 0, format_buffer, FF_RJUSTIFY);
				break;
			case NG_TIMER_POSITION_DOWN_DAMAGE_BAR:
			case NG_TIMER_POSITION_DOWN_COLD_BAR:
				PrintString(phd_centerx, phd_winymax - font_height * 0.25, 0, format_buffer, FF_CENTER);
				break;
			case NG_TIMER_POSITION_DOWN_LEFT_BARS:
				PrintString(0, font_height, 0, format_buffer, 0);
			case NG_TIMER_POSITION_DOWN_RIGHT_BARS:
				PrintString(phd_winxmax, font_height, 0, format_buffer, FF_RJUSTIFY);
				break;
			default:
				break;
			}
		}
	}
}

void NGDrawPhase() {
	if (timer_tracker >= 0) {
		if (items[timer_tracker].timer <= 0) {
			if (timer_tracker_remaining_until_timeout > 0)
				timer_tracker_remaining_until_timeout--;
		}

		if (items[timer_tracker].timer > 0 || timer_tracker_remaining_until_timeout > 0) {
			char format_buffer[64];
			int current_timer = items[timer_tracker].timer;
			if (current_timer < 0)
				current_timer = 0;

			int remainder = (current_timer % 30) * (100 / 30);
			int seconds = current_timer / 30;

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

	// Timers
	NGDrawTimer(ng_local_timer, ng_local_timer_position, ng_local_timer_time_until_hide);
	NGDrawTimer(ng_global_timer, ng_global_timer_position, ng_global_timer_time_until_hide);
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

// Items

bool NGIsItemPerformingContinousAction(unsigned int item_num) {
	return NGGetItemHorizontalRotationRemaining(item_num) ||
		NGGetItemVerticalRotationRemaining(item_num) ||
		NGGetItemHorizontalMovementRemainingUnits(item_num) ||
		NGGetItemVerticalMovementRemainingUnits(item_num);
}

bool NGIsItemPerformingRotation(unsigned int item_num) {
	return NGGetItemHorizontalRotationRemaining(item_num) ||
		NGGetItemVerticalRotationRemaining(item_num);
}

bool NGIsItemPerformingMovement(unsigned int item_num) {
	return NGGetItemHorizontalMovementRemainingUnits(item_num) ||
		NGGetItemVerticalMovementRemainingUnits(item_num);
}

short NGGetItemHorizontalRotationSpeed(unsigned int item_num) {
	return ng_items_extradata[item_num].movement_info.horizontal_rotation_speed;
}

void NGSetItemHorizontalRotationSpeed(unsigned int item_num, short speed) {
	ng_items_extradata[item_num].movement_info.horizontal_rotation_speed = speed;
}

short NGGetItemVerticalRotationSpeed(unsigned int item_num) {
	return ng_items_extradata[item_num].movement_info.vertical_rotation_speed;
}

void NGSetItemVerticalRotationSpeed(unsigned int item_num, short speed) {
	ng_items_extradata[item_num].movement_info.vertical_rotation_speed = speed;
}

int NGGetItemHorizontalRotationRemaining(unsigned int item_num) {
	return ng_items_extradata[item_num].movement_info.horizontal_rotation_remaining;
}

extern void NGSetItemHorizontalRotationRemaining(unsigned int item_num, int remaining) {
	ng_items_extradata[item_num].movement_info.horizontal_rotation_remaining = remaining;
}

extern int NGGetItemVerticalRotationRemaining(unsigned int item_num) {
	return ng_items_extradata[item_num].movement_info.vertical_rotation_remaining;
}

extern void NGSetItemVerticalRotationRemaining(unsigned int item_num, int remaining) {
	ng_items_extradata[item_num].movement_info.vertical_rotation_remaining = remaining;
}

//
void NGSetItemHorizontalMovementAngle(unsigned int item_num, short angle) {
	ng_items_extradata[item_num].movement_info.move_horizontal_angle = angle;
}

short NGGetItemHorizontalMovementAngle(unsigned int item_num) {
	return ng_items_extradata[item_num].movement_info.move_horizontal_angle;
}

int NGGetItemHorizontalMovementRemainingUnits(unsigned int item_num) {
	return ng_items_extradata[item_num].movement_info.move_horizontal_remaining_units;
}

void NGSetItemHorizontalMovementRemainingUnits(unsigned int item_num, int units) {
	ng_items_extradata[item_num].movement_info.move_horizontal_remaining_units = units;
}

int NGGetItemVerticalMovementRemainingUnits(unsigned int item_num) {
	return ng_items_extradata[item_num].movement_info.move_vertical_remaining_units;
}

void NGSetItemVerticalMovementRemainingUnits(unsigned int item_num, int units) {
	ng_items_extradata[item_num].movement_info.move_vertical_remaining_units = units;
}

int NGGetItemHorizontalMovementRepeatUnits(unsigned int item_num) {
	return ng_items_extradata[item_num].movement_info.move_horizontal_repeat_units;
}

void NGSetItemHorizontalMovementRepeatUnits(unsigned int item_num, int units) {
	ng_items_extradata[item_num].movement_info.move_horizontal_repeat_units = units;
}

int NGGetItemVerticalMovementRepeatUnits(unsigned int item_num) {
	return ng_items_extradata[item_num].movement_info.move_vertical_repeat_units;
}

void NGSetItemVerticalMovementRepeatUnits(unsigned int item_num, int units) {
	ng_items_extradata[item_num].movement_info.move_vertical_repeat_units = units;
}

int NGGetItemHorizontalMovementSpeed(unsigned int item_num) {
	return ng_items_extradata[item_num].movement_info.horizontal_movement_speed;
}

void NGSetItemHorizontalMovementSpeed(unsigned int item_num, unsigned int movement_speed) {
	ng_items_extradata[item_num].movement_info.horizontal_movement_speed = movement_speed;
}

int NGGetItemVerticalMovementSpeed(unsigned int item_num) {
	return ng_items_extradata[item_num].movement_info.vertical_movement_speed;
}

void NGSetItemVerticalMovementSpeed(unsigned int item_num, unsigned int movement_speed) {
	ng_items_extradata[item_num].movement_info.vertical_movement_speed = movement_speed;
}

int NGGetItemMovementInProgressSound(unsigned int item_num) {
	return ng_items_extradata[item_num].movement_info.movement_in_progress_sound;
}

void NGSetItemMovementInProgressSound(unsigned int item_num, int sound_effect_id) {
	ng_items_extradata[item_num].movement_info.movement_in_progress_sound = sound_effect_id;
}

int NGGetItemMovementFinishedSound(unsigned int item_num) {
	return ng_items_extradata[item_num].movement_info.movement_finished_sound;
}

void NGSetItemMovementFinishedSound(unsigned int item_num, int sound_effect_id) {
	ng_items_extradata[item_num].movement_info.movement_finished_sound = sound_effect_id;
}

// Statics

bool NGIsStaticPerformingContinousAction(unsigned int static_num) {
	return NGGetStaticHorizontalRotationRemaining(static_num) ||
		NGGetStaticVerticalRotationRemaining(static_num) ||
		NGGetStaticHorizontalMovementRemainingUnits(static_num) ||
		NGGetStaticVerticalMovementRemainingUnits(static_num);
}

bool NGIsStaticPerformingRotation(unsigned int static_num) {
	return NGGetStaticHorizontalRotationRemaining(static_num) ||
		NGGetStaticVerticalRotationRemaining(static_num);
}

bool NGIsStaticPerformingMovement(unsigned int static_num) {
	return NGGetStaticHorizontalMovementRemainingUnits(static_num) ||
		NGGetStaticVerticalMovementRemainingUnits(static_num);
}

short NGGetStaticHorizontalRotationSpeed(unsigned int static_num) {
	return ng_statics_movement_info[static_num].horizontal_rotation_speed;
}

void NGSetStaticHorizontalRotationSpeed(unsigned int static_num, short speed) {
	ng_statics_movement_info[static_num].horizontal_rotation_speed = speed;
}

short NGGetStaticVerticalRotationSpeed(unsigned int static_num) {
	return ng_statics_movement_info[static_num].vertical_rotation_speed;
}

void NGSetStaticVerticalRotationSpeed(unsigned int static_num, short speed) {
	ng_statics_movement_info[static_num].vertical_rotation_speed = speed;
}

int NGGetStaticHorizontalRotationRemaining(unsigned int static_num) {
	return ng_statics_movement_info[static_num].horizontal_rotation_remaining;
}

extern void NGSetStaticHorizontalRotationRemaining(unsigned int static_num, int remaining) {
	ng_statics_movement_info[static_num].horizontal_rotation_remaining = remaining;
}

extern int NGGetStaticVerticalRotationRemaining(unsigned int static_num) {
	return ng_statics_movement_info[static_num].vertical_rotation_remaining;
}

extern void NGSetStaticVerticalRotationRemaining(unsigned int static_num, int remaining) {
	ng_statics_movement_info[static_num].vertical_rotation_remaining = remaining;
}

//
void NGSetStaticHorizontalMovementAngle(unsigned int static_num, short angle) {
	ng_statics_movement_info[static_num].move_horizontal_angle = angle;
}

short NGGetStaticHorizontalMovementAngle(unsigned int static_num) {
	return ng_statics_movement_info[static_num].move_horizontal_angle;
}

int NGGetStaticHorizontalMovementRemainingUnits(unsigned int static_num) {
	return ng_statics_movement_info[static_num].move_horizontal_remaining_units;
}

void NGSetStaticHorizontalMovementRemainingUnits(unsigned int static_num, int units) {
	ng_statics_movement_info[static_num].move_horizontal_remaining_units = units;
}

int NGGetStaticVerticalMovementRemainingUnits(unsigned int static_num) {
	return ng_statics_movement_info[static_num].move_vertical_remaining_units;
}

void NGSetStaticVerticalMovementRemainingUnits(unsigned int static_num, int units) {
	ng_statics_movement_info[static_num].move_vertical_remaining_units = units;
}

int NGGetStaticHorizontalMovementRepeatUnits(unsigned int static_num) {
	return ng_statics_movement_info[static_num].move_horizontal_repeat_units;
}

void NGSetStaticHorizontalMovementRepeatUnits(unsigned int static_num, int units) {
	ng_statics_movement_info[static_num].move_horizontal_repeat_units = units;
}

int NGGetStaticVerticalMovementRepeatUnits(unsigned int static_num) {
	return ng_statics_movement_info[static_num].move_vertical_repeat_units;
}

void NGSetStaticVerticalMovementRepeatUnits(unsigned int static_num, int units) {
	ng_statics_movement_info[static_num].move_vertical_repeat_units = units;
}

int NGGetStaticHorizontalMovementSpeed(unsigned int static_num) {
	return ng_statics_movement_info[static_num].horizontal_movement_speed;
}

void NGSetStaticHorizontalMovementSpeed(unsigned int static_num, unsigned int movement_speed) {
	ng_statics_movement_info[static_num].horizontal_movement_speed = movement_speed;
}

int NGGetStaticVerticalMovementSpeed(unsigned int static_num) {
	return ng_statics_movement_info[static_num].vertical_movement_speed;
}

void NGSetStaticVerticalMovementSpeed(unsigned int static_num, unsigned int movement_speed) {
	ng_statics_movement_info[static_num].vertical_movement_speed = movement_speed;
}

int NGGetStaticMovementInProgressSound(unsigned int static_num) {
	return ng_statics_movement_info[static_num].movement_in_progress_sound;
}

void NGSetStaticMovementInProgressSound(unsigned int static_num, int sound_effect_id) {
	ng_statics_movement_info[static_num].movement_in_progress_sound = sound_effect_id;
}

int NGGetStaticMovementFinishedSound(unsigned int static_num) {
	return ng_statics_movement_info[static_num].movement_finished_sound;
}

void NGSetStaticMovementFinishedSound(unsigned int static_num, int sound_effect_id) {
	ng_statics_movement_info[static_num].movement_finished_sound = sound_effect_id;
}

//

bool NGIsItemCollisionDisabled(unsigned int item_num) {
	return ng_items_extradata[item_num].collison_disabled;
}

void NGDisableItemCollision(unsigned int item_num) {
	ng_items_extradata[item_num].collison_disabled = true;
}

void NGEnableItemCollision(unsigned int item_num) {
	ng_items_extradata[item_num].collison_disabled = false;
}

void NGToggleItemMeshVisibilityMaskBit(unsigned int item_num, unsigned int mask_bit, bool enabled) {
	if (enabled)
		ng_items_extradata[item_num].mesh_visibility_mask |= (1 << mask_bit);
	else
		ng_items_extradata[item_num].mesh_visibility_mask &= ~(1 << mask_bit);
}

unsigned int NGGetItemMeshVisibilityMask(unsigned int item_num) {
	return ng_items_extradata[item_num].mesh_visibility_mask;
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

void NGToggleOrganizer(int organizer_id, bool is_enabled) {
	ng_organizer_states[organizer_id].is_enabled = is_enabled;
}

bool NGIsOrganizerEnabled(int organizer_id) {
	return ng_organizer_states[organizer_id].is_enabled;
}

void NGResetOrganizer(int organizer_id) {
	ng_organizer_states[organizer_id].current_tick = 0;
}

extern bool NGIsTriggerGroupContinuous(int trigger_group_id) {
	return ng_trigger_group_states[trigger_group_id].continuous;
}

extern void NGSetTriggerGroupContinuous(int trigger_group_id, bool is_continuous) {
	ng_trigger_group_states[trigger_group_id].continuous = is_continuous;
}

void NGSetDisplayTimerForMoveableWithType(int item_id, NGTimerTrackerType new_timer_tracker_type) {
	timer_tracker = item_id;
	timer_tracker_type = new_timer_tracker_type;
#if 0
	if (items[timer_tracker].timer > 0) {
		timer_tracker_remaining_until_timeout = 0;
	} else {
		timer_tracker_remaining_until_timeout = TIMER_TRACKER_TIMEOUT;
	}
#endif
}

extern void NGSetAfterDeathOverride(int item_id, short after_death) {
	ng_items_extradata[item_id].after_death_override = after_death;
}

extern short NGGetAfterDeathOverride(int item_id) {
	return ng_items_extradata[item_id].after_death_override;
}

int ng_draw_item_number = NO_ITEM;

void NGSetCurrentDrawItemNumber(int item_num) {
	ng_draw_item_number = item_num;
}

int NGGetCurrentDrawItemNumber() {
	return ng_draw_item_number;
}

void NGUpdateFlipeffectFloorstateData(bool heavy) {
	int index = ng_room_offset_table[ng_current_trigger_state.room] + ng_current_trigger_state.index;

	if (heavy) {
		ng_heavy_current_flipeffect_floor_trigger = index;
		ng_heavy_last_flipeffect_floor_trigger = index;
	} else {
		ng_current_flipeffect_floor_trigger = index;
		ng_last_flipeffect_floor_trigger = index;
	}
}

void NGUpdateActionFloorstateData(bool heavy) {
	int index = ng_room_offset_table[ng_current_trigger_state.room] + ng_current_trigger_state.index;

	if (heavy) {
		ng_heavy_current_action_floor_trigger = index;
		ng_heavy_last_action_floor_trigger = index;
	} else {
		ng_current_action_floor_trigger = index;
		ng_last_action_floor_trigger = index;
	}
}

void NGUpdateFlipeffectOneshot() {
	int index = ng_room_offset_table[ng_current_trigger_state.room] + ng_current_trigger_state.index;
	ng_flipeffect_oneshot_floorstate[index] = true;
}

void NGUpdateActionOneshot() {
	int index = ng_room_offset_table[ng_current_trigger_state.room] + ng_current_trigger_state.index;
	ng_action_oneshot_floorstate[index] = true;
}

void NGSetupExtraState() {
	ng_lara_extrastate.TightRopeFall = 0;
	ng_lara_extrastate.TightRopeOff = 0;
	ng_lara_extrastate.TightRopeOnCount = 0;

	ng_draw_item_number = NO_ITEM;
	ng_lara_infinite_air = false;

	// Animation
	ng_animation_current_animation = -1;
	ng_animation_prev_hands_state = LG_NO_ARMS;
	ng_animation_target_item = -1;
	ng_animation_target_test_position = -1;

	// Timers
	ng_global_timer = 0;
	ng_global_timer_frame_increment = 0;
	ng_global_timer_position = NG_TIMER_POSITION_INVISIBLE;
	ng_global_timer_time_until_hide = 0;

	ng_local_timer = 0;
	ng_local_timer_frame_increment = 0;
	ng_local_timer_position = NG_TIMER_POSITION_INVISIBLE;
	ng_local_timer_time_until_hide = 0;

	// Level
	pending_level_load_timer = -1;
	pending_level_load_id = 0;

	// Variables
	ng_current_value = 0;
	ng_global_alfa = 0;
	ng_global_beta = 0;
	ng_global_delta = 0;
	ng_local_alfa = 0;
	ng_local_beta = 0;
	ng_local_delta = 0;
	ng_last_input_number = 0;

	// Timer Trackers
	timer_tracker_type = TTT_ONLY_SHOW_SECONDS;
	timer_tracker = -1;
	timer_tracker_remaining_until_timeout = 0;

	// Items
	ng_items_extradata = (NG_ITEM_EXTRADATA*)game_malloc(ITEM_COUNT * sizeof(NG_ITEM_EXTRADATA));
	for (int i = 0; i < ITEM_COUNT; i++) {
		NGResetItemExtraData(i);
	}

	// Statics
	ng_statics_movement_info = (NG_MOVEMENT_INFO*)game_malloc(NG_STATIC_ID_TABLE_SIZE * sizeof(NG_MOVEMENT_INFO));
	for (int i = 0; i < NG_STATIC_ID_TABLE_SIZE; i++) {
		NGResetMovementInfo(&ng_statics_movement_info[i]);
	}

	// Records
	NG_LEVEL_RECORD_DATA *current_record_data = ng_levels[gfCurrentLevel].records;

	// Global triggers
	{
		for (int i = 0; i < MAX_NG_GLOBAL_TRIGGERS; i++) {
			ng_global_trigger_states[i].is_disabled = false;
			ng_global_trigger_states[i].is_halted = false;
		}

		if (current_record_data) {
			int global_trigger_count = current_record_data->global_trigger_count;
			for (int i = 0; i < global_trigger_count; i++) {
				NG_GLOBAL_TRIGGER* global_trigger = &current_record_data->global_trigger_table[i].record;
				int record_id = current_record_data->global_trigger_table[i].record_id;
				if (global_trigger->flags != 0xffff) {
					// FGT_DISABLED
					if (global_trigger->flags & 0x0008) {
						ng_global_trigger_states[record_id].is_disabled = true;
					}
				}
			}
		}
	}

	// Trigger groups
	{
		for (int i = 0; i < MAX_NG_TRIGGER_GROUPS; i++) {
			ng_trigger_group_states[i].continuous = false;
			ng_trigger_group_states[i].one_shot = false;
		}
	}

	// Organizers
	{
		for (int i = 0; i < MAX_NG_ORGANIZERS; i++) {
			ng_organizer_states[i].is_enabled = false;
			ng_organizer_states[i].current_tick = 0;
		}

		if (current_record_data) {
			int organizer_count = current_record_data->organizer_count;
			for (int i = 0; i < organizer_count; i++) {
				NG_ORGANIZER* organizer = &current_record_data->organizer_table[i].record;
				int record_id = current_record_data->organizer_table[i].record_id;
				if (organizer->flags != 0xffff) {
					// FO_ENABLED
					if (organizer->flags & 0x0001) {
						ng_organizer_states[record_id].is_enabled = true;
					}
				}
			}
		}
	}

	// Cinema
	ng_cinema_timer = -1;
	ng_cinema_type = 0;

	// Floorstate
	ng_last_flipeffect_floor_trigger = -1;
	ng_current_flipeffect_floor_trigger = -1;
	ng_last_action_floor_trigger = -1;
	ng_current_action_floor_trigger = -1;
	ng_heavy_last_flipeffect_floor_trigger = -1;
	ng_heavy_current_flipeffect_floor_trigger = -1;
	ng_heavy_last_action_floor_trigger = -1;
	ng_heavy_current_action_floor_trigger = -1;

	ng_floorstate_data_size = 0;
	for (int i = 0; i < number_rooms; i++) {
		ng_room_offset_table[i] = ng_floorstate_data_size;
		ng_floorstate_data_size += room[i].x_size * room[i].y_size;
	}
	ng_flipeffect_oneshot_floorstate = (char*)game_malloc(ng_floorstate_data_size);
	memset(ng_flipeffect_oneshot_floorstate, 0x00, ng_floorstate_data_size);

	ng_action_oneshot_floorstate = (char*)game_malloc(ng_floorstate_data_size);
	memset(ng_action_oneshot_floorstate, 0x00, ng_floorstate_data_size);

	// Input lock and simulator
	memset(ng_input_lock_timers, 0x00, sizeof(ng_input_lock_timers));
	memset(ng_input_simulate_timers, 0x00, sizeof(ng_input_simulate_timers));

	// Looped samples
	memset(ng_looped_sound_state, 0x00, NumSamples * sizeof(int));

	ng_found_item_index = -1;
	ng_lara_moveable_collision_size = 0;
	ng_lara_static_collision_size = 0;

	// Damage
	lara_damage_resistence = 1000;

	// Triggerstate
	ng_current_trigger_state.room = -1;
	ng_current_trigger_state.index = -1;
	ng_backup_trigger_state_count = 0;
}

void NGFrameFinishExtraState() {
	// Lara
	ng_last_flipeffect_floor_trigger = ng_current_flipeffect_floor_trigger;
	ng_current_flipeffect_floor_trigger = -1;

	ng_last_action_floor_trigger = ng_current_action_floor_trigger;
	ng_current_action_floor_trigger = -1;

	// Heavy
	ng_heavy_last_flipeffect_floor_trigger = ng_heavy_current_flipeffect_floor_trigger;
	ng_heavy_current_flipeffect_floor_trigger = -1;

	ng_heavy_last_action_floor_trigger = ng_heavy_current_action_floor_trigger;
	ng_heavy_current_action_floor_trigger = -1;
}
