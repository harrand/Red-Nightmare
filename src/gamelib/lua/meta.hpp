#ifndef REDNIGHTMARE_GAMELIB_LUA_META_HPP
#define REDNIGHTMARE_GAMELIB_LUA_META_HPP
#include "tz/lua/state.hpp"

namespace game::meta
{
	struct modinfo_t
	{
		std::string name;
		std::string description;
	};

	struct prefabinfo_t
	{
		std::string name;
		std::string mod;
		bool has_static_init;
		bool has_pre_instantiate;
		bool has_instantiate;
		bool has_update;
		bool has_on_collision;
	};

	std::span<modinfo_t> get_mods();
	std::span<prefabinfo_t> get_prefabs();
	void lua_initialise(tz::lua::state& state);
	void reflect();
}

#endif // REDNIGHTMARE_GAMELIB_LUA_META_HPP