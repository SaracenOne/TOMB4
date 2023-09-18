#include "../../tomb4/pch.h"

#include "../gameflow.h"

#include "trng.h"
#include "trng_condition.h"
#include "trng_script_parser.h"

#include "../../tomb4/mod_config.h"
#include "trng_globaltrigger.h"

char *ng_strings[MAX_NG_STRINGS];
NG_LEVEL ng_levels[MAX_NG_LEVELS];
NG_PLUGIN ng_plugins[MAX_NG_PLUGINS];

NG_GLOBAL_TRIGGER current_global_triggers[MAX_NG_GLOBAL_TRIGGERS];
NG_TRIGGER_GROUP current_trigger_groups[MAX_NG_TRIGGER_GROUPS];
NG_ORGANIZER current_organizers[MAX_NG_ORGANIZERS];
NG_ITEM_GROUP current_item_groups[MAX_NG_ITEM_GROUPS];
NG_ANIMATION current_animations[MAX_NG_ANIMATIONS];
NG_MULTI_ENV_CONDITION current_multi_env_conditions[MAX_NG_MULTI_ENV_CONDITIONS];
NG_TEST_POSITION current_test_positions[MAX_NG_TEST_POSITIONS];

#define NG_READ_8(scr_buffer, scr_offset) scr_buffer[scr_offset]; \
offset += sizeof(char)

#define NG_READ_16(scr_buffer, scr_offset) (unsigned short)((unsigned char)scr_buffer[scr_offset]) | ((unsigned short)(scr_buffer[scr_offset + 1])) << 8; \
scr_offset += sizeof(short)

#define NG_READ_32(scr_buffer, scr_offset) (unsigned int)(((unsigned char)scr_buffer[scr_offset]) | (((unsigned int)(unsigned char)scr_buffer[scr_offset + 1]) << 8) | (((unsigned int)(unsigned char)scr_buffer[scr_offset + 2]) << 16) | (((unsigned int)(unsigned char)scr_buffer[scr_offset + 3]) << 24)); \
scr_offset += sizeof(int)

void NGScriptInit() {
	for (int i = 0; i < MAX_NG_LEVELS; i++) {
		ng_levels[i].records = NULL;
	}

	for (int i = 0; i < MAX_NG_STRINGS; i++) {
		ng_strings[i] = NULL;
	}

	for (int i = 0; i < MAX_NG_PLUGINS; i++) {
		ng_plugins[i].is_enabled;
		ng_plugins[i].plugin_string_id = 0;
	}
}

#define NG_FREE_RECORD(record_name_lowercase) level.records->record_name_lowercase##_count = 0; \
if (level.records->record_name_lowercase##_table) { \
	free(level.records->record_name_lowercase##_table); \
	level.records->record_name_lowercase##_table = NULL; \
}

#define NG_ALLOCATE_RECORD(record_name_lowercase, record_name_uppercase) level.records->record_name_lowercase##_count = record_name_lowercase##_table_count; \
level.records->record_name_lowercase##_table = NULL; \
if (record_name_lowercase##_table_count) { \
	level.records->record_name_lowercase##_table = (NG_##record_name_uppercase##_RECORD*)malloc(sizeof(NG_##record_name_uppercase##_RECORD) * record_name_lowercase##_table_count); \
	if (!level.records->record_name_lowercase##_table) { \
		NGLog(NG_LOG_TYPE_ERROR, "NGReallocateLevel: Memory allocation failed!"); \
		return false; \
	} \
	memset(level.records->record_name_lowercase##_table, 0x00, sizeof(NG_##record_name_uppercase##_RECORD) * record_name_lowercase##_table_count); \
} \

void NGFreeLevel(NG_LEVEL& level) {
	if (level.records) {
		NG_FREE_RECORD(global_trigger);
		NG_FREE_RECORD(trigger_group);
		NG_FREE_RECORD(organizer);
		NG_FREE_RECORD(item_group);
		NG_FREE_RECORD(animation);
		NG_FREE_RECORD(multi_env_condition);
		NG_FREE_RECORD(test_position);
	}
}

bool NGReallocateLevel(
	NG_LEVEL& level,
	unsigned int global_trigger_table_count,
	unsigned int trigger_group_table_count,
	unsigned int organizer_table_count,
	unsigned int item_group_table_count,
	unsigned int animation_table_count, 
	unsigned int multi_env_condition_table_count,
	unsigned int test_position_table_count) {
	NGFreeLevel(level);

	level.records = (NG_LEVEL_RECORD_DATA*)malloc(sizeof(NG_LEVEL_RECORD_DATA));

	if (level.records) {
		NG_ALLOCATE_RECORD(global_trigger, GLOBAL_TRIGGER);
		NG_ALLOCATE_RECORD(trigger_group, TRIGGER_GROUP);
		NG_ALLOCATE_RECORD(organizer, ORGANIZER);
		NG_ALLOCATE_RECORD(item_group, ITEM_GROUP);
		NG_ALLOCATE_RECORD(animation, ANIMATION);
		NG_ALLOCATE_RECORD(multi_env_condition, MULTI_ENV_CONDITION);
		NG_ALLOCATE_RECORD(test_position, TEST_POSITION);

		return true;
	} else {
		NGLog(NG_LOG_TYPE_ERROR, "NGReallocateLevel: Memory allocation failed!");
		return false;
	}
}

char *NGGetString(short string_id) {
	if (string_id >= 0) {
		if (string_id > TXT_NUM_STRINGS) {
			NGLog(NG_LOG_TYPE_ERROR, "Invalid string ID");
		} else {
			return SCRIPT_TEXT(string_id);
		}
	} else {
		short ng_string_id = string_id & ~(0x8000);
		if (ng_string_id < MAX_NG_STRINGS) {
			return ng_strings[ng_string_id];
		} else {
			NGLog(NG_LOG_TYPE_ERROR, "MAX_NG_STRINGS exceeded!");
		}
	}

	return NULL;
}

char *NGGetPluginString(short plugin_id) {
	if (plugin_id < MAX_NG_PLUGINS) {
		if (ng_plugins[plugin_id].is_enabled) {
			return NGGetString(ng_plugins[plugin_id].plugin_string_id);
		}
	} else {
		NGLog(NG_LOG_TYPE_ERROR, "MAX_NG_STRINGS exceeded!");
	}

	return NULL;
}

void NGScriptCleanup() {
	for (int i = 0; i < MAX_NG_LEVELS; i++) {
		NGFreeLevel(ng_levels[i]);
	}

	for (int i = 0; i < MAX_NG_STRINGS; i++) {
		if (ng_strings) {
			free(ng_strings[i]);
			ng_strings[i] = NULL;
		}
	}
}

#define NG_LOAD_RECORD_TABLE(record_name_lowercase, record_name_uppercase) for (int i = 0; i < ng_levels[level].records->record_name_lowercase##_count; i++) { \
unsigned int id = ng_levels[level].records->record_name_lowercase##_table[i].record_id; \
memcpy(&current_##record_name_lowercase##s[id], &ng_levels[level].records->record_name_lowercase##_table[i].record, sizeof(NG_##record_name_uppercase)); \
}

void NGLoadTablesForLevel(unsigned int level) {
	memset(&current_global_triggers, 0x00, sizeof(NG_GLOBAL_TRIGGER)* MAX_NG_GLOBAL_TRIGGERS);
	memset(&current_trigger_groups, 0x00, sizeof(NG_TRIGGER_GROUP)* MAX_NG_TRIGGER_GROUPS);
	memset(&current_organizers, 0x00, sizeof(NG_ORGANIZER)* MAX_NG_ORGANIZERS);
	memset(&current_item_groups, 0x00, sizeof(NG_ITEM_GROUP)* MAX_NG_ITEM_GROUPS);
	memset(&current_animations, 0x00, sizeof(NG_ANIMATION) * MAX_NG_ANIMATIONS);
	memset(&current_multi_env_conditions, 0x00, sizeof(NG_MULTI_ENV_CONDITION) * MAX_NG_MULTI_ENV_CONDITIONS);
	memset(&current_test_positions, 0x00, sizeof(NG_TEST_POSITION) * MAX_NG_TEST_POSITIONS);

	if (ng_levels[level].records) {
		NG_LOAD_RECORD_TABLE(global_trigger, GLOBAL_TRIGGER)
		NG_LOAD_RECORD_TABLE(trigger_group, TRIGGER_GROUP);
		NG_LOAD_RECORD_TABLE(organizer, ORGANIZER);
		NG_LOAD_RECORD_TABLE(item_group, ITEM_GROUP);
		NG_LOAD_RECORD_TABLE(animation, ANIMATION);
		NG_LOAD_RECORD_TABLE(multi_env_condition, MULTI_ENV_CONDITION);
		NG_LOAD_RECORD_TABLE(test_position, TEST_POSITION);
	}
}

void NGSetupFlareCustomization(int current_level,
	unsigned short flare_flags,
	unsigned short flare_lifetime_in_seconds,
	unsigned char flare_light_r,
	unsigned char flare_light_g,
	unsigned char flare_light_b,
	unsigned char flare_light_intensity) {

	MOD_LEVEL_FLARE_INFO* flare_info = get_game_mod_level_flare_info(current_level);

	if (flare_light_r != 0xff && flare_light_g != 0xff && flare_light_b != 0xff) {
		flare_info->light_color_r = flare_light_r;
		flare_info->light_color_g = flare_light_g;
		flare_info->light_color_b = flare_light_b;
	}
	if (flare_light_intensity != 0xff)
		flare_info->light_intensity = flare_light_intensity;
	if (flare_lifetime_in_seconds != 0xffff)
		flare_info->flare_lifetime_in_ticks = flare_lifetime_in_seconds * 30;
	if (flare_flags != 0xffff) {
		flare_info->has_sparks = flare_flags & 0x0001;
		flare_info->has_fire = flare_flags & 0x0002; // Unsupported
		if (flare_info->has_fire)
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Flare fire effect unimplemented!");
		flare_info->sparks_include_smoke = flare_flags & 0x0004;
		flare_info->has_glow = flare_flags & 0x0008;
		flare_info->flat_light = flare_flags & 0x0010;
	}
}

void NGSetupBugfixCustomization(int current_level, unsigned short bug_fix_flags) {
	if (bug_fix_flags & ~(BUGF_TRANSPARENT_WHITE_ON_FOG | BUGF_DART_NO_POISON_LARA | BUGF_LAND_WATER_SFX_ENEMIES)) {
		NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: CUST_FIX_BUGS unknown flags!");
	}

	if (bug_fix_flags & BUGF_TRANSPARENT_WHITE_ON_FOG) {
		NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: BUGF_TRANSPARENT_WHITE_ON_FOG unsupported! (level %u)", current_level);
	}

	if (bug_fix_flags & BUGF_DART_NO_POISON_LARA) {
		get_game_mod_level_misc_info(current_level)->darts_poison_fix = true;
	} else {
		get_game_mod_level_misc_info(current_level)->darts_poison_fix = false;
	}

	if (bug_fix_flags & BUGF_LAND_WATER_SFX_ENEMIES) {
		get_game_mod_level_misc_info(current_level)->enemy_gun_hit_underwater_sfx_fix = true;
	} else {
		get_game_mod_level_misc_info(current_level)->enemy_gun_hit_underwater_sfx_fix = false;
	}
}

int NGReadLevelBlock(char* gfScriptFile, unsigned int offset, NG_LEVEL_RECORD_TABLES *tables, int current_level, int world_far_view) {
	memset(tables->level_global_triggers_table, 0x00, sizeof(NG_GLOBAL_TRIGGER_RECORD) * MAX_NG_GLOBAL_TRIGGERS);
	memset(tables->level_trigger_group_table, 0x00, sizeof(NG_TRIGGER_GROUP_RECORD) * MAX_NG_TRIGGER_GROUPS);
	memset(tables->level_organizer_table, 0x00, sizeof(NG_ORGANIZER_RECORD) * MAX_NG_ORGANIZERS);
	memset(tables->level_item_group_table, 0x00, sizeof(NG_ITEM_GROUP_RECORD) * MAX_NG_ITEM_GROUPS);
	memset(tables->level_animation_table, 0x00, sizeof(NG_ANIMATION_RECORD) * MAX_NG_ANIMATIONS);
	memset(tables->level_multi_env_condition_table, 0x00, sizeof(NG_MULTI_ENV_CONDITION_RECORD) * MAX_NG_MULTI_ENV_CONDITIONS);
	memset(tables->level_test_position_table, 0x00, sizeof(NG_TEST_POSITION_RECORD) * MAX_NG_TEST_POSITIONS);

	tables->level_global_trigger_count = 0;
	tables->level_trigger_group_count = 0;
	tables->level_organizer_count = 0;
	tables->level_item_group_count = 0;
	tables->level_animation_count = 0;
	tables->level_multi_env_condition_count = 0;
	tables->level_test_position_count = 0;

	unsigned int level_block_start_position = offset;
	unsigned short level_block_size = NG_READ_16(gfScriptFile, offset);
	unsigned short level_block_unknown_variable = NG_READ_16(gfScriptFile, offset);

	if (level_block_size == 0) {
		return 0;
	}

	// Defaults for TRNG levels
	get_game_mod_level_audio_info(current_level)->old_cd_trigger_system = false;
	get_game_mod_level_audio_info(current_level)->new_audio_system = true;

	unsigned int level_block_end_pos = level_block_start_position + level_block_size * sizeof(short);

	int command_blocks_parsed = 0;
	int command_blocks_failed = 0;

	NGLog(NG_LOG_TYPE_PRINT, "NGReadNGGameflowInfo: === Parsing Level %u ===", current_level);

	// Do the commands
	while (1) {
		unsigned int data_block_start_start_position = offset;
		unsigned char current_data_block_size_wide = NG_READ_8(gfScriptFile, offset);

		unsigned char block_type = NG_READ_8(gfScriptFile, offset);

		unsigned int command_block_end_position = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));

		if (offset >= level_block_end_pos) {
			if (offset != level_block_end_pos) {
				NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Level block size mismatch! (level %u)", current_level);
			}
			offset = level_block_end_pos;
			break;
		}

		command_blocks_parsed++;

		switch (block_type) {
		case 0x01: {
			// AssignSlot
			unsigned int plugin_id = 0;
			unsigned short slot_a = NG_READ_16(gfScriptFile, offset);
			unsigned short slot_b = 0;
			if (!is_mod_trng_version_equal_or_greater_than_target(1, 3, 0, 0)) {
				slot_b = NG_READ_16(gfScriptFile, offset);
			}
			else {
				slot_b = NG_READ_16(gfScriptFile, offset);
				plugin_id = NG_READ_16(gfScriptFile, offset);
			}
#
			if (plugin_id != 0) {
				char* plugin_string = NGGetPluginString(plugin_id);
				if (plugin_string) {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Plugin (%s) AssignSlot(%u, %u) commands are not currently supported (level %u)", plugin_string, slot_a, slot_b, current_level);
				}
				else {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Plugin (%u) AssignSlot(%u, %u) are not currently supported (level %u)", plugin_id, slot_a, slot_b, current_level);
				}
			}
			else {
				assign_slot_for_level(current_level, slot_a, slot_b);
			}

			break;
		}
		case 0x02: {
			// Snow
			unsigned short snow_type = NG_READ_16(gfScriptFile, offset);
			switch (snow_type) {
			case 0:
				get_game_mod_level_misc_info(current_level)->snow_type = WEATHER_DISABLED;
				break;
			case 1:
				get_game_mod_level_misc_info(current_level)->snow_type = WEATHER_ENABLED_IN_SPECIFIC_ROOMS;
				break;
			case 2:
				get_game_mod_level_misc_info(current_level)->snow_type = WEATHER_ENABLED_ALL_OUTSIDE;
				break;
			default:
				NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: unknown snow type! (level %u)", current_level);
				break;
			}
			break;
		}
		case 0x03: {
			// LevelFarView
			unsigned short far_view = NG_READ_16(gfScriptFile, offset);

			if (far_view > world_far_view)
				far_view = world_far_view;

			get_game_mod_level_misc_info(current_level)->far_view = (unsigned int)far_view * 1024;
			break;
		}
		case 0x04: {
			// FogRange
			// Negative font values appear to control the intensity, but that is currently unsupported
			short fog_start = NG_READ_16(gfScriptFile, offset);
			if (fog_start < 0) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "FogRange: negative fog range values currently unsupported!");
			}
			else {
				get_game_mod_level_misc_info(current_level)->fog_start_range = (unsigned int)fog_start * 1024;
			}

			short fog_end = NG_READ_16(gfScriptFile, offset);
			if (fog_end < 0) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "FogRange: negative fog range values currently unsupported!");
			}
			else {
				get_game_mod_level_misc_info(current_level)->fog_end_range = (unsigned int)fog_end * 1024;
			}
			break;
		}
		case 0x06: {
			// TextFormat
			unsigned short text_color_id = NG_READ_16(gfScriptFile, offset);
			if (text_color_id != 0xffff) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: TextFormat color unimplemented! (level %u)", current_level);
			}
			unsigned short text_format_flags = NG_READ_16(gfScriptFile, offset);
			if (text_format_flags != 0xffff) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: TextFormat flags unimplemented! (level %u)", current_level);
			}

			unsigned short text_blink_time = NG_READ_16(gfScriptFile, offset);
			if (text_blink_time != 0xffff) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: TextFormat blink time unimplemented! (level %u)", current_level);
			}

			unsigned short size_character_menu = NG_READ_16(gfScriptFile, offset);
			if (size_character_menu != 0xffff) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: TextFormat size character menu unimplemented! (level %u)", current_level);
			}

			break;
		}
		case 0x07: {
			// Rain
			unsigned short rain_type = NG_READ_16(gfScriptFile, offset);
			switch (rain_type) {
			case 0:
				get_game_mod_level_misc_info(current_level)->rain_type = WEATHER_DISABLED;
				break;
			case 1:
				get_game_mod_level_misc_info(current_level)->rain_type = WEATHER_ENABLED_IN_SPECIFIC_ROOMS;
				break;
			case 2:
				get_game_mod_level_misc_info(current_level)->rain_type = WEATHER_ENABLED_ALL_OUTSIDE;
				break;
			default:
				NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: unknown rain type! (level %u)", current_level);
				break;
			}
			break;
		}
		case 0x08: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Damage unimplemented! (level %u)", current_level);

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
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Enemy unimplemented! (level %u)", current_level);

			// Enemy (WIP)
			unsigned short slot = NG_READ_16(gfScriptFile, offset);
			unsigned short hp = NG_READ_16(gfScriptFile, offset);
			unsigned short nef_flags = NG_READ_16(gfScriptFile, offset);
			unsigned short tomb_flags = NG_READ_16(gfScriptFile, offset);
			unsigned short extra_flags = NG_READ_16(gfScriptFile, offset);
			if (offset < command_block_end_position) {
				unsigned short damage_1 = NG_READ_16(gfScriptFile, offset);
			}
			if (offset < command_block_end_position) {
				unsigned short damage_2 = NG_READ_16(gfScriptFile, offset);
			}
			if (offset < command_block_end_position) {
				unsigned short damage_3 = NG_READ_16(gfScriptFile, offset);
			}

			break;
		}
		case 0x0a: {
			// Animation (WIP)
			if (tables->level_animation_count >= MAX_NG_ANIMATIONS) {
				NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Animation overflow! (level %u)", current_level);
				return 0;
				// Broken
			}

			tables->level_animation_table[tables->level_animation_count].record_id = tables->level_animation_count;

			tables->level_animation_table[tables->level_animation_count].record.animation_index = NG_READ_16(gfScriptFile, offset);
			tables->level_animation_table[tables->level_animation_count].record.key_1 = NG_READ_16(gfScriptFile, offset);
			tables->level_animation_table[tables->level_animation_count].record.key_2 = NG_READ_16(gfScriptFile, offset);
			tables->level_animation_table[tables->level_animation_count].record.fan_flags = NG_READ_16(gfScriptFile, offset);
			tables->level_animation_table[tables->level_animation_count].record.environment.env_condition = NG_READ_16(gfScriptFile, offset);
			tables->level_animation_table[tables->level_animation_count].record.environment.distance_for_env = NG_READ_16(gfScriptFile, offset);
			tables->level_animation_table[tables->level_animation_count].record.environment.extra = NG_READ_16(gfScriptFile, offset);

			while ((offset != command_block_end_position)) {
				if (tables->level_animation_table[tables->level_animation_count].record.state_or_animation_condition_count >= NG_ANIMATION_CONDTION_MAX_SIZE) {
					NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Animation state/condition overflow! (level %u)", current_level);
					return 0;
					// Broken
				}

				tables->level_animation_table[tables->level_animation_count].record.state_or_animation_condition_array[
					tables->level_animation_table[tables->level_animation_count].record.state_or_animation_condition_count] = NG_READ_16(gfScriptFile, offset);

					tables->level_animation_table[tables->level_animation_count].record.state_or_animation_condition_count++;
			}

			tables->level_animation_count++;

			break;
		}
		case 0x0b: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: MirrorEffect Unimplemented! (level %u)", current_level);

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
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Elevator Unimplemented! (level %u)", current_level);

			// Skip to the end
			offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
			break;
		}
		case 0x0d: {
			// Keypad
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Keypad Unimplemented! (level %u)", current_level);

			// Skip to the end
			offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
			break;
		}
		case 0x0e: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: AddEffect Unimplemented! (level %u)", current_level);

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
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Detector Unimplemented! (level %u)", current_level);

			// Skip to the end
			offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
			break;
		}
		case 0x11: {
			// TextureSequence
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: TextureSequence Unimplemented! (level %u)", current_level);
			// Skip to the end
			offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
			break;
		}
		case 0x12: {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Equipment unimplemented (level %u)", current_level);

			// Equipment (WIP)
			unsigned short slot_item = NG_READ_16(gfScriptFile, offset);
			unsigned short amount = NG_READ_16(gfScriptFile, offset);

			break;
		}
		case 0x13: {
			// MultiEnvCondition (WIP)
			unsigned short id = NG_READ_16(gfScriptFile, offset);

			if (id >= MAX_NG_MULTI_ENV_CONDITIONS) {
				NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Multi Env Condition id is not valid! (level %u)", current_level);
				return 0;
				// Broken
			}

			tables->level_multi_env_condition_table[tables->level_multi_env_condition_count].record_id = id;

			while (offset < command_block_end_position) {
				if (tables->level_multi_env_condition_table[tables->level_multi_env_condition_count].record.env_condition_triplet_count >= NG_MULTI_ENV_CONDITION_MAX_TRIPLETS) {
					NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: MultiEnvCondition triplet overflow! (level %u)", current_level);
					return 0;
					// Broken
				}

				int index = tables->level_multi_env_condition_table[tables->level_multi_env_condition_count].record.env_condition_triplet_count;

				tables->level_multi_env_condition_table[tables->level_multi_env_condition_count].record.env_condition_triplet_array[index].env_condition = NG_READ_16(gfScriptFile, offset);
				tables->level_multi_env_condition_table[tables->level_multi_env_condition_count].record.env_condition_triplet_array[index].distance_for_env = NG_READ_16(gfScriptFile, offset);
				tables->level_multi_env_condition_table[tables->level_multi_env_condition_count].record.env_condition_triplet_array[index].extra = NG_READ_16(gfScriptFile, offset);

				tables->level_multi_env_condition_table[tables->level_multi_env_condition_count].record.env_condition_triplet_count++;
			}

			tables->level_multi_env_condition_count++;

			break;
		}
		case 0x14: {
			// Customize (WIP)
			unsigned int customization_category = 0;
			unsigned int plugin_id = 0;
			if (!is_mod_trng_version_equal_or_greater_than_target(1, 3, 0, 0)) {
				customization_category = NG_READ_16(gfScriptFile, offset);
			}
			else {
				customization_category = NG_READ_16(gfScriptFile, offset);
				plugin_id = NG_READ_16(gfScriptFile, offset);
			}

			if (plugin_id == 0) {
				switch (customization_category) {
				case CUST_DISABLE_SCREAMING_HEAD: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_DISABLE_SCREAMING_HEAD unimplemented (level %u)", current_level);
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_SET_SECRET_NUMBER: {
					unsigned short secret_count = NG_READ_16(gfScriptFile, offset);
					if (current_level == 0) {
						for (int i = 0; i < MOD_LEVEL_COUNT; i++) {
							get_game_mod_level_stat_info(i)->secret_count = secret_count;
						}
					}
					else {
						get_game_mod_level_stat_info(current_level)->secret_count = secret_count;
					}
					break;
				}
				case CUST_SET_CREDITS_LEVEL: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SET_CREDITS_LEVEL unimplemented! (level %u)", current_level);
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_DISABLE_FORCING_ANIM_96: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_DISABLE_FORCING_ANIM_96 unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_ROLLINGBALL_PUSHING: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_ROLLINGBALL_PUSHING unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_NEW_SOUND_ENGINE: {
					unsigned short new_sound_engine_flags = NG_READ_16(gfScriptFile, offset);
					if (new_sound_engine_flags == 0xffff || new_sound_engine_flags == 0) {
						new_sound_engine_flags = 0;
					}
					else {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_NEW_SOUND_ENGINE flags not supported! (level %u)", current_level);
					}

					unsigned short sound_extension = NG_READ_16(gfScriptFile, offset); // Obsolete
					unsigned short long_fade_out = NG_READ_16(gfScriptFile, offset);
					if (long_fade_out != 0xffff) {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_NEW_SOUND_ENGINE custom long_fade_out unsupported! (level %u)", current_level);
					}

					unsigned short short_fade_out = NG_READ_16(gfScriptFile, offset);
					if (short_fade_out != 0xffff) {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_NEW_SOUND_ENGINE custom short_fade_out unsupported! (level %u)", current_level);
					}

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_SPEED_MOVING: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SPEED_MOVING unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_SHATTER_RANGE: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SHATTER_RANGE unimplemented! (level %u)", current_level);


					unsigned short first_static_as_shatter = NG_READ_16(gfScriptFile, offset);
					unsigned short last_static_as_shatter = NG_READ_16(gfScriptFile, offset);
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_WEAPON: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_WEAPON unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_AMMO: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_AMMO unimplemented! (level %u)", current_level);

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
				case CUST_SHOW_AMMO_COUNTER: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SHOW_AMMO_COUNTER unimplemented! (level %u)", current_level);

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
				case CUST_SET_INV_ITEM: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SET_INV_ITEM unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_SET_JEEP_KEY_SLOT: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SET_JEEP_KEY_SLOT unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_STATIC_TRANSPARENCY: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_STATIC_TRANSPARENCY unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_SET_STATIC_DAMAGE: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SET_STATIC_DAMAGE unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_LOOK_TRANSPARENT: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_LOOK_TRASPARENT unimplemented! (level %u)", current_level);

					unsigned short is_enabled = NG_READ_16(gfScriptFile, offset);
					break;
				}
				case CUST_HAIR_TYPE: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_HAIR_TYPE unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_KEEP_DEAD_ENEMIES: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_KEEP_DEAD_ENEMIES unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_SET_OLD_CD_TRIGGER: {
					get_game_mod_level_audio_info(current_level)->old_cd_trigger_system = NG_READ_8(gfScriptFile, offset);
					break;
				}
				case CUST_ESCAPE_FLY_CAMERA: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_ESCAPE_FLY_CAMERA unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_PAUSE_FLY_CAMERA: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_PAUSE_FLY_CAMERA unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_TEXT_ON_FLY_SCREEN: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_TEXT_ON_FLY_SCREEN unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_CD_SINGLE_PLAYBACK: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_CD_SINGLE_PLAYBACK unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_ADD_DEATH_ANIMATION: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_ADD_DEATH_ANIMATION unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_BAR: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_ADD_DEATH_ANIMATION unimplemented! (level %u)", current_level);

					printf("CUST_BAR unimplemented!\n");
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_NO_TIME_IN_SAVELIST: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_NO_TIME_IN_SAVELIST unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_PARALLEL_BARS: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_PARALLEL_BARS unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_CAMERA: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_CAMERA unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_DISABLE_MISSING_SOUNDS: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_DISABLE_MISSING_SOUNDS unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_INNER_SCREENSHOT: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_INNER_SCREENSHOT unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_FMV_CUTSCENE: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_FMV_CUTSCENE unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_FIX_WATER_FOG_BUG: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_FIX_WATER_FOG_BUG unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_SAVE_LOCUST: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SAVE_LOCUST unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_LIGHT_OBJECT: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_LIGHT_OBJECT unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_HARPOON: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_HARPOON unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_SCREENSHOT_CAPTURE: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SCREENSHOT_CAPTURE unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_RAIN: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_RAIN unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_TR5_UNDERWATER_COLLISIONS: {
					if (current_level == 0) {
						for (int i = 0; i < MOD_LEVEL_COUNT; i++) {
							get_game_mod_level_lara_info(i)->use_tr5_swimming_collision = true;
						}
					}
					else {
						get_game_mod_level_lara_info(current_level)->use_tr5_swimming_collision = true;
					}
					break;
				}
				case CUST_DARTS: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_DARTS unimplemented! (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_FLARE: {
					unsigned short flare_flags = NG_READ_16(gfScriptFile, offset);
					unsigned short flare_lifetime_in_seconds = NG_READ_16(gfScriptFile, offset);
					unsigned char flare_light_r = NG_READ_16(gfScriptFile, offset);
					unsigned char flare_light_g = NG_READ_16(gfScriptFile, offset);
					unsigned char flare_light_b = NG_READ_16(gfScriptFile, offset);
					unsigned char flare_light_intensity = NG_READ_16(gfScriptFile, offset);

					if (current_level == 0) {
						for (int i = 0; i < MOD_LEVEL_COUNT; i++) {
							NGSetupFlareCustomization(i, flare_flags, flare_lifetime_in_seconds, flare_light_r, flare_light_g, flare_light_b, flare_light_intensity);
						}
					}
					else {
						NGSetupFlareCustomization(current_level, flare_flags, flare_lifetime_in_seconds, flare_light_r, flare_light_g, flare_light_b, flare_light_intensity);
					}

					break;
				}
				case CUST_SET_TEXT_COLOR: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SET_TEXT_COLOR unimplemented (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_SET_STILL_COLLISION: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SET_STILL_COLLISION unimplemented (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_WATERFALL_SPEED: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_WATERFALL_SPEED unimplemented (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_ROLLING_BOAT: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_ROLLING_BOAT unimplemented (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_SFX: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SFX unimplemented (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_TITLE_FMV: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_TITLE_FMV unimplemented (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_KEEP_LARA_HP: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_KEEP_LARA_HP unimplemented (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_BINOCULARS: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_BINOCULARS unimplemented (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_BACKGROUND: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_BACKGROUND unimplemented (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_DISABLE_PUSH_AWAY_ANIMATION: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_DISABLE_PUSH_AWAY_ANIMATION unimplemented (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_SLOT_FLAGS: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SLOT_FLAGS unimplemented (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				case CUST_FIX_BUGS: {
					unsigned char bug_fix_flags_lower = NG_READ_8(gfScriptFile, offset);
					unsigned char bug_fix_flags_upper = 0;

					if (offset != command_block_end_position) {
						bug_fix_flags_upper = NG_READ_8(gfScriptFile, offset);
						if (bug_fix_flags_upper) {
							NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_FIX_BUGS unknown upper bits! (level %u)", current_level);
						}
					}

					if (current_level == 0) {
						for (int i = 0; i < MOD_LEVEL_COUNT; i++) {
							NGSetupBugfixCustomization(i, (bug_fix_flags_upper << 8) | bug_fix_flags_lower);
						}
					}
					else {
						NGSetupBugfixCustomization(current_level, (bug_fix_flags_upper << 8) | bug_fix_flags_lower);
					}

					// Sometimes this can be one or two bytes, so just set it
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));

					break;
				}
				case CUST_SHATTER_SPECIFIC: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: CUST_SHATTER_SPECIFIC unimplemented (level %u)", current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				default: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Unimplemented NG customization category: %u (level %u)", customization_category, current_level);

					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				}

				int command_block_end_position = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));

				if (offset != command_block_end_position) {
					NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Customize block size mismatch for category %u (level %u)", customization_category, current_level);
				}

				// Skip to the end
				offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
			}
			else {
				if (offset != command_block_end_position) {
					char* plugin_string = NGGetPluginString(plugin_id);
					if (plugin_string) {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Plugin (%s) customizations are not currently supported (level %u)", plugin_string, current_level);
					}
					else {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Plugin (%u) customizations are not currently supported (level %u)", plugin_id, current_level);
					}
				}

				// Skip to the end
				offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
			}
			break;
		}
		case 0x15:
			// TriggerGroup (legacy/plugin)
			// Older builds of TRNG seem to use this opcode for TriggerGroups. Newer ones seem to use it for triggers exported from plugins
			if (!is_mod_trng_version_equal_or_greater_than_target(1, 3, 0, 0)) {
				// TriggerGroup (WIP)
				unsigned short id = NG_READ_16(gfScriptFile, offset);

				if (id >= MAX_NG_TRIGGER_GROUPS) {
					NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: TriggerGroup id is not valid!  (level %u)", current_level);
					return 0;
					// Broken
				}

				tables->level_trigger_group_table[tables->level_trigger_group_count].record_id = id;

				unsigned char data_index = 0;
				while (offset < data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short))) {
					unsigned short first_field = NG_READ_16(gfScriptFile, offset);
					// I assume this indicates the end of the command.
					if (first_field == 0x0000 || first_field == 0xffff) {
						break;
					}
					unsigned short second_field = NG_READ_16(gfScriptFile, offset);
					unsigned short third_field = NG_READ_16(gfScriptFile, offset);

					tables->level_trigger_group_table[tables->level_trigger_group_count].record.data[data_index].plugin_id = 0;
					tables->level_trigger_group_table[tables->level_trigger_group_count].record.data[data_index].first_field = first_field;
					tables->level_trigger_group_table[tables->level_trigger_group_count].record.data[data_index].second_field_lower = second_field;
					tables->level_trigger_group_table[tables->level_trigger_group_count].record.data[data_index].second_field_upper = 0;
					tables->level_trigger_group_table[tables->level_trigger_group_count].record.data[data_index].third_field_lower = third_field;
					tables->level_trigger_group_table[tables->level_trigger_group_count].record.data[data_index].third_field_upper = 0;

					data_index++;
					if (data_index >= NG_TRIGGER_GROUP_DATA_SIZE) {
						NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: TriggerGroup size overflow! (level %u)", current_level);
						return 0;
					}

					tables->level_trigger_group_table[tables->level_trigger_group_count].record.data_size = data_index;
				}
				tables->level_trigger_group_count++;
			}
			else {
				unsigned short id = NG_READ_16(gfScriptFile, offset);

				if (id > MAX_NG_TRIGGER_GROUPS) {
					NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: TriggerGroup id is not valid!  (level %u)", current_level);
					return 0;
					// Broken
				}

				tables->level_trigger_group_table[tables->level_trigger_group_count].record_id = id;

				unsigned char data_index = 0;
				while (offset < data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short))) {
					unsigned short first_field = NG_READ_16(gfScriptFile, offset);
					// I assume this indicates the end of the command.
					if (first_field == 0x0000 || first_field == 0xffff) {
						break;
					}

					unsigned short plugin_id = NG_READ_16(gfScriptFile, offset);
					unsigned short second_field_lower = NG_READ_16(gfScriptFile, offset);
					unsigned short second_field_upper = NG_READ_16(gfScriptFile, offset);
					unsigned short third_field_lower = NG_READ_16(gfScriptFile, offset);
					unsigned short third_field_upper = NG_READ_16(gfScriptFile, offset);

					if (plugin_id != 0) {
						char* plugin_string = NGGetPluginString(plugin_id);
						if (plugin_string) {
							NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TriggerGroup %u - Plugin TriggerGroup plugin:%s, first_field:0x%x, second_field:%u, third_field:0x%x (level %u)",
								id,
								plugin_string,
								first_field,
								((int)second_field_upper << 16 | (int)second_field_lower),
								((int)third_field_upper << 16 | (int)third_field_lower),
								current_level);
						}
						else {
							NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "TriggerGroup %u - Plugin TriggerGroup plugin_id:%u, first_field:0x%x, second_field:%u, third_field:0x%x (level %u)",
								id,
								plugin_id,
								first_field,
								((int)second_field_upper << 16 | (int)second_field_lower),
								((int)third_field_upper << 16 | (int)third_field_lower),
								current_level);
						}
					}

					tables->level_trigger_group_table[tables->level_trigger_group_count].record.data[data_index].plugin_id = plugin_id;
					tables->level_trigger_group_table[tables->level_trigger_group_count].record.data[data_index].first_field = first_field;
					tables->level_trigger_group_table[tables->level_trigger_group_count].record.data[data_index].second_field_lower = second_field_lower;
					tables->level_trigger_group_table[tables->level_trigger_group_count].record.data[data_index].second_field_upper = second_field_upper;
					tables->level_trigger_group_table[tables->level_trigger_group_count].record.data[data_index].third_field_lower = third_field_lower;
					tables->level_trigger_group_table[tables->level_trigger_group_count].record.data[data_index].third_field_upper = third_field_upper;

					data_index++;
					if (data_index >= NG_TRIGGER_GROUP_DATA_SIZE) {
						NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: TriggerGroup size overflow! (level %u)", current_level);
						return 0;
					}

					tables->level_trigger_group_table[tables->level_trigger_group_count].record.data_size = data_index;
				}
				tables->level_trigger_group_count++;
			}
			break;
		case 0x16: {
			// Global Trigger
			unsigned short id = NG_READ_16(gfScriptFile, offset);

			if (id > MAX_NG_GLOBAL_TRIGGERS) {
				NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Global Trigger id is not valid! (level %u)", current_level);
				return 0;
				// Broken
			}

			tables->level_global_triggers_table[tables->level_global_trigger_count].record_id = id;

			unsigned short flags = NG_READ_16(gfScriptFile, offset);
			if (flags == 0xffff)
				flags = 0;

			tables->level_global_triggers_table[tables->level_global_trigger_count].record.flags = flags;

			unsigned short global_trigger_type = NG_READ_16(gfScriptFile, offset);
			if (global_trigger_type != GT_ENEMY_KILLED &&
				global_trigger_type != GT_LARA_HP_LESS_THAN &&
				global_trigger_type != GT_LARA_HP_HIGHER_THAN &&
				global_trigger_type != GT_LARA_POISONED &&
				global_trigger_type != GT_CONDITION_GROUP &&
				global_trigger_type != GT_COLLIDE_ITEM &&
				global_trigger_type != GT_COLLIDE_SLOT &&
				global_trigger_type != GT_COLLIDE_CREATURE &&
				global_trigger_type != GT_COLLIDE_STATIC_SLOT &&
				global_trigger_type != GT_ALWAYS &&
				global_trigger_type != GT_TRNG_G_TIMER_EQUALS &&
				global_trigger_type != GT_TRNG_L_TIMER_EQUALS &&
				global_trigger_type != GT_SELECTED_INVENTORY_ITEM) {
				NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Unimplemented GlobalTrigger type %u (level %u)", global_trigger_type, current_level);
			}
			tables->level_global_triggers_table[tables->level_global_trigger_count].record.type = global_trigger_type;

			tables->level_global_triggers_table[tables->level_global_trigger_count].record.parameter = NG_READ_32(gfScriptFile, offset);
			tables->level_global_triggers_table[tables->level_global_trigger_count].record.condition_trigger_group = NG_READ_16(gfScriptFile, offset);
			tables->level_global_triggers_table[tables->level_global_trigger_count].record.perform_trigger_group = NG_READ_16(gfScriptFile, offset);
			// The block may end here on older version of TRNG
			if (offset < command_block_end_position) {
				tables->level_global_triggers_table[tables->level_global_trigger_count].record.on_false_trigger_group = NG_READ_16(gfScriptFile, offset);
			}
			else {
				tables->level_global_triggers_table[tables->level_global_trigger_count].record.on_false_trigger_group = 0xffff;
			}

			tables->level_global_trigger_count++;

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
				NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Organizer id is not valid! (level %u)", current_level);
				return 0;
				// Broken
			}

			tables->level_organizer_table[tables->level_organizer_count].record_id = id;

			unsigned short flags = NG_READ_16(gfScriptFile, offset);;
			if (flags == 0xffff)
				flags = 0;

			// FO_DEMO_ORGANIZER
			if (flags & 0x08) {
				NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Organizer FO_DEMO_ORGANIZER flag not supported (level %u)", current_level);
				break;
			}

			tables->level_organizer_table[tables->level_organizer_count].record.flags = flags;
			tables->level_organizer_table[tables->level_organizer_count].record.parameters = NG_READ_16(gfScriptFile, offset);
			if (!(tables->level_organizer_table[tables->level_organizer_count].record.parameters == 0 || tables->level_organizer_table[tables->level_organizer_count].record.parameters == -1)) {
				NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Organizer parameters are not supported! (level %u)", current_level);

			}

			tables->level_organizer_table[tables->level_organizer_count].record.appointment_count = 0;

			unsigned int index = 0;
			unsigned int current_time = 0;
			while (offset < data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short))) {
				int relative_time = NG_READ_16(gfScriptFile, offset);

				// Organizer complete
				if (offset == command_block_end_position) {
					break;
				}

				// !FO_TICK_TIME
				if (!(flags & 0x04)) {
					relative_time *= 30;
				}

				current_time += relative_time;
				tables->level_organizer_table[tables->level_organizer_count].record.appointments[index].time = current_time;
				tables->level_organizer_table[tables->level_organizer_count].record.appointments[index].trigger_group = NG_READ_16(gfScriptFile, offset);

				index++;

				tables->level_organizer_table[tables->level_organizer_count].record.appointment_count = index;

				if (index >= NG_ORGANIZER_MAX_APPOINTMENTS) {
					NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Organizer appointment size overflow! (level %u)", current_level);
					return 0;
				}
			}
			tables->level_organizer_count++;
			break;
		}
		case 0x19: {
			// Item Groups
			unsigned short id = NG_READ_16(gfScriptFile, offset);

			if (id > MAX_NG_ITEM_GROUPS) {
				NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: ItemGroup id is not valid! (level %u)", current_level);

				return 0;
				// Broken
			}

			tables->level_item_group_table[tables->level_item_group_count].record_id = id;

			unsigned char index = 0;
			while (offset < data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short))) {
				tables->level_item_group_table[tables->level_item_group_count].record.item_list[index] = NG_READ_16(gfScriptFile, offset);

				index++;
				if (index >= NG_ITEM_GROUP_MAX_LIST) {
					NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: ItemGroup record size overflow! (level %u)", current_level);
					return 0;
				}
			}
			tables->level_item_group_count++;
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
			unsigned int param_category = 0;
			unsigned int plugin_id = 0;
			if (!is_mod_trng_version_equal_or_greater_than_target(1, 3, 0, 0)) {
				param_category = NG_READ_16(gfScriptFile, offset);
			}
			else {
				param_category = NG_READ_16(gfScriptFile, offset);
				plugin_id = NG_READ_16(gfScriptFile, offset);
			}

			if (plugin_id == 0) {
				switch (param_category) {
					// PARAM_MOVE_ITEM
				case 0x02: {
					unsigned short id = NG_READ_16(gfScriptFile, offset);
					unsigned short flags = NG_READ_16(gfScriptFile, offset);
					if (flags == 0xffff || flags == 0) {
						flags = 0;
					}
					else {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: PARAM_MOVE_ITEM flags unsupported! (level %u)", current_level);
					}
					unsigned short index_item = NG_READ_16(gfScriptFile, offset);
					unsigned short direction = NG_READ_16(gfScriptFile, offset);
					unsigned short distance = NG_READ_16(gfScriptFile, offset);
					unsigned short speed = NG_READ_16(gfScriptFile, offset);
					short moving_sound = NG_READ_16(gfScriptFile, offset);
					short final_sound = NG_READ_16(gfScriptFile, offset);
					short extra = 0;
					if (offset < command_block_end_position) {
						extra = NG_READ_16(gfScriptFile, offset);
					}
					break;
				}
						 // PARAM_ROTATE_ITEM
				case 0x03: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Parameter category PARAM_ROTATE_ITEM not implemented! (level %u)", current_level);

					// Skip to the end
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
						 // PARAM_COLOR_ITEM
				case 0x04: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Parameter category PARAM_COLOR_ITEM not implemented! (level %u)", current_level);

					// Skip to the end
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
						 // PARAM_PRINT_TEXT
				case 0x05: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Parameter category PARAM_PRINT_TEXT not implemented! (level %u)", current_level);

					// Skip to the end
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
						 // PARAM_SET_CAMERA
				case 0x06: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Parameter category PARAM_SET_CAMERA not implemented! (level %u)", current_level);

					// Skip to the end
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
						 // PARAM_BIG_NUMBERS
				case 0x07: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Parameter category PARAM_BIG_NUMBERS not implemented! (level %u)", current_level);

					// Skip to the end
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
						 // PARAM_SCALE_ITEM
				case 0x08: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Parameter category PARAM_SCALE_ITEM not implemented! (level %u)", current_level);

					// Skip to the end
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
						 // PARAM_SHOW_SPRITE
				case 0x09: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Parameter category PARAM_SHOW_SPRITE not implemented! (level %u)", current_level);

					// Skip to the end
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
						 // PARAM_TRIANGLE
				case 0x0a: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Parameter category PARAM_TRIANGLE not implemented! (level %u)", current_level);

					// Skip to the end
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
						 // PARAM_QUADRILATERAL
				case 0x0b: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Parameter category PARAM_QUADRILATERAL not implemented! (level %u)", current_level);

					// Skip to the end
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
						 // PARAM_CIRCLE
				case 0x0c: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Parameter category PARAM_CIRCLE not implemented! (level %u)", current_level);

					// Skip to the end
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
						 // PARAM_LIGHTNING
				case 0x0d: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Parameter category PARAM_LIGHTNING not implemented! (level %u)", current_level);

					// Skip to the end
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
						 // PARAM_WTEXT
				case 0x0e: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Parameter category PARAM_WTEXT not implemented! (level %u)", current_level);

					// Skip to the end
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
						 // PARAM_RECT
				case 0x0f: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Parameter category PARAM_RECT not implemented! (level %u)", current_level);

					// Skip to the end
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
						 // PARAM_SWAP_ANIMATIONS
				case 0x10: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Parameter category PARAM_SWAP_ANIMATIONS not implemented! (level %u)", current_level);

					// Skip to the end
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
						 // PARAM_ACTOR_SPEECH
				case 0x11: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Parameter category PARAM_ACTOR_SPEECH not implemented! (level %u)", current_level);

					// Skip to the end
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
						 // PARAM_INPUT_BOX
				case 0x12: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Parameter category PARAM_INPUT_BOX not implemented! (level %u)", current_level);

					// Skip to the end
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				default: {
					NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Parameter category %u not implemented! (level %u)", param_category, current_level);

					// Skip to the end
					offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
					break;
				}
				}
			}
			else {
				if (offset != command_block_end_position) {
					char* plugin_string = NGGetPluginString(plugin_id);
					if (plugin_string) {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Plugin (%s) parameters are not currently supported (level %u)", plugin_string, current_level);
					}
					else {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Plugin (%u) parameters are not currently supported (level %u)", plugin_id, current_level);
					}
				}

				// Skip to the end
				offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
			}
			break;
		}
		case 0x1c: {
			// Turbo (WIP)
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Turbo is not implemented! (level %u)", current_level);
			// Skip to the end
			offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
			break;
		}
		case 0x1d: {
			// WindowTitle (WIP)
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: WindowTitle is not implemented! (level %u)", current_level);

			// Skip to the end
			offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
			break;
		}
		case 0x1e: {
			// TestPosition (WIP)
			unsigned short id = NG_READ_16(gfScriptFile, offset);

			if (id > MAX_NG_TEST_POSITIONS) {
				NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Test position id is not valid! (level %u)", current_level);
				return 0;
				// Broken
			}

			tables->level_test_position_table[tables->level_test_position_count].record_id = id;

			tables->level_test_position_table[tables->level_test_position_count].record.flags = NG_READ_16(gfScriptFile, offset);
			tables->level_test_position_table[tables->level_test_position_count].record.moveable_slot = NG_READ_16(gfScriptFile, offset);
			tables->level_test_position_table[tables->level_test_position_count].record.x_distance_min = NG_READ_16(gfScriptFile, offset);
			tables->level_test_position_table[tables->level_test_position_count].record.x_distance_max = NG_READ_16(gfScriptFile, offset);
			tables->level_test_position_table[tables->level_test_position_count].record.y_distance_min = NG_READ_16(gfScriptFile, offset);
			tables->level_test_position_table[tables->level_test_position_count].record.y_distance_max = NG_READ_16(gfScriptFile, offset);
			tables->level_test_position_table[tables->level_test_position_count].record.z_distance_min = NG_READ_16(gfScriptFile, offset);
			tables->level_test_position_table[tables->level_test_position_count].record.z_distance_max = NG_READ_16(gfScriptFile, offset);

			tables->level_test_position_table[tables->level_test_position_count].record.h_orient_diff_min = NG_READ_16(gfScriptFile, offset);
			tables->level_test_position_table[tables->level_test_position_count].record.h_orient_diff_max = NG_READ_16(gfScriptFile, offset);
			tables->level_test_position_table[tables->level_test_position_count].record.v_orient_diff_min = NG_READ_16(gfScriptFile, offset);
			tables->level_test_position_table[tables->level_test_position_count].record.v_orient_diff_max = NG_READ_16(gfScriptFile, offset);
			tables->level_test_position_table[tables->level_test_position_count].record.r_orient_diff_min = NG_READ_16(gfScriptFile, offset);
			tables->level_test_position_table[tables->level_test_position_count].record.r_orient_diff_max = NG_READ_16(gfScriptFile, offset);

			tables->level_test_position_count++;
			break;
		}
		case 0x20: {
			// WindowsFont (?)
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: WindowsFont(?) is not implemented! (level %u)", current_level);

			unsigned short id = NG_READ_16(gfScriptFile, offset);
			unsigned short window_font_name = NG_READ_16(gfScriptFile, offset);
			unsigned short windows_font_flags = NG_READ_16(gfScriptFile, offset);
			unsigned short size_font = NG_READ_16(gfScriptFile, offset);
			unsigned short color_rgb_id = NG_READ_16(gfScriptFile, offset);
			unsigned short shadow_color_rgb_id = NG_READ_16(gfScriptFile, offset);

			break;
		}
		case 0x21: {
			// Diary
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Diary is not implemented! (level %u)", current_level);

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
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: SavegamePanel is not implemented (level %u)", current_level);

			// Skip to the end
			offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
			break;
		}
		case 0x25: {
			// Switch
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Switch is not implemented (level %u)", current_level);

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
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Standby is not implemented! (level %u)", current_level);

			// Skip to the end
			offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
			break;
		}
		case 0x28: {
			// AnimationSlot
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: AnimationSlot is not implemented! (level %u)", current_level);

			// Skip to the end
			offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
			break;
		}
		case 0x2a: {
			// Demo
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Demo is not implemented! (level %u)", current_level);

			// Skip to the end
			offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
			break;
		}
		case 0x2c: {
			// LaraStartPos
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: LaraStartPos is not implemented! (level %u)", current_level);

			// Skip to the end
			offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
			break;
		}
		case 0x2d: {
			// StaticMIP
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: StaticMIP is not implemented! (level %u)", current_level);

			// Skip to the end
			offset = data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short));
			break;
		}
		case 0x2e: {
			// TriggerGroup (WIP)
			unsigned short id = NG_READ_16(gfScriptFile, offset);

			NGLog(NG_LOG_TYPE_PRINT, "Triggergroup %u: (level %u)", id, current_level);

			if (id > MAX_NG_TRIGGER_GROUPS) {
				NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: TriggerGroup is not implemented! (level %u)", current_level);

				return 0;
				// Broken
			}

			tables->level_trigger_group_table[tables->level_trigger_group_count].record_id = id;

			unsigned char data_index = 0;
			while (offset < data_block_start_start_position + (current_data_block_size_wide * sizeof(short) + sizeof(short))) {
				unsigned short first_field = NG_READ_16(gfScriptFile, offset);
				// I assume this indicates the end of the command.
				if (first_field == 0x0000 || first_field == 0xffff) {
					break;
				}
				unsigned short second_field = NG_READ_16(gfScriptFile, offset);
				unsigned short third_field = NG_READ_16(gfScriptFile, offset);

				NGLog(NG_LOG_TYPE_PRINT, "0x%04x, 0x%04x, 0x%04x", first_field, second_field, third_field);

				tables->level_trigger_group_table[tables->level_trigger_group_count].record.data[data_index].plugin_id = 0;
				tables->level_trigger_group_table[tables->level_trigger_group_count].record.data[data_index].first_field = first_field;
				tables->level_trigger_group_table[tables->level_trigger_group_count].record.data[data_index].second_field_lower = second_field;
				tables->level_trigger_group_table[tables->level_trigger_group_count].record.data[data_index].second_field_upper = 0;
				tables->level_trigger_group_table[tables->level_trigger_group_count].record.data[data_index].third_field_lower = third_field;
				tables->level_trigger_group_table[tables->level_trigger_group_count].record.data[data_index].third_field_upper = 0;

				data_index++;
				if (data_index > NG_TRIGGER_GROUP_DATA_SIZE) {
					NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: TriggerGroup size overflow! (level %u)", current_level);
					return 0;
				}

				tables->level_trigger_group_table[tables->level_trigger_group_count].record.data_size = data_index;
			}
			tables->level_trigger_group_count++;
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

			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Unimplemented NG level data block type: %u! (level %u)", block_type, current_level);
			break;
		}
		}
		if (offset != command_block_end_position) {
			int size_difference = offset - command_block_end_position;
			NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Level command block size mismatch for command %u! (level %u), off by %i", block_type, current_level, size_difference);
		}
		offset = command_block_end_position;
	}

	return offset;
}

void NGAllocateLevelRecordTablesContent(NG_LEVEL_RECORD_TABLES *tables) {
	tables->level_global_trigger_count = 0;
	tables->level_trigger_group_count = 0;
	tables->level_organizer_count = 0;
	tables->level_item_group_count = 0;
	tables->level_animation_count = 0;
	tables->level_multi_env_condition_count = 0;
	tables->level_test_position_count = 0;

	tables->level_global_triggers_table = (NG_GLOBAL_TRIGGER_RECORD*)malloc(sizeof(NG_GLOBAL_TRIGGER_RECORD) * MAX_NG_GLOBAL_TRIGGERS);
	tables->level_trigger_group_table = (NG_TRIGGER_GROUP_RECORD*)malloc(sizeof(NG_TRIGGER_GROUP_RECORD) * MAX_NG_TRIGGER_GROUPS);
	tables->level_organizer_table = (NG_ORGANIZER_RECORD*)malloc(sizeof(NG_ORGANIZER_RECORD) * MAX_NG_ORGANIZERS);
	tables->level_item_group_table = (NG_ITEM_GROUP_RECORD*)malloc(sizeof(NG_ITEM_GROUP_RECORD) * MAX_NG_ITEM_GROUPS);
	tables->level_animation_table = (NG_ANIMATION_RECORD*)malloc(sizeof(NG_ANIMATION_RECORD) * MAX_NG_ANIMATIONS);
	tables->level_multi_env_condition_table = (NG_MULTI_ENV_CONDITION_RECORD*)malloc(sizeof(NG_MULTI_ENV_CONDITION_RECORD) * MAX_NG_MULTI_ENV_CONDITIONS);
	tables->level_test_position_table = (NG_TEST_POSITION_RECORD*)malloc(sizeof(NG_TEST_POSITION_RECORD) * MAX_NG_TEST_POSITIONS);
}

void NGFreeLevelRecordTablesContent(NG_LEVEL_RECORD_TABLES *tables) {
	free(tables->level_global_triggers_table);
	free(tables->level_trigger_group_table);
	free(tables->level_organizer_table);
	free(tables->level_item_group_table);
	free(tables->level_animation_table);
	free(tables->level_multi_env_condition_table);
	free(tables->level_test_position_table);
}

void NGReadNGGameflowInfo(char *gfScriptFile, unsigned int offset, unsigned int len) {
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
		if (!is_mod_trng_version_equal_or_greater_than_target(0, 0, 0, 0)) {
			NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGHeader found in Script.dat in invalid TRNG version!");
			return;
		}

		// TRNG Stuff
		get_game_mod_global_info()->trng_flipeffects_enabled = true;
		get_game_mod_global_info()->trng_conditionals_enabled = true;
		get_game_mod_global_info()->trng_actions_enabled = true;
		get_game_mod_global_info()->trng_ng_anim_commands_enabled = true;
		get_game_mod_global_info()->trng_rollingball_extended_ocb = true;
		get_game_mod_global_info()->trng_statics_extended_ocb = true;
		get_game_mod_global_info()->trng_pushable_extended_ocb = true;
		if (!is_mod_trng_version_equal_or_greater_than_target(1, 2, 2, 3)) {
			get_game_mod_global_info()->trng_switch_extended_ocb = true;
		}
		get_game_mod_global_info()->trng_hack_allow_meshes_with_exactly_256_vertices = true;

		unsigned int options_header_block_start_position = offset;

		unsigned short options_header_block_size = NG_READ_16(gfScriptFile, offset);
		unsigned int options_header_block_end_pos = options_header_block_start_position + (options_header_block_size * sizeof(short));
		unsigned short options_block_unknown_variable = NG_READ_16(gfScriptFile, offset);

		unsigned short world_far_view = 127;

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
				// WorldFarView
				case 0x05: {
					world_far_view = NG_READ_16(gfScriptFile, offset);
					for (int i = 0; i < MOD_LEVEL_COUNT; i++) {
						get_game_mod_level_misc_info(i)->far_view = (unsigned int)world_far_view * 1024;
					}
					break;
				}
				// Plugin
				case 0x2b: {
					unsigned short plugin_id = NG_READ_16(gfScriptFile, offset);
					if (plugin_id >= MAX_NG_PLUGINS) {
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Plugin id exceeds MAX_NG_PLUGINS!");
						break;
					}

					unsigned short plugin_string_id = NG_READ_16(gfScriptFile, offset);

					unsigned short plugin_settings = NG_READ_16(gfScriptFile, offset);
					if (plugin_settings != 0xffff)
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Plugin settings is not supported!");

					unsigned short disable_array = NG_READ_16(gfScriptFile, offset);
					if (disable_array != 0xffff)
						NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGReadNGGameflowInfo: Plugin disable array is not supported!");

					ng_plugins[plugin_id].is_enabled = true;
					ng_plugins[plugin_id].plugin_string_id = plugin_string_id;

					break;
				}
				// SHOW_LARA_IN_TITLE
				case 0xc8: {
					unsigned short flags = NG_READ_16(gfScriptFile, offset);
					get_game_mod_global_info()->show_lara_in_title = flags & 0x40;
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
				int size_difference = offset - command_block_end_position;
				NGLog(NG_LOG_TYPE_ERROR, "NGReadNGGameflowInfo: Options command block size mismatch for command %u! off by %i", block_type, size_difference);
			}
			offset = command_block_end_position;
		}
		
		offset = options_header_block_end_pos;

		unsigned short second_header_block_size = NG_READ_16(gfScriptFile, offset);
		offset += (second_header_block_size - 1) * sizeof(short);

		int current_level = 0;

		// Allocate the trigger table for each level.
		NG_LEVEL_RECORD_TABLES record_tables;

		NGAllocateLevelRecordTablesContent(&record_tables);

		// Do the levels
		while (1) {
			// Call level blcok load function here!
			offset = NGReadLevelBlock(gfScriptFile, offset, &record_tables, current_level, world_far_view);
			if (offset == 0) {
				return;
			}

			// Now save the tables
			NGReallocateLevel(ng_levels[current_level],
				record_tables.level_global_trigger_count,
				record_tables.level_trigger_group_count,
				record_tables.level_organizer_count,
				record_tables.level_item_group_count,
				record_tables.level_animation_count,
				record_tables.level_multi_env_condition_count,
				record_tables.level_test_position_count);
			
			memcpy(ng_levels[current_level].records->global_trigger_table, record_tables.level_global_triggers_table, sizeof(NG_GLOBAL_TRIGGER_RECORD) * record_tables.level_global_trigger_count);
			memcpy(ng_levels[current_level].records->trigger_group_table, record_tables.level_trigger_group_table, sizeof(NG_TRIGGER_GROUP_RECORD) * record_tables.level_trigger_group_count);
			memcpy(ng_levels[current_level].records->organizer_table, record_tables.level_organizer_table, sizeof(NG_ORGANIZER_RECORD) * record_tables.level_organizer_count);
			memcpy(ng_levels[current_level].records->item_group_table, record_tables.level_item_group_table, sizeof(NG_ITEM_GROUP_RECORD) * record_tables.level_item_group_count);
			memcpy(ng_levels[current_level].records->animation_table, record_tables.level_animation_table, sizeof(NG_ANIMATION_RECORD) * record_tables.level_animation_count);
			memcpy(ng_levels[current_level].records->multi_env_condition_table, record_tables.level_multi_env_condition_table, sizeof(NG_MULTI_ENV_CONDITION_RECORD)* record_tables.level_multi_env_condition_count);
			memcpy(ng_levels[current_level].records->test_position_table, record_tables.level_test_position_table, sizeof(NG_TEST_POSITION_RECORD) * record_tables.level_test_position_count);

			current_level++;
		}

		// Cleanup
		NGFreeLevelRecordTablesContent(&record_tables);
	}
}

void NGReadNGExtraStrings(char *gfLanguageFile, unsigned int offset, unsigned int len) {
	unsigned int footer_ident = NG_READ_32(gfLanguageFile, offset);
	if (footer_ident != 0x454c474e) { // NGLE
		return;
	}

	unsigned int footer_offset = NG_READ_32(gfLanguageFile, offset);
	offset -= footer_offset;

	unsigned short header_ident = NG_READ_16(gfLanguageFile, offset);
	if (header_ident != 0x474e) { // NGLE
		return;
	}

	unsigned short block_len = NG_READ_16(gfLanguageFile, offset);
	unsigned short block_type = NG_READ_16(gfLanguageFile, offset);

	if (block_type != 0x800a) {
		return;
	}

	unsigned short string_count = NG_READ_16(gfLanguageFile, offset);

	for (int i = 0; i < string_count; i++) {
		unsigned int string_id = NG_READ_16(gfLanguageFile, offset);
		unsigned int string_len = NG_READ_16(gfLanguageFile, offset);
		string_len *= 2; // Two byte

		if (string_id < 0 || string_id >= MAX_NG_STRINGS) {
			NGLog(NG_LOG_TYPE_ERROR, "Invalid string ID %u!", string_id);
			continue;
		}

		char *current_string = (char *)malloc(string_len);
		if (current_string) {
			memset(current_string, 0x00, string_len);

			for (unsigned int j = 0; j < string_len; j++) {
				current_string[j] = NG_READ_8(gfLanguageFile, offset);
				if (current_string[j] != 0x00) {
					current_string[j] ^= 0xa5;
				}
			}
			
			// Detect duplicates...
			if (ng_strings[string_id]) {
				free(ng_strings[string_id]);
				ng_strings[string_id] = NULL;
				NGLog(NG_LOG_TYPE_ERROR, "Duplicate string ID %u!", string_id);
			}

			ng_strings[string_id] = current_string;
			NGLog(NG_LOG_TYPE_PRINT, "NGString (%u): %s", string_id, current_string);
		} else {
			NGLog(NG_LOG_TYPE_ERROR, "Failed to allocate memory for string %u!", string_id);
		}
	}
}