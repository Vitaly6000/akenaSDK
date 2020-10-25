#include "options.hpp"
#include "base64.h"
#include "../helpers/utils.hpp"
#include <shlobj.h>

std::unordered_map <std::string, float[4]> colors;
C_ConfigManager* cfg_manager = new C_ConfigManager();

void C_ConfigManager::setup() {

}

void C_ConfigManager::add_item(void* pointer, const char* name, const std::string& type) {
	items.push_back(new C_ConfigItem(std::string(name), pointer, type));
}

void C_ConfigManager::setup_item(int* pointer, int value, const std::string& name) {
	add_item(pointer, name.c_str(), ("int"));
	*pointer = value;
}

void C_ConfigManager::setup_item(bool* pointer, bool value, const std::string& name) {
	add_item(pointer, name.c_str(), ("bool"));
	*pointer = value;
}

void C_ConfigManager::setup_item(float* pointer, float value, const std::string& name) {
	add_item(pointer, name.c_str(), ("float"));
	*pointer = value;
}

void C_ConfigManager::setup_item(Color* pointer, Color value, const std::string& name) {
	colors[name][0] = (float)value.r() / 255.0f;
	colors[name][1] = (float)value.g() / 255.0f;
	colors[name][2] = (float)value.b() / 255.0f;
	colors[name][3] = (float)value.a() / 255.0f;

	add_item(pointer, name.c_str(), ("Color"));
	*pointer = value;
}

void C_ConfigManager::setup_item(std::vector< int >* pointer, int size, const std::string& name) {
	add_item(pointer, name.c_str(), ("vector<int>"));
	pointer->clear();

	for (int i = 0; i < size; i++)
		pointer->push_back(FALSE);
}

void C_ConfigManager::setup_item(std::vector< std::string >* pointer, const std::string& name) {
	add_item(pointer, name.c_str(), ("vector<string>"));
}

void C_ConfigManager::setup_item(std::string* pointer, const std::string& value, const std::string& name) {
	add_item(pointer, name.c_str(), ("string"));
	*pointer = value;
}

void C_ConfigManager::save(std::string config) {

}

void C_ConfigManager::load(std::string config, bool load_script_items) {

}

void C_ConfigManager::remove(std::string config) {

}

void C_ConfigManager::config_files() {

}