#include "../../tomb4/pch.h"

#include "trng.h"
#include "trng_script_parser.h"

NG_GLOBAL_TRIGGER global_triggers[MAX_NG_GLOBAL_TRIGGERS];
NG_TRIGGER_GROUP trigger_group_table[MAX_NG_TRIGGER_GROUPS];
NG_ORGANIZER organizer_table[MAX_NG_ORGANIZERS];
NG_ITEM_GROUP item_group_table[MAX_NG_ITEM_GROUPS];

NG_LEVEL ng_levels[MAX_NG_LEVELS];

#define NG_READ_8(scr_buffer, scr_offset) scr_buffer[scr_offset]; \
offset += sizeof(char)

#define NG_READ_16(scr_buffer, scr_offset) (unsigned short)((unsigned char)scr_buffer[scr_offset]) | ((unsigned short)(scr_buffer[scr_offset + 1])) << 8; \
scr_offset += sizeof(short)

#define NG_READ_32(scr_buffer, scr_offset) (unsigned int)(((unsigned char)scr_buffer[scr_offset]) | (((unsigned int)(unsigned char)scr_buffer[scr_offset + 1]) << 8) | (((unsigned int)(unsigned char)scr_buffer[scr_offset + 2]) << 16) | (((unsigned int)(unsigned char)scr_buffer[scr_offset + 3]) << 24)); \
scr_offset += sizeof(int)

void NGLoaderHeader(char* gfScriptFile, unsigned int offset, unsigned int len) {
	bool ng_header_found = false;

	while (offset < len) {
		char* ptr = gfScriptFile + offset;

		if (ptr[0] == 0x00) {
			if (offset < len - 3) {
				if (ptr[1] == 'N' && ptr[2] == 'G') {
					offset += 3;
					ng_header_found = true;
					break;
				}
			}
		}

		offset++;
	}

	if (ng_header_found) {
		unsigned short first_header_block_size = NG_READ_16(gfScriptFile, offset);
		offset += (first_header_block_size - 1) * sizeof(short);

		unsigned short second_header_block_size = NG_READ_16(gfScriptFile, offset);
		offset += (second_header_block_size - 1) * sizeof(short);

		int current_level = 0;
		while (1) {
			unsigned int level_block_start_position = offset;
			unsigned short level_block_size = NG_READ_16(gfScriptFile, offset);
			unsigned short unknown_variable = NG_READ_16(gfScriptFile, offset);

			unsigned int level_block_end_pos = level_block_start_position + level_block_size * sizeof(short);

			int command_blocks_parsed = 0;
			int command_blocks_failed = 0;

			while (1) {
				unsigned int data_block_start_start_position = offset;
				unsigned char current_data_block_size_wide = NG_READ_8(gfScriptFile, offset);

				unsigned char block_type = NG_READ_8(gfScriptFile, offset);

				if (current_data_block_size_wide == 0 && block_type == 0) {
					if (offset != level_block_end_pos) {
						printf("Level block size mismatch!\n");
					}
					break;
				}

				command_blocks_parsed++;

				switch (block_type) {
					case 0x01: {
						// AssignSlot (WIP)
						unsigned short slot_a = NG_READ_16(gfScriptFile, offset);
						unsigned int slot_b = NG_READ_32(gfScriptFile, offset);

						break;
					}
					case 0x04: {
						// FogRange (WIP)
						unsigned short fog_start = NG_READ_16(gfScriptFile, offset);
						unsigned short fog_end = NG_READ_16(gfScriptFile, offset);

						break;
					}
					case 0x09: {
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
					case 0x12: {
						// Equipment (WIP)
						unsigned short slot_item = NG_READ_16(gfScriptFile, offset);
						unsigned short amount = NG_READ_16(gfScriptFile, offset);

						break;
					}
					case 0x16: {
						// Global Trigger
						unsigned short id = NG_READ_16(gfScriptFile, offset);

						if (id > MAX_NG_GLOBAL_TRIGGERS) {
							printf("Global Trigger id is not valid!\n");
							return;
							// Broken
						}

						global_triggers[id].flags = NG_READ_16(gfScriptFile, offset);
						global_triggers[id].type = NG_READ_16(gfScriptFile, offset);
						global_triggers[id].parameter = NG_READ_32(gfScriptFile, offset);
						global_triggers[id].condition_trigger_group = NG_READ_16(gfScriptFile, offset);
						global_triggers[id].perform_trigger_group = NG_READ_16(gfScriptFile, offset);
						global_triggers[id].on_false_trigger_group = NG_READ_16(gfScriptFile, offset);

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
							printf("Organizer id is not valid!\n");
							return;
							// Broken
						}

						organizer_table[id].flags = NG_READ_16(gfScriptFile, offset);
						organizer_table[id].parameters = NG_READ_16(gfScriptFile, offset);

						unsigned char index = 0;
						while (offset < data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short))) {
							organizer_table[id].appointments[index].time = NG_READ_16(gfScriptFile, offset);
							organizer_table[id].appointments[index].trigger_group = NG_READ_16(gfScriptFile, offset);

							index++;
							if (index > NG_ORGANIZER_MAX_APPOINTMENTS) {
								printf("Organizer appointment size overflow!\n");
								return;
							}
						}
						break;
					}
					case 0x19: {
						// Item Groups
						unsigned short id = NG_READ_16(gfScriptFile, offset);

						if (id > MAX_NG_ITEM_GROUPS) {
							printf("ItemGroup id is not valid!\n");
							return;
							// Broken
						}

						unsigned char index = 0;
						while (offset < data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short))) {
							item_group_table[id].item_records[index] = NG_READ_16(gfScriptFile, offset);

							index++;
							if (index > NG_ITEM_GROUP_MAX_RECORDS) {
								printf("ItemGroup record size overflow!\n");
								return;
							}
						}

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
					case 0x26: {
						// CombineItems
						unsigned short first_item = NG_READ_16(gfScriptFile, offset);
						unsigned short second_item = NG_READ_16(gfScriptFile, offset);
						unsigned short final_item = NG_READ_16(gfScriptFile, offset);
						break;
					}
					case 0x2e: {
						// TriggerGroup (WIP)
						unsigned short id = NG_READ_16(gfScriptFile, offset);

							if (id > MAX_NG_TRIGGER_GROUPS) {
								printf("TriggerGroup id is not valid!\n");
								return;
								// Broken
							}

						unsigned char index = 0;
						while (offset < data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short))) {
							unsigned short data = NG_READ_16(gfScriptFile, offset);
							trigger_group_table[id].data[index] = data;

							index++;
							if (index > NG_TRIGGER_GROUP_DATA_SIZE) {
								printf("TriggerGroup size overflow!\n");
								return;
							}
						}
						break;
					}
					case 0xc9: {
						// Unknown, ends the level block? (WIP)
						unsigned short unk = NG_READ_16(gfScriptFile, offset);
						break;
					}
					default: {
						offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
						command_blocks_failed++;
						break;
					}
				}
			}
			current_level++;
		}
	}
}