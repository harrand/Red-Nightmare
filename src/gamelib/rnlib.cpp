#include "gamelib/rnlib.hpp"
#include "gamelib/logic/stats.hpp"
#include "gamelib/entity/scene.hpp"
#include "gamelib/renderer/texture_manager.hpp"
#include "tz/core/debug.hpp"
#include "tz/core/profile.hpp"
#include "tz/core/imported_text.hpp"
#include "tz/lua/api.hpp"
#include <memory>

#include ImportedTextHeader(ability, lua)
#include ImportedTextHeader(combat, lua)
#include ImportedTextHeader(equipment, lua)
#include ImportedTextHeader(item, lua)

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
		game::entity::scene scene;
		game::render::texture_manager texmgr{scene.get_renderer().get_renderer()};
		dbgui_data_t dbgui;
	};

	std::unique_ptr<game_system_t> game_system = nullptr;

	void initialise()
	{
		TZ_PROFZONE("rnlib - initialise", 0xFF00AAFF);
		game_system = std::make_unique<game_system_t>();
		lua_initialise();

		game_system->scene.add(0);
		// lua equivalent: rn.scene():add(0)
	}

	void terminate()
	{
		game_system = nullptr;
	}

	void update(std::uint64_t delta_micros)
	{
		TZ_PROFZONE("rnlib - update", 0xFF00AAFF);
		float delta_seconds = delta_micros / 1000000.0f;
		game_system->scene.update(delta_seconds);

		if(game_system->dbgui.display_scene)
		{
			if(ImGui::Begin("Scene", &game_system->dbgui.display_scene))
			{
				game_system->scene.dbgui();
				ImGui::End();
			}
		}

		if(game_system->dbgui.display_animation_renderer)
		{
			if(ImGui::Begin("Renderer", &game_system->dbgui.display_animation_renderer))
			{
				game_system->scene.get_renderer().dbgui();
				ImGui::End();
			}
		}
		{
			TZ_PROFZONE("rnlib - lua update", 0xFF00AAFF);
			tz::lua::get_state().assign_float("rn.delta_time", delta_seconds);
			tz::lua::get_state().execute("if rn.update ~= nil then rn.update() end");
		}
		tz::gl::get_device().render();
	}

	void fixed_update(std::uint64_t delta_micros, std::uint64_t unprocessed)
	{
	}

	void dbgui()
	{
		ImGui::MenuItem("Scene", nullptr, &game_system->dbgui.display_scene);
		ImGui::MenuItem("Animation Renderer", nullptr, &game_system->dbgui.display_animation_renderer);
	}

	void dbgui_game_bar()
	{
		game_system->scene.dbgui_game_bar();
	}

	LUA_BEGIN(rn_impl_get_scene)
		using namespace game::entity;
		LUA_CLASS_PUSH(state, rn_impl_scene, {.sc = &game_system->scene});
		return 1;
	LUA_END

	LUA_BEGIN(rn_impl_get_texture_manager)
		using namespace game::render;
		LUA_CLASS_PUSH(state, impl_rn_texture_manager, {.texmgr = &game_system->texmgr})
		return 1;
	LUA_END

	// called directly from initialise.
	void lua_initialise()
	{
		TZ_PROFZONE("rnlib - lua initialise", 0xFF00AAFF);
		tz::lua::for_all_states([](tz::lua::state& state)
		{
			TZ_PROFZONE("rnlib - lua single state init", 0xFF00AAFF);
			state.execute("math.randomseed(os.time())");
			state.assign_emptytable("rn");
			std::string cwd = std::filesystem::current_path().string();
			state.stack_push_string(cwd);
			state.assign_stack("rn_impl_cwd");
			state.execute("rn.cwd = rn_impl_cwd");
			state.execute("rn.rootdir = rn.cwd .. \"\\\\src\\\\game\\\\lua\"");
			// debug builds: use root_dir/lua
			state.execute("package.path = rn.rootdir .. \"\\\\?.lua\"");
			state.execute("package.path = package.path .. \";\" .. rn.rootdir .. \"\\\\entity\\\\?.lua\"");
			state.execute("package.path = package.path .. \";\" .. rn.rootdir .. \"\\\\item\\\\?.lua\"");
			state.execute("package.path = package.path .. \";\" .. rn.rootdir .. \"\\\\ability\\\\?.lua\"");

			game_system->scene.lua_initialise(state);
			game_system->texmgr.lua_initialise(state);
			game::logic::stats_static_initialise(state);
			state.assign_func("rn.scene", LUA_FN_NAME(rn_impl_get_scene));
			state.assign_func("rn.texture_manager", LUA_FN_NAME(rn_impl_get_texture_manager));

			{
				std::string str{ImportedTextData(ability, lua)};
				state.execute(str.c_str());
			}
			{
				std::string str{ImportedTextData(combat, lua)};
				state.execute(str.c_str());
			}
			{
				std::string str{ImportedTextData(equipment, lua)};
				state.execute(str.c_str());
			}
			{
				std::string str{ImportedTextData(item, lua)};
				state.execute(str.c_str());
			}
		});

		TZ_PROFZONE("rnlib - lua entity static init", 0xFF00AAFF);
		tz::lua::get_state().execute("rn.entity_static_init()");
		tz::lua::get_state().execute("rn.item_static_init()");
	}
}