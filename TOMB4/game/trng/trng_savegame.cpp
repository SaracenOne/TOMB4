#include "../../tomb4/pch.h"

#include "trng.h"
#include "trng_savegame.h"

size_t NGReadNGSavegameInfo(char* savegameFileBuffer, size_t offset, size_t len) {
	size_t savegame_block_start_position = offset;
	unsigned short savegame_block_size = NG_READ_16(savegameFileBuffer, offset);
	unsigned short savegame_block_unknown_variable = NG_READ_16(savegameFileBuffer, offset);

	if (savegame_block_size == 0) {
		return 0;
	}

	size_t level_block_end_pos = savegame_block_start_position + savegame_block_size * sizeof(short);

	size_t command_blocks_parsed = 0;
	size_t command_blocks_failed = 0;

	NGLog(NG_LOG_TYPE_PRINT, "NGReadNGSavegameInfo: === Parsing Savegame %u ===");

	// Do the commands
	while (1) {
		size_t data_block_start_start_position = offset;
		unsigned char current_data_block_size_wide = NG_READ_8(savegameFileBuffer, offset);
	}
}