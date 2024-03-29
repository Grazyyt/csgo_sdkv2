#include "chams.hpp"

#include <fstream>

#include "../sdk/csgostructs.hpp"
#include "../configurations.hpp"

#include "../hooks.hpp"

#include "../sdk/utils/input.hpp"


Chams::Chams() 
{
	materialRegular = g_MatSystem->FindMaterial("debug/debugambientcube");
	materialFlat = g_MatSystem->FindMaterial("debug/debugdrawflat");
}

Chams::~Chams() 
{

}

void Chams::OverrideMaterial(bool ignoreZ, bool flat, bool wireframe, bool glass, const Color& rgba) 
{
	IMaterial* material = nullptr;

	if (flat) 
		material = materialFlat;
	else 
		material = materialRegular;

	material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignoreZ);

	if (glass) 
	{
		material = materialFlat;
		material->AlphaModulate(0.45f);
	}
	else 
		material->AlphaModulate(rgba.a() / 255.0f);

	material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, wireframe);
	material->ColorModulate(
		rgba.r() / 255.0f,
		rgba.g() / 255.0f,
		rgba.b() / 255.0f);

	g_MdlRender->ForcedMaterialOverride(material);
}

void Chams::OnDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix)
{
	static auto fnDME = Hooks::mdlrender_hook.get_original<decltype(&Hooks::hkDrawModelExecute)>(index::DrawModelExecute);

	const auto mdl = info.pModel;

	bool is_arm = strstr(mdl->szName, "arms") != nullptr;
	bool is_player = strstr(mdl->szName, "models/player") != nullptr;
	bool is_sleeve = strstr(mdl->szName, "sleeve") != nullptr;
	bool is_weapon = strstr(mdl->szName, "weapons/v_")  != nullptr;

	if (is_player && g_Configurations.chams_player_enabled) 
	{
		// 
		// Draw player chams.
		// 
		auto ent = C_BasePlayer::GetPlayerByIndex(info.entity_index);

		if (ent && g_LocalPlayer && ent->IsAlive()) 
		{
			const auto enemy = ent->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();
			if (!enemy && g_Configurations.chams_player_enemies_only)
				return;

			const auto clr_front = enemy ? (Color)g_Configurations.color_chams_player_enemy_visible : (Color)g_Configurations.color_chams_player_ally_visible;
			const auto clr_back = enemy ? (Color)g_Configurations.color_chams_player_enemy_occluded : (Color)g_Configurations.color_chams_player_ally_occluded;

			if (g_Configurations.chams_player_ignorez) 
			{
				OverrideMaterial(true, g_Configurations.chams_player_flat, g_Configurations.chams_player_wireframe, false, clr_back);
				fnDME(g_MdlRender, 0, ctx, state, info, matrix);
				OverrideMaterial( false, g_Configurations.chams_player_flat, g_Configurations.chams_player_wireframe, false, clr_front);
			}
			else 
			{
				OverrideMaterial(false, g_Configurations.chams_player_flat, g_Configurations.chams_player_wireframe, g_Configurations.chams_player_glass, clr_front);
			}
		}
	}
	else if (is_sleeve && g_Configurations.chams_sleeve_enabled)
	{
		auto material = g_MatSystem->FindMaterial(mdl->szName, TEXTURE_GROUP_MODEL);
		if (!material)
			return;
		// 
		// Draw sleeve chams.
		//
		if (g_Configurations.chams_sleeve_ignorez)
		{
			OverrideMaterial(true, g_Configurations.chams_sleeve_flat, g_Configurations.chams_sleeve_wireframe, false, (Color)g_Configurations.color_chams_sleeve_occluded);
			fnDME(g_MdlRender, 0, ctx, state, info, matrix);
			OverrideMaterial(false, g_Configurations.chams_sleeve_flat, g_Configurations.chams_sleeve_wireframe, false, (Color)g_Configurations.color_chams_sleeve_visible);
		}
		else
		{
			OverrideMaterial(false, g_Configurations.chams_sleeve_flat, g_Configurations.chams_sleeve_wireframe, g_Configurations.chams_sleeve_glass,
				(Color)g_Configurations.color_chams_sleeve_visible);
		}
	}
	else if (is_arm && g_Configurations.chams_arms_enabled && !g_Configurations.misc_no_hands)
	{
		auto material = g_MatSystem->FindMaterial(mdl->szName, TEXTURE_GROUP_MODEL);
		if (!material)
			return;

		if (g_Configurations.chams_arms_ignorez)
		{
			OverrideMaterial(true, g_Configurations.chams_arms_flat, g_Configurations.chams_arms_wireframe, false, (Color)g_Configurations.color_chams_arms_occluded);
			fnDME(g_MdlRender, 0, ctx, state, info, matrix);
			OverrideMaterial(false, g_Configurations.chams_arms_flat, g_Configurations.chams_arms_wireframe, false, (Color)g_Configurations.color_chams_arms_visible);
		}
		else
		{
			OverrideMaterial(false, g_Configurations.chams_arms_flat, g_Configurations.chams_arms_wireframe, g_Configurations.chams_arms_glass,
				(Color)g_Configurations.color_chams_arms_visible);
		}
	}
	else if (is_arm && g_Configurations.misc_no_hands)
	{
		auto material = g_MatSystem->FindMaterial(mdl->szName, TEXTURE_GROUP_MODEL);
		if (!material)
			return;
		// 
		// No hands.
		// 
		material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
		g_MdlRender->ForcedMaterialOverride(material);
	}
	else if (is_weapon && g_Configurations.chams_weapon_enabled && !is_arm)
	{
		auto material = g_MatSystem->FindMaterial(mdl->szName, TEXTURE_GROUP_MODEL);
		if (!material)
			return;
		// 
		// Draw weapon chams.
		//
		if (g_Configurations.chams_weapon_ignorez)
		{
			OverrideMaterial(true, g_Configurations.chams_weapon_flat, g_Configurations.chams_weapon_wireframe, false, (Color)g_Configurations.color_chams_weapon_occluded);
			fnDME(g_MdlRender, 0, ctx, state, info, matrix);
			OverrideMaterial(false, g_Configurations.chams_weapon_flat, g_Configurations.chams_weapon_wireframe, false, (Color)g_Configurations.color_chams_weapon_visible);
		}
		else
		{
			OverrideMaterial(false, g_Configurations.chams_weapon_flat, g_Configurations.chams_weapon_wireframe, g_Configurations.chams_weapon_glass,
				(Color)g_Configurations.color_chams_weapon_visible);
		}
	}
}