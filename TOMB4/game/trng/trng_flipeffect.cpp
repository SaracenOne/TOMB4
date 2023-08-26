#include "../../tomb4/pch.h"

#include "../../specific/function_stubs.h"
#include "../../specific/audio.h"
#include "../control.h"
#include "../debris.h"
#include "../effects.h"
#include "../effect2.h"
#include "../objects.h"
#include "../lara.h"
#include "../traps.h"
#include "../../specific/file.h"
#include "../camera.h"
#include "../sound.h"
#include "../../specific/dxsound.h"

#include "trng.h"
#include "trng_action.h"
#include "trng_arithmetic.h"
#include "trng_extra_state.h"
#include "trng_flipeffect.h"
#include "trng_script_parser.h"
#include "trng_triggergroup.h"

bool NGTriggerItemGroupWithTimer(unsigned char item_group, unsigned char timer, bool anti) {
	NG_ITEM_GROUP current_item_group = current_item_groups[item_group];
	int index = 0;
	for (int i = 0; i < NG_ITEM_GROUP_MAX_LIST; i++) {
		int current_script_item = current_item_group.item_list[i];
		if (current_script_item < 0) {
			break;
		}
		int current_item = ng_script_id_table[current_script_item];

		items[current_item].timer = ((short)timer) * 30;
		NGItemActivator(current_item, anti);

		index++;
	}
	return true;
}

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
	S_CDPlayExt(track_id, 0, looping, false);

	return true;
}

// NGLE - 69
bool stop_all_cd_tracks(unsigned char unused_1, unsigned char unused_2) {
	S_CDStopExt(0);
	S_CDStopExt(1);

	return true;
}

// NGLE - 70
bool play_sound_from_first_group(unsigned char sound_sample, unsigned char timer) {
	int indexed_sound_sample = sound_sample;

	if (timer == 0) {
		// INFINITE LOOP
		ng_looped_sound_state[indexed_sound_sample] = -1;
	} else if (timer == 0x1f) {
		// ONESHOT
		ng_looped_sound_state[indexed_sound_sample] = 0;
	} else {
		ng_looped_sound_state[indexed_sound_sample] = timer * 30;
	}

	SoundEffect(indexed_sound_sample, NULL, SFX_ALWAYS);

	return true;
}

// NGLE - 71
bool play_sound_from_second_group(unsigned char sound_sample, unsigned char timer) {
	int indexed_sound_sample = 256 + sound_sample;

	if (timer == 0) {
		// INFINITE LOOP
		ng_looped_sound_state[indexed_sound_sample] = -1;
	} else if (timer == 0x1f) {
		// ONESHOT
		ng_looped_sound_state[indexed_sound_sample] = 0;
	} else {
		ng_looped_sound_state[indexed_sound_sample] = timer * 30;
	}

	SoundEffect(indexed_sound_sample, NULL, SFX_ALWAYS);

	return true;
}

// NGLE - 72
bool stop_sound_from_first_group(unsigned char sound_sample, unsigned char unused_2) {
	int indexed_sound_sample = sound_sample;

	StopSoundEffect(indexed_sound_sample);
	ng_looped_sound_state[indexed_sound_sample] = 0;

	return true;
}

// NGLE - 73
bool stop_sound_from_second_group(unsigned char sound_sample, unsigned char unused_2) {
	int indexed_sound_sample = 256 + sound_sample;

	StopSoundEffect(indexed_sound_sample);
	ng_looped_sound_state[indexed_sound_sample] = 0;

	return true;
}

// NGLE - 74
bool stop_all_sound_samples(unsigned char unused_1, unsigned char unused_2) {
	S_SoundStopAllSamples();

	memset(ng_looped_sound_state, 0x00, NumSamples * sizeof(int));

	return true;
}

// NGLE - 77
bool force_lara_animation_0_255_of_slot_animation(unsigned char animation_index, unsigned char object_id) {
	int animation_index_offset = objects[object_id].anim_index + animation_index;

	lara_item->anim_number = animation_index_offset;
	lara_item->frame_number = anims[animation_index_offset].frame_base;

	return true;
}

// NGLE - 79
bool move_lara_to_lara_start_pos_in_x_way(unsigned char ocb, unsigned char teleport_type) {
	int lara_start_pos_id = NGFindIndexForLaraStartPosWithMatchingOCB(ocb);
	if (lara_start_pos_id >= 0) {
		AIOBJECT* ai = &AIObjects[lara_start_pos_id];
		if (ai) {
			if (teleport_type == 1) {
				// Keep sector displacement
				lara_item->pos.x_pos = (ai->x & ~0x3ff) | (lara_item->pos.x_pos & 0x3ff);
				int lara_y_offset =  lara_item->floor - lara_item->pos.y_pos;
				lara_item->pos.z_pos = (ai->z & ~0x3ff) | (lara_item->pos.z_pos & 0x3ff);

				FLOOR_INFO* ai_floor_info = GetFloor(lara_item->pos.x_pos, ai->y, lara_item->pos.z_pos, &ai->room_number);
				lara_item->pos.y_pos = GetHeight(ai_floor_info, lara_item->pos.x_pos, ai->y, lara_item->pos.z_pos) - lara_y_offset;

				camera.fixed_camera = 1;

				lara_item->room_number = ai->room_number;

				return true;
			} else {
				// Warp directly
				lara_item->pos.x_pos = ai->x;
				lara_item->pos.y_pos = ai->y;
				lara_item->pos.z_pos = ai->z;
				lara_item->pos.y_rot = ai->y_rot;
				lara_item->room_number = ai->room_number;

				camera.fixed_camera = 1;

				return true;
			}
		}
	}

	return false;
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

// NGLE - 90
bool recharge_lara_life_by_percentage(unsigned char action_data_1, unsigned char unused2) {
	const int MAX_LARA_HEALTH = 1000; // May need this to be customizable.

	if (action_data_1 <= 9) {
		int health_multiple = (MAX_LARA_HEALTH / 1000);
		lara_item->hit_points += (int)(health_multiple * (action_data_1 + 1));
	} else if (action_data_1 <= 18) {
		int health_multiple = (MAX_LARA_HEALTH / 100);
		lara_item->hit_points += (int)(health_multiple * (action_data_1 - 8));
	} else {
		int health_multiple = (MAX_LARA_HEALTH / 10);
		lara_item->hit_points += (int)(health_multiple * (action_data_1 - 17));
	}

	if (lara_item->hit_points > MAX_LARA_HEALTH)
		lara_item->hit_points = MAX_LARA_HEALTH;

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

// NGLE - 104
bool lara_toggle_infinite_air(unsigned char enabled, unsigned char _unused) {
	ng_lara_infinite_air = enabled;

	return true;
}

// NGLE - 115
bool set_room_type(unsigned char room_number, unsigned char room_type) {
	ROOM_INFO* r = &room[room_number];
	if (r) {
		switch (room_type) {
			case 0: {
				r->flags |= ROOM_UNDERWATER;
				break;
			}
			case 2: {
				r->flags |= ROOM_SKYBOX;
				break;
			}
			case 4: {
				r->flags |= 0x10;
				break;
			}
			case 5: {
				r->flags |= ROOM_NOT_INSIDE; // TODO: check this
				break;
			}
			case 10: {
				r->flags |= ROOM_SNOW;
				break;
			}
			case 11: {
				r->flags |= ROOM_RAIN;
				break;
			}
			case 12: {
				r->flags |= ROOM_COLD;
				break;
			}
			default: {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "SET_ROOM_TYPE: unsupported room type!");
				return false;
			}
		}
	}

	return true;
}

// NGLE - 116
bool remove_room_type(unsigned char room_number, unsigned char room_type) {
	ROOM_INFO* r = &room[room_number];
	if (r) {
		switch (room_type) {
			case 0: {
				r->flags &= ~ROOM_UNDERWATER;
				break;
			}
			case 2: {
				r->flags &= ~ROOM_SKYBOX;
				break;
			}
			case 4: {
				r->flags &= ~0x10;
				break;
			}
			case 5: {
				r->flags &= ~ROOM_NOT_INSIDE; // TODO: check this
				break;
			}
			case 10: {
				r->flags &= ~ROOM_SNOW;
				break;
			}
			case 11: {
				r->flags &= ~ROOM_RAIN;
				break;
			}
			case 12: {
				r->flags &= ~ROOM_COLD;
				break;
			}
			default: {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "REMOVE_ROOM_TYPE: unsupported room type!");
				return false;
			}
		}
	}

	return true;
}

// NGLE - 118
bool perform_triggergroup_from_script_in_specific_way(unsigned char trigger_group_id, unsigned char execution_type) {
	return NGTriggerGroupFunction(trigger_group_id, execution_type);
}

// NGLE - 125
bool flipmap_on(unsigned char flipmap_id, unsigned char unused_2) {
	NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "FLIPMAP_ON not yet implemented");

	return false;
}

// NGLE - 126
bool flipmap_off(unsigned char flipmap_id, unsigned char unused_2) {
	NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "FLIPMAP_OFF not yet implemented");

	return false;
}

// NGLE - 127
bool organizer_enable(unsigned char organizer_id_lower, unsigned char organizer_id_upper) {
	unsigned short organizer_id = ((short)organizer_id_upper << 8) | (short)organizer_id_lower;
	NGToggleOrganizer(organizer_id, true);

	return true;
}

// NGLE - 128
bool organizer_disable(unsigned char organizer_id_lower, unsigned char organizer_id_upper) {
	unsigned short organizer_id = ((short)organizer_id_upper << 8) | (short)organizer_id_lower;
	NGToggleOrganizer(organizer_id, false);

	return true;
}

// NGLE - 129
bool play_cd_track_channel_2(unsigned char track_id, unsigned char looping) {
	S_CDPlayExt(track_id, 1, looping, false);

	return true;
}

// NGLE - 130
bool stop_cd_track_on_channel(unsigned char channel_id, unsigned char unused) {
	S_CDStopExt(channel_id);

	return true;
}


// NGLE - 133
bool set_volume_for_audio_track_on_channel(unsigned char volume, unsigned char channel) {
	S_CDSetChannelVolume(volume, channel);
	return true;
}

// NGLE - 145
bool activate_item_group_with_timer(unsigned char item_group, unsigned char timer) {
	return NGTriggerItemGroupWithTimer(item_group, timer, false);
}

// NGLE - 146
bool untrigger_item_group_with_timer(unsigned char item_group, unsigned char timer) {
	return NGTriggerItemGroupWithTimer(item_group, timer, true);
}

// NGLE - 160
bool static_shatter(unsigned char static_id_lower, unsigned char static_id_upper) {
	unsigned short static_id = ((short)static_id_upper << 8) | (short)static_id_lower;

	NGStaticTableEntry* entry = &ng_static_id_table[static_id];
	int room_number = entry->room_index;
	if (room_number >= 0 && room_number < number_rooms) {
		MESH_INFO* mesh = &room[room_number].mesh[entry->mesh_id];

		PHD_3DPOS pos;
		pos.x_pos = mesh->x;
		pos.y_pos = mesh->y;
		pos.z_pos = mesh->z;

		if (mesh->Flags & 1) {
			ShatterObject(0, mesh, 128, room_number, 0);
			SmashedMeshRoom[SmashedMeshCount] = room_number;
			SmashedMesh[SmashedMeshCount] = mesh;
			SmashedMeshCount++;
			mesh->Flags &= ~1;
			SoundEffect(SFX_HIT_ROCK, &pos, SFX_DEFAULT);
		}
	}

	return true;
}

// NGLE - 168
bool play_sound(unsigned char lower_sample_id, unsigned char upper_sample_id) {
	int indexed_sound_sample = (int)lower_sample_id | ((int)(upper_sample_id) << 8 & 0xff00);
	
	SoundEffect(indexed_sound_sample, NULL, SFX_ALWAYS);
	return true;
}

// NGLE - 180
bool static_explode(unsigned char static_id_lower, unsigned char static_id_upper) {
	unsigned short static_id = ((short)static_id_upper << 8) | (short)static_id_lower;

	NGStaticTableEntry* entry = &ng_static_id_table[static_id];
	int room_number = entry->room_index;
	if (room_number >= 0 && room_number < number_rooms) {
		MESH_INFO* mesh = &room[room_number].mesh[entry->mesh_id];

		PHD_3DPOS pos;
		pos.x_pos = mesh->x;
		pos.y_pos = mesh->y;
		pos.z_pos = mesh->z;

		if (mesh->Flags & 1) {
			ShatterObject(0, mesh, 128, room_number, 0);
			SmashedMeshRoom[SmashedMeshCount] = room_number;
			SmashedMesh[SmashedMeshCount] = mesh;
			SmashedMeshCount++;
			mesh->Flags &= ~1;
			
			// TODO: explosion effect is not accurate
			TriggerExplosionSparks(pos.x_pos, pos.y_pos, pos.z_pos, 3, -2, 0, room_number);
			for (int i = 0; i < 3; i++)
				TriggerExplosionSparks(pos.x_pos, pos.y_pos, pos.z_pos, 3, -1, 0, room_number);

			SoundEffect(SFX_EXPLOSION1, 0, SFX_DEFAULT);
			camera.bounce = -75;
		}
	}

	return true;
}


// NGLE - 189
bool static_visibility_set_as_invisible(unsigned char static_id_lower, unsigned char static_id_upper) {
	unsigned short static_id = ((short)static_id_upper << 8) | (short)static_id_lower;

	NGStaticTableEntry* entry = &ng_static_id_table[static_id];
	int room_number = entry->room_index;
	if (room_number >= 0 && room_number < number_rooms) {
		MESH_INFO* mesh = &room[room_number].mesh[entry->mesh_id];
		if (mesh->Flags & 1) {
			mesh->Flags &= ~1;
		}
	}

	return true;
}

// NGLE - 190
bool static_visibility_render_newly_visible(unsigned char static_id_lower, unsigned char static_id_upper) {
	unsigned short static_id = ((short)static_id_upper << 8) | (short)static_id_lower;

	NGStaticTableEntry* entry = &ng_static_id_table[static_id];
	int room_number = entry->room_index;
	if (room_number >= 0 && room_number < number_rooms) {
		MESH_INFO* mesh = &room[room_number].mesh[entry->mesh_id];
		if (mesh->Flags & 1) {
			mesh->Flags |= 1;
		}
	}

	return true;
}

// NGLE - 193
bool play_track_on_channel_with_restore(unsigned char track_id, unsigned char channel_id) {
	S_CDPlayExt(track_id, channel_id, false, true);
	return true;
}

// NGLE - 231
bool variables_add_value_to_variable(unsigned char variable, unsigned char value) {
	NGNumericOperation(NG_ADD, variable, value);
	return true;
}

// NGLE - 233
bool variables_subtract_value_from_variable(unsigned char variable, unsigned char value) {
	NGNumericOperation(NG_SUBTRACT, variable, value);
	return true;
}

// NGLE - 251
bool variables_multiply_variable_by_value(unsigned char variable, unsigned char value) {
	NGNumericOperation(NG_MULTIPLY, variable, value);
	return true;
}

// NGLE - 253
bool variables_divide_variable_by_value(unsigned char variable, unsigned char value) {
	NGNumericOperation(NG_DIVIDE, variable, value);
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

// NGLE - 371
bool perform_triggergroup_from_script_in_single_execution_mode(unsigned char trigger_group_id_lower, unsigned char trigger_group_id_upper) {
	unsigned short trigger_group_id = (trigger_group_id_upper << 8) | trigger_group_id_lower;
	return NGTriggerGroupFunction(trigger_group_id, 1);
}

// NGLE - 372
bool perform_triggergroup_from_script_in_multi_execution_mode(unsigned char trigger_group_id_lower, unsigned char trigger_group_id_upper) {
	unsigned short trigger_group_id = (trigger_group_id_upper << 8) | trigger_group_id_lower;
	return NGTriggerGroupFunction(trigger_group_id, 0);
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

bool NGFlipEffect(unsigned short param, short extra, bool heavy, bool skip_checks) {
	char action_data_1 = (char)extra & 0xff;
	char action_data_2 = (char)(extra >> 8) & 0xff;

	switch (param) {
		case INVENTORY_INCREASE_INVENTORY_ITEMS_BY_ONE_IN_X_WAY: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "INVENTORY_INCREASE_INVENTORY_ITEMS_BY_ONE_IN_X_WAY unimplemented!");
				return true;
			}
			break;
		}
		case INVENTORY_DECREASE_INVENTORY_ITEMS_BY_ONE: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "INVENTORY_DECREASE_INVENTORY_ITEMS_BY_ONE unimplemented!");
				return true;
			}
			break;
		}
		case INVENTORY_SET_INVENTORY_ITEMS: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "INVENTORY_SET_INVENTORY_ITEMS unimplemented!");
				return true;
			}
			break;
		}
		case DISABLE_INPUT_FOR_TIME: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return disable_input_for_time(action_data_1, action_data_2);
			break;
		}
		case ENABLE_INPUT: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return enable_input(action_data_1, action_data_2);
			break;
		}
		case KILL_AND_OR_SET_LARA_ON_FIRE: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return kill_and_or_set_lara_on_fire(action_data_1, action_data_2);
			break;
		}
		case PLAY_CD_TRACK_ON_CHANNEL_1: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return play_cd_track_channel_1(action_data_1, action_data_2);
			break;
		}
		case STOP_ALL_CD_TRACKS: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return stop_all_cd_tracks(action_data_1, action_data_2);
			break;
		}
		case PLAY_SOUND_FROM_FIRST_GROUP: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return play_sound_from_first_group(action_data_1, action_data_2);
			break;
		}
		case PLAY_SOUND_FROM_SECOND_GROUP: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return play_sound_from_second_group(action_data_1, action_data_2);
			break;
		}
		case STOP_SOUND_FROM_FIRST_GROUP: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return stop_sound_from_first_group(action_data_1, action_data_2);
			break;
		}
		case STOP_SOUND_FROM_SECOND_GROUP: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return stop_sound_from_second_group(action_data_1, action_data_2);
			break;
		}
		case STOP_ALL_SOUND_SAMPLES: {
			if (skip_checks || !NGIsOneShotTriggeredForTile())
				return stop_all_sound_samples(action_data_1, action_data_2);
			break;
		}
		case FORCE_LARA_ANIMATION_0_255_OF_SLOT_ANIMATION: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return force_lara_animation_0_255_of_slot_animation(action_data_1, action_data_2);
			break;
		}
		case MOVE_LARA_TO_START_POS_WITH_OCB_VALUE_IN_X_WAY: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return move_lara_to_lara_start_pos_in_x_way(action_data_1, action_data_2);
			break;
		}
		case FORCE_LARA_ANIMATION_256_512_OF_SLOT_ANIMATION: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return force_lara_animation_256_512_of_slot_animation(action_data_1, action_data_2);
			break;
		}
		case REMOVE_WEAPONS_OR_FLARES_FROM_LARAS_HANDS: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return remove_weapons_or_flares_from_laras_hands(action_data_1, action_data_2);
			break;
		}
		case CUTSCENE_SET_FADEIN_FOR_X_TIME: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_SET_FADEIN_FOR_X_TIME unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_SET_FADEOUT_FOR_X_TIME: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_SET_FADEOUT_FOR_X_TIME unimplemented!");
				return true;
			}
			break;
		}
		case RECHARGE_LARA_LIFE_BY_PERCENTAGE_OF_FULL_VITALITY: {
			if ((skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) || action_data_2 == 1)
				return recharge_lara_life_by_percentage(action_data_1, action_data_2);
			break;
		}
		case DISARM_LARA: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return disarm_lara(action_data_1, action_data_2);
			break;
		}
		case LARA_TOGGLE_INFINITE_AIR: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return lara_toggle_infinite_air(action_data_1, action_data_2);
			break;
		}
		case GLOBAL_TRIGGER_ENABLE_DISABLE: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "GLOBAL_TRIGGER_ENABLE_DISABLE unimplemented!");
				return true;
			}
			break;
		}
		case SET_ROOM_TYPE: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return set_room_type(action_data_1, action_data_2);
			break;
		}
		case REMOVE_ROOM_TYPE: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return remove_room_type(action_data_1, action_data_2);
			break;
		}
		case PERFORM_TRIGGERGROUP_FROM_SCRIPT_IN_SPECIFIC_WAY:
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return perform_triggergroup_from_script_in_specific_way(action_data_1, action_data_2);
			break;
		case FLIPMAP_ON: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return flipmap_on(action_data_1, action_data_2);
			break;
		}
		case FLIPMAP_OFF: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return flipmap_off(action_data_1, action_data_2);
			break;
		}
		case ORGANIZER_ENABLE:
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return organizer_enable(action_data_1, action_data_2);
			break;
		case ORGANIZER_DISABLE:
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return organizer_disable(action_data_1, action_data_2);
			break;
		case PLAY_CD_TRACK_ON_CHANNEL_2: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return play_cd_track_channel_2(action_data_1, action_data_2);
			break;
		}
		case STOP_CD_TRACK_ON_CHANNEL: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return stop_cd_track_on_channel(action_data_1, action_data_2);
			break;
		}
		case SET_VOLUME_OF_AUDIO_TRACK_ON_CHANNEL: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return set_volume_for_audio_track_on_channel(action_data_1, action_data_2);
			break;
		}
		case ACTIVATE_ITEM_GROUP_WITH_TIMER: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return activate_item_group_with_timer(action_data_1, action_data_2);
			break;
		}
		case UNTRIGGER_ITEM_GROUP_WITH_TIMER: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return untrigger_item_group_with_timer(action_data_1, action_data_2);
			break;
		}
		case ATTRACT_LARA_IN_DIRECTION_IN_AIR: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ATTRACT_LARA_IN_DIRECTION_IN_AIR unimplemented!");
				return true;
			}
			break;
		}
		case ATTRACT_LARA_IN_DIRECTION_ON_GROUND_AND_IN_AIR: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ATTRACT_LARA_IN_DIRECTION_ON_GROUND_AND_IN_AIR unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_SHATTER: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return static_shatter(action_data_1, action_data_2);
			}
			break;
		}
		case STATIC_REMOVE_COLLISION_FROM: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_REMOVE_COLLISION_FROM unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_RESTORE_COLLISION_TO: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_RESTORE_COLLISION_TO unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_SET_ICE_TRANSPARENCY: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_SET_ICE_TRANSPARENCY unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_SET_GLASS_TRANSPARENCY: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_SET_GLASS_TRANSPARENCY unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_REMOVE_TRANSPARENCY: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_REMOVE_TRANSPARENCY unimplemented!");
				return true;
			}
			break;
		}
		case PLAY_SOUND: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return play_sound(action_data_1, action_data_2);
			break;
		}
		case STATIC_EXPLODE: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return static_explode(action_data_1, action_data_2);
			break;
		}
		case STATIC_SET_POISON_ATTRIBUTE: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_SET_POISON_ATTRIBUTE unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_REMOVE_POISON_ATTRIBUTE: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_REMOVE_POISON_ATTRIBUTE unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_SET_DAMAGE_ATTRIBUTE: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_SET_DAMAGE_ATTRIBUTE unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_REMOVE_DAMAGE_ATTRIBUTE: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_REMOVE_DAMAGE_ATTRIBUTE unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_VISIBILITY_SET_AS_INVISIBLE: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return static_visibility_set_as_invisible(action_data_1, action_data_2);
			break;
		}
		case STATIC_VISIBILITY_RENDER_NEWLY_VISIBLE: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return static_visibility_render_newly_visible(action_data_1, action_data_2);
			break;
		}
		case STATIC_SET_COLOR: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_SET_COLOR unimplemented!");
				return true;
			}
			break;
		}
		case PLAY_TRACK_ON_CHANNEL_WITH_RESTORE: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return play_track_on_channel_with_restore(action_data_1, action_data_2);
			break;
		}
		case TEXT_PRINT_FORMATTED_TEXT_NGEXTRA_STRING_WITH_FORMATTING_DATA: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_PRINT_FORMATTED_TEXT_NGEXTRA_STRING_WITH_FORMATTING_DATA unimplemented!");
				return true;
			}
			break;
		}
		case DIARY_ADD_EXTRA_NG_STIRNG_TO_DIARY: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "DIARY_ADD_EXTRA_NG_STIRNG_TO_DIARY unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_ADD_VALUE_TO_VARIABLE: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return variables_add_value_to_variable(action_data_1, action_data_2);
			break;
		}
		case VARIABLES_SUBTRACT_VALUE_FROM_VARIABLE: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return variables_subtract_value_from_variable(action_data_1, action_data_2);
			break;
		}
		case VARIABLES_MULTIPLY_VARIABLE_BY_VALUE: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return variables_multiply_variable_by_value(action_data_1, action_data_2);
			break;
		}
		case VARIABLES_DIVIDE_VARIABLE_BY_VALUE: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return variables_divide_variable_by_value(action_data_1, action_data_2);
			break;
		}
		case VARIABLES_START_TRNG_TIMER_TO_X_MODE: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_START_TRNG_TIMER_TO_X_MODE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_INITIALIZE_TRNG_TIMER_TO_X_SECONDS: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_INITIALIZE_TRNG_TIMER_TO_X_SECONDS unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_SHOW_TRNG_TIMER_IN_X_POSITION: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_SHOW_TRNG_TIMER_IN_X_POSITION unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_HIDE_TRNG_TIMER_IN_X_POSITION: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_HIDE_TRNG_TIMER_IN_X_POSITION unimplemented!");
				return true;
			}
			break;
		}
		case SCREEN_FLASH_SCREEN_WITH_LIGHT_COLOR_FOR_DURATION: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "SCREEN_FLASH_SCREEN_WITH_LIGHT_COLOR_FOR_DURATION unimplemented!");
				return true;
			}
			break;		
		}
		case SPRITE_SHOW_SPRITE_WITH_DATA_IN_PARAMETERS_FOR_DURATION: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "SPRITE_SHOW_SPRITE_WITH_DATA_IN_PARAMETERS_FOR_DURATION unimplemented!");
				return true;
			}
			break;
		}
		case CAMERA_SHOW_BLACK_SCREEN_FOR_SECONDS_WITH_FINAL_CURTAIN_EFFECT: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return camera_show_black_screen_for_seconds_with_final_curtain_effect(action_data_1, action_data_2);
			break;
		}
		case CAMERA_SET_CINEMA_EFFECT_TYPE_FOR_SECONDS: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return camera_set_cinema_effect_type_for_seconds(action_data_1, action_data_2);
			break;
		}
		case PERFORM_TRIGGERGROUP_FROM_SCRIPT_IN_SINGLE_EXECUTION: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return perform_triggergroup_from_script_in_single_execution_mode(action_data_1, action_data_2);
			break;
		}
		case PERFORM_TRIGGERGROUP_FROM_SCRIPT_IN_MULTI_EXECUTION: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return perform_triggergroup_from_script_in_multi_execution_mode(action_data_1, action_data_2);
			break;
		}
		case GLOBAL_TRIGGER_ENABLE_WITH_ID: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "GLOBAL_TRIGGER_ENABLE_WITH_ID unimplemented!");
				return true;
			}
			break;
		}
		case GLOBAL_TRIGGER_DISABLE_WITH_ID: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "GLOBAL_TRIGGER_DISABLE_WITH_ID unimplemented!");
				return true;
			}
			break;
		}
		case SET_LARA_HOLSTER_TYPE: {
			if (skip_checks || !NGIsOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return set_lara_holsters(action_data_1, action_data_2);
			break;
		}
		default: {
			if (param < 47) {
				if (skip_checks || !NGIsOneShotTriggeredForTile()) {
					char original_trigger_timer = TriggerTimer;
					TriggerTimer = action_data_1;
					effect_routines[param](lara_item);
					TriggerTimer = original_trigger_timer;
				}
			} else {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "Unimplemented NGFlipEffect %u!", param);
				return true;
			}
		}
	}

	return true;
}

bool NGFlipEffectTrigger(unsigned short param, short extra, bool heavy) {
	NGStoreBackupTriggerRoomAndIndex();
	
	bool result = NGFlipEffect(param, extra, heavy, false);
	
	NGRestoreBackupTriggerRoomAndIndex();

	return result;
}