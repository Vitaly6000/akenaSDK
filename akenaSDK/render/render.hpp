#pragma once
#include <string>
#include <sstream>
#include <stdint.h>
#include <d3d9.h>
#pragma comment(lib,"d3d9.lib")

#include "../helpers/singleton.hpp"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/impl/imgui_impl_dx9.h"
#include "../imgui/impl/imgui_impl_win32.h"

#include "../sdk/misc/Color.hpp"
#include "../sdk/csgostructs.hpp"
#include "../helpers/math.hpp"

extern ImFont* g_pDefaultFont;
extern ImFont* g_pSecondFont;

class Vector;

class Render : public Singleton<Render> {
	friend class Singleton<Render>;

private:
	ImDrawList * draw_list_act;
	ImDrawList * draw_list_rendering;
	ImDrawList*  draw_list;
	ImDrawData   draw_data;

	ImU32 GetU32(Color _color)
	{
		return ((_color[3] & 0xff) << 24) + ((_color[2] & 0xff) << 16) + ((_color[1] & 0xff) << 8)
			+ (_color[0] & 0xff);
	}
public:
	void Initialize();
	void GetFonts();
	void ClearDrawList();
	void BeginScene();
	ImDrawList* RenderScene();

	float RenderText(const std::string& text, ImVec2 position, float size, Color color, bool center = false, bool outline = true, ImFont* pFont = g_pDefaultFont);

	void RenderCircle3D(Vector position, float points, float radius, Color color);

	void RenderImage(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a = ImVec2(0, 0), const ImVec2& uv_b = ImVec2(1, 1), ImU32 col = 0xFFFFFFFF) {
		draw_list->AddImage(user_texture_id, a, b, uv_a, uv_b, col);
	}

	template <class T>
	inline void RenderBox(T x1, T y1, T x2, T y2, Color color, float thickness = 1.f, float rounding = 0.f) {
		draw_list->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), GetU32(color), rounding, 15, thickness);
	}
	inline void RenderBox(RECT r, Color color,float thickness = 1.f, float rounding = 0.f) {
		RenderBox(r.left, r.top, r.right, r.bottom, color, thickness, rounding);
	}
	template <class T>
	inline void RenderBoxFilled(T x1, T y1, T x2, T y2, Color color, float thickness = 1.f, float rounding = 0.f) {
		draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), GetU32(color), rounding, 15);
	}
	template <class T>
	inline void RenderLine(T x1, T y1, T x2, T y2, Color color, float thickness = 1.f) {
		draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), GetU32(color), thickness);
	}
	template <class T>
	inline float RenderText(const std::string& text, T x, T y, float size, Color clr, bool center = false, bool outline = true, ImFont* pFont = g_pDefaultFont) {
		return RenderText(text, ImVec2(x, y), size, clr, center, outline, pFont);
	}
	template <class T>
	inline void RenderCircle(T x, T y, float radius, int points, Color color, float thickness = 1.f) {
		draw_list->AddCircle(ImVec2(x, y), radius, GetU32(color), points, thickness);
	}
	template <class T>
	inline void RenderCircleFilled(T x, T y, float radius, int points, Color color) {
		draw_list->AddCircleFilled(ImVec2(x, y), radius, GetU32(color), points);
	}
	//--------------------------------------
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
};