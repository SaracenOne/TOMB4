#include "../tomb4/pch.h"
#include "mod_config.h"
#include "../specific/file.h"

#include "libs/tiny-json/tiny-json.h"

GAME_MOD_CONFIG game_mod_config;

extern MOD_GLOBAL_INFO& get_game_mod_global_info() {
    return game_mod_config.global_info;
}

MOD_LEVEL_CREATURE_HEALTH_INFO &get_game_mod_level_creature_health_info(int level) {
    return game_mod_config.level_info[level].creature_health_info;
}

MOD_LEVEL_AUDIO_INFO &get_game_mod_level_audio_info(int level) {
	return game_mod_config.level_info[level].audio_info;
}

MOD_LEVEL_LARA_INFO &get_game_mod_level_lara_info(int level) {
	return game_mod_config.level_info[level].lara_info;
}

MOD_LEVEL_STAT_INFO &get_game_mod_level_stat_info(int level) {
    return game_mod_config.level_info[level].stat_info;
}

MOD_LEVEL_FLARE_INFO& get_game_mod_level_flare_info(int level) {
    return game_mod_config.level_info[level].flare_info;
}

void LoadGameModConfigFirstPass() {
    char* json_buf = NULL;
    if (LoadFile("game_mod_config.json", &json_buf) <= 0) {
        return;
    }

    json_t mem[32];
    const json_t* json = json_create(json_buf, mem, sizeof mem / sizeof * mem);
    if (!json) {
        // Could not create JSON!
        free(json_buf);
        return;
    }

    free(json_buf);
    return;
}

void LoadGameModConfigSecondPass() {
    char* json_buf = NULL;
    if (LoadFile("game_mod_config.json", &json_buf) <= 0) {
        return;
    }

    json_t mem[32];
    const json_t* json = json_create(json_buf, mem, sizeof mem / sizeof * mem);
    if (!json) {
        // Could not create JSON!
        free(json_buf);
        return;
    }


    const json_t* levels = json_getProperty(json, "levels");
    if (levels && JSON_ARRAY == json_getType(levels)) {

    }

    free(json_buf);
    return;
}