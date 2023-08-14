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

#define READ_JSON_SINT32(value_name, json, my_struct) { const json_t* value_name = json_getProperty(json, #value_name); \
    if (value_name && JSON_INTEGER == json_getType(value_name)) { \
        (my_struct)->value_name = (signed int)json_getInteger(value_name); } \
    }

#define READ_JSON_UINT32(value_name, json, my_struct) { const json_t* value_name = json_getProperty(json, #value_name); \
    if (value_name && JSON_INTEGER == json_getType(value_name)) { \
        (my_struct)->value_name = (unsigned int)json_getInteger(value_name); } \
    }

#define READ_JSON_BOOL(value_name, json, my_struct) { const json_t* value_name = json_getProperty(json, #value_name); \
    if (value_name && JSON_BOOLEAN == json_getType(value_name)) { \
        (my_struct)->value_name = (bool)json_getBoolean(value_name); } \
    }

#define READ_JSON_BOOL_AND_SET_FLAG(value_name, json, my_struct, flag) { const json_t* value_name = json_getProperty(json, #value_name); \
    if (value_name && JSON_BOOLEAN == json_getType(value_name)) { \
        (my_struct)->value_name = (bool)json_getBoolean(value_name); } \
    } \
    flag = true;
    

bool scorpion_poison_override_found = false;

GAME_MOD_CONFIG game_mod_config;

extern MOD_GLOBAL_INFO& get_game_mod_global_info() {
    return game_mod_config.global_info;
}

MOD_LEVEL_CREATURE_INFO & get_game_mod_level_creature_info(int level) {
    return game_mod_config.level_info[level].creature_info;
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

void LoadGameModLevelLaraInfo(const json_t* level, MOD_LEVEL_LARA_INFO *lara_info) {
    READ_JSON_SINT32(hair_gravity, level, lara_info);

    READ_JSON_SINT32(braid_x, level, lara_info);
    READ_JSON_SINT32(braid_y, level, lara_info);
    READ_JSON_SINT32(braid_z, level, lara_info);

    READ_JSON_SINT32(pigtail_left_x, level, lara_info);
    READ_JSON_SINT32(pigtail_left_y, level, lara_info);
    READ_JSON_SINT32(pigtail_left_z, level, lara_info);

    READ_JSON_SINT32(pigtail_right_x, level, lara_info);
    READ_JSON_SINT32(pigtail_right_y, level, lara_info);
    READ_JSON_SINT32(pigtail_right_z, level, lara_info);

    READ_JSON_SINT32(crawlspace_jump_animation, level, lara_info);
    READ_JSON_SINT32(crawlspace_jump_pit_deepness_threshold, level, lara_info);
}

void LoadGameModLevelCreatureInfo(const json_t* creature, MOD_LEVEL_CREATURE_INFO *creature_info) {
    READ_JSON_BOOL_AND_SET_FLAG(small_scorpion_is_poisonous, creature, creature_info, scorpion_poison_override_found);
    READ_JSON_SINT32(small_scorpion_poison_strength, creature, creature_info);

    READ_JSON_BOOL(remove_knights_templar_sparks, creature, creature_info);

    READ_JSON_BOOL(remove_ahmet_death_flames, creature, creature_info);
    READ_JSON_BOOL(remove_ahmet_death_loop, creature, creature_info);
}

void LoadGameModLevel(const json_t *level, MOD_LEVEL_INFO *level_info) {
    const json_t* lara_info = json_getProperty(level, "lara_info");
    if (lara_info && JSON_OBJ == json_getType(lara_info)) {
        LoadGameModLevelLaraInfo(lara_info, &level_info->lara_info);
    }

    const json_t* creature_info = json_getProperty(level, "creature_info");
    if (creature_info && JSON_OBJ == json_getType(creature_info)) {
        LoadGameModLevelCreatureInfo(creature_info, &level_info->creature_info);
    }
}

void SetupLevelDefaults() {
    for (int i = 0; i < MOD_LEVEL_COUNT; i++) {
        if (i <= 3) {
            if (!scorpion_poison_override_found)
                game_mod_config.level_info[i].creature_info.small_scorpion_is_poisonous = false;
        }
    }
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

    const json_t* level = json_getProperty(root_json, "global_level_info");
    MOD_LEVEL_INFO global_level_info;
    if (level && JSON_OBJ == json_getType(level)) {
        LoadGameModLevel(level, &global_level_info);
    }
    for (int i = 0; i < MOD_LEVEL_COUNT; i++) {
        memcpy(&game_mod_config.level_info[i], &global_level_info, sizeof(MOD_LEVEL_INFO));
    }

    SetupLevelDefaults();

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
        json_t const* level;
        int level_index = 0;
        for (level = json_getChild(levels); level != 0; level = json_getSibling(level)) {
            if (JSON_OBJ == json_getType(level)) {
                LoadGameModLevel(level, &game_mod_config.level_info[level_index]);
            }
            level_index++;
        }
    }

    free(json_buf);
    return;
}