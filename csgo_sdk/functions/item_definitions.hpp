#pragma once
#include <map>
#include <string>
#include <vector>
#include "../sdk/csgostructs.hpp"

static auto IsKnife(const int i) -> bool
{
	return (i >= WEAPON_KNIFE_BAYONET && i < GLOVE_STUDDED_BLOODHOUND) || i == WEAPON_KNIFE_T || i == WEAPON_KNIFE;
}


struct WeaponInfo
{
	constexpr WeaponInfo(const char* model, const char* icon = nullptr) :
		model(model),
		icon(icon)
	{}

	const char* model;
	const char* icon;
};

struct WeaponName
{
	int definition_index = 0;
	std::string name = nullptr;

	WeaponName(const int definition_index, const char* name) :
		definition_index(definition_index),
		name(name)
	{}
};
extern const std::map<size_t, WeaponInfo> k_WeaponInfo;
extern const std::vector<WeaponName> k_KnifeNames;
extern const std::vector<WeaponName> k_GloveNames;
extern const std::vector<WeaponName> k_WeaponNames;
extern int definition_vector_index;

extern const std::vector<WeaponName> k_KnifeNamesPreview;
extern const std::vector<WeaponName> k_GloveNamesPreview;
extern const std::vector<WeaponName> k_WeaponNamesPreview;