#include "gamelib/rnlib.hpp"
#include "gamelib/logic/stats.hpp"
#include "gamelib/entity/scene.hpp"
#include "gamelib/renderer/texture_manager.hpp"
#include "tz/core/debug.hpp"
#include "tz/core/profile.hpp"
#include "tz/core/imported_text.hpp"
#include "tz/core/data/data_store.hpp"
#include "tz/lua/api.hpp"
#include <memory>

#include ImportedTextHeader(ability, lua)
#include ImportedTextHeader(combat, lua)
#include ImportedTextHeader(director, lua)
#include ImportedTextHeader(equipment, lua)
#include ImportedTextHeader(item, lua)
#include ImportedTextHeader(level, lua)

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
		tz::data_store data_store;
		dbgui_data_t dbgui;
	};

	std::unique_ptr<game_system_t> game_system = nullptr;

	void initialise()
	{
		TZ_PROFZONE("rnlib - initialise", 0xFF00AAFF);
		game_system = std::make_unique<game_system_t>();
		lua_initialise();

		tz::lua::get_state().execute(R"(
		rn.texture_manager():register_texture("blanchfield_cemetary.background", "./res/images/scenery/backgrounds/background_grassy.png")
		rn.texture_manager():register_texture("blanchfield_cemetary.background_normals", "./res/images/scenery/backgrounds/background_grassy_normals.png")
		rn.texture_manager():register_texture("blanchfield_cemetary.foreground", "./res/images/scenery/foregrounds/foreground_blanchfield_cemetary.png")
		rn.texture_manager():register_texture("blackrock_dungeon.background", "./res/images/scenery/backgrounds/background_blackrock.png")
		rn.texture_manager():register_texture("blackrock_dungeon.background_normals", "./res/images/scenery/backgrounds/background_blackrock_normals.png")

		--rn.load_level{name = "blackrock_dungeon"}
		rn.load_level{name = "blanchfield"}

		rn.scene():add(15)
		)");
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
		// SCENE UPDATE STARTS ASYNC WORK (ANIMATION ADVANCE)
		// im about to specify a begin and end comment label in this code.
		// for the duration of this region, this async work is still going on, as a result its evil to touch any of the stuff its working on.
		// within this region, it is unsafe to do the following:
		// - read/write animation renderer object data (specifically global transform)
		// - read/write scene *position* data (non-position stuff is okay, like buffs!)
		// BEGIN UNSAFE REGION

		// what else could go here...
		// extra actor logic? buffs etc are already here.
		// audio stuffs! (btw add audio at some point)
		// NETWORKING PACKET STUFF!?!? i need to go to bed.

		// additional note: in debug it would be a big perf gain to actually render the dbgui at this point aswell.
		// however, that is hard-coded by the engine to happen at the end of the frame, which we cant move around.

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
		// rendering uses the global transform object data. that's already been handled by now, and isnt touched during animation advance.
		// as a result it's safe to render now while the animation advance is going on. bussin
		tz::gl::get_device().render();
		// END UNSAFE REGION
		// note that lua update must wait till animation advance is done - it uses local transforms constantly, aswell as potentially adding/removing from the scene.
		{
			TZ_PROFZONE("rnlib - lua update", 0xFF00AAFF);
			tz::lua::get_state().assign_float("rn.delta_time", delta_seconds);
			game_system->scene.block();
			tz::lua::get_state().execute("if rn.update ~= nil then rn.update() end");
		}
	}

	void fixed_update(std::uint64_t delta_micros, std::uint64_t unprocessed)
	{
		float delta_seconds = delta_micros / 1000000.0f;
		tz::lua::get_state().assign_float("rn.fixed_delta_time", delta_seconds);
		tz::lua::get_state().execute("if rn.fixed_update ~= nil then rn.fixed_update() end");
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
		LUA_CLASS_PUSH(state, impl_rn_texture_manager, {.texmgr = &game_system->texmgr});
		return 1;
	LUA_END

	LUA_BEGIN(rn_impl_get_data_store)
		using namespace tz;
		LUA_CLASS_PUSH(state, tz_lua_data_store, {.ds = &game_system->data_store});
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
			state.execute("package.path = package.path .. \";\" .. rn.rootdir .. \"\\\\levels\\\\?.lua\"");

			game_system->scene.lua_initialise(state);
			game_system->texmgr.lua_initialise(state);
			game::logic::stats_static_initialise(state);
			state.assign_func("rn.scene", LUA_FN_NAME(rn_impl_get_scene));
			state.assign_func("rn.texture_manager", LUA_FN_NAME(rn_impl_get_texture_manager));
			state.assign_func("rn.data_store", LUA_FN_NAME(rn_impl_get_data_store));

			{
				std::string str{ImportedTextData(ability, lua)};
				state.execute(str.c_str());
			}
			{
				std::string str{ImportedTextData(combat, lua)};
				state.execute(str.c_str());
			}
			{
				std::string str{ImportedTextData(director, lua)};
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
			{
				std::string str{ImportedTextData(level, lua)};
				state.execute(str.c_str());
			}
		});

		TZ_PROFZONE("rnlib - lua entity static init", 0xFF00AAFF);
		tz::lua::get_state().execute("rn.entity_static_init()");
		tz::lua::get_state().execute("rn.item_static_init()");
	}
}