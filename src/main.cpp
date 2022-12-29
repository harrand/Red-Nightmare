#include "scene.hpp"
#include "actor.hpp"
#include "game.hpp"
#include "tz/core/time.hpp"
#include "tz/dbgui/dbgui.hpp"

int main()
{
	game::initialise();
	{
		game::Scene scene;

		struct DbguiData
		{
			bool show_current_scene = false;
			bool show_story_levels = false;
			bool show_procedural_level = false;
		} dd;
		tz::dbgui::game_menu().add_callback([&dd]()
		{
			ImGui::MenuItem("Current Scene", nullptr, &dd.show_current_scene);
			ImGui::MenuItem("Story Levels", nullptr, &dd.show_story_levels);
			ImGui::MenuItem("Procedural Level", nullptr, &dd.show_procedural_level);
		});

		scene.load_zone(game::StoryZone::BlanchfieldGraveyard);

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

			tz::dbgui::run([&dd, &scene]()
			{
				if(dd.show_current_scene)
				{
					ImGui::Begin("Current Scene", &dd.show_current_scene);
					scene.dbgui_current_scene();
					ImGui::End();
				}
				if(dd.show_story_levels)
				{
					ImGui::Begin("Story Levels", &dd.show_story_levels);
					scene.dbgui_story_levels();
					ImGui::End();
				}
				if(dd.show_procedural_level)
				{
					ImGui::Begin("Level Generator", &dd.show_procedural_level);
					scene.dbgui_procedural_level();
					ImGui::End();
				}
			});

			tz::window().end_frame();
		}
	}
	game::terminate();
}
