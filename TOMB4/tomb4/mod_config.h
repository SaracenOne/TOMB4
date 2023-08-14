#pragma once

enum CREATURE_HIT_TYPE {
	CREATURE_HIT_SMOKE,
	CREATURE_HIT_BLOOD,
	CREATURE_HIT_NO_ACTION,
	CREATURE_HIT_RICHOCHET,
	CREATURE_HIT_CLEAR_DAMAGE
};

struct MOD_LEVEL_CREATURE_INFO {
	bool small_scorpion_is_poisonous = true;
	int small_scorpion_poison_strength = 512;
	bool remove_knights_templar_sparks = false;
	bool remove_ahmet_death_flames = false;
	bool remove_ahmet_death_loop = false;
	bool disable_ahmet_heavy_trigger = false;
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
};

struct MOD_GLOBAL_INFO {
	// Default to latest known version.
	unsigned char trng_version_major = 1;
	unsigned char trng_version_minor = 3;
	unsigned char trng_version_maintainence = 0;
	unsigned char trng_version_build = 7;

	bool trng_flipeffects_enabled = true; // TRNG (special TRNG flipeffects. Disable if conflicting with FURR)
	bool trng_conditionals_enabled = true; // TRNG replaces monkeybar trigger type with customizable conditional conditional
	bool trng_actions_enabled = true;
	bool trng_rollingball_extended_ocb = true; // TRNG (moveable and regular trigger activation)
	bool trng_statics_extended_ocb = true; // TRNG (touch inflicting poison)

	// Climable pushables (both implementations have different quirks. Will attempt to more accurately recreate them later)
	bool trng_pushable_extended_ocb = true; // TRNG (climable pushables)
	bool trep_pushable_extended_ocb = true; // TREP (climable pushables)

	bool show_lara_in_title = false;
	unsigned short max_particles = 256;
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

struct MOD_LEVEL_INFO {
	MOD_LEVEL_CREATURE_INFO creature_info;
	MOD_LEVEL_STAT_INFO stat_info;
	MOD_LEVEL_LARA_INFO lara_info;
	MOD_LEVEL_AUDIO_INFO audio_info;
	MOD_LEVEL_FLARE_INFO flare_info;
};

#define MOD_LEVEL_COUNT 64

struct GAME_MOD_CONFIG {
	MOD_GLOBAL_INFO global_info;

	MOD_LEVEL_INFO level_info[MOD_LEVEL_COUNT];
};

extern GAME_MOD_CONFIG game_mod_config;

extern MOD_GLOBAL_INFO &get_game_mod_global_info();

extern MOD_LEVEL_CREATURE_INFO &get_game_mod_level_creature_info(int level);
extern MOD_LEVEL_AUDIO_INFO &get_game_mod_level_audio_info(int level);
extern MOD_LEVEL_LARA_INFO &get_game_mod_level_lara_info(int level);
extern MOD_LEVEL_STAT_INFO &get_game_mod_level_stat_info(int level);
extern MOD_LEVEL_FLARE_INFO &get_game_mod_level_flare_info(int level);

extern void LoadGameModConfigFirstPass();
extern void LoadGameModConfigSecondPass();