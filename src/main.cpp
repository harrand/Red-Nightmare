#include "quad_renderer.hpp"
#include "tz/core/tz.hpp"
#include "tz/dbgui/dbgui.hpp"


int main()
{
	tz::initialise({.name = "Red Nightmare"});
	{
		game::QuadRenderer qrenderer;

		bool show_game_menu = false;
		tz::dbgui::game_menu().add_callback([&show_game_menu]()
		{
			ImGui::MenuItem("Blah", nullptr, &show_game_menu);
		});

		while(!tz::window().is_close_requested())
		{
			tz::window().begin_frame();
			qrenderer.render();

			tz::dbgui::run([&show_game_menu]()
			{
				if(show_game_menu)
				{
					ImGui::Begin("Blah", &show_game_menu);
					ImGui::Text("Herp");
					ImGui::End();
				}
			});
			tz::window().end_frame();
		}
	}
	tz::terminate();
}
