#include "../tomb4/pch.h"
#include "mod_config.h"
#include "../specific/file.h"

#include "libs/tiny-json/tiny-json.h"

GAME_MOD_CONFIG game_mod_config;

MOD_CREATURE_HEALTH_INFO get_game_mod_creature_health_info() {
	return game_mod_config.creature_health_info;
}

MOD_AUDIO_INFO get_game_mod_audio_info() {
	return game_mod_config.audio_info;
}

MOD_GRAPHICS_INFO get_game_mod_graphics_info() {
    return game_mod_config.graphics_info;
}

MOD_LARA_INFO get_game_mod_lara_info() {
	return game_mod_config.lara_info;
}

MOD_GAME_INFO get_game_mod_game_info() {
	return game_mod_config.game_info;
}

MOD_LEVEL_INFO get_global_level_info() {
    return game_mod_config.global_level_info;
}

void LoadGameModConfig() {
    GAME_MOD_CONFIG game_mod_config_new;
    game_mod_config = game_mod_config_new;

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

    const json_t *info = json_getProperty(json, "info");
    if (info && JSON_OBJ == json_getType(info)) {
        MOD_GAME_INFO *mod_game_info = &game_mod_config.game_info;

        const json_t *secret_count = json_getProperty(info, "secret_count");
        if (secret_count && JSON_INTEGER == json_getType(secret_count)) {
            mod_game_info->secret_count = (unsigned char)json_getInteger(secret_count);
        }
    }

    free(json_buf);
    return;
}