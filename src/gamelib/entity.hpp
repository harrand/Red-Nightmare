#ifndef REDNIGHTMARE_GAMELIB_ENTITY_HPP
#define REDNIGHTMARE_GAMELIB_ENTITY_HPP
#include "tz/lua/state.hpp"
#include "tz/core/algorithms/static.hpp"
#include <limits>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace game
{
	using entity_uuid = std::uint_fast64_t;
	constexpr entity_uuid null_uuid = std::numeric_limits<entity_uuid>::max();

	struct entity
	{
		entity_uuid uuid;
		std::string name = "";
		std::unordered_map<std::string, tz::lua::lua_generic> internal_variables = {};
		std::vector<std::string> active_buffs = {};

		static entity null();
		bool is_null() const;

		// set an internal variable. if the variable doesnt exist, also returns nil.
		tz::lua::lua_generic get_internal(const std::string& varname) const
		{
			auto iter = this->internal_variables.find(varname);
			if(iter == this->internal_variables.end())
			{
				return tz::lua::nil{};
			}
			return iter->second;
		}

		template<typename T>
		void set_internal(const std::string& varname, T&& val) requires(tz::static_find<std::decay_t<T>, bool, double, std::int64_t, std::string>() || std::is_integral_v<T> || std::is_same_v<T, tz::lua::nil>)
		{
			this->internal_variables[varname] = val; 
		}
	};

	void entity_lua_initialise(tz::lua::state& state);
}

#endif // REDNIGHTMARE_GAMELIB_ENTITY_HPP
