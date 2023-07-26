#include "../tomb4/pch.h"
#include "mod_config.h"
#include "../specific/file.h"

#include "libs/tiny-json/tiny-json.h"

#define READ_JSON_UINT8(value_name, json, my_struct) { const json_t* value_name = json_getProperty(json, #value_name); \
    if (value_name && JSON_INTEGER == json_getType(value_name)) { \
        (my_struct)->value_name = (unsigned char)json_getInteger(value_name); } \
    }

#define READ_JSON_UINT16(value_name, json, my_struct) { const json_t* value_name = json_getProperty(json, #value_name); \
    if (value_name && JSON_INTEGER == json_getType(value_name)) { \
        (my_struct)->value_name = (unsigned short)json_getInteger(value_name); } \
    }

#define READ_JSON_BOOL(value_name, json, my_struct) { const json_t* value_name = json_getProperty(json, #value_name); \
    if (value_name && JSON_BOOLEAN == json_getType(value_name)) { \
        (my_struct)->value_name = (bool)json_getBoolean(value_name); } \
    }

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
    const json_t* root_json = json_create(json_buf, mem, sizeof mem / sizeof * mem);
    if (!root_json) {
        // Could not create JSON!
        free(json_buf);
        return;
    }

    const json_t* global = json_getProperty(root_json, "global_info");
    if (global && JSON_OBJ == json_getType(global)) {
        MOD_GLOBAL_INFO* mod_global_info = &game_mod_config.global_info;

        READ_JSON_UINT8(trng_version_major, global, mod_global_info);
        READ_JSON_UINT8(trng_version_minor, global, mod_global_info);
        READ_JSON_UINT8(trng_version_maintainence, global, mod_global_info);
        READ_JSON_UINT8(trng_version_build, global, mod_global_info);
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
    const json_t* root_json = json_create(json_buf, mem, sizeof mem / sizeof * mem);
    if (!root_json) {
        // Could not create JSON!
        free(json_buf);
        return;
    }

    const json_t* global = json_getProperty(root_json, "global_info");
    if (global && JSON_OBJ == json_getType(global)) {
        MOD_GLOBAL_INFO* mod_global_info = &game_mod_config.global_info;

        READ_JSON_UINT8(trng_version_major, global, mod_global_info);
        READ_JSON_UINT8(trng_version_minor, global, mod_global_info);
        READ_JSON_UINT8(trng_version_maintainence, global, mod_global_info);
        READ_JSON_UINT8(trng_version_build, global, mod_global_info);

        READ_JSON_BOOL(trng_flipeffects_enabled, global, mod_global_info);
        READ_JSON_BOOL(trng_actions_enabled, global, mod_global_info);
        READ_JSON_BOOL(trng_rollingball_extended_ocb, global, mod_global_info);
        READ_JSON_BOOL(trng_statics_extended_ocb, global, mod_global_info);
        READ_JSON_BOOL(trng_pushable_extended_ocb, global, mod_global_info);

        READ_JSON_BOOL(show_lara_in_title, global, mod_global_info);
        READ_JSON_UINT16(max_particles, global, mod_global_info);
    }

    const json_t* levels = json_getProperty(root_json, "levels");
    if (levels && JSON_ARRAY == json_getType(levels)) {

    }

    free(json_buf);
    return;
}