#include "sequence.hpp"
#include "item_definitions.hpp"

namespace Sequence::general
{
	struct hud_weapons_t {
		std::int32_t* GetWeaponCount() {
			return reinterpret_cast<std::int32_t*>(std::uintptr_t(this) + 0x80);
		}
	};

	void FullUpdate()
	{
		if (!g_EngineClient->IsInGame())
			return;

		static auto clear_hud_weapon_icon_fn = reinterpret_cast<std::int32_t(__thiscall*)(void*, std::int32_t)>(
			Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 51 53 56 8B 75 08 8B D9 57 6B"));

		auto hud_weapons = Utils::FindHudElement<hud_weapons_t>(("CCSGO_HudWeaponSelection")) - 0xa0;

		if (hud_weapons == nullptr)
			return;

		for (std::size_t i = 0; i < *hud_weapons->GetWeaponCount(); i++)
			i = clear_hud_weapon_icon_fn(hud_weapons, i);

		g_ClientState->ForceFullUpdate();
	}
}

namespace Sequence
{
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

		if (!local || !local->IsAlive())
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
			
			if (k_WeaponInfo.count(view_model_weapon->m_Item().m_iItemDefinitionIndex()))
			{
				const auto override_model = k_WeaponInfo.at(view_model_weapon->m_Item().m_iItemDefinitionIndex()).model;

				auto& sequence = data->m_Value.m_Int;
				sequence = GetNewAnimation(override_model, sequence);
			}
		}
	}
}