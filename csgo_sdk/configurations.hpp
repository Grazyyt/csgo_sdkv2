#pragma once

#include <map>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include "sdk/misc/Color.hpp"

#define CONFIGURATION(type, var, val) type var = val

template <typename T = bool>
class Var 
{
public:
	std::string name;
	std::shared_ptr<T> value;
	int32_t size;
	Var(std::string name, T v) : name(name) 
	{
		value = std::make_shared<T>(v);
		size = sizeof(T);
	}
	operator T() { return *value; }
	operator T*() { return &*value; }
	operator T() const { return *value; }
	//operator T*() const { return value; }
};

struct Legitbot_Configurations
{
	bool enabled = false;
	bool deathmatch = false;
	bool autopistol = false;
	bool smoke_check = false;
	bool flash_check = false;
	bool jump_check = false;
	bool autowall = false;
	bool silent = false;
	bool autofire = false;
	bool on_key = true;
	bool rcs = false;
	bool rcs_fov_enabled = false;
	bool rcs_smooth_enabled = false;
	bool autostop = false;
	float backtrack_time = 0.0f;
	bool only_in_zoom = true;
	int autofire_key = 1;
	int key = 1;
	int aim_type = 1;
	int priority = 0;
	int fov_type = 0;
	int smooth_type = 0;
	int rcs_type = 0;
	int hitbox = 1;
	float fov = 0.f;
	float silent_fov = 0.f;
	float rcs_fov = 0.f;
	float smooth = 1;
	float rcs_smooth = 1;
	int shot_delay = 0;
	int kill_delay = 0;
	int rcs_x = 100;
	int rcs_y = 100;
	int rcs_start = 1;
	int min_damage = 1;
};

struct item_setting
{
	bool enableskinchanger = false;
	char name[32] = "Default";
	bool enabled = false;
	int stickers_place = 0;
	int definition_vector_index = 0;
	int definition_index = 0;
	bool   enabled_stickers = 0;
	int paint_kit_vector_index = 0;
	int paint_kit_index = 0;
	int definition_override_vector_index = 0;
	int definition_override_index = 0;
	int seed = 0;
	bool stat_trak = 0;
	bool live_stat_trak = false;
	//std::array<sticker_setting, 5> stickers;
	float wear = FLT_MIN;
	char custom_name[32];
};

struct statrack_setting
{
	int definition_index = 1;
	struct
	{
		int counter = 0;
	}statrack_new;
};

class Configurations
{
public:
		//
		// AIMBOT
		//
		std::map<int, Legitbot_Configurations> legitbot_items = { };

		// 
		// ESP
		// 
		CONFIGURATION(bool, esp_enabled, false);
		CONFIGURATION(bool, esp_enemies_only, false);
		CONFIGURATION(bool, esp_player_boxes, false);
		CONFIGURATION(bool, esp_player_names, false);
		CONFIGURATION(bool, esp_player_health, false);
		CONFIGURATION(bool, esp_player_armour, false);
		CONFIGURATION(bool, esp_player_weapons, false);
		CONFIGURATION(int,  esp_player_weapons_type, 0);
		CONFIGURATION(bool, esp_player_snaplines, false);
		CONFIGURATION(bool, esp_crosshair, false);
		CONFIGURATION(bool, esp_dropped_weapons, false);
		CONFIGURATION(int,  esp_dropped_weapons_type, 0);
		CONFIGURATION(bool, esp_defuse_kit, false);
		CONFIGURATION(bool, esp_planted_c4, false);
		CONFIGURATION(bool, esp_items, false);
		CONFIGURATION(bool, esp_grenade_prediction, false);
		CONFIGURATION(bool, esp_draw_weapon_fov, false);

		// 
		// GLOW
		// 
		CONFIGURATION(bool, glow_enabled, false);
		CONFIGURATION(bool, glow_enemies_only, false);
		CONFIGURATION(bool, glow_players, false);
		CONFIGURATION(bool, glow_chickens, false);
		CONFIGURATION(bool, glow_c4_carrier, false);
		CONFIGURATION(bool, glow_planted_c4, false);
		CONFIGURATION(bool, glow_defuse_kits, false);
		CONFIGURATION(bool, glow_weapons, false);
		CONFIGURATION(int, glow_style, 0);
		CONFIGURATION(bool, glow_fullbloom, false);

		//
		// CHAMS
		//
		CONFIGURATION(bool, chams_player_enabled, false);
		CONFIGURATION(bool, chams_player_enemies_only, false);
		CONFIGURATION(bool, chams_player_wireframe, false);
		CONFIGURATION(bool, chams_player_flat, false);
		CONFIGURATION(bool, chams_player_ignorez, false);
		CONFIGURATION(bool, chams_player_glass, false);

		CONFIGURATION(bool, chams_sleeve_enabled, false);
		CONFIGURATION(bool, chams_sleeve_wireframe, false);
		CONFIGURATION(bool, chams_sleeve_flat, false);
		CONFIGURATION(bool, chams_sleeve_ignorez, false);
		CONFIGURATION(bool, chams_sleeve_glass, false);

		CONFIGURATION(bool, chams_arms_enabled, false);
		CONFIGURATION(bool, chams_arms_wireframe, false);
		CONFIGURATION(bool, chams_arms_flat, false);
		CONFIGURATION(bool, chams_arms_ignorez, false);
		CONFIGURATION(bool, chams_arms_glass, false);

		CONFIGURATION(bool, chams_weapon_enabled, false);
		CONFIGURATION(bool, chams_weapon_wireframe, false);
		CONFIGURATION(bool, chams_weapon_flat, false);
		CONFIGURATION(bool, chams_weapon_ignorez, false);
		CONFIGURATION(bool, chams_weapon_glass, false);

		//
		// MISC
		//
		CONFIGURATION(bool, misc_bhop, false);
		CONFIGURATION(bool, misc_autostrafe, false);
		CONFIGURATION(bool, misc_no_hands, false);
		CONFIGURATION(bool, misc_thirdperson, false);
		CONFIGURATION(bool, misc_showranks, true);
		CONFIGURATION(bool, misc_watermark, true);
		CONFIGURATION(float, misc_thirdperson_dist, 130.f);
		CONFIGURATION(int, misc_viewmodel_fov, 68);
		CONFIGURATION(float, misc_mat_ambient_light_r, 0.0f);
		CONFIGURATION(float, misc_mat_ambient_light_g, 0.0f);
		CONFIGURATION(float, misc_mat_ambient_light_b, 0.0f);

		CONFIGURATION(bool, misc_nightmode, false);
		CONFIGURATION(bool, misc_no_fog, false);
		CONFIGURATION(bool, misc_edgejump, false);
		CONFIGURATION(int, misc_edgejumpkey, 0);
		CONFIGURATION(bool, misc_jumpbug, false);
		CONFIGURATION(int, misc_jumpbugkey, 0);

		CONFIGURATION(bool, misc_edgebug, false);
		CONFIGURATION(int, misc_edgebugkey, 0);

		CONFIGURATION(int, misc_knifemodel, 0);
		CONFIGURATION(int, misc_knifeskin, 0);

		CONFIGURATION(bool, misc_editparticle, false);
		CONFIGURATION(bool, misc_changemolotov_nosmoke, false);

		CONFIGURATION(bool, misc_blockbot, false);
		CONFIGURATION(int, misc_blockbotkey, 0);

		CONFIGURATION(bool, misc_fakebackwards, false);
		CONFIGURATION(int, misc_fakebackwardskey, 0);
		CONFIGURATION(int, misc_fakebackwardsturnsmoothness, 5);

		//skins
		CONFIGURATION(bool, skins_enabled, false);
		CONFIGURATION(bool, skin_preview, false);
		CONFIGURATION(bool, show_cur, true);
		CONFIGURATION(bool, live_stat_trak, false);
		std::map<int, item_setting> m_items = { };
		std::map<int, statrack_setting> statrack_items = { };
		std::map<std::string, std::string> m_IconOverrides;

		// 
		// COLORS
		//

		//Changed to float, because it's more comfortable for me :)
		float nightmode_color_world[4] = { 0.15f, 0.15f, 0.15f, 1.0f };
		float nightmode_color_prop[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
		float nightmode_color_sky[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		float color_esp_ally_visible[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_esp_enemy_visible[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_esp_ally_occluded[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_esp_enemy_occluded[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_esp_crosshair[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_esp_weapons[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_esp_defuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_esp_c4[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_esp_item[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_grenade_prediction[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		float color_glow_ally[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_glow_enemy[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_glow_chickens[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_glow_c4_carrier[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_glow_planted_c4[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_glow_defuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_glow_weapons[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		float color_chams_player_ally_visible[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_chams_player_ally_occluded[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_chams_player_enemy_visible[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_chams_player_enemy_occluded[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		float color_chams_arms_visible[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_chams_arms_occluded[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		float color_chams_sleeve_visible[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_chams_sleeve_occluded[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		float color_chams_weapon_visible[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_chams_weapon_occluded[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		float color_watermark[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float misc_changemolotov_color[4] = { 1.0f, 0.7f, 0.4f, 1.0f };
		float misc_changeblood_color[4] = { 0.3f, 0.f, 0.f, 1.0f };
		float misc_changesmoke_color[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
		float misc_changegrenade_color[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
};

inline Configurations g_Configurations;
inline bool g_Unload;
