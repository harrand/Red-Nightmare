#include "gamelib/rnlib.hpp"
#include "gamelib/audio.hpp"
#include "tz/core/debug.hpp"
#include "tz/core/profile.hpp"
#include "tz/core/imported_text.hpp"
#include "tz/lua/api.hpp"
#include <memory>
#include <filesystem>

// new stuff.
#include "tz/ren/mesh.hpp"
#include "gamelib/messaging/scene.hpp"

namespace game
{
	void lua_initialise();
	struct dbgui_data_t
	{
		bool display_scene = false;
		bool display_animation_renderer = false;
	};
	struct game_system_t
	{
		game::scene scene2;
		dbgui_data_t dbgui;

		tz::ren::mesh_renderer placeholder_renderer;
	};

	std::unique_ptr<game_system_t> game_system = nullptr;

	void initialise()
	{
		TZ_PROFZONE("rnlib - initialise", 0xFF00AAFF);
		game_system = std::make_unique<game_system_t>();
		tz::gl::get_device().set_vsync_enabled(true);
		lua_initialise();
		audio_initialise();

		// need to render something...
		game_system->placeholder_renderer.append_to_render_graph();

		for(std::size_t i = 0; i < 128; i++)
		{
			game_system->scene2.add_entity(i);
		}
	}

	void terminate()
	{
		audio_terminate();
		game_system = nullptr;
	}

	void update(std::uint64_t delta_micros)
	{
		TZ_PROFZONE("rnlib - update", 0xFF00AAFF);
		float delta_seconds = delta_micros / 1000000.0f;

		if(game_system->dbgui.display_scene)
		{
			if(ImGui::Begin("Scene", &game_system->dbgui.display_scene))
			{
				//game_system->scene.dbgui();
				ImGui::End();
			}
		}

		if(game_system->dbgui.display_animation_renderer)
		{
			if(ImGui::Begin("Renderer", &game_system->dbgui.display_animation_renderer))
			{
				//game_system->scene.get_renderer().dbgui();
				ImGui::End();
			}
		}
		game_system->scene2.update(delta_seconds);
		tz::gl::get_device().render();
		game_system->scene2.block();
		game::messaging::scene_messaging_update();
	}

	void fixed_update(std::uint64_t delta_micros, std::uint64_t unprocessed)
	{
		float delta_seconds = delta_micros / 1000000.0f;

		game_system->scene2.fixed_update(delta_seconds, unprocessed);
	}

	void dbgui()
	{
		ImGui::MenuItem("Scene", nullptr, &game_system->dbgui.display_scene);
		ImGui::MenuItem("Animation Renderer", nullptr, &game_system->dbgui.display_animation_renderer);
	}

	void dbgui_game_bar()
	{
		//game_system->scene.dbgui_game_bar();
	}

	// called directly from initialise.
	void lua_initialise()
	{
		TZ_PROFZONE("rnlib - lua initialise", 0xFF00AAFF);
		audio_lua_initialise();
		game::messaging::scene_messaging_lua_initialise(game_system->scene2); // rn.current_scene()
		game::entity_lua_initialise(); // rn.entity.*
	}
}
