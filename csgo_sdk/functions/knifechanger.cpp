#include "knifechanger.hpp"

namespace skins::general
{
	struct hud_weapons_t
	{
		std::int32_t* wpn_count()
		{
			return reinterpret_cast<std::int32_t*>(std::uintptr_t(this) + 0x80);
		}
	};

	void FullUpdate()
	{
		if (!g_EngineClient->IsInGame())
			return;

		static auto clear_hud_weapon_icon_fn = reinterpret_cast<std::int32_t(__thiscall*)(void*, std::int32_t)>(
			Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 51 53 56 8B 75 08 8B D9 57 6B"));

		auto hud_weapons = Utils::FindHudElement<std::uintptr_t>(("CCSGO_HudWeaponSelection")) - 0x28;

		if (hud_weapons == nullptr)
			return;

		for (std::size_t i = 0; i < *(hud_weapons + 0x20); i++)
			i = clear_hud_weapon_icon_fn(hud_weapons, i);

		g_ClientState->ForceFullUpdate();
	}
}

namespace skins::knifes
{
	uintptr_t world_model_handle;
	C_BaseViewModel* world_model;
	std::unordered_map<int, const char*> viewmodel_cfg;

	std::vector<knife_t> knives = {
		{ 500, "models/weapons/v_knife_bayonet.mdl", "bayonet" },
		{ 503, "models/weapons/v_knife_css.mdl", "knife_css" },
		{ 505, "models/weapons/v_knife_flip.mdl", "knife_flip" },
		{ 506, "models/weapons/v_knife_gut.mdl", "knife_gut" },
		{ 507, "models/weapons/v_knife_karam.mdl", "knife_karambit" },
		{ 508, "models/weapons/v_knife_m9_bay.mdl", "knife_m9_bayonet" },
		{ 509, "models/weapons/v_knife_tactical.mdl", "knife_tactical"},
		{ 512, "models/weapons/v_knife_falchion_advanced.mdl", "knife_falchion" },
		{ 514, "models/weapons/v_knife_survival_bowie.mdl", "knife_survival_bowie" },
		{ 515, "models/weapons/v_knife_butterfly.mdl", "knife_butterfly" },
		{ 516, "models/weapons/v_knife_push.mdl", "knife_push"},
		{ 517, "models/weapons/v_knife_cord.mdl", "knife_cord"},
		{ 518, "models/weapons/v_knife_canis.mdl", "knife_canis"},
		{ 519, "models/weapons/v_knife_ursus.mdl", "knife_ursus"},
		{ 520, "models/weapons/v_knife_gypsy_jackknife.mdl", "knife_gypsy_jackknife"},
		{ 521, "models/weapons/v_knife_outdoor.mdl", "knife_outdoor" },
		{ 522, "models/weapons/v_knife_stiletto.mdl", "knife_stiletto" },
		{ 523, "models/weapons/v_knife_widowmaker.mdl", "knife_widowmaker" },
		{ 525, "models/weapons/v_knife_skeleton.mdl", "knife_skeleton" }
	};

	void UpdateKnife()
	{
		if (!g_LocalPlayer)
		{
			skins::general::FullUpdate();
			return;
		}

		auto weapons = g_LocalPlayer->m_hMyWeapons();
		for (int i = 0; weapons[i].IsValid(); i++)
		{
			C_BaseCombatWeapon* weapon = (C_BaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(weapons[i]);
			if (!weapon)
				continue;

			C_BaseAttributableItem* weapon_attributable = (C_BaseAttributableItem*)weapon;

			world_model_handle = weapon_attributable->m_hWeaponWorldModel();

			if (world_model_handle)
				world_model = (C_BaseViewModel*)g_EntityList->GetClientEntity(world_model_handle);

			if (weapon->IsKnife())
			{
				int chosen_knife = g_Configurations.misc_knifemodel - 1;
				if (chosen_knife < 0)
					return;

				int model = g_MdlInfo->GetModelIndex(knives[chosen_knife].model_name);

				weapon_attributable->m_Item().m_iItemDefinitionIndex() = knives[chosen_knife].item_definition_index;

				if (g_LocalPlayer->m_hViewModel())
				{
					if (g_LocalPlayer->m_hActiveWeapon() && g_LocalPlayer->m_hActiveWeapon()->IsKnife())
					{
						g_LocalPlayer->m_hViewModel()->m_nModelIndex() = model;
						g_LocalPlayer->m_hViewModel()->m_nViewModelIndex() = model;
						g_LocalPlayer->m_hViewModel()->m_nSequence();
					}
				}

				((C_BaseViewModel*)weapon)->m_nModelIndex() = model;
				((C_BaseViewModel*)weapon)->m_nViewModelIndex() = model;

				if (world_model)
				{
					world_model->m_nModelIndex() = model + 1;
				}

				weapon_attributable->m_flFallbackWear() = 0.000000001f;
				weapon_attributable->m_nFallbackSeed() = 0;
				weapon_attributable->m_nFallbackStatTrak() = -1;
				weapon_attributable->m_Item().m_iItemIDHigh() = -1;
				weapon_attributable->m_Item().m_iEntityQuality() = 3;
			}
			static int last_knife = g_Configurations.misc_knifemodel;
			if (last_knife != g_Configurations.misc_knifemodel)
			{
				skins::general::FullUpdate();
				last_knife = g_Configurations.misc_knifemodel;
			}
		}
	}

	const char* UpdateKillIcons()
	{
		int chosen_knife = g_Configurations.misc_knifemodel - 1;
		if (chosen_knife < 0)
		{
			return "";
		}

		return knives[chosen_knife].killfeed_name;
	}

	const std::map<size_t, weapon_info> k_weapon_info =
	{
		{WEAPON_KNIFE,{"models/weapons/v_knife_default_ct.mdl", "knife"}},
		{WEAPON_KNIFE_T,{"models/weapons/v_knife_default_t.mdl", "knife_t"}},
		{WEAPON_KNIFE_BAYONET, {"models/weapons/v_knife_bayonet.mdl", "bayonet"}},
		{WEAPON_KNIFE_CSS,{"models/weapons/v_knife_css.mdl", "knife_css"}},

		{WEAPON_KNIFE_SKELETON,{"models/weapons/v_knife_skeleton.mdl", "knife_skeleton"}},
		{WEAPON_KNIFE_OUTDOOR,{"models/weapons/v_knife_outdoor.mdl", "knife_outdoor"}},
		{WEAPON_KNIFE_CANIS,{"models/weapons/v_knife_canis.mdl", "knife_canis"}},
		{WEAPON_KNIFE_CORD,{"models/weapons/v_knife_cord.mdl", "knife_cord"}},


		{WEAPON_KNIFE_FLIP, {"models/weapons/v_knife_flip.mdl", "knife_flip"}},
		{WEAPON_KNIFE_GUT, {"models/weapons/v_knife_gut.mdl", "knife_gut"}},
		{WEAPON_KNIFE_KARAMBIT, {"models/weapons/v_knife_karam.mdl", "knife_karambit"}},
		{WEAPON_KNIFE_M9_BAYONET, {"models/weapons/v_knife_m9_bay.mdl", "knife_m9_bayonet"}},
		{WEAPON_KNIFE_TACTICAL, {"models/weapons/v_knife_tactical.mdl", "knife_tactical"}},
		{WEAPON_KNIFE_FALCHION, {"models/weapons/v_knife_falchion_advanced.mdl", "knife_falchion"}},
		{WEAPON_KNIFE_SURVIVAL_BOWIE, {"models/weapons/v_knife_survival_bowie.mdl", "knife_survival_bowie"}},
		{WEAPON_KNIFE_BUTTERFLY, {"models/weapons/v_knife_butterfly.mdl", "knife_butterfly"}},
		{WEAPON_KNIFE_PUSH, {"models/weapons/v_knife_push.mdl", "knife_push"}},
		{WEAPON_KNIFE_URSUS,{"models/weapons/v_knife_ursus.mdl", "knife_ursus"}},
		{WEAPON_KNIFE_GYPSY_JACKKNIFE,{"models/weapons/v_knife_gypsy_jackknife.mdl", "knife_gypsy_jackknife"}},
		{WEAPON_KNIFE_STILETTO,{"models/weapons/v_knife_stiletto.mdl", "knife_stiletto"}},
		{WEAPON_KNIFE_WIDOWMAKER,{"models/weapons/v_knife_widowmaker.mdl", "knife_widowmaker"}},
	};

	int random_sequence(const int low, const int high)
	{
		return rand() % (high - low + 1) + low;
	}

	// This only fixes if the original knife was a default knife.
	// The best would be having a function that converts original knife's sequence
	// into some generic enum, then another function that generates a sequence
	// from the sequences of the new knife. I won't write that.
	int GetNewAnimation(const char* model, const int sequence)
	{
		enum ESequence
		{
			SEQUENCE_DEFAULT_DRAW = 0,
			SEQUENCE_DEFAULT_IDLE1 = 1,
			SEQUENCE_DEFAULT_IDLE2 = 2,
			SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
			SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
			SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
			SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
			SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
			SEQUENCE_DEFAULT_LOOKAT01 = 12,

			SEQUENCE_BUTTERFLY_DRAW = 0,
			SEQUENCE_BUTTERFLY_DRAW2 = 1,
			SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
			SEQUENCE_BUTTERFLY_LOOKAT03 = 15,

			SEQUENCE_FALCHION_IDLE1 = 1,
			SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
			SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
			SEQUENCE_FALCHION_LOOKAT01 = 12,
			SEQUENCE_FALCHION_LOOKAT02 = 13,

			SEQUENCE_CSS_LOOKAT01 = 14,
			SEQUENCE_CSS_LOOKAT02 = 15,

			SEQUENCE_DAGGERS_IDLE1 = 1,
			SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
			SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
			SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
			SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,

			SEQUENCE_BOWIE_IDLE1 = 1,
		};

		// Hashes for best performance.
		
		if (strstr(model, "models/weapons/v_knife_butterfly.mdl"))
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_DRAW:
				return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
			default:
				return sequence + 1;
			}
		}
		else if (strstr(model, "models/weapons/v_knife_falchion_advanced.mdl"))
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_IDLE2:
				return SEQUENCE_FALCHION_IDLE1;
			case SEQUENCE_DEFAULT_HEAVY_MISS1:
				return random_sequence(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
			case SEQUENCE_DEFAULT_DRAW:
			case SEQUENCE_DEFAULT_IDLE1:
				return sequence;
			default:
				return sequence - 1;
			}
		}
		else if (strstr(model, "models/weapons/v_knife_css.mdl"))
		{
			switch (sequence)
			{
			case 14:
				return random_sequence(SEQUENCE_CSS_LOOKAT01, SEQUENCE_CSS_LOOKAT02);
			default:
				return sequence;
			}
		}
		else if (strstr(model, "models/weapons/v_knife_push.mdl"))
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_IDLE2:
				return SEQUENCE_DAGGERS_IDLE1;
			case SEQUENCE_DEFAULT_LIGHT_MISS1:
			case SEQUENCE_DEFAULT_LIGHT_MISS2:
				return random_sequence(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
			case SEQUENCE_DEFAULT_HEAVY_MISS1:
				return random_sequence(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
			case SEQUENCE_DEFAULT_HEAVY_HIT1:
			case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
			case SEQUENCE_DEFAULT_LOOKAT01:
				return sequence + 3;
			case SEQUENCE_DEFAULT_DRAW:
			case SEQUENCE_DEFAULT_IDLE1:
				return sequence;
			default:
				return sequence + 2;
			}
		}
		else if (strstr(model, "models/weapons/v_knife_survival_bowie.mdl"))
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_DRAW:
			case SEQUENCE_DEFAULT_IDLE1:
				return sequence;
			case SEQUENCE_DEFAULT_IDLE2:
				return SEQUENCE_BOWIE_IDLE1;
			default:
				return sequence - 1;
			}
		}
		else if (strstr(model, "models/weapons/v_knife_ursus.mdl"))
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_DRAW:
				return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
			default:
				return sequence + 1;
			}
		}
		else if (strstr(model, "models/weapons/v_knife_cord.mdl"))
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_DRAW:
				return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
			default:
				return sequence + 1;
			}
		}
		else if (strstr(model, "models/weapons/v_knife_canis.mdl"))
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_DRAW:
				return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
			default:
				return sequence + 1;
			}
		}
		else if (strstr(model, "models/weapons/v_knife_outdoor.mdl"))
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_DRAW:
				return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
			default:
				return sequence + 1;
			}
		}
		else if (strstr(model, "models/weapons/v_knife_skeleton.mdl"))
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_DRAW:
				return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
			default:
				return sequence + 1;
			}
		}
		else if (strstr(model, "models/weapons/v_knife_stiletto.mdl"))
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(12, 13);
			default:
				return sequence;
			}
		}
		else if (strstr(model, "models/weapons/v_knife_widowmaker.mdl"))
		{
			switch (sequence)
			{
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(14, 15);
			default:
				return sequence;
			}
		}
		else {
			return sequence;
		}
	}

	void DoSequenceRemapping(CRecvProxyData* data, C_BaseViewModel* entity)
	{
		const auto local = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));

		if (!local)
			return;

		if (!local->IsAlive())
			return;

		const auto owner = (C_BasePlayer*)g_EntityList->GetClientEntityFromHandle(entity->m_hOwner());

		if (owner != local)
			return;
		if (entity && entity->m_hOwner() && entity->m_hOwner().IsValid()) {
			const auto view_model_weapon_handle = entity->m_hWeapon();

			if (!view_model_weapon_handle.IsValid())
				return;
			const auto view_model_weapon = static_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntityFromHandle(view_model_weapon_handle));

			if (!view_model_weapon)
				return;
			
			if (k_weapon_info.count(view_model_weapon->m_Item().m_iItemDefinitionIndex()))
			{
				int chosenknife = g_Configurations.misc_knifemodel - 1;

				if (g_Configurations.misc_knifemodel == 0)
					return;

				const auto override_model = knives[chosenknife].model_name;

				auto& sequence = data->m_Value.m_Int;
				sequence = GetNewAnimation(override_model, sequence);
			}
		}
	}
}