#include "skins.hpp"
#include "../configurations.hpp"
#include "../functions/item_definitions.hpp"


ImVec4 skins::GetColorRatiry(int rar)
{
	switch (rar)
	{
	case 1:
		return ImColor(150, 150, 150, 255);
		break;
	case 2:
		return ImColor(100, 100, 255, 255);
		break;
	case 3:
		return ImColor(50, 50, 255, 255);
		break;
	case 4:
		return ImColor(255, 64, 242, 255);
		break;
	case 5:
		return ImColor(255, 50, 50, 255);
		break;
	case 6:
		return ImColor(255, 50, 50, 255);
		break;
	case 7:
		return ImColor(255, 196, 46, 255);
		break;
	default:
		return ImColor(150, 150, 150, 255);
		break;
	}
}

ImVec4 skins::ggnibba(int bob)
{
	switch (bob)
	{
	case 1:
		return ImColor(50, 255, 50, 255);
		break;
	case 2:
		return ImColor(255, 50, 50, 255);
		break;
	default:
		return ImColor(150, 150, 150, 255);
		break;
	}
}

static auto erase_override_if_exists_by_index(const int definition_index) -> void
{
	if (k_WeaponInfo.count(definition_index))
	{
		auto& icon_override_map = g_Configurations.m_IconOverrides;
		const auto& original_item = k_WeaponInfo.at(definition_index);
		if (original_item.icon && icon_override_map.count(original_item.icon))
			icon_override_map.erase(icon_override_map.at(original_item.icon)); // Remove the leftover override
	}
}

bool get_skins_cur(int weaponidx)
{
	return true;
}

const char* skins::GetIconOverride(const std::string original)
{
	return g_Configurations.m_IconOverrides.count(original) ? g_Configurations.m_IconOverrides.at(original).data() : nullptr;
}

static auto ApplyConfigOnAttributableItem(C_BaseAttributableItem* item, const item_setting* config,
	const unsigned xuid_low) -> void
{
	item->m_Item().m_iItemIDHigh() = -1;
	item->m_Item().m_iAccountID() = xuid_low;
	if (config->custom_name[0])
		strcpy_s(item->m_Item().m_iCustomName(), config->custom_name);
	if (config->paint_kit_index)
		item->m_nFallbackPaintKit() = config->paint_kit_index;
	if (config->seed)
		item->m_nFallbackSeed() = config->seed;
	if (config->stat_trak) {
		item->m_nFallbackStatTrak() = g_Configurations.statrack_items[config->definition_index].statrack_new.counter;
		item->m_Item().m_iEntityQuality() = 9;
	}
	else {
		item->m_Item().m_iEntityQuality() = IsKnife(config->definition_index) ? 3 : 0;
	}
	item->m_flFallbackWear() = config->wear;
	auto& definition_index = item->m_Item().m_iItemDefinitionIndex();
	auto& icon_override_map = g_Configurations.m_IconOverrides;
	if (config->definition_override_index && config->definition_override_index != definition_index && k_WeaponInfo.count(config->definition_override_index))
	{
		const auto old_definition_index = definition_index;
		definition_index = config->definition_override_index;
		const auto& replacement_item = k_WeaponInfo.at(config->definition_override_index);
		item->m_nModelIndex() = g_MdlInfo->GetModelIndex(replacement_item.model);
		item->SetGloveModelIndex(g_MdlInfo->GetModelIndex(replacement_item.model));
		item->GetClientNetworkable()->PreDataUpdate(0);
		if (old_definition_index && k_WeaponInfo.count(old_definition_index))
		{
			const auto& original_item = k_WeaponInfo.at(old_definition_index);
			if (original_item.icon && replacement_item.icon)
			{
				icon_override_map[original_item.icon] = replacement_item.icon;
			}
		}
	}
	else
	{
		erase_override_if_exists_by_index(definition_index);
	}
}
static auto GetWearableCreateFn() -> CreateClientClassFn
{
	auto clazz = g_CHLClient->GetAllClasses();
	// Please, if you gonna paste it into a cheat use classids here. I use names because they
	// won't change in the foreseeable future and i dont need high speed, but chances are
	// you already have classids, so use them instead, they are faster.
	while (strcmp(clazz->m_pNetworkName, "CEconWearable"))
		clazz = clazz->m_pNext;
	return clazz->m_pCreateFn;
}

void skins::onFrameStageNotify(bool frame_end)
{
	const auto local_index = g_EngineClient->GetLocalPlayer();
	const auto local = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(local_index));
	if (!local)
		return;
	player_info_t player_info;
	if (!g_EngineClient->GetPlayerInfo(local_index, &player_info))
		return;


	if (frame_end)
	{
		const auto wearables = local->m_hMyWearables();
		const auto glove_config = &g_Configurations.m_items[local->m_iTeamNum() == 3 ? GLOVE_CT_SIDE : GLOVE_T_SIDE];
		static auto glove_handle = CBaseHandle(0);
		auto glove = reinterpret_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntityFromHandle(wearables[0]));
		if (!glove)
		{
			const auto our_glove = reinterpret_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntityFromHandle(glove_handle));
			if (our_glove) // Our glove still exists
			{
				wearables[0] = glove_handle;
				glove = our_glove;
			}
		}
		if (!local->IsAlive())
		{
			if (glove)
			{
				glove->GetClientNetworkable()->SetDestroyedOnRecreateEntities();
				glove->GetClientNetworkable()->Release();
			}
			return;
		}
		if (glove_config && glove_config->definition_override_index)
		{
			if (!glove)
			{
				static auto create_wearable_fn = GetWearableCreateFn();
				const auto entry = g_EntityList->GetHighestEntityIndex() + 1;
				const auto serial = rand() % 0x1000;
				//glove = static_cast<C_BaseAttributableItem*>(create_wearable_fn(entry, serial));
				create_wearable_fn(entry, serial);
				glove = reinterpret_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntity(entry));
				assert(glove);
				{
					static auto set_abs_origin_addr = Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8");
					const auto set_abs_origin_fn = reinterpret_cast<void(__thiscall*)(void*, const std::array<float, 3>&)>(set_abs_origin_addr);
					static constexpr std::array<float, 3> new_pos = { 10000.f, 10000.f, 10000.f };
					set_abs_origin_fn(glove, new_pos);
				}
				const auto wearable_handle = reinterpret_cast<CBaseHandle*>(&wearables[0]);
				*wearable_handle = entry | serial << 16;
				glove_handle = wearables[0];
			}
			// Thanks, Beakers
			glove->SetGloveModelIndex(-1);
			ApplyConfigOnAttributableItem(glove, glove_config, player_info.xuid_low);
		}
	}
	else
	{
		auto weapons = local->m_hMyWeapons();
		for (int i = 0; weapons[i].IsValid(); i++)
		{
			C_BaseAttributableItem* weapon = (C_BaseAttributableItem*)g_EntityList->GetClientEntityFromHandle(weapons[i]);
			if (!weapon)
				continue;
			auto& definition_index = weapon->m_Item().m_iItemDefinitionIndex();
			const auto active_conf = &g_Configurations.m_items[IsKnife(definition_index) ? (local->m_iTeamNum() == 3 ? WEAPON_KNIFE : WEAPON_KNIFE_T) : definition_index];
			ApplyConfigOnAttributableItem(weapon, active_conf, player_info.xuid_low);
		}
	}
	const auto view_model_handle = local->m_hViewModel();
	if (!view_model_handle.IsValid())
		return;
	const auto view_model = static_cast<C_BaseViewModel*>(g_EntityList->GetClientEntityFromHandle(view_model_handle));
	if (!view_model)
		return;
	const auto view_model_weapon_handle = view_model->m_hWeapon();
	if (!view_model_weapon_handle.IsValid())
		return;
	const auto view_model_weapon = static_cast<C_BaseCombatWeapon*>(g_EntityList->GetClientEntityFromHandle(view_model_weapon_handle));
	if (!view_model_weapon)
		return;
	if (k_WeaponInfo.count(view_model_weapon->m_Item().m_iItemDefinitionIndex()))
	{
		const auto override_model = k_WeaponInfo.at(view_model_weapon->m_Item().m_iItemDefinitionIndex()).model;
		auto override_model_index = g_MdlInfo->GetModelIndex(override_model);

		view_model->m_nModelIndex() = override_model_index;
		auto world_model_handle = view_model_weapon->m_hWeaponWorldModel();
		if (!world_model_handle.IsValid())
			return;
		const auto world_model = static_cast<C_BaseWeaponWorldModel*>(g_EntityList->GetClientEntityFromHandle(world_model_handle));
		if (!world_model)
			return;
		world_model->m_nModelIndex() = override_model_index + 1;

	}
}