#include <map>
#include "events.hpp"
#include "../sdk/csgostructs.hpp"
#include "../functions/knifechanger.hpp"

void Events::BEvent(IGameEvent* event)
{
	if (strstr(event->GetName(), "bomb_planted"))
	{
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
}
