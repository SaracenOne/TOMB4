#pragma once

#include "../game/objects.h"

#define DEFAULT_FOG_START_VALUE 12288
#define DEFAULT_FOG_END_VALUE 20480
#define DEFAULT_FAR_VIEW_VALUE 20480

struct MOD_LEVEL_CAMERA_INFO {
	int chase_cam_distance = 1536;
	int chase_camera_vertical_orientation = -1820;
	int chase_camera_horizontal_orientation = 0;

	int combat_cam_distance = 1536;
	int combat_cam_vertical_orientation = -2730;

	int look_camera_distance = -1024;
	int look_camera_height = 16;

	int camera_speed = 10;
};

enum CREATURE_HIT_TYPE {
	CREATURE_HIT_SMOKE,
	CREATURE_HIT_BLOOD,
	CREATURE_HIT_NO_ACTION,
	CREATURE_HIT_RICHOCHET,
	CREATURE_HIT_CLEAR_DAMAGE
};

enum SLOT_EXPLODE_TYPE {
	NO_EXPLOSION,
	EXPLODE_IMMEDIATELY,
	EXPLODE_AFTER_DEATH_ANIMATION
};

enum SLOT_HIT_TYPE {
	HIT_NONE,
	HIT_BLOOD,
	HIT_FRAGMENTS,
	HIT_SMOKE
};

struct MOD_LEVEL_OBJECT_INFO {
	short hit_points = 0;
	short damage_1 = 0;
	short damage_2 = 0;
	short damage_3 = 0;

	bool override_hp : 1;
	bool override_hit_type : 1;

	bool explode_immediately : 1;
	bool explode_after_death_animation : 1;
	SLOT_HIT_TYPE hit_type: 2;
	bool explosive_death_only : 1;
};

struct MOD_LEVEL_CREATURE_INFO {
	bool small_scorpion_is_poisonous = true;
	int small_scorpion_poison_strength = 512;
	
	bool remove_knights_templar_sparks = false;
	
	bool remove_ahmet_death_flames = false;
	bool remove_ahmet_death_loop = false;
	bool disable_ahmet_heavy_trigger = false;

	bool remove_mummy_stun_animations = false;
};

struct MOD_LEVEL_AUDIO_INFO {
	short inside_jeep_track = 98;
	short outside_jeep_track = 110;
	short secret_track = 5;
};

struct MOD_LEVEL_LARA_INFO {
	long hair_gravity = 10;

	long braid_x = -4;
	long braid_y = -4;
	long braid_z = -48;

	long pigtail_left_x = -52;
	long pigtail_left_y = -48;
	long pigtail_left_z = -50;

	long pigtail_right_x = 44;
	long pigtail_right_y = -48;
	long pigtail_right_z = -50;

	int crawlspace_jump_animation = 421;
	int crawlspace_jump_pit_deepness_threshold = 768;

	bool use_tr5_swimming_collision = false;
	bool disable_hardcoded_breath_sound = false;
};

#define MAX_PLUGIN_COUNT 64
#define MAX_PLUGIN_NAME_LEN 256

struct MOD_GLOBAL_INFO {
	// Default to latest known version.
	unsigned char trng_version_major = 1;
	unsigned char trng_version_minor = 3;
	unsigned char trng_version_maintainence = 0;
	unsigned char trng_version_build = 7;

	// TRNG Stuff
	bool trng_flipeffects_enabled = false; // TRNG (special TRNG flipeffects. Disable if conflicting with FURR)
	bool trng_conditionals_enabled = false; // TRNG replaces monkeybar trigger type with customizable conditional conditional
	bool trng_actions_enabled = false;
	bool trng_rollingball_extended_ocb = false; // TRNG (moveable and regular trigger activation)
	bool trng_statics_extended_ocb = false; // TRNG (touch inflicting poison)
	// Climable pushables (both implementations have different quirks. Will attempt to more accurately recreate them later)
	bool trng_pushable_extended_ocb = false; // TRNG (climable pushables)
	bool trng_hack_allow_meshes_with_exactly_256_vertices = false; // TRNG seems to have a special hack which allows meshes of 256 verticies

	// TREP stuff
	bool trep_pushable_extended_ocb = false; // TREP (climable pushables)

	// Tomo stuff
	bool tomo_enable_weather_flipeffect = false;
	bool tomo_swap_whitelight_for_teleporter = false;

	bool show_lara_in_title = false;
	unsigned short max_particles = 256;

	unsigned int plugin_count = 0;
	char plugins[MAX_PLUGIN_COUNT][MAX_PLUGIN_NAME_LEN];
};

struct MOD_LEVEL_BAR_INFO {

};

struct MOD_LEVEL_STAT_INFO {
	unsigned int secret_count = 70;
};

struct MOD_LEVEL_FLARE_INFO {
	unsigned char light_color_r = 128;
	unsigned char light_color_g = 192;
	unsigned char light_color_b = 0;
	int flare_lifetime_in_ticks = 30 * 30;
	int light_intensity = 16;
	bool has_sparks = false;
	bool has_fire = false; // Unimplemented
	bool sparks_include_smoke = false;
	bool has_glow = false; // Unimplemented
	bool flat_light = false;
};

struct MOD_LEVEL_MISC_INFO {
	bool enemy_gun_hit_underwater_sfx_fix = false;
	bool darts_poison_fix = false;
	unsigned int fog_start_range = DEFAULT_FOG_START_VALUE;
	unsigned int fog_end_range = DEFAULT_FOG_END_VALUE;
	unsigned int far_view = DEFAULT_FAR_VIEW_VALUE;
};

struct MOD_LEVEL_INFO {
	MOD_LEVEL_CAMERA_INFO camera_info;
	MOD_LEVEL_CREATURE_INFO creature_info;
	MOD_LEVEL_STAT_INFO stat_info;
	MOD_LEVEL_LARA_INFO lara_info;
	MOD_LEVEL_AUDIO_INFO audio_info;
	MOD_LEVEL_FLARE_INFO flare_info;
	MOD_LEVEL_MISC_INFO misc_info;
	MOD_LEVEL_OBJECT_INFO object_info[NUMBER_OBJECTS];
	short slot_info[NUMBER_OBJECTS];
};

#define MOD_LEVEL_COUNT 64

struct GAME_MOD_CONFIG {
	MOD_GLOBAL_INFO global_info;

	MOD_LEVEL_INFO level_info[MOD_LEVEL_COUNT];
};

extern GAME_MOD_CONFIG game_mod_config;

extern void setup_custom_slots_for_level(int level, OBJECT_INFO* current_object_info_array);
extern void assign_slot_for_level(int level, int dest_slot, int src_slot);

extern MOD_GLOBAL_INFO &get_game_mod_global_info();

extern MOD_LEVEL_CAMERA_INFO &get_game_mod_level_camera_info(int level);
extern MOD_LEVEL_CREATURE_INFO &get_game_mod_level_creature_info(int level);
extern MOD_LEVEL_AUDIO_INFO &get_game_mod_level_audio_info(int level);
extern MOD_LEVEL_LARA_INFO &get_game_mod_level_lara_info(int level);
extern MOD_LEVEL_STAT_INFO &get_game_mod_level_stat_info(int level);
extern MOD_LEVEL_FLARE_INFO &get_game_mod_level_flare_info(int level);
extern MOD_LEVEL_MISC_INFO& get_game_mod_level_misc_info(int level);

extern void LoadGameModConfigFirstPass();
extern void LoadGameModConfigSecondPass();