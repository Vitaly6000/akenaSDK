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

class Visuals : public Singleton<Visuals> {
	friend class Singleton<Visuals>;

	CRITICAL_SECTION cs;

	Visuals();
	~Visuals();
public:

	enum font_render_flag {
		font_left = 0,
		font_right = 1,
		font_center = 2
	};

	vgui::HFont esp_font;
	vgui::HFont weapon_font;
	vgui::HFont flags_font;
	vgui::HFont weapon_icons;
	vgui::HFont hud_font;

	void create_fonts();
	void text(int X, int Y, const char* text, vgui::HFont font, Color color, bool center);
	void text_size(int& width, int& height, const char* text, vgui::HFont font);
	void filled_rectange(int x1, int y1, int x2, int y2, Color color);
	void outlined_rectange(int x1, int y1, int x2, int y2, Color color);
	void line(int x1, int y1, int x2, int y2, Color color);
	void circle(int x, int y, int r, int seg, Color color);
	void draw_string(unsigned long font, int x, int y, Color color, unsigned long alignment, const char* msg, ...);
	void draw_string(unsigned long font, bool center, int x, int y, Color color, const char* fmt, ...);
	void text_w(bool center, unsigned long font, int x, int y, Color color, wchar_t* string);
	void draw_line(float x1, float y1, float x2, float y2, Color color, float size = 1.f);
	void draw_3dcircle(Vector position, float points, float radius, Color color);
	void draw_box_edges(float x1, float y1, float x2, float y2, Color clr, float edge_size, float size = 1.f);
	RECT get_viewport();

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
