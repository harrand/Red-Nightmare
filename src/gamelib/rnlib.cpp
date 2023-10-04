#include "gamelib/rnlib.hpp"
#include "gamelib/renderer/scene_renderer.hpp"
#include "tz/core/debug.hpp"
#include <memory>

namespace game
{
	struct dbgui_data_t
	{
		bool display_animation_renderer = false;
	};
	struct game_system_t
	{
		game::render::scene_renderer sceneren;
		dbgui_data_t dbgui;
	};

	std::unique_ptr<game_system_t> game_system = nullptr;

	void initialise()
	{
		game_system = std::make_unique<game_system_t>();

		game_system->sceneren.add_model(game::render::scene_renderer::model::humanoid);
		game_system->sceneren.add_model(game::render::scene_renderer::model::quad);
	}

	void terminate()
	{
		game_system = nullptr;
	}

	void update(std::uint64_t delta_micros)
	{
		float delta_seconds = delta_micros / 1000000.0f;
		game_system->sceneren.update(delta_seconds);

		if(game_system->dbgui.display_animation_renderer)
		{
			if(ImGui::Begin("Scene Renderer", &game_system->dbgui.display_animation_renderer))
			{
				game_system->sceneren.dbgui();
				ImGui::End();
			}
		}
		tz::gl::get_device().render();
	}

	void fixed_update(std::uint64_t delta_micros, std::uint64_t unprocessed)
	{
	}

	void dbgui()
	{
		ImGui::MenuItem("Animation Renderer", nullptr, &game_system->dbgui.display_animation_renderer);
	}

	void dbgui_game_bar()
	{
		ImGui::Text("Well met!");
	}
}