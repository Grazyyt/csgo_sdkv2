#include <algorithm>
#include "kit_parser.hpp"
#include "../sdk/utils/utils.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <string>
#include "parser.h"
#include <set>
#include <map>
#include "../configurations.hpp"
#include "parser.hpp"


struct skin_info
{
	int seed = -1;
	int paintkit;
	int rarity = 0;
	std::string tag_name;
	std::string cdn_name;
};

bool SkinsParsed = false;
std::vector<paint_kit> k_Skins;
std::vector<paint_kit> k_Gloves;
std::vector<paint_kit> k_Stickers;
class CCStrike15ItemSchema;
class CCStrike15ItemSystem;
template <typename Key, typename Value>
struct Node_t
{
	int previous_id; //0x0000
	int next_id; //0x0004
	void* _unknown_ptr; //0x0008
	int _unknown; //0x000C
	Key key; //0x0010
	Value value; //0x0014
};
template <typename Key, typename Value>
struct Head_t
{
	Node_t<Key, Value>* memory; //0x0000
	int allocation_count; //0x0004
	int grow_size; //0x0008
	int start_element; //0x000C
	int next_available; //0x0010
	int _unknown; //0x0014
	int last_element; //0x0018
}; //Size=0x001C
   // could use CUtlString but this is just easier and CUtlString isn't needed anywhere else
struct String_t
{
	char* buffer; //0x0000
	int capacity; //0x0004
	int grow_size; //0x0008
	int length; //0x000C
}; //Size=0x0010
struct CPaintKit
{
	int id; //0x0000
	String_t name; //0x0004
	String_t description; //0x0014
	String_t item_name; //0x0024
	String_t material_name; //0x0034
	String_t image_inventory; //0x0044
	char pad_0x0054[0x8C]; //0x0054
}; //Size=0x00E0

struct CStickerKit
{
	int id;

	int item_rarity;

	String_t name;
	String_t description;
	String_t item_name;
	String_t material_name;
	String_t image_inventory;

	int tournament_event_id;
	int tournament_team_id;
	int tournament_player_id;
	bool is_custom_sticker_material;

	float rotate_end;
	float rotate_start;

	float scale_min;
	float scale_max;

	float wear_min;
	float wear_max;

	String_t image_inventory2;
	String_t image_inventory_large;

	std::uint32_t pad0[4];
};
int GetWeaponRarity(std::string rarity)
{
	if (rarity == "default")
		return 0;
	else if (rarity == "common")
		return 1;
	else if (rarity == "uncommon")
		return 2;
	else if (rarity == "rare")
		return 3;
	else if (rarity == "mythical")
		return 4;
	else if (rarity == "legendary")
		return 5;
	else if (rarity == "ancient")
		return 6;
	else if (rarity == "immortal")
		return 7;
	else if (rarity == "unusual")
		return 99;

	return 0;
}
auto get_export(const char* module_name, const char* export_name) -> void*
{
	HMODULE mod;
	while (!((mod = GetModuleHandleA(module_name))))
		Sleep(100);
	return reinterpret_cast<void*>(GetProcAddress(mod, export_name));
}

auto InitializeKits() -> void
{
	std::unordered_map<std::string, std::set<std::string>> weaponSkins;
	std::unordered_map<std::string, skin_info> skinMap;
	std::unordered_map<std::string, std::string> skinNames;


	valve_parser::document doc;
	auto r = doc.load(R"(.\csgo\scripts\items\items_game.txt)", valve_parser::encoding::utf8);
	if (!r)
		return;

	valve_parser::document english;
	r = english.load(R"(.\csgo\resource\csgo_english.txt)", valve_parser::encoding::utf16_le);
	if (!r)
		return;

	auto weapon_skin_combo = doc.breadth_first_search("weapon_icons");
	if (!weapon_skin_combo || !weapon_skin_combo->to_object())
		return;

	auto paint_kits_rarity = doc.breadth_first_search_multiple("paint_kits_rarity");
	if (paint_kits_rarity.empty())
		return;

	auto skin_data_vec = doc.breadth_first_search_multiple("paint_kits");
	if (skin_data_vec.empty())
		return;

	auto paint_kit_names = english.breadth_first_search("Tokens");
	if (!paint_kit_names || !paint_kit_names->to_object())
		return;

	std::array weaponNames = {
		std::string("deagle"),
		std::string("elite"),
		std::string("fiveseven"),
		std::string("glock"),
		std::string("ak47"),
		std::string("aug"),
		std::string("awp"),
		std::string("famas"),
		std::string("g3sg1"),
		std::string("galilar"),
		std::string("m249"),
		std::string("m4a1_silencer"),
		std::string("m4a1"),
		std::string("mac10"),
		std::string("p90"),
		std::string("ump45"),
		std::string("xm1014"),
		std::string("bizon"),
		std::string("mag7"),
		std::string("negev"),
		std::string("sawedoff"),
		std::string("tec9"),
		std::string("hkp2000"),
		std::string("mp5sd"),
		std::string("mp7"),
		std::string("mp9"),
		std::string("nova"),
		std::string("p250"),
		std::string("scar20"),
		std::string("sg556"),
		std::string("ssg08"),
		std::string("usp_silencer"),
		std::string("cz75a"),
		std::string("revolver"),
		std::string("knife_m9_bayonet"),
		std::string("bayonet"),
		std::string("knife_flip"),
		std::string("knife_gut"),
		std::string("knife_css"),
		std::string("knife_cord"),
		std::string("knife_canis"),
		std::string("knife_karambit"),
		std::string("knife_tactical"),
		std::string("knife_outdoor"),
		std::string("knife_skeleton"),
		std::string("knife_falchion"),
		std::string("knife_survival_bowie"),
		std::string("knife_butterfly"),
		std::string("knife_push"),
		std::string("knife_ursus"),
		std::string("knife_gypsy_jackknife"),
		std::string("knife_stiletto"),
		std::string("knife_widowmaker"),
		std::string("studded_bloodhound_gloves"),
		std::string("sporty_gloves"),
		std::string("slick_gloves"),
		std::string("leather_handwraps"),
		std::string("motorcycle_gloves"),
		std::string("specialist_gloves"),
		std::string("studded_hydra_gloves")
	};

	for (const auto& child : weapon_skin_combo->children)
	{
		if (child->to_object())
		{
			for (const auto& weapon : weaponNames)
			{
				auto skin_name = child->to_object()->get_key_by_name("icon_path")->to_key_value()->value.to_string();
				const auto pos = skin_name.find(weapon);
				if (pos != std::string::npos)
				{
					const auto pos2 = skin_name.find_last_of('_');
					weaponSkins[weapon].insert(
						skin_name.substr(pos + weapon.length() + 1,
							pos2 - pos - weapon.length() - 1)
					);
					break;
				}
			}
		}
	}

	for (const auto& skin_data : skin_data_vec)
	{
		if (skin_data->to_object())
		{
			for (const auto& skin : skin_data->children)
			{
				if (skin->to_object())
				{
					skin_info si;
					si.paintkit = skin->to_object()->name.to_int();

					if (si.paintkit == 0)
						continue;

					auto skin_name = skin->to_object()->get_key_by_name("name")->to_key_value()->value.to_string();
					si.cdn_name = skin_name;
					auto tag_node = skin->to_object()->get_key_by_name("description_tag");
					if (tag_node)
					{
						auto tag = tag_node->to_key_value()->value.to_string();
						tag = tag.substr(1, std::string::npos);
						std::transform(tag.begin(), tag.end(), tag.begin(), towlower);
						si.tag_name = tag;
					}



					auto key_val = skin->to_object()->get_key_by_name("seed");
					if (key_val != nullptr)
						si.seed = key_val->to_key_value()->value.to_int();

					skinMap[skin_name] = si;
				}
			}
		}
	}

	for (const auto& child : paint_kit_names->children)
	{
		if (child->to_key_value())
		{
			auto key = child->to_key_value()->key.to_string();
			std::transform(key.begin(), key.end(), key.begin(), towlower);
			if (key.find("paintkit") != std::string::npos &&
				key.find("tag") != std::string::npos)
			{
				skinNames[key] = child->to_key_value()->value.to_string();
			}
		}
	}

	for (const auto& rarity : paint_kits_rarity)
	{
		if (rarity->to_object())
		{
			for (const auto& child : rarity->children)
			{
				if (child->to_key_value())
				{
					std::string paint_kit_name = child->to_key_value()->key.to_string();
					std::string paint_kit_rarity = child->to_key_value()->value.to_string();

					auto skinInfo = &skinMap[paint_kit_name];

					skinInfo->rarity = GetWeaponRarity(paint_kit_rarity);
				}
			}
		}
	}

	for (auto weapon : weaponNames)
	{
		for (auto skin : weaponSkins[weapon])
		{
			if (skinMap[skin].paintkit < 10000)
			{
				paint_kit info; //= &k_skins[skinMap[skin].paintkit];
				info.Id = skinMap[skin].paintkit;
				info.WeaponName.push_back(weapon);
				info.NameShort = skin;
				info.Name = skin == "cu_m4a4_ancestral" ? "Dragon King" : skinNames[skinMap[skin].tag_name];
				info.Rarity = skinMap[skin].rarity;

				if (!skinNames[skinMap[skin].tag_name].empty())
					k_Skins.push_back(info);
			}
			else
			{
				paint_kit info; //= //&k_skins[skinMap[skin].paintkit];
				info.Id = skinMap[skin].paintkit;
				info.WeaponName.push_back(weapon);
				info.NameShort = skin;
				info.Name = skinNames[skinMap[skin].tag_name];
				info.Rarity = skinMap[skin].rarity;
				k_Gloves.push_back(info);
			}

			std::sort(k_Gloves.begin(), k_Gloves.end());
			std::sort(k_Skins.begin(), k_Skins.end());
		}
	}
	SkinsParsed = true;
}
