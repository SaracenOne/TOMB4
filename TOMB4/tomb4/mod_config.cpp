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
    

// Full overrides
bool scorpion_poison_override_found = false;

GAME_MOD_CONFIG game_mod_config;

void setup_custom_slots_for_level(int level, OBJECT_INFO* current_object_info_array) {
    OBJECT_INFO* backup_object_info_array = (OBJECT_INFO* )malloc(sizeof(OBJECT_INFO) * NUMBER_OBJECTS);

    if (backup_object_info_array) {
        memcpy(backup_object_info_array, current_object_info_array, sizeof(OBJECT_INFO) * NUMBER_OBJECTS);

        for (int i = 0; i < NUMBER_OBJECTS; i++) {
            if (game_mod_config.level_info[level].slot_info[i] != i) {
                memcpy(&current_object_info_array[i], &backup_object_info_array[game_mod_config.level_info[level].slot_info[i]], sizeof(OBJECT_INFO));
            }
        }
        free(backup_object_info_array);
    }
}

void assign_slot_for_level(int level, int dest_slot, int src_slot) {
    if (src_slot < NUMBER_OBJECTS && dest_slot < NUMBER_OBJECTS && level < MOD_LEVEL_COUNT) {
        game_mod_config.level_info[level].slot_info[dest_slot] = src_slot;
    } else {
        printf("Invalid slot assignment!\n");
    }
}

extern MOD_GLOBAL_INFO& get_game_mod_global_info() {
    return game_mod_config.global_info;
}

extern MOD_LEVEL_CAMERA_INFO& get_game_mod_level_camera_info(int level) {
    return game_mod_config.level_info[level].camera_info;
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

MOD_LEVEL_MISC_INFO& get_game_mod_level_misc_info(int level) {
    return game_mod_config.level_info[level].misc_info;
}

void LoadGameModLevelCameraInfo(const json_t* camera, MOD_LEVEL_CAMERA_INFO* camera_info) {
    READ_JSON_SINT32(chase_cam_distance, camera, camera_info);
    READ_JSON_SINT32(chase_camera_vertical_orientation, camera, camera_info);
    READ_JSON_SINT32(chase_camera_horizontal_orientation, camera, camera_info);

    READ_JSON_SINT32(combat_cam_distance, camera, camera_info);
    READ_JSON_SINT32(combat_cam_vertical_orientation, camera, camera_info);

    READ_JSON_SINT32(look_camera_distance, camera, camera_info);
    READ_JSON_SINT32(look_camera_height, camera, camera_info);

    READ_JSON_SINT32(camera_speed, camera, camera_info);
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

    READ_JSON_BOOL(use_tr5_swimming_collision, level, lara_info);
}

void LoadGameModLevelCreatureInfo(const json_t* creature, MOD_LEVEL_CREATURE_INFO *creature_info) {
    READ_JSON_BOOL_AND_SET_FLAG(small_scorpion_is_poisonous, creature, creature_info, scorpion_poison_override_found);
    READ_JSON_SINT32(small_scorpion_poison_strength, creature, creature_info);

    READ_JSON_BOOL(remove_knights_templar_sparks, creature, creature_info);

    READ_JSON_BOOL(remove_ahmet_death_flames, creature, creature_info);
    READ_JSON_BOOL(remove_ahmet_death_loop, creature, creature_info);
    READ_JSON_BOOL(disable_ahmet_heavy_trigger, creature, creature_info);

    READ_JSON_BOOL(remove_mummy_stun_animations, creature, creature_info);
}

void LoadGameModLevelMiscInfo(const json_t *misc, MOD_LEVEL_MISC_INFO *misc_info) {
    READ_JSON_BOOL(enemy_gun_hit_underwater_sfx_fix, misc, misc_info);
    READ_JSON_BOOL(darts_poison_fix, misc, misc_info);
    READ_JSON_UINT32(fog_start_range, misc, misc_info);
    READ_JSON_UINT32(fog_end_range, misc, misc_info);
    READ_JSON_UINT32(far_view, misc, misc_info);
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

    const json_t* misc_info = json_getProperty(level, "misc_info");
    if (misc_info && JSON_OBJ == json_getType(misc_info)) {
        LoadGameModLevelMiscInfo(misc_info, &level_info->misc_info);
    }
}

void SetupDefaultSlotInfoForLevel(MOD_LEVEL_INFO* level_info) {
    for (int i = 0; i < NUMBER_OBJECTS; i++) {
        level_info->slot_info[i] = i;
    }
}

void SetupDefaultObjectInfoForLevel(MOD_LEVEL_INFO* level_info) {
    MOD_LEVEL_OBJECT_INFO* obj = nullptr;

    for (int i = 0; i < NUMBER_OBJECTS; i++) {
        obj = &level_info->object_info[i];
        obj->hit_points = -16384;
        obj->damage_1 = 0;
        obj->damage_2 = 0;
        obj->damage_3 = 0;
        obj->override_hit_type = false;
        obj->override_hit_type = false;
        obj->explode_immediately = false;
        obj->explode_after_death_animation = false;
        obj->hit_type = HIT_NONE;
        obj->explosive_death_only = false;
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

void SetupGlobalDefaults() {
    MOD_GLOBAL_INFO* mod_global_info = &game_mod_config.global_info;

    mod_global_info->plugin_count = 0;
    for (int i = 0; i < MAX_PLUGIN_COUNT; i++) {
        memset(mod_global_info->plugins[i], 0x00, MAX_PLUGIN_NAME_LEN);
    }
}

void LoadGameModConfigFirstPass() {
    SetupGlobalDefaults();

    char* json_buf = NULL;
    LoadFile("game_mod_config.json", &json_buf);

    const json_t* level = nullptr;
   
    json_t mem[32];
    if (json_buf) {
        const json_t* root_json = json_create(json_buf, mem, sizeof mem / sizeof * mem);
        if (root_json) {
            const json_t* global = json_getProperty(root_json, "global_info");
            if (global && JSON_OBJ == json_getType(global)) {
                MOD_GLOBAL_INFO* mod_global_info = &game_mod_config.global_info;

                READ_JSON_UINT8(trng_version_major, global, mod_global_info);
                READ_JSON_UINT8(trng_version_minor, global, mod_global_info);
                READ_JSON_UINT8(trng_version_maintainence, global, mod_global_info);
                READ_JSON_UINT8(trng_version_build, global, mod_global_info);

                const json_t* plugins = json_getProperty(global, "plugins");
                if (plugins && JSON_ARRAY == json_getType(plugins)) {
                    json_t const* plugin;
                    mod_global_info->plugin_count = 0;
                    for (plugin = json_getChild(plugins); plugin != 0; plugin = json_getSibling(plugin)) {
                        if (mod_global_info->plugin_count < MAX_PLUGIN_COUNT) {
                            if (JSON_TEXT == json_getType(plugin)) {
                                int plugin_str_length = strlen(plugin->u.value);
                                if (plugin_str_length < MAX_PLUGIN_NAME_LEN-1) {
                                    memcpy(mod_global_info->plugins[mod_global_info->plugin_count], plugin->u.value, plugin_str_length);
                                } else {
                                    printf("Invalid plugin name. Too long\n");
                                }
                            }
                        } else {
                            break;
                        }
                        mod_global_info->plugin_count++;
                    }
                }
            }

            level = json_getProperty(root_json, "global_level_info");
        }
    }

    MOD_LEVEL_INFO global_level_info;

    SetupDefaultSlotInfoForLevel(&global_level_info);
    SetupDefaultObjectInfoForLevel(&global_level_info);

    if (level && JSON_OBJ == json_getType(level)) {
        LoadGameModLevel(level, &global_level_info);
    }
    for (int i = 0; i < MOD_LEVEL_COUNT; i++) {
        memcpy(&game_mod_config.level_info[i], &global_level_info, sizeof(MOD_LEVEL_INFO));
    }

    SetupLevelDefaults();

    if (json_buf)
        free(json_buf);
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