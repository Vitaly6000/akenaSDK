#pragma once

#include <string>
#include "../helpers/singleton.hpp"
#include "../imgui/imgui.h"

struct IDirect3DDevice9;

class Menu : public Singleton<Menu> {
public:
    void Initialize();
    void Shutdown();

    void OnDeviceLost();
    void OnDeviceReset();

    void Render();

    void Toggle();

    bool IsVisible() const { return _visible; }
   
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;
private:
    void CreateStyle();

    ImGuiStyle        _style;
    bool              _visible;
};