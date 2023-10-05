#include "gamelib/rnlib.hpp"
#include "gamelib/renderer/scene_renderer.hpp"
#include "tz/core/debug.hpp"
#include "tz/lua/api.hpp"
#include <memory>

namespace game
{
	void lua_initialise();
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
		lua_initialise();

		// lua version:
		tz::lua::get_state().execute(R"(
			ren = rn.get_scene_renderer()
			e0 = ren:add_model(1)

			e0:play_animation(0, false)
			for i=1,e0:get_animation_count()-1,1 do
				e0:queue_animation(i, false)
			end
		)");
		tz::io::image img
		{
			.width = 2u,
			.height = 2u,
			.data = 
			{
				std::byte{255},
				std::byte{255},
				std::byte{255},
				std::byte{255},

				std::byte{255},
				std::byte{0},
				std::byte{255},
				std::byte{255},

				std::byte{255},
				std::byte{0},
				std::byte{255},
				std::byte{255},

				std::byte{255},
				std::byte{255},
				std::byte{255},
				std::byte{255},
			}
		};
		tz::ren::animation_renderer::texture_handle th = game_system->sceneren.get_renderer().add_texture(img);
		tz::report("new texture: %zu", static_cast<std::size_t>(static_cast<tz::hanval>(th)));

		// c++ version:
		//auto e = game_system->sceneren.add_model(game::render::scene_renderer::model::humanoid);
		//game::render::scene_element ele = game_system->sceneren.get_element(e);
		//ele.play_animation(0);
		//for(std::size_t i = 1; i < ele.get_animation_count(); i++)
		//{
		//	ele.queue_animation(i);
		//}

		//game_system->sceneren.add_model(game::render::scene_renderer::model::quad);
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

	// called directly from initialise.
	void lua_initialise()
	{
		tz::lua::for_all_states([](tz::lua::state& state)
		{
			game_system->sceneren.lua_initialise(state);
		});
	}
}