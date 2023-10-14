#include "gamelib/logic/stats.hpp"
#include "tz/core/debug.hpp"

namespace game::logic
{
	buff buff::operator*(float amplification) const
	{
		tz::assert(amplification > 0.0f, "Negative amplification on buff is not allowed.");
		buff cpy = *this;
		cpy.increased_health *= amplification;
		cpy.amplified_health *= amplification;
		return cpy;
	}

	stats stats::operator+(const buff& rhs) const
	{
		stats cpy = *this;
		cpy.health *= rhs.amplified_health;
		cpy.health += rhs.increased_health;
		cpy.movement_speed *= (100.0f + rhs.increased_movement_speed) / 100.0f;
		return cpy;
	}

	// lua API

	int rn_impl_buff::get_name(tz::lua::state& state)
	{
		state.stack_push_string(this->b.name);
		return 1;
	}

	int rn_impl_buff::set_name(tz::lua::state& state)
	{
		auto [_, name] = tz::lua::parse_args<tz::lua::nil, std::string>(state);
		this->b.name = name;
		return 0;
	}

	int rn_impl_buff::set_increased_health(tz::lua::state& state)
	{
		auto [_, hp] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
		this->b.increased_health = hp;
		return 0;
	}

	int rn_impl_buff::set_amplified_health(tz::lua::state& state)
	{
		auto [_, hpmul] = tz::lua::parse_args<tz::lua::nil, float>(state);
		this->b.amplified_health = hpmul;
		return 0;
	}

	int rn_impl_buff::set_increased_movement_speed(tz::lua::state& state)
	{
		auto [_, speed] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
		this->b.increased_movement_speed = speed;
		return 0;
	}

	int rn_impl_buff::get_time_remaining(tz::lua::state& state)
	{
		if(this->b.time_remaining_seconds.has_value())
		{
			state.stack_push_float(this->b.time_remaining_seconds.value());
		}
		else
		{
			state.stack_push_nil();
		}
		return 1;
	}

	int rn_impl_buff::set_time_remaining(tz::lua::state& state)
	{
		auto [_, duration] = tz::lua::parse_args<tz::lua::nil, float>(state);
		this->b.time_remaining_seconds = duration;
		return 0;
	}
	
	/////////////////

	int rn_impl_stats::get_health(tz::lua::state& state)
	{
		state.stack_push_uint(this->s.health);
		return 1;
	}

	int rn_impl_stats::set_health(tz::lua::state& state)
	{
		auto [_, hp] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
		this->s.health = hp;
		return 0;
	}

	int rn_impl_stats::get_movement_speed(tz::lua::state& state)
	{
		state.stack_push_float(this->s.movement_speed);
		return 1;
	}

	int rn_impl_stats::set_movement_speed(tz::lua::state& state)
	{
		auto [_, speed] = tz::lua::parse_args<tz::lua::nil, float>(state);
		this->s.movement_speed = speed;
		return 0;
	}

	LUA_BEGIN(rn_impl_buff_new)
		LUA_CLASS_PUSH(state, rn_impl_buff, {.b = {}});
		return 1;
	LUA_END

	void stats_static_initialise(tz::lua::state& state)
	{
		state.new_type("rn_impl_stats", LUA_CLASS_NAME(rn_impl_stats)::registers);
		state.new_type("rn_impl_buff", LUA_CLASS_NAME(rn_impl_buff)::registers);
		state.assign_func("rn.new_buff", LUA_FN_NAME(rn_impl_buff_new));
	}
}