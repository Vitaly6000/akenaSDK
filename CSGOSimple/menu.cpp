#include "Menu.hpp"
#define NOMINMAX
#include <Windows.h>
#include <chrono>

#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "options.hpp"
#include "ui.hpp"
#include "config.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "imgui/impl/imgui_impl_dx9.h"
#include "imgui/impl/imgui_impl_win32.h"

#pragma region MenuTabs
bool SubTabEx(const char* label, const char* icon, const bool selected, const ImVec2& size_arg)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;

    ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

    if (selected) {
        window->DrawList->AddRectFilled({ bb.Min.x + 20,bb.Max.y - 7 }, { bb.Max.x - 20,bb.Max.y - 5 }, ImColor(255, 255, 255), 6);
    }

    window->DrawList->AddText(ImVec2(bb.Min.x + size_arg.x / 2 - ImGui::CalcTextSize(label).x / 2, bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize(label).y / 2), ImColor(255, 255, 255, 255), label);

    return pressed;
}

bool SubTab(const char* label, const char* icon, const ImVec2& size_arg, const bool selected)
{
    return SubTabEx(label, icon, selected, size_arg);
}

bool TabEx(const char* label, const char* icon, const bool selected, const ImVec2& size_arg)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;

    ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

    if (selected) {
        window->DrawList->AddRectFilled({ bb.Min.x,bb.Max.y - 7 }, { bb.Max.x,bb.Max.y - 5 }, ImColor(255, 255, 255), 6);
    }

    window->DrawList->AddText(ImVec2(bb.Min.x + size_arg.x / 2 - ImGui::CalcTextSize(label).x / 2, bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize(label).y / 2), ImColor(255, 255, 255, 255), label);

    return pressed;
}

bool Tab(const char* label, const char* icon, const ImVec2& size_arg, const bool selected)
{
    return TabEx(label, icon, selected, size_arg);
}
#pragma endregion

void RenderEspTab()
{
    static char* esp_tab_names[] = { "ESP", "GLOW", "CHAMS" };
    static int   active_esp_tab = 0;

    bool placeholder_true = true;

    auto& style = ImGui::GetStyle();
    float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

    ImGui::PopStyleVar();
    ImGui::BeginGroupBox("##body_content");
    {
        if(active_esp_tab == 0) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.WindowPadding.x, style.ItemSpacing.y });
            ImGui::Columns(3, nullptr, false);
            ImGui::SetColumnOffset(1, group_w / 3.0f);
            ImGui::SetColumnOffset(2, 2 * group_w / 3.0f);
            ImGui::SetColumnOffset(3, group_w);

            ImGui::Checkbox("Enabled", g_Options.esp_enabled);
            ImGui::Checkbox("Team check", g_Options.esp_enemies_only);
            ImGui::Checkbox("Boxes", g_Options.esp_player_boxes);
            ImGui::Checkbox("Names", g_Options.esp_player_names);
            ImGui::Checkbox("Health", g_Options.esp_player_health);
            ImGui::Checkbox("Armour", g_Options.esp_player_armour);
            ImGui::Checkbox("Weapon", g_Options.esp_player_weapons);
            ImGui::Checkbox("Snaplines", g_Options.esp_player_snaplines);

            ImGui::NextColumn();

            ImGui::Checkbox("Crosshair", g_Options.esp_crosshair);
            ImGui::Checkbox("Dropped Weapons", g_Options.esp_dropped_weapons);
            ImGui::Checkbox("Defuse Kit", g_Options.esp_defuse_kit);
            ImGui::Checkbox("Planted C4", g_Options.esp_planted_c4);
			ImGui::Checkbox("Item Esp", g_Options.esp_items);

            ImGui::NextColumn();

           ImGui::PushItemWidth(100);
            ImGui::ColorEdit4("Allies Visible", g_Options.color_esp_ally_visible);
            ImGui::ColorEdit4("Enemies Visible", g_Options.color_esp_enemy_visible);
            ImGui::ColorEdit4("Allies Occluded", g_Options.color_esp_ally_occluded);
            ImGui::ColorEdit4("Enemies Occluded", g_Options.color_esp_enemy_occluded);
            ImGui::ColorEdit4("Crosshair", g_Options.color_esp_crosshair);
            ImGui::ColorEdit4("Dropped Weapons", g_Options.color_esp_weapons);
            ImGui::ColorEdit4("Defuse Kit", g_Options.color_esp_defuse);
            ImGui::ColorEdit4("Planted C4", g_Options.color_esp_c4);
			ImGui::ColorEdit4("Item Esp", g_Options.color_esp_item);
            ImGui::PopItemWidth();

            ImGui::Columns(1, nullptr, false);
            ImGui::PopStyleVar();
        } else if(active_esp_tab == 1) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.WindowPadding.x, style.ItemSpacing.y });
            ImGui::Columns(3, nullptr, false);
            ImGui::SetColumnOffset(1, group_w / 3.0f);
            ImGui::SetColumnOffset(2, 2 * group_w / 3.0f);
            ImGui::SetColumnOffset(3, group_w);

            ImGui::Checkbox("Enabled", g_Options.glow_enabled);
            ImGui::Checkbox("Team check", g_Options.glow_enemies_only);
            ImGui::Checkbox("Players", g_Options.glow_players);
            ImGui::Checkbox("Chickens", g_Options.glow_chickens);
            ImGui::Checkbox("C4 Carrier", g_Options.glow_c4_carrier);
            ImGui::Checkbox("Planted C4", g_Options.glow_planted_c4);
            ImGui::Checkbox("Defuse Kits", g_Options.glow_defuse_kits);
            ImGui::Checkbox("Weapons", g_Options.glow_weapons);

            ImGui::NextColumn();

            ImGui::PushItemWidth(100);
            ImGui::ColorEdit4("Ally", g_Options.color_glow_ally);
            ImGui::ColorEdit4("Enemy", g_Options.color_glow_enemy);
            ImGui::ColorEdit4("Chickens", g_Options.color_glow_chickens);
            ImGui::ColorEdit4("C4 Carrier", g_Options.color_glow_c4_carrier);
            ImGui::ColorEdit4("Planted C4", g_Options.color_glow_planted_c4);
            ImGui::ColorEdit4("Defuse Kits", g_Options.color_glow_defuse);
            ImGui::ColorEdit4("Weapons", g_Options.color_glow_weapons);
            ImGui::PopItemWidth();

            ImGui::NextColumn();

            ImGui::Columns(1, nullptr, false);
            ImGui::PopStyleVar();
        } else if(active_esp_tab == 2) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.WindowPadding.x, style.ItemSpacing.y });
            ImGui::Columns(3, nullptr, false);
            ImGui::SetColumnOffset(1, group_w / 3.0f);
            ImGui::SetColumnOffset(2, 2 * group_w / 2.9f);
            ImGui::SetColumnOffset(3, group_w);

            ImGui::BeginGroupBox("Players");
            {
                ImGui::Checkbox("Enabled", g_Options.chams_player_enabled); ImGui::SameLine();
                ImGui::Checkbox("Team Check", g_Options.chams_player_enemies_only);
                ImGui::Checkbox("Wireframe", g_Options.chams_player_wireframe);
                ImGui::Checkbox("Flat", g_Options.chams_player_flat);
                ImGui::Checkbox("Ignore-Z", g_Options.chams_player_ignorez); ImGui::SameLine();
                ImGui::Checkbox("Glass", g_Options.chams_player_glass);
               ImGui::PushItemWidth(110);
                ImGui::ColorEdit4("Ally (Visible)", g_Options.color_chams_player_ally_visible);
                ImGui::ColorEdit4("Ally (Occluded)", g_Options.color_chams_player_ally_occluded);
                ImGui::ColorEdit4("Enemy (Visible)", g_Options.color_chams_player_enemy_visible);
                ImGui::ColorEdit4("Enemy (Occluded)", g_Options.color_chams_player_enemy_occluded);
                ImGui::PopItemWidth();
            }
            ImGui::EndGroupBox();

            ImGui::NextColumn();

            ImGui::BeginGroupBox("Arms");
            {
                ImGui::Checkbox("Enabled", g_Options.chams_arms_enabled);
                ImGui::Checkbox("Wireframe", g_Options.chams_arms_wireframe);
                ImGui::Checkbox("Flat", g_Options.chams_arms_flat);
                ImGui::Checkbox("Ignore-Z", g_Options.chams_arms_ignorez);
                ImGui::Checkbox("Glass", g_Options.chams_arms_glass);
                ImGui::PushItemWidth(110);
                ImGui::ColorEdit4("Color (Visible)", g_Options.color_chams_arms_visible);
                ImGui::ColorEdit4("Color (Occluded)", g_Options.color_chams_arms_occluded);
                ImGui::PopItemWidth();
            }
            ImGui::EndGroupBox();

            ImGui::Columns(1, nullptr, false);
            ImGui::PopStyleVar();
        }
    }
    ImGui::EndGroupBox();
}

void RenderMiscTab()
{
    bool placeholder_true = true;

    auto& style = ImGui::GetStyle();
    float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::ToggleButton("MISC", &placeholder_true, ImVec2{ group_w, 25.0f });
    ImGui::PopStyleVar();

    ImGui::BeginGroupBox("##body_content");
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.WindowPadding.x, style.ItemSpacing.y });
        ImGui::Columns(3, nullptr, false);
        ImGui::SetColumnOffset(1, group_w / 3.0f);
        ImGui::SetColumnOffset(2, 2 * group_w / 3.0f);
        ImGui::SetColumnOffset(3, group_w);

        ImGui::Checkbox("Bunny hop", g_Options.misc_bhop);
		ImGui::Checkbox("Third Person", g_Options.misc_thirdperson);
		if(g_Options.misc_thirdperson)
			ImGui::SliderFloat("Distance", g_Options.misc_thirdperson_dist, 0.f, 150.f);
        ImGui::Checkbox("No hands", g_Options.misc_no_hands);
		ImGui::Checkbox("Rank reveal", g_Options.misc_showranks);
		ImGui::Checkbox("Watermark##hc", g_Options.misc_watermark);
        //ImGui::PushItemWidth(-1.0f);
		ImGui::NextColumn();
        ImGui::SliderInt("viewmodel_fov:", g_Options.viewmodel_fov, 68, 120);
        //ImGui::PopItemWidth();

        ImGui::Columns(1, nullptr, false);
        ImGui::PopStyleVar();
    }
    ImGui::EndGroupBox();
}

void RenderConfigTab()
{
    auto& style = ImGui::GetStyle();
    float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

    bool placeholder_true = true;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
    ImGui::ToggleButton("CONFIG", &placeholder_true, ImVec2{ group_w, 25.0f });
    ImGui::PopStyleVar();

    ImGui::BeginGroupBox("##body_content");
    {
		if (ImGui::Button("Save cfg")) {
			Config::Get().Save();
		}
		if (ImGui::Button("Load cfg")) {
			Config::Get().Load();
		}
    }
    ImGui::EndGroupBox();
}

void Menu::Initialize()
{
	CreateStyle();

    _visible = true;
}

void Menu::Shutdown()
{
    ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Menu::OnDeviceLost()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
}

void Menu::OnDeviceReset()
{
    ImGui_ImplDX9_CreateDeviceObjects();
}

void Menu::Render()
{
	ImGui::GetIO().MouseDrawCursor = _visible;

    if(!_visible)
        return;

    static int Tabs = 1;
    static int SubTabLegit = 1;
    static int SubTabMisc = 1;
    static int SubTabVisuals = 1;

    static auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
    ImGui::SetNextWindowSize({ 430,430 });
    if (ImGui::Begin("CSGOSimple", &_visible, flags)) {
        if (ImGui::BeginChild("Child1", ImVec2(-1, 25), false, flags)) {
            ImVec2 p = ImGui::GetCursorScreenPos();

            ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x, p.y + 3), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + -3), ImColor(87, 48, 161));
            ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x, p.y + 3), ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y + -3), ImColor(0, 0, 0, 125), ImColor(0, 0, 0, 15), ImColor(0, 0, 0, 15), ImColor(0, 0, 0, 125));
            ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y + 3), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + -3), ImColor(0, 0, 0, 15), ImColor(0, 0, 0, 125), ImColor(0, 0, 0, 125), ImColor(0, 0, 0, 15));
            ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x, p.y + 2), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + 2), ImColor(0, 0, 0, 150));

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);

            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("CSGOSimple").x) / 2);
            ImGui::TextColored(ImColor(255, 255, 255), "CSGOSimple");
        }
        ImGui::EndChild();
        if (ImGui::BeginChild("Child2", ImVec2(-1, 50), false, flags)) {

            if (Tab("LegitBot", "", { 100,50 }, Tabs == 1 ? true : false))
                Tabs = 1;

            ImGui::SameLine();

            if (Tab("Visuals", "", { 100,50 }, Tabs == 2 ? true : false))
                Tabs = 2;

            ImGui::SameLine();

            if (Tab("Skins", "", { 100,50 }, Tabs == 3 ? true : false))
                Tabs = 3;

            ImGui::SameLine();

            if (Tab("Misc", "", { 100,50 }, Tabs == 4 ? true : false))
                Tabs = 4;
        }
        ImGui::EndChild();
        if (Tabs == 1 || Tabs == 2 || Tabs == 4) {
            if (ImGui::BeginChild("Child4", ImVec2(75, -1), false, flags)) {
                if (Tabs == 1) {
                    if (SubTab("Legit", "", { 75,50 }, SubTabLegit == 1 ? true : false))
                        SubTabLegit = 1;

                    if (SubTab("Trigger", "", { 75,50 }, SubTabLegit == 2 ? true : false))
                        SubTabLegit = 2;
                }
                if (Tabs == 2) {
                    if (SubTab("Esp", "", { 75,50 }, SubTabVisuals == 1 ? true : false))
                        SubTabVisuals = 1;

                    if (SubTab("Glow", "", { 75,50 }, SubTabVisuals == 2 ? true : false))
                        SubTabVisuals = 2;

                    if (SubTab("Chams", "", { 75,50 }, SubTabVisuals == 3 ? true : false))
                        SubTabVisuals = 3;

                    if (SubTab("Misc", "", { 75,50 }, SubTabVisuals == 4 ? true : false))
                        SubTabVisuals = 4;

                    if (SubTab("Color", "", { 75,50 }, SubTabVisuals == 5 ? true : false))
                        SubTabVisuals = 5;
                }
                if (Tabs == 4) {
                    if (SubTab("Misc", "", { 75,50 }, SubTabMisc == 1 ? true : false))
                        SubTabMisc = 1;

                    if (SubTab("Cfg", "", { 75,50 }, SubTabMisc == 2 ? true : false))
                        SubTabMisc = 2;
                }
            }
            ImGui::EndChild();

            ImGui::SameLine();
        }
        if (ImGui::BeginChild("Child3", ImVec2(-1, -1), false, flags)) {
            if (Tabs == 1) {
                if (SubTabLegit == 1) {
                    ImGui::Text("Here legit tab");
                }
                if (SubTabLegit == 2) {
                    ImGui::Text("Here trigger tab");
                }
            }
            if (Tabs == 2) {
                if (SubTabVisuals == 1) {
                    ImGui::Columns(2, nullptr, false);

                    ImGui::Checkbox("Enabled", g_Options.esp_enabled);
                    ImGui::Checkbox("Team check", g_Options.esp_enemies_only);
                    ImGui::Checkbox("Boxes", g_Options.esp_player_boxes);
                    ImGui::Checkbox("Names", g_Options.esp_player_names);
                    ImGui::Checkbox("Health", g_Options.esp_player_health);
                    ImGui::Checkbox("Armour", g_Options.esp_player_armour);
                    ImGui::Checkbox("Weapon", g_Options.esp_player_weapons);
                    ImGui::Checkbox("Snaplines", g_Options.esp_player_snaplines);

                    ImGui::NextColumn();

                    ImGui::Checkbox("Crosshair", g_Options.esp_crosshair);
                    ImGui::Checkbox("Dropped Weapons", g_Options.esp_dropped_weapons);
                    ImGui::Checkbox("Defuse Kit", g_Options.esp_defuse_kit);
                    ImGui::Checkbox("Planted C4", g_Options.esp_planted_c4);
                    ImGui::Checkbox("Item Esp", g_Options.esp_items);
                }
                if (SubTabVisuals == 2) {
                    ImGui::Columns(2, nullptr, false);
                    ImGui::BeginChild("##firstchild", ImVec2(0, 0)); {
                        ImGui::Checkbox("Enabled", g_Options.glow_enabled);
                        ImGui::Checkbox("Team check", g_Options.glow_enemies_only);
                        ImGui::Checkbox("Players", g_Options.glow_players);             
                    }
                    ImGui::EndChild();

                    ImGui::NextColumn();

                    ImGui::BeginChild("##secondchild", ImVec2(0, 0)); {
                        ImGui::Checkbox("Chickens", g_Options.glow_chickens);
                        ImGui::Checkbox("C4 Carrier", g_Options.glow_c4_carrier);
                        ImGui::Checkbox("Planted C4", g_Options.glow_planted_c4);
                        ImGui::Checkbox("Defuse Kits", g_Options.glow_defuse_kits);
                        ImGui::Checkbox("Weapons", g_Options.glow_weapons);
                    }
                    ImGui::EndChild();
                }
                if (SubTabVisuals == 3) {
                    ImGui::Columns(2, nullptr, false);
                    static char* chams_material[] = { "Regular", "Flat", "Glass", "Glow" };
                    static char* chams_material_arms[] = { "Regular", "Flat", "Glass", "Glow" };
                    ImGui::BeginChild("##firstchild", ImVec2(0, 0)); {
                        ImGui::Checkbox("Enabled Chams", g_Options.chams_player_enabled);
                        ImGui::Checkbox("Team Check", g_Options.chams_player_enemies_only);
                        ImGui::Combo("Material##player", g_Options.chams_material, chams_material, IM_ARRAYSIZE(chams_material));
                        ImGui::Checkbox("Wireframe", g_Options.chams_player_wireframe);
                        ImGui::Checkbox("Ignore-Z", g_Options.chams_player_ignorez);
                    }
                    ImGui::EndChild();

                    ImGui::NextColumn();

                    ImGui::BeginChild("##secondchild", ImVec2(0, 0)); {
                        ImGui::Checkbox("Enabled Arms", g_Options.chams_arms_enabled);
                        ImGui::Combo("Material##arms", g_Options.chams_material_arms, chams_material_arms, IM_ARRAYSIZE(chams_material_arms));
                        ImGui::Checkbox("Wireframe", g_Options.chams_arms_wireframe);
                        ImGui::Checkbox("Ignore-Z", g_Options.chams_arms_ignorez);                   
                    }
                    ImGui::EndChild();
                }
                if (SubTabVisuals == 4) {
                    ImGui::SliderInt("##viewmodel_fov", g_Options.viewmodel_fov, 68, 120, "Viewmodel FOV : %.1f");
                    ImGui::Checkbox("Third Person", g_Options.misc_thirdperson);
                    if (g_Options.misc_thirdperson)
                        ImGui::SliderFloat("Distance", g_Options.misc_thirdperson_dist, 0.f, 150.f);
                    ImGui::Checkbox("No hands", g_Options.misc_no_hands);
                }
                if (SubTabVisuals == 5) {
                    ImGui::Columns(3, nullptr, false);
                    ImGui::BeginChild("##firstchild", ImVec2(0, 0)); {
                        ImGui::ColorEdit4("Allies Visible", g_Options.color_esp_ally_visible, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit4("Enemies Visible", g_Options.color_esp_enemy_visible, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit4("Allies Occluded", g_Options.color_esp_ally_occluded, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit4("Enemies Occluded", g_Options.color_esp_enemy_occluded, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit4("Crosshair", g_Options.color_esp_crosshair, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit4("Dropped Weapons", g_Options.color_esp_weapons, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit4("Defuse Kit", g_Options.color_esp_defuse, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit4("Planted C4", g_Options.color_esp_c4, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit4("Item Esp", g_Options.color_esp_item, ImGuiColorEditFlags_NoInputs);
                    }
                    ImGui::EndChild();

                    ImGui::NextColumn();

                    ImGui::BeginChild("##secondchild", ImVec2(0, 0)); {
                        ImGui::ColorEdit4("Ally", g_Options.color_glow_ally, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit4("Enemy", g_Options.color_glow_enemy, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit4("Chickens", g_Options.color_glow_chickens, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit4("C4 Carrier", g_Options.color_glow_c4_carrier, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit4("Planted C4", g_Options.color_glow_planted_c4, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit4("Defuse Kits", g_Options.color_glow_defuse, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit4("Weapons", g_Options.color_glow_weapons, ImGuiColorEditFlags_NoInputs);
                    }
                    ImGui::EndChild();

                    ImGui::NextColumn();

                    ImGui::BeginChild("##thirddchild", ImVec2(0, 0)); {
                        ImGui::ColorEdit4("Ally Visible", g_Options.color_chams_player_ally_visible, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit4("Ally Occluded", g_Options.color_chams_player_ally_occluded, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit4("Enemy Visible", g_Options.color_chams_player_enemy_visible, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit4("Enemy Occluded", g_Options.color_chams_player_enemy_occluded, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit4("Arms Color Visible", g_Options.color_chams_arms_visible, ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit4("Arms Color Occluded", g_Options.color_chams_arms_occluded, ImGuiColorEditFlags_NoInputs);
                    }
                    ImGui::EndChild();
                }
            }
            if (Tabs == 3) {

            }
            if (Tabs == 4) {
                if (SubTabMisc == 1) {
                    ImGui::Checkbox("Bunny hop", g_Options.misc_bhop);
                    ImGui::Checkbox("Rank reveal", g_Options.misc_showranks);
                    ImGui::Checkbox("Watermark##hc", g_Options.misc_watermark);
                    if (ImGui::Button("Unload")) {
                        g_Unload = true;
                    }
                }
                if (SubTabMisc == 2) {
                    if (ImGui::Button("Save cfg")) {
                        Config::Get().Save();
                    }
                    if (ImGui::Button("Load cfg")) {
                        Config::Get().Load();
                    }
                }
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

void Menu::Toggle()
{
    _visible = !_visible;
}

void Menu::CreateStyle()
{
	ImGui::StyleColorsDark();
	ImGui::SetColorEditOptions(ImGuiColorEditFlags_HEX);
	_style.FrameRounding = 0.f;
	_style.WindowRounding = 0.f;
	_style.ChildRounding = 0.f;
	_style.Colors[ImGuiCol_Button] = ImVec4(0.260f, 0.590f, 0.980f, 0.670f);
	_style.Colors[ImGuiCol_Header] = ImVec4(0.260f, 0.590f, 0.980f, 0.670f);
	_style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.260f, 0.590f, 0.980f, 1.000f);
	//_style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.000f, 0.545f, 1.000f, 1.000f);
	//_style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.060f, 0.416f, 0.980f, 1.000f);
	_style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.30f, 1.0f);
	_style.Colors[ImGuiCol_WindowBg] = ImVec4(0.000f, 0.009f, 0.120f, 0.940f);
	_style.Colors[ImGuiCol_PopupBg] = ImVec4(0.076f, 0.143f, 0.209f, 1.000f);
	ImGui::GetStyle() = _style;
}

