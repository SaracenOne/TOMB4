#include "../../tomb4/pch.h"

#include "trng.h"
#include "trng_extra_state.h"
#include "trng_flipeffect.h"
#include "trng_script_parser.h"
#include "../../specific/function_stubs.h"
#include "../control.h"
#include "../effects.h"
#include "../objects.h"
#include "../lara.h"
#include "../traps.h"

// FlipEffects

// NGLE - 51
bool disable_input_for_time(unsigned char input, unsigned char timer) {
	NGDisableInputForTime(input, (int)timer * 30);
	
	return true;
}

// NGLE - 52
bool enable_input(unsigned char input, unsigned char unused) {
	NGEnableInput(input);
	
	return true;
}

// NGLE - 63
bool kill_and_or_set_lara_on_fire(unsigned char death_type, unsigned char action_data_2) {
	switch (death_type) {
		case 0x00: // Zero Lara hitpoints
			lara_item->hit_points = 0;
			lara_item->hit_status = 1;
			break;
		case 0x01: // Zero Lara hitpoints and set on fire
			lara_item->hit_points = 0;
			lara_item->hit_status = 1;
			LaraBurn();
			break;
		case 0x02: // Just set Lara on fire (With the original executable, it sometimes doesn't execute. TRNG bug?)
			LaraBurn();
			break;
	}
	return true;
}

// NGLE - 68
bool play_cd_track_channel_1(unsigned char track_id, unsigned char looping) {
	TriggerChannelTrack(track_id, 0, looping);

	return true;
}


// NGLE - 77
bool force_lara_animation_0_255_of_slot_animation(unsigned char animation_index, unsigned char object_id) {
	int animation_index_offset = objects[object_id].anim_index + animation_index;

	lara_item->anim_number = animation_index_offset;
	lara_item->frame_number = anims[animation_index_offset].frame_base;

	return true;
}

// NGLE - 80
bool force_lara_animation_256_512_of_slot_animation(unsigned char animation_index, unsigned char object_id) {
	int animation_index_offset = objects[object_id].anim_index + animation_index + 256;

	lara_item->anim_number = animation_index_offset;
	lara_item->frame_number = anims[animation_index_offset].frame_base;

	return true;
}

// NGLE - 83
bool remove_weapons_or_flares_from_laras_hands(unsigned char unused1, unsigned char unused2) {
	lara.request_gun_type = WEAPON_NONE;

	return true;
}

// NGLE - 96
bool disarm_lara(unsigned char remove_weapons_only, unsigned char _unusued) {
	lara.request_gun_type = WEAPON_NONE;
	lara.last_gun_type = WEAPON_NONE;
	lara.gun_status = LG_NO_ARMS;

	lara.pistols_type_carried = W_NONE;
	lara.uzis_type_carried = W_NONE;
	lara.shotgun_type_carried = W_NONE;
	lara.crossbow_type_carried = W_NONE;
	lara.grenade_type_carried = W_NONE;
	lara.sixshooter_type_carried = W_NONE;

	lara.weapon_item = NO_ITEM;

	lara.back_gun = 0;
	lara.holster = lara.holster != LARA ? LARA_HOLSTERS : LARA;

	lara.mesh_ptrs[LM_RHAND] = meshes[objects[LARA].mesh_index + LM_RHAND * 2];
	lara.mesh_ptrs[LM_LHAND] = meshes[objects[LARA].mesh_index + LM_LHAND * 2];

	lara.left_arm.frame_number = 0;
	lara.right_arm.frame_number = 0;
	lara.target = 0;
	lara.right_arm.lock = 0;
	lara.left_arm.lock = 0;

	if (remove_weapons_only != 1) {
		lara.num_uzi_ammo = 0;
		lara.num_revolver_ammo = 0;
		lara.num_shotgun_ammo1 = 0;
		lara.num_shotgun_ammo2 = 0;
		lara.num_grenade_ammo1 = 0;
		lara.num_grenade_ammo2 = 0;
		lara.num_grenade_ammo3 = 0;
		lara.num_crossbow_ammo1 = 0;
		lara.num_crossbow_ammo2 = 0;
		lara.num_crossbow_ammo3 = 0;
	}
	return true;
}

// NGLE - 118
bool perform_triggergroup_from_script_in_specific_way(unsigned char trigger_group_id, unsigned char execution_type) {
	if (execution_type != 1) {
		printf("Unimplemented execution type!\n");
		return false;
	}

	NG_TRIGGER_GROUP& trigger_group = current_trigger_groups[trigger_group_id];
	int index = 0;
	bool result;
	while (index < NG_TRIGGER_GROUP_DATA_SIZE) {
		// Flipeffect
		if (trigger_group.data[index].first_field == 0x2000) {
			result = NGFlipEffect(trigger_group.data[index].second_field, trigger_group.data[index].third_field, false, false);
		}
		// ActionNG
		else if (trigger_group.data[index].first_field == 0x5000) {
			NGActionTrigger(trigger_group.data[index].second_field, trigger_group.data[index].third_field);
			result = true; //TODO: check for conditions on ActionNG triggers
		}
		// End
		else if (trigger_group.data[index].first_field == 0x0000) {
			result = true;
			break;
		}
		else {
			printf("Unknown triggergroup command!\n");
			result = false;
			break;
		}
		index++;
	}

	return result;
}

// NGLE - 129
bool play_cd_track_channel_2(unsigned char track_id, unsigned char looping) {
	TriggerChannelTrack(track_id, 1, looping);

	return true;
}

// NGLE - 133
bool set_volume_for_audio_track_on_channel(unsigned char volume, unsigned char channel) {
	return true;
}

// NGLE - 407
bool set_lara_holsters(unsigned int holster_type, unsigned char unused) {
	switch (holster_type) {
		case 0x0d: {
			lara.holster = LARA_HOLSTERS;
			break;
		};
		case 0x0e: {
			lara.holster = LARA_HOLSTERS_PISTOLS;
			break;
		};
		case 0x10: {
			lara.holster = LARA_HOLSTERS_SIXSHOOTER;
			break;
		};
		case 0x0f: {
			lara.holster = LARA_HOLSTERS_UZIS;
			break;
		};
		case 0x00: {
			lara.holster = LARA;
			break;
		};
		default: {
			lara.holster = holster_type;
		}
	}
	return true;
}

// NGLE - 367
bool camera_show_black_screen_for_seconds_with_final_curtain_effect(unsigned char timer, unsigned char unused) {
	NGSetCurtainTimer(timer * 30);

	return true;
}

// NGLE - 369
bool camera_set_cinema_effect_type_for_seconds(unsigned char action_data_1, unsigned char action_data_2) {
	NGSetCinemaTypeAndTimer(action_data_1, action_data_2 * 30);

	return true;
}

bool NGFlipEffect(unsigned short param, short extra, bool oneshot, bool skip_checks) {
	char action_data_1 = (char)extra & 0xff;
	char action_data_2 = (char)(extra >> 8) & 0xff;

	switch (param) {
		case DISABLE_INPUT_FOR_TIME: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame())
				return disable_input_for_time(action_data_1, action_data_2);
			break;
		}
		case ENABLE_INPUT: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame())
				return enable_input(action_data_1, action_data_2);
			break;
		}
		case KILL_AND_OR_SET_LARA_ON_FIRE: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame())
				return kill_and_or_set_lara_on_fire(action_data_1, action_data_2);
			break;
		}
		case PLAY_CD_TRACK_ON_CHANNEL_1: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame())
				return play_cd_track_channel_1(action_data_1, action_data_2);
			break;
		}
		case FORCE_LARA_ANIMATION_0_255_OF_SLOT_ANIMATION: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame())
				return force_lara_animation_0_255_of_slot_animation(action_data_1, action_data_2);
			break;
		}
		case FORCE_LARA_ANIMATION_256_512_OF_SLOT_ANIMATION: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame())
				return force_lara_animation_256_512_of_slot_animation(action_data_1, action_data_2);
			break;
		}
		case REMOVE_WEAPONS_OR_FLARES_FROM_LARAS_HANDS: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame())
				return remove_weapons_or_flares_from_laras_hands(action_data_1, action_data_2);
			break;
		}
		case DISARM_LARA: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame())
				return disarm_lara(action_data_1, action_data_2);
			break;
		}
		case PERFORM_TRIGGERGROUP_FROM_SCRIPT_IN_SPECIFIC_WAY:
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame())
				return perform_triggergroup_from_script_in_specific_way(action_data_1, action_data_2);
			break;
		case PLAY_CD_TRACK_ON_CHANNEL_2: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame())
				return play_cd_track_channel_2(action_data_1, action_data_2);
			break;
		}
		case SET_VOLUME_OF_AUDIO_TRACK_ON_CHANNEL: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame())
				return set_volume_for_audio_track_on_channel(action_data_1, action_data_2);
			break;
		}
		case SET_LARA_HOLSTER_TYPE: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame())
				return set_lara_holsters(action_data_1, action_data_2);
			break;
		}
		case CAMERA_SHOW_BLACK_SCREEN_FOR_SECONDS_WITH_FINAL_CURTAIN_EFFECT: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame())
				return camera_show_black_screen_for_seconds_with_final_curtain_effect(action_data_1, action_data_2);
			break;
		}
		case CAMERA_SET_CINEMA_EFFECT_TYPE_FOR_SECONDS: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame())
				return camera_set_cinema_effect_type_for_seconds(action_data_1, action_data_2);
			break;
		}
		default: {
			printf("Unimplemented NGFlipEffect %u\n", param);
			return false;
		}
	}

	return false;
}