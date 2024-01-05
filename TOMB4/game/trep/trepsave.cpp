#include "../../tomb4/pch.h"

#include "trepsave.h"
#include "furr.h"
#include "../../tomb4/tomb4plus/t4plus_weather.h"

#define TREPSAVE_BUFFER_SIZE 0x2ff0

void S_TREPLoadgame(long slot_num) {
	HANDLE file;
	ulong bytes_read = 0;

	char buffer[TREPSAVE_BUFFER_SIZE];
	memset(buffer, 0x00, TREPSAVE_BUFFER_SIZE);

	wsprintf(buffer, "trepsave.%d", slot_num);
	file = CreateFile(buffer, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	furr_clear_oneshot_buffer();
	rain_type = WEATHER_DISABLED;
	snow_type = WEATHER_DISABLED;

	if (file != INVALID_HANDLE_VALUE)
	{
		BOOL result = ReadFile(file, buffer, TREPSAVE_BUFFER_SIZE, &bytes_read, 0);
		if (result == TRUE) {
			memcpy(furr_oneshot_buffer, buffer, LAST_FURR_FLIPEFFECT);

			char weather = 0;
			memcpy(&weather, buffer + 0x270, sizeof(char));
			switch (weather) {
				case 1:
					rain_type = WEATHER_ENABLED_ALL_OUTSIDE;
					snow_type = WEATHER_DISABLED;
					break;
				case 2:
					rain_type = WEATHER_DISABLED;
					snow_type = WEATHER_ENABLED_ALL_OUTSIDE;
					break;
				default:
					rain_type = WEATHER_DISABLED;
					snow_type = WEATHER_DISABLED;
					break;
			}
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

	char weather = 0;
	if (rain_type == WEATHER_ENABLED_ALL_OUTSIDE) {
		weather += 1;
	}
	if (snow_type == WEATHER_ENABLED_ALL_OUTSIDE) {
		weather += 2;
	}

	memcpy(buffer + 0x270, &weather, sizeof(char));

	WriteFile(file, buffer, TREPSAVE_BUFFER_SIZE, &bytes_written, 0);

	CloseHandle(file);
}