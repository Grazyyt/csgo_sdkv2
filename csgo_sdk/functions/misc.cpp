#include "misc.hpp"
#include "../configurations.hpp"
#include "../sdk/utils/math.hpp"
#include "../sdk/utils/input.hpp"

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

void Misc::Bhop(CUserCmd* cmd)
{
	static bool jumped_last_tick = false;
	static bool should_fake_jump = false;

	if (!bhop2)
		return;

	if (!g_LocalPlayer)
		return;

	if (!g_LocalPlayer->IsAlive())
		return;

	if (g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER || g_LocalPlayer->m_nMoveType() == MOVETYPE_NOCLIP)
		return;

	if (g_LocalPlayer->m_fFlags() & FL_INWATER)
		return;

	if (!jumped_last_tick && should_fake_jump)
	{
		should_fake_jump = false;
		cmd->buttons |= IN_JUMP;
	}
	else if (cmd->buttons & IN_JUMP)
	{
		if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
		{
			jumped_last_tick = true;
			should_fake_jump = true;
		}
		else
		{
			cmd->buttons &= ~IN_JUMP;
			jumped_last_tick = false;
		}
	}
	else
	{
		jumped_last_tick = false;
		should_fake_jump = false;
	}
}

void Misc::JumpBug(CUserCmd* cmd)
{
	float max_radias = D3DX_PI * 2;
	float step = max_radias / 128;
	float xThick = 23;
	if (g_Configurations.misc_jumpbug&& InputSys::Get().IsKeyDown(g_Configurations.misc_jumpbugkey)) {
		if (g_LocalPlayer->m_fFlags() & FL_ONGROUND) {
			bhop2 = false;
			bool unduck = cmd->buttons &= ~IN_DUCK;
			if (unduck) {
				cmd->buttons &= ~IN_DUCK; // duck
				cmd->buttons |= IN_JUMP; // jump
				unduck = false;
			}
			Vector pos = g_LocalPlayer->GetAbsOrigin();
			for (float a = 0.f; a < max_radias; a += step) {
				Vector pt;
				pt.x = (xThick * cos(a)) + pos.x;
				pt.y = (xThick * sin(a)) + pos.y;
				pt.z = pos.z;


				Vector pt2 = pt;
				pt2.z -= 8192;

				trace_t fag;

				Ray_t ray;
				ray.Init(pt, pt2);

				CTraceFilter flt;
				flt.pSkip = g_LocalPlayer;
				g_EngineTrace->TraceRay(ray, MASK_PLAYERSOLID, &flt, &fag);

				if (fag.fraction != 1.f && fag.fraction != 0.f) {
					cmd->buttons |= IN_DUCK; // duck
					cmd->buttons &= ~IN_JUMP; // jump
					unduck = true;
				}
			}
			for (float a = 0.f; a < max_radias; a += step) {
				Vector pt;
				pt.x = ((xThick - 2.f) * cos(a)) + pos.x;
				pt.y = ((xThick - 2.f) * sin(a)) + pos.y;
				pt.z = pos.z;

				Vector pt2 = pt;
				pt2.z -= 8192;

				trace_t fag;

				Ray_t ray;
				ray.Init(pt, pt2);

				CTraceFilter flt;
				flt.pSkip = g_LocalPlayer;
				g_EngineTrace->TraceRay(ray, MASK_PLAYERSOLID, &flt, &fag);

				if (fag.fraction != 1.f && fag.fraction != 0.f) {
					cmd->buttons |= IN_DUCK; // duck
					cmd->buttons &= ~IN_JUMP; // jump
					unduck = true;
				}
			}
			for (float a = 0.f; a < max_radias; a += step) {
				Vector pt;
				pt.x = ((xThick - 20.f) * cos(a)) + pos.x;
				pt.y = ((xThick - 20.f) * sin(a)) + pos.y;
				pt.z = pos.z;

				Vector pt2 = pt;
				pt2.z -= 8192;

				trace_t fag;

				Ray_t ray;
				ray.Init(pt, pt2);

				CTraceFilter flt;
				flt.pSkip = g_LocalPlayer;
				g_EngineTrace->TraceRay(ray, MASK_PLAYERSOLID, &flt, &fag);

				if (fag.fraction != 1.f && fag.fraction != 0.f) {
					cmd->buttons |= IN_DUCK; // duck
					cmd->buttons &= ~IN_JUMP; // jump
					unduck = true;
				}
			}
		}
	}
	else bhop2 = true;
}