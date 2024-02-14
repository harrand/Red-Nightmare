#include "gamelib/lua/meta.hpp"
#include "tz/lua/api.hpp"

namespace game::meta
{
	std::vector<modinfo_t> internal_mod_list = {};
	std::vector<prefabinfo_t> internal_prefab_list = {};
	std::vector<levelinfo_t> internal_level_list = {};

	LUA_BEGIN(rn_inform_mod)
		auto [name, description] = tz::lua::parse_args<std::string, std::string>(state);
		internal_mod_list.push_back({.name = name, .description = description});
		return 0;
	LUA_END

	LUA_BEGIN(rn_inform_prefab)
		auto [name, mod, has_static_init, has_pre_instantiate, has_instantiate, has_update, has_on_collision] = tz::lua::parse_args<std::string, std::string, bool, bool, bool, bool, bool>(state);
		auto iter = std::find_if(internal_mod_list.begin(), internal_mod_list.end(),
		[&mod](const auto& cur_mod)
		{
			return cur_mod.name == mod;
		});
		tz::assert(iter != internal_mod_list.end());
		std::size_t mod_id = std::distance(internal_mod_list.begin(), iter);
		internal_prefab_list.push_back({.name = name, .mod_id = mod_id, .has_static_init = has_static_init, .has_pre_instantiate = has_pre_instantiate, .has_instantiate = has_instantiate, .has_update = has_update, .has_on_collision = has_on_collision});
		return 0;
	LUA_END

	LUA_BEGIN(rn_inform_level)
		auto [name, mod] = tz::lua::parse_args<std::string, std::string>(state);
		auto iter = std::find_if(internal_mod_list.begin(), internal_mod_list.end(),
		[&mod](const auto& cur_mod)
		{
			return cur_mod.name == mod;
		});
		tz::assert(iter != internal_mod_list.end());
		std::size_t mod_id = std::distance(internal_mod_list.begin(), iter);
		internal_level_list.push_back({.name = name, .mod_id = mod_id});
		return 0;
	LUA_END

	std::span<modinfo_t> get_mods()
	{
		return internal_mod_list;
	}

	std::span<prefabinfo_t> get_prefabs()
	{
		return internal_prefab_list;
	}

	std::span<levelinfo_t> get_levels()
	{
		return internal_level_list;
	}

	void lua_initialise(tz::lua::state& state)
	{
		state.assign_func("rn.inform_mod", LUA_FN_NAME(rn_inform_mod));
		state.assign_func("rn.inform_prefab", LUA_FN_NAME(rn_inform_prefab));
		state.assign_func("rn.inform_level", LUA_FN_NAME(rn_inform_level));
	}

	void reflect()
	{
		internal_mod_list.clear();
		internal_prefab_list.clear();
		tz::lua::get_state().execute(R"(
			-- reflect mods
			for modname, moddata in pairs(rn.mods) do
				rn.inform_mod(modname, moddata.description)
			end

			-- reflect prefabs
			for prefabname, prefabdata in pairs(rn.entity.prefabs) do
				rn.inform_prefab(prefabname, prefabdata.mod, prefabdata.static_init ~= nil, prefabdata.pre_instantiate ~= nil, prefabdata.instantiate ~= nil, prefabdata.update ~= nil, prefabdata.on_collision ~= nil)
			end

			-- reflect levels
			for levelname, leveldata in pairs(rn.level.levels) do
				rn.inform_level(levelname, leveldata.mod)
			end
		)");
	}
}