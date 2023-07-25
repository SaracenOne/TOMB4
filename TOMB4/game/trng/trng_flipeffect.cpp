#include "../../tomb4/pch.h"

#include "trng.h"
#include "trng_extra_state.h"
#include "../../specific/function_stubs.h"
#include "../control.h"
#include "../effects.h"
#include "../objects.h"
#include "../lara.h"

// FlipEffects

// NGLE - 51
void disable_input_for_time(unsigned char input, unsigned char timer) {
	if (!NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame()) {
		NGDisableInputForTime(input, (int)timer * 30);
	}
}

// NGLE - 68

void play_cd_track_channel_1(unsigned char track_id, unsigned char looping) {
	if (!NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame()) {
		TriggerChannelTrack(track_id, 0, looping);
	}
}


// NGLE - 77
void force_lara_animation_0_255_of_slot_animation(unsigned char animation_index, unsigned char object_id) {
	if (!NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame()) {
		int animation_index_offset = objects[object_id].anim_index + animation_index;

		lara_item->anim_number = animation_index_offset;
		lara_item->frame_number = anims[animation_index_offset].frame_base;
	}
}

// NGLE - 80
void force_lara_animation_256_512_of_slot_animation(unsigned char animation_index, unsigned char object_id) {
	if (!NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame()) {
		int animation_index_offset = objects[object_id].anim_index + animation_index + 256;

		lara_item->anim_number = animation_index_offset;
		lara_item->frame_number = anims[animation_index_offset].frame_base;
	}
}

// NGLE - 83

void remove_weapons_or_flares_from_laras_hands(unsigned char unused1, unsigned char unused2) {
	lara.request_gun_type = WEAPON_NONE;
}

// NGLE - 96
void disarm_lara(unsigned char remove_weapons_only, unsigned char _unusued) {
	if (!NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame()) {
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
	}
}

// NGLE - 129
void play_cd_track_channel_2(unsigned char track_id, unsigned char looping) {
	if (!NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame()) {
		TriggerChannelTrack(track_id, 1, looping);
	}
}

// NGLE - 133
void set_volume_for_audio_track_on_channel(unsigned char volume, unsigned char channel) {
	if (!NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame()) {

	}
}

// NGLE - 407
void set_lara_holsters(unsigned int holster_type, unsigned char unused) {
	if (!NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame()) {
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
	}
}

void camera_show_black_screen_for_seconds_with_final_curtain_effect(unsigned char timer, unsigned char unused) {
	if (!NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame()) {
		NGSetCurtainTimer(timer * 30);
	}
}

void camera_set_cinema_effect_type_for_seconds(unsigned char action_data_1, unsigned char action_data_2) {
	if (!NGIsOneShotTriggeredForTile() && !NGCheckFloorStatePressedThisFrameOrLastFrame()) {
		NGSetCinemaTypeAndTimer(action_data_1, action_data_2 * 30);
	}
}

void NGFlipEffect(unsigned short param, short extra, bool oneshot) {
	char action_data_1 = (char)extra & 0xff;
	char action_data_2 = (char)(extra >> 8) & 0xff;

	switch (param) {
		case DISABLE_INPUT_FOR_TIME: {
			disable_input_for_time(action_data_1, action_data_2);
			break;
		}
		case PLAY_CD_TRACK_ON_CHANNEL_1: {
			play_cd_track_channel_1(action_data_1, action_data_2);
			break;
		}
		case REMOVE_WEAPONS_OR_FLARES_FROM_LARAS_HANDS: {
			remove_weapons_or_flares_from_laras_hands(action_data_1, action_data_2);
		}
		case FORCE_LARA_ANIMATION_0_255_OF_SLOT_ANIMATION: {
			force_lara_animation_0_255_of_slot_animation(action_data_1, action_data_2);
			break;
		}
		case FORCE_LARA_ANIMATION_256_512_OF_SLOT_ANIMATION: {
			force_lara_animation_256_512_of_slot_animation(action_data_1, action_data_2);
			break;
		}
		case DISARM_LARA: {
			disarm_lara(action_data_1, action_data_2);
			break;
		}
		case PLAY_CD_TRACK_ON_CHANNEL_2: {
			play_cd_track_channel_2(action_data_1, action_data_2);
			break;
		}
		case SET_VOLUME_OF_AUDIO_TRACK_ON_CHANNEL: {
			set_volume_for_audio_track_on_channel(action_data_1, action_data_2);
			break;
		}
		case SET_LARA_HOLSTER_TYPE: {
			set_lara_holsters(action_data_1, action_data_2);
			break;
		}
		case CAMERA_SHOW_BLACK_SCREEN_FOR_SECONDS_WITH_FINAL_CURTAIN_EFFECT: {
			camera_show_black_screen_for_seconds_with_final_curtain_effect(action_data_1, action_data_2);
			break;
		}
		case CAMERA_SET_CINEMA_EFFECT_TYPE_FOR_SECONDS: {
			camera_set_cinema_effect_type_for_seconds(action_data_1, action_data_2);
			break;
		}
		default: {
			printf("Unimplemented NGFlipEffect %u\n", param);
			return;
		}
	}
}