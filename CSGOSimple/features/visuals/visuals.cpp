#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <random>

#include "visuals.hpp"

#include "../configs/options.hpp"
#include "../helpers/math.hpp"
#include "../helpers/utils.hpp"

RECT GetBBox(C_BaseEntity* ent) {
	RECT rect{};
	auto collideable = ent->GetCollideable();

	if (!collideable)
		return rect;

	auto min = collideable->OBBMins();
	auto max = collideable->OBBMaxs();

	const matrix3x4_t& trans = ent->m_rgflCoordinateFrame();

	Vector points[] = {
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
	for (int i = 0; i < 8; i++) {
		Math::VectorTransform(points[i], trans, pointsTransformed[i]);
	}

	Vector screen_points[8] = {};

	for (int i = 0; i < 8; i++) {
		if (!Math::WorldToScreen(pointsTransformed[i], screen_points[i]))
			return rect;
	}

	auto left = screen_points[0].x;
	auto top = screen_points[0].y;
	auto right = screen_points[0].x;
	auto bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++) {
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

Visuals::Visuals() {
	InitializeCriticalSection(&cs);
}

Visuals::~Visuals() {
	DeleteCriticalSection(&cs);
}

RECT Visuals::get_viewport() {
	RECT viewport = { 0, 0, 0, 0 };
	int w, h;
	g_EngineClient->GetScreenSize(w, h);
	viewport.right = w; viewport.bottom = h;

	return viewport;
}

void Visuals::create_fonts() {
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

void Visuals::text(int X, int Y, const char* text, vgui::HFont font, Color color, bool center) {
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
void Visuals::text_size(int& width, int& height, const char* text, vgui::HFont font) {
	std::wstring WText = std::wstring(std::string_view(text).begin(), std::string_view(text).end());
	g_VGuiSurface->GetTextSize(font, WText.c_str(), width, height);
}
void Visuals::filled_rectange(int x1, int y1, int x2, int y2, Color color) {
	g_VGuiSurface->DrawSetColor(color);
	g_VGuiSurface->DrawFilledRect(x1, y1, x2, y2);
}
void Visuals::outlined_rectange(int x1, int y1, int x2, int y2, Color color) {
	g_VGuiSurface->DrawSetColor(color);
	g_VGuiSurface->DrawOutlinedRect(x1, y1, x2, y2);
}
void Visuals::line(int x1, int y1, int x2, int y2, Color color) {
	g_VGuiSurface->DrawSetColor(color);
	g_VGuiSurface->DrawLine(x1, y1, x2, y2);
}
void Visuals::circle(int x, int y, int r, int seg, Color color) {
	g_VGuiSurface->DrawSetColor(0, 0, 0, 255);
	g_VGuiSurface->DrawSetColor(color);
	g_VGuiSurface->DrawOutlinedCircle(x, y, r, seg);
}
void Visuals::draw_string(unsigned long font, int x, int y, Color color, unsigned long alignment, const char* msg, ...) {
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
void Visuals::draw_string(unsigned long font, bool center, int x, int y, Color color, const char* fmt, ...) {
	wchar_t* pszStringWide = reinterpret_cast<wchar_t*>(malloc((strlen(fmt) + 1) * sizeof(wchar_t)));
	mbstowcs(pszStringWide, fmt, (strlen(fmt) + 1) * sizeof(wchar_t));
	text_w(center, font, x, y, color, pszStringWide);
	free(pszStringWide);
}
void Visuals::text_w(bool center, unsigned long font, int x, int y, Color color, wchar_t* string) {
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
void Visuals::draw_line(float x1, float y1, float x2, float y2, Color color, float size) {
	g_VGuiSurface->DrawSetColor(color);

	if (size == 1.f) g_VGuiSurface->DrawLine(x1, y1, x2, y2);
	else g_VGuiSurface->DrawFilledRect(x1 - (size / 2.f), y1 - (size / 2.f), x2 + (size / 2.f), y2 + (size / 2.f));
}
void Visuals::draw_3dcircle(Vector position, float points, float radius, Color color) {
	float step = (float)M_PI * 2.0f / points;

	for (float a = 0; a < (M_PI * 2.0f); a += step) {
		Vector start(radius * cosf(a) + position.x, radius * sinf(a) + position.y, position.z);
		Vector end(radius * cosf(a + step) + position.x, radius * sinf(a + step) + position.y, position.z);

		Vector start2d, end2d;
		if (g_DebugOverlay->ScreenPosition(start, start2d) || g_DebugOverlay->ScreenPosition(end, end2d))return;

		draw_line(start2d.x, start2d.y, end2d.x, end2d.y, color);
	}
}
void Visuals::draw_box_edges(float x1, float y1, float x2, float y2, Color clr, float edge_size, float size) {
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
//--------------------------------------------------------------------------------
void Visuals::Render() {
}
//--------------------------------------------------------------------------------
bool Visuals::Player::Begin(C_BasePlayer* pl)
{
	if (pl->IsDormant() || !pl->IsAlive())
		return false;

	ctx.pl = pl;
	ctx.is_enemy = g_LocalPlayer->m_iTeamNum() != pl->m_iTeamNum();
	ctx.is_visible = g_LocalPlayer->CanSeePlayer(pl, HITBOX_CHEST);

	if (!ctx.is_enemy && g_Options.esp_enemies_only)
		return false;

	ctx.clr = ctx.is_enemy ? (ctx.is_visible ? Color(g_Options.color_esp_enemy_visible) : Color(g_Options.color_esp_enemy_occluded)) : (ctx.is_visible ? Color(g_Options.color_esp_ally_visible) : Color(g_Options.color_esp_ally_occluded));

	auto head = pl->GetHitboxPos(HITBOX_HEAD);
	auto origin = pl->m_vecOrigin();

	head.z += 15;

	if (!Math::WorldToScreen(head, ctx.head_pos) ||
		!Math::WorldToScreen(origin, ctx.feet_pos))
		return false;

	auto h = fabs(ctx.head_pos.y - ctx.feet_pos.y);
	auto w = h / 1.65f;

	ctx.bbox.left = static_cast<long>(ctx.feet_pos.x - w * 0.5f);
	ctx.bbox.right = static_cast<long>(ctx.bbox.left + w);
	ctx.bbox.bottom = static_cast<long>(ctx.feet_pos.y);
	ctx.bbox.top = static_cast<long>(ctx.head_pos.y);

	return true;
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderBox() {
	float edge_size = 25.f;
	switch (g_Options.esp_player_boxes_type) {
	case 0:
		Visuals::Get().outlined_rectange(ctx.bbox.left, ctx.bbox.top, ctx.bbox.right, ctx.bbox.bottom, ctx.clr);
		Visuals::Get().outlined_rectange(ctx.bbox.left + 1, ctx.bbox.top + 1, ctx.bbox.right - 1, ctx.bbox.bottom - 1, Color::Black);
		Visuals::Get().outlined_rectange(ctx.bbox.left - 1, ctx.bbox.top - 1, ctx.bbox.right + 1, ctx.bbox.bottom + 1, Color::Black);
		break;
	case 1:
		if (ctx.pl != g_LocalPlayer)
			edge_size = 4000.f / Math::VectorDistance(g_LocalPlayer->m_vecOrigin(), ctx.pl->m_vecOrigin());
		Visuals::Get().draw_box_edges(ctx.bbox.left, ctx.bbox.top, ctx.bbox.right, ctx.bbox.bottom, ctx.clr, edge_size, 1);
		Visuals::Get().draw_box_edges(ctx.bbox.left + 1, ctx.bbox.top + 1, ctx.bbox.right - 1, ctx.bbox.bottom - 1, Color::Black, edge_size, 1);
		Visuals::Get().draw_box_edges(ctx.bbox.left - 1, ctx.bbox.top - 1, ctx.bbox.right + 1, ctx.bbox.bottom + 1, Color::Black, edge_size, 1);
		break;
	}
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderName() {
	player_info_t player_info;
	g_EngineClient->GetPlayerInfo(ctx.pl->EntIndex(), &player_info);

	int text_width, text_height;
	Visuals::Get().text_size(text_width, text_height, player_info.szName, Visuals::Get().esp_font);
	Visuals::Get().text(ctx.bbox.left + (ctx.bbox.right - ctx.bbox.left) / 2, ctx.bbox.top - text_height, player_info.szName, Visuals::Get().esp_font, Color(255, 255, 255, 255), true);
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderHealth() {
	float health_value = ctx.pl->m_iHealth();
	int i_health_value = health_value;
	int red = 255 - (health_value * 2.00);
	int green = health_value * 2.00;
	float fl_boxes = std::ceil(ctx.pl->m_iHealth() / 10.f);

	float height = (ctx.bbox.bottom - ctx.bbox.top) * (health_value / 100);
	float height2 = (ctx.bbox.bottom - ctx.bbox.top) * (100 / 100);
	float fl_height = height2 / 10.f;

	int x = ctx.bbox.left - 3;

	Visuals::Get().filled_rectange(x - 5, ctx.bbox.top - 1, x - 1, ctx.bbox.bottom + 1, Color::Black);
	Visuals::Get().filled_rectange(x - 4, ctx.bbox.bottom - height, x - 2, ctx.bbox.bottom, Color(red, green, 0, 255));

	for (int i = 0; i < 10; i++) Visuals::Get().draw_line(x - 5, ctx.bbox.top + i * fl_height, x - 2, ctx.bbox.top + i * fl_height, Color::Black);
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderArmour() {
	int armour = ctx.pl->m_ArmorValue();
	int width = ((ctx.bbox.right - ctx.bbox.left) * armour) / 100;

	Visuals::Get().filled_rectange(ctx.bbox.left - 1, ctx.bbox.bottom + 3, ctx.bbox.right + 1, ctx.bbox.bottom + 7, Color(0, 0, 0, 80));
	Visuals::Get().outlined_rectange(ctx.bbox.left - 1, ctx.bbox.bottom + 3, ctx.bbox.right + 1, ctx.bbox.bottom + 7, Color::Black);
	Visuals::Get().filled_rectange(ctx.bbox.left, ctx.bbox.bottom + 4, ctx.bbox.left + width, ctx.bbox.bottom + 6, Color::Blue);
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderWeaponName() {
	C_BaseCombatWeapon* weapon = ctx.pl->m_hActiveWeapon();
	if (!weapon || !weapon->GetCSWeaponData()) return;
	auto weapon_name = weapon->GetCSWeaponData()->szWeaponName + 7;
	int text_width, text_height;
	Visuals::Get().text_size(text_width, text_height, weapon_name, Visuals::Get().weapon_font);
	Visuals::Get().text(ctx.bbox.left + (ctx.bbox.right - ctx.bbox.left) / 2, ctx.bbox.bottom + 7, weapon_name, Visuals::Get().weapon_font, Color(255, 255, 255, 255), true);
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderSnapline() {
	int screen_w, screen_h;
	g_EngineClient->GetScreenSize(screen_w, screen_h);

	Visuals::Get().draw_line(screen_w / 2.f, screen_h / 2.f, ctx.feet_pos.x, ctx.feet_pos.y, ctx.clr);
}
//--------------------------------------------------------------------------------
void Visuals::RenderCrosshair() {
	int w, h;

	g_EngineClient->GetScreenSize(w, h);

	int cx = w / 2;
	int cy = h / 2;
	Render::Get().RenderLine(cx - 25, cy, cx + 25, cy, Color(g_Options.color_esp_crosshair));
	Render::Get().RenderLine(cx, cy - 25, cx, cy + 25, Color(g_Options.color_esp_crosshair));
}
//--------------------------------------------------------------------------------
void Visuals::RenderWeapon(C_BaseCombatWeapon* ent) {
	auto clean_item_name = [](const char* name) -> const char* {
		if (name[0] == 'C')
			name++;

		auto start = strstr(name, "Weapon");
		if (start != nullptr)
			name = start + 6;

		return name;
	};

	if (ent->m_hOwnerEntity().IsValid()) return;

	auto bbox = GetBBox(ent);
	if (bbox.right == 0 || bbox.bottom == 0) return;

	Render::Get().RenderBox(bbox, Color(g_Options.color_esp_weapons));

	auto name = clean_item_name(ent->GetClientClass()->m_pNetworkName);
	auto sz = g_pDefaultFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, name);
	int w = bbox.right - bbox.left;

	Render::Get().RenderText(name, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 14.f, Color(g_Options.color_esp_weapons));
}
//--------------------------------------------------------------------------------
void Visuals::RenderDefuseKit(C_BaseEntity* ent) {
	if (ent->m_hOwnerEntity().IsValid()) return;

	auto bbox = GetBBox(ent);

	if (bbox.right == 0 || bbox.bottom == 0) return;

	Render::Get().RenderBox(bbox, Color(g_Options.color_esp_defuse));

	auto name = "Defuse Kit";
	auto sz = g_pDefaultFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, name);
	int w = bbox.right - bbox.left;
	Render::Get().RenderText(name, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 14.f, Color(g_Options.color_esp_defuse));
}
//--------------------------------------------------------------------------------
void Visuals::RenderPlantedC4(C_BaseEntity* ent) {
	auto bbox = GetBBox(ent);

	if (bbox.right == 0 || bbox.bottom == 0) return;

	Render::Get().RenderBox(bbox, Color(g_Options.color_esp_c4));

	int bombTimer = std::ceil(ent->m_flC4Blow() - g_GlobalVars->curtime);
	std::string timer = std::to_string(bombTimer);

	auto name = (bombTimer < 0.f) ? "Bomb" : timer;
	auto sz = g_pDefaultFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, name.c_str());
	int w = bbox.right - bbox.left;

	Render::Get().RenderText(name, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 14.f, Color(g_Options.color_esp_c4));
}
//--------------------------------------------------------------------------------
void Visuals::RenderItemEsp(C_BaseEntity* ent) {
	std::string itemstr = "Undefined";
	const model_t * itemModel = ent->GetModel();
	if (!itemModel) return;
	studiohdr_t * hdr = g_MdlInfo->GetStudiomodel(itemModel);
	if (!hdr) return;
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
	else {
		return;
	}
	
	auto bbox = GetBBox(ent);
	if (bbox.right == 0 || bbox.bottom == 0)
		return;
	auto sz = g_pDefaultFont->CalcTextSizeA(14.f, FLT_MAX, 0.0f, itemstr.c_str());
	int w = bbox.right - bbox.left;

	Render::Get().RenderText(itemstr, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 14.f, Color(g_Options.color_esp_item));
}
//--------------------------------------------------------------------------------
void Visuals::ThirdPerson() {
	if (!g_LocalPlayer) return;

	if (g_Options.misc_thirdperson && g_LocalPlayer->IsAlive()) {
		if (!g_Input->m_fCameraInThirdPerson) {
			g_Input->m_fCameraInThirdPerson = true;
		}

		float dist = g_Options.misc_thirdperson_dist;

		QAngle *view = g_LocalPlayer->GetVAngles();
		trace_t tr;
		Ray_t ray;

		Vector desiredCamOffset = Vector(cos(DEG2RAD(view->yaw)) * dist,
			sin(DEG2RAD(view->yaw)) * dist,
			sin(DEG2RAD(-view->pitch)) * dist
		);

		//cast a ray from the Current camera Origin to the Desired 3rd person Camera origin
		ray.Init(g_LocalPlayer->GetEyePos(), (g_LocalPlayer->GetEyePos() - desiredCamOffset));
		CTraceFilter traceFilter;
		traceFilter.pSkip = g_LocalPlayer;
		g_EngineTrace->TraceRay(ray, MASK_SHOT, &traceFilter, &tr);

		Vector diff = g_LocalPlayer->GetEyePos() - tr.endpos;

		float distance2D = sqrt(abs(diff.x * diff.x) + abs(diff.y * diff.y));// Pythagorean

		bool horOK = distance2D > (dist - 2.0f);
		bool vertOK = (abs(diff.z) - abs(desiredCamOffset.z) < 3.0f);

		float cameraDistance;

		if (horOK && vertOK) {
			cameraDistance = dist; // go ahead and set the distance to the setting
		}
		else {
			if (vertOK) {
				cameraDistance = distance2D * 0.95f;
			}
			else {
				cameraDistance = abs(diff.z) * 0.95f;
			}
		}
		g_Input->m_fCameraInThirdPerson = true;

		g_Input->m_vecCameraOffset.z = cameraDistance;
	}
	else {
		g_Input->m_fCameraInThirdPerson = false;
	}
}
//--------------------------------------------------------------------------------
void Visuals::DrawSpeed() {
	auto last_log = 0;

	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive()) return;

	int x, y;
	g_EngineClient->GetScreenSize(x, y);

	Vector vec = g_LocalPlayer->m_vecVelocity();
	float velocity = sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
	bool in_air = g_LocalPlayer->m_fFlags() & IN_JUMP;

	Render::Get().RenderText(std::to_string((int)round(velocity)), ImVec2(x / 2 + 2, y - 100), 27.f, Color::White, false, false);

	Render::Get().RenderLine(x / 2 - 100, y / 2 + 325, x / 2 - 100, y / 2 + 445, Color(100, 100, 100, 125));
	Render::Get().RenderLine(x / 2 - 115, y / 2 + 430, x / 2 + 95, y / 2 + 430, Color(100, 100, 100, 125));

	std::deque<velo_info>velocity_data;
	if (g_GlobalVars->curtime - last_log > g_GlobalVars->interval_per_tick) {
		last_log = g_GlobalVars->curtime;
		velocity_data.push_back({ velocity, in_air });
	}

	if (velocity_data.size() > 40) {
		velocity_data.pop_front();
	}
	
	for (auto i = 0; i < velocity_data.size() - 1; i++) {
		auto cur = velocity_data[i].velocity;
		auto next = velocity_data[i + 1].velocity;
		auto landed = velocity_data[i].on_ground && !velocity_data[i + 1].on_ground;

	//	Render::Get().RenderLine(x / 2 + 90 - (i - 1) * 5, y / 2 + 430 - (std::clamp(cur, 0, 450) * 75 / 320), x / 2 + 90 - i * 5, y / 2 + 130 - (std::clamp(next, 0, 450) * 75 / 320), Color(200, 200, 200, 255), 1.0f);

		if (landed) {
		//	Render::Get().RenderLine(x / 2 + 90 - (i - 1) * 5, y / 2 + 430 - (std::clamp(cur, 0, 450) * 75 / 320), x / 2 + 90 - i * 5, y / 2 + 130 - (std::clamp(next, 0, 450) * 75 / 320), Color(200, 200, 200, 255), 1.0f);
			Render::Get().RenderText(std::to_string(round(next)), ImVec2(x / 2 + 100 - (i + 1) * 5, y / 2 + 415 * 75 / 320), 3.f, Color(245, 245, 220, 255), false);
		}
	}
}
//--------------------------------------------------------------------------------
void Visuals::DrawKeyPresses() {
	int w, h;
	g_EngineClient->GetScreenSize(w, h);
	if (GetAsyncKeyState(int('W')))
		Render::Get().RenderText("W", ImVec2(w / 2, h - h / 5), 15.f, Color::White, true, false);
	else Render::Get().RenderText("_", ImVec2(w / 2, h - h / 5), 15.f, Color::White, true, false);

	if (GetAsyncKeyState(int('S')))
		Render::Get().RenderText("S", ImVec2(w / 2, h - h / 5 + 15), 15.f, Color::White, true, false);
	else Render::Get().RenderText("_", ImVec2(w / 2, h - h / 5 + 15), 15.f, Color::White, true, false);

	if (GetAsyncKeyState(int('A')))
		Render::Get().RenderText("A", ImVec2(w / 2 - 15, h - h / 5), 15.f, Color::White, true, false);
	else Render::Get().RenderText("_", ImVec2(w / 2 - 15, h - h / 5), 15.f, Color::White, true, false);

	if (GetAsyncKeyState(int('D')))
		Render::Get().RenderText("D", ImVec2(w / 2 + 15, h - h / 5), 15.f, Color::White, true, false);
	else Render::Get().RenderText("_", ImVec2(w / 2 + 15, h - h / 5), 15.f, Color::White, true, false);
}
//--------------------------------------------------------------------------------
void Visuals::AddToDrawList() {
	for (auto i = 1; i <= g_EntityList->GetHighestEntityIndex(); ++i) {
		auto entity = C_BaseEntity::GetEntityByIndex(i);

		if (!entity) continue;
		
		if (entity == g_LocalPlayer && !g_Input->m_fCameraInThirdPerson) continue;

		if (i <= g_GlobalVars->maxClients) {
			auto player = Player();
			if (player.Begin((C_BasePlayer*)entity)) {
				if (g_Options.esp_player_snaplines) player.RenderSnapline();
				if (g_Options.esp_player_boxes)     player.RenderBox();
				if (g_Options.esp_player_weapons)   player.RenderWeaponName();
				if (g_Options.esp_player_names)     player.RenderName();
				if (g_Options.esp_player_health)    player.RenderHealth();
				if (g_Options.esp_player_armour)    player.RenderArmour();
			}
		}
		else if (g_Options.esp_dropped_weapons && entity->IsWeapon())
			RenderWeapon(static_cast<C_BaseCombatWeapon*>(entity));
		else if (g_Options.esp_dropped_weapons && entity->IsDefuseKit())
			RenderDefuseKit(entity);
		else if (entity->IsPlantedC4() && g_Options.esp_planted_c4)
			RenderPlantedC4(entity);
		else if (entity->IsLoot() && g_Options.esp_items)
			RenderItemEsp(entity);
	}

	if (g_Options.esp_crosshair)
		RenderCrosshair();
}
