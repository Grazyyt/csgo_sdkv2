#include "misc.hpp"
#include "../configurations.hpp"
#include "../sdk/utils/math.hpp"
#include "../sdk/utils/input.hpp"
#include "../functions/prediction.hpp"
#include "../hooks.hpp"

#include <algorithm>
#include <Windows.h>
#include <d3dx9math.h>

void Misc::AutoStrafe(CUserCmd* cmd)
{
	if (g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER)
		return;

	if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
		return;

	static auto cl_sidespeed = g_CVar->FindVar("cl_sidespeed");
	auto side_speed = cl_sidespeed->GetFloat();


	static auto old_yaw = 0.0f;

	auto get_velocity_degree = [](float velocity)
	{
		auto tmp = RAD2DEG(atan(30.0f / velocity));

		if (CheckIfNonValidNumber(tmp) || tmp > 90.0f)
			return 90.0f;

		else if (tmp < 0.0f)
			return 0.0f;
		else
			return tmp;
	};

	if (g_LocalPlayer->m_nMoveType() != MOVETYPE_WALK)
		return;

	auto velocity = g_LocalPlayer->m_vecVelocity();
	velocity.z = 0.0f;

	auto forwardmove = cmd->forwardmove;
	auto sidemove = cmd->sidemove;

	if (velocity.Length2D() < 5.0f && !forwardmove && !sidemove)
		return;

	static auto flip = false;
	flip = !flip;

	auto turn_direction_modifier = flip ? 1.0f : -1.0f;
	auto viewangles = cmd->viewangles;

	if (forwardmove || sidemove)
	{
		cmd->forwardmove = 0.0f;
		cmd->sidemove = 0.0f;

		auto turn_angle = atan2(-sidemove, forwardmove);
		viewangles.yaw += turn_angle * M_RADPI;
	}
	else if (forwardmove)
		cmd->forwardmove = 0.0f;

	auto strafe_angle = RAD2DEG(atan(15.0f / velocity.Length2D()));

	if (strafe_angle > 90.0f)
		strafe_angle = 90.0f;
	else if (strafe_angle < 0.0f)
		strafe_angle = 0.0f;

	auto temp = Vector(0.0f, viewangles.yaw - old_yaw, 0.0f);
	temp.y = Math::NormalizeYaw(temp.y);

	auto yaw_delta = temp.y;
	old_yaw = viewangles.yaw;

	auto abs_yaw_delta = fabs(yaw_delta);

	if (abs_yaw_delta <= strafe_angle || abs_yaw_delta >= 30.0f)
	{
		QAngle velocity_angles;
		Math::VectorAngles(velocity, velocity_angles);

		temp = Vector(0.0f, viewangles.yaw - velocity_angles.yaw, 0.0f);
		temp.y = Math::NormalizeYaw(temp.y);

		auto velocityangle_yawdelta = temp.y;
		auto velocity_degree = get_velocity_degree(velocity.Length2D());

		if (velocityangle_yawdelta <= velocity_degree || velocity.Length2D() <= 15.0f)
		{
			if (-velocity_degree <= velocityangle_yawdelta || velocity.Length2D() <= 15.0f)
			{
				viewangles.yaw += strafe_angle * turn_direction_modifier;
				cmd->sidemove = side_speed * turn_direction_modifier;
			}
			else
			{
				viewangles.yaw = velocity_angles.yaw - velocity_degree;
				cmd->sidemove = side_speed;
			}
		}
		else
		{
			viewangles.yaw = velocity_angles.yaw + velocity_degree;
			cmd->sidemove = -side_speed;
		}
	}
	else if (yaw_delta > 0.0f)
		cmd->sidemove = -side_speed;
	else if (yaw_delta < 0.0f)
		cmd->sidemove = side_speed;

	auto move = Vector(cmd->forwardmove, cmd->sidemove, 0.0f);
	auto speed = move.Length();

	QAngle angles_move;
	Math::VectorAngles(move, angles_move);

	auto normalized_x = fmod(cmd->viewangles.pitch + 180.0f, 360.0f) - 180.0f;
	auto normalized_y = fmod(cmd->viewangles.yaw + 180.0f, 360.0f) - 180.0f;

	auto yaw = DEG2RAD(normalized_y - viewangles.yaw + angles_move.yaw);

	if (normalized_x >= 90.0f || normalized_x <= -90.0f || cmd->viewangles.pitch >= 90.0f && cmd->viewangles.pitch <= 200.0f ||
		cmd->viewangles.pitch <= -90.0f && cmd->viewangles.pitch <= 200.0f)
		cmd->forwardmove = -cos(yaw) * speed;
	else
		cmd->forwardmove = cos(yaw) * speed;

	cmd->sidemove = sin(yaw) * speed;
}

bool jumpbugged;

void Misc::PrePrediction(CUserCmd* cmd, int pre_flags)
{
	if (g_Configurations.misc_bhop)
		if (!InputSys::Get().IsKeyDown(g_Configurations.misc_jumpbugkey) && g_LocalPlayer->m_nMoveType() != MOVETYPE_LADDER && g_LocalPlayer->m_nMoveType() != MOVETYPE_NOCLIP)
			if (!(pre_flags & (FL_ONGROUND)) && cmd->buttons & (IN_JUMP) && !(pre_flags & (FL_INWATER)))
				cmd->buttons &= ~(IN_JUMP);

	if (InputSys::Get().IsKeyDown(g_Configurations.misc_blockbotkey) && g_LocalPlayer->IsAlive() && g_Configurations.misc_blockbot) {

		float bestdist = 250.f;
		int index = -1;

		for (int i = 1; i < g_EngineClient->GetMaxClients(); i++) {

			C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

			if (!entity || !entity->IsAlive() || entity->IsDormant() || entity == g_LocalPlayer)
				continue;

			float dist = g_LocalPlayer->m_vecOrigin().DistTo(entity->m_vecOrigin());
			if (dist < bestdist) {
				bestdist = dist;
				index = i;
			}
		}
		if (index == -1)
			return;
		C_BasePlayer* target = (C_BasePlayer*)g_EntityList->GetClientEntity(index);
		if (!target)
			return;

		Vector vecForward = target->GetAbsOrigin() - g_LocalPlayer->GetAbsOrigin();

		Math::NormalizeAngles(vecForward);

		auto wtf_sidemove = ((cos(DEG2RAD(cmd->viewangles.yaw)) * -vecForward.y) + (sin(DEG2RAD(cmd->viewangles.yaw)) * vecForward.x));
		auto wtf_forwardmove = ((sin(DEG2RAD(cmd->viewangles.yaw)) * vecForward.y) + (cos(DEG2RAD(cmd->viewangles.yaw)) * vecForward.x));
		if (g_LocalPlayer->m_vecOrigin().z > target->m_vecOrigin().z) {
			cmd->sidemove = wtf_sidemove * 20;
			cmd->forwardmove = wtf_forwardmove * 20;
		}
	}
}
void Misc::PostPrediction(CUserCmd* cmd, int pre_flags, int post_flags)
{
	if (g_Configurations.misc_jumpbug)
	{
		if (InputSys::Get().IsKeyDown(g_Configurations.misc_jumpbugkey))
		{
			Hooks::b_predicting = true;
			if (!(pre_flags & (1 << 0)) && post_flags & (1 << 0))
			{
				cmd->buttons |= IN_DUCK;
			}

			if (post_flags & FL_ONGROUND)
			{
				cmd->buttons &= ~IN_JUMP;
				jumpbugged = true;
			}
		}
	}


	if (g_Configurations.misc_edgejump)
	{
		if (InputSys::Get().IsKeyDown(g_Configurations.misc_edgejumpkey))
		{
			if (pre_flags & (1 << 0) && !(post_flags & 1 << 0))
			{
				cmd->buttons |= IN_JUMP;
			}
		}
	//	if (*config::get<bool>(crc("misc:lj")))
	//	{
	//		// kinda weird, dont know why we need a seperate check. Prob because of tickrate. anyways you could remove this check and make a lj on every jump feature :sunglasses:
	//		if (input::hold(*config::get<int>(crc("misc:ej:key"))))
	//		{
	//			if (!(post_flags & 1 << 0))
	//			{
	//				cmd->buttons |= in_duck;
	//			}
	//		}
	//	}
	}

	if (g_Configurations.misc_edgebug)
	{
		if (!(pre_flags & (1 << 0)) && post_flags & (1 << 0) && InputSys::Get().IsKeyDown(g_Configurations.misc_edgebugkey))
		{
			cmd->buttons |= (IN_DUCK);
		}
	}
	////this is basically just the exact same as eb assist but you dont hold the key lOl
	//if (*config::get<bool>(crc("misc:stathelper")))
	//{
	//	if (!(pre_flags & (1 << 0)) && post_flags & (1 << 0))
	//	{
	//		cmd->buttons |= (IN_DUCK);
	//		cmd->buttons |= (IN_DUCK);
	//	}
	//}
}

void Misc::FakeBackwards(CUserCmd* cmd)
{
	static float yawfb = 0;
	auto pWeapon = g_LocalPlayer->m_hActiveWeapon();

	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected() || !g_LocalPlayer->IsAlive())
		return;

	if (g_Configurations.misc_fakebackwards && g_LocalPlayer->m_nMoveType() != MOVETYPE_LADDER && g_LocalPlayer->m_nMoveType() != MOVETYPE_NOCLIP && !(cmd->buttons & IN_USE)) {
		if (yawfb != 180)
			yawfb += g_Configurations.misc_fakebackwardsturnsmoothness; //5
		cmd->viewangles.yaw += yawfb;	//set yaw to 180
	}
	else if (g_LocalPlayer->IsAlive() && g_LocalPlayer->m_nMoveType() != MOVETYPE_LADDER) {
		if (yawfb != 0)
			yawfb -= g_Configurations.misc_fakebackwardsturnsmoothness; //5
		cmd->viewangles.yaw += yawfb; //set yaw to 0
	}
	else if (g_LocalPlayer->IsAlive() && g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER) {
		if (yawfb != 0)
			yawfb -= 180;
		cmd->viewangles.yaw += yawfb;
	}
}