#include "../../tomb4/pch.h"

#include "../../specific/audio.h"
#include "../../specific/function_stubs.h"
#include "../control.h"
#include "../effects.h"
#include "../objects.h"
#include "../lara.h"
#include "../gameflow.h"
#include "trng.h"
#include "trng_extra_state.h"
#include "trng_script_parser.h"

#include "../../tomb4/mod_config.h"
#include "../../specific/file.h"

bool ngle_footer_found = false;
bool is_ngle_level = false;

int ng_floor_id_size = 0;
char *ng_floor_id_table = NULL;

short ng_script_id_table[NG_SCRIPT_ID_TABLE_SIZE];
short ng_room_remap_table[NG_ROOM_REMAP_TABLE_SIZE];
NGStaticTableEntry ng_static_id_table[NG_STATIC_ID_TABLE_SIZE];

#define NGLE_START_SIGNATURE 0x474e
#define NGLE_END_SIGNATURE 0x454c474e

void NGLoadInfo(FILE* level_fp) {
	is_ngle_level = false;
	ngle_footer_found = false;

	memset(&ng_script_id_table, 0x00, NG_SCRIPT_ID_TABLE_SIZE * sizeof(short));
	memset(&ng_room_remap_table, -1, NG_ROOM_REMAP_TABLE_SIZE * sizeof(short));
	memset(&ng_static_id_table, 0x00, NG_STATIC_ID_TABLE_SIZE * sizeof(short));

	long level_version = 0;
	long ngle_ident = 0;
	long ngle_offset = 0;

	ng_floor_id_size = 0;
	ng_floor_id_table = NULL;

	// Check footer for NGLE info
	fseek(level_fp, -8L, SEEK_END);
	fread(&ngle_ident, 1, sizeof(long), level_fp);
	fread(&ngle_offset, 1, sizeof(long), level_fp);

	if (ngle_ident == NGLE_END_SIGNATURE) {
		ngle_footer_found = true;
		fseek(level_fp, -ngle_offset, SEEK_END);

		unsigned short start_ident = 0;
		fread(&start_ident, 1, sizeof(short), level_fp);
		if (start_ident == NGLE_START_SIGNATURE) {
			while (1) {
				unsigned short chunk_size = 0;
				fread(&chunk_size, 1, sizeof(unsigned short), level_fp);
				unsigned short chunk_ident = 0;
				fread(&chunk_ident, 1, sizeof(unsigned short), level_fp);

				if (chunk_size == 0 || chunk_ident == 0) {
					break;
				}

				switch (chunk_ident) {
				// Animated Textures
				case 0x8002: {
					fseek(level_fp, (chunk_size * sizeof(short)) - (sizeof(short) * 2), SEEK_CUR);
					break;
				}
				// Moveables Table
				case 0x8005: {
					int start_position = ftell(level_fp);
					int target_offset = (chunk_size * sizeof(short)) - (sizeof(short) * 2);
					fread(&ng_script_id_table, 1, target_offset, level_fp);
					break;
				}
				// Extra room flags
				case 0x8009: {
					unsigned short room_count;
					fread(&room_count, 1, sizeof(unsigned short), level_fp);

					for (int i = 0; i < room_count; i++) {
						unsigned char flags[8];
						fread(&flags, sizeof(flags), sizeof(unsigned char), level_fp);
					}
					break;
				}
				// Level Flags
				case 0x800d: {
					unsigned int flags;
					fread(&flags, 1, sizeof(unsigned int), level_fp);
					if (flags & 0x02) {
						is_ngle_level = true;
					}

					break;
				}
				// Tex Partial (?)
				case 0x8017: {
					fseek(level_fp, (chunk_size * sizeof(short)) - (sizeof(short) * 2), SEEK_CUR);
					break;
				}
				// Statics Table
				case 0x8021: {
					int target_offset = (chunk_size * sizeof(short)) - (sizeof(short) * 2);
					fread(&ng_static_id_table, 1, target_offset, level_fp);
					break;
				}
				// Level version
				case 0x8024: {
					unsigned short version_info[4];
					for (int i = 0; i < 4; i++) {
						fread(&version_info[i], 1, sizeof(unsigned short), level_fp);
					}
					break;
				}
				// TOM version
				case 0x8025: {
					unsigned short version_info[4];
					for (int i = 0; i < 4; i++) {
						fread(&version_info[i], 1, sizeof(unsigned short), level_fp);
					}
					break;
				}
				// Room remap table
				case 0x8037: {
					int target_offset = (chunk_size * sizeof(short)) - (sizeof(short) * 2);
					fread(&ng_room_remap_table, 1, target_offset, level_fp);
					break;
				}
				// Plugin Names
				case 0x8047: {
					fseek(level_fp, (chunk_size * sizeof(short)) - (sizeof(short) * 2), SEEK_CUR);
					break;
				}
				// Floor ID table
				case 0x8048: {
					ng_floor_id_size = (chunk_size * sizeof(short)) - (sizeof(short) * 2);
					if (ng_floor_id_size > sizeof(short)) {
						ng_floor_id_table = (char*)game_malloc(ng_floor_id_size);
						fread(ng_floor_id_table, 1, ng_floor_id_size, level_fp);
					} else {
						fseek(level_fp, (chunk_size * sizeof(short)) - (sizeof(short) * 2), SEEK_CUR);
					}
					break;
				}
				default: {
					NGLog(NG_LOG_TYPE_PRINT, "NGLoadInfo: Unknown NG level chunk ident: 0x%04x", chunk_ident);
					fseek(level_fp, (chunk_size * sizeof(short)) - (sizeof(short) * 2), SEEK_CUR);
					break;
				}
				}
			}
		}
	}
	else {
		return;
	}
}

// Move the item in a direction by the number of units
void NGMoveItemByUnits(unsigned short item_id, NG_DIRECTIONS direction, unsigned int units) {
	switch (direction) {
		case NG_NORTH: {
			items[item_id].pos.z_pos += units;
			return;
		}
		case NG_EAST: {
			items[item_id].pos.x_pos += units;
			return;
		}
		case NG_SOUTH: {
			items[item_id].pos.z_pos -= units;
			return;
		}
		case NG_WEST: {
			items[item_id].pos.x_pos -= units;
			return;
		}
		case NG_UP: {
			items[item_id].pos.y_pos -= units;
			return;
		}
		case NG_DOWN: {
			items[item_id].pos.y_pos += units;
			return;
		}
	}
}

void NGRotateItemY(unsigned short item_id, short rotation) {
	items[item_id].pos.y_rot += rotation;
}

void NGSetup() {
	S_Reset(); // Reset audio channels.

	SetUsingNewAudioSystem(ng_levels[gfCurrentLevel].new_audio_system);
	SetUsingOldTriggerMode(ng_levels[gfCurrentLevel].old_cd_trigger_system);
	
	NGLoadTablesForLevel(gfCurrentLevel);
	NGSetupExtraState();
}

void NGFrameFinish() {
	NGFrameFinishExtraState();
}

bool NGUseNGConditionals() {
	MOD_GLOBAL_INFO global_info = get_game_mod_global_info();

	return global_info.trng_conditionals_enabled && ngle_footer_found;
}

bool NGUseNGFlipEffects() {
	MOD_GLOBAL_INFO global_info = get_game_mod_global_info();

	return global_info.trng_flipeffects_enabled && ngle_footer_found;
}

bool NGUseNGActions() {
	MOD_GLOBAL_INFO global_info = get_game_mod_global_info();

	return global_info.trng_actions_enabled && ngle_footer_found;
}

bool NGUseNGAnimCommands() {
	MOD_GLOBAL_INFO global_info = get_game_mod_global_info();

	return global_info.trng_ng_anim_commands_enabled && ngle_footer_found;
}

int NGFindIndexForLaraStartPosWithMatchingOCB(unsigned int ocb) {
	for (int i = 0; i < nAIObjects; i++) {
		if (AIObjects[i].object_number == LARA_START_POS && ocb == AIObjects[i].trigger_flags) {
			return i;
		}
	}

	return -1;
}

bool NGLaraHasInfiniteAir() {
	return ng_lara_infinite_air;
}

bool NGTestUseInventoryObjectAndManagementReplaced(short inventory_object_id) {
	return NGProcessGlobalTriggers(inventory_object_id);
}

void NGInit() {
	NGScriptInit();
}

void NGCleanup() {
	NGScriptCleanup();
}

void NGLog(NGLogType type, const char* s, ...) {
	va_list list;
	char buf[4096];

	va_start(list, s);
	vsprintf(buf, s, list);
	va_end(list);

	switch (type) {
		case NG_LOG_TYPE_PRINT: {
			Log(0, "NGLogPrint: %s", buf);
			break;
		}
		case NG_LOG_TYPE_UNIMPLEMENTED_FEATURE: {
			Log(0, "NGLogUnimplementedFeature: %s", buf);
			break;
		}
		case NG_LOG_TYPE_ERROR: {
			Log(0, "NGLogError: %s", buf);
			break;
		}
	}
}