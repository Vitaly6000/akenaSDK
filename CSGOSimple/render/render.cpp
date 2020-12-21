#include "render.hpp"

#include <mutex>

#include "../features/visuals/visuals.hpp"
#include "../features/misc/nade_prediction.h"
#include "../sdk/csgostructs.hpp"
#include "../helpers/input.hpp"
#include "../menu/menu.hpp"
#include "../configs/options.hpp"
#include "../fonts/fonts.hpp"
#include "../helpers/math.hpp"
#include "../features/notify/notify.h"

ImFont* g_pDefaultFont;
ImFont* g_pSecondFont;

ImDrawListSharedData _data;

std::mutex render_mutex;

void Render::Initialize()
{
	ImGui::CreateContext();


	ImGui_ImplWin32_Init(InputSys::Get().GetMainWindow());
	ImGui_ImplDX9_Init(g_D3DDevice9);

	draw_list = new ImDrawList(ImGui::GetDrawListSharedData());
	draw_list_act = new ImDrawList(ImGui::GetDrawListSharedData());
	draw_list_rendering = new ImDrawList(ImGui::GetDrawListSharedData());

	GetFonts();
}

void Render::GetFonts() {

	// menu font
	ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
		Fonts::Droid_compressed_data,
		Fonts::Droid_compressed_size,
		14.f);
	
	// esp font
	g_pDefaultFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
		Fonts::Droid_compressed_data,
		Fonts::Droid_compressed_size,
		18.f);
	

	// font for watermark; just example
	g_pSecondFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
		Fonts::Cousine_compressed_data,
		Fonts::Cousine_compressed_size,
		18.f); 
}

void Render::ClearDrawList() {
	render_mutex.lock();
	draw_list_act->Clear();
	render_mutex.unlock();
}

void Render::BeginScene() {
	draw_list->Clear();
	draw_list->PushClipRectFullScreen();

	notify::render();

	if (g_Options.misc_watermark) {
		std::stringstream ss;
		ss << "csgosimple";

		auto sz = g_pDefaultFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, ss.str().c_str());

		int x = sz.x + 14, y = sz.y + 14;

		Render::Get().RenderBoxFilled(7, 6, x, 8, Color(87, 48, 161));
		Render::Get().RenderBoxFilled(7, 7, x, y, Color(10, 10, 10, 100));
		Render::Get().RenderText(ss.str(), 10, 10, 14.f, Color(255, 255, 255, 255), false, false, g_pDefaultFont);
	}

	int w, h;
	g_EngineClient->GetScreenSize(w, h);

	if (g_EngineClient->IsInGame() && g_LocalPlayer && g_Options.esp_enabled) {
		if (g_Options.misc_info) {
			Visuals::Get().DrawKeyPresses();
			Visuals::Get().DrawSpeed();
		}
		Visuals::Get().AddToDrawList();
		nade_pred.draw();
	}


	render_mutex.lock();
	*draw_list_act = *draw_list;
	render_mutex.unlock();
}

ImDrawList* Render::RenderScene() {

	if (render_mutex.try_lock()) {
		*draw_list_rendering = *draw_list_act;
		render_mutex.unlock();
	}

	return draw_list_rendering;
}

float Render::RenderText(const std::string& text, ImVec2 pos, float size, Color color, bool center, bool outline, ImFont* pFont)
{
	ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, text.c_str());
	if (!pFont->ContainerAtlas) return 0.f;
	draw_list->PushTextureID(pFont->ContainerAtlas->TexID);

	if (center)
		pos.x -= textSize.x / 2.0f;

	if (outline) {
		draw_list->AddText(pFont, size, ImVec2(pos.x + 1, pos.y + 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
		draw_list->AddText(pFont, size, ImVec2(pos.x - 1, pos.y - 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
		draw_list->AddText(pFont, size, ImVec2(pos.x + 1, pos.y - 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
		draw_list->AddText(pFont, size, ImVec2(pos.x - 1, pos.y + 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
	}

	draw_list->AddText(pFont, size, pos, GetU32(color), text.c_str());

	draw_list->PopTextureID();

	return pos.y + textSize.y;
}

void Render::RenderCircle3D(Vector position, float points, float radius, Color color)
{
	float step = (float)M_PI * 2.0f / points;

	for (float a = 0; a < (M_PI * 2.0f); a += step)
	{
		Vector start(radius * cosf(a) + position.x, radius * sinf(a) + position.y, position.z);
		Vector end(radius * cosf(a + step) + position.x, radius * sinf(a + step) + position.y, position.z);

		Vector start2d, end2d;
		if (g_DebugOverlay->ScreenPosition(start, start2d) || g_DebugOverlay->ScreenPosition(end, end2d))
			return;

		RenderLine(start2d.x, start2d.y, end2d.x, end2d.y, color);
	}
}
//--------------------------------------
RECT Render::get_viewport() {
	RECT viewport = { 0, 0, 0, 0 };
	int w, h;
	g_EngineClient->GetScreenSize(w, h);
	viewport.right = w; viewport.bottom = h;

	return viewport;
}
void Render::create_fonts() {
	esp_font = g_VGuiSurface->CreateFont_();
	weapon_font = g_VGuiSurface->CreateFont_();
	weapon_icons = g_VGuiSurface->CreateFont_();
	flags_font = g_VGuiSurface->CreateFont_();
	hud_font = g_VGuiSurface->CreateFont_();

	g_VGuiSurface->SetFontGlyphSet(esp_font, "Verdana", 12, 350, 0, 0, FONTFLAG_DROPSHADOW);
	g_VGuiSurface->SetFontGlyphSet(weapon_font, "Arial", 12, 400, 0, 0, FONTFLAG_DROPSHADOW);
	g_VGuiSurface->SetFontGlyphSet(weapon_icons, "undefeated", 20, 200, 0, 0, FONTFLAG_OUTLINE);
	g_VGuiSurface->SetFontGlyphSet(flags_font, "Small Fonts", 11, 400, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	g_VGuiSurface->SetFontGlyphSet(hud_font, "Tahoma", 16, 400, 0, 0, FONTFLAG_ANTIALIAS);
}
void Render::text(int X, int Y, const char* text, vgui::HFont font, Color color, bool center) {
	std::wstring w_text = std::wstring(std::string_view(text).begin(), std::string_view(text).end());

	g_VGuiSurface->DrawSetTextFont(font);
	g_VGuiSurface->DrawSetTextColor(color);

	if (center) {
		int TextWidth, TextHeight;
		text_size(TextWidth, TextHeight, text, font);
		g_VGuiSurface->DrawSetTextPos(X - TextWidth / 2, Y);
	}
	else g_VGuiSurface->DrawSetTextPos(X, Y);
	g_VGuiSurface->DrawPrintText(w_text.c_str(), wcslen(w_text.c_str()));
}
void Render::text_size(int& width, int& height, const char* text, vgui::HFont font) {
	std::wstring WText = std::wstring(std::string_view(text).begin(), std::string_view(text).end());
	g_VGuiSurface->GetTextSize(font, WText.c_str(), width, height);
}
void Render::filled_rectange(int x1, int y1, int x2, int y2, Color color) {
	g_VGuiSurface->DrawSetColor(color);
	g_VGuiSurface->DrawFilledRect(x1, y1, x2, y2);
}
void Render::outlined_rectange(int x1, int y1, int x2, int y2, Color color) {
	g_VGuiSurface->DrawSetColor(color);
	g_VGuiSurface->DrawOutlinedRect(x1, y1, x2, y2);
}
void Render::line(int x1, int y1, int x2, int y2, Color color) {
	g_VGuiSurface->DrawSetColor(color);
	g_VGuiSurface->DrawLine(x1, y1, x2, y2);
}
void Render::circle(int x, int y, int r, int seg, Color color) {
	g_VGuiSurface->DrawSetColor(0, 0, 0, 255);
	g_VGuiSurface->DrawSetColor(color);
	g_VGuiSurface->DrawOutlinedCircle(x, y, r, seg);
}
void Render::draw_string(unsigned long font, int x, int y, Color color, unsigned long alignment, const char* msg, ...) {
	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	int r = 255, g = 255, b = 255, a = 255;
	color.GetColor(r, g, b, a);

	int width, height;
	g_VGuiSurface->GetTextSize(font, wbuf, width, height);

	if (alignment & font_right)
		x -= width;
	if (alignment & font_center)
		x -= width / 2;

	g_VGuiSurface->DrawSetTextFont(font);
	g_VGuiSurface->DrawSetTextColor(r, g, b, a);
	g_VGuiSurface->DrawSetTextPos(x, y - height / 2);
	g_VGuiSurface->DrawPrintText(wbuf, wcslen(wbuf));
}
void Render::draw_string(unsigned long font, bool center, int x, int y, Color color, const char* fmt, ...) {
	wchar_t* pszStringWide = reinterpret_cast<wchar_t*>(malloc((strlen(fmt) + 1) * sizeof(wchar_t)));
	mbstowcs(pszStringWide, fmt, (strlen(fmt) + 1) * sizeof(wchar_t));
	text_w(center, font, x, y, color, pszStringWide);
	free(pszStringWide);
}
void Render::text_w(bool center, unsigned long font, int x, int y, Color color, wchar_t* string) {
	if (center) {
		int wide, tall;
		g_VGuiSurface->GetTextSize(font, string, wide, tall);
		x -= wide / 2;
		y -= tall / 2;
	}
	g_VGuiSurface->DrawSetTextColor(color);
	g_VGuiSurface->DrawSetTextFont(font);
	g_VGuiSurface->DrawSetTextPos(x, y);
	g_VGuiSurface->DrawPrintText(string, (int)wcslen(string), FONT_DRAW_DEFAULT);
}
void Render::draw_line(float x1, float y1, float x2, float y2, Color color, float size) {
	g_VGuiSurface->DrawSetColor(color);

	if (size == 1.f) g_VGuiSurface->DrawLine(x1, y1, x2, y2);
	else g_VGuiSurface->DrawFilledRect(x1 - (size / 2.f), y1 - (size / 2.f), x2 + (size / 2.f), y2 + (size / 2.f));
}
void Render::draw_3dcircle(Vector position, float points, float radius, Color color) {
	float step = (float)M_PI * 2.0f / points;

	for (float a = 0; a < (M_PI * 2.0f); a += step) {
		Vector start(radius * cosf(a) + position.x, radius * sinf(a) + position.y, position.z);
		Vector end(radius * cosf(a + step) + position.x, radius * sinf(a + step) + position.y, position.z);

		Vector start2d, end2d;
		if (g_DebugOverlay->ScreenPosition(start, start2d) || g_DebugOverlay->ScreenPosition(end, end2d))return;

		draw_line(start2d.x, start2d.y, end2d.x, end2d.y, color);
	}
}
void Render::draw_box_edges(float x1, float y1, float x2, float y2, Color clr, float edge_size, float size) {
	if (fabs(x1 - x2) < (edge_size * 2)) {
		edge_size = fabs(x1 - x2) / 4.f;
	}

	draw_line(x1, y1, x1, y1 + edge_size + (0.5f * edge_size), clr, size);
	draw_line(x2, y1, x2, y1 + edge_size + (0.5f * edge_size), clr, size);
	draw_line(x1, y2, x1, y2 - edge_size - (0.5f * edge_size), clr, size);
	draw_line(x2, y2, x2, y2 - edge_size - (0.5f * edge_size), clr, size);
	draw_line(x1, y1, x1 + edge_size, y1, clr, size);
	draw_line(x2, y1, x2 - edge_size, y1, clr, size);
	draw_line(x1, y2, x1 + edge_size, y2, clr, size);
	draw_line(x2, y2, x2 - edge_size, y2, clr, size);
}