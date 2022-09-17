#pragma once
#include "../sdk/misc/Recv.hpp"
#include "../sdk/sdk.hpp"
#include "../configurations.hpp"
#include "../sdk/utils/utils.hpp"
#include "../sdk/csgostructs.hpp"

class C_BaseCombatWeapon;
class attributable_item_t;

namespace skins
{
	namespace general
	{
		void FullUpdate();
	}

	namespace knifes
	{
		void UpdateKnife();
		const char* UpdateKillIcons();
		int GetNewAnimation(const char* model, const int sequence);
		void DoSequenceRemapping(CRecvProxyData* data, C_BaseViewModel* entity);
	}

	struct knife_t
	{
		int item_definition_index;
		const char* model_name;
		const char* killfeed_name;
	};

	struct weapon_info
	{
		constexpr weapon_info(const char* model, const char* icon = nullptr) :
			model(model),
			icon(icon)
		{}

		const char* model;
		const char* icon;
	};
	
}