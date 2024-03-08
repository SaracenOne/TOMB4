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
#include "../../specific/3dmath.h"
#include "trng_animation.h"
#include "trng_triggergroup.h"
#include "trng_organizer.h"

bool ngle_footer_found = false;
bool is_ngle_level = false;

int ng_floor_id_size = 0;
char *ng_floor_id_table = NULL;

int ng_script_id_count = 0;
NGScriptIDTableEntry ng_script_id_table[NG_SCRIPT_ID_TABLE_SIZE];

int ng_room_remap_count = 0;
NGRoomRemapTableEntry ng_room_remap_table[NG_ROOM_REMAP_TABLE_SIZE];

int ng_static_id_count = 0;
NGStaticTableEntry ng_static_id_table[NG_STATIC_ID_TABLE_SIZE];

#define NGLE_START_SIGNATURE 0x474e
#define NGLE_END_SIGNATURE 0x454c474e

void NGLoadInfo(FILE* level_fp) {
	is_ngle_level = false;
	ngle_footer_found = false;

	memset(&ng_script_id_table, 0x00, NG_SCRIPT_ID_TABLE_SIZE * sizeof(short));
	memset(&ng_room_remap_table, -1, NG_ROOM_REMAP_TABLE_SIZE * sizeof(short));
	memset(&ng_static_id_table, 0x00, NG_STATIC_ID_TABLE_SIZE * sizeof(short));

	ng_script_id_count = 0;
	ng_room_remap_count = 0;
	ng_static_id_count = 0;

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
					ng_script_id_count = target_offset / sizeof(NGScriptIDTableEntry);
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
					ng_static_id_count = target_offset / sizeof(NGStaticTableEntry);
					fread(&ng_static_id_table, 1, target_offset, level_fp);
					break;
				}
				// Level version
				case 0x8024: {
					unsigned short version_info[4] = {};
					for (int i = 0; i < 4; i++) {
						fread(&version_info[i], 1, sizeof(unsigned short), level_fp);
					}
					break;
				}
				// TOM version
				case 0x8025: {
					unsigned short version_info[4] = {};
					for (int i = 0; i < 4; i++) {
						fread(&version_info[i], 1, sizeof(unsigned short), level_fp);
					}
					break;
				}
				// Room remap table
				case 0x8037: {
					int target_offset = (chunk_size * sizeof(short)) - (sizeof(short) * 2);
					ng_room_remap_count = target_offset / sizeof(NGRoomRemapTableEntry);
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
						ng_floor_id_table = (char*)game_malloc(sizeof(char) * ng_floor_id_size);
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
void NGMoveItemByUnits(unsigned short item_id, NG_DIRECTIONS direction, int units) {
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

// Move the item in an angle by the number of units
void NGMoveItemHorizontalByUnits(unsigned short item_id, short angle, int units) {
	int c = (int)units * phd_cos(angle) >> W2V_SHIFT;
	int s = (int)units * phd_sin(angle) >> W2V_SHIFT;

	items[item_id].pos.x_pos += s;
	items[item_id].pos.z_pos += c;
}

// Move the item up or down by the number of units
void NGMoveItemVerticalByUnits(unsigned short item_id, int units) {
	items[item_id].pos.y_pos += units;
}

void NGRotateItemX(unsigned short item_id, short rotation) {
	items[item_id].pos.x_rot += rotation;
}

void NGRotateItemY(unsigned short item_id, short rotation) {
	items[item_id].pos.y_rot += rotation;
}

// Move the item in a direction by the number of units
void NGStaticItemByUnits(unsigned short static_id, NG_DIRECTIONS direction, int units) {
	NGStaticTableEntry* entry = &ng_static_id_table[static_id];
	int room_number = ng_room_remap_table[entry->remapped_room_index].room_index;
	if (room_number >= 0 && room_number < number_rooms) {
		MESH_INFO* mesh = &room[room_number].mesh[entry->mesh_id];

		switch (direction) {
			case NG_NORTH: {
				mesh->z += units;
				return;
			}
			case NG_EAST: {
				mesh->x += units;
				return;
			}
			case NG_SOUTH: {
				mesh->z -= units;
				return;
			}
			case NG_WEST: {
				mesh->x -= units;
				return;
			}
			case NG_UP: {
				mesh->y -= units;
				return;
			}
			case NG_DOWN: {
				mesh->y += units;
				return;
			}
		}
	}
}

void NGMoveStaticHorizontalByUnits(unsigned short static_id, short angle, int units) {
	int c = (int)units * phd_cos(angle) >> W2V_SHIFT;
	int s = (int)units * phd_sin(angle) >> W2V_SHIFT;

	NGStaticTableEntry* entry = &ng_static_id_table[static_id];
	int room_number = ng_room_remap_table[entry->remapped_room_index].room_index;
	if (room_number >= 0 && room_number < number_rooms) {
		MESH_INFO* mesh = &room[room_number].mesh[entry->mesh_id];
		mesh->x += s;
		mesh->z += c;
	}
}

void NGMoveStaticVerticalByUnits(unsigned short static_id, int units) {
	NGStaticTableEntry* entry = &ng_static_id_table[static_id];
	int room_number = ng_room_remap_table[entry->remapped_room_index].room_index;
	if (room_number >= 0 && room_number < number_rooms) {
		MESH_INFO* mesh = &room[room_number].mesh[entry->mesh_id];
		mesh->y += units;
	}
}

void NGRotateStaticX(unsigned short static_id, short rotation) {
	NGStaticTableEntry* entry = &ng_static_id_table[static_id];
	int room_number = ng_room_remap_table[entry->remapped_room_index].room_index;
	if (room_number >= 0 && room_number < number_rooms) {
		NGLog(NG_LOG_TYPE_UNIMPLEMENTED_FEATURE, "NGRotateStaticX unimplemented!");
	}
}

void NGRotateStaticY(unsigned short static_id, short rotation) {
	NGStaticTableEntry* entry = &ng_static_id_table[static_id];
	int room_number = ng_room_remap_table[entry->remapped_room_index].room_index;
	if (room_number >= 0 && room_number < number_rooms) {
		MESH_INFO* mesh = &room[room_number].mesh[entry->mesh_id];
		mesh->y_rot;
	}
}

void NGForceItemAnimation(unsigned short item_id, unsigned int animation) {
	items[item_id].anim_number = objects[items[item_id].object_number].anim_index + animation;
	items[item_id].frame_number = anims[items[item_id].anim_number].frame_base;

	if (items[item_id].current_anim_state != anims[items[item_id].anim_number].current_anim_state)
	{
		items[item_id].current_anim_state = anims[items[item_id].anim_number].current_anim_state;
		// Do we want to force the goal anim state too?
		items[item_id].goal_anim_state = anims[items[item_id].anim_number].current_anim_state;
	}

	// Is required_anim_state_relevant?
	if (items[item_id].required_anim_state == anims[items[item_id].anim_number].current_anim_state)
		items[item_id].required_anim_state = 0;
}

void NGSetup() {	
	NGLoadTablesForLevel(gfCurrentLevel);
	NGSetupExtraState();
}

void NGFrameStart() {
	NGProcessAnimations();

	NGProcessGlobalTriggers(NO_ITEM);
	NGProcessTriggerGroups();
	NGProcessOrganizers();

	NGFrameStartExtraState();
}

void NGFrameFinish() {
	NGFrameFinishExtraState();
}

bool NGUseNGConditionals() {
	MOD_GLOBAL_INFO *global_info = get_game_mod_global_info();

	return global_info->trng_conditionals_enabled && ngle_footer_found;
}

bool NGUseNGFlipEffects() {
	MOD_GLOBAL_INFO *global_info = get_game_mod_global_info();

	return global_info->trng_flipeffects_enabled && ngle_footer_found;
}

bool NGUseNGActions() {
	MOD_GLOBAL_INFO *global_info = get_game_mod_global_info();

	return global_info->trng_actions_enabled && ngle_footer_found;
}

bool NGUseNGAnimCommands() {
	MOD_GLOBAL_INFO *global_info = get_game_mod_global_info();

	return global_info->trng_ng_anim_commands_enabled && ngle_footer_found;
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

bool NGTestSelectedInventoryObjectAndManagementReplaced(int inventory_object_id) {
	return NGProcessGlobalTriggers(inventory_object_id);
}

void NGSetUsedInventoryObject(int inventory_object_id) {
	ng_used_inventory_object_for_frame = inventory_object_id;
}

void NGSetUsedSmallMedipack() {
	ng_used_small_medipack = true;
}

void NGSetUsedLargeMedipack() {
	ng_used_large_medipack = true;
}

void NGInit() {
}

void NGCleanup() {
	NGScriptCleanup();
}

void NGLog(NGLogType type, const char* s, ...) {
#ifndef DEBUG
	if (type == NG_LOG_TYPE_POSSIBLE_INACCURACY)
		return;
#endif

	va_list list;
	char buf[8192];

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
		case NG_LOG_TYPE_POSSIBLE_INACCURACY: {
			Log(0, "NGLogPossibleInaccuracy: %s", buf);
			break;
		}
		case NG_LOG_TYPE_ERROR: {
			Log(0, "NGLogError: %s", buf);
			break;
		}
	}
}