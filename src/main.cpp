#include "scene.hpp"
#include "actor.hpp"
#include "tz/core/tz.hpp"
#include "tz/core/time.hpp"
#include "tz/dbgui/dbgui.hpp"

int main()
{
	tz::initialise({.name = "Red Nightmare", .version = {2, 0, 0}});
	{
		game::Scene scene;

		bool show_current_scene = false;
		tz::dbgui::game_menu().add_callback([&show_current_scene]()
		{
			ImGui::MenuItem("Current Scene", nullptr, &show_current_scene);
		});

		scene.add(game::ActorType::PlayerClassic);
		scene.add(game::ActorType::PlayerClassic_Orb);
		scene.add(game::ActorType::EvilPlayer_TestSpawner);

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

			tz::dbgui::run([&show_current_scene, &scene]()
			{
				if(show_current_scene)
				{
					ImGui::Begin("Current Scene", &show_current_scene);
					scene.dbgui();
					ImGui::End();
				}
			});

			tz::window().end_frame();
		}
	}
	tz::terminate();
}
