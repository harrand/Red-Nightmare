#include "gamelib/rnlib.hpp"
#include "gamelib/renderer/scene_renderer.hpp"
#include "tz/core/debug.hpp"
#include <memory>

namespace game
{
	struct dbgui_data_t
	{

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
	}

	void terminate()
	{
		game_system = nullptr;
	}

	void update(std::uint64_t delta_micros)
	{
		tz::gl::get_device().render();
	}

	void fixed_update(std::uint64_t delta_micros, std::uint64_t unprocessed)
	{

	}

	void dbgui()
	{

	}

	void dbgui_game_bar()
	{
		ImGui::Text("Well met!");
	}
}