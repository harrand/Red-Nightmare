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
		std::size_t mod_id;
		bool has_static_init;
		bool has_pre_instantiate;
		bool has_instantiate;
		bool has_update;
		bool has_on_collision;
	};

	struct levelinfo_t
	{
		std::string name;
		std::size_t mod_id;
	};

	std::span<modinfo_t> get_mods();
	std::span<prefabinfo_t> get_prefabs();
	std::span<levelinfo_t> get_levels();
	void lua_initialise(tz::lua::state& state);
	void reflect();
}

#endif // REDNIGHTMARE_GAMELIB_LUA_META_HPP