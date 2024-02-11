#include "gamelib/input/kbm.hpp"
#include "gamelib/messaging/scene.hpp"
#include "gamelib/rnlib.hpp"
#include "gamelib/audio.hpp"
#include "tz/ren/mesh.hpp"
#include "tz/lua/api.hpp"
#include "tz/core/debug.hpp"
#include "tz/core/profile.hpp"
#include "tz/core/imported_text.hpp"
#include "tz/core/imported_text.hpp"
#include <memory>
#include <filesystem>

#include ImportedTextHeader(game, lua)
#include ImportedTextHeader(level, lua)
#include ImportedTextHeader(mod, lua)
#include ImportedTextHeader(spell, lua)

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
	};

	std::unique_ptr<game_system_t> game_system = nullptr;

	void initialise()
	{
		TZ_PROFZONE("rnlib - initialise", 0xFF00AAFF);
		game_system = std::make_unique<game_system_t>();
		#if !TZ_PROFILE
			tz::gl::get_device().set_vsync_enabled(true);
		#endif
		game::messaging::set_current_scene(game_system->scene2);
		lua_initialise();
		audio_initialise();
		game::input::input_initialise();

		// add default models...
		// try not to add too many. mods should be responsible for adding the models they need. default models should only be for the most obvious things (like a plane for a 2d sprite)
		//game_system->scene2.get_renderer().add_model("plane", tz::io::gltf::from_memory(ImportedTextData(plane, glb)));
		// and default textures...
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
				game_system->scene2.dbgui();
				ImGui::End();
			}
		}

		if(game_system->dbgui.display_animation_renderer)
		{
			if(ImGui::Begin("Renderer", &game_system->dbgui.display_animation_renderer))
			{
				game_system->scene2.get_renderer().dbgui();
				ImGui::End();
			}
		}

		game::input::input_advance();
		game_system->scene2.update(delta_seconds);
		tz::gl::get_device().render();
		{
			std::string update_src = std::format("rn.update({})", delta_seconds);
			tz::lua::get_state().execute(update_src.c_str());
		}
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
		std::string bar_string;
		bar_string = std::format("{} entities | {} intersections", game_system->scene2.entity_count(), game_system->scene2.get_intersections().size());
		ImGui::Text("%s", bar_string.c_str());
	}

	// called directly from initialise.
	void lua_initialise()
	{
		TZ_PROFZONE("rnlib - lua initialise", 0xFF00AAFF);

		// generate lua code to include all mods.
		std::string lua_require_cmd = "";
		#if TZ_DEBUG
			std::string cwd = std::filesystem::current_path().string();
			tz::assert(std::filesystem::exists("./mods"), "./mods directory not found. Current path: \"%s\"", cwd.c_str());
		#endif
		constexpr const char mods_folder[] = "./mods";
		for(const auto& entry : std::filesystem::recursive_directory_iterator(mods_folder))
		{
			// stem is just filename without extension (which is what require likes)
			if(entry.path().extension() != ".lua")
			{
				// dont try to require non .lua files
				continue;
			}
			std::string path = entry.path().string();
			std::string suffix_to_remove = entry.path().extension().string();
			auto iter = path.find(mods_folder);
			path.erase(iter, sizeof(mods_folder));
			path.erase(path.find(suffix_to_remove), suffix_to_remove.size());
			// single backslashes detected in path, which is bad.
			// note: linux might possibly prefer double backslash instead of single forward slash. windows doesn't care.
			std::replace(path.begin(), path.end(), '\\', '/');
			lua_require_cmd += std::format("require(\"{}\")", path);
		}

		std::string game_lua_src{ImportedTextData(game, lua)};
		std::string level_lua_src{ImportedTextData(level, lua)};
		std::string mod_lua_src{ImportedTextData(mod, lua)};
		std::string spell_lua_src{ImportedTextData(spell, lua)};

		tz::lua::for_all_states([&lua_require_cmd, &mod_lua_src, &level_lua_src, &spell_lua_src, &game_lua_src](tz::lua::state& state)
		{
			//state.execute(R"(
			//)");
			state.execute(game_lua_src.c_str());
			state.execute(mod_lua_src.c_str());
			state.execute(level_lua_src.c_str());
			state.execute(spell_lua_src.c_str());
			game::audio_lua_initialise(state);
			game::messaging::scene_messaging_lua_initialise(state);
			game_system->scene2.get_renderer().lua_initialise(state);
			game::entity_lua_initialise(state); // rn.entity.*
			game::input::input_lua_initialise(state);

			// add require/dofile access to mods/
			state.execute("package.path = package.path .. \";./mods/?.lua\"");
			// iterate over /mods and require everything.
			state.execute(lua_require_cmd.c_str());
			state.execute("rn.load_mods()");
		});

		tz::lua::get_state().execute("rn.static_init()");
	}
}
