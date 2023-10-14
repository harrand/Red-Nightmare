#ifndef RN_GAMELIB_LOGIC_STATS_HPP
#define RN_GAMELIB_LOGIC_STATS_HPP
#include "tz/lua/api.hpp"
#include <string>
#include <cstdint>

namespace game::logic
{
	struct buff
	{
		std::string name = "???BUFF???";

		// increased absolute health. e.g + 50 health.
		std::uint64_t increased_health = 0u;
		// increased health (as a percentage). e.g + 10% health.
		float amplified_health = 1.0f;
		// increased movement speed (as a percentage). e.g + 15% increased movement speed.
		unsigned int increased_movement_speed = 0u;

		std::optional<float> time_remaining_seconds = std::nullopt;

		buff operator*(float amplification) const;
	};

	struct rn_impl_buff
	{
		buff b;
		int get_name(tz::lua::state& state);
		int set_name(tz::lua::state& state);
		int set_increased_health(tz::lua::state& state);
		int set_amplified_health(tz::lua::state& state);
		int set_increased_movement_speed(tz::lua::state& state);
		int get_time_remaining(tz::lua::state& state);
		int set_time_remaining(tz::lua::state& state);
	};

	LUA_CLASS_BEGIN(rn_impl_buff)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(rn_impl_buff, get_name)
			LUA_METHOD(rn_impl_buff, set_name)
			LUA_METHOD(rn_impl_buff, set_increased_health)
			LUA_METHOD(rn_impl_buff, set_amplified_health)
			LUA_METHOD(rn_impl_buff, set_increased_movement_speed)
			LUA_METHOD(rn_impl_buff, get_time_remaining)
			LUA_METHOD(rn_impl_buff, set_time_remaining)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END

	struct stats
	{
		std::uint64_t health = 0;
		float movement_speed = 3.0f;

		stats operator+(const buff& rhs) const;
	};

	struct rn_impl_stats
	{
		stats s;
		int get_health(tz::lua::state& state);
		int set_health(tz::lua::state& state);
		int get_movement_speed(tz::lua::state& state);
		int set_movement_speed(tz::lua::state& state);
	};

	LUA_CLASS_BEGIN(rn_impl_stats)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(rn_impl_stats, get_health)
			LUA_METHOD(rn_impl_stats, set_health)
			LUA_METHOD(rn_impl_stats, get_movement_speed)
			LUA_METHOD(rn_impl_stats, set_movement_speed)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END
	
	void stats_static_initialise(tz::lua::state& state);
}

#endif // RN_GAMELIB_LOGIC_STATS_HPP