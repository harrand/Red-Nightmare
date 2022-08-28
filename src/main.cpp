#include "scene.hpp"
#include "tz/core/tz.hpp"
#include "tz/dbgui/dbgui.hpp"


int main()
{
	tz::initialise({.name = "Red Nightmare"});
	{
		game::Scene scene;

		bool show_game_menu = false;
		tz::dbgui::game_menu().add_callback([&show_game_menu]()
		{
			ImGui::MenuItem("Scene", nullptr, &show_game_menu);
		});

		while(!tz::window().is_close_requested())
		{
			tz::window().begin_frame();
			scene.render();

			tz::dbgui::run([&show_game_menu, &scene]()
			{
				if(show_game_menu)
				{
					ImGui::Begin("Scene", &show_game_menu);
					scene.dbgui();
					ImGui::End();
				}
			});

			tz::window().end_frame();
		}
	}
	tz::terminate();
}
