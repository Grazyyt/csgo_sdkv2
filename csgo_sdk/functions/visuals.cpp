#include <algorithm>

#include "visuals.hpp"
#include "grenade_pred.hpp"
#include "aimbot.hpp"

#include "../configurations.hpp"
#include "../sdk/utils/math.hpp"
#include "../sdk/utils/utils.hpp"
#include "../helpers/events.hpp"

#include "../imgui/imgui.h"

RECT GetBBox(C_BaseEntity* ent)
{
	RECT rect{};
	auto collideable = ent->GetCollideable();

	if (!collideable)
		return rect;

	auto min = collideable->OBBMins();
	auto max = collideable->OBBMaxs();

	const matrix3x4_t& trans = ent->m_rgflCoordinateFrame();

	Vector points[] = 
	{
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector pointsTransformed[8];
	for (int i = 0; i < 8; i++) 
	{
		Math::VectorTransform(points[i], trans, pointsTransformed[i]);
	}

	Vector screen_points[8] = {};

	for (int i = 0; i < 8; i++) 
	{
		if (!Math::WorldToScreen(pointsTransformed[i], screen_points[i]))
			return rect;
	}

	auto left = screen_points[0].x;
	auto top = screen_points[0].y;
	auto right = screen_points[0].x;
	auto bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++) 
	{
		if (left > screen_points[i].x)
			left = screen_points[i].x;
		if (top < screen_points[i].y)
			top = screen_points[i].y;
		if (right < screen_points[i].x)
			right = screen_points[i].x;
		if (bottom > screen_points[i].y)
			bottom = screen_points[i].y;
	}

	return RECT{ (long)left, (long)top, (long)right, (long)bottom };
}

std::map< int, std::string > m_weapon_icons =
{
		{ ItemDefinitionIndex::WEAPON_DEAGLE, "A" },
		{ ItemDefinitionIndex::WEAPON_ELITE, "B" },
		{ ItemDefinitionIndex::WEAPON_FIVESEVEN, "C" },
		{ ItemDefinitionIndex::WEAPON_GLOCK, "D" },
		{ ItemDefinitionIndex::WEAPON_AK47, "W" },
		{ ItemDefinitionIndex::WEAPON_AUG, "U" },
		{ ItemDefinitionIndex::WEAPON_AWP, "Z" },
		{ ItemDefinitionIndex::WEAPON_FAMAS, "R" },
		{ ItemDefinitionIndex::WEAPON_G3SG1, "X" },
		{ ItemDefinitionIndex::WEAPON_GALILAR, "Q" },
		{ ItemDefinitionIndex::WEAPON_M249, "g" },
		{ ItemDefinitionIndex::WEAPON_M4A1, "S" },
		{ ItemDefinitionIndex::WEAPON_MAC10, "K" },
		{ ItemDefinitionIndex::WEAPON_P90, "P" },
		{ ItemDefinitionIndex::WEAPON_MP5, "N" },
		{ ItemDefinitionIndex::WEAPON_UMP45, "L" },
		{ ItemDefinitionIndex::WEAPON_XM1014, "b" },
		{ ItemDefinitionIndex::WEAPON_BIZON, "M" },
		{ ItemDefinitionIndex::WEAPON_MAG7, "d" },
		{ ItemDefinitionIndex::WEAPON_NEGEV, "f" },
		{ ItemDefinitionIndex::WEAPON_SAWEDOFF, "c" },
		{ ItemDefinitionIndex::WEAPON_TEC9, "H" },
		{ ItemDefinitionIndex::WEAPON_TASER, "h" },
		{ ItemDefinitionIndex::WEAPON_HKP2000, "E" },
		{ ItemDefinitionIndex::WEAPON_MP7, "E" },
		{ ItemDefinitionIndex::WEAPON_MP9, "O" },
		{ ItemDefinitionIndex::WEAPON_NOVA, "e" },
		{ ItemDefinitionIndex::WEAPON_P250, "F" },
		{ ItemDefinitionIndex::WEAPON_SCAR20, "Y" },
		{ ItemDefinitionIndex::WEAPON_SG556, "V" },
		{ ItemDefinitionIndex::WEAPON_SSG08, "a" },
		{ ItemDefinitionIndex::WEAPON_KNIFE, "]" },
		{ ItemDefinitionIndex::WEAPON_FLASHBANG, "i" },
		{ ItemDefinitionIndex::WEAPON_HEGRENADE, "j" },
		{ ItemDefinitionIndex::WEAPON_SMOKEGRENADE, "k" },
		{ ItemDefinitionIndex::WEAPON_MOLOTOV, "l" },
		{ ItemDefinitionIndex::WEAPON_DECOY, "m" },
		{ ItemDefinitionIndex::WEAPON_INCGRENADE, "n" },
		{ ItemDefinitionIndex::WEAPON_C4, "o" },
		{ ItemDefinitionIndex::WEAPON_KNIFE_T, "[" },
		{ ItemDefinitionIndex::WEAPON_M4A1_SILENCER, "T" },
		{ ItemDefinitionIndex::WEAPON_USP_SILENCER, "G" },
		{ ItemDefinitionIndex::WEAPON_CZ75A, "I" },
		{ ItemDefinitionIndex::WEAPON_REVOLVER, "J" },
		{ ItemDefinitionIndex::WEAPON_KNIFE_BAYONET, "1" },
		{ ItemDefinitionIndex::WEAPON_KNIFE_FLIP, "2" },
		{ ItemDefinitionIndex::WEAPON_KNIFE_GUT, "3" },
		{ ItemDefinitionIndex::WEAPON_KNIFE_KARAMBIT, "4" },
		{ ItemDefinitionIndex::WEAPON_KNIFE_M9_BAYONET, "5" },
		{ ItemDefinitionIndex::WEAPON_KNIFE_TACTICAL, "6" },
		{ ItemDefinitionIndex::WEAPON_KNIFE_FALCHION, "0" },
		{ ItemDefinitionIndex::WEAPON_KNIFE_SURVIVAL_BOWIE, "7" },
		{ ItemDefinitionIndex::WEAPON_KNIFE_BUTTERFLY, "8" },
		{ ItemDefinitionIndex::WEAPON_KNIFE_PUSH, "9" },
		{ ItemDefinitionIndex::WEAPON_KNIFE_CORD, "[" },
		{ ItemDefinitionIndex::WEAPON_KNIFE_CANIS, "[" },
		{ ItemDefinitionIndex::WEAPON_KNIFE_URSUS, "[" },
		{ ItemDefinitionIndex::WEAPON_KNIFE_GYPSY_JACKKNIFE, "[" },
		{ ItemDefinitionIndex::WEAPON_KNIFE_OUTDOOR, "[" },
		{ ItemDefinitionIndex::WEAPON_KNIFE_STILETTO, "[" },
		{ ItemDefinitionIndex::WEAPON_KNIFE_WIDOWMAKER, "[" },
		{ ItemDefinitionIndex::WEAPON_KNIFE_SKELETON, "[" },
		{ ItemDefinitionIndex::WEAPON_HEALTHSHOT, "u" },
		{ ItemDefinitionIndex::WEAPON_SHIELD, "p" },
		{ ItemDefinitionIndex::WEAPON_TAGRENADE, "i" },
		{ ItemDefinitionIndex::WEAPON_BREACHCHARGE, "o" }
};

Visuals::Visuals()
{
	InitializeCriticalSection(&cs);
}

Visuals::~Visuals() 
{
	DeleteCriticalSection(&cs);
}

void Visuals::Render() 
{

}

bool Visuals::Player::Begin(C_BasePlayer* pl)
{
	if (pl->IsDormant() || !pl->IsAlive())
		return false;

	ctx.pl = pl;
	ctx.is_enemy = g_LocalPlayer->m_iTeamNum() != pl->m_iTeamNum();
	ctx.is_visible = g_LocalPlayer->CanSeePlayer(pl, HITBOX_CHEST);

	if (!ctx.is_enemy && g_Configurations.esp_enemies_only)
		return false;

	ctx.clr = ctx.is_enemy ? (ctx.is_visible ? (Color)g_Configurations.color_esp_enemy_visible 
		: (Color)g_Configurations.color_esp_enemy_occluded) : (ctx.is_visible ? (Color)g_Configurations.color_esp_ally_visible 
			: (Color)g_Configurations.color_esp_ally_occluded);

	auto head = pl->GetHitboxPos(HITBOX_HEAD);
	auto origin = pl->m_vecOrigin();

	head.z += 15;

	if (!Math::WorldToScreen(head, ctx.head_pos) || !Math::WorldToScreen(origin, ctx.feet_pos))
		return false;

	auto h = fabs(ctx.head_pos.y - ctx.feet_pos.y);
	auto w = h / 1.65f;

	ctx.bbox.left = static_cast<long>(ctx.feet_pos.x - w * 0.5f);
	ctx.bbox.right = static_cast<long>(ctx.bbox.left + w);
	ctx.bbox.bottom = static_cast<long>(ctx.feet_pos.y);
	ctx.bbox.top = static_cast<long>(ctx.head_pos.y);

	return true;
}

void Visuals::Player::RenderBox() 
{
	Render::Get().RenderBoxByType(ctx.bbox.left, ctx.bbox.top, ctx.bbox.right, ctx.bbox.bottom, ctx.clr, 1);
	Render::Get().RenderBoxByType(ctx.bbox.left + 1, ctx.bbox.top + 1, ctx.bbox.right - 1, ctx.bbox.bottom - 1, Color::Black, 1);
	Render::Get().RenderBoxByType(ctx.bbox.left - 1, ctx.bbox.top - 1, ctx.bbox.right + 1, ctx.bbox.bottom + 1, Color::Black, 1);
}

void Visuals::Player::RenderName()
{
	player_info_t info = ctx.pl->GetPlayerInfo();

	auto sz = g_pDefaultFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, info.szName);

	Render::Get().RenderText(info.szName, ctx.feet_pos.x - sz.x / 2, ctx.head_pos.y - sz.y, 14.f,  ctx.clr);
}

void Visuals::Player::RenderHealth()
{
	auto  hp = ctx.pl->m_iHealth();
	float box_h = (float)fabs(ctx.bbox.bottom - ctx.bbox.top);
	float off = 8;

	int height = (box_h * hp) / 100;

	int green = int(hp * 2.55f);
	int red = 255 - green;

	int x = ctx.bbox.left - off;
	int y = ctx.bbox.top;
	int w = 4;
	int h = box_h;

	Render::Get().RenderBox(x, y, x + w, y + h, Color::Black, 1.f, true);
	Render::Get().RenderBox(x + 1, y + 1, x + w - 1, y + height - 2, Color(red, green, 0, 255), 1.f, true);
}

void Visuals::Player::RenderArmour()
{
	auto  armour = ctx.pl->m_ArmorValue();
	float box_h = (float)fabs(ctx.bbox.bottom - ctx.bbox.top);
	float off = 4;

	int height = (((box_h * armour) / 100));

	int x = ctx.bbox.right + off;
	int y = ctx.bbox.top;
	int w = 4;
	int h = box_h;

	Render::Get().RenderBox(x, y, x + w, y + h, Color::Black, 1.f, true);
	Render::Get().RenderBox(x + 1, y + 1, x + w - 1, y + height - 2, Color(0, 50, 255, 255), 1.f, true);
}

void Visuals::Player::RenderWeaponName()
{
	if (g_Configurations.esp_player_weapons_type == 0)
		return;

	auto weapon = ctx.pl->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	
	if (g_Configurations.esp_player_weapons_type == 1)
	Render::Get().RenderText(weapon->get_name().c_str(), ctx.feet_pos.x, ctx.feet_pos.y, 14.f, ctx.clr, true, g_pDefaultFont);
	else
	Render::Get().RenderText(m_weapon_icons[weapon->GetItemDefinitionIndex()], ctx.feet_pos.x, ctx.feet_pos.y, 14.f, ctx.clr, true, true, g_IconEsp);
}

void Visuals::Player::RenderSnapline()
{

	int screen_w, screen_h;
	g_EngineClient->GetScreenSize(screen_w, screen_h);

	Render::Get().RenderLine(screen_w / 2.f, (float)screen_h,
		ctx.feet_pos.x, ctx.feet_pos.y, ctx.clr);
}

void Visuals::RenderCrosshair()
{
	int w, h;

	g_EngineClient->GetScreenSize(w, h);

	int cx = w / 2;
	int cy = h / 2;
	Render::Get().RenderLine(cx - 25, cy, cx + 25, cy, (Color)g_Configurations.color_esp_crosshair);
	Render::Get().RenderLine(cx, cy - 25, cx, cy + 25, (Color)g_Configurations.color_esp_crosshair);
}

void Visuals::RenderWeapon(C_BaseCombatWeapon* ent)
{
	auto clean_item_name = [](const char* name) -> const char* 
	{
		if (name[0] == 'C')
			name++;

		auto start = strstr(name, "Weapon");
		if (start != nullptr)
			name = start + 6;

		return name;
	};

	if (ent->m_hOwnerEntity().IsValid())
		return;

	auto bbox = GetBBox(ent);

	if (bbox.right == 0 || bbox.bottom == 0)
		return;

	Render::Get().RenderBox(bbox, (Color)g_Configurations.color_esp_weapons);

	auto name = clean_item_name(ent->GetClientClass()->m_pNetworkName);

	auto sz = g_pDefaultFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, name);
	int w = bbox.right - bbox.left;

	if (g_Configurations.esp_dropped_weapons_type == 0)
		return;
	else if (g_Configurations.esp_dropped_weapons_type == 1)
	Render::Get().RenderText(name, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 14.f, (Color)g_Configurations.color_esp_weapons);
	else
	Render::Get().RenderText(m_weapon_icons[ent->GetItemDefinitionIndex()], ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 14.f, (Color)g_Configurations.color_esp_weapons, true, true, g_IconEsp);
}

void Visuals::RenderDefuseKit(C_BaseEntity* ent)
{
	if (ent->m_hOwnerEntity().IsValid())
		return;

	auto bbox = GetBBox(ent);

	if (bbox.right == 0 || bbox.bottom == 0)
		return;

	Render::Get().RenderBox(bbox, (Color)g_Configurations.color_esp_defuse);

	auto name = "Defuse Kit";
	auto sz = g_pDefaultFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, name);
	int w = bbox.right - bbox.left;
	Render::Get().RenderText(name, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 14.f, (Color)g_Configurations.color_esp_defuse);
}

void Visuals::RenderPlantedC4(C_BaseEntity* ent)
{
	auto bbox = GetBBox(ent);

	if (bbox.right == 0 || bbox.bottom == 0)
		return;

	Render::Get().RenderBox(bbox, (Color)g_Configurations.color_esp_c4);

	int bombTimer = std::ceil(ent->m_flC4Blow() - g_GlobalVars->curtime);
	std::string timer = std::to_string(bombTimer);

	auto name = (bombTimer < 0.f) ? "Bomb" : timer;
	auto sz = g_pDefaultFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, name.c_str());
	int w = bbox.right - bbox.left;

	Render::Get().RenderText(name, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 14.f, (Color)g_Configurations.color_esp_c4);
}

float GetDefuseTime(C_PlantedC4* bomb)
{
	static float defuse_time = -1;

	if (!bomb)
		return 0;

	if (!bomb->m_hBombDefuser())
		defuse_time = -1;

	else if (defuse_time == -1)
		defuse_time = g_GlobalVars->curtime + bomb->m_flDefuseLength();

	if (defuse_time > -1 && bomb->m_hBombDefuser())
		return defuse_time - g_GlobalVars->curtime;

	return 0;
}

C_BasePlayer* GetBombPlayer()
{
	for (auto i = 1; i <= g_EntityList->GetHighestEntityIndex(); ++i)
	{
		auto* entity = C_BasePlayer::GetPlayerByIndex(i);
		if (!entity || entity->IsPlayer() || entity->IsDormant() || entity == g_LocalPlayer)
			continue;

		if (entity->IsPlantedC4())
			return entity;
	}

	return nullptr;
}

C_PlantedC4* GetBomb()
{
	C_BaseEntity* entity;
	for (auto i = 1; i <= g_EntityList->GetMaxEntities(); ++i)
	{
		entity = C_BaseEntity::GetEntityByIndex(i);
		if (entity && !entity->IsDormant() && entity->IsPlantedC4())
			return reinterpret_cast<C_PlantedC4*>(entity);
	}

	return nullptr;
}

void Visuals::RenderC4Window(C_BaseEntity* ent)
{
	if (!ent)
		return;

	float bombTimer = ent->m_flC4Blow() - (g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick);

	int x;
	int y;

	g_EngineClient->GetScreenSize(x, y);

	int windowX = 0;
	int windowY = y - 430;

	int windowSizeX = 120;
	static int windowSizeY = 0;
	const auto bomb = GetBomb();



	if (!bomb)
		return;

	if (bombTimer < 0)
		return;

	if (bomb->m_bBombDefused())
		return;

	int site = ent->m_nBombSite();

	int w, h;

	g_EngineClient->GetScreenSize(w, h);

	std::string namemap = g_EngineClient->GetLevelNameShort();

	std::string ssffee;

	if (!site)
		ssffee = "A";
	else
		ssffee = "B";
	

	Color yaint = Color(255, 100, 100, 255);
	Color yaint2 = Color(255, 100, 100, 255);
	float fldefuse = ent->m_flDefuseCountDown();
	float DefuseTimeRemaining = fldefuse - (g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick);

	if (bombTimer > 10)
	{
		yaint = Color(0, 255, 0, 255);
	}
	else if (bombTimer > 5)
	{
		yaint = Color(255, 255, 0, 255);
	}
	else if (bombTimer > 0)
	{
		yaint = Color(255, 0, 0, 255);
	}

	if (bombTimer >= DefuseTimeRemaining)
	{
		yaint2 = Color(0, 255, 255, 255);
	}
	else if (bombTimer > 0)
	{
		yaint2 = Color(255, 0, 0, 255);
	}


	Render::Get().RenderText(ssffee.c_str(), w / w + 10, h / h + 10, 40, g_LocalPlayer->m_iTeamNum() == 3 ? Color(255, 50, 50, 255) : Color(50, 255, 50, 255));

	Render::Get().RenderBoxFilled(windowX, windowY, windowX + windowSizeX, windowY + windowSizeY, Color(0, 0, 0, 100));
	Render::Get().RenderBoxFilled(windowX, windowY, windowX + 2, windowY + windowSizeY, Color(0, 0, 200, 100));

	Render::Get().RenderText("bomb: ", ImVec2(windowX + 4, windowY + 2), 15, Color(255, 255, 255, 255), false, false, g_SmallFont);	
	ImVec2 textSize_bomb = g_SmallFont->CalcTextSizeA(15, FLT_MAX, 0.0f, "bomb: ");	
	std::string bomb_timer_text;

	char buff[228];

	snprintf(buff, sizeof(buff), "%4.3f", bombTimer);

	bomb_timer_text = buff;

	float time = GetDefuseTime(bomb);

	Render::Get().RenderText(bombTimer >= 0 ? bomb_timer_text : "0", ImVec2(windowX + 4 + textSize_bomb.x, windowY + 2), 15, yaint, false, false, g_SmallFont);	

	ImVec2 textSize_defuse = g_SmallFont->CalcTextSizeA(15, FLT_MAX, 0.0f, "defuse: ");	


	std::string defuse_timer_text;
	char buff_2[228];

	snprintf(buff_2, sizeof(buff_2), "%4.3f", time);

	defuse_timer_text = buff_2;

	const auto bomb_1 = GetBombPlayer();

	if (!bomb_1)
		return;

	float flArmor = g_LocalPlayer->m_ArmorValue();
	float flHealth = g_LocalPlayer->m_iHealth();
	float flDistance = g_LocalPlayer->GetEyePos().DistTo(bomb_1->GetAbsOrigin());

	float a = 450.7f;
	float b = 75.68f;
	float c = 789.2f;
	float d = ((flDistance - b) / c);
	float flDamage = a * exp(-d * d);

	float flDmg = flDamage;

	if (flArmor > 0)
	{
		float flNew = flDmg * 0.5f;
		float flArmor = (flDmg - flNew) * 0.5f;

		if (flArmor > static_cast<float>(flArmor))
		{
			flArmor = static_cast<float>(flArmor) * (1.f / 0.5f);
			flNew = flDmg - flArmor;
		}

		flDamage = flNew;
	}

	std::string damage;
	char buff_3[228];

	snprintf(buff_3, sizeof(buff_3), "%i", (int)flDamage);

	damage = buff_3;

	ImVec2 textSize_damage = g_SmallFont->CalcTextSizeA(15, FLT_MAX, 0.0f, "damage: ");	

	if (GetDefuseTime(bomb) > 0)
	{
		Render::Get().RenderText("defuse: ", ImVec2(windowX + 4, windowY + 12), 15, Color(255, 255, 255, 255), false, false, g_SmallFont);	
		if (bombTimer < time - 0.15)
		{
			Render::Get().RenderText(defuse_timer_text, ImVec2(windowX + 4 + textSize_defuse.x, windowY + 12), 15, Color(255, 50, 50, 255), false, false, g_SmallFont);	
		}
		else if (bombTimer >= time - 0.15)
		{
			Render::Get().RenderText(defuse_timer_text, ImVec2(windowX + 4 + textSize_defuse.x, windowY + 12), 15, Color(50, 255, 50, 255), false, false, g_SmallFont);	
		}
		if (flDamage > 1 && bombTimer >= 0)
		{
			Render::Get().RenderText("damage: ", ImVec2(windowX + 4, windowY + 22), 15, Color(255, 255, 255, 255), false, 0, g_SmallFont); 	
			Render::Get().RenderText(flDamage < flHealth ? damage : "you dead", ImVec2(windowX + 4 + textSize_damage.x, windowY + 22), 15, Color(255, 50, 50, 255), false, false, g_SmallFont);
			windowSizeY = 37;
		}
		else
		{
			windowSizeY = 25;
		}

		float box_w = (float)fabs(0 - windowSizeX);

		float max_time;

		float width;

		if (g_Events.bomb_defusing_with_kits)
		{
			width = (((box_w * time) / 5.f));
		}
		else
		{
			width = (((box_w * time) / 10.f));
		}

		Render::Get().RenderBoxFilled(windowX, windowY + windowSizeY + 5, windowX + (int)width, windowY + windowSizeY + 10, yaint2);

	}
	else if (GetDefuseTime(bomb) <= 0)
	{
		if (flDamage > 1 && bombTimer >= 0)
		{
			Render::Get().RenderText("damage: ", ImVec2(windowX + 4, windowY + 12), 15, Color(255, 255, 255, 255), false, false, g_SmallFont); 	
			Render::Get().RenderText(flDamage < flHealth ? damage : "you dead", ImVec2(windowX + 4 + textSize_damage.x, windowY + 12), 15, Color(255, 50, 50, 255), false, false, g_SmallFont);
			windowSizeY = 27;
		}
		else
		{
			windowSizeY = 17;
		}
	}

	float box_w = (float)fabs(0 - windowSizeX);

	auto width = (((box_w * bombTimer) / ent->m_flTimerLength()));
	Render::Get().RenderBoxFilled(windowX, windowY + windowSizeY, windowX + (int)width, windowY + windowSizeY + 5, yaint);

	Render::Get().RenderCircleFilled(w / 2, y / 2 - 400, 40, 40, Color(50, 50, 50, 100));
	Render::Get().RenderText("o", ImVec2(w / 2, y / 2 - 415), 30, Color(255, 255, 255, 200), true, true, g_IconEsp);

	C_BasePlayer* Defuser = (C_BasePlayer*)C_BasePlayer::get_entity_from_handle(ent->m_hBombDefuser());

	if (Defuser)
		Render::Get().CircularProgressBar(w / 2, y / 2 - 400, 37, 40, 0, 360 * (DefuseTimeRemaining / (Defuser->m_bHasDefuser() ? 5 : 10)), yaint2, true);
	else
		Render::Get().CircularProgressBar(w / 2, y / 2 - 400, 37, 40, 0, 360 * (bombTimer / ent->m_flTimerLength()), yaint, true);

}

void Visuals::RenderItemEsp(C_BaseEntity* ent)
{
	std::string itemstr = "Undefined";

	const model_t * itemModel = ent->GetModel();
	if (!itemModel)
		return;

	studiohdr_t * hdr = g_MdlInfo->GetStudiomodel(itemModel);
	if (!hdr)
		return;

	itemstr = hdr->szName;

	if (ent->GetClientClass()->m_ClassID == ClassId_CBumpMine)
		itemstr = "";
	else if (itemstr.find("case_pistol") != std::string::npos)
		itemstr = "Pistol Case";
	else if (itemstr.find("case_light_weapon") != std::string::npos)
		itemstr = "Light Case";
	else if (itemstr.find("case_heavy_weapon") != std::string::npos)
		itemstr = "Heavy Case";
	else if (itemstr.find("case_explosive") != std::string::npos)
		itemstr = "Explosive Case";
	else if (itemstr.find("case_tools") != std::string::npos)
		itemstr = "Tools Case";
	else if (itemstr.find("random") != std::string::npos)
		itemstr = "Airdrop";
	else if (itemstr.find("dz_armor_helmet") != std::string::npos)
		itemstr = "Full Armor";
	else if (itemstr.find("dz_helmet") != std::string::npos)
		itemstr = "Helmet";
	else if (itemstr.find("dz_armor") != std::string::npos)
		itemstr = "Armor";
	else if (itemstr.find("upgrade_tablet") != std::string::npos)
		itemstr = "Tablet Upgrade";
	else if (itemstr.find("briefcase") != std::string::npos)
		itemstr = "Briefcase";
	else if (itemstr.find("parachutepack") != std::string::npos)
		itemstr = "Parachute";
	else if (itemstr.find("dufflebag") != std::string::npos)
		itemstr = "Cash Dufflebag";
	else if (itemstr.find("ammobox") != std::string::npos)
		itemstr = "Ammobox";
	else if (itemstr.find("dronegun") != std::string::npos)
		itemstr = "Turrel";
	else if (itemstr.find("exojump") != std::string::npos)
		itemstr = "Exojump";
	else if (itemstr.find("healthshot") != std::string::npos)
		itemstr = "Healthshot";
	else 
		return;
	
	auto bbox = GetBBox(ent);
	if (bbox.right == 0 || bbox.bottom == 0)
		return;

	auto sz = g_pDefaultFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, itemstr.c_str());
	int w = bbox.right - bbox.left;

	Render::Get().RenderText(itemstr, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 14.f, (Color)g_Configurations.color_esp_item);
}

void Visuals::ThirdPerson() 
{
	if (!g_LocalPlayer)
		return;

	if (g_Configurations.misc_thirdperson && g_LocalPlayer->IsAlive())
	{
		if (!g_Input->m_fCameraInThirdPerson)
			g_Input->m_fCameraInThirdPerson = true;

		float dist = g_Configurations.misc_thirdperson_dist;

		QAngle *view = g_LocalPlayer->GetVAngles();
		trace_t tr;
		Ray_t ray;

		Vector desiredCamOffset = Vector(cos(DEG2RAD(view->yaw)) * dist, sin(DEG2RAD(view->yaw)) * dist, sin(DEG2RAD(-view->pitch)) * dist);

		ray.Init(g_LocalPlayer->GetEyePos(), (g_LocalPlayer->GetEyePos() - desiredCamOffset));
		CTraceFilter traceFilter;
		traceFilter.pSkip = g_LocalPlayer;
		g_EngineTrace->TraceRay(ray, MASK_SHOT, &traceFilter, &tr);

		Vector diff = g_LocalPlayer->GetEyePos() - tr.endpos;

		float distance2D = sqrt(abs(diff.x * diff.x) + abs(diff.y * diff.y)); 

		bool horOK = distance2D > (dist - 2.0f);
		bool vertOK = (abs(diff.z) - abs(desiredCamOffset.z) < 3.0f);

		float cameraDistance;

		if (horOK && vertOK)        
			cameraDistance = dist;          
		else
		{
			if (vertOK)       
				cameraDistance = distance2D * 0.95f;
			else            
				cameraDistance = abs(diff.z) * 0.95f;
		}
		g_Input->m_fCameraInThirdPerson = true;
		g_Input->m_vecCameraOffset.z = cameraDistance;
	}
	else
		g_Input->m_fCameraInThirdPerson = false;
}

void Visuals::DrawFOV()
{
	auto pWeapon = g_LocalPlayer->m_hActiveWeapon();
	if (!pWeapon) 
		return;

	auto settings = g_Configurations.legitbot_items[pWeapon->m_Item().m_iItemDefinitionIndex()];

	if (settings.enabled) {

		float fov = static_cast<float>(g_LocalPlayer->GetFOV());

		int w, h;
		g_EngineClient->GetScreenSize(w, h);

		Vector2D screenSize = Vector2D(w, h);
		Vector2D center = screenSize * 0.5f;

		float ratio = screenSize.x / screenSize.y;
		float screenFov = atanf((ratio) * (0.75f) * tan(DEG2RAD(fov * 0.5f)));

		float radiusFOV = tanf(DEG2RAD(LegitBot::Get().GetFov())) / tanf(screenFov) * center.x;

		Render::Get().RenderCircle(center.x, center.y, radiusFOV, 32, Color(0, 0, 0, 100));

		if (settings.silent) {
			float silentRadiusFOV = tanf(DEG2RAD(settings.silent_fov)) / tanf(screenFov) * center.x;
			Render::Get().RenderCircle(center.x, center.y, silentRadiusFOV, 32, Color(255, 25, 10, 100));
		}
	}
}

void Visuals::Nightmode() {

	if (!g_EngineClient->IsInGame() && !g_EngineClient->IsConnected())
		return;

	static ConVar* ThreeD = g_CVar->FindVar("r_3dsky");
	static ConVar* r_DrawSpecificStaticProp = g_CVar->FindVar("r_DrawSpecificStaticProp");
	r_DrawSpecificStaticProp->SetValue(1);
	ThreeD->SetValue(0);
	auto world_color = g_Configurations.nightmode_color_world;
	auto prop_color = g_Configurations.nightmode_color_prop;
	auto sky_color = g_Configurations.nightmode_color_sky;
	bool enabled = g_Configurations.misc_nightmode;

	for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
	{
		IMaterial* pMaterial = g_MatSystem->GetMaterial(i);

		if (!pMaterial)
			continue;

		if (!pMaterial->IsPrecached())
			continue;

		if (strstr(pMaterial->GetTextureGroupName(), "World") && enabled && !g_Unload)
		{
			pMaterial->ColorModulate(world_color[0], world_color[1], world_color[2]);
			pMaterial->AlphaModulate(world_color[3]);
		}
		else if (strstr(pMaterial->GetTextureGroupName(), "World") && !enabled || strstr(pMaterial->GetTextureGroupName(), "StaticProp") && !enabled ||  strstr(pMaterial->GetTextureGroupName(), "SkyBox") && !enabled || g_Unload)
		{
			pMaterial->ColorModulate(1.f, 1.f, 1.f);
			pMaterial->AlphaModulate(1.f);
		}

		if (strstr(pMaterial->GetTextureGroupName(), "StaticProp") && enabled && !g_Unload)
		{
			pMaterial->ColorModulate(prop_color[0], prop_color[1], prop_color[2]);
			pMaterial->AlphaModulate(prop_color[3]);
		}

		if (strstr(pMaterial->GetTextureGroupName(), "SkyBox") && enabled && !g_Unload)
		{
			pMaterial->ColorModulate(sky_color[0], sky_color[1], sky_color[2]);
			pMaterial->AlphaModulate(sky_color[3]);
		}
	}
}

void Visuals::AddToDrawList() 
{
	if (g_Configurations.esp_grenade_prediction)
		Grenade_Pred::Get().DrawPrediction();

	for (auto i = 1; i <= g_EntityList->GetHighestEntityIndex(); ++i) 
	{
		auto entity = C_BaseEntity::GetEntityByIndex(i);

		if (!entity)
			continue;
		
		if (entity == g_LocalPlayer && !g_Input->m_fCameraInThirdPerson)
			continue;

		if (i <= g_GlobalVars->maxClients) 
		{
			auto player = Player();
			if (player.Begin((C_BasePlayer*)entity)) 
			{
				if (g_Configurations.esp_player_snaplines) 
					player.RenderSnapline();
				if (g_Configurations.esp_player_boxes)     
					player.RenderBox();  

					player.RenderWeaponName();
				if (g_Configurations.esp_player_names)     
					player.RenderName();
				if (g_Configurations.esp_player_health)    
					player.RenderHealth();
				if (g_Configurations.esp_player_armour)    
					player.RenderArmour();
			}
		}
		else if (g_Configurations.esp_dropped_weapons && entity->IsWeapon())
			RenderWeapon(static_cast<C_BaseCombatWeapon*>(entity));
		else if (g_Configurations.esp_dropped_weapons && entity->IsDefuseKit())
			RenderDefuseKit(entity);
		else if (entity->IsPlantedC4() && g_Configurations.esp_planted_c4)
			RenderC4Window(entity);
		else if (entity->IsLoot() && g_Configurations.esp_items)
			RenderItemEsp(entity);
	}

	if (g_Configurations.esp_crosshair)
		RenderCrosshair();
}
