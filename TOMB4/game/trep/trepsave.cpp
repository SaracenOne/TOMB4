#include "../../tomb4/pch.h"

#include "trepsave.h"
#include "furr.h"

#define TREPSAVE_BUFFER_SIZE 0x2ff0

void S_TREPLoadgame(long slot_num) {
	HANDLE file;
	ulong bytes_read = 0;

	char buffer[TREPSAVE_BUFFER_SIZE];
	memset(buffer, 0x00, TREPSAVE_BUFFER_SIZE);

	wsprintf(buffer, "trepsave.%d", slot_num);
	file = CreateFile(buffer, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (file != INVALID_HANDLE_VALUE)
	{
		BOOL result = ReadFile(file, buffer, TREPSAVE_BUFFER_SIZE, &bytes_read, 0);
		if (result == TRUE) {
			memcpy(furr_oneshot_buffer, buffer, LAST_FURR_FLIPEFFECT);
		}
		CloseHandle(file);

		return;
	}
}

void S_TREPSavegame(long slot_num) {
	HANDLE file;
	ulong bytes_written = 0;

	char buffer[TREPSAVE_BUFFER_SIZE];
	memset(buffer, 0x00, TREPSAVE_BUFFER_SIZE);

	wsprintf(buffer, "trepsave.%d", slot_num);
	file = CreateFile(buffer, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	memcpy(buffer, furr_oneshot_buffer, LAST_FURR_FLIPEFFECT);

	WriteFile(file, buffer, TREPSAVE_BUFFER_SIZE, &bytes_written, 0);

	CloseHandle(file);
}