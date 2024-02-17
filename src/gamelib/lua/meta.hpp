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
		std::string description;
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

	struct spellinfo_t
	{
		std::string name;
		std::size_t mod_id;
		std::string description;
		std::string magic_type;
	};

	std::span<modinfo_t> get_mods();
	std::span<prefabinfo_t> get_prefabs();
	std::span<levelinfo_t> get_levels();
	std::span<spellinfo_t> get_spells();
	void lua_initialise(tz::lua::state& state);
	void reflect();
}

#endif // REDNIGHTMARE_GAMELIB_LUA_META_HPP