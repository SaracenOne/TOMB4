#include "../../tomb4/pch.h"

#include "../../specific/function_stubs.h"
#include "../../specific/audio.h"
#include "../camera.h"
#include "../control.h"
#include "../debris.h"
#include "../effects.h"
#include "../effect2.h"
#include "../flmtorch.h"
#include "../gameflow.h"
#include "../items.h"
#include "../lara.h"
#include "../objects.h"
#include "../savegame.h"
#include "../sound.h"
#include "../tomb4fx.h"
#include "../traps.h"
#include "../../specific/file.h"
#include "../../specific/dxsound.h"

#include "trng.h"
#include "trng_action.h"
#include "trng_arithmetic.h"
#include "trng_extra_state.h"
#include "trng_flipeffect.h"
#include "trng_script_parser.h"
#include "trng_triggergroup.h"

#include "../../tomb4/mod_config.h"
#include "../../tomb4/tomb4plus/t4plus_inventory.h"
#include "../../tomb4/tomb4plus/t4plus_savegame.h"

void NGAttractLaraInDirection(unsigned char direction, unsigned char speed) {
	switch (direction) {
		// West
	case 0x00:
		lara_item->pos.x_pos -= speed;
		break;
		// North West
	case 0x01:
		lara_item->pos.z_pos += speed;
		lara_item->pos.x_pos -= speed;
		break;
		// North
	case 0x02:
		lara_item->pos.z_pos += speed;
		break;
		// North East
	case 0x03:
		lara_item->pos.z_pos += speed;
		lara_item->pos.x_pos += speed;
		break;
		// East
	case 0x04:
		lara_item->pos.x_pos += speed;
		break;
		// South East
	case 0x05:
		lara_item->pos.x_pos += speed;
		lara_item->pos.z_pos -= speed;
		break;
		// South
	case 0x06:
		lara_item->pos.z_pos -= speed;
		break;
		// South West
	case 0x07:
		lara_item->pos.x_pos -= speed;
		lara_item->pos.z_pos -= speed;
		break;
	default:
		NGLog(NG_LOG_TYPE_ERROR, "lara_attract_lara_in_direction_on_ground_and_in_air_with_speed: unknown direction!");
		break;
	}
}

bool NGTriggerItemGroupWithTimer(unsigned char item_group, unsigned char timer, bool anti) {
	NG_ITEM_GROUP current_item_group = current_item_groups[item_group];
	int index = 0;
	for (int i = 0; i < NG_ITEM_GROUP_MAX_LIST; i++) {
		int current_script_item = current_item_group.item_list[i];
		if (current_script_item < 0) {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "Negative item group IDs (statics) not yet supported!");
			break;
		}
		int current_item = ng_script_id_table[current_script_item].script_index;

		items[current_item].timer = ((short)timer) * 30;
		NGItemActivator(current_item, anti);

		index++;
	}
	return true;
}

short NGGetInventoryObjectIDForByte(unsigned char inventory_id) {
	if (inventory_id >= 0x6B) {
		return ((inventory_id - 0x6B) + SIXSHOOTER_ITEM);
	} else if (inventory_id >= 0x67) {
		return ((inventory_id - 0x67) + GRENADE_GUN_ITEM);
	} else if (inventory_id >= 0x5C) {
		return ((inventory_id - 0x5C) + PISTOLS_ITEM);
	} else {
		return inventory_id + PUZZLE_ITEM1;
	}
}

// NGLE - 47
bool inventory_remove_inventory_item(unsigned char inventory_id, unsigned char _unused) {
	short object_number = NGGetInventoryObjectIDForByte(inventory_id);

	T4PlusSetInventoryCount(object_number, 0, true);

	return true;
}

// NGLE - 48
bool inventory_increase_inventory_items_by_one_in_x_way(unsigned char inventory_id, unsigned char show_popup) {
	short object_number = NGGetInventoryObjectIDForByte(inventory_id);

	if (show_popup)
		T4ShowObjectPickup(object_number);

	int current_inventory_count = T4PlusGetInventoryCount(object_number);
	current_inventory_count++;
	T4PlusSetInventoryCount(object_number, current_inventory_count, true);

	return true;
}

// NGLE - 49
bool inventory_decrease_inventory_items_by_one_in_x_way(unsigned char inventory_id, unsigned char _unused) {
	short object_number = NGGetInventoryObjectIDForByte(inventory_id);

	int current_inventory_count = T4PlusGetInventoryCount(object_number);
	current_inventory_count--;
	if (current_inventory_count < 0)
		current_inventory_count = 0;

	T4PlusSetInventoryCount(object_number, current_inventory_count, true);

	return true;
}

// NGLE - 50
bool inventory_set_inventory_items(unsigned char inventory_id, unsigned char count) {
	short object_number = NGGetInventoryObjectIDForByte(inventory_id);

	T4PlusSetInventoryCount(object_number, count, true);

	return true;
}

// NGLE - 51
bool disable_input_for_time(unsigned char input, unsigned char timer) {
	NGDisableInputForTime(input, (int)timer * 30);
	
	return true;
}

// NGLE - 52
bool keyboard_enable_input(unsigned char input, unsigned char unused) {
	NGEnableInput(input);
	
	return true;
}

// NGLE - 53
bool keyboard_simulate_receivement_of_keyboard_command(unsigned char input, unsigned char timer) {
	NGSimulateInputForTime(input, (int)timer * 30);

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

	if (lara_item->anim_number != animation_index_offset) {
		NGForceItemAnimation(lara.item_number, animation_index_offset);
	}

	return true;
}

// NGLE - 78
bool lara_force_x_state_id_and_e_next_state_id_for_lara(unsigned char state_id, unsigned char next_state_id) {
	items[lara.item_number].current_anim_state = state_id;
	items[lara.item_number].goal_anim_state = next_state_id;

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

				if (lara_item->room_number != ai->room_number)
					ItemNewRoom(lara.item_number, ai->room_number);

				return true;
			} else {
				// Warp directly
				lara_item->pos.x_pos = ai->x;
				lara_item->pos.y_pos = ai->y;
				lara_item->pos.z_pos = ai->z;
				lara_item->pos.y_rot = ai->y_rot;

				if (lara_item->room_number != ai->room_number)
					ItemNewRoom(lara.item_number, ai->room_number);

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

	if (lara_item->anim_number != animation_index_offset) {
		NGForceItemAnimation(lara.item_number, animation_index_offset);
	}

	return true;
}

// NGLE - 82
bool delay_load_x_level_in_seconds(unsigned char level_id, unsigned char level_timer) {
	if (level_timer < 0x1f) {
		pending_level_load_timer = level_timer * 30;
	} else {
		// Not sure how this works. Editor calls it 'perform one single time', but it doesn't seem to do anything.
		pending_level_load_timer = -1;
	}
	pending_level_load_id = level_id;

	return true;
}


// NGLE - 83
bool remove_weapons_or_flares_from_laras_hands(unsigned char unused1, unsigned char unused2) {
	lara.request_gun_type = WEAPON_NONE;

	return true;
}

// NGLE - 84
bool cutscene_set_fade_in_for_x_time(unsigned char fade_time, unsigned char unused2) {
	SetScreenFadeIn(fade_time);

	return true;
}

// NGLE - 85
bool cutscene_set_fade_out_for_x_time_in_way(unsigned char fade_time, unsigned char fade_type) {
	SetScreenFadeOut(fade_time, fade_type);

	return true;
}

// NGLE - 89
bool damage_lara_life_by_percentage(unsigned char action_data_1, unsigned char unused2) {
	const int MAX_LARA_HEALTH = 1000; // May need this to be customizable.
	int new_hit_points = lara_item->hit_points;
	if (action_data_1 <= 9) {
		int health_multiple = (MAX_LARA_HEALTH / 1000);
		new_hit_points -= (int)(health_multiple * (action_data_1 + 1));
	}
	else if (action_data_1 <= 18) {
		int health_multiple = (MAX_LARA_HEALTH / 100);
		new_hit_points -= (int)(health_multiple * (action_data_1 - 8));
	}
	else {
		int health_multiple = (MAX_LARA_HEALTH / 10);
		new_hit_points -= (int)(health_multiple * (action_data_1 - 17));
	}

	if (new_hit_points < 0)
		new_hit_points = 0;

	lara_item->hit_points = new_hit_points;

	return true;
}

// NGLE - 90
bool recharge_lara_life_by_percentage(unsigned char action_data_1, unsigned char unused2) {
	const int MAX_LARA_HEALTH = 1000; // May need this to be customizable.

	int new_hit_points = lara_item->hit_points;
	if (action_data_1 <= 9) {
		int health_multiple = (MAX_LARA_HEALTH / 1000);
		new_hit_points += (int)(health_multiple * (action_data_1 + 1));
	} else if (action_data_1 <= 18) {
		int health_multiple = (MAX_LARA_HEALTH / 100);
		new_hit_points += (int)(health_multiple * (action_data_1 - 8));
	} else {
		int health_multiple = (MAX_LARA_HEALTH / 10);
		new_hit_points += (int)(health_multiple * (action_data_1 - 17));
	}

	if (new_hit_points > MAX_LARA_HEALTH)
		new_hit_points = MAX_LARA_HEALTH;

	lara_item->hit_points = new_hit_points;

	return true;
}

// NGLE - 96
bool lara_disarm_lara(unsigned char remove_weapons_only, unsigned char _unusued) {
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

// NGLE - 109
bool global_trigger_enable_disable(unsigned char enable, unsigned char global_trigger_id) {
	ng_global_trigger_states[global_trigger_id].is_disabled = enable > 0 ? false : true;

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
	flipmap[flipmap_id] |= IFL_CODEBITS;

	if (!flip_stats[flipmap_id])
		FlipMap(flipmap_id);

	return true;
}

// NGLE - 126
bool flipmap_off(unsigned char flipmap_id, unsigned char unused_2) {
	flipmap[flipmap_id] &= ~(IFL_CODEBITS);

	if (flip_stats[flipmap_id])
		FlipMap(flipmap_id);

	return true;
}

// NGLE - 127
bool organizer_enable(unsigned char organizer_id_lower, unsigned char organizer_id_upper) {
	unsigned short organizer_id = ((short)organizer_id_upper << 8) | (short)organizer_id_lower;
	bool should_reset = NGIsOrganizerEnabled(organizer_id) == false;

	NGToggleOrganizer(organizer_id, true);

	// Docs say that if the organizer is disabled, if enable with flipeffect, it should always start from the beginning.
	if (should_reset)
		NGResetOrganizer(organizer_id);

	return true;
}

// NGLE - 128
bool organizer_disable(unsigned char organizer_id_lower, unsigned char organizer_id_upper) {
	unsigned short organizer_id = ((short)organizer_id_upper << 8) | (short)organizer_id_lower;
	NGToggleOrganizer(organizer_id, false);

	return true;
}

// NGLE - 129
bool sound_play_cd_track_channel_2(unsigned char track_id, unsigned char looping) {
	S_CDPlayExt(track_id, 1, looping, false);

	return true;
}

// NGLE - 130
bool sound_stop_cd_track_on_channel(unsigned char channel_id, unsigned char unused) {
	S_CDStopExt(channel_id);

	return true;
}


// NGLE - 133
bool sound_set_x_volume_for_audio_track_on_channel(unsigned char volume, unsigned char channel) {
	S_CDSetChannelVolume(volume, channel);
	return true;
}

// NGLE - 134
bool lara_attract_lara_in_direction_on_ground_with_speed(unsigned char direction, unsigned char speed) {
	if (lara_item->pos.y_pos == lara_item->floor)
		NGAttractLaraInDirection(direction, speed);

	return true;
}

// NGLE - 135
bool lara_attract_lara_in_direction_in_air_with_speed(unsigned char direction, unsigned char speed) {
	if (lara_item->pos.y_pos != lara_item->floor)
		NGAttractLaraInDirection(direction, speed);

	return true;
}

// NGLE - 145
bool itemgroup_activate_item_group_with_timer(unsigned char item_group, unsigned char timer) {
	return NGTriggerItemGroupWithTimer(item_group, timer, false);
}

// NGLE - 146
bool itemgroup_untrigger_item_group_with_timer(unsigned char item_group, unsigned char timer) {
	return NGTriggerItemGroupWithTimer(item_group, timer, true);
}

// NGLE - 158
bool lara_attract_lara_in_direction_on_ground_and_in_air_with_speed(unsigned char direction, unsigned char speed) {
	NGAttractLaraInDirection(direction, speed);

	return true;
}

// NGLE - 160
bool static_shatter(unsigned char static_id_lower, unsigned char static_id_upper) {
	unsigned short static_id = ((short)static_id_upper << 8) | (short)static_id_lower;

	NGStaticTableEntry* entry = &ng_static_id_table[static_id];
	int room_number = ng_room_remap_table[entry->remapped_room_index].room_index;
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

// NGLE - 161
bool static_remove_collision_for_x_static(unsigned char static_id_lower, unsigned char static_id_upper) {
	unsigned short static_id = ((short)static_id_upper << 8) | (short)static_id_lower;

	NGStaticTableEntry* entry = &ng_static_id_table[static_id];
	int room_number = ng_room_remap_table[entry->remapped_room_index].room_index;
	if (room_number >= 0 && room_number < number_rooms) {
		MESH_INFO* mesh = &room[room_number].mesh[entry->mesh_id];

		mesh->Flags |= 4; // Add no-collision flag
	}

	return true;
}

// NGLE - 162
bool static_restore_collision_for_x_static(unsigned char static_id_lower, unsigned char static_id_upper) {
	unsigned short static_id = ((short)static_id_upper << 8) | (short)static_id_lower;

	NGStaticTableEntry *entry = &ng_static_id_table[static_id];
	int room_number = ng_room_remap_table[entry->remapped_room_index].room_index;
	if (room_number >= 0 && room_number < number_rooms) {
		MESH_INFO* mesh = &room[room_number].mesh[entry->mesh_id];

		mesh->Flags &= ~4; // Removes no-collision flag
	}

	return true;
}

// NGLE - 166
bool static_move_static_with_data_in_x_parameter_list(unsigned char move_param_id_lower, unsigned char move_param_id_upper) {
	unsigned short move_param_id = ((short)move_param_id_upper << 8) | (short)move_param_id_lower;

	if (move_param_id >= MAX_NG_MOVE_ITEMS)
		return false;

	bool is_valid = false;

	NG_MOVE_ITEM *move_item = &current_move_items[move_param_id];
	int direction = move_item->direction & 0xff;
	int script_static = move_item->index_item;

	if (move_item->flags != 0xffff) {
		if (move_item->flags & ~(FMOV_INFINITE_LOOP)) {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "static_move_static_with_data_in_x_parameter_list: unimplemented flags 0x%04x", move_item->flags);
			return false;
		}
	}

	switch (direction) {
		case DIR_NORTH: {
			if (!NGGetStaticHorizontalMovementRemainingUnits(script_static)) {
				short current_angle = 0xC000;
				if (move_item->direction & DIR_INVERT_DIRECTION) {
					current_angle += 0x8000;
				}
				NGSetStaticHorizontalMovementAngle(script_static, (short)current_angle);
				NGSetStaticHorizontalMovementRemainingUnits(script_static, move_item->distance);
				NGSetStaticHorizontalMovementSpeed(script_static, move_item->speed);
				if (move_item->flags & FMOV_INFINITE_LOOP) {
					NGSetStaticHorizontalMovementRepeatUnits(script_static, move_item->distance);
				} else {
					NGSetStaticHorizontalMovementRepeatUnits(script_static, 0);
				}
				is_valid = true;
			}
			break;
		}
		case DIR_EAST: {
			if (!NGGetStaticHorizontalMovementRemainingUnits(script_static)) {
				short current_angle = 0x0000;
				if (move_item->direction & DIR_INVERT_DIRECTION) {
					current_angle += 0x8000;
				}
				NGSetStaticHorizontalMovementAngle(script_static, (short)current_angle);
				NGSetStaticHorizontalMovementRemainingUnits(script_static, move_item->distance);
				NGSetStaticHorizontalMovementSpeed(script_static, move_item->speed);
				if (move_item->flags & FMOV_INFINITE_LOOP) {
					NGSetStaticHorizontalMovementRepeatUnits(script_static, move_item->distance);
				} else {
					NGSetStaticHorizontalMovementRepeatUnits(script_static, 0);
				}
				is_valid = true;
			}
			break;
		}
		case DIR_SOUTH: {
			if (!NGGetStaticHorizontalMovementRemainingUnits(script_static)) {
				short current_angle = 0x4000;
				if (move_item->direction & DIR_INVERT_DIRECTION) {
					current_angle += 0x8000;
				}
				NGSetStaticHorizontalMovementAngle(script_static, (short)current_angle);
				NGSetStaticHorizontalMovementRemainingUnits(script_static, move_item->distance);
				NGSetStaticHorizontalMovementSpeed(script_static, move_item->speed);
				if (move_item->flags & FMOV_INFINITE_LOOP) {
					NGSetStaticHorizontalMovementRepeatUnits(script_static, move_item->distance);
				} else {
					NGSetStaticHorizontalMovementRepeatUnits(script_static, 0);
				}
				is_valid = true;
			}
			break;
		}
		case DIR_WEST: {
			if (!NGGetStaticHorizontalMovementRemainingUnits(script_static)) {
				short current_angle = 0x8000;
				if (move_item->direction & DIR_INVERT_DIRECTION) {
					current_angle += 0x8000;
				}
				NGSetStaticHorizontalMovementAngle(script_static, (short)current_angle);
				NGSetStaticHorizontalMovementRemainingUnits(script_static, move_item->distance);
				NGSetStaticHorizontalMovementSpeed(script_static, move_item->speed);
				if (move_item->flags & FMOV_INFINITE_LOOP) {
					NGSetStaticHorizontalMovementRepeatUnits(script_static, move_item->distance);
				} else {
					NGSetStaticHorizontalMovementRepeatUnits(script_static, 0);
				}
				is_valid = true;
			}
			break;
		}
		default: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "moveable_move_moveable_with_data_in_x_parameter_list: unimplemented direction 0x%04x", move_item->direction);
			break;
		}
	}

	if (is_valid) {
		NGSetStaticMovementInProgressSound(script_static, move_item->moving_sound);
		NGSetStaticMovementFinishedSound(script_static, move_item->final_sound);
	}

	return true;
}

// NGLE - 167
bool moveable_move_moveable_with_data_in_x_parameter_list(unsigned char move_param_id_lower, unsigned char move_param_id_upper) {
	unsigned short move_param_id = ((short)move_param_id_upper << 8) | (short)move_param_id_lower;

	if (move_param_id >= MAX_NG_MOVE_ITEMS)
		return false;

	bool is_valid = false;

	NG_MOVE_ITEM *move_item = &current_move_items[move_param_id];
	int direction = move_item->direction & 0xff;
	int script_item = ng_script_id_table[move_item->index_item].script_index;

	if (move_item->flags != 0xffff) {
		if (move_item->flags & ~(FMOV_INFINITE_LOOP)) {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "moveable_move_moveable_with_data_in_x_parameter_list: unimplemented flags 0x%04x", move_item->flags);
			return false;
		}
	}

	switch (direction) {
		case DIR_NORTH: {
			if (!NGGetItemHorizontalMovementRemainingUnits(script_item)) {
				short current_angle = (short)0xC000;
				if (move_item->direction & DIR_INVERT_DIRECTION) {
					current_angle += (short)0x8000;
				}
				NGSetItemHorizontalMovementAngle(script_item, (short)current_angle);
				NGSetItemHorizontalMovementRemainingUnits(script_item, move_item->distance);
				NGSetItemHorizontalMovementSpeed(script_item, move_item->speed);
				if (move_item->flags & FMOV_INFINITE_LOOP) {
					NGSetItemHorizontalMovementRepeatUnits(script_item, move_item->distance);
				} else {
					NGSetItemHorizontalMovementRepeatUnits(script_item, 0);
				}
				is_valid = true;
			}
			break;
		}
		case DIR_EAST: {
			if (!NGGetItemHorizontalMovementRemainingUnits(script_item)) {
				short current_angle = (short)0x0000;
				if (move_item->direction & DIR_INVERT_DIRECTION) {
					current_angle += (short)0x8000;
				}
				NGSetItemHorizontalMovementAngle(script_item, (short)current_angle);
				NGSetItemHorizontalMovementRemainingUnits(script_item, move_item->distance);
				NGSetItemHorizontalMovementSpeed(script_item, move_item->speed);
				if (move_item->flags & FMOV_INFINITE_LOOP) {
					NGSetItemHorizontalMovementRepeatUnits(script_item, move_item->distance);
				} else {
					NGSetItemHorizontalMovementRepeatUnits(script_item, 0);
				}
				is_valid = true;
			}
			break;
		}
		case DIR_SOUTH: {
			if (!NGGetItemHorizontalMovementRemainingUnits(script_item)) {
				short current_angle = (short)0x4000;
				if (move_item->direction & DIR_INVERT_DIRECTION) {
					current_angle += (short)0x8000;
				}
				NGSetItemHorizontalMovementAngle(script_item, (short)current_angle);
				NGSetItemHorizontalMovementRemainingUnits(script_item, move_item->distance);
				NGSetItemHorizontalMovementSpeed(script_item, move_item->speed);
				if (move_item->flags & FMOV_INFINITE_LOOP) {
					NGSetItemHorizontalMovementRepeatUnits(script_item, move_item->distance);
				} else {
					NGSetItemHorizontalMovementRepeatUnits(script_item, 0);
				}
				is_valid = true;
			}
			break;
		}
		case DIR_WEST: {
			if (!NGGetItemHorizontalMovementRemainingUnits(script_item)) {
				short current_angle = (short)0x8000;
				if (move_item->direction & DIR_INVERT_DIRECTION) {
					current_angle += (short)0x8000;
				}
				NGSetItemHorizontalMovementAngle(script_item, (short)current_angle);
				NGSetItemHorizontalMovementRemainingUnits(script_item, move_item->distance);
				NGSetItemHorizontalMovementSpeed(script_item, move_item->speed);
				if (move_item->flags & FMOV_INFINITE_LOOP) {
					NGSetItemHorizontalMovementRepeatUnits(script_item, move_item->distance);
				} else {
					NGSetItemHorizontalMovementRepeatUnits(script_item, 0);
				}
				is_valid = true;
			}
			break;
		}
		default: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "moveable_move_moveable_with_data_in_x_parameter_list: unimplemented direction 0x%04x", move_item->direction);
			break;
		}
	}

	if (is_valid) {
		NGSetItemMovementInProgressSound(script_item, move_item->moving_sound);
		NGSetItemMovementFinishedSound(script_item, move_item->final_sound);
	}

	return true;
}

// NGLE - 168
bool sound_play_sound_single_playback_of_global_sound_map(unsigned char lower_sample_id, unsigned char upper_sample_id) {
	int indexed_sound_sample = (int)lower_sample_id | ((int)(upper_sample_id) << 8 & 0xff00);
	
	SoundEffect(indexed_sound_sample, NULL, SFX_ALWAYS);
	return true;
}

// NGLE - 169
bool lara_force_x_animation_for_lara_preserve_state_id(unsigned char lower_anim_id, unsigned char upper_anim_id) {
	int animation_index = (int)lower_anim_id | ((int)(upper_anim_id) << 8 & 0xff00);

	int original_anim_state = lara_item->current_anim_state;
	int animation_index_offset = objects[LARA].anim_index + animation_index;

	NGForceItemAnimation(lara.item_number, animation_index_offset);

	lara_item->current_anim_state = original_anim_state;

	return true;
}

// NGLE - 170
bool lara_force_x_animation_for_lara_set_new_state_id(unsigned char lower_anim_id, unsigned char upper_anim_id) {
	int animation_index = (int)lower_anim_id | ((int)(upper_anim_id) << 8 & 0xff00);
	int animation_index_offset = objects[LARA].anim_index + animation_index;

	NGForceItemAnimation(lara.item_number, animation_index_offset);

	return true;
}

// NGLE - 171
bool lara_force_x_animation_for_lara_set_neutral_state_id(unsigned char lower_anim_id, unsigned char upper_anim_id) {
	int animation_index = (int)lower_anim_id | ((int)(upper_anim_id) << 8 & 0xff00);
	int animation_index_offset = objects[LARA].anim_index + animation_index;

	NGForceItemAnimation(lara.item_number, animation_index_offset);

	// Do we set the goal state ID too?
	items[lara.item_number].current_anim_state = 69;
	items[lara.item_number].goal_anim_state = 69;

	return true;
}


// NGLE - 180
bool static_explode(unsigned char static_id_lower, unsigned char static_id_upper) {
	unsigned short static_id = ((short)static_id_upper << 8) | (short)static_id_lower;

	NGStaticTableEntry* entry = &ng_static_id_table[static_id];
	int room_number = ng_room_remap_table[entry->remapped_room_index].room_index;
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
	int room_number = ng_room_remap_table[entry->remapped_room_index].room_index;
	if (room_number >= 0 && room_number < number_rooms) {
		MESH_INFO* mesh = &room[room_number].mesh[entry->mesh_id];
		if (mesh)
			mesh->Flags &= ~1;
		else
			NGLog(NG_LOG_TYPE_ERROR, "static_visibility_set_as_invisible: Missing static mesh!");
	}

	return true;
}

// NGLE - 190
bool static_visibility_render_newly_visible(unsigned char static_id_lower, unsigned char static_id_upper) {
	unsigned short static_id = ((short)static_id_upper << 8) | (short)static_id_lower;

	NGStaticTableEntry* entry = &ng_static_id_table[static_id];
	int room_number = ng_room_remap_table[entry->remapped_room_index].room_index;
	if (room_number >= 0 && room_number < number_rooms) {
		MESH_INFO* mesh = &room[room_number].mesh[entry->mesh_id];
		if (mesh)
			mesh->Flags |= 1;
		else
			NGLog(NG_LOG_TYPE_ERROR, "static_visibility_render_newly_visible: Missing static mesh!");
	}

	return true;
}

// NGLE - 193
bool play_track_on_channel_with_restore(unsigned char track_id, unsigned char channel_id) {
	S_CDPlayExt(track_id, channel_id, false, true);
	return true;
}

// NGLE - 199
bool lara_light_or_put_out_torch_in_laras_hand(unsigned char light_torch, unsigned char _unused) {
	lara.LitTorch = light_torch;
	return true;
}

// NGLE - 200
bool lara_give_or_remove_torch_to_or_from_hand_of_lara(unsigned char give_torch, unsigned char _unused) {
	if (give_torch) {
		if (lara.gun_type != WEAPON_TORCH) {
			lara.request_gun_type = WEAPON_TORCH;
			lara.gun_type = WEAPON_TORCH;
			lara.flare_control_left = 1;
			lara.left_arm.anim_number = objects[TORCH_ANIM].anim_index;
			lara.gun_status = LG_READY;
			lara.left_arm.lock = 0;
			lara.left_arm.frame_number = 0;
			lara.left_arm.frame_base = anims[objects[TORCH_ANIM].anim_index].frame_ptr;
			lara.mesh_ptrs[LM_LHAND] = meshes[objects[TORCH_ANIM].mesh_index + LM_LHAND * 2];
		}
	} else {
		if (lara.gun_type == WEAPON_TORCH) {
			lara.flare_control_left = 0;
			lara.LitTorch = 0;
			lara.left_arm.lock = 0;
			lara.gun_type = WEAPON_NONE;
			lara.request_gun_type = WEAPON_NONE;
			lara.gun_status = LG_NO_ARMS;
			lara.mesh_ptrs[LM_LHAND] = meshes[objects[LARA].mesh_index + LM_LHAND * 2];
		}
	}
	return true;
}

// NGLE - 231
bool variables_add_value_to_x_variable(unsigned char variable, unsigned char value) {
	NGNumericOperation(NG_ADD, variable, value);
	return true;
}

// NGLE - 232
bool variables_set_x_variable_to_value(unsigned char variable, unsigned char value) {
	NGNumericOperation(NG_SET, variable, value);
	return true;
}

// NGLE - 233
bool variables_subtract_value_from_x_variable(unsigned char variable, unsigned char value) {
	NGNumericOperation(NG_SUBTRACT, variable, value);
	return true;
}

// NGLE - 234
bool variables_set_in_x_variable_the_bit(unsigned char variable, unsigned char bit) {
	NGNumericOperation(NG_BIT_SET, variable, bit);
	return true;
}

// NGLE - 235
bool variables_clear_in_x_variable_the_bit(unsigned char variable, unsigned char bit) {
	NGNumericOperation(NG_BIT_CLEAR, variable, bit);
	return true;
}

// NGLE - 244
bool variables_copy_to_x_numeric_variable_the_savegame_memory_value(unsigned char variable, unsigned char savegame_value) {
	NGNumericOperation(NG_SET, variable, NGNumericGetSavegameValue(savegame_value));
	return true;
}

// NGLE - 251
bool variables_multiply_x_variable_by_value(unsigned char variable, unsigned char value) {
	NGNumericOperation(NG_MULTIPLY, variable, value);
	return true;
}

// NGLE - 252
bool variables_set_in_x_numeric_variable_the_negative_value(unsigned char variable, unsigned char value) {
	NGNumericOperation(NG_SET, variable, -128 + value);
	return true;
}

// NGLE - 253
bool variables_divide_x_variable_by_value(unsigned char variable, unsigned char value) {
	NGNumericOperation(NG_DIVIDE, variable, value);
	return true;
}

// NGLE - 264
bool variables_start_the_x_trng_timer_in_mode(unsigned char set_global_timer, unsigned char countdown_timer) {
	if (set_global_timer) {
		if (countdown_timer)
			ng_global_timer_frame_increment = -1;
		else
			ng_global_timer_frame_increment = 1;
	} else {
		if (countdown_timer)
			ng_local_timer_frame_increment = -1;
		else
			ng_local_timer_frame_increment = 1;
	}

	return true;
}

// NGLE - 265
bool variables_stop_the_x_trng_timer(unsigned char set_global_timer, unsigned char _unused) {
	if (set_global_timer) {
		ng_global_timer_frame_increment = 0;
	} else {
		ng_local_timer_frame_increment = 0;
	}

	return true;
}

// NGLE - 266
bool variables_initialize_the_x_trng_timer_to_seconds(unsigned char set_global_timer, unsigned char seconds) {
	if (set_global_timer)
		ng_global_timer = (int)(seconds) * 30;
	else
		ng_local_timer = (int)(seconds) * 30;

	return true;
}

// NGLE - 267
bool variables_initialize_the_x_trng_timer_to_big_number_seconds(unsigned char set_global_timer, unsigned char big_number_id) {
	NGLog(NG_LOG_TYPE_ERROR, "variables_initialize_the_x_trng_timer_to_big_number_seconds: unimplemented!");
	return true;
}

// NGLE - 268
bool variables_initialize_the_x_trng_timer_to_frame_ticks(unsigned char set_global_timer, unsigned char frame_ticks) {
	if (set_global_timer)
		ng_global_timer = (int)(frame_ticks);
	else
		ng_local_timer = (int)(frame_ticks);

	return true;
}

// NGLE - 269
bool variables_show_x_trng_timer_in_position(unsigned char set_global_timer, unsigned char position) {

	if (position == NGTimerPosition::NG_TIMER_POSITION_DOWN_DAMAGE_BAR) {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NG_TIMER_POSITION_DOWN_DAMAGE_BAR unimplemented!");
	} else if (position == NGTimerPosition::NG_TIMER_POSITION_DOWN_COLD_BAR) {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NG_TIMER_POSITION_DOWN_COLD_BAR unimplemented!");
	} else if (position == NGTimerPosition::NG_TIMER_POSITION_DOWN_LEFT_BARS) {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NG_TIMER_POSITION_DOWN_LEFT_BARS unimplemented!");
	} else if (position == NGTimerPosition::NG_TIMER_POSITION_DOWN_RIGHT_BARS) {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NG_TIMER_POSITION_DOWN_RIGHT_BARS unimplemented!");
	}

	if (set_global_timer) {
		ng_global_timer_position = (NGTimerPosition)(position);
		ng_global_timer_time_until_hide = -1;
	} else {
		ng_local_timer_position = (NGTimerPosition)(position);
		ng_local_timer_time_until_hide = -1;
	}

	return true;
}

// NGLE - 270
bool variables_hide_the_x_trng_timer_in_seconds(unsigned char set_global_timer, unsigned char seconds) {
	if (set_global_timer) {
		if (ng_global_timer_time_until_hide < 0) {
			ng_global_timer_time_until_hide = (int)(seconds) * 30;
		}
	} else {
		if (ng_local_timer_time_until_hide < 0) {
			ng_local_timer_time_until_hide = (int)(seconds) * 30;
		}
	}

	return true;
}

// NGLE - 284
bool variables_numeric_invert_the_sign_of_x_numeric_value(unsigned char variable, unsigned char unused) {
	NGNumericOperation(NG_INVERT_SIGN, variable, NGNumericGetVariable(variable));
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
	return NGTriggerGroupFunction(trigger_group_id, TRIGGER_GROUP_EXECUTION_SINGLE);
}

// NGLE - 372
bool perform_triggergroup_from_script_in_multi_execution_mode(unsigned char trigger_group_id_lower, unsigned char trigger_group_id_upper) {
	unsigned short trigger_group_id = (trigger_group_id_upper << 8) | trigger_group_id_lower;
	return NGTriggerGroupFunction(trigger_group_id, TRIGGER_GROUP_EXECUTION_MULTIPLE);
}

// NGLE - 373
bool perform_triggergroup_from_script_in_continuous_mode(unsigned char trigger_group_id_lower, unsigned char trigger_group_id_upper) {
	unsigned short trigger_group_id = (trigger_group_id_upper << 8) | trigger_group_id_lower;
	return NGTriggerGroupFunction(trigger_group_id, TRIGGER_GROUP_EXECUTION_CONTINUOUS);
}


// NGLE - 374
bool enable_global_trigger_with_id(unsigned char global_trigger_id_lower, unsigned char global_trigger_id_upper) {
	unsigned short global_trigger_id = (global_trigger_id_upper << 8) | global_trigger_id_lower;
	ng_global_trigger_states[global_trigger_id].is_disabled = false;
	return true;
}

// NGLE - 375
bool disable_global_trigger_with_id(unsigned char global_trigger_id_lower, unsigned char global_trigger_id_upper) {
	unsigned short global_trigger_id = (global_trigger_id_upper << 8) | global_trigger_id_lower;
	ng_global_trigger_states[global_trigger_id].is_disabled = true;
	return true;
}

// NGLE - 404
bool trigger_secret(unsigned char secret_number, unsigned char unused) {
	T4TriggerSecret(secret_number);
	return true;
}

// NGLE - 407
bool set_lara_holsters(unsigned char holster_type, unsigned char unused) {
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

// NGLE - 411
bool lara_set_x_opacity_for_lara_for_seconds(unsigned char opacity, unsigned char seconds) {
	return false;
}

bool NGFlipEffect(unsigned short param, short extra, bool heavy, bool skip_checks) {
	char action_data_1 = (char)extra & 0xff;
	char action_data_2 = (char)(extra >> 8) & 0xff;

	switch (param) {
		case INVENTORY_REMOVE_INVENTORY_ITEM: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return inventory_remove_inventory_item(action_data_1, action_data_2);
			}
			break;
		}
		case INVENTORY_INCREASE_INVENTORY_ITEMS_BY_ONE_IN_X_WAY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return inventory_increase_inventory_items_by_one_in_x_way(action_data_1, action_data_2);
			}
			break;
		}
		case INVENTORY_DECREASE_INVENTORY_ITEMS_BY_ONE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return inventory_decrease_inventory_items_by_one_in_x_way(action_data_1, action_data_2);

			}
			break;
		}
		case INVENTORY_SET_INVENTORY_ITEMS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return inventory_set_inventory_items(action_data_1, action_data_2);
			}
			break;
		}
		case KEYBOARD_DISABLE_INPUT_FOR_TIME: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return disable_input_for_time(action_data_1, action_data_2);
			break;
		}
		case KEYBOARD_ENABLE_INPUT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return keyboard_enable_input(action_data_1, action_data_2);
			break;
		}
		case KEYBOARD_SIMULATE_RECEIVEMENT_OF_KEYBOARD_COMMAND: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return keyboard_simulate_receivement_of_keyboard_command(action_data_1, action_data_2);
			break;
		}
		case SCREEN_HIDE_SCREEN_FOR_X_TIME_IN_WAY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "SCREEN_HIDE_SCREEN_FOR_X_TIME_IN_WAY unimplemented!");
			break;
		}
		case SCREEN_SHOW_SCREEN: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "SCREEN_SHOW_SCREEN unimplemented!");
			break;
		}
		case ANIMATED_TEXTURES_STOP_ANIMATION_RANGE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ANIMATED_TEXTURES_STOP_ANIMATION_RANGE unimplemented!");
			break;
		}
		case ANIMATED_TEXTURES_RESTART_ANIMATION_RANGE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ANIMATED_TEXTURES_RESTART_ANATIMION_RANGE unimplemented!");
			break;
		}
		case ANIMATED_TEXTURES_SET_THE_FRAME_OF_TEXTURE_OF_FIRST_P_RANGE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ANIMATED_TEXTURES_SET_THE_FRAME_OF_TEXTURE_OF_FIRST_P_RANGE unimplemented!");
			break;
		}
		case ANIMATED_TEXTURES_SET_THE_FRAME_OF_TEXTURE_OF_SECOND_P_RANGE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ANIMATED_TEXTURES_SET_THE_FRAME_OF_TEXTURE_OF_SECOND_P_RANGE unimplemented!");
			break;
		}
		case ANIMATED_TEXTURES_INVERT_SCROLL_DIRECTION: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ANIMATED_TEXTURES_INVERT_SCROLL_DIRECTION unimplemented!");
			break;
		}
		case WEATHER_FOG_ENABLE_VOLUMETRIC_FX: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_FOG_ENABLE_VOLUMETRIC_FX unimplemented!");
			break;
		}
		case WEATHER_FOG_DISABLE_VOLUMETRIC_FX: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_FOG_DISABLE_VOLUMETRIC_FX unimplemented!");
			break;
		}
		case KILL_AND_OR_SET_LARA_ON_FIRE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return kill_and_or_set_lara_on_fire(action_data_1, action_data_2);
			break;
		}
		case TEXT_PRINT_NGEXTRA_STRING_ON_SCREEN_FOR_X_SECONDS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_PRINT_NGEXTRA_STRING_ON_SCREEN_FOR_X_SECONDS unimplemented!");
			break;
		}
		case TEXT_PRINT_STANDARD_STRING_ON_SCREEN_FOR_X_SECONDS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_PRINT_STANDARD_STRING_ON_SCREEN_FOR_X_SECONDS unimplemented!");
			break;
		}
		case TEXT_SET_COLOR_AND_POSITION_FOR_NEXT_PRINT_STRING_FLIPEFFECT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_SET_COLOR_AND_POSITION_FOR_NEXT_PRINT_STRING_FLIPEFFECT unimplemented!");
			break;
		}
		case TEXT_ERASE_ALL_STRINGS_SHOWED_WITH_PRINT_STRING_EFFECT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_ERASE_ALL_STRINGS_SHOWED_WITH_PRINT_STRING_EFFECT unimplemented!");
			break;
		}
		case PLAY_CD_TRACK_ON_CHANNEL_1: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return play_cd_track_channel_1(action_data_1, action_data_2);
			break;
		}
		case STOP_ALL_CD_TRACKS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return stop_all_cd_tracks(action_data_1, action_data_2);
			break;
		}
		case PLAY_SOUND_FROM_FIRST_GROUP: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return play_sound_from_first_group(action_data_1, action_data_2);
			break;
		}
		case PLAY_SOUND_FROM_SECOND_GROUP: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return play_sound_from_second_group(action_data_1, action_data_2);
			break;
		}
		case STOP_SOUND_FROM_FIRST_GROUP: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return stop_sound_from_first_group(action_data_1, action_data_2);
			break;
		}
		case STOP_SOUND_FROM_SECOND_GROUP: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return stop_sound_from_second_group(action_data_1, action_data_2);
			break;
		}
		case STOP_ALL_SOUND_SAMPLES: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile())
				return stop_all_sound_samples(action_data_1, action_data_2);
			break;
		}
		case TEXT_SET_BLINK_STATUS_AND_SPEED: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_SET_BLINK_STATUS_AND_SPEED unimplemented!");
			break;
		}
		case TEXT_RESET_ALL_TEXT_FORMATTING_SETTINGS_WITH_DEFAULT_VALUES: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_RESET_ALL_TEXT_FORMATTING_SETTINGS_WITH_DEFAULT_VALUES unimplemented!");
			break;
		}
		case FORCE_LARA_ANIMATION_0_255_OF_SLOT_ANIMATION: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return force_lara_animation_0_255_of_slot_animation(action_data_1, action_data_2);
			break;
		}
		case FORCE_LARA_STATE_ID_AND_NEXT_STATE_ID: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return lara_force_x_state_id_and_e_next_state_id_for_lara(action_data_1, action_data_2);
			break;
		}
		case MOVE_LARA_TO_START_POS_WITH_OCB_VALUE_IN_X_WAY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return move_lara_to_lara_start_pos_in_x_way(action_data_1, action_data_2);
			break;
		}
		case FORCE_LARA_ANIMATION_256_512_OF_SLOT_ANIMATION: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return force_lara_animation_256_512_of_slot_animation(action_data_1, action_data_2);
			break;
		}
		case TEXT_SET_SIZE_CHARACTERS_FOR_NEXT_PRINT_STRING_COMMAND: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_SET_SIZE_CHARACTERS_FOR_NEXT_PRINT_STRING_COMMAND unimplemented!");
			break;
		}
		case DELAY_LOAD_X_LEVEL_IN_SECONDS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return delay_load_x_level_in_seconds(action_data_1, action_data_2);
			break;
		}
		case REMOVE_WEAPONS_OR_FLARES_FROM_LARAS_HANDS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return remove_weapons_or_flares_from_laras_hands(action_data_1, action_data_2);
			break;
		}
		case CUTSCENE_SET_FADEIN_FOR_X_TIME: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				cutscene_set_fade_in_for_x_time(action_data_1, action_data_2);
				return true;
			}
			break;
		}
		case CUTSCENE_SET_FADEOUT_FOR_X_TIME_IN_WAY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				cutscene_set_fade_out_for_x_time_in_way(action_data_1, action_data_2);
				return true;
			}
			break;
		}
		case TIMER_SHOW_OR_HIDE_SCREEN_TIMER: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TIMER_SHOW_OR_HIDE_SCREEN_TIMER unimplemented!");
				return true;
			}
			break;
		}
		case ANIMCOMMAND_ADD_TO_CURRENT_OBJECT_EFFECT_FOR_X_TIME: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ANIMCOMMAND_ADD_TO_CURRENT_OBJECT_EFFECT_FOR_X_TIME unimplemented!");
				return true;
			}
			break;
		}
		case ENABLE_OR_DISABLE_MIRROR_WITH_X_HIDDEN_ROOM: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ENABLE_OR_DISABLE_MIRROR_WITH_X_HIDDEN_ROOM unimplemented!");
				return true;
			}
			break;
		}
		case DAMAGE_LARA_LIFE_BY_PERCENTAGE_OF_FULL_VITALITY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy) || action_data_2 == 1) {
				return damage_lara_life_by_percentage(action_data_1, action_data_2);
			}
			break;
		}
		case RECHARGE_LARA_LIFE_BY_PERCENTAGE_OF_FULL_VITALITY: {
			if ((skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) || action_data_2 == 1)
				return recharge_lara_life_by_percentage(action_data_1, action_data_2);
			break;
		}

		case LARA_INVULNERABLE_FOR_X_TIME_WITH_EFFECT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_INVULNERABLE_FOR_X_TIME_WITH_EFFECT unimplemented!");
				return true;
			}
			break;
		}
		case LARA_REMOVE_FLAMES: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_REMOVE_FLAMES unimplemented!");
				return true;
			}
			break;
		}
		case LARA_REMOVE_INVULNERABLE_STATUS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_REMOVE_INVULNERABLE_STATUS unimplemented!");
				return true;
			}
			break;
		}
		case ANIMATED_TEXTURES_START_THE_X_TEXTURE_SEQUENCE_FOR_ANIMATED_RANGE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ANIMATED_TEXTURES_START_THE_X_TEXTURE_SEQUENCE_FOR_ANIMATED_RANGE unimplemented!");
				return true;
			}
			break;
		}
		case ANIMATED_TEXTURES_STOP_TEXTURE_SEQUENCE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ANIMATED_TEXTURES_STOP_TEXTURE_SEQUENCE unimplemented!");
				return true;
			}
			break;
		}
		case LARA_DISARM_LARA: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return lara_disarm_lara(action_data_1, action_data_2);
			break;
		}
		case BACKUP_SAVE_IN_SILENT_WAY_THE_CURRENT_GAME_IN_X_BACKUP_FILE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "BACKUP_SAVE_IN_SILENT_WAY_THE_CURRENT_GAME_IN_X_BACKUP_FILE unimplemented!");
				return true;
			}
			break;
		}
		case BACKUP_RESTORE_THE_X_BACKUP_FILE_IN_WAY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "BACKUP_RESTORE_THE_X_BACKUP_FILE_IN_WAY unimplemented!");
				return true;
			}
			break;
		}
		case LARA_SWAP_MESHES_OF_LARA_WITH_SLOT_IN_WAY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_SWAP_MESHES_OF_LARA_WITH_SLOT_IN_WAY unimplemented!");
				return true;
			}
			break;
		}
		case LARA_SET_SINGLE_X_LARA_MESH_WITH_MESH_GOT_FROM_SLOT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_SET_SINGLE_X_LARA_MESH_WITH_MESH_GOT_FROM_SLOT unimplemented!");
				return true;
			}
			break;
		}
		case ANIMCOMMAND_ACTIVATE_HEAVY_TRIGGERS_IN_SECTOR_WHERE_LARA_IS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ANIMCOMMAND_ACTIVATE_HEAVY_TRIGGERS_IN_SECTOR_WHERE_LARA_IS unimplemented!");
				return true;
			}
			break;
		}
		case ANIMCOMMAND_TURN_CURRENT_OBJECT_OF_X_DEGREES_IN_WAY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ANIMCOMMAND_TURN_CURRENT_OBJECT_OF_X_DEGREES_IN_WAY unimplemented!");
				return true;
			}
			break;
		}
		case ANIMCOMMAND_UPDATE_ALSO_ORIGINAL_LARA_POSITION_AFTER_A_SETPOSITION: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ANIMCOMMAND_UPDATE_ALSO_ORIGINAL_LARA_POSITION_AFTER_A_SETPOSITION unimplemented!");
				return true;
			}
			break;
		}
		case LARA_TOGGLE_INFINITE_AIR: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return lara_toggle_infinite_air(action_data_1, action_data_2);
			break;
		}
		case LARA_COPY_MESHES_FROM_X_SLOT_TO_LARA_MESHES_IN_WAY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_COPY_MESHES_FROM_X_SLOT_TO_LARA_MESHES_IN_WAY unimplemented!");
				return true;
			}
			break;
		}
		case LARA_BACKUP_MESHES_OF_LARA_IN_X_SLOT_IN_WAY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_BACKUP_MESHES_OF_LARA_IN_X_SLOT_IN_WAY unimplemented!");
				return true;
			}
			break;
		}
		case LARA_ENABLE_OR_DISABLE_WEAPONS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_ENABLE_OR_DISABLE_WEAPONS unimplemented!");
				return true;
			}
			break;
		}
		case LARA_HIDE_OR_SHOW_HOLSTER_MESHES: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_HIDE_OR_SHOW_HOLSTER_MESHES unimplemented!");
				return true;
			}
			break;
		}
		case GLOBAL_TRIGGER_ENABLE_DISABLE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return global_trigger_enable_disable(action_data_1, action_data_2);
			}
			break;
		}
		case LARA_INCREASE_AIR_FOR_LARA_OF_X_UNITS_IN_WAY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_INCREASE_AIR_FOR_LARA_OF_X_UNITS_IN_WAY unimplemented!");
				return true;
			}
			break;
		}
		case LARA_DECREASE_DAMAGE_BAR_OF_DAMAGE_ROOM_OF_X_UNITS_IN_WAY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_DECREASE_DAMAGE_BAR_OF_DAMAGE_ROOM_OF_X_UNITS_IN_WAY unimplemented!");
				return true;
			}
			break;
		}
		case LARA_DECREASE_COLD_BAR_OF_COLD_ROOM_OF_X_UNITS_IN_WAY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_DECREASE_COLD_BAR_OF_COLD_ROOM_OF_X_UNITS_IN_WAY unimplemented!");
				return true;
			}
			break;
		}
		case LARA_HEALTH_POISON_LARA_WITH_X_INTENSITY_OF_POISON: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_HEALTH_POISON_LARA_WITH_X_INTENSITY_OF_POISON unimplemented!");
				return true;
			}
			break;
		}
		case LARA_REMOVE_POSION_FROM_LARA: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_REMOVE_POSION_FROM_LARA unimplemented!");
				return true;
			}
			break;
		}
		case SET_ROOM_TYPE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return set_room_type(action_data_1, action_data_2);
			break;
		}
		case REMOVE_ROOM_TYPE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return remove_room_type(action_data_1, action_data_2);
			break;
		}
		case WEATHER_SET_RAIN_OR_SNOW_INTENSITY_FOR_X_ROOM_WITH_NEW_INTENSITY_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_SET_RAIN_OR_SNOW_INTENSITY_FOR_X_ROOM_WITH_NEW_INTENSITY_VALUE unimplemented!");
				return true;
			}
			break;
		}
		case PERFORM_TRIGGERGROUP_FROM_SCRIPT_IN_SPECIFIC_WAY:
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return perform_triggergroup_from_script_in_specific_way(action_data_1, action_data_2);
			break;

		case CAMERA_SET_CURRENT_CAMERA_AS_FOLLOW_CAMERA_ON_AXIS_UNTIL_CONDITION: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CAMERA_SET_CURRENT_CAMERA_AS_FOLLOW_CAMERA_ON_AXIS_UNTIL_CONDITION unimplemented!");
				return true;
			}
			break;
		}
		case CAMERA_STOP_CURRENT_AXIS_OR_EFFECT_CAMERA_AND_THE_CAMERA_LINKED_WITH_IT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CAMERA_STOP_CURRENT_AXIS_OR_EFFECT_CAMERA_AND_THE_CAMERA_LINKED_WITH_IT unimplemented!");
				return true;
			}
			break;
		}
		case LARA_DISABLE_X_SKILL: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_DISABLE_X_SKILL unimplemented!");
				return true;
			}
			break;
		}
		case LARA_RESTORE_X_SKILL: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_RESTORE_X_SKILL unimplemented!");
				return true;
			}
			break;
		}
		case CAMERA_USE_CURRENT_CAMERA_TO_PERFORM_X_EFFECT_AT_DISTANCE_FROM_TARGET: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CAMERA_USE_CURRENT_CAMERA_TO_PERFORM_X_EFFECT_AT_DISTANCE_FROM_TARGET unimplemented!");
				return true;
			}
			break;
		}
		case FLIPMAP_ENABLED_X_FLIPMAP_WITH_BUTTONS_FOR_ACTIVATION: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "FLIPMAP_ENABLED_X_FLIPMAP_WITH_BUTTONS_FOR_ACTIVATION unimplemented!");
				return true;
			}
			break;
		}
		case FLIPMAP_ON: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return flipmap_on(action_data_1, action_data_2);
			break;
		}
		case FLIPMAP_OFF: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return flipmap_off(action_data_1, action_data_2);
			break;
		}
		case ORGANIZER_ENABLE:
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return organizer_enable(action_data_1, action_data_2);
			break;
		case ORGANIZER_DISABLE:
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return organizer_disable(action_data_1, action_data_2);
			break;
		case SOUND_PLAY_CD_TRACK_ON_CHANNEL_2: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return sound_play_cd_track_channel_2(action_data_1, action_data_2);
			break;
		}
		case SOUND_STOP_CD_TRACK_ON_CHANNEL: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return sound_stop_cd_track_on_channel(action_data_1, action_data_2);
			break;
		}
		case SOUND_PLAY_X_IMPORTED_FILE_IN_LOOP_MODE_ON_CHANNEL: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "SOUND_PLAY_X_IMPORTED_FILE_IN_LOOP_MODE_ON_CHANNEL unimplemented!");
				return true;
			}
			break;
		}
		case SOUND_PLAY_X_IMPORTED_FILE_IN_SINGLE_PLAY_MODE_ON_CHANNEL: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "SOUND_PLAY_X_IMPORTED_FILE_IN_SINGLE_PLAY_MODE_ON_CHANNEL unimplemented!");
				return true;
			}
			break;
		}
		case SOUND_SET_X_VOLUME_OF_AUDIO_TRACK_ON_CHANNEL: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return sound_set_x_volume_for_audio_track_on_channel(action_data_1, action_data_2);
			break;
		}
		case LARA_ATTRACT_LARA_IN_DIRECTION_ON_GROUND_WITH_SPEED: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return lara_attract_lara_in_direction_on_ground_with_speed(action_data_1, action_data_2);
			}
			break;
		}
		case LARA_ATTRACT_LARA_IN_DIRECTION_IN_AIR_WITH_SPEED: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return lara_attract_lara_in_direction_in_air_with_speed(action_data_1, action_data_2);
			}
			break;
		}
		case LARA_ATTRACT_LARA_UP_DOWN: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_ATTRACT_LARA_UP_DOWN unimplemented!");
				return true;
			}
			break;
		}
		case ITEMGROUP_MOVE_CONTINUOUSLY_FORWARD_BACKWARD_X_ITEMGROUP_OF_CLICKS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ITEMGROUP_MOVE_CONTINUOUSLY_FORWARD_BACKWARD_X_ITEMGROUP_OF_CLICKS unimplemented!");
				return true;
			}
			break;
		}
		case ITEMGROUP_MOVE_UP_X_ITEMGROUP_FOR_CLICKS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ITEMGROUP_MOVE_UP_X_ITEMGROUP_FOR_CLICKS unimplemented!");
				return true;
			}
			break;
		}
		case ITEMGROUP_MOVE_DOWN_X_ITEMGROUP_FOR_CLICKS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ITEMGROUP_MOVE_DOWN_X_ITEMGROUP_FOR_CLICKS unimplemented!");
				return true;
			}
			break;
		}
		case ITEMGROUP_MOVE_TO_WEST_X_ITEMGROUP_FOR_CLICKS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ITEMGROUP_MOVE_TO_WEST_X_ITEMGROUP_FOR_CLICKS unimplemented!");
				return true;
			}
			break;
		}
		case ITEMGROUP_MOVE_TO_NORTH_X_ITEMGROUP_FOR_CLICKS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ITEMGROUP_MOVE_TO_NORTH_X_ITEMGROUP_FOR_CLICKS unimplemented!");
				return true;
			}
			break;
		}
		case ITEMGROUP_MOVE_TO_EAST_X_ITEMGROUP_FOR_CLICKS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ITEMGROUP_MOVE_TO_EAST_X_ITEMGROUP_FOR_CLICKS unimplemented!");
				return true;
			}
			break;
		}
		case ITEMGROUP_MOVE_TO_SOUTH_X_ITEMGROUP_FOR_CLICKS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ITEMGROUP_MOVE_TO_EAST_X_ITEMGROUP_FOR_CLICKS unimplemented!");
				return true;
			}
			break;
		}
		case ITEMGROUP_MOVE_CONTINOUSLY_UPSTAIRS_DOWNSTAIRS_X_ITEMGROUP_FOR_CLICKS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ITEMGROUP_MOVE_TO_EAST_X_ITEMGROUP_FOR_CLICKS unimplemented!");
				return true;
			}
			break;
		}
		case ITEMGROUP_ACTIVATE_ITEMGROUP_WITH_TIMER: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return itemgroup_activate_item_group_with_timer(action_data_1, action_data_2);
			break;
		}
		case ITEMGROUP_UNTRIGGER_ITEMGROUP_WITH_TIMER: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return itemgroup_untrigger_item_group_with_timer(action_data_1, action_data_2);
			break;
		}
		case ITEMGROUP_DISABLE_ALL_CONTINUOUS_ACTIONS_X_ITEMGROUP: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ITEMGROUP_DISABLE_ALL_CONTINUOUS_ACTIONS_X_ITEMGROUP unimplemented!");
				return true;
			}
			break;
		}
		case TEXT_PRINT_NGEXTRA_STRING_ONSCREEN_FREEZE_GAME_AND_WAIT_FOR_ESCAPE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_PRINT_NGEXTRA_STRING_ONSCREEN_FREEZE_GAME_AND_WAIT_FOR_ESCAPE unimplemented!");
				return true;
			}
			break;
		}
		case TEXT_PRINT_STANDARD_STRING_ONSCREEN_FREEZE_GAME_AND_WAIT_FOR_ESCAPE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_PRINT_STANDARD_STRING_ONSCREEN_FREEZE_GAME_AND_WAIT_FOR_ESCAPE unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_SKY_X_ENABLE_DISABLE_THE_LAYER: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_SKY_X_ENABLE_DISABLE_THE_LAYER unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_SKY_X_ENABLE_DISABLE_LIGHTNING: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_SKY_X_ENABLE_DISABLE_LIGHTNING unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_SKY_SET_NEW_X_COLOR_FOR_LAYER: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_SKY_SET_NEW_X_COLOR_FOR_LAYER unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_SKY_SET_NEW_X_SPEED_FOR_LAYER: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_SKY_SET_NEW_X_SPEED_FOR_LAYER unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_SKY_CHANGE_SLOWLY_THE_COLOR_OF_LAYER1_TO_X_COLOR_IN_SECONDS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_SKY_CHANGE_SLOWLY_THE_COLOR_OF_LAYER1_TO_X_COLOR_IN_SECONDS unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_SKY_CHANGE_SLOWLY_THE_COLOR_OF_LAYER2_TO_X_COLOR_IN_SECONDS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_SKY_CHANGE_SLOWLY_THE_COLOR_OF_LAYER2_TO_X_COLOR_IN_SECONDS unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_SET_X_NEW_STATE_FOR_SNOW_IN_CURRENT_LEVEL: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_SET_X_NEW_STATE_FOR_SNOW_IN_CURRENT_LEVEL unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_SET_X_NEW_STATE_FOR_RAIN_IN_CURRENT_LEVEL: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_SET_X_NEW_STATE_FOR_RAIN_IN_CURRENT_LEVEL unimplemented!");
				return true;
			}
			break;
		}
		case LARA_ATTRACT_LARA_IN_DIRECTION_ON_GROUND_AND_IN_AIR_WITH_SPEED: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile()) {
				return lara_attract_lara_in_direction_on_ground_and_in_air_with_speed(action_data_1, action_data_2);
			}
			break;
		}
		case DISTANCE_SET_LEVEL_FAR_VIEW_DISTANCE_TO_X_NUMBER_OF_SECTORS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "DISTANCE_SET_LEVEL_FAR_VIEW_DISTANCE_TO_X_NUMBER_OF_SECTORS unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_SHATTER: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return static_shatter(action_data_1, action_data_2);
			}
			break;
		}
		case STATIC_REMOVE_COLLISION_FROM: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return static_remove_collision_for_x_static(action_data_1, action_data_2);
			}
			break;
		}
		case STATIC_RESTORE_COLLISION_TO: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return static_restore_collision_for_x_static(action_data_1, action_data_2);
			}
			break;
		}
		case STATIC_SET_ICE_TRANSPARENCY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_SET_ICE_TRANSPARENCY unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_SET_GLASS_TRANSPARENCY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_SET_GLASS_TRANSPARENCY unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_REMOVE_TRANSPARENCY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_REMOVE_TRANSPARENCY unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_MOVE_STATIC_WITH_DATA_IN_X_PARAMETERS_LIST: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				static_move_static_with_data_in_x_parameter_list(action_data_1, action_data_2);
				return true;
			}
			break;
		}
		case MOVEABLE_MOVE_MOVEABLE_WITH_DATA_IN_X_PARAMETERS_LIST: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				moveable_move_moveable_with_data_in_x_parameter_list(action_data_1, action_data_2);
				return true;
			}
			break;
		}
		case SOUND_PLAY_SOUND_SINGLE_PLAYBACK_OF_GLOBAL_SOUND_MAP: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return sound_play_sound_single_playback_of_global_sound_map(action_data_1, action_data_2);
			break;
		}
		case LARA_FORCE_X_ANIMATION_FOR_LARA_PRESERVE_STATE_ID: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return lara_force_x_animation_for_lara_preserve_state_id(action_data_1, action_data_2);
				return true;
			}
			break;
		}
		case LARA_FORCE_X_ANIMATION_FOR_LARA_SET_NEW_STATE_ID: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return lara_force_x_animation_for_lara_set_new_state_id(action_data_1, action_data_2);
			}
			break;
		}
		case LARA_FORCE_X_ANIMATION_FOR_LARA_SET_NEUTRAL_STATE_ID: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return lara_force_x_animation_for_lara_set_neutral_state_id(action_data_1, action_data_2);
			}
			break;
		}
		case STATIC_ROTATE_STATIC_WITH_DATA_OF_X_PARAMETER_LIST: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_ROTATE_STATIC_WITH_DATA_OF_X_PARAMETER_LIST unimplemented!");
				return true;
			}
			break;
		}
		case MOVEABLE_ROTATE_MOVEABLE_WITH_DATA_IN_X_PARAMETERS_LIST: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "MOVEABLE_ROTATE_MOVEABLE_WITH_DATA_IN_X_PARAMETERS_LIST unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_STOP_ALL_ROTATION_FOR_X_STATIC: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_STOP_ALL_ROTATION_FOR_X_STATIC unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_STOP_ALL_ROTATION_FOR_STATICS_IN_X_ROOM: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_STOP_ALL_ROTATION_FOR_STATICS_IN_X_ROOM unimplemented!");
				return true;
			}
			break;
		}
		case MOVEABLE_STOP_ALL_ROTATION_FOR_MOVEABLES_IN_X_ROOM: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "MOVEABLE_STOP_ALL_ROTATION_FOR_MOVEABLES_IN_X_ROOM unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_STOP_THE_MOTION_OF_X_STATIC: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_STOP_THE_MOTION_OF_X_STATIC unimplemented!");
				return true;
			}
			break;
		}
		case MOVEABLE_STOP_ALL_MOTIONS_OF_ALL_MOVEABLES_IN_X_ROOM: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "MOVEABLE_STOP_ALL_MOTIONS_OF_ALL_MOVEABLES_IN_X_ROOM unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_STOP_THE_MOVEMENTS_OF_ALL_STATICS_IN_X_ROOM: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_STOP_THE_MOVEMENTS_OF_ALL_STATICS_IN_X_ROOM unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_EXPLODE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return static_explode(action_data_1, action_data_2);
			break;
		}
		case STATIC_SET_EXPLOSIVE_ATTRIBUTE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_SET_EXPLOSIVE_ATTRIBUTE unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_REMOVE_EXPLOSIVE_ATTRIBUTE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_REMOVE_EXPLOSIVE_ATTRIBUTE unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_SET_POISON_ATTRIBUTE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_SET_POISON_ATTRIBUTE unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_REMOVE_POISON_ATTRIBUTE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_REMOVE_POISON_ATTRIBUTE unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_SET_DAMAGE_ATTRIBUTE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_SET_DAMAGE_ATTRIBUTE unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_REMOVE_DAMAGE_ATTRIBUTE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_REMOVE_DAMAGE_ATTRIBUTE unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_SET_BURNING_ATTRIBUTE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_SET_BURNING_ATTRIBUTE unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_REMOVE_BURNING_ATTRIBUTE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_REMOVE_BURNING_ATTRIBUTE unimplemented!");
				return true;
			}
			break;
		}
		case STATIC_VISIBILITY_SET_AS_INVISIBLE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return static_visibility_set_as_invisible(action_data_1, action_data_2);
			break;
		}
		case STATIC_VISIBILITY_RENDER_NEWLY_VISIBLE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return static_visibility_render_newly_visible(action_data_1, action_data_2);
			break;
		}
		case STATIC_SET_COLOR: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATIC_SET_COLOR unimplemented!");
				return true;
			}
			break;
		}
		case TRIGGERGROUP_STOP_X_TRIGGERGROUP: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TRIGGERGROUP_STOP_X_TRIGGERGROUP unimplemented!");
				return true;
			}
			break;
		}
		case PLAY_TRACK_ON_CHANNEL_WITH_RESTORE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return play_track_on_channel_with_restore(action_data_1, action_data_2);
			break;
		}
		case WEATHER_SET_X_DISTANCE_FOG_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_SET_X_DISTANCE_FOG_VALUE unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_CHANGE_START_FOR_DISTANCE_TO_X_DISTANCE_IN_SECONDS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_CHANGE_START_FOR_DISTANCE_TO_X_DISTANCE_IN_SECONDS unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_PULSE_START_DISTANCE_FOG_FROM_CURRENT_TO_X_DISTANCE_IN_SECONDS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_PULSE_START_DISTANCE_FOG_FROM_CURRENT_TO_X_DISTANCE_IN_SECONDS unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_ENABLE_OR_DISABLE_ALL_FOG: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_ENABLE_OR_DISABLE_ALL_FOG unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_STOP_THE_PULSE_START_DISTANCE_FOG_AND_SET_NEW_X_FOG_DISTANCE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_STOP_THE_PULSE_START_DISTANCE_FOG_AND_SET_NEW_X_FOG_DISTANCE unimplemented!");
				return true;
			}
			break;
		}
		case LARA_LIGHT_OR_PUT_OUT_THE_TORCH_IN_LARAS_HAND: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return lara_light_or_put_out_torch_in_laras_hand(action_data_1, action_data_2);
			}
			break;
		}
		case LARA_GIVE_OR_REMOVE_TORCH_TO_OR_FROM_HAND_OF_LARA: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return lara_give_or_remove_torch_to_or_from_hand_of_lara(action_data_1, action_data_2);
			}
			break;
		}
		case TEXT_VERTICAL_SCROLLING_OF_NGEXTRA_X_STRING_WITH_SPEED: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_VERTICAL_SCROLLING_OF_NGEXTRA_X_STRING_WITH_SPEED unimplemented!");
				return true;
			}
			break;
		}
		case TEXT_ABORT_ALL_VERTICAL_SCROLLING_TEXT_OPERATIONS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_ABORT_ALL_VERTICAL_SCROLLING_TEXT_OPERATIONS unimplemented!");
				return true;
			}
			break;
		}
		case TEXT_PRINT_FORMATTED_TEXT_NGEXTRA_STRING_WITH_FORMATTING_DATA: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_PRINT_FORMATTED_TEXT_NGEXTRA_STRING_WITH_FORMATTING_DATA unimplemented!");
				return true;
			}
			break;
		}
		case TEXT_REMOVE_X_EXTRA_NGSTRING_FROM_SCREEN: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_REMOVE_X_EXTRA_NGSTRING_FROM_SCREEN unimplemented!");
				return true;
			}
			break;
		}
		case TEXT_VERTICAL_SCROLLING_OF_NGEXTRA_X_STRING_WITH_FORMATTING_DATA: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_VERTICAL_SCROLLING_OF_NGEXTRA_X_STRING_WITH_FORMATTING_DATA unimplemented!");
				return true;
			}
			break;
		}
		case TEXT_HORIZONTAL_SCROLLING_OF_NGEXTRA_X_STRING_WITH_FORMATTING_DATA: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_HORIZONTAL_SCROLLING_OF_NGEXTRA_X_STRING_WITH_FORMATTING_DATA unimplemented!");
				return true;
			}
			break;
		}
		case TEXT_PRINT_PSX_X_STRING_WITH_FORMATTING_DATA: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_PRINT_PSX_X_STRING_WITH_FORMATTING_DATA unimplemented!");
				return true;
			}
			break;
		}
		case TEXT_PRINT_PC_X_STRING_WITH_FORMATTING_DATA: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_PRINT_PC_X_STRING_WITH_FORMATTING_DATA unimplemented!");
				return true;
			}
			break;
		}
		case TEXT_PRINT_PC_X_STRING_WITH_FORMATTING_DATA_AND_WAIT_FOR_ESCAPE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_PRINT_PC_X_STRING_WITH_FORMATTING_DATA_AND_WAIT_FOR_ESCAPE unimplemented!");
				return true;
			}
			break;
		}
		case TEXT_PRINT_NGEXTRA_X_STRING_WITH_FORMATTING_DATA_AND_WAIT_FOR_ESCAPE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_PRINT_NGEXTRA_X_STRING_WITH_FORMATTING_DATA_AND_WAIT_FOR_ESCAPE unimplemented!");
				return true;
			}
			break;
		}
		case ANIMCOMMAND_SET_TEMPORARY_FREE_HANDS_UNTIL_IS_PERFORMING_X_ANIMATION: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ANIMCOMMAND_SET_TEMPORARY_FREE_HANDS_UNTIL_IS_PERFORMING_X_ANIMATION unimplemented!");
				return true;
			}
			break;
		}
		case ANIMCOMMAND_SET_TEMPORARY_FREE_HANDS_FOR_X_SECONDS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ANIMCOMMAND_SET_TEMPORARY_FREE_HANDS_FOR_X_SECONDS unimplemented!");
				return true;
			}
			break;
		}
		case ANIMCOMMAND_REMOVE_FREE_HANDS_AND_RESTORE_PREVIOUS_STATUS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ANIMCOMMAND_REMOVE_FREE_HANDS_AND_RESTORE_PREVIOUS_STATUS unimplemented!");
				return true;
			}
			break;
		}
		case CAMERA_CHANGE_CAMERA_MODE_WITH_X_PARAMETER_FOR_TIME: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CAMERA_CHANGE_CAMERA_MODE_WITH_X_PARAMETER_FOR_TIME unimplemented!");
				return true;
			}
			break;
		}
		case CAMERA_RESTORE_CAMERA_MODE_AFTER_A_CHANGE_CAMERA_FLIPEFFECT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CAMERA_RESTORE_CAMERA_MODE_AFTER_A_CHANGE_CAMERA_FLIPEFFECT unimplemented!");
				return true;
			}
			break;
		}
		case ANIMCOMMAND_RESET_THE_NUMBER_OF_TURNS_JUMP_POWER_OF_PARALLEL_BAR: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ANIMCOMMAND_RESET_THE_NUMBER_OF_TURNS_JUMP_POWER_OF_PARALLEL_BAR unimplemented!");
				return true;
			}
			break;
		}
		case IMAGES_SHOW_IMAGE_WITH_DATA_IN_X_IMAGE_SCRIPT_COMMAND_FOR_SECONDS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "IMAGES_SHOW_IMAGE_WITH_DATA_IN_X_IMAGE_SCRIPT_COMMAND_FOR_SECONDS unimplemented!");
				return true;
			}
			break;
		}
		case IMAGES_REMOVE_FROM_SCREEN_THE_CURRENT_POP_UP_IMAGE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "IMAGES_REMOVE_FROM_SCREEN_THE_CURRENT_POP_UP_IMAGE unimplemented!");
				return true;
			}
			break;
		}
		case DIARY_ADD_EXTRA_NG_STIRNG_TO_DIARY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "DIARY_ADD_EXTRA_NG_STIRNG_TO_DIARY unimplemented!");
				return true;
			}
			break;
		}
		case DIARY_CLEAR_ALL_STRINGS_IN_X_DIARY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "DIARY_CLEAR_ALL_STRINGS_IN_X_DIARY unimplemented!");
				return true;
			}
			break;
		}
		case DIARY_REMOVE_LAST_STRING_FROM_X_DIARY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "DIARY_REMOVE_LAST_STRING_FROM_X_DIARY unimplemented!");
				return true;
			}
			break;
		}
		case DIARY_SHOW_DIARY_AT_PAGE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "DIARY_SHOW_DIARY_AT_PAGE unimplemented!");
				return true;
			}
			break;
		}
		case SHOW_STATISTICS_SCREEN: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "SHOW_STATISTICS_SCREEN unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_SET_THE_X_COLOR_FOR_DISTANCE_FOG: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_SET_THE_X_COLOR_FOR_DISTANCE_FOG unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_ENABLE_HARDWARE_FOG: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_ENABLE_HARDWARE_FOG unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_SET_X_MAX_VISIBILITY_DISTANCE_FOR_FOG_BULBS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_SET_X_MAX_VISIBILITY_DISTANCE_FOR_FOG_BULBS unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_SET_X_END_FOG_LIMIT_FOR_DISTANCE_FOG: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_SET_X_END_FOG_LIMIT_FOR_DISTANCE_FOG unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_CHANGE_END_LIMIT_OF_DISTANCE_FOG_IN_X_WAY_WITH_SPEED: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_CHANGE_END_LIMIT_OF_DISTANCE_FOG_IN_X_WAY_WITH_SPEED unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_CHANGE_START_LIMIT_OF_DISTANCE_FOG_IN_X_WAY_WITH_SPEED: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_CHANGE_START_LIMIT_OF_DISTANCE_FOG_IN_X_WAY_WITH_SPEED unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_STOP_THE_X_CHANGE_LIMIT_OF_DISTANEC_FOG_EFFECT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_STOP_THE_X_CHANGE_LIMIT_OF_DISTANEC_FOG_EFFECT unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_ADD_VALUE_TO_X_VARIABLE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return variables_add_value_to_x_variable(action_data_1, action_data_2);
			break;
		}
		case VARIABLES_SET_X_VARIABLE_TO_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return variables_set_x_variable_to_value(action_data_1, action_data_2);
			break;
		}
		case VARIABLES_SUBTRACT_VALUE_FROM_X_VARIABLE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return variables_subtract_value_from_x_variable(action_data_1, action_data_2);
			break;
		}
		case VARIABLES_SET_IN_X_VARIABLE_THE_BIT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return variables_set_in_x_variable_the_bit(action_data_1, action_data_2);
			}
			break;
		}
		case VARIABLES_CLEAR_IN_X_VARIABLE_THE_BIT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return variables_clear_in_x_variable_the_bit(action_data_1, action_data_2);
			}
			break;
		}
		case VARIABLES_COPY_CURRENTVALUE_TO_X_STORE_VARIABLE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_COPY_CURRENTVALUE_TO_X_STORE_VARIABLE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_COPY_X_STORE_VARIABLE_TO_CURRENTVALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_COPY_X_STORE_VARIABLE_TO_CURRENTVALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_COPY_THE_X_TEXT_VARIABLE_TO_Y_TEXT_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_COPY_THE_X_TEXT_VARIABLE_TO_Y_TEXT_VALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_COPY_THE_X_NG_STRING_TO_Y_TEXT_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_COPY_THE_X_NG_STRING_TO_Y_TEXT_VALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_ADD_TO_BIG_TEXT_THE_X_NG_STRING_WITH_SEPARATOR: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_ADD_TO_BIG_TEXT_THE_X_NG_STRING_WITH_SEPARATOR unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_CLEAR_THE_X_VARIABLE_GROUP: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_CLEAR_THE_X_VARIABLE_GROUP unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_ADD_TO_BIG_TEXT_THE_X_TEXT_VARIABLE_WITH_SEPARATOR: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_ADD_TO_BIG_TEXT_THE_X_TEXT_VARIABLE_WITH_SEPARATOR unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_ADD_TO_BIG_TEXT_THE_X_NUMERIC_VARIABLE_WITH_SEPARATOR: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_ADD_TO_BIG_TEXT_THE_X_TEXT_VARIABLE_WITH_SEPARATOR unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_COPY_TO_X_NUMERIC_VARIABLE_THE_SAVEGAME_MEMORY_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return variables_copy_to_x_numeric_variable_the_savegame_memory_value(action_data_1, action_data_2);
			}
			break;
		}
		case VARIABLES_COPY_FROM_X_NUMERIC_VARIABLE_THE_SAVEGAME_MEMORY_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_COPY_FROM_X_NUMERIC_VARIABLE_THE_SAVEGAME_MEMORY_VALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_SET_IN_X_SAVEGAME_MEMORY_THE_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_COPY_FROM_X_NUMERIC_VARIABLE_THE_SAVEGAME_MEMORY_VALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_SET_IN_X_SAVEGAME_MEMORY_THE_BIT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_SET_IN_X_SAVEGAME_MEMORY_THE_BIT unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_CLEAR_IN_X_SAVEGAME_MEMORY_THE_BIT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_CLEAR_IN_X_SAVEGAME_MEMORY_THE_BIT unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_ADD_TO_X_SAVEGAME_MEMORY_THE_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_ADD_TO_X_SAVEGAME_MEMORY_THE_VALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_SUBTRACT_FROM_X_SAVEGAME_MEMORY_THE_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_SUBTRACT_FROM_X_SAVEGAME_MEMORY_THE_VALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_MULTIPLY_X_VARIABLE_BY_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return variables_multiply_x_variable_by_value(action_data_1, action_data_2);
			break;
		}
		case VARIABLES_SET_IN_X_NUMERIC_VARIABLE_THE_NEGATIVE_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return variables_set_in_x_numeric_variable_the_negative_value(action_data_1, action_data_2);
			}
			break;
		}
		case VARIABLES_DIVIDE_X_VARIABLE_BY_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return variables_divide_x_variable_by_value(action_data_1, action_data_2);
			break;
		}
		case VARIABLES_SET_IN_X_SAVEGAME_MEMORY_THE_NEGATIVE_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_SET_IN_X_SAVEGAME_MEMORY_THE_NEGATIVE_VALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_SET_IN_X_SELECTED_ITEM_MEMORY_THE_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_SET_IN_X_SELECTED_ITEM_MEMORY_THE_VALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_COPY_TO_X_NUMERIC_VARIABLE_THE_SELECTED_ITEM_MEMORY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_COPY_TO_X_NUMERIC_VARIABLE_THE_SELECTED_ITEM_MEMORY unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_COPY_FROM_X_NUMERIC_VARIABLE_TO_SELECTED_ITEM_MEMORY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_COPY_FROM_X_NUMERIC_VARIABLE_TO_SELECTED_ITEM_MEMORY unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_ADD_TO_X_SELECTED_ITEM_MEMORY_THE_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_ADD_TO_X_SELECTED_ITEM_MEMORY_THE_VALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_SET_IN_X_SELECTED_ITEM_MEMORY_THE_BIT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_SET_IN_X_SELECTED_ITEM_MEMORY_THE_BIT unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_CLEAR_IN_X_SELECTED_ITEM_MEMORY_THE_BIT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_CLEAR_IN_X_SELECTED_ITEM_MEMORY_THE_BIT unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_SET_IN_X_SELECTED_ITEM_MEMORY_THE_BIG_NUMBER_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_SET_IN_X_SELECTED_ITEM_MEMORY_THE_BIG_NUMBER_VALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_SET_IN_X_SAVEGAME_MEMORY_THE_BIG_NUMBER_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_SET_IN_X_SAVEGAME_MEMORY_THE_BIG_NUMBER_VALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_SET_IN_X_NUMBERIC_VARIABLE_THE_BIG_NUMBER_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_SET_IN_X_NUMBERIC_VARIABLE_THE_BIG_NUMBER_VALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_START_THE_X_TRNG_TIMER_TO_MODE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return variables_start_the_x_trng_timer_in_mode(action_data_1, action_data_2);
			}
			break;
		}
		case VARIABLES_STOP_THE_X_TRNG_TIMER: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return variables_stop_the_x_trng_timer(action_data_1, action_data_2);
			}
			break;
		}
		case VARIABLES_INITIALIZE_TRNG_TIMER_TO_X_SECONDS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return variables_initialize_the_x_trng_timer_to_seconds(action_data_1, action_data_2);
			}
			break;
		}
		case VARIABLES_INITIALIZE_X_TRNG_TIMER_TO_BIG_NUMBER_SECONDS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_INITIALIZE_X_TRNG_TIMER_TO_BIG_NUMBER_SECONDS unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_INITIALIZE_X_TRNG_TIMER_TO_FRAME_TICKS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return variables_initialize_the_x_trng_timer_to_frame_ticks(action_data_1, action_data_2);
			}
			break;
		}
		case VARIABLES_SHOW_X_TRNG_TIMER_IN_POSITION: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				variables_show_x_trng_timer_in_position(action_data_1, action_data_2);
				return true;
			}
			break;
		}
		case VARIABLES_HIDE_X_TRNG_TIMER_IN_SECONDS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				variables_hide_the_x_trng_timer_in_seconds(action_data_1, action_data_2);
				return true;
			}
			break;
		}
		case VARIABLES_COPY_X_NUMERIC_VARIABLE_TO_CURRENTVALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_COPY_X_NUMERIC_VARIABLE_TO_CURRENTVALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_COPY_CURRENTVALUE_TO_X_NUMBERIC_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_COPY_CURRENTVALUE_TO_X_NUMBERIC_VALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_ADD_TO_X_SAVEGAME_MEMORY_THE_CURRENTVALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_ADD_TO_X_SAVEGAME_MEMORY_THE_CURRENTVALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_SUBTRACT_FROM_X_SAVEGAME_MEMORY_THE_CURRENTVALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_SUBTRACT_FROM_X_SAVEGAME_MEMORY_THE_CURRENTVALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_ADD_TO_SELECTED_ITEM_MEMORY_THE_CURRENTVALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_ADD_TO_SELECTED_ITEM_MEMORY_THE_CURRENTVALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_SUBTRACT_TO_SELECTED_ITEM_MEMORY_THE_CURRENTVALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_SUBTRACT_TO_SELECTED_ITEM_MEMORY_THE_CURRENTVALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_COPY_FROM_X_CODE_MEMORY_TO_CURRENTVALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_COPY_FROM_X_CODE_MEMORY_TO_CURRENTVALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_COPY_FROM_CURRENTVALUE_TO_X_CODE_MEMORY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_COPY_FROM_CURRENTVALUE_TO_X_CODE_MEMORY unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_SET_IN_X_CODE_MEMORY_THE_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_SET_IN_X_CODE_MEMORY_THE_VALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_ADD_TO_X_CODE_MEMORY_THE_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_ADD_TO_X_CODE_MEMORY_THE_VALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_SET_IN_X_CODE_MEMORY_THE_BIT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_SET_IN_X_CODE_MEMORY_THE_BIT unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_CLEAR_IN_X_CODE_MEMORY_THE_BIT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_CLEAR_IN_X_CODE_MEMORY_THE_BIT unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_ADD_TO_X_CODE_MEMORY_THE_CURRENTVALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_ADD_TO_X_CODE_MEMORY_THE_CURRENTVALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_INVERT_THE_SIGN_OF_X_NUMERIC_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return variables_numeric_invert_the_sign_of_x_numeric_value(action_data_1, action_data_2);
			}
			break;
		}
		case VARIABLES_ADD_TO_CURRENTVALUE_THE_X_NUMERIC_VARIABLE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_ADD_TO_CURRENTVALUE_THE_X_NUMERIC_VARIABLE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_SUBTRACT_FROM_CURRENTVALUE_THE_X_NUMERIC_VARIABLE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_SUBTRACT_FROM_CURRENTVALUE_THE_X_NUMERIC_VARIABLE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_DIVIDE_CURRENTVALUE_BY_X_NUMERIC_VARIABLE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_DIVIDE_CURRENTVALUE_BY_X_NUMERIC_VARIABLE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_MULTIPLY_CURRENTVALUE_BY_X_NUMERIC_VARIABLE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_MULTIPLY_CURRENTVALUE_BY_X_NUMERIC_VARIABLE unimplemented!");
				return true;
			}
			break;
		}
		case SWITCH_PERFORM_THE_X_SWITCH_SCRIPT_COMMAND: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "SWITCH_PERFORM_THE_X_SWITCH_SCRIPT_COMMAND unimplemented!");
				return true;
			}
			break;
		}
		case ORGANIZER_RESUME_X_ORGANIZER_IN_Y_WAY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ORGANIZER_RESUME_X_ORGANIZER_IN_Y_WAY unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_COPY_TO_X_NUMERIC_VARIABLE_THE_Y_COLOR_RGB: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_COPY_TO_X_NUMERIC_VARIABLE_THE_Y_COLOR_RGB unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_SET_THE_X_SLOT_AS_SELECTED_SLOT_MEMORY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_SET_THE_X_SLOT_AS_SELECTED_SLOT_MEMORY unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_COPY_FROM_SELECTED_SLOT_MEMORY_TO_X_NUMERIC_VARIABLE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_COPY_FROM_SELECTED_SLOT_MEMORY_TO_X_NUMERIC_VARIABLE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_COPY_FROM_X_NUMERIC_VARIABLE_TO_SELECTED_SLOT_MEMORY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_COPY_FROM_X_NUMERIC_VARIABLE_TO_SELECTED_SLOT_MEMORY unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_COPY_FROM_SELECTED_ANIMATION_MEMORY_TO_X_NUMERIC_VARIABLE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_COPY_FROM_SELECTED_ANIMATION_MEMORY_TO_X_NUMERIC_VARIABLE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_COPY_FROM_X_NUMERIC_VARIABLE_TO_SELECTED_ANIMATION_MEMORY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_COPY_FROM_X_NUMERIC_VARIABLE_TO_SELECTED_ANIMATION_MEMORY unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_CONVERT_THE_X_NGLE_ROOM_INDEX_IN_TOMB_ROOM_INDEX_TO_CURRENTVALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_CONVERT_THE_X_NGLE_ROOM_INDEX_IN_TOMB_ROOM_INDEX_TO_CURRENTVALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_CONVERT_THE_TOMB_ROOM_INDEX_TO_NGLE_ROOM_INDEX_IN_CURRENTVALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_CONVERT_THE_TOMB_ROOM_INDEX_TO_NGLE_ROOM_INDEX_IN_CURRENTVALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_CONVERT_THE_NGLE_ROOM_INDEX_TO_TOMB_ROOM_INDEX_IN_CURRENTVALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_CONVERT_THE_NGLE_ROOM_INDEX_TO_TOMB_ROOM_INDEX_IN_CURRENTVALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_CONVERT_THE_TOMB_ITEM_INDEX_TO_NGLE_ITEM_INDEX_IN_CURRENTVALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_CONVERT_THE_TOMB_ITEM_INDEX_TO_NGLE_ITEM_INDEX_IN_CURRENTVALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_CONVERT_THE_NGLE_ITEM_INDEX_TO_TOMB_ITEM_INDEX_IN_CURRENTVALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_CONVERT_THE_NGLE_ITEM_INDEX_TO_TOMB_ITEM_INDEX_IN_CURRENTVALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_SAVE_THE_COORDINATES_AND_FACING_OF_ITEM_INDEX_IN_CURRENTVALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_SAVE_THE_COORDINATES_AND_FACING_OF_ITEM_INDEX_IN_CURRENTVALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_GENERATE_IN_X_NUMERIC_VARIABLE_THE_RANDOM_NUMBER: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_GENERATE_IN_X_NUMERIC_VARIABLE_THE_RANDOM_NUMBER unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_GENERATE_IN_X_NUMERIC_VARIABLE_A_RANDOM_CURRENTVALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_GENERATE_IN_X_NUMERIC_VARIABLE_A_RANDOM_CURRENTVALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_PERFORM_OPERATION_X_NUMERICVARIABLE_AND_NUMBER: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_PERFORM_OPERATION_X_NUMERICVARIABLE_AND_NUMBER unimplemented!");
				return true;
			}
			break;
		}
		case INVENTORY_POP_UP_INVENTORY_TO_SELECT_X_ITEM_IN_WAY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "INVENTORY_POP_UP_INVENTORY_TO_SELECT_X_ITEM_IN_WAY unimplemented!");
				return true;
			}
			break;
		}
		case CUSTOM_BAR_SHOW_THE_X_CUSTOM_BAR_ON_SCREEN_FOR_SECONDS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUSTOM_BAR_SHOW_THE_X_CUSTOM_BAR_ON_SCREEN_FOR_SECONDS unimplemented!");
				return true;
			}
			break;
		}
		case CUSTOM_BAR_HIDE_THE_X_CUSTOM_BAR: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUSTOM_BAR_HIDE_THE_X_CUSTOM_BAR unimplemented!");
				return true;
			}
			break;
		}
		case FLIPMAP_ALTERNATIVE_CONTINOUSLY_THE_X_FLIPMAP_WITH_THE_E_FRAME_INTERVAL: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "FLIPMAP_ALTERNATIVE_CONTINOUSLY_THE_X_FLIPMAP_WITH_THE_E_FRAME_INTERVAL unimplemented!");
				return true;
			}
			break;
		}
		case FLIPMAP_STOP_THE_ALTERNATE_OF_X_FLIPMAP_LETTING_THE_E_FLIPMAP_TYPE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "FLIPMAP_STOP_THE_ALTERNATE_OF_X_FLIPMAP_LETTING_THE_E_FLIPMAP_TYPE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_SET_THE_X_INVENTORY_ITEM_AS_SELECTED_INVENTORY_MEMORY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_SET_THE_X_INVENTORY_ITEM_AS_SELECTED_INVENTORY_MEMORY unimplemented!");
				return true;
			}
			break;
		}
		case VARAIBLES_COPY_FROM_X_NUMERIC_VARIABLE_TO_E_INVENTORY_MEMOY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARAIBLES_COPY_FROM_X_NUMERIC_VARIABLE_TO_E_INVENTORY_MEMOY unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_SET_IN_X_INVENTORY_MEMORY_THE_E_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_SET_IN_X_INVENTORY_MEMORY_THE_E_VALUE unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_SET_IN_X_INVENTORY_MEMORY_THE_E_BIG_NUMBER: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_SET_IN_X_INVENTORY_MEMORY_THE_E_BIG_NUMBER unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_COPY_TO_X_NUMERIC_VARIBLE_THE_E_INVENTORY_MEMORY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_COPY_TO_X_NUMERIC_VARIBLE_THE_E_INVENTORY_MEMORY unimplemented!");
				return true;
			}
			break;
		}
		case LARA_SWAP_MESH_OF_X_LARA_SLOTS_WITH_THAT_OF_E_SLOT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_SWAP_MESH_OF_X_LARA_SLOTS_WITH_THAT_OF_E_SLOT unimplemented!");
				return true;
			}
			break;
		}
		case SWAP_MESH_OF_X_SLOT_WITH_MESH_OF_E_SLOT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "SWAP_MESH_OF_X_SLOT_WITH_MESH_OF_E_SLOT unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_SET_IN_X_CODE_MEMORY_THE_E_NEGATIVE_NUMBER: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_SET_IN_X_CODE_MEMORY_THE_E_NEGATIVE_NUMBER unimplemented!");
				return true;
			}
			break;
		}
		case FISH_CLEAR_ALL_FISH_OF_X_TYPE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "FISH_CLEAR_ALL_FISH_OF_X_TYPE unimplemented!");
				return true;
			}
			break;
		}
		case SWAP_MESH_OF_CURRENT_HORIZONT_OBJECT_WITH_X_SLOT_MESH: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "SWAP_MESH_OF_CURRENT_HORIZONT_OBJECT_WITH_X_SLOT_MESH unimplemented!");
				return true;
			}
			break;
		}
		case TRIGGERGROUP_ENABLE_NEWLY_THE_ONESHOT_X_TRIGGERGROUP_ALREADY_PERFORMED: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TRIGGERGROUP_ENABLE_NEWLY_THE_ONESHOT_X_TRIGGERGROUP_ALREADY_PERFORMED unimplemented!");
				return true;
			}
			break;
		}
		case CAMERA_ENABLE_THE_X_STANDBY_CAMERA_EFFECT_FOR_E_SECONDS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CAMERA_ENABLE_THE_X_STANDBY_CAMERA_EFFECT_FOR_E_SECONDS unimplemented!");
				return true;
			}
			break;
		}
		case ENEMY_FREEZE_ALL_ENEMIES_FOR_X_SECONDS_IN_E_WAY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ENEMY_FREEZE_ALL_ENEMIES_FOR_X_SECONDS_IN_E_WAY unimplemented!");
				return true;
			}
			break;
		}
		case ENEMY_REMOVE_THE_FREEZE_ALL_ENEMIES_MODE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ENEMY_REMOVE_THE_FREEZE_ALL_ENEMIES_MODE unimplemented!");
				return true;
			}
			break;
		}
		case LARA_TRIGGER_CLOCKWISE_WHIRL_AT_X_CENTER_AND_E_DIAMETER: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_TRIGGER_CLOCKWISE_WHIRL_AT_X_CENTER_AND_E_DIAMETER unimplemented!");
				return true;
			}
			break;
		}
		case LARA_TRIGGER_UNCLOCKWISE_WHIRL_AT_X_CENTER_AND_E_DIAMETER: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_TRIGGER_UNCLOCKWISE_WHIRL_AT_X_CENTER_AND_E_DIAMETER unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_CONVERT_FROM_ITEM_ADDRESS_TO_ITEM_INDEX_THE_VALUE_IN_CURRENT_VALUE_VARIABLE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_CONVERT_FROM_ITEM_ADDRESS_TO_ITEM_INDEX_THE_VALUE_IN_CURRENT_VALUE_VARIABLE unimplemented!");
				return true;
			}
			break;
		}
		case STATICS_SCALE_A_STATIC_ITEM_USING_THE_DATA_IN_X_PARAMETERS_COMMAND: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATICS_SCALE_A_STATIC_ITEM_USING_THE_DATA_IN_X_PARAMETERS_COMMAND unimplemented!");
				return true;
			}
			break;
		}
		case STATICS_STOP_THE_ENDLESS_SCALING_OF_THE_STATICS_START_WITH_THE_X_PARAMETERS_COMMAND: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "STATICS_STOP_THE_ENDLESS_SCALING_OF_THE_STATICS_START_WITH_THE_X_PARAMETERS_COMMAND unimplemented!");
				return true;
			}
			break;
		}
		case ITEMGROUP_PERFORM_X_TRIGGER_WITH_E_ITEMGROUP_OF_STATICS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ITEMGROUP_PERFORM_X_TRIGGER_WITH_E_ITEMGROUP_OF_STATICS unimplemented!");
				return true;
			}
			break;
		}
		case SCREEN_FLASH_SCREEN_WITH_LIGHT_COLOR_FOR_DURATION: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "SCREEN_FLASH_SCREEN_WITH_LIGHT_COLOR_FOR_DURATION unimplemented!");
				return true;
			}
			break;		
		}
		case SCREEN_REMOVE_INFINITE_FLASH_EFFECT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "SCREEN_REMOVE_INFINITE_FLASH_EFFECT unimplemented!");
				return true;
			}
			break;
		}
		case SPRITE_SHOW_SPRITE_WITH_DATA_IN_X_PARAMETERS_FOR_DURATION: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "SPRITE_SHOW_SPRITE_WITH_DATA_IN_PARAMETERS_FOR_DURATION unimplemented!");
				return true;
			}
			break;
		}
		case SPRITE_REMOVE_FROM_SCREEN_THE_SPRITE_WITH_DATA_IN_X_PARAMETERS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "SPRITE_REMOVE_FROM_SCREEN_THE_SPRITE_WITH_DATA_IN_X_PARAMETERS unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_PERFORM_A_LIGHTNING_WITH_DATA_IN_X_PARAMETER_FOR_E_DURATE_IN_TICK_FRAMES: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_PERFORM_A_LIGHTNING_WITH_DATA_IN_X_PARAMETER_FOR_E_DURATE_IN_TICK_FRAMES unimplemented!");
				return true;
			}
			break;
		}
		case TEXT_PRINT_UNLIMITED_X_EXTRA_NG_STRING_WITH_CURRENT_SETTINGS_FOR_INFINITE_TIME: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_PRINT_UNLIMITED_X_EXTRA_NG_STRING_WITH_CURRENT_SETTINGS_FOR_INFINITE_TIME unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_SET_IN_ADVANCE_THE_RAIN_SNOW_SETTINGS_USING_THE_INTENSITY_OF_X_ROOM: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_SET_IN_ADVANCE_THE_RAIN_SNOW_SETTINGS_USING_THE_INTENSITY_OF_X_ROOM unimplemented!");
				return true;
			}
			break;
		}
		case WEATHER_STOP_THE_ENDLESS_LIGHTNING_EFFECT_WITH_X_PARAMETERS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "WEATHER_STOP_THE_ENDLESS_LIGHTNING_EFFECT_WITH_X_PARAMETERS unimplemented!");
				return true;
			}
			break;
		}
		case TEXT_PRINT_EXTRA_NG_X_STRING_WITH_WINDOWS_FONT_AND_FORMATTING_DATA_IN_THE_E_PARAMETERS_COMMAND: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_PRINT_EXTRA_NG_X_STRING_WITH_WINDOWS_FONT_AND_FORMATTING_DATA_IN_THE_E_PARAMETERS_COMMAND unimplemented!");
				return true;
			}
			break;
		}
		case TEXT_REMOVE_PRINT_EXTRA_NG_X_STRING_WITH_WINDOWS_FONT_THAT_USED_IN_THE_E_PARAMETERS_COMMAND: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TEXT_REMOVE_PRINT_EXTRA_NG_X_STRING_WITH_WINDOWS_FONT_THAT_USED_IN_THE_E_PARAMETERS_COMMAND unimplemented!");
				return true;
			}
			break;
		}
		case CAMERA_INCREASE_THE_ZOOM_FACTOR_FOR_BINOCULAR: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CAMERA_INCREASE_THE_ZOOM_FACTOR_FOR_BINOCULAR unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_MOVE_X_FUEL_TO_BOAT_TANK_IN_E_WAY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_MOVE_X_FUEL_TO_BOAT_TANK_IN_E_WAY unimplemented!");
				return true;
			}
			break;
		}
		case CAMERA_SHOW_BLACK_SCREEN_FOR_SECONDS_WITH_FINAL_CURTAIN_EFFECT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return camera_show_black_screen_for_seconds_with_final_curtain_effect(action_data_1, action_data_2);
			break;
		}
		case CAMERA_STOP_BLACK_SCREEN_AND_OPEN_CURTAIN: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CAMERA_STOP_BLACK_SCREEN_AND_OPEN_CURTAIN unimplemented!");
				return true;
			}
			break;
		}
		case CAMERA_SET_CINEMA_EFFECT_TYPE_FOR_SECONDS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return camera_set_cinema_effect_type_for_seconds(action_data_1, action_data_2);
			break;
		}
		case ANIMCOMMAND_IF_CURRENT_ITEM_IS_WADING_WATER_ADD_TWIRLS_WITH_X_INTENSITY: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ANIMCOMMAND_IF_CURRENT_ITEM_IS_WADING_WATER_ADD_TWIRLS_WITH_X_INTENSITY unimplemented!");
				return true;
			}
			break;
		}
		case PERFORM_X_TRIGGERGROUP_FROM_SCRIPT_IN_SINGLE_EXECUTION: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return perform_triggergroup_from_script_in_single_execution_mode(action_data_1, action_data_2);
			break;
		}
		case PERFORM_X_TRIGGERGROUP_FROM_SCRIPT_IN_MULTI_EXECUTION: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return perform_triggergroup_from_script_in_multi_execution_mode(action_data_1, action_data_2);
			break;
		}
		case PERFORM_X_TRIGGERGROUP_FROM_SCRIPT_IN_CONTINUOUS_EXECUTION: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return perform_triggergroup_from_script_in_continuous_mode(action_data_1, action_data_2);
			break;
		}
		case GLOBAL_TRIGGER_ENABLE_WITH_ID: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return enable_global_trigger_with_id(action_data_1, action_data_2);
			}
			break;
		}
		case GLOBAL_TRIGGER_DISABLE_WITH_ID: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				return disable_global_trigger_with_id(action_data_1, action_data_2);
			}
			break;
		}
		case ORGANIZER_RESUME_X_ORGANIZER_FROM_THE_FIRST_COMMAND: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ORGANIZER_RESUME_X_ORGANIZER_FROM_THE_FIRST_COMMAND unimplemented!");
				return true;
			}
			break;
		}
		case ORGANIZER_RESUME_X_ORGANIZER_FROM_NEXT_COMMAND_IMMEDIATE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ORGANIZER_RESUME_X_ORGANIZER_FROM_NEXT_COMMAND_IMMEDIATE unimplemented!");
				return true;
			}
			break;
		}
		case ORGANIZER_RESUME_X_ORGANIZER_FROM_NEXT_COMMAND_IN_THE_GIVEN_TIME_SET_IN_NEXT_COMMAND: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "ORGANIZER_RESUME_X_ORGANIZER_FROM_NEXT_COMMAND_IN_THE_GIVEN_TIME_SET_IN_NEXT_COMMAND unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_PERFORM_THE_DEMO_PAK_AT_X_INDEX_OF_DEMO_SCRIPT_COMMAND: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_PERFORM_THE_DEMO_PAK_AT_X_INDEX_OF_DEMO_SCRIPT_COMMAND unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_STOP_THE_CURRENT_DEMO_IN_PROGRESS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_STOP_THE_CURRENT_DEMO_IN_PROGRESS unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_LOOK_AT_LEADING_ACTOR_FROM_X_VIEW_ANGLE_AND_E_DISTANCE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_LOOK_AT_LEADING_ACTOR_FROM_X_VIEW_ANGLE_AND_E_DISTANCE unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_LOOK_EXTRA_ACTOR_FROM_X_VIEW_ANGLE_AND_E_DISTANCE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_LOOK_EXTRA_ACTOR_FROM_X_VIEW_ANGLE_AND_E_DISTANCE unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_LOOK_LARA_FROM_X_VIEW_ANGLE_AND_E_DISTANCE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_LOOK_LARA_FROM_X_VIEW_ANGLE_AND_E_DISTANCE unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_RESET_CUTSCENE_CAMERA_AND_COME_BACK_TO_LARAS_CHASE_CAMERA: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_RESET_CUTSCENE_CAMERA_AND_COME_BACK_TO_LARAS_CHASE_CAMERA unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_MOVE_UP_CUTSCENE_CAMERA_OF_X_CLICKS_IN_E_DEMO_FRAMES: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_MOVE_UP_CUTSCENE_CAMERA_OF_X_CLICKS_IN_E_DEMO_FRAMES unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_MOVE_DOWN_CUTSCENE_CAMERA_OF_X_CLICKS_IN_E_DEMO_FRAMES: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_MOVE_DOWN_CUTSCENE_CAMERA_OF_X_CLICKS_IN_E_DEMO_FRAMES unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_ZOOM_IN_CUTSCENE_CAMERA_COVERING_X_DISTANCE_IN_E_DEMO_FRAMES: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_ZOOM_IN_CUTSCENE_CAMERA_COVERING_X_DISTANCE_IN_E_DEMO_FRAMES unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_ZOOM_OUT_CUTSCENE_CAMERA_COVERING_X_DISTANCE_IN_E_DEMO_FRAMES: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_ZOOM_OUT_CUTSCENE_CAMERA_COVERING_X_DISTANCE_IN_E_DEMO_FRAMES unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_ROTATE_AT_RIGHT_CUTSCENE_CAMERA_AROUND_TARGET_BY_X_DEGREES_IN_E_DEMO_FRAMES: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_ROTATE_AT_RIGHT_CUTSCENE_CAMERA_AROUND_TARGET_BY_X_DEGREES_IN_E_DEMO_FRAMES unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_ROTATE_AT_LEFT_CUTSCENE_CAMERA_AROUND_TARGET_BY_X_DEGREES_IN_E_DEMO_FRAMES: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_ROTATE_AT_LEFT_CUTSCENE_CAMERA_AROUND_TARGET_BY_X_DEGREES_IN_E_DEMO_FRAMES unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_FREEZE_CUTSCENE_CAMERA_FOR_X_DEMO_FRAMES: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_FREEZE_CUTSCENE_CAMERA_FOR_X_DEMO_FRAMES unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_REMOVE_FREEZE_FROM_CUTSCENE_CAMERA: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_REMOVE_FREEZE_FROM_CUTSCENE_CAMERA unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_SWAP_ANIMATIONS_SET_IN_DATA_OF_X_PARAMETER_COMMAND_FOR_E_ACTOR: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_SWAP_ANIMATIONS_SET_IN_DATA_OF_X_PARAMETER_COMMAND_FOR_E_ACTOR unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_FREE_CUTSCENE_X_RESOURCES: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_FREE_CUTSCENE_X_RESOURCES unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_ACTOR_WITH_X_ROLE_WILL_LOOK_AT_LARA: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_ACTOR_WITH_X_ROLE_WILL_LOOK_AT_LARA unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_LARA_WILL_LOOK_ACTOR_WITH_X_ROLE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_LARA_WILL_LOOK_ACTOR_WITH_X_ROLE unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_SWAP_VON_CROY_X_MESH_WITH_SAME_MESH_OF_E_SLOT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_SWAP_VON_CROY_X_MESH_WITH_SAME_MESH_OF_E_SLOT unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_SWAP_VON_CROY_X_DATA_WITH_E_VALUE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_SWAP_VON_CROY_X_DATA_WITH_E_VALUE unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_SET_X_COLOR_E_POSITION_OF_LARAS_TEXTS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_SET_X_COLOR_E_POSITION_OF_LARAS_TEXTS unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_SET_X_COLOR_AND_E_POSITION_FOR_LEADING_ACTORS_TEXTS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_SET_X_COLOR_AND_E_POSITION_FOR_LEADING_ACTORS_TEXTS unimplemented!");
				return true;
			}
			break;
		}
		case CUTSCENE_SET_X_COLOR_AND_E_POSITION_FOR_EXTRA_ACTORS_TEXTS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CUTSCENE_SET_X_COLOR_AND_E_POSITION_FOR_EXTRA_ACTORS_TEXTS unimplemented!");
				return true;
			}
			break;
		}
		case SPRITE_STOP_THE_ANIMATED_SPRITES_WITH_DATA_IN_X_PARAMETER_AT_E_FRAME: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "SPRITE_STOP_THE_ANIMATED_SPRITES_WITH_DATA_IN_X_PARAMETER_AT_E_FRAME unimplemented!");
				return true;
			}
			break;
		}
		case SPRITE_RESUME_ANIMATION_OF_SPRITES_WITH_DATA_IN_X_PARAMETER: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "SPRITE_RESUME_ANIMATION_OF_SPRITES_WITH_DATA_IN_X_PARAMETER unimplemented!");
				return true;
			}
			break;
		}
		case TRIGGER_SECRET: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return trigger_secret(action_data_1, action_data_2);
			break;
		}
		case FMV_PLAY_X_FMV: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "FMV_PLAY_X_FMV unimplemented!");
				return true;
			}
			break;
		}
		case CAMERA_GET_OR_REMOVE_X_INFINITE_DURATE_FOR_CURRENT_CAMERA: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "CAMERA_GET_OR_REMOVE_X_INFINITE_DURATE_FOR_CURRENT_CAMERA unimplemented!");
				return true;
			}
			break;
		}
		case SET_LARA_HOLSTER_TYPE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy))
				return set_lara_holsters(action_data_1, action_data_2);
			break;
		}
		case LARA_SET_CURRENT_SELECTED_WEAPON_FOR_X: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_SET_CURRENT_SELECTED_WEAPON_FOR_X unimplemented!");
				return true;
			}
			break;
		}
		case IMAGES_PERFORM_X_INPUT_BOX_PARAMETERS_AND_WAIT_FOR_PLAYER_INPUT: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "IMAGES_PERFORM_X_INPUT_BOX_PARAMETERS_AND_WAIT_FOR_PLAYER_INPUT unimplemented!");
				return true;
			}
			break;
		}
		case VARIABLES_CLEAR_X_TEXT_VARIABLE: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "VARIABLES_CLEAR_X_TEXT_VARIABLE unimplemented!");
				return true;
			}
			break;
		}
		case LARA_SET_X_OPACITY_LEVEL_OFLARA_FOR_E_SECONDS: {
			if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile() && !NGCheckFlipeffectFloorStatePressedThisFrameOrLastFrame(heavy)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "LARA_SET_X_OPACITY_LEVEL_OFLARA_FOR_E_SECONDS unimplemented!");
				return true;
			}
			break;
		}
		default: {
			if (param < 47) {
				if (skip_checks || !NGIsFlipeffectOneShotTriggeredForTile()) {
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