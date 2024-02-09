#include "../../tomb4/pch.h"
#include "../../specific/cmdline.h"

#include "trepsave.h"
#include "furr.h"
#include "../../tomb4/tomb4plus/t4plus_weather.h"

#define TREPSAVE_BUFFER_SIZE 0x2ff0

void S_TREPLoadgame(long slot_num) {
	ulong bytes_read = 0;

	char buffer[TREPSAVE_BUFFER_SIZE];
	memset(buffer, 0x00, TREPSAVE_BUFFER_SIZE);

	wsprintf(buffer, "trepsave.%d", slot_num);

	char full_path[4096];
	memset(full_path, 0, 4096);
	memcpy(full_path, working_dir_path, strlen(working_dir_path));
	strcat(full_path, buffer);

	FILE* file = fopen(full_path, "rb");

	furr_clear_oneshot_buffer();
	rain_type = WEATHER_DISABLED;
	snow_type = WEATHER_DISABLED;

	if (file)
	{
		bytes_read = fread(buffer, 1, TREPSAVE_BUFFER_SIZE, file);
		if (bytes_read == TREPSAVE_BUFFER_SIZE) {
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
		fclose(file);

		return;
	}
}

void S_TREPSavegame(long slot_num) {
	ulong bytes_written = 0;

	char buffer[TREPSAVE_BUFFER_SIZE];
	memset(buffer, 0x00, TREPSAVE_BUFFER_SIZE);

	wsprintf(buffer, "trepsave.%d", slot_num);

	char full_path[4096];
	memset(full_path, 0, 4096);
	memcpy(full_path, working_dir_path, strlen(working_dir_path));
	strcat(full_path, buffer);

	FILE* file = fopen(full_path, "wb");
	if (file)
	{
		memcpy(buffer, furr_oneshot_buffer, LAST_FURR_FLIPEFFECT);

		char weather = 0;
		if (rain_type == WEATHER_ENABLED_ALL_OUTSIDE) {
			weather += 1;
		}
		if (snow_type == WEATHER_ENABLED_ALL_OUTSIDE) {
			weather += 2;
		}

		memcpy(buffer + 0x270, &weather, sizeof(char));

		bytes_written = fwrite(buffer, 1, TREPSAVE_BUFFER_SIZE, file);

		fclose(file);
	}
}