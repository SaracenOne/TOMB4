#include "../../tomb4/pch.h"

#include "trng.h"
#include "trng_savegame.h"

#define MAX_NG_SAVEGAME_BUFFER_SIZE 0x8000

uint32_t ng_savegame_buffer_size = 0;
char ng_savegame_buffer[MAX_NG_SAVEGAME_BUFFER_SIZE];

bool NGIsNGSavegame() {
	return ng_savegame_buffer_size > 0;
}

void NGReadNGSavegameInfo() {
	size_t offset = 0;

	if (NGIsNGSavegame()) {
		uint16_t header_ident = NG_READ_16(ng_savegame_buffer, offset);
		if (header_ident != NGLE_START_SIGNATURE) { // NGLE
			return;
		}

		while (1) {
			size_t start_offset = offset;

			uint16_t block_size = NG_READ_16(ng_savegame_buffer, offset);

			if (offset + (block_size - sizeof(uint16_t)) >= ng_savegame_buffer_size) {
				NGLog(NG_LOG_TYPE_ERROR, "NGReadNGSavegameBuffer: overflow!");
				return;
			}

			uint16_t block_type = NG_READ_16(ng_savegame_buffer, offset);

			if (block_type == 0) {
				return;
			}

			switch (block_type) {
				case 0x8003: { // OLD_EFFECTS
					uint16_t total_old_flipeffects = NG_READ_16(ng_savegame_buffer, offset);
					for (int i = 0; i < total_old_flipeffects; i++) {
						uint16_t flags = NG_READ_16(ng_savegame_buffer, offset);
						uint32_t offset_floor_data = NG_READ_32(ng_savegame_buffer, offset);
					}
					break;
				}
				case 0x8004: { // OLD_FMV
					for (int i = 0; i < 128; i++) {
						uint8_t performed_fmv = NG_READ_8(ng_savegame_buffer, offset);
					}
					break;
				}
				case 0x800E: { // OLD_CONDITION
					break;
				}
				case 0x800F: { // VARIABLE_DATA
					break;
				}
				case 0x8019: { // SWAP_MESH
					break;
				}
				case 0x801A: { // STATUS_GTRIGGERS
					break;
				}
				case 0x801C: { // ROOM_FLAGS
					break;
				}
				case 0x801D: { // WEATHER_INTENSITY
					break;
				}
				case 0x801E: { // STATUS_ORGANIZER
					break;
				}
				case 0x8011: { // PRINT_STRING
					break;
				}
				case 0x8014: { // BLIND_SAVE
					break;
				}
				case 0x8015: { // CASUALE
					break;
				}
				case 0x8023: { // OBJECT_TIMER
					break;
				}
				case 0x8030: { // STATUS_ANIM_RANGES
					break;
				}
				case 0x8031: { // SAVEGAME_INFOS
					break;
				}
				case 0x8038: { // VAR_GLOBAL_TRNG
					break;
				}
				case 0x8039: { // VAR_LOCAL_TRNG
					break;
				}
				case 0x803A: { // FROZEN_ITEMS
					break;
				}
				case 0x803C: { // NO_COLL_ITEMS
					break;
				}
				case 0x8043: { // DIARY_DATA
					break;
				}
				default: {
					break;
				}
			}

			size_t foo = start_offset + (block_size * sizeof(uint16_t));

			if (offset != foo) {
				NGLog(NG_LOG_TYPE_ERROR, "NGReadNGSavegameBuffer: size of block mismatch!");
				offset = start_offset + (block_size * sizeof(uint16_t));
			}

			if (offset >= ng_savegame_buffer_size) {
				NGLog(NG_LOG_TYPE_ERROR, "NGReadNGSavegameBuffer: overflow!");
				return;
			}
		}
	}
}

void NGReadNGSavegameBuffer(FILE *file) {
	ng_savegame_buffer_size = 0;

	long original_file_position = ftell(file);
	if (fseek(file, -long((sizeof(uint32_t) * 2)), SEEK_END) == 0) {
		uint32_t ngle_ident = 0;
		fread(&ngle_ident, sizeof(uint32_t), 1, file);
		if (ngle_ident == NGLE_END_SIGNATURE) {
			uint32_t ngle_buffer_end = ftell(file);

			uint32_t footer_offset = 0;
			fread(&footer_offset, sizeof(uint32_t), 1, file);
			if (fseek(file, -long(footer_offset), SEEK_END) == 0) {
				int ngle_buffer_start = ftell(file);
				uint16_t header_ident;
				fread(&header_ident, sizeof(uint16_t), 1, file);
				if (header_ident == 0x474e) {
					if (fseek(file, -long(sizeof(uint16_t)), SEEK_CUR) == 0) {
						uint32_t buffer_size = ngle_buffer_end - ngle_buffer_start;
						if (buffer_size < MAX_NG_SAVEGAME_BUFFER_SIZE) {
							if (fread(ng_savegame_buffer, sizeof(char), buffer_size, file) == buffer_size) {
								ng_savegame_buffer_size = buffer_size;
							}
						} else {
							NGLog(NG_LOG_TYPE_ERROR, "NGReadNGSavegameBuffer: overflow!");
						}
					}
				}
			}
		}
	}
	fseek(file, original_file_position, SEEK_SET);
}