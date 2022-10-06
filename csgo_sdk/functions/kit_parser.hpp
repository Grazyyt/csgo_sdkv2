#pragma once
#include <vector>
#include <map>
#include <xstring>


struct paint_kit
{
	int Id;
	int Rarity;
	std::string Name;
	std::string NameShort;
	std::vector<std::string> WeaponName;
	auto operator < (const paint_kit& other) const -> bool
	{
		return Name < other.Name;
	}
};
extern bool SkinsParsed;
extern std::vector<paint_kit> k_Skins;
extern std::vector<paint_kit> k_Gloves;
extern std::vector<paint_kit> k_Stickers;
extern auto InitializeKits() -> void;
