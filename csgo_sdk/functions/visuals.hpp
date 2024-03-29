#pragma once

#include "../sdk/utils/singleton.hpp"
#include "../render/render.hpp"
#include "../sdk/utils/math.hpp"
#include "../sdk/csgostructs.hpp"

class Visuals : public Singleton<Visuals>
{
	friend class Singleton<Visuals>;

	CRITICAL_SECTION cs;

	Visuals();
	~Visuals();
public:
	class Player
	{
	public:
		struct
		{
			C_BasePlayer* pl;
			bool          is_enemy;
			bool          is_visible;
			Color         clr;
			Vector        head_pos;
			Vector        feet_pos;
			RECT          bbox;
		} ctx;

		bool Begin(C_BasePlayer * pl);
		void RenderBox();
		void RenderName();
		void RenderWeaponName();
		void RenderHealth();
		void RenderArmour();
		void RenderSnapline();
	};
	void RenderCrosshair();
	void RenderWeapon(C_BaseCombatWeapon* ent);
	void RenderDefuseKit(C_BaseEntity* ent);
	void RenderPlantedC4(C_BaseEntity* ent);
	void RenderC4Window(C_BaseEntity* ent);
	void RenderItemEsp(C_BaseEntity* ent);
	void ThirdPerson();
	void DrawFOV();
	void Nightmode();
	void MotionBlur(CViewSetup* setup);
public:
	void AddToDrawList();
	void Render();
};