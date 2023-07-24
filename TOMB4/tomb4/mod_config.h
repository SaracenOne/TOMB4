#pragma once

enum CREATURE_HIT_TYPE {
	CREATURE_HIT_SMOKE,
	CREATURE_HIT_BLOOD,
	CREATURE_HIT_NO_ACTION,
	CREATURE_HIT_RICHOCHET,
	CREATURE_HIT_CLEAR_DAMAGE
};

struct MOD_LEVEL_CREATURE_HEALTH_INFO {
	unsigned short skeleton_hp = 15;
	CREATURE_HIT_TYPE skeleton_hit_type = CREATURE_HIT_SMOKE;
	unsigned short baddy_1_hp = 25;
	CREATURE_HIT_TYPE baddy_1_hit_type = CREATURE_HIT_BLOOD;
	unsigned short baddy_2_hp = 35;
	CREATURE_HIT_TYPE baddy_2_hit_type = CREATURE_HIT_BLOOD;
	unsigned short big_scorpion_hp = 80;
	CREATURE_HIT_TYPE big_scorpion_hit_type = CREATURE_HIT_BLOOD;
	unsigned short mummy_hp = 15;
	CREATURE_HIT_TYPE mummy_hit_type = CREATURE_HIT_SMOKE;
	unsigned short knights_templer_hp = 15;
	CREATURE_HIT_TYPE knights_templer_hit_type = CREATURE_HIT_SMOKE;
	unsigned short sphinx_hp = 1000;
	CREATURE_HIT_TYPE sphinx_hit_type = CREATURE_HIT_RICHOCHET;
	unsigned short seth_hp = 500;
	CREATURE_HIT_TYPE seth_hit_type = CREATURE_HIT_NO_ACTION;
	unsigned short horseman_hp = 25;
	CREATURE_HIT_TYPE horseman_hit_type = CREATURE_HIT_RICHOCHET;
	unsigned short hammerhead_hp = 45;
	CREATURE_HIT_TYPE hammerhead_hit_type = CREATURE_HIT_BLOOD;
	unsigned short crocodile_hp = 36;
	CREATURE_HIT_TYPE crocodile_hit_type = CREATURE_HIT_BLOOD;
	unsigned short mutant_hp = 15;
	CREATURE_HIT_TYPE mutant_hit_type = CREATURE_HIT_RICHOCHET;
	unsigned short guide_hp = -16384;
	CREATURE_HIT_TYPE guide_hit_type = CREATURE_HIT_SMOKE;
	unsigned short demigod_1_hp = 200;
	CREATURE_HIT_TYPE demigod_1_hit_type = CREATURE_HIT_RICHOCHET;
	unsigned short demigod_2_hp = 200;
	CREATURE_HIT_TYPE demigod_2_hit_type = CREATURE_HIT_CLEAR_DAMAGE;
	unsigned short demigod_3_hp = 200;
	CREATURE_HIT_TYPE demigod_3_hit_type = CREATURE_HIT_CLEAR_DAMAGE;
	unsigned short troops_hp = 40;
	CREATURE_HIT_TYPE troops_hit_type = CREATURE_HIT_BLOOD;
	unsigned short sas_hp = 40;
	CREATURE_HIT_TYPE sas_hit_type = CREATURE_HIT_BLOOD;
	unsigned short harpy_hp = 60;
	CREATURE_HIT_TYPE harpy_hit_type = CREATURE_HIT_BLOOD;
	unsigned short wild_boar_hp = 40;
	CREATURE_HIT_TYPE wild_boar_hit_type = CREATURE_HIT_BLOOD;
	unsigned short dog_hp = 16;
	CREATURE_HIT_TYPE dog_hit_type = CREATURE_HIT_BLOOD;
	unsigned short ahmet_hp = 80;
	CREATURE_HIT_TYPE ahmet_hit_type = CREATURE_HIT_BLOOD;
	unsigned short baboon_hp = 30;
	CREATURE_HIT_TYPE baboon_hit_type = CREATURE_HIT_BLOOD;
	unsigned short bat_hp = 5;
	CREATURE_HIT_TYPE bat_hit_type = CREATURE_HIT_BLOOD;
	unsigned short big_beetle_hp = 30;
	CREATURE_HIT_TYPE big_beetle_hit_type = CREATURE_HIT_BLOOD;
	unsigned short von_croy_hp = 15;
	CREATURE_HIT_TYPE von_croy_hit_type = CREATURE_HIT_SMOKE;
	unsigned short small_scorpion_hp = 8;
	CREATURE_HIT_TYPE small_scorpion_hit_type = CREATURE_HIT_BLOOD;
	bool small_scorpion_is_poisonous = true;
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
};

struct MOD_GLOBAL_INFO {
	unsigned short max_particles = 256;
	bool show_lara_in_title = false;
	bool rollingball_extended_ocb = true; // TRNG (moveable and regular trigger activation)
	bool statics_extended_ocb = true; // TRNG (touch inflicting poison)
	bool pushable_extended_ocb = true; // TRNG (climable pushables)
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
	MOD_LEVEL_CREATURE_HEALTH_INFO creature_health_info;
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

extern MOD_LEVEL_CREATURE_HEALTH_INFO &get_game_mod_level_creature_health_info(int level);
extern MOD_LEVEL_AUDIO_INFO &get_game_mod_level_audio_info(int level);
extern MOD_LEVEL_LARA_INFO &get_game_mod_level_lara_info(int level);
extern MOD_LEVEL_STAT_INFO &get_game_mod_level_stat_info(int level);
extern MOD_LEVEL_FLARE_INFO &get_game_mod_level_flare_info(int level);

extern void LoadGameModConfigFirstPass();
extern void LoadGameModConfigSecondPass();