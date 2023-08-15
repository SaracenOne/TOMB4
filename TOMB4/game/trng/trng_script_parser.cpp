#include "../../tomb4/pch.h"

#include "trng.h"
#include "trng_script_parser.h"

#include "../../tomb4/mod_config.h"

NG_LEVEL ng_levels[MAX_NG_LEVELS];

NG_GLOBAL_TRIGGER current_global_triggers[MAX_NG_GLOBAL_TRIGGERS];
NG_TRIGGER_GROUP current_trigger_groups[MAX_NG_TRIGGER_GROUPS];
NG_ORGANIZER current_organizers[MAX_NG_ORGANIZERS];
NG_ITEM_GROUP current_item_groups[MAX_NG_ITEM_GROUPS];

#define NG_READ_8(scr_buffer, scr_offset) scr_buffer[scr_offset]; \
offset += sizeof(char)

#define NG_READ_16(scr_buffer, scr_offset) (unsigned short)((unsigned char)scr_buffer[scr_offset]) | ((unsigned short)(scr_buffer[scr_offset + 1])) << 8; \
scr_offset += sizeof(short)

#define NG_READ_32(scr_buffer, scr_offset) (unsigned int)(((unsigned char)scr_buffer[scr_offset]) | (((unsigned int)(unsigned char)scr_buffer[scr_offset + 1]) << 8) | (((unsigned int)(unsigned char)scr_buffer[scr_offset + 2]) << 16) | (((unsigned int)(unsigned char)scr_buffer[scr_offset + 3]) << 24)); \
scr_offset += sizeof(int)

void NGInitLevelArray() {
	for (int i = 0; i < MAX_NG_LEVELS; i++) {
		ng_levels[i].records = NULL;
	}
}

void NGFreeLevel(NG_LEVEL& level) {
	if (level.records) {
		level.records->global_trigger_count = 0;
		if (level.records->global_triggers_table) {
			free(level.records->global_triggers_table);
			level.records->global_triggers_table = NULL;
		}

		level.records->trigger_group_count = 0;
		if (level.records->trigger_group_table) {
			free(level.records->trigger_group_table);
			level.records->trigger_group_table = NULL;
		}

		level.records->organizer_count = 0;
		if (level.records->organizer_table) {
			free(level.records->organizer_table);
			level.records->organizer_table = NULL;
		}

		level.records->item_group_count = 0;
		if (level.records->item_group_table) {
			free(level.records->item_group_table);
			level.records->item_group_table = NULL;
		}
	}
}

bool NGReallocateLevel(NG_LEVEL& level, unsigned int global_triggers_table_count, unsigned int trigger_group_table_count, unsigned int organizer_table_count, unsigned int item_group_table_count) {
	NGFreeLevel(level);

	level.records = (NG_LEVEL_RECORD_DATA*)malloc(sizeof(NG_LEVEL_RECORD_DATA));

	if (level.records) {
		level.records->global_triggers_table = NULL;
		level.records->trigger_group_table = NULL;
		level.records->organizer_table = NULL;
		level.records->item_group_table = NULL;
		
		level.records->global_trigger_count = global_triggers_table_count;
		if (global_triggers_table_count) {
			level.records->global_triggers_table = (NG_GLOBAL_TRIGGER_RECORD*)malloc(sizeof(NG_GLOBAL_TRIGGER_RECORD) * global_triggers_table_count);
			if (!level.records->global_triggers_table) {
				NGLog(NG_LOG_TYPE_ERROR, "NGReallocateLevel: Memory allocation failed!");
				return false;
			}
			memset(level.records->global_triggers_table, 0x00, sizeof(NG_GLOBAL_TRIGGER_RECORD) * global_triggers_table_count);
		}

		level.records->trigger_group_count = trigger_group_table_count;
		if (trigger_group_table_count) {
			level.records->trigger_group_table = (NG_TRIGGER_GROUP_RECORD*)malloc(sizeof(NG_TRIGGER_GROUP_RECORD) * trigger_group_table_count);
			if (!level.records->trigger_group_table) {
				NGLog(NG_LOG_TYPE_ERROR, "NGReallocateLevel: Memory allocation failed!");
				return false;
			}
			memset(level.records->trigger_group_table, 0x00, sizeof(NG_TRIGGER_GROUP_RECORD) * trigger_group_table_count);
		}


		level.records->organizer_count = organizer_table_count;
		if (organizer_table_count) {
			level.records->organizer_table = (NG_ORGANIZER_RECORD*)malloc(sizeof(NG_ORGANIZER_RECORD) * organizer_table_count);
			if (!level.records->organizer_table) {
				NGLog(NG_LOG_TYPE_ERROR, "NGReallocateLevel: Memory allocation failed!");
				return false;
			}
			memset(level.records->organizer_table, 0x00, sizeof(NG_ORGANIZER_RECORD) * organizer_table_count);
		}

		level.records->item_group_count = item_group_table_count;
		if (item_group_table_count) {
			level.records->item_group_table = (NG_ITEM_GROUP_RECORD*)malloc(sizeof(NG_ITEM_GROUP_RECORD) * item_group_table_count);
			if (!level.records->item_group_table) {
				NGLog(NG_LOG_TYPE_ERROR, "NGReallocateLevel: Memory allocation failed!");
				return false;
			}
			memset(level.records->item_group_table, 0x00, sizeof(NG_ITEM_GROUP) * item_group_table_count);
		}

		return true;
	} else {
		NGLog(NG_LOG_TYPE_ERROR, "NGReallocateLevel: Memory allocation failed!");
		return false;
	}
}

void NGLoadTablesForLevel(unsigned int level) {
	memset(&current_global_triggers, 0x00, sizeof(NG_GLOBAL_TRIGGER) * MAX_NG_GLOBAL_TRIGGERS);
	memset(&current_trigger_groups, 0x00, sizeof(NG_TRIGGER_GROUP) * MAX_NG_TRIGGER_GROUPS);
	memset(&current_organizers, 0x00, sizeof(NG_ORGANIZER) * MAX_NG_ORGANIZERS);
	memset(&current_item_groups, 0x00, sizeof(NG_ITEM_GROUP) * MAX_NG_ITEM_GROUPS);

	if (ng_levels[level].records) {
		for (int i = 0; i < ng_levels[level].records->global_trigger_count; i++) {
			unsigned int id = ng_levels[level].records->global_triggers_table[i].record_id;

			memcpy(&current_global_triggers[id], &ng_levels[level].records->global_triggers_table[i].global_trigger, sizeof(NG_GLOBAL_TRIGGER));
		}
		for (int i = 0; i < ng_levels[level].records->trigger_group_count; i++) {
			unsigned int id = ng_levels[level].records->trigger_group_table[i].record_id;

			memcpy(&current_trigger_groups[id], &ng_levels[level].records->trigger_group_table[i].trigger_group, sizeof(NG_TRIGGER_GROUP));
		}
		for (int i = 0; i < ng_levels[level].records->organizer_count; i++) {
			unsigned int id = ng_levels[level].records->organizer_table[i].record_id;

			memcpy(&current_organizers[id], &ng_levels[level].records->organizer_table[i].organizer, sizeof(NG_ORGANIZER));
		}
		for (int i = 0; i < ng_levels[level].records->item_group_count; i++) {
			unsigned int id = ng_levels[level].records->item_group_table[i].record_id;

			memcpy(&current_item_groups[id], &ng_levels[level].records->item_group_table[i].item_group, sizeof(NG_ITEM_GROUP));
		}
	}
}

extern void NGScriptCleanup() {
	for (int i = 0; i < MAX_NG_LEVELS; i++) {
		NGFreeLevel(ng_levels[i]);
	}
}

void NGReadNGGameflowInfo(char* gfScriptFile, unsigned int offset, unsigned int len) {
	bool ng_header_found = false;

	unsigned int footer_ident = NG_READ_32(gfScriptFile, offset);
	if (footer_ident != 0x454c474e) { // NGLE
		return;
	}

	unsigned int footer_offset = NG_READ_32(gfScriptFile, offset);
	offset -= footer_offset;

	unsigned short header_ident = NG_READ_16(gfScriptFile, offset);
	if (header_ident != 0x474e) { // NGLE
		return;
	}

	ng_header_found = true;

	//offset += 3;

	if (ng_header_found) {
		unsigned int options_header_block_start_position = offset;

		unsigned short options_header_block_size = NG_READ_16(gfScriptFile, offset);
		unsigned int options_header_block_end_pos = options_header_block_start_position + (options_header_block_size * sizeof(short));
		unsigned short options_block_unknown_variable = NG_READ_16(gfScriptFile, offset);

		while (1) {
			unsigned int data_block_start_start_position = offset;
			unsigned char current_data_block_size_wide = NG_READ_8(gfScriptFile, offset);

			unsigned char block_type = NG_READ_8(gfScriptFile, offset);

			if (offset >= options_header_block_end_pos) {
				if (offset != options_header_block_end_pos) {
					NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Options header block size mismatch!");
				}
				break;
			}

			switch (block_type) {
				case 0xc8: {
					unsigned char flags = NG_READ_16(gfScriptFile, offset);
					get_game_mod_global_info().show_lara_in_title = flags & 0x40;
					break;
				}
				default: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Unimplemented NG option data block type: %u", block_type);
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
			}

			int command_block_end_position = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
			if (offset != command_block_end_position) {
				NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Command header block size mismatch!");
			}
			offset = command_block_end_position;
		}
		
		offset = options_header_block_end_pos;

		unsigned short second_header_block_size = NG_READ_16(gfScriptFile, offset);
		offset += (second_header_block_size - 1) * sizeof(short);

		int current_level = 0;

		// Allocate the trigger table for each level.
		NG_GLOBAL_TRIGGER_RECORD *level_global_triggers_table = (NG_GLOBAL_TRIGGER_RECORD *)malloc(sizeof(NG_GLOBAL_TRIGGER_RECORD) * MAX_NG_GLOBAL_TRIGGERS);
		NG_TRIGGER_GROUP_RECORD *level_trigger_group_table = (NG_TRIGGER_GROUP_RECORD *)malloc(sizeof(NG_TRIGGER_GROUP_RECORD) * MAX_NG_TRIGGER_GROUPS);
		NG_ORGANIZER_RECORD *level_organizer_table = (NG_ORGANIZER_RECORD *)malloc(sizeof(NG_ORGANIZER_RECORD) * MAX_NG_ORGANIZERS);
		NG_ITEM_GROUP_RECORD *level_item_group_table = (NG_ITEM_GROUP_RECORD*)malloc(sizeof(NG_ITEM_GROUP_RECORD) * MAX_NG_ITEM_GROUPS);

		// Do the levels
		while (1) {
			memset(level_global_triggers_table, 0x00, sizeof(NG_GLOBAL_TRIGGER_RECORD) * MAX_NG_GLOBAL_TRIGGERS);
			memset(level_trigger_group_table, 0x00, sizeof(NG_TRIGGER_GROUP_RECORD) * MAX_NG_TRIGGER_GROUPS);
			memset(level_organizer_table, 0x00, sizeof(NG_ORGANIZER_RECORD) * MAX_NG_ORGANIZERS);
			memset(level_item_group_table, 0x00, sizeof(NG_ITEM_GROUP_RECORD) * MAX_NG_ITEM_GROUPS);

			unsigned int level_global_trigger_count = 0;
			unsigned int level_trigger_group_count = 0;
			unsigned int level_organizer_count = 0;
			unsigned int level_item_group_count = 0;

			unsigned int level_block_start_position = offset;
			unsigned short level_block_size = NG_READ_16(gfScriptFile, offset);
			unsigned short level_block_unknown_variable = NG_READ_16(gfScriptFile, offset);

			if (level_block_size == 0) {
				return;
			}

			// Defaults for TRNG levels
			ng_levels[current_level].old_cd_trigger_system = false;
			ng_levels[current_level].new_audio_system = true;

			unsigned int level_block_end_pos = level_block_start_position + level_block_size * sizeof(short);

			int command_blocks_parsed = 0;
			int command_blocks_failed = 0;

			// Do the commands
			while (1) {
				unsigned int data_block_start_start_position = offset;
				unsigned char current_data_block_size_wide = NG_READ_8(gfScriptFile, offset);

				unsigned char block_type = NG_READ_8(gfScriptFile, offset);

				unsigned int command_block_end_position = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));

				if (offset >= level_block_end_pos) {
					if (offset != level_block_end_pos) {
						NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Level block size mismatch!");
					}
					offset = level_block_end_pos;
					break;
				}

				command_blocks_parsed++;

				switch (block_type) {
					case 0x01: {
						// AssignSlot (WIP)
						unsigned int slot_a = NG_READ_16(gfScriptFile, offset);
						unsigned int slot_b = 0;
						if (get_game_mod_global_info().trng_version_major == 1 && get_game_mod_global_info().trng_version_minor < 3) {
							slot_b = NG_READ_16(gfScriptFile, offset);
						} else {
							slot_b = NG_READ_32(gfScriptFile, offset);
						}

						break;
					}
					case 0x02: {
						// Snow
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Snow unimplemented");

						// Skip to the end
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						break;
					}
					case 0x03: {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: LevelFarView unimplemented");

						// LevelFarView (WIP)
						unsigned short level_far_view = NG_READ_16(gfScriptFile, offset);
						break;
					}
					case 0x04: {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: FogRange unimplemented");

						// FogRange (WIP)
						unsigned short fog_start = NG_READ_16(gfScriptFile, offset);
						unsigned short fog_end = NG_READ_16(gfScriptFile, offset);

						break;
					}
					case 0x06: {
						// TextFormat
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: TextFormat unimplemented");
						// Skip to the end
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						break;
					}
					case 0x07: {
						// Rain
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Rain unimplemented");
						// Skip to the end
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						break;
					}
					case 0x08: {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Damage unimplemented");

						// Damage (WIP)
						unsigned short damage_flags = NG_READ_16(gfScriptFile, offset);
						unsigned short seconds_for_death = NG_READ_16(gfScriptFile, offset);
						unsigned short seconds_for_bar_restore = NG_READ_16(gfScriptFile, offset);
						unsigned int bar_color = NG_READ_32(gfScriptFile, offset);
						unsigned short bar_name = NG_READ_16(gfScriptFile, offset);
						unsigned short blink_percentage = NG_READ_16(gfScriptFile, offset);

						break;
					}
					case 0x09: {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Enemy unimplemented");

						// Enemy (WIP)
						unsigned short slot = NG_READ_16(gfScriptFile, offset);
						unsigned short hp = NG_READ_16(gfScriptFile, offset);
						unsigned short nef_flags = NG_READ_16(gfScriptFile, offset);
						unsigned short tomb_flags = NG_READ_16(gfScriptFile, offset);
						unsigned short extra_flags = NG_READ_16(gfScriptFile, offset);
						unsigned short damage_1 = NG_READ_16(gfScriptFile, offset);
						unsigned short damage_2 = NG_READ_16(gfScriptFile, offset);
						unsigned short damage_3 = NG_READ_16(gfScriptFile, offset);

						break;
					}
					case 0x0a: {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Animation unimplemented");

						// Animation (WIP)
						unsigned short anim_index = NG_READ_16(gfScriptFile, offset);
						unsigned short key_1 = NG_READ_16(gfScriptFile, offset);
						unsigned short key_2 = NG_READ_16(gfScriptFile, offset);
						unsigned short fan_flags = NG_READ_16(gfScriptFile, offset);
						unsigned short env_environment = NG_READ_16(gfScriptFile, offset);
						unsigned short distance_for_env = NG_READ_16(gfScriptFile, offset);
						unsigned short extra = NG_READ_16(gfScriptFile, offset);

						// TODO: Figure out the array format
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));

						break;
					}
					case 0x0b: {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: MirrorEffect Unimplemented!");

						// MirrorEffect (WIP)
						unsigned short in_front_room = NG_READ_16(gfScriptFile, offset);
						unsigned short hidden_room = NG_READ_16(gfScriptFile, offset);
						unsigned short mirror_type = NG_READ_16(gfScriptFile, offset);

						// The rest of it should be an array of animatings:
						while ((offset != command_block_end_position)) {
							unsigned short animating_index = NG_READ_16(gfScriptFile, offset);
						}
						break;
					}
					case 0x0c: {
						// Elevator
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Elevator Unimplemented!");

						// Skip to the end
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						break;
					}
					case 0x0d: {
						// Keypad
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Keypad Unimplemented!");

						// Skip to the end
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						break;
					}
					case 0x0e: {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: AddEffect Unimplemented!");

						// AddEffect
						unsigned short id = NG_READ_16(gfScriptFile, offset);
						unsigned short effect_type = NG_READ_16(gfScriptFile, offset); // Obsolete
						unsigned short flags_effects = NG_READ_16(gfScriptFile, offset);
						unsigned short joint_type = NG_READ_16(gfScriptFile, offset);
						short disp_x = NG_READ_16(gfScriptFile, offset);
						short disp_y = NG_READ_16(gfScriptFile, offset);
						short disp_z = NG_READ_16(gfScriptFile, offset);
						unsigned short durate_emit = NG_READ_16(gfScriptFile, offset);
						unsigned short durate_pause = NG_READ_16(gfScriptFile, offset);

						switch ((NG_ADD_EFFECT_TYPE)effect_type) {
							case NG_ADD_MIST: {
								if (offset == command_block_end_position)
									break;
								unsigned short size_of_mist_ball = NG_READ_16(gfScriptFile, offset);
								if (offset == command_block_end_position)
									break;
								unsigned short number_of_mist_balls = NG_READ_16(gfScriptFile, offset);
								if (offset == command_block_end_position)
									break;
								unsigned short color_of_mist = NG_READ_16(gfScriptFile, offset);
								if (offset == command_block_end_position)
									break;
								unsigned short persistence_of_mist = NG_READ_16(gfScriptFile, offset);
								if (offset == command_block_end_position)
									break;
								break;
							}
							case NG_ADD_LIGHT_BLINK:
							case NG_ADD_LIGHT_FLAT:
							case NG_ADD_LIGHT_GLOVE:
							case NG_ADD_LIGHT_SPOT: {
								if (offset == command_block_end_position)
									break;
								unsigned short light_intensity = NG_READ_16(gfScriptFile, offset);
								if (offset == command_block_end_position)
									break;
								unsigned short maximum_spotlight_distance = NG_READ_16(gfScriptFile, offset);
								if (offset == command_block_end_position)
									break;
								unsigned short light_color = NG_READ_16(gfScriptFile, offset);
								if (offset == command_block_end_position)
									break;
								break;
							}
							case NG_ADD_FLAME: {
								if (offset == command_block_end_position)
									break;
								unsigned short flame_intensity = NG_READ_16(gfScriptFile, offset);
								if (offset == command_block_end_position)
									break;
								unsigned short lara_burn_settings = NG_READ_16(gfScriptFile, offset);
								if (offset == command_block_end_position)
									break;
								unsigned short flame_direction = NG_READ_16(gfScriptFile, offset);
								if (offset == command_block_end_position)
									break;
								unsigned short flame_unknown = NG_READ_16(gfScriptFile, offset);
								if (offset == command_block_end_position)
									break;
								break;
							}
						}
						break;
					}
					case 0x0f: {
						// Detector
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Detector Unimplemented!");

						// Skip to the end
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						break;
					}
					case 0x11: {
						// TextureSequence
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: TextureSequence Unimplemented!");
						// Skip to the end
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						break;
					}
					case 0x12: {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Equipment unimplemented");

						// Equipment (WIP)
						unsigned short slot_item = NG_READ_16(gfScriptFile, offset);
						unsigned short amount = NG_READ_16(gfScriptFile, offset);

						break;
					}
					case 0x13: {
						// MultiEnvCondition (WIP)
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: MultiEnvCondition Unimplemented!");

						// Skip to the end
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						break;
					}
					case 0x14: {
						// Customize (WIP)
						unsigned int customization_category = 0;
						if (get_game_mod_global_info().trng_version_major == 1 && get_game_mod_global_info().trng_version_minor < 3) {
							customization_category = NG_READ_16(gfScriptFile, offset);
						} else {
							customization_category = NG_READ_32(gfScriptFile, offset);
						}
						switch (customization_category) {
							// CUST_DISABLE_SCREAMING_HEAD	
							case 0x0001: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_DISABLE_SCREAMING_HEAD unimplemented");
								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_SET_SECRET_NUMBER	
							case 0x0002: {
								unsigned short secret_count = NG_READ_16(gfScriptFile, offset);
								if (current_level == 0) {
									for (int i = 0; i < MOD_LEVEL_COUNT; i++) {
										get_game_mod_level_stat_info(i).secret_count = secret_count;
									}
								} else {
									get_game_mod_level_stat_info(current_level).secret_count = secret_count;
								}
								break;
							}
							// CUST_SET_CREDITS_LEVEL
							case 0x0003: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SET_CREDITS_LEVEL unimplemented");
								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_DISABLE_FORCING_ANIM_96
							case 0x0004: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_DISABLE_FORCING_ANIM_96 unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_ROLLINGBALL_PUSHING
							case 0x0005: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_ROLLINGBALL_PUSHING unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_NEW_SOUND_ENGINE
							case 0x0006: {
								unsigned short new_sound_engine_flags = NG_READ_16(gfScriptFile, offset);
								if (new_sound_engine_flags == 0xffff || new_sound_engine_flags == 0) {
									new_sound_engine_flags = 0;
								} else {
									NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_NEW_SOUND_ENGINE flags not supported!");
								}

								unsigned short sound_extension = NG_READ_16(gfScriptFile, offset); // Obsolete
								unsigned short long_fade_out = NG_READ_16(gfScriptFile, offset);
								if (long_fade_out != 0xffff) {
									NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_NEW_SOUND_ENGINE custom long_fade_out unsupported!");
								}

								unsigned short short_fade_out = NG_READ_16(gfScriptFile, offset);
								if (short_fade_out != 0xffff) {
									NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_NEW_SOUND_ENGINE custom short_fade_out unsupported!");
								}

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_SPEED_MOVING
							case 0x0007: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SPEED_MOVING unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_SHATTER_RANGE
							case 0x0008: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SHATTER_RANGE unimplemented");


								unsigned short first_static_as_shatter = NG_READ_16(gfScriptFile, offset);
								unsigned short last_static_as_shatter = NG_READ_16(gfScriptFile, offset);
								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_WEAPON
							case 0x0009: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_WEAPON unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_AMMO
							case 0x000a: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_AMMO unimplemented");

								unsigned short ammo_slot = NG_READ_16(gfScriptFile, offset);
								unsigned short ammo_flags = NG_READ_16(gfScriptFile, offset);
								unsigned short damage = NG_READ_16(gfScriptFile, offset);
								unsigned short shots_for_box = NG_READ_16(gfScriptFile, offset);
								unsigned short shots_with_weapon = NG_READ_16(gfScriptFile, offset);
								unsigned short extra = NG_READ_16(gfScriptFile, offset);
								unsigned short trigger_group_when_hit_enemy = NG_READ_16(gfScriptFile, offset);
								unsigned short damage_for_explosion = NG_READ_16(gfScriptFile, offset);
								unsigned short speed = NG_READ_16(gfScriptFile, offset);
								unsigned short gravity = NG_READ_16(gfScriptFile, offset);
								unsigned short id_add_effect_to_ammo = NG_READ_16(gfScriptFile, offset);
								unsigned short id_trigger_group_at_end = 0xffff;
								if (offset < command_block_end_position) {
									id_trigger_group_at_end = NG_READ_16(gfScriptFile, offset);
								}
								break;
							}
							// CUST_SHOW_AMMO_COUNTER
							case 0x000b: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SHOW_AMMO_COUNTER unimplemented");

								unsigned short color = NG_READ_16(gfScriptFile, offset);
								unsigned short format_flags = NG_READ_16(gfScriptFile, offset); // Obsolete
								unsigned short blink_time = NG_READ_16(gfScriptFile, offset);
								unsigned short size_character = NG_READ_16(gfScriptFile, offset);
								unsigned short show_counter_flags = 0xffff;
								if (offset < command_block_end_position) {
									show_counter_flags = NG_READ_16(gfScriptFile, offset);
								}
								break;
							}
							// CUST_SET_INV_ITEM
							case 0x000c: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SET_INV_ITEM unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_SET_JEEP_KEY_SLOT
							case 0x000d: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SET_JEEP_KEY_SLOT unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_STATIC_TRANSPARENCY
							case 0x000e: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_STATIC_TRANSPARENCY unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_SET_STATIC_DAMAGE
							case 0x000f: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SET_STATIC_DAMAGE unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_LOOK_TRASPARENT
							case 0x0010: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_LOOK_TRASPARENT unimplemented");

								unsigned short is_enabled = NG_READ_16(gfScriptFile, offset);
								break;
							}
							// CUST_HAIR_TYPE
							case 0x0011: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_HAIR_TYPE unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_KEEP_DEAD_ENEMIES
							case 0x0012: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_KEEP_DEAD_ENEMIES unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_SET_OLD_CD_TRIGGER
							case 0x0013: {
								ng_levels[current_level].old_cd_trigger_system = NG_READ_8(gfScriptFile, offset);
								break;
							}
							// CUST_ESCAPE_FLY_CAMERA
							case 0x0014: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_ESCAPE_FLY_CAMERA unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_PAUSE_FLY_CAMERA
							case 0x0015: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_PAUSE_FLY_CAMERA unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_TEXT_ON_FLY_SCREEN
							case 0x0016: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_TEXT_ON_FLY_SCREEN unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_CD_SINGLE_PLAYBACK
							case 0x0017: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_CD_SINGLE_PLAYBACK unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_ADD_DEATH_ANIMATION
							case 0x0018: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_ADD_DEATH_ANIMATION unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_BAR 
							case 0x0019: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_ADD_DEATH_ANIMATION unimplemented");

								printf("CUST_BAR unimplemented!\n");
								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_NO_TIME_IN_SAVELIST
							case 0x001a: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_NO_TIME_IN_SAVELIST unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_PARALLEL_BARS
							case 0x001b: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_PARALLEL_BARS unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_CAMERA
							case 0x001c: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_CAMERA unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_DISABLE_MISSING_SOUNDS
							case 0x001d: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_DISABLE_MISSING_SOUNDS unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_INNER_SCREENSHOT
							case 0x001e: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_INNER_SCREENSHOT unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_FMV_CUTSCENE
							case 0x001f: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_FMV_CUTSCENE unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_FIX_WATER_FOG_BUG 
							case 0x0020: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_FIX_WATER_FOG_BUG unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_SAVE_LOCUST 
							case 0x0021: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SAVE_LOCUST unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_LIGHT_OBJECT 
							case 0x0022: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_LIGHT_OBJECT unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_HARPOON
							case 0x0023: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_HARPOON unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_SCREENSHOT_CAPTURE
							case 0x0024: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SCREENSHOT_CAPTURE unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_RAIN
							case 0x0025: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_RAIN unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_TR5_UNDERWATER_COLLISIONS 
							case 0x0026: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_TR5_UNDERWATER_COLLISIONS unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_DARTS 
							case 0x0027: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_DARTS unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_FLARE 
							case 0x0028: {
								unsigned short flare_flags = NG_READ_16(gfScriptFile, offset);
								unsigned short flare_lifetime_in_seconds = NG_READ_16(gfScriptFile, offset);
								unsigned char flare_light_r = NG_READ_16(gfScriptFile, offset);
								unsigned char flare_light_g = NG_READ_16(gfScriptFile, offset);
								unsigned char flare_light_b = NG_READ_16(gfScriptFile, offset);
								unsigned char flare_light_intensity = NG_READ_16(gfScriptFile, offset);

								if (current_level == 0) {
									for (int i = 0; i < MOD_LEVEL_COUNT; i++) {
										if (flare_light_r != 0xff && flare_light_g != 0xff && flare_light_b != 0xff) {
											get_game_mod_level_flare_info(i).light_color_r = flare_light_r;
											get_game_mod_level_flare_info(i).light_color_g = flare_light_g;
											get_game_mod_level_flare_info(i).light_color_b = flare_light_b;
										}
										if (flare_light_intensity != 0xff)
											get_game_mod_level_flare_info(i).light_intensity = flare_light_intensity;
										if (flare_lifetime_in_seconds != 0xffff)
											get_game_mod_level_flare_info(i).flare_lifetime_in_ticks = flare_lifetime_in_seconds * 30;
										if (flare_flags != 0xffff) {
											get_game_mod_level_flare_info(i).has_sparks = flare_flags & 0x0001;
											get_game_mod_level_flare_info(i).has_fire = flare_flags & 0x0002; // Unsupported
											if (get_game_mod_level_flare_info(i).has_fire)
												NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Flare fire effect unimplemented!");
											get_game_mod_level_flare_info(i).sparks_include_smoke = flare_flags & 0x0004;
											get_game_mod_level_flare_info(i).has_glow = flare_flags & 0x0008;
											get_game_mod_level_flare_info(i).flat_light = flare_flags & 0x0010;
										}
									}
								} else {
									if (flare_light_r != 0xff && flare_light_g != 0xff && flare_light_b != 0xff) {
										get_game_mod_level_flare_info(current_level).light_color_r = flare_light_r;
										get_game_mod_level_flare_info(current_level).light_color_g = flare_light_g;
										get_game_mod_level_flare_info(current_level).light_color_b = flare_light_b;
									}
									if (flare_light_intensity != 0xff)
										get_game_mod_level_flare_info(current_level).light_intensity = flare_light_intensity;
									if (flare_lifetime_in_seconds != 0xffff)
										get_game_mod_level_flare_info(current_level).flare_lifetime_in_ticks = flare_lifetime_in_seconds * 30;
									if (flare_flags != 0xffff) {
										get_game_mod_level_flare_info(current_level).has_sparks = flare_flags & 0x0001;
										get_game_mod_level_flare_info(current_level).has_fire = flare_flags & 0x0002; // Unsupported
										get_game_mod_level_flare_info(current_level).sparks_include_smoke = flare_flags & 0x0004;
										get_game_mod_level_flare_info(current_level).has_glow = flare_flags & 0x0008;
										get_game_mod_level_flare_info(current_level).flat_light = flare_flags & 0x0010;
									}
								}

								break;
							}
							 // CUST_SET_TEXT_COLOR 
							case 0x0029: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SET_TEXT_COLOR unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							 // CUST_SET_STILL_COLLISION 
							case 0x002a: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SET_STILL_COLLISION unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							 // CUST_WATERFALL_SPEED 
							case 0x002b: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_WATERFALL_SPEED unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							 // CUST_ROLLING_BOAT 
							case 0x002c: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_ROLLING_BOAT unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							 // CUST_SFX 
							case 0x002d: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SFX unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							 // CUST_TITLE_FMV 
							case 0x002e: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_TITLE_FMV unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							 // CUST_KEEP_LARA_HP 
							case 0x002f: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_KEEP_LARA_HP unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_BINOCULARS
							case 0x0030: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_BINOCULARS unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_BACKGROUND
							case 0x0031: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_BACKGROUND unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_DISABLE_PUSH_AWAY_ANIMATION
							case 0x0032: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_DISABLE_PUSH_AWAY_ANIMATION unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_SLOT_FLAGS
							case 0x0034: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SLOT_FLAGS unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_FIX_BUGS
							case 0x0035: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_FIX_BUGS unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							// CUST_SHATTER_SPECIFIC
							case 0x0036: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SHATTER_SPECIFIC unimplemented");

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
							default: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Unimplemented NG customization category: %u", customization_category);

								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
								break;
							}
						}

						int command_block_end_position = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));

						if (offset != command_block_end_position) {
							NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Customize block size mismatch for category %u", customization_category);
						}

						// Skip to the end
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						break;
					}
					case 0x15:
						// TriggerGroup (legacy?)
						// Older builds of TRNG seem to use this opcode for TriggerGroups. Not sure why it changed though.
						if (get_game_mod_global_info().trng_version_major == 1 && get_game_mod_global_info().trng_version_minor < 3) {
							// TriggerGroup (WIP)
							unsigned short id = NG_READ_16(gfScriptFile, offset);

							if (id > MAX_NG_TRIGGER_GROUPS) {
								NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: TriggerGroup id is not valid!");
								return;
								// Broken
							}

							level_trigger_group_table[level_trigger_group_count].record_id = id;

							unsigned char data_index = 0;
							while (offset < data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short))) {
								unsigned short first_field = NG_READ_16(gfScriptFile, offset);
								// I assume this indicates the end of the command.
								if (first_field == 0x0000) {
									break;
								}
								unsigned short second_field = NG_READ_16(gfScriptFile, offset);
								unsigned short third_field = NG_READ_16(gfScriptFile, offset);

								level_trigger_group_table[level_trigger_group_count].trigger_group.data[data_index].first_field = first_field;
								level_trigger_group_table[level_trigger_group_count].trigger_group.data[data_index].second_field = second_field;
								level_trigger_group_table[level_trigger_group_count].trigger_group.data[data_index].third_field = third_field;

								data_index++;
								if (data_index > NG_TRIGGER_GROUP_DATA_SIZE) {
									NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: TriggerGroup size overflow!");
									return;
								}
							}
							level_trigger_group_count++;
						} else {
							NGLog(NG_LOG_TYPE_PRINT, "NGReadNGGameflowInfo: Encountered a legacy(?) TriggerGroup opcode in an TRNG 1.3+ level.");
							offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						}
						break;
					case 0x16: {
						// Global Trigger
						unsigned short id = NG_READ_16(gfScriptFile, offset);

						if (id > MAX_NG_GLOBAL_TRIGGERS) {
							NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Global Trigger id is not valid!");
							return;
							// Broken
						}

						level_global_triggers_table[level_global_trigger_count].record_id = id;

						unsigned short flags = NG_READ_16(gfScriptFile, offset);
						if (flags == 0xffff)
							flags = 0;

						level_global_triggers_table[level_global_trigger_count].global_trigger.flags = flags;

						unsigned short global_trigger_type = NG_READ_16(gfScriptFile, offset);
						if (global_trigger_type != 0x000b && global_trigger_type != 0x0020) {
							NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Unimplmented GlobalTrigger type %u", global_trigger_type);
						}
						level_global_triggers_table[level_global_trigger_count].global_trigger.type = global_trigger_type;

						level_global_triggers_table[level_global_trigger_count].global_trigger.parameter = NG_READ_32(gfScriptFile, offset);
						level_global_triggers_table[level_global_trigger_count].global_trigger.condition_trigger_group = NG_READ_16(gfScriptFile, offset);
						level_global_triggers_table[level_global_trigger_count].global_trigger.perform_trigger_group = NG_READ_16(gfScriptFile, offset);
						// The block may end here on older version of TRNG
						if (offset < command_block_end_position) {
							level_global_triggers_table[level_global_trigger_count].global_trigger.on_false_trigger_group = NG_READ_16(gfScriptFile, offset);
						} else {
							level_global_triggers_table[level_global_trigger_count].global_trigger.on_false_trigger_group = 0xffff;
						}

						level_global_trigger_count++;

						break;
					}
					case 0x17: {
						// Organizer
						// 2 bytes - ID
						// 2 bytes - flags (0x01 - FO_ENABLED, 0x02 - FO_LOOP, 0x04 - FO_TICK_TIME)
						// 2 bytes - dummy (?)
						// 2 bytes - execution time
						// 2 bytes - trigger group
						// ... cont.

						unsigned short id = NG_READ_16(gfScriptFile, offset);

						if (id > MAX_NG_ORGANIZERS) {
							NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Organizer id is not valid!");
							return;
							// Broken
						}

						level_organizer_table[level_organizer_count].record_id = id;

						unsigned short flags = NG_READ_16(gfScriptFile, offset);;
						if (flags == 0xffff)
							flags = 0;

						level_organizer_table[level_organizer_count].organizer.flags = flags;
						level_organizer_table[level_organizer_count].organizer.parameters = NG_READ_16(gfScriptFile, offset);
						if (level_organizer_table[level_organizer_count].organizer.parameters != -1) {
							NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Organizer parameters are not supported!");

						}

						level_organizer_table[level_organizer_count].organizer.appointment_count = 0;

						unsigned int index = 0;
						while (offset < data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short))) {
							level_organizer_table[level_organizer_count].organizer.appointments[index].time = NG_READ_16(gfScriptFile, offset);
							// !FO_TICK_TIME
							if (!(flags & 0x04)) {
								level_organizer_table[level_organizer_count].organizer.appointments[index].time *= 30;
							}

							level_organizer_table[level_organizer_count].organizer.appointments[index].trigger_group = NG_READ_16(gfScriptFile, offset);

							index++;

							level_organizer_table[level_organizer_count].organizer.appointment_count = index;

							if (index > NG_ORGANIZER_MAX_APPOINTMENTS) {
								NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Organizer appointment size overflow!");
								return;
							}
						}
						level_organizer_count++;
						break;
					}
					case 0x19: {
						// Item Groups
						unsigned short id = NG_READ_16(gfScriptFile, offset);

						if (id > MAX_NG_ITEM_GROUPS) {
							NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: ItemGroup id is not valid!");

							return;
							// Broken
						}

						level_item_group_table[level_item_group_count].record_id = id;

						unsigned char index = 0;
						while (offset < data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short))) {
							level_item_group_table[level_item_group_count].item_group.item_list[index] = NG_READ_16(gfScriptFile, offset);

							index++;
							if (index > NG_ITEM_GROUP_MAX_LIST) {
								NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: ItemGroup record size overflow!");
								return;
							}
						}
						level_item_group_count++;
						break;
					}
					case 0x1a: {
						// ColorRGB (WIP)
						unsigned short id = NG_READ_16(gfScriptFile, offset);
						unsigned short r = NG_READ_16(gfScriptFile, offset);
						unsigned short g = NG_READ_16(gfScriptFile, offset);
						unsigned short b = NG_READ_16(gfScriptFile, offset);

						break;
					}
					case 0x1b: {
						// Parameters (WIP)
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Parameters is not implemented!");

						unsigned short param_type = NG_READ_16(gfScriptFile, offset);
						switch (param_type) {
							case 0x02: {
								unsigned short id = NG_READ_16(gfScriptFile, offset);
								unsigned short flags = NG_READ_16(gfScriptFile, offset);
								if (flags == 0xffff) {
									flags = 0;
								}
								unsigned short index_item = NG_READ_16(gfScriptFile, offset);
								unsigned short direction = NG_READ_16(gfScriptFile, offset);
								unsigned short distance = NG_READ_16(gfScriptFile, offset);
								unsigned short speed = NG_READ_16(gfScriptFile, offset);
								short moving_sound = NG_READ_16(gfScriptFile, offset);
								short final_sound = NG_READ_16(gfScriptFile, offset);
								break;
							}
							default: {
								NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Parameter type %u not implemented!", param_type);

								// Skip to the end
								offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
							}
						}
						break;
					}
					case 0x1c: {
						// Turbo (WIP)
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Turbo is not implemented");
						// Skip to the end
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						break;
					}
					case 0x1d: {
						// WindowTitle (WIP)
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: WindowTitle is not implemented");

						// Skip to the end
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						break;
					}
					case 0x1e: {
						// TestPosition (WIP)
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: TestPosition is not implemented");

						// Skip to the end
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						break;
					}
					case 0x21: {
						// Diary
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Diary is not implemented");

						// Skip to the end
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						break;
					}
					case 0x22: {
						// Image
						unsigned short image_command_id = NG_READ_16(gfScriptFile, offset);
						unsigned short image_file = NG_READ_16(gfScriptFile, offset);
						unsigned short image_flags = NG_READ_16(gfScriptFile, offset);
						unsigned short effect_time = NG_READ_16(gfScriptFile, offset);
						unsigned short audio_track = NG_READ_16(gfScriptFile, offset);
						unsigned short x_position = NG_READ_16(gfScriptFile, offset);
						unsigned short y_position = NG_READ_16(gfScriptFile, offset);
						unsigned short size_x = NG_READ_16(gfScriptFile, offset);
						unsigned short size_y = NG_READ_16(gfScriptFile, offset);
						
						break;
					}
					case 0x23: {
						// SavegamePanel
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: SavegamePanel is not implemented");

						// Skip to the end
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						break;
					}
					case 0x25: {
						// Switch
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Switch is not implemented");

						// Skip to the end
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						break;
					}
					case 0x26: {
						// CombineItems
						unsigned short first_item = NG_READ_16(gfScriptFile, offset);
						unsigned short second_item = NG_READ_16(gfScriptFile, offset);
						unsigned short final_item = NG_READ_16(gfScriptFile, offset);
						break;
					}
					case 0x27: {
						// Standby
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Standby is not implemented");

						// Skip to the end
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						break;
					}
					case 0x28: {
						// AnimationSlot
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: AnimationSlot is not implemented");

						// Skip to the end
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						break;
					}
					case 0x2a: {
						// Demo
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Demo is not implemented");

						// Skip to the end
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						break;
					}
					case 0x2c: {
						// LaraStartPos
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: LaraStartPos is not implemented");

						// Skip to the end
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						break;
					}
					case 0x2d: {
						// StaticMIP
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: StaticMIP is not implemented");

						// Skip to the end
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						break;
					}
					case 0x2e: {
						// TriggerGroup (WIP)
						unsigned short id = NG_READ_16(gfScriptFile, offset);

						if (id > MAX_NG_TRIGGER_GROUPS) {
							NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: TriggerGroup is not implemented");

							return;
							// Broken
						}

						level_trigger_group_table[level_trigger_group_count].record_id = id;

						unsigned char data_index = 0;
						while (offset < data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short))) {
							unsigned short first_field = NG_READ_16(gfScriptFile, offset);
							// I assume this indicates the end of the command.
							if (first_field == 0x0000) {
								break;
							}
							unsigned short second_field = NG_READ_16(gfScriptFile, offset);
							unsigned short third_field = NG_READ_16(gfScriptFile, offset);

							level_trigger_group_table[level_trigger_group_count].trigger_group.data[data_index].first_field = first_field;
							level_trigger_group_table[level_trigger_group_count].trigger_group.data[data_index].second_field = second_field;
							level_trigger_group_table[level_trigger_group_count].trigger_group.data[data_index].third_field = third_field;

							data_index++;
							if (data_index > NG_TRIGGER_GROUP_DATA_SIZE) {
								NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: TriggerGroup size overflow!");
								return;
							}
						}
						level_trigger_group_count++;
						break;
					}
					case 0xc9: {
						// Level flags (?)
						unsigned short flags = NG_READ_16(gfScriptFile, offset);
						break;
					}
					default: {
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						command_blocks_failed++;

						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Unimplemented NG level data block type: %u!", block_type);
						break;
					}
				}
				if (offset != command_block_end_position) {
					NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Command block size mismatch for command %u", block_type);
				}
				offset = command_block_end_position;
			}
			// Now save the tables
			NGReallocateLevel(ng_levels[current_level], level_global_trigger_count, level_trigger_group_count, level_organizer_count, level_item_group_count);
			memcpy(ng_levels[current_level].records->global_triggers_table, level_global_triggers_table, sizeof(NG_GLOBAL_TRIGGER_RECORD) * level_global_trigger_count);
			memcpy(ng_levels[current_level].records->trigger_group_table, level_trigger_group_table, sizeof(NG_TRIGGER_GROUP_RECORD) * level_trigger_group_count);
			memcpy(ng_levels[current_level].records->organizer_table, level_organizer_table, sizeof(NG_ORGANIZER_RECORD) * level_organizer_count);
			memcpy(ng_levels[current_level].records->item_group_table, level_item_group_table, sizeof(NG_ITEM_GROUP_RECORD) * level_item_group_count);

			current_level++;
		}

		// Cleanup
		free(level_global_triggers_table);
		free(level_trigger_group_table);
		free(level_organizer_table);
		free(level_item_group_table);
	}
}