#pragma once

#include <string>
#include <deque>

#include "../../render/render.hpp"
#include "../../sdk/sdk.hpp"
#include "../../configs/options.hpp"

struct notify_t {
	std::string s_pre;
	std::string s_body;
	Color c_pre;
	Color c_body;

	float time_del;
	float time_life = 0;

	float x_body = 0;
	float x_main = 0;
	float y = 0;

	float max_x;
};

enum log_type {
	console = 0,
	screen_standart = 1,
	screen_big = 2
};

namespace notify {
	void add(std::string pre, std::string body, Color color_pre, Color color_body, int life_time, log_type type);
	void add(std::string pre, std::string body, Color color_pre, int life_time, log_type type);
	void add(std::string pre, std::string body, Color color_pre, log_type type);
	void add(std::string pre, std::string body, log_type type);
	void add(std::string pre, std::string body, Color color_pre, Color color_body = Color(255, 255, 255, 255));
	void add(std::string pre, std::string body, int life_time);
	void add(std::string pre, std::string body);
	void add(std::string body, int life_time);
	void add(std::string body);

	void render();
}