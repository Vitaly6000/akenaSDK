#pragma once

#include "../helpers/singleton.hpp"

#include "../render/render.hpp"
#include "../helpers/math.hpp"
#include "../sdk/csgostructs.hpp"
#include <deque>

struct velo_info {
public:
	float velocity;
	bool on_ground;
};

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
	void RenderItemEsp(C_BaseEntity* ent);
	void ThirdPerson();
	void DrawSpeed();
	void DrawKeyPresses();
public:
	void AddToDrawList();
	void Render();
};
