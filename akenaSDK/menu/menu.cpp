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
#include "../features/notify/notify.h"

#pragma region Tabs
void legitbot_sub() {
    for (int i = 0; i < IM_ARRAYSIZE(sub_legit); i++) {
        if (ImGui::MenuSubTab(sub_legit[i].c_str(), ImVec2(110, 30), SubTabLegit == i)) {
            SubTabLegit = i;
        }
        ImGui::SameLine();
    }
}
void legitbot_tab() {
    legitbot_sub();

    switch (SubTabLegit) {
    case 0:
        ImGui::Text("Here legit tab");
        break;
    case 1:
        ImGui::Text("Here trigger tab");
        break;
    }
}
//-----------------
void visuals_sub() {
    for (int i = 0; i < IM_ARRAYSIZE(sub_visuals); i++) {
        if (ImGui::MenuSubTab(sub_visuals[i].c_str(), ImVec2(110, 30), SubTabVisuals == i)) {
            SubTabVisuals = i;
        }
        ImGui::SameLine();
    }
}
void visuals_tab() {
    visuals_sub();

    switch (SubTabVisuals) {
    case 0: {
        ImGui::Columns(2, nullptr, false);

        ImGui::Checkbox("Enabled", g_Options.esp_enabled);
        ImGui::Checkbox("Team check", g_Options.esp_enemies_only);
        ImGui::Checkbox("Boxes", g_Options.esp_player_boxes);
        if (g_Options.esp_player_boxes)
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
    case 1: {
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
    case 2: {
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
            ImGui::Combo("Material##arms", g_Options.chams_material_arms, chams_material, IM_ARRAYSIZE(chams_material));
            ImGui::Checkbox("Enabled Weapon", g_Options.chams_weapon_enabled);
            ImGui::Combo("Material##weapon", g_Options.chams_material_weapon, chams_material, IM_ARRAYSIZE(chams_material));
            ImGui::Checkbox("Enabled Sleeve", g_Options.chams_sleeve_enabled);
            ImGui::Combo("Material##sleeve", g_Options.chams_material_sleeve, chams_material, IM_ARRAYSIZE(chams_material));
        }
        ImGui::EndChild();
    } break;
    case 3: {
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
    case 4: {
        ImGui::Columns(3, nullptr, false);
        ImGui::BeginChild("##firstchild", ImVec2(0, 0)); {
            ImGui::Text("Esp");
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
            ImGui::Text("Glow");
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
            ImGui::Text("Chams");
            ImGui::ColorEdit4("Ally Visible", g_Options.color_chams_player_ally_visible, ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Ally Occluded", g_Options.color_chams_player_ally_occluded, ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Enemy Visible", g_Options.color_chams_player_enemy_visible, ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Enemy Occluded", g_Options.color_chams_player_enemy_occluded, ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Arms color", g_Options.color_chams_arms, ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Weapon color", g_Options.color_chams_weapon, ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Sleeve color", g_Options.color_chams_sleeve, ImGuiColorEditFlags_NoInputs);
        }
        ImGui::EndChild();
    } break;
    }
}
//-----------------
void skins_sub() {
    for (int i = 0; i < IM_ARRAYSIZE(sub_skins); i++) {
        if (ImGui::MenuSubTab(sub_skins[i].c_str(), ImVec2(110, 30), SubTabSkins == i)) {
            SubTabSkins = i;
        }
        ImGui::SameLine();
    }
}
void skins_tab() {
    skins_sub();
}
//-----------------
void misc_sub() {
    for (int i = 0; i < IM_ARRAYSIZE(sub_misc); i++) {
        if (ImGui::MenuSubTab(sub_misc[i].c_str(), ImVec2(110, 30), SubTabMisc == i)) {
            SubTabMisc = i;
        }
        ImGui::SameLine();
    }
}
void misc_tab() {
    misc_sub();

    switch (SubTabMisc) {
    case 0: {
        ImGui::Checkbox("Bunny hop", g_Options.misc_bhop);
        if (g_Options.misc_bhop)
            ImGui::Checkbox("Null Strafe", g_Options.misc_null_strafe);

        ImGui::Checkbox("Rank reveal", g_Options.misc_showranks);
        ImGui::Checkbox("Watermark##wm", g_Options.misc_watermark);
        ImGui::Checkbox("Draw Info", g_Options.misc_info);
        if (ImGui::Button("Unload")) {
            g_Unload = true;
        }
        //just for test
        if (ImGui::Button("type 1")) {
            notify::add("injected", log_type::screen_standart);
        }
        ImGui::SameLine();
        if (ImGui::Button("type 1 + timer")) {
            notify::add("injected", g_Options.notify_timer);
        }

        if (ImGui::Button("type 2")) {
            notify::add("akenaSDK", "injected", log_type::screen_standart);
        }
        ImGui::SameLine();
        if (ImGui::Button("type 2 + timer")) {
            notify::add("akenaSDK", "injected", g_Options.notify_timer);
        }

        ImGui::ColorEdit3("Custom Color##menu_color", g_Options.menu_color, ImGuiColorEditFlags_NoInputs);
        ImGui::SliderInt("##Timer", g_Options.notify_timer, 0, 10000, "Timer : %.1f");

    } break;
    case 1: {
        if (ImGui::Button("Save cfg")) {
            notify::add("config saved", log_type::screen_standart);
        }
        if (ImGui::Button("Load cfg")) {
            notify::add("config loaded", log_type::screen_standart);
        }
    } break;
    }
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
void Menu::Render() {
	ImGui::GetIO().MouseDrawCursor = _visible;

    if(!_visible) return;

    ImGui::SetNextWindowSizeConstraints(ImVec2(730, 480), ImVec2(730, 480));
    if (ImGui::Begin("testgui", &_visible, flags)) {
        if (ImGui::BeginChild("Child1", ImVec2(-1, 5), false, flags)) {
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x, p.y + 3), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + -3), ImColor(137, 50, 168));
            ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x, p.y + 3), ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y + -3), ImColor(0, 0, 0, 125), ImColor(0, 0, 0, 15), ImColor(0, 0, 0, 15), ImColor(0, 0, 0, 125));
            ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y + 3), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + -3), ImColor(0, 0, 0, 15), ImColor(0, 0, 0, 125), ImColor(0, 0, 0, 125), ImColor(0, 0, 0, 15));
            ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x, p.y + 2), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + 2), ImColor(0, 0, 0, 150));
        }
        ImGui::EndChild();

        if (ImGui::BeginChild("Child2", ImVec2(100, 0), false, flags)) {
            for (int i = 0; i < IM_ARRAYSIZE(tabs); i++) {
                if (ImGui::MenuTab(tabs[i].c_str(), Tabs == i, ImVec2(100, 30))) {
                    Tabs = i;
                }
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        if (ImGui::BeginChild("Child3", ImVec2(0, 0), false, flags)) {
            switch (Tabs) {
            case 0: legitbot_tab(); break;
            case 1: visuals_tab();  break;
            case 2: skins_tab();    break;
            case 3: misc_tab();     break;
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
}