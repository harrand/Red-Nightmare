#ifndef REDNIGHTMARE_UTIL_HPP
#define REDNIGHTMARE_UTIL_HPP
#include "hdk/data/vector.hpp"

namespace game::util
{
	constexpr float stat_global_multiplier = 60.0f;
	hdk::vec2 get_mouse_world_location();
}

#endif // REDNIGHTMARE_UTIL_HPP
