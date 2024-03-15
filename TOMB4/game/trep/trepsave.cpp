#include "../../tomb4/pch.h"
#include "../../specific/platform.h"

#include "trepsave.h"
#include "furr.h"
#include "../../tomb4/tomb4plus/t4plus_weather.h"

#define TREPSAVE_BUFFER_SIZE 0x2ff0

void S_TREPLoadgame(long slot_num) {
	ulong bytes_read = 0;

	char buffer[TREPSAVE_BUFFER_SIZE];
	memset(buffer, 0x00, TREPSAVE_BUFFER_SIZE);

	wsprintf(buffer, "trepsave.%d", slot_num);

	std::string full_path = savegame_dir_path + buffer;

	FILE* file = fopen(full_path.c_str(), "rb");

	furr_clear_oneshot_buffer();
	t4_rain_type = WEATHER_DISABLED;
	t4_snow_type = WEATHER_DISABLED;

	if (file)
	{
		bytes_read = fread(buffer, 1, TREPSAVE_BUFFER_SIZE, file);
		if (bytes_read == TREPSAVE_BUFFER_SIZE) {
			memcpy(furr_oneshot_buffer, buffer, LAST_FURR_FLIPEFFECT);

			char weather = 0;
			memcpy(&weather, buffer + 0x270, sizeof(char));
			switch (weather) {
				case 1:
					t4_rain_type = WEATHER_ENABLED_ALL_OUTSIDE;
					t4_snow_type = WEATHER_DISABLED;
					break;
				case 2:
					t4_rain_type = WEATHER_DISABLED;
					t4_snow_type = WEATHER_ENABLED_ALL_OUTSIDE;
					break;
				default:
					t4_rain_type = WEATHER_DISABLED;
					t4_snow_type = WEATHER_DISABLED;
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

	std::string full_path = savegame_dir_path + buffer;

	FILE* file = fopen(full_path.c_str(), "wb");
	if (file)
	{
		memcpy(buffer, furr_oneshot_buffer, LAST_FURR_FLIPEFFECT);

		char weather = 0;
		if (t4_rain_type == WEATHER_ENABLED_ALL_OUTSIDE) {
			weather += 1;
		}
		if (t4_snow_type == WEATHER_ENABLED_ALL_OUTSIDE) {
			weather += 2;
		}

		memcpy(buffer + 0x270, &weather, sizeof(char));

		bytes_written = fwrite(buffer, 1, TREPSAVE_BUFFER_SIZE, file);

		fclose(file);
	}
}