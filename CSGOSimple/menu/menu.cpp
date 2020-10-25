#include "Menu.hpp"
#define NOMINMAX
#include <Windows.h>
#include <chrono>

#include "../sdk/csgostructs.hpp"
#include "../helpers/input.hpp"
#include "../configs/options.hpp"
#include "../configs/config.hpp"
#include "../ui/ui.hpp"
#include "../resource/constchar.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "../imgui/imgui_internal.h"
#include "../imgui/impl/imgui_impl_dx9.h"
#include "../imgui/impl/imgui_impl_win32.h"

#pragma region CustomImGui
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
void Menu::Initialize() {
	CreateStyle();

    _visible = true;
}
void Menu::Shutdown() {
    ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
void Menu::OnDeviceLost() {
    ImGui_ImplDX9_InvalidateDeviceObjects();
}
void Menu::OnDeviceReset() {
    ImGui_ImplDX9_CreateDeviceObjects();
}
#pragma region Tabs
void legitbot_tab() {
    switch (SubTabLegit) {
    case 1:
        ImGui::Text("Here legit tab");
        break;
    case 2:
        ImGui::Text("Here trigger tab");
        break;
    }
}
void visuals_tab() {
    switch (SubTabVisuals) {
    case 1: {
        ImGui::Columns(2, nullptr, false);

        ImGui::Checkbox("Enabled", g_Options.esp_enabled);
        ImGui::Checkbox("Team check", g_Options.esp_enemies_only);
        ImGui::Checkbox("Boxes", g_Options.esp_player_boxes);
        if(g_Options.esp_player_boxes)
            ImGui::Combo("Type##boxes", g_Options.esp_player_boxes_type, esp_boxes, IM_ARRAYSIZE(esp_boxes));
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
    } break;
    case 2: {
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
    } break;
    case 3: {
        ImGui::Columns(2, nullptr, false);
       
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
    } break;
    case 4: {
        ImGui::SliderInt("##viewmodel_fov", g_Options.viewmodel_fov, 68, 120, "Viewmodel FOV : %.1f");
        ImGui::Checkbox("Third Person", g_Options.misc_thirdperson);
        if (g_Options.misc_thirdperson)
            ImGui::SliderFloat("Distance", g_Options.misc_thirdperson_dist, 0.f, 150.f);
        ImGui::Checkbox("No hands", g_Options.misc_no_hands);
        ImGui::SliderInt("##Aspect Ratio", g_Options.aspectratio, 0, 15, "Aspect Ratio: %.3f");
        ImGui::Combo("Model Ct##agents", &g_Options.agent_changer_ct, player_model_ct, IM_ARRAYSIZE(player_model_ct));
        ImGui::Combo("Model T##agents", &g_Options.agent_changer_t, player_model_t, IM_ARRAYSIZE(player_model_t));
        ImGui::Checkbox("Nade Prediction", g_Options.esp_nade_prediction);
    } break;
    case 5: {
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
            ImGui::ColorEdit4("Arms Color", g_Options.color_chams_arms, ImGuiColorEditFlags_NoInputs);
        }
        ImGui::EndChild();
    } break;
    }
}
void skins_tab() {

}
void misc_tab() {
    switch (SubTabMisc) {
    case 1: {
        ImGui::Checkbox("Bunny hop", g_Options.misc_bhop);
        ImGui::Checkbox("Rank reveal", g_Options.misc_showranks);
        ImGui::Checkbox("Watermark##wm", g_Options.misc_watermark);
        ImGui::Checkbox("Draw Info", g_Options.misc_info);
        if (ImGui::Button("Unload")) {
            g_Unload = true;
        }
    } break;
    case 2: {
        if (ImGui::Button("Save cfg")) {
            Config::Get().Save();
        }
        if (ImGui::Button("Load cfg")) {
            Config::Get().Load();
        }
    } break;
    }
}
#pragma endregion
void Menu::Render() {
	ImGui::GetIO().MouseDrawCursor = _visible;

    if(!_visible) return;

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
            switch (Tabs) {
            case 1: legitbot_tab(); break;
            case 2: visuals_tab();  break;
            case 3: skins_tab();    break;
            case 4: misc_tab();     break;
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
}
void Menu::Toggle() {
    _visible = !_visible;
}
void Menu::CreateStyle() {
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

