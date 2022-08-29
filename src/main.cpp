#include "scene.hpp"
#include "actor.hpp"
#include "tz/core/tz.hpp"
#include "tz/core/time.hpp"
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

		scene.add(game::ActorType::PlayerClassic);

		using namespace tz::literals;
		tz::Delay fixed_update = 1667_us;

		while(!tz::window().is_close_requested())
		{
			tz::window().begin_frame();
			scene.render();

			// Fixed update. 60fps.
			if(fixed_update.done())
			{
				scene.update();
				fixed_update.reset();
			}

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
