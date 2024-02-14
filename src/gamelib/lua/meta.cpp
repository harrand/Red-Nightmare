#include "gamelib/lua/meta.hpp"
#include "tz/lua/api.hpp"

namespace game::meta
{
	std::vector<modinfo_t> internal_mod_list = {};
	std::vector<prefabinfo_t> internal_prefab_list = {};

	LUA_BEGIN(rn_inform_mod)
		auto [name, description] = tz::lua::parse_args<std::string, std::string>(state);
		internal_mod_list.push_back({.name = name, .description = description});
		return 0;
	LUA_END

	LUA_BEGIN(rn_inform_prefab)
		auto [name, mod, has_static_init, has_pre_instantiate, has_instantiate, has_update, has_on_collision] = tz::lua::parse_args<std::string, std::string, bool, bool, bool, bool, bool>(state);
		internal_prefab_list.push_back({.name = name, .mod = mod, .has_static_init = has_static_init, .has_pre_instantiate = has_pre_instantiate, .has_instantiate = has_instantiate, .has_update = has_update, .has_on_collision = has_on_collision});
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

	void lua_initialise(tz::lua::state& state)
	{
		state.assign_func("rn.inform_mod", LUA_FN_NAME(rn_inform_mod));
		state.assign_func("rn.inform_prefab", LUA_FN_NAME(rn_inform_prefab));
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

			for prefabname, prefabdata in pairs(rn.entity.prefabs) do
				rn.inform_prefab(prefabname, prefabdata.mod, prefabdata.static_init ~= nil, prefabdata.pre_instantiate ~= nil, prefabdata.instantiate ~= nil, prefabdata.update ~= nil, prefabdata.on_collision ~= nil)
			end
		)");
	}
}