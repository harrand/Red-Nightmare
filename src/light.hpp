#ifndef REDNIGHTMARE_SRC_LIGHT_HPP
#define REDNIGHTMARE_SRC_LIGHT_HPP
#include "tz/core/data/vector.hpp"

namespace game
{
	struct PointLight
	{
		tz::vec2 position{0.0f, 0.0f};
		float pad0[2];
		tz::vec3 colour{1.0f, 1.0f, 1.0f};
		float power = 0.0f;
	};
}

#endif // REDNIGHTMARE_SRC_LIGHT_HPP
