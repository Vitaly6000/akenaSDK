#include "notify.h"

#include <map>
#include <algorithm>
#include <math.h>
#include <chrono>

std::deque<notify_t> notifications_standart;
std::deque<notify_t> notifications_big;

namespace notify {
	void add(std::string pre, std::string body, Color color_pre, Color color_body, int life_time, log_type type) {
		switch (type) {
		case console:
			if (!g_EngineClient || !g_CVar) return;

			g_CVar->ConsoleDPrintf("[ ");
			g_CVar->ConsoleColorPrintf(color_pre, pre.data());
			g_CVar->ConsoleDPrintf(" ] ");
			g_CVar->ConsoleColorPrintf(color_body, body.data());
			g_CVar->ConsoleDPrintf("\n");
			break;
		case screen_standart:
			notifications_standart.push_front(notify_t{ pre, body, color_pre, color_body, (float)life_time });
			break;
		case screen_big:
			notifications_big.push_front(notify_t{ "", body, color_pre, Color(0,0,0,0), (float)life_time });
			break;
		}
	}

	void add(std::string body, int life_time) {
		add("", body, Color(g_Options.menu_color), Color::White, life_time, log_type::screen_big);
	}

	void add(std::string body) {
		add("", body, Color(g_Options.menu_color), Color::White, 2000.f, log_type::screen_big);
	}

	void add(std::string pre, std::string body) {
		add(pre, body, Color(g_Options.menu_color), Color::White, 2000.f, log_type::screen_standart);
	}

	void add(std::string pre, std::string body, int life_time) {
		add(pre, body, Color(g_Options.menu_color), Color::White, life_time, log_type::screen_standart);
	}

	void add(std::string pre, std::string body, Color color_pre, Color color_body) {
		add(pre, body, color_pre, color_body, 2000.f, log_type::screen_standart);
	}

	void add(std::string pre, std::string body, Color color_pre, int life_time, log_type type) {
		add(pre, body, color_pre, Color::White, life_time, type);
	}

	void add(std::string pre, std::string body, Color color_pre, log_type type) {
		add(pre, body, color_pre, Color::White, 2000.f, type);
	}

	void add(std::string pre, std::string body, log_type type) {
		add(pre, body, Color(g_Options.menu_color), Color::White, 2000.f, type);
	}

	void render() {
		int screen_w, screen_h;
		g_EngineClient->GetScreenSize(screen_w, screen_h);
		if (!notifications_standart.empty()) {
			for (size_t i = 0; i < notifications_standart.size(); i++) {
				auto& notify = notifications_standart.at(i);

				int left_size = g_pSecondFont->CalcTextSizeA(12.f, FLT_MAX, 0.f, "[ ").x;
				int right_size = g_pSecondFont->CalcTextSizeA(12.f, FLT_MAX, 0.f, " ]").x;
				int space_size = g_pSecondFont->CalcTextSizeA(12.f, FLT_MAX, 0.f, "  ").x;

				std::string all_text = Utils::printf("[ %s ] %s", notify.s_pre.c_str(), notify.s_body.c_str());

				ImVec2 pre_size = g_pSecondFont->CalcTextSizeA(12.f, FLT_MAX, 0.0f, notify.s_pre.c_str());
				ImVec2 all_text_size = g_pSecondFont->CalcTextSizeA(12.f, FLT_MAX, 0.f, all_text.c_str());
				notify.max_x = all_text_size.x + 10;

				notify.y = Utils::lerp(notify.y, i * (all_text_size.y + 3), 0.03f);

				if (notify.y > screen_h + 10 || notify.x_main < -3) {
					notifications_standart.erase(notifications_standart.begin() + i);
					continue;
				}

				if (notify.time_life == 0 || notify.time_life >= Utils::epoch_time()) {
					notify.x_main = Utils::lerp(notify.x_main, notify.max_x + 5, 0.06f);

					if (notify.x_main > notify.max_x - (notify.max_x / 3)) {
						notify.x_body = Utils::lerp(notify.x_body, notify.max_x + 3, 0.05f);

						if (notify.x_body >= notify.max_x && notify.time_life == 0) {
							notify.time_life = Utils::epoch_time() + notify.time_del;
						}
					}
				}
				else {
					notify.x_body = Utils::lerp(notify.x_body, -20, 0.06f);

					if (notify.x_body < notify.max_x / 3) {
						notify.x_main = Utils::lerp(notify.x_main, -20, 0.07f);
					}
				}

				int procent_x = std::clamp((100 * notify.x_body) / notify.max_x, 0.f, 100.f);
				int opacity = (255.f / 100.f) * procent_x;

				notify.c_body = Color(notify.c_body, opacity);
				notify.c_pre = Color(notify.c_pre, opacity);

				int start_text = notify.x_body - all_text_size.x - 7;

				Render::Get().RenderBoxFilled(0.f, notify.y, notify.x_body, notify.y + all_text_size.y + 3, Color(0, 0, 0, 100));
				Render::Get().RenderBoxFilled(notify.x_body, notify.y, notify.x_main, notify.y + all_text_size.y + 3, Color(notify.c_pre, 255));

				Render::Get().RenderText("[", ImVec2(start_text, notify.y + 2), 12.f, Color(255, 255, 255, 255), false, false);
				Render::Get().RenderText(" ]", ImVec2(start_text + pre_size.x + left_size + 1, notify.y + 1), 12.f, Color(255, 255, 255, 255), false, false);
				Render::Get().RenderText(notify.s_pre, ImVec2(start_text + left_size + 1, notify.y + 2), 12.f, notify.c_pre, false, false);
				Render::Get().RenderText(notify.s_body, ImVec2(start_text + left_size + right_size + space_size + pre_size.x + 2, notify.y + 1), 12.f, notify.c_body, false, false);
			}
		}
		if (!notifications_big.empty()) {
			for (size_t i = 0; i < notifications_big.size(); i++) {
				auto& notify = notifications_big.at(i);

				float round = ImGui::GetStyle().FrameRounding;

				ImVec2 text_size = g_pSecondFont->CalcTextSizeA(12.f, FLT_MAX, 0.0f, notify.s_body.c_str());

				float box_size_x = 50.f + text_size.x;
				float box_size_y = 40.f;

				float procent_x = 100;

				if (notify.x_body == 0 && notify.y == 0) {
					notify.x_body = screen_w + box_size_x;
					notify.y = screen_h + box_size_y;
				}

				float start_pos_x = screen_w - 100.f;
				float start_pos_y = screen_h - (i * box_size_y) - (i * 15.f) - 50.f;

				if (notify.time_life == 0 || notify.time_life >= Utils::epoch_time()) {
					notify.x_body = Utils::lerp(notify.x_body, start_pos_x, 0.03f);
					notify.y = Utils::lerp(notify.y, start_pos_y, 0.03f);

					if (notify.time_life == 0 && notify.x_body < start_pos_x + 2 && notify.y < start_pos_y + 2)
						notify.time_life = Utils::epoch_time() + notify.time_del;
				}
				else notify.x_body = Utils::lerp(notify.x_body, screen_w + box_size_x + 40, 0.03f);

				if (notify.x_body > screen_w + box_size_x + 4 || notify.y < 0) {
					notifications_big.erase(notifications_big.begin() + i);
					continue;
				}

				if (notify.time_life != 0) {
					procent_x = (100 * (notify.time_life - Utils::epoch_time())) / notify.time_del;
				}

				float opacity = std::clamp((box_size_y / 100.f) * procent_x, 0.f, box_size_y);

				Render::Get().RenderBoxFilled(notify.x_body - box_size_x, notify.y - box_size_y, notify.x_body, notify.y, Color(50, 50, 50, 200), 0.f, round);
				Render::Get().RenderBoxFilled(notify.x_body - box_size_x - 4.f, notify.y - box_size_y, notify.x_body - box_size_x, notify.y, Color(notify.c_pre, 255), 1.f, round);
				Render::Get().RenderBoxFilled(notify.x_body - box_size_x, notify.y - opacity, notify.x_body - box_size_x + 2.f, notify.y, Color(notify.c_pre, 255));

				Render::Get().RenderText(notify.s_body, notify.x_body - (box_size_x / 2), notify.y - (box_size_y / 2) - (text_size.y / 2), 12, Color(255, 255, 255, 255), true, false);
			}
		}
	}
}
