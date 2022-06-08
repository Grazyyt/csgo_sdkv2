#include "config.hpp"
#include "configurations.hpp"

#include "sdk/csgostructs.hpp"

#include <fileapi.h>
#include <WinBase.h>

//Trash code but need other format for configuration system
std::map<int, const char*> WeaponNames_Config =
{
	{ 7, "AK-47" },
	{ 8, "AUG" },
	{ 9, "AWP" },
	{ 63, "CZ75 Auto" },
	{ 1, "Desert Eagle" },
	{ 2, "Dual Berettas" },
	{ 10, "FAMAS" },
	{ 3, "Five-SeveN" },
	{ 11, "G3SG1" },
	{ 13, "Galil AR" },
	{ 4, "Glock-18" },
	{ 14, "M249" },
	{ 60, "M4A1-S" },
	{ 16, "M4A4" },
	{ 17, "MAC-10" },
	{ 27, "MAG-7" },
	{ 33, "MP7" },
	{ 23, "MP5" },
	{ 34, "MP9" },
	{ 28, "Negev" },
	{ 35, "Nova" },
	{ 32, "P2000" },
	{ 36, "P250" },
	{ 19, "P90" },
	{ 26, "PP-Bizon" },
	{ 64, "R8 Revolver" },
	{ 29, "Sawed-Off" },
	{ 38, "SCAR-20" },
	{ 40, "SSG 08" },
	{ 39, "SG 553" },
	{ 30, "Tec-9" },
	{ 24, "UMP-45" },
	{ 61, "USP-S" },
	{ 25, "XM1014" },
};

void CConfig::SetupValue(int& value, int def, std::string category, std::string name) 
{
	value = def; ints.push_back(new ConfigValue< int >(category, name, &value, def));
}

void CConfig::SetupValue(char* value, char* def, std::string category, std::string name) 
{
	value = def; chars.push_back(new ConfigValue< char >(category, name, value, *def));
}

void CConfig::SetupValue(float& value, float def, std::string category, std::string name) 
{
	value = def; floats.push_back(new ConfigValue< float >(category, name, &value, def));
}

void CConfig::SetupValue(bool& value, bool def, std::string category, std::string name) 
{
	value = def; bools.push_back(new ConfigValue< bool >(category, name, &value, def));
}

void CConfig::SetupColor(float value[4], std::string name) 
{
	SetupValue(value[0], value[0], ("color"), name + "_r");
	SetupValue(value[1], value[1], ("color"), name + "_g");
	SetupValue(value[2], value[2], ("color"), name + "_b");
	SetupValue(value[3], value[3], ("color"), name + "_a");
}

void CConfig::SetupLegit() 
{
	for (auto& [key, val] : WeaponNames_Config)
	{
		SetupValue(g_Configurations.legitbot_items[key].enabled, false, (val), "legaim_enabled");
		SetupValue(g_Configurations.legitbot_items[key].deathmatch, false, (val), "legaim_deathmatch");
		SetupValue(g_Configurations.legitbot_items[key].autopistol, false, (val), "legaim_autopistol");
		SetupValue(g_Configurations.legitbot_items[key].smoke_check, false, (val), "legaim_smoke_check");
		SetupValue(g_Configurations.legitbot_items[key].flash_check, false, (val), "legaim_flash_check");
		SetupValue(g_Configurations.legitbot_items[key].jump_check, false, (val), "legaim_jump_check");
		SetupValue(g_Configurations.legitbot_items[key].autowall, false, (val), "legaim_autowall");
		SetupValue(g_Configurations.legitbot_items[key].silent, false, (val), "legaim_silent");
		SetupValue(g_Configurations.legitbot_items[key].autofire, false, (val), "legaim_autofire");
		SetupValue(g_Configurations.legitbot_items[key].autofire_key, 1, (val), "legaim_autofire_key");
		SetupValue(g_Configurations.legitbot_items[key].on_key, false, (val), "legaim_on_key");
		SetupValue(g_Configurations.legitbot_items[key].key, 1, (val), "legaim_key");
		SetupValue(g_Configurations.legitbot_items[key].rcs, false, (val), "legaim_rcs");
		SetupValue(g_Configurations.legitbot_items[key].rcs_fov_enabled, false, (val), "legaim_rcs_fov_enabled");
		SetupValue(g_Configurations.legitbot_items[key].rcs_smooth_enabled, false, (val), "legaim_rcs_smooth_enabled");
		SetupValue(g_Configurations.legitbot_items[key].autostop, false, (val), "legaim_autostop");
		SetupValue(g_Configurations.legitbot_items[key].backtrack_time, 0.0f, (val), "legaim_backtrack_time");
		SetupValue(g_Configurations.legitbot_items[key].only_in_zoom, false, (val), "legaim_only_in_zoom");
		SetupValue(g_Configurations.legitbot_items[key].aim_type, 1, (val), "legaim_aim_type");
		SetupValue(g_Configurations.legitbot_items[key].priority, 0, (val), "legaim_priority");
		SetupValue(g_Configurations.legitbot_items[key].rcs_type, 0, (val), "legaim_rcs_type");
		SetupValue(g_Configurations.legitbot_items[key].hitbox, 0, (val), "legaim_hitbox");
		SetupValue(g_Configurations.legitbot_items[key].fov, 0.f, (val), "legaim_fov");
		SetupValue(g_Configurations.legitbot_items[key].silent_fov, 0.f, (val), "legaim_silent_fov");
		SetupValue(g_Configurations.legitbot_items[key].rcs_fov, 0.f, (val), "legaim_rcs_fov");
		SetupValue(g_Configurations.legitbot_items[key].smooth, 1, (val), "legaim_smooth");
		SetupValue(g_Configurations.legitbot_items[key].rcs_smooth, 1, (val), "legaim_rcs_smooth");
		SetupValue(g_Configurations.legitbot_items[key].shot_delay, 0, (val), "legaim_shot_delay");
		SetupValue(g_Configurations.legitbot_items[key].kill_delay, 0, (val), "legaim_kill_delay");
		SetupValue(g_Configurations.legitbot_items[key].rcs_x, 100, (val), "legaim_rcs_x");
		SetupValue(g_Configurations.legitbot_items[key].rcs_y, 100, (val), "legaim_rcs_y");
		SetupValue(g_Configurations.legitbot_items[key].rcs_start, 1, (val), "legaim_rcs_start");
		SetupValue(g_Configurations.legitbot_items[key].min_damage, 1, (val), "legaim_min_damage");
	}
}

void CConfig::SetupVisuals() 
{
	SetupValue(g_Configurations.esp_enabled, false, "visuals", "visuals_esp_enabled");
	SetupValue(g_Configurations.esp_enemies_only, false, "visuals", "visuals_esp_enemies_only");
	SetupValue(g_Configurations.esp_player_boxes, false, "visuals", "visuals_esp_player_boxes");
	SetupValue(g_Configurations.esp_player_names, false, "visuals", "visuals_esp_player_names");
	SetupValue(g_Configurations.esp_player_health, false, "visuals", "visuals_esp_player_health");
	SetupValue(g_Configurations.esp_player_armour, false, "visuals", "visuals_esp_player_armour");
	SetupValue(g_Configurations.esp_player_weapons, false, "visuals", "visuals_esp_player_weapons");
	SetupValue(g_Configurations.esp_player_snaplines, false, "visuals", "visuals_esp_player_snaplines");
	SetupValue(g_Configurations.esp_crosshair, false, "visuals", "visuals_esp_crosshair");
	SetupValue(g_Configurations.esp_dropped_weapons, false, "visuals", "visuals_esp_dropped_weapons");
	SetupValue(g_Configurations.esp_defuse_kit, false, "visuals", "visuals_esp_defuse_kit");
	SetupValue(g_Configurations.esp_planted_c4, false, "visuals", "visuals_esp_planted_c4");
	SetupValue(g_Configurations.esp_items, false, "visuals", "visuals_esp_items");
	SetupValue(g_Configurations.esp_grenade_prediction, false, "visuals", "visuals_esp_grenade_prediction");
	SetupValue(g_Configurations.esp_draw_weapon_fov, true, "visuals", "visuals_esp_draw_weapon_fov");

	SetupValue(g_Configurations.glow_enabled, false, "visuals", "visuals_glow_enabled");
	SetupValue(g_Configurations.glow_enemies_only, false, "visuals", "visuals_glow_enemies_only");
	SetupValue(g_Configurations.glow_players, false, "visuals", "visuals_glow_players");
	SetupValue(g_Configurations.glow_chickens, false, "visuals", "visuals_glow_chickens");
	SetupValue(g_Configurations.glow_c4_carrier, false, "visuals", "visuals_glow_c4_carrier");
	SetupValue(g_Configurations.glow_planted_c4, false, "visuals", "visuals_glow_planted_c4");
	SetupValue(g_Configurations.glow_defuse_kits, false, "visuals", "visuals_glow_defuse_kits");
	SetupValue(g_Configurations.glow_weapons, false, "visuals", "visuals_glow_weapons");

	SetupValue(g_Configurations.chams_player_enabled, false, "visuals", "visuals_chams_player_enabled");
	SetupValue(g_Configurations.chams_player_enemies_only, false, "visuals", "visuals_chams_player_enemies_only");
	SetupValue(g_Configurations.chams_player_wireframe, false, "visuals", "visuals_chams_player_wireframe");
	SetupValue(g_Configurations.chams_player_flat, false, "visuals", "visuals_chams_player_flat");
	SetupValue(g_Configurations.chams_player_ignorez, false, "visuals", "visuals_chams_player_ignorez");
	SetupValue(g_Configurations.chams_player_glass, false, "visuals", "visuals_chams_player_glass");

	SetupValue(g_Configurations.chams_sleeve_enabled, false, "visuals", "visuals_chams_sleeve_enabled");
	SetupValue(g_Configurations.chams_sleeve_wireframe, false, "visuals", "visuals_chams_sleeve_wireframe");
	SetupValue(g_Configurations.chams_sleeve_flat, false, "visuals", "visuals_chams_sleeve_flat");
	SetupValue(g_Configurations.chams_sleeve_ignorez, false, "visuals", "visuals_chams_sleeve_ignorez");
	SetupValue(g_Configurations.chams_sleeve_glass, false, "visuals", "visuals_chams_sleeve_glass");

	SetupValue(g_Configurations.chams_arms_enabled, false, "visuals", "visuals_chams_arms_enabled");
	SetupValue(g_Configurations.chams_arms_wireframe, false, "visuals", "visuals_chams_arms_wireframe");
	SetupValue(g_Configurations.chams_arms_flat, false, "visuals", "visuals_chams_arms_flat");
	SetupValue(g_Configurations.chams_arms_ignorez, false, "visuals", "visuals_chams_arms_ignorez");
	SetupValue(g_Configurations.chams_arms_glass, false, "visuals", "visuals_chams_arms_glass");

	SetupValue(g_Configurations.chams_weapon_enabled, false, "visuals", "visuals_chams_weapon_enabled");
	SetupValue(g_Configurations.chams_weapon_wireframe, false, "visuals", "visuals_chams_weapon_wireframe");
	SetupValue(g_Configurations.chams_weapon_flat, false, "visuals", "visuals_chams_weapon_flat");
	SetupValue(g_Configurations.chams_weapon_ignorez, false, "visuals", "visuals_chams_weapon_ignorez");
	SetupValue(g_Configurations.chams_weapon_glass, false, "visuals", "visuals_chams_weapon_glass");
}

void CConfig::SetupMisc() 
{
	SetupValue(g_Configurations.misc_bhop, false, "miscellaneous", "miscellaneous_misc_bhop");
	SetupValue(g_Configurations.misc_autostrafe, false, "miscellaneous", "miscellaneous_misc_autostrafe");
	SetupValue(g_Configurations.misc_no_hands, false, "miscellaneous", "miscellaneous_misc_no_hands");
	SetupValue(g_Configurations.misc_thirdperson, false, "miscellaneous", "miscellaneous_misc_thirdperson");
	SetupValue(g_Configurations.misc_showranks, false, "miscellaneous", "miscellaneous_misc_showranks");
	SetupValue(g_Configurations.misc_watermark, false, "miscellaneous", "miscellaneous_misc_watermark");
	SetupValue(g_Configurations.misc_thirdperson_dist, 130.f, "miscellaneous", "miscellaneous_misc_thirdperson_dist");
	SetupValue(g_Configurations.misc_viewmodel_fov, 68, "miscellaneous", "miscellaneous_misc_viewmodel_fov");
	SetupValue(g_Configurations.misc_mat_ambient_light_r, 0.f, "miscellaneous", "miscellaneous_misc_mat_ambient_light_r");
	SetupValue(g_Configurations.misc_mat_ambient_light_g, 0.f, "miscellaneous", "miscellaneous_misc_mat_ambient_light_g");
	SetupValue(g_Configurations.misc_mat_ambient_light_b, 0.f, "miscellaneous", "miscellaneous_misc_mat_ambient_light_b");
}

void CConfig::SetupColors() 
{
	SetupColor(g_Configurations.color_esp_ally_visible, "color_esp_ally_visible");
	SetupColor(g_Configurations.color_esp_enemy_visible, "color_esp_enemy_visible");
	SetupColor(g_Configurations.color_esp_ally_occluded, "color_esp_ally_occluded");
	SetupColor(g_Configurations.color_esp_enemy_occluded, "color_esp_enemy_occluded");
	SetupColor(g_Configurations.color_esp_crosshair, "color_esp_crosshair");
	SetupColor(g_Configurations.color_esp_weapons, "color_esp_weapons");
	SetupColor(g_Configurations.color_esp_defuse, "color_esp_defuse");
	SetupColor(g_Configurations.color_esp_c4, "color_esp_c4");
	SetupColor(g_Configurations.color_esp_item, "color_esp_item");
	SetupColor(g_Configurations.color_grenade_prediction, "color_grenade_prediction");

	SetupColor(g_Configurations.color_glow_ally, "color_glow_ally");
	SetupColor(g_Configurations.color_glow_enemy, "color_glow_enemy");
	SetupColor(g_Configurations.color_glow_chickens, "color_glow_chickens");
	SetupColor(g_Configurations.color_glow_c4_carrier, "color_glow_c4_carrier");
	SetupColor(g_Configurations.color_glow_planted_c4, "color_glow_planted_c4");
	SetupColor(g_Configurations.color_glow_defuse, "color_glow_defuse");
	SetupColor(g_Configurations.color_glow_weapons, "color_glow_weapons");

	SetupColor(g_Configurations.color_chams_player_ally_visible, "color_chams_player_ally_visible");
	SetupColor(g_Configurations.color_chams_player_ally_occluded, "color_chams_player_ally_occluded");
	SetupColor(g_Configurations.color_chams_player_enemy_visible, "color_chams_player_enemy_visible");
	SetupColor(g_Configurations.color_chams_player_enemy_occluded, "color_chams_player_enemy_occluded");

	SetupColor(g_Configurations.color_chams_arms_visible, "color_chams_arms_visible");
	SetupColor(g_Configurations.color_chams_arms_occluded, "color_chams_arms_occluded");

	SetupColor(g_Configurations.color_chams_sleeve_visible, "color_chams_sleeve_visible");
	SetupColor(g_Configurations.color_chams_sleeve_occluded, "color_chams_sleeve_occluded");

	SetupColor(g_Configurations.color_chams_weapon_visible, "color_chams_weapon_visible");
	SetupColor(g_Configurations.color_chams_weapon_occluded, "color_chams_weapon_occluded");

	SetupColor(g_Configurations.color_watermark, "color_watermark");
}

void CConfig::Setup() 
{
	CreateDirectory("C:\\csgo_sdk\\", NULL);
	CConfig::SetupLegit();
	CConfig::SetupVisuals();
	CConfig::SetupMisc();
	CConfig::SetupColors();
}

void CConfig::Save(const std::string& name) 
{
	if (name.empty()) 
		return;

	CreateDirectory("C:\\csgo_sdk\\", NULL);
	std::string file = "C:\\csgo_sdk\\" + name;

	for (auto value : ints)
		WritePrivateProfileString(value->category.c_str(), value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());

	for (auto value : floats)
		WritePrivateProfileString(value->category.c_str(), value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());

	for (auto value : bools)
		WritePrivateProfileString(value->category.c_str(), value->name.c_str(), *value->value ? "true" : "false", file.c_str());

	for (auto value : chars)
		WritePrivateProfileString(value->category.c_str(), value->name.c_str(), value->value, file.c_str());
}

void CConfig::Load(const std::string& name) 
{
	if (name.empty()) 
		return;

	g_ClientState->ForceFullUpdate();

	CreateDirectory("C:\\csgo_sdk\\", NULL);
	std::string file = "C:\\csgo_sdk\\" + name;

	char value_l[32] = { '\0' };

	for (auto value : ints)
	{
		GetPrivateProfileString(value->category.c_str(), value->name.c_str(), "0", value_l, 32, file.c_str()); *value->value = atoi(value_l);
	}

	for (auto value : floats)
	{
		GetPrivateProfileString(value->category.c_str(), value->name.c_str(), "0.0f", value_l, 32, file.c_str()); *value->value = atof(value_l);
	}

	for (auto value : bools)
	{
		GetPrivateProfileString(value->category.c_str(), value->name.c_str(), "false", value_l, 32, file.c_str()); *value->value = !strcmp(value_l, "true");
	}
}