#include <map>
#include "events.hpp"
#include "../sdk/csgostructs.hpp"
#include "../functions/knifechanger.hpp"

void Events::BEvent(IGameEvent* event)
{
	if (strstr(event->GetName(), "bomb_planted"))
	{
		Planting = false;

		int userid = event->GetInt("userid");

		if (!userid)
			return;

		int userid_id = g_EngineClient->GetPlayerForUserID(userid);

		player_info_t userid_info;
		if (!g_EngineClient->GetPlayerInfo(userid_id, &userid_info))
			return;

		auto m_player = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(userid_id));

		if (!m_player)
			return;

	}

	if (strstr(event->GetName(), "bomb_begindefuse"))
	{
		auto userid = event->GetInt("userid");

		if (!userid)
			return;

		auto userid_id = g_EngineClient->GetPlayerForUserID(userid);

		player_info_t userid_info;
		if (!g_EngineClient->GetPlayerInfo(userid_id, &userid_info))
			return;

		auto m_player = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(userid_id));

		if (!m_player)
			return;

		bomb_defusing_with_kits = event->GetBool("haskit");
	}

	if (strstr(event->GetName(), "bomb_beginplant"))
	{
		auto userid = event->GetInt("userid");

		if (!userid)
			return;
		auto userid_id = g_EngineClient->GetPlayerForUserID(userid);
		player_info_t userid_info;

		if (!g_EngineClient->GetPlayerInfo(userid_id, &userid_info))
			return;
		int SiteIndex = event->GetInt("site");
		IClientEntity* SiteEntity = g_EntityList->GetClientEntity(SiteIndex);
		auto collideable = SiteEntity->GetCollideable();
		Vector SiteCenter = (collideable->OBBMins() + collideable->OBBMaxs()) / 2.f;
		Vector SiteA = (*g_PlayerResource)->m_bombsiteCenterA();
		Vector SiteB = (*g_PlayerResource)->m_bombsiteCenterB();
		// true for A, false for B
		PlantingSite = SiteCenter.DistTo(SiteA) < SiteCenter.DistTo(SiteB);
		// tell our esp that it has to render something, set to false on abort plant/bomb planted/end round/etc events
		Planting = true;
		//for plant time
		PlantTime = g_GlobalVars->curtime + 3.1f;

		auto g_ChatElement = Utils::FindHudElement<C_BaseHudChat>("CHudChat");
		g_ChatElement->ChatPrintf(0, 0, std::string("").
			append(" \x0A"). //Light Blue
			append(userid_info.szName).
			append("\x01"). //Default Color
			append(" : Planting at Bombsite ").
			append((PlantingSite ? std::string(" \x02").append("A") : std::string(" \x02").append("B"))).c_str());
	}

	if (strstr(event->GetName(), "bomb_abortplant") || strstr(event->GetName(), "round_end") || strstr(event->GetName(), "round_start"))
	{
		Planting = false;
	}
}
