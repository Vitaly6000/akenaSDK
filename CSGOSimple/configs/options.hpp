#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include "../sdk/Misc/Color.hpp"
#include "../helpers/json.hpp"

#define A( s ) #s
#define OPTION(type, var, val) Var<type> var = {A(var), val}

template <typename T = bool>
class Var {
public:
	std::string name;
	std::shared_ptr<T> value;
	int32_t size;
	Var(std::string name, T v) : name(name) {
		value = std::make_shared<T>(v);
		size = sizeof(T);
	}
	operator T() { return *value; }
	operator T*() { return &*value; }
	operator T() const { return *value; }
	//operator T*() const { return value; }
};

using json = nlohmann::json;

class C_ConfigManager
{
public:
	class C_ConfigItem
	{
	public:
		std::string name;
		void* pointer;
		std::string type;

		C_ConfigItem(std::string name, void* pointer, std::string type)  //-V818
		{
			this->name = name; //-V820
			this->pointer = pointer;
			this->type = type; //-V820
		}
	};

	void add_item(void* pointer, const char* name, const std::string& type);
	void setup_item(int*, int, const std::string&);
	void setup_item(bool*, bool, const std::string&);
	void setup_item(float*, float, const std::string&);
	void setup_item(Color*, Color, const std::string&);
	void setup_item(std::vector< int >*, int, const std::string&);
	void setup_item(std::vector< std::string >*, const std::string&);
	void setup_item(std::string*, const std::string&, const std::string&);

	std::vector <C_ConfigItem*> items;

	C_ConfigManager()
	{
		setup();
	};

	void setup();
	void save(std::string config);
	void load(std::string config, bool load_script_items);
	void remove(std::string config);
	std::vector<std::string> files;
	void config_files();
};

extern C_ConfigManager* cfg_manager;

class Options
{
public:
		// 
		// ESP
		// 
		OPTION(bool, esp_enabled, false);
		OPTION(bool, esp_enemies_only, false);
		OPTION(bool, esp_player_boxes, false);
		OPTION(int, esp_player_boxes_type, 0);
		OPTION(bool, esp_player_names, false);
		OPTION(bool, esp_player_health, false);
		OPTION(bool, esp_player_armour, false);
		OPTION(bool, esp_player_weapons, false);
		OPTION(bool, esp_player_snaplines, false);
		OPTION(bool, esp_crosshair, false);
		OPTION(bool, esp_dropped_weapons, false);
		OPTION(bool, esp_defuse_kit, false);
		OPTION(bool, esp_planted_c4, false);
		OPTION(bool, esp_items, false);

		// 
		// GLOW
		// 
		OPTION(bool, glow_enabled, false);
		OPTION(bool, glow_enemies_only, false);
		OPTION(bool, glow_players, false);
		OPTION(bool, glow_chickens, false);
		OPTION(bool, glow_c4_carrier, false);
		OPTION(bool, glow_planted_c4, false);
		OPTION(bool, glow_defuse_kits, false);
		OPTION(bool, glow_weapons, false);

		//
		// CHAMS
		//
		OPTION(bool, chams_player_enabled, false);
		OPTION(bool, chams_player_enemies_only, false);
		OPTION(bool, chams_player_wireframe, false);
		OPTION(bool, chams_player_flat, false);
		OPTION(bool, chams_player_ignorez, false);
		OPTION(bool, chams_player_glass, false);

		OPTION(bool, chams_arms_enabled, false);
		OPTION(bool, chams_weapon_enabled, false);
		OPTION(bool, chams_sleeve_enabled, false);

		OPTION(int, chams_material, 0);
		OPTION(int, chams_material_arms, 0);
		OPTION(int, chams_material_weapon, 0);
		OPTION(int, chams_material_sleeve, 0);

		//
		// MISC
		//
		OPTION(bool, misc_bhop, false);
		OPTION(bool, misc_no_hands, false);
		OPTION(bool, misc_thirdperson, false);
		OPTION(bool, misc_showranks, true);
		OPTION(bool, misc_watermark, true);
		OPTION(bool, misc_info, false);	
		OPTION(float, misc_thirdperson_dist, 50.f);
		OPTION(int, viewmodel_fov, 68);
		OPTION(int, aspectratio, 0);		
		OPTION(bool, esp_nade_prediction, false);

		/*
		OPTION(int, agent_changer_ct, 0);
		OPTION(int, agent_changer_t, 0);
		*/

		int agent_changer_ct{ 0 };
		int agent_changer_t{ 0 };

		// 
		// COLORS
		// 
		float color_esp_ally_visible[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_esp_enemy_visible[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_esp_ally_occluded[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_esp_enemy_occluded[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_esp_crosshair[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_esp_weapons[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_esp_defuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_esp_c4[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_esp_item[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		float color_glow_ally[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_glow_enemy[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_glow_chickens[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_glow_c4_carrier[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_glow_planted_c4[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_glow_defuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_glow_weapons[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		float color_chams_player_ally_visible[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_chams_player_ally_occluded[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_chams_player_enemy_visible[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_chams_player_enemy_occluded[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_chams_arms[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_chams_sleeve[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float color_chams_weapon[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
};

inline Options g_Options;
inline bool   g_Unload;
